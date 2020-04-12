/*=================================================
* FileName: BuoyantMeshComponent.h
*
* Created by: quantumv
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/09/21
*
* Last Edited on: 2018/03/15
* Last Edited by: Felipe "Zoc" Silveira
*
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#include "BuoyantMesh/BuoyantMeshComponent.h"
#include "OceanShaderPlugin/Private/OceanShaderManager.h"
#include "PhysicsEngine/BodySetup.h"
#include "BuoyantMesh/BuoyantMeshTriangle.h"
#include "BuoyantMesh/BuoyantMeshSubtriangle.h"
#include "BuoyantMesh/WaterHeightmapComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "PhysXPublic.h"


using FForce = UBuoyantMeshComponent::FForce;

// Sets default values for this component's properties
UBuoyantMeshComponent::UBuoyantMeshComponent()
{
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	UActorComponent::SetComponentTickEnabled(true);
}

float UBuoyantMeshComponent::GetHeightAboveWater(const FVector& Position) const
{
	float WaterHeight = 0.f;
	if (IsValid(OceanManager))
	{
		if (bUseWaterPatch && IsValid(WaterHeightmap))
		{
			WaterHeight = WaterHeightmap->GetHeightAtPosition(Position);
		}
		else
		{
			WaterHeight = OceanManager->GetWaveHeight(Position, World);
		}
	}
	return Position.Z - WaterHeight;
}

UPrimitiveComponent* UBuoyantMeshComponent::GetParentPrimitive() const
{
	if (IsValid(GetAttachParent()))
	{
		const auto PrimitiveComponent = Cast<UPrimitiveComponent>(GetAttachParent());
		if (PrimitiveComponent)
		{
			return PrimitiveComponent;
		}
	}
	return nullptr;
}

AOceanShaderManager* UBuoyantMeshComponent::FindOceanManager() const
{
	for (auto Actor : TActorRange<AOceanShaderManager>(GetWorld()))
	{
		return Actor;
	}
	return nullptr;
}

UWaterHeightmapComponent* UBuoyantMeshComponent::FindWaterHeightmap() const
{
	TInlineComponentArray<UWaterHeightmapComponent*> WaterHeightmaps;
	const auto Owner = GetOwner();
	check(Owner);
	Owner->GetComponents(WaterHeightmaps);
	if (WaterHeightmaps.Num() > 0)
	{
		return WaterHeightmaps[0];
	}
	else
	{
		return nullptr;
	}
}

void UBuoyantMeshComponent::SetupTickOrder()
{
	// This component needs to tick before the updated component.
	if (UpdatedComponent != this)
	{
		UpdatedComponent->PrimaryComponentTick.AddPrerequisite(this, PrimaryComponentTick);
	}

	// The water heightmap needs to tick before this component.
	if (WaterHeightmap)
	{
		PrimaryComponentTick.AddPrerequisite(WaterHeightmap, WaterHeightmap->PrimaryComponentTick);
	}
}

void UBuoyantMeshComponent::Initialize()
{
	if (UpdatedComponent == nullptr)
	{
		const auto ParentPrimitive = GetParentPrimitive();
		UpdatedComponent = ParentPrimitive ? ParentPrimitive : this;
	}

	if (!OceanManager)
	{
		OceanManager = FindOceanManager();
	}

	WaterHeightmap = FindWaterHeightmap();

	SetupTickOrder();

	TriangleMeshes = TMeshUtilities::GetTriangleMeshes(this);

	World = GetWorld();
	GravityMagnitude = FMath::Abs(World->GetGravityZ());

	SetMassProperties();
}


void UBuoyantMeshComponent::SetMassProperties()
{
	if (UpdatedComponent)
	{
		if (bOverrideMeshDensity)
		{
			const auto MeshVolume = TMathUtilities::MeshVolume(this);
			const auto ComputedMass = MeshDensity * MeshVolume;
			UpdatedComponent->SetMassOverrideInKg(NAME_None, ComputedMass);
		}

		if (bOverrideMass)
		{
			UpdatedComponent->SetMassOverrideInKg(NAME_None, Mass);
		}
	}
}

void UBuoyantMeshComponent::DrawDebugTriangle(
    UWorld* World, const FVector& A, const FVector& B, const FVector& C, const FColor& Color, const float Thickness)
{
	DrawDebugLine(World, A, B, Color, false, -1.f, 0, Thickness);
	DrawDebugLine(World, B, C, Color, false, -1.f, 0, Thickness);
	DrawDebugLine(World, C, A, Color, false, -1.f, 0, Thickness);
}

// Called every frame
void UBuoyantMeshComponent::TickComponent(float DeltaTime,
                                          ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bHasInitialized)
	{
		Initialize();
		bHasInitialized = true;

		if (!IsValid(UpdatedComponent) || !UpdatedComponent->IsSimulatingPhysics())
		{
			UE_LOG(LogTemp,
			       Error,
			       TEXT("BuoyantMeshComponent has no updated component set up. Use a ")
			           TEXT("parent component with \"Simulate Physics\" turned on."));
			return;
		}
	}

	ApplyMeshForces();
}


void UBuoyantMeshComponent::ApplyMeshForces()
{
	auto debugWorld = GetWorld();

	const auto LocalToWorld = GetComponentTransform();

	for (const auto TriangleMesh : TriangleMeshes)
	{
		TArray<FBuoyantMeshVertex> BuoyantMeshVertices{};
		for (const auto Vertex : TriangleMesh.Vertices)
		{
			const auto WorldVertex = LocalToWorld.TransformPosition(Vertex);
			BuoyantMeshVertices.Emplace(WorldVertex, GetHeightAboveWater(WorldVertex));
		}

		const auto TriangleCount = TriangleMesh.TriangleVertexIndices.Num() / 3;
		for (int32 i = 0; i < TriangleCount; ++i)
		{
			const auto A = BuoyantMeshVertices[TriangleMesh.TriangleVertexIndices[i * 3 + 0]];
			const auto B = BuoyantMeshVertices[TriangleMesh.TriangleVertexIndices[i * 3 + 1]];
			const auto C = BuoyantMeshVertices[TriangleMesh.TriangleVertexIndices[i * 3 + 2]];

			if (bDrawTriangles)
			{
				DrawDebugTriangle(debugWorld, A.Position, B.Position, C.Position, FColor::White, 4.f);
			}

			const auto Triangle = FBuoyantMeshTriangle::FromClockwiseVertices(A, B, C);

			const auto SubTriangles = Triangle.GetSubmergedPortion(debugWorld, bDrawWaterline);

			for (const auto& SubTriangle : SubTriangles)
			{
				if (bDrawSubtriangles)
				{
					DrawDebugTriangle(debugWorld, SubTriangle.A, SubTriangle.B, SubTriangle.C, FColor::Yellow, 6.f);
				}

				const auto SubtriangleForce = GetSubmergedTriangleForce(SubTriangle, Triangle.Normal);
				ApplyMeshForce(SubtriangleForce);
			}
		}
	}
}

void UBuoyantMeshComponent::ApplyMeshForce(const FForce& Force)
{
	const auto ForceVector = bVerticalForcesOnly ? FVector{0.f, 0.f, Force.Vector.Z} : Force.Vector;
	const auto bIsValidForce = !ForceVector.IsNearlyZero() && !ForceVector.ContainsNaN();
	if (!bIsValidForce) return;

	UpdatedComponent->AddForceAtLocation(ForceVector, Force.Point);
	if (bDrawForceArrows)
	{
		DrawDebugLine(World, Force.Point - (ForceVector * ForceArrowSize * 0.0001f), Force.Point, FColor::Blue);
	}
}

FForce UBuoyantMeshComponent::GetSubmergedTriangleForce(const FBuoyantMeshSubtriangle& Subtriangle,
                                                        const FVector& TriangleNormal) const
{
	const auto CenterPosition = Subtriangle.GetCenter();
	const FBuoyantMeshVertex CenterVertex{CenterPosition, GetHeightAboveWater(CenterPosition)};
	const auto TriangleArea = Subtriangle.GetArea();
	if (FMath::IsNearlyZero(TriangleArea)) return FForce{FVector::ZeroVector, FVector::ZeroVector};

	FVector Force = FVector::ZeroVector;

	if (bUseStaticForces)
	{
		const auto StaticForce = FBuoyantMeshSubtriangle::GetHydrostaticForce(
		    WaterDensity, GravityMagnitude, CenterVertex, TriangleNormal, TriangleArea);
		Force += StaticForce;
	}

	if (bUseDynamicForces)
	{
		const auto CenterVelocity = UpdatedComponent->GetBodyInstance()->GetUnrealWorldVelocityAtPoint(CenterPosition);
		const auto DynamicForce = FBuoyantMeshSubtriangle::GetHydrodynamicForce(
		    WaterDensity, CenterPosition, CenterVelocity, TriangleNormal, TriangleArea);
		Force += DynamicForce;
	}

	return FForce{Force, CenterPosition};
}

TArray<FVector> TMeshUtilities::GetVertices(const PxTriangleMesh* TriangleMesh)
{
	// Get vertices
	const PxVec3* const VerticesPx = TriangleMesh->getVertices();
	const PxU32 VertexCount = TriangleMesh->getNbVertices();

	TArray<FVector> VerticesUE;
	for (PxU32 VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex)
	{
		VerticesUE.Add(P2UVector(VerticesPx[VertexIndex]));
	}
	return VerticesUE;
}

TArray<int32> TMeshUtilities::GetTriangleVertexIndices(const PxTriangleMesh* TriangleMesh)
{
	// Get triangles
	const PxU32 TriangleCount = TriangleMesh->getNbTriangles();
	const void* const TriangleVertexIndicesPx = TriangleMesh->getTriangles();
	const auto b16BitIndices = TriangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;

	TArray<int32> TriangleVertexIndicesUE;
	for (PxU32 i = 0.f; i < TriangleCount * 3; ++i)
	{
		if (b16BitIndices)
		{
			const auto Indices = static_cast<const PxU16*>(TriangleVertexIndicesPx);
			TriangleVertexIndicesUE.Add(Indices[i]);
		}
		else
		{
			const auto Indices = static_cast<const PxU32*>(TriangleVertexIndicesPx);
			TriangleVertexIndicesUE.Add(Indices[i]);
		}
	}
	return TriangleVertexIndicesUE;
}

// Reference: https://wiki.unrealengine.com/Accessing_mesh_triangles_and_vertex_positions_in_build
TArray<FTriangleMesh> TMeshUtilities::GetTriangleMeshes(UStaticMeshComponent* StaticMeshComponent)
{
	if (!StaticMeshComponent) return {};
	const auto BodySetup = StaticMeshComponent->GetBodySetup();
	if (!BodySetup) return {};

	TArray<FTriangleMesh> Meshes;
	for (const auto TriangleMesh : BodySetup->TriMeshes)
	{
		Meshes.Emplace(GetVertices(TriangleMesh), GetTriangleVertexIndices(TriangleMesh));
	}
	return Meshes;
}

float TMathUtilities::SignedVolumeOfTriangle(const FVector& p1, const FVector& p2, const FVector& p3)
{
	float v321 = p3.X * p2.Y * p1.Z;
	float v231 = p2.X * p3.Y * p1.Z;
	float v312 = p3.X * p1.Y * p2.Z;
	float v132 = p1.X * p3.Y * p2.Z;
	float v213 = p2.X * p1.Y * p3.Z;
	float v123 = p1.X * p2.Y * p3.Z;

	return (1.0f / 6.0f) * (-v321 + v231 + v312 - v132 - v213 + v123);
}

// References:
// http://stackoverflow.com/questions/1406029/how-to-calculate-the-volume-of-a-3d-mesh-object-the-surface-of-which-is-made-up-t
// http://research.microsoft.com/en-us/um/people/chazhang/publications/icip01_ChaZhang.pdf
// TODO: Use local space for float precision.
float TMathUtilities::MeshVolume(UStaticMeshComponent* StaticMeshComponent)
{
	float Volume = 0.f;
	for (const auto& TriangleMesh : TMeshUtilities::GetTriangleMeshes(StaticMeshComponent))
	{
		const auto TriangleCount = TriangleMesh.TriangleVertexIndices.Num() / 3;
		for (int32 i = 0; i < TriangleCount; ++i)
		{
			const auto Vertex1 = TriangleMesh.Vertices[TriangleMesh.TriangleVertexIndices[i * 3 + 0]];
			const auto Vertex2 = TriangleMesh.Vertices[TriangleMesh.TriangleVertexIndices[i * 3 + 1]];
			const auto Vertex3 = TriangleMesh.Vertices[TriangleMesh.TriangleVertexIndices[i * 3 + 2]];

			const auto LocalToWorld = StaticMeshComponent->GetComponentTransform();

			const auto WorldVertex1 = LocalToWorld.TransformPosition(Vertex1);
			const auto WorldVertex2 = LocalToWorld.TransformPosition(Vertex2);
			const auto WorldVertex3 = LocalToWorld.TransformPosition(Vertex3);

			Volume += SignedVolumeOfTriangle(WorldVertex1, WorldVertex2, WorldVertex3);
		}
	}
	return FMath::Abs(Volume);
}