/*=================================================
* FileName: BuoyantMeshComponent.cpp
*
* Created by: quantumv
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/09/21
*
* Last Edited on: 2015/09/21
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

typedef UBuoyantMeshComponent::FForce FForce;

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

	if (!OceanManager)
	{
		for (auto Actor : TActorRange<AOceanManager>(GetWorld()))
		{
			OceanManager = Cast<AOceanManager>(Actor);
			break;
		}
	}
}

// Called when the game starts
void UBuoyantMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UBuoyantMeshComponent::GetHeightAboveWater(const FVector& Position) const
{
	SCOPE_CYCLE_COUNTER(STAT_GetHeightAboveWater);
	float WaterHeight = 0.f;
	if (IsValid(UpdatedPrimitive) && IsValid(OceanManager))
	{
		WaterHeight = OceanManager->GetWaveHeight(Position);
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
}

void UBuoyantMeshComponent::GetTriangleVertexIndices(const TArray<FVector>& WorldVertexPositions,
													 const void* const VertexIndices,
													 const PxU32 TriangleIndex,
													 const bool b16BitIndices, int32* OutIndex1,
													 int32* OutIndex2, int32* OutIndex3)
{
	// TODO: Make static
	if (b16BitIndices)
	{
		const auto Indices = static_cast<const PxU16*>(VertexIndices);
		*OutIndex1 = Indices[TriangleIndex * 3];
		*OutIndex2 = Indices[TriangleIndex * 3 + 1];
		*OutIndex3 = Indices[TriangleIndex * 3 + 2];
		return;
	}
	else
	{
		const auto Indices = static_cast<const PxU32*>(VertexIndices);
		*OutIndex1 = Indices[TriangleIndex * 3];
		*OutIndex2 = Indices[TriangleIndex * 3 + 1];
		*OutIndex3 = Indices[TriangleIndex * 3 + 2];
		return;
	}
}

void UBuoyantMeshComponent::GetSubtriangleForces(TArray<FForce>& InOutForces,
												 const float GravityMagnitude,
												 const FVector& TriangleNormal,
												 const FBuoyantMeshSubtriangle& Subtriangle) const
{
	const auto CenterPosition = Subtriangle.GetCenter();
	const FBuoyantMeshVertex TriangleCenter{CenterPosition, GetHeightAboveWater(CenterPosition)};
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
		VertexHeights.Emplace(GetHeightAboveWater(Vertex));
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

		const auto Triangle = FBuoyantMeshTriangle::FromClockwiseVertices(A, B, C);

		const auto SubTriangles = Triangle.GetSubmergedPortion(&InWorld, bDrawDebugWaterline);

		for (const auto& SubTriangle : SubTriangles)
		{
			GetSubtriangleForces(InOutForces, GravityMagnitude, Triangle.Normal, SubTriangle);
		}
	}
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
			DrawDebugDirectionalArrow(
				&World, Force.Point - (ForceVector * ForceArrowSize * 0.00001f), Force.Point,
				ForceVector.Size() * ForceArrowSize * 0.2f, FColor(255, 255, 255));
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