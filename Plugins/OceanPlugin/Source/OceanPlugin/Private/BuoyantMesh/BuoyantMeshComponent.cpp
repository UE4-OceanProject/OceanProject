/*=================================================
* FileName: BuoyantMeshComponent.cpp
*
* Created by: quantumv
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/09/21
*
* Last Edited on: 2015/09/23
* Last Edited by: quantumv
*
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#include "OceanPluginPrivatePCH.h"
#include "BuoyantMesh/BuoyantMeshComponent.h"

#include "PhysicsEngine/BodySetup.h"
#include "PhysXIncludes.h"
#include "PhysXPublic.h"

#include "OceanManager.h"
#include "BuoyantMesh/BuoyantMeshTriangle.h"
#include "BuoyantMesh/BuoyantMeshSubtriangle.h"

using FForce = UBuoyantMeshComponent::FForce;

// Sets default values for this component's properties
UBuoyantMeshComponent::UBuoyantMeshComponent()
{
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	bVerticalForcesOnly = false;
	UActorComponent::SetComponentTickEnabled(true);

	WaterDensity = 0.001027f;
}

void UBuoyantMeshComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called when the game starts
void UBuoyantMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UBuoyantMeshComponent::GetHeightAboveWater(const UWorld& World, const FVector& Position) const
{
	SCOPE_CYCLE_COUNTER(STAT_GetHeightAboveWater);
	float WaterHeight = 0.f;
	if (IsValid(UpdatedPrimitive) && IsValid(OceanManager))
	{
		WaterHeight = OceanManager->GetWaveHeight(Position, &World);
	}
	return Position.Z - WaterHeight;
}

void UBuoyantMeshComponent::Initialize()
{
	AActor* OwnerActor = GetOwner();
	if (IsValid(OwnerActor))
	{
		UpdatedComponent = OwnerActor->GetRootComponent();
		if (IsValid(UpdatedComponent))
		{
			UpdatedPrimitive = Cast<UPrimitiveComponent>(UpdatedComponent);
			UpdatedComponent->PrimaryComponentTick.AddPrerequisite(this, PrimaryComponentTick);
		}
	}
	if (!OceanManager)
	{
		for (auto Actor : TActorRange<AOceanManager>(GetWorld()))
		{
			OceanManager = Cast<AOceanManager>(Actor);
			break;
		}
	}
}

template <class T>
void UBuoyantMeshComponent::GetTriangleVertexIndices(const TArray<FVector>& WorldVertexPositions,
													 const T* const VertexIndices,
													 const PxU32 TriangleIndex, int32* OutIndex1,
													 int32* OutIndex2, int32* OutIndex3)
{
	*OutIndex1 = VertexIndices[TriangleIndex * 3];
	*OutIndex2 = VertexIndices[TriangleIndex * 3 + 1];
	*OutIndex3 = VertexIndices[TriangleIndex * 3 + 2];
	return;
}


void UBuoyantMeshComponent::GetTriangleVertexIndices(const TArray<FVector>& WorldVertexPositions,
													 const void* const VertexIndices,
													 const PxU32 TriangleIndex,
													 const bool b16BitIndices, int32* OutIndex1,
													 int32* OutIndex2, int32* OutIndex3)
{
	if (b16BitIndices)
	{
		const auto Indices = static_cast<const PxU16*>(VertexIndices);
		GetTriangleVertexIndices(WorldVertexPositions, Indices, TriangleIndex, OutIndex1, OutIndex2,
								 OutIndex3);
		return;
	}
	else
	{
		const auto Indices = static_cast<const PxU32*>(VertexIndices);
		GetTriangleVertexIndices(WorldVertexPositions, Indices, TriangleIndex, OutIndex1, OutIndex2,
								 OutIndex3);
		return;
	}
}

void UBuoyantMeshComponent::GetSubtriangleForces(const UWorld& World, TArray<FForce>& InOutForces,
												 const float GravityMagnitude,
												 const FVector& TriangleNormal,
												 const FBuoyantMeshSubtriangle& Subtriangle) const
{
	const auto CenterPosition = Subtriangle.GetCenter();
	const FBuoyantMeshVertex TriangleCenter{CenterPosition,
											GetHeightAboveWater(World, CenterPosition)};
	const auto ForceVector = Subtriangle.GetHydrostaticForce(WaterDensity, GravityMagnitude,
															 TriangleCenter, TriangleNormal);

	const FForce Force = {ForceVector, CenterPosition};
	InOutForces.Emplace(Force);
}

void UBuoyantMeshComponent::GetTriangleMeshForces(TArray<FForce>& InOutForces, UWorld& InWorld,
												  const PxTriangleMesh& TriangleMesh) const
{
	// Get Vertices
	const PxVec3* const Vertices = TriangleMesh.getVertices();
	const PxU32 VertexCount = TriangleMesh.getNbVertices();

	const auto LocalToWorld = GetComponentTransform();

	TArray<FVector> WorldVertexPositions;
	TArray<float> VertexHeights;
	for (PxU32 i = 0; i < VertexCount; ++i)
	{
		const auto Vertex = LocalToWorld.TransformPosition(P2UVector(Vertices[i]));
		WorldVertexPositions.Emplace(Vertex);
		VertexHeights.Emplace(GetHeightAboveWater(InWorld, Vertex));
		if (bDrawVertices)
		{
			DrawDebugPoint(&InWorld, Vertex, 2.f, FColor::White);
		}
	}

	// Get Triangles
	const PxU32 TriangleCount = TriangleMesh.getNbTriangles();
	const void* const VertexIndices = TriangleMesh.getTriangles();
	const auto b16BitIndices =
		TriangleMesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;

	const float GravityMagnitude = FMath::Abs(InWorld.GetGravityZ());

	for (PxU32 TriangleIndex = 0; TriangleIndex < TriangleCount; ++TriangleIndex)
	{
		// A, B and C are the vertices of the triangle in clockwise order.
		// The order determines the normal.
		int32 AIndex;
		int32 BIndex;
		int32 CIndex;

		GetTriangleVertexIndices(WorldVertexPositions, VertexIndices, TriangleIndex, b16BitIndices,
								 &AIndex, &BIndex, &CIndex);

		const FBuoyantMeshVertex A{WorldVertexPositions[AIndex], VertexHeights[AIndex]};
		const FBuoyantMeshVertex B{WorldVertexPositions[BIndex], VertexHeights[BIndex]};
		const FBuoyantMeshVertex C{WorldVertexPositions[CIndex], VertexHeights[CIndex]};

		if (bDrawTriangles)
		{
			DrawDebugTriangle(InWorld, A.Position, B.Position, C.Position, FColor::White, 4.f);
		}

		const auto Triangle = FBuoyantMeshTriangle::FromClockwiseVertices(A, B, C);

		const auto SubTriangles = Triangle.GetSubmergedPortion(&InWorld, bDrawWaterline);

		for (const auto& SubTriangle : SubTriangles)
		{
			if (bDrawSubtriangles)
			{
				DrawDebugTriangle(InWorld, SubTriangle.A, SubTriangle.B, SubTriangle.C,
								  FColor::Yellow, 6.f);
			}
			GetSubtriangleForces(InWorld, InOutForces, GravityMagnitude, Triangle.Normal,
								 SubTriangle);
		}
	}
}

void UBuoyantMeshComponent::DrawDebugTriangle(const UWorld& World, const FVector& A,
											  const FVector& B, const FVector& C,
											  const FColor& Color, const float Thickness)
{
	DrawDebugLine(&World, A, B, Color, false, -1.f, 0, Thickness);
	DrawDebugLine(&World, B, C, Color, false, -1.f, 0, Thickness);
	DrawDebugLine(&World, C, A, Color, false, -1.f, 0, Thickness);
}

void UBuoyantMeshComponent::GetStaticMeshForces(TArray<FForce>& InOutForces, UWorld& InWorld,
												const UBodySetup& BodySetup) const
{
	for (const auto TriangleMesh : BodySetup.TriMeshes)
	{
		if (TriangleMesh != nullptr)
		{
			GetTriangleMeshForces(InOutForces, InWorld, *TriangleMesh);
		}
	}
}

void UBuoyantMeshComponent::ApplyHydrostaticForce(UWorld& World, const FForce& Force)
{
	const auto ForceVector = bVerticalForcesOnly ? FVector{0.f, 0.f, Force.Vector.Z} : Force.Vector;

	if (!ForceVector.IsNearlyZero() && isfinite(ForceVector.X) && isfinite(ForceVector.Y) &&
		isfinite(ForceVector.Z))
	{
		UpdatedPrimitive->AddForceAtLocation(ForceVector, Force.Point);
		if (bDrawForceArrows)
		{
			DrawDebugLine(&World, Force.Point - (ForceVector * ForceArrowSize * 0.0001f),
						  Force.Point, FColor::Blue);
		}
	}
}

// Called every frame
void UBuoyantMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType,
										  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bNeedsInitialization)
	{
		bNeedsInitialization = false;
		Initialize();
	}

	const auto World = GetWorld();
	if (!IsValid(World)) return;

	const auto LocalToWorld = GetComponentTransform();

	const auto BodySetup = GetBodySetup();
	if (IsValid(BodySetup))
	{
		TArray<FForce> Forces;
		GetStaticMeshForces(Forces, *World, *BodySetup);

		for (const auto& Force : Forces)
		{
			ApplyHydrostaticForce(*World, Force);
		}
	}
}