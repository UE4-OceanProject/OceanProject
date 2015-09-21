// Fill out your copyright notice in the Description page of Project Settings.

#include "BandK.h"
#include "BuoyantMeshCompoment.h"

#include "PhysicsEngine/BodySetup.h"
#include "PhysXIncludes.h"
#include "PhysXPublic.h"

#include "OceanManager.h"
#include "BuoyantMeshTriangle.h"

typedef FBuoyantMeshTriangle::FForceApplicationParameters ForceParams;
typedef UBuoyantMeshCompoment::FTriangle FTriangle;

// Sets default values for this component's properties
UBuoyantMeshCompoment::UBuoyantMeshCompoment()
{
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	bVerticalForcesOnly = false;
	UActorComponent::SetComponentTickEnabled(true);

	WaterDensity = 0.001027f;
}

void UBuoyantMeshCompoment::InitializeComponent()
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
void UBuoyantMeshCompoment::BeginPlay()
{
	Super::BeginPlay();
}

float UBuoyantMeshCompoment::GetHeightAboveWater(const FVector& Position) const
{
	SCOPE_CYCLE_COUNTER(STAT_GetHeightAboveWater);
	float WaterHeight = 0.f;
	if (IsValid(UpdatedPrimitive) && IsValid(OceanManager))
	{
		WaterHeight = OceanManager->GetWaveHeightValue(Position).Z;
	}
	return Position.Z - WaterHeight;
}

TArray<ForceParams> UBuoyantMeshCompoment::GetHydrostaticForces(
	const FTriangle& Triangle) const
{
	SCOPE_CYCLE_COUNTER(STAT_GetHydrostaticForces);
	const auto GravityMagnitude = FMath::Abs(GetWorld()->GetGravityZ());

	// Get hydrostatic forces that are affecting the triangle.
	return FBuoyantMeshTriangle::GetHydrostaticForces(Triangle.A, Triangle.B, Triangle.C,
		WaterDensity, GravityMagnitude,
		[this](FVector Position)
	{
		return GetHeightAboveWater(Position);
	},
		World, bDrawDebugWaterline);
}

void UBuoyantMeshCompoment::ApplyHydrostaticForces(const TArray<FBuoyantMeshTriangle::FForceApplicationParameters>& ForceParams)
{
	for (const auto ForceParam : ForceParams)
	{
		const auto Force =
			bVerticalForcesOnly ? FVector{ 0.f, 0.f, ForceParam.Force.Z } : ForceParam.Force;
		// Don't propagate artifacts caused by dividing by small numbers
		if (!Force.IsNearlyZero() && isfinite(Force.X) && isfinite(Force.Y) && isfinite(Force.Z))
		{
			if (bDrawForceArrows)
			{
				DrawDebugDirectionalArrow(
					World, ForceParam.ApplicationPoint - (ForceParam.Force * ForceArrowSize * 0.00001f),
					ForceParam.ApplicationPoint, ForceParam.Force.Size() * ForceArrowSize * 0.2f,
					FColor(255, 255, 255));
			}
			UpdatedPrimitive->AddForceAtLocation(ForceParam.Force, ForceParam.ApplicationPoint);
		}
	}
}

void UBuoyantMeshCompoment::Initialize()
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
	const auto BodySetup = GetBodySetup();
	if (IsValid(BodySetup))
	{
		Triangles = GetTriangles(*BodySetup);
	}

}

TArray<FTriangle> UBuoyantMeshCompoment::GetTriangles(const UBodySetup& BodySetup) const
{
	TArray<FTriangle> Triangles;
	for (const auto TriangleMesh : BodySetup.TriMeshes)
	{
		if (TriangleMesh != nullptr)
		{
			Triangles.Append(GetTriangles(*TriangleMesh));
		}
	}
	return Triangles;
}

TArray<FTriangle> UBuoyantMeshCompoment::GetTriangles(
	const PxTriangleMesh& TriangleMesh) const
{
	// Get Triangles
	const PxU32 TriangleCount = TriangleMesh.getNbTriangles();
	const void* const TriangleIndices = TriangleMesh.getTriangles();

	// Get Vertices
	const PxVec3* const Vertices = TriangleMesh.getVertices();

	TArray<FTriangle> Triangles;

	if (TriangleMesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES)
	{
		// The mesh has 16bit vertex indices.
		const auto Triangles16 = static_cast<const PxU16*>(TriangleIndices);
		for (PxU32 TriangleIndex = 0; TriangleIndex < TriangleCount; ++TriangleIndex)
		{
			FTriangle Triangle = { P2UVector(Vertices[Triangles16[TriangleIndex * 3]]),
				P2UVector(Vertices[Triangles16[TriangleIndex * 3 + 1]]),
				P2UVector(Vertices[Triangles16[TriangleIndex * 3 + 2]]) };
			Triangles.Emplace(Triangle);
		}
	}
	else
	{
		// The mesh has 32bit vertex indices.
		const auto Triangles32 = static_cast<const PxU32*>(TriangleIndices);
		for (PxU32 TriangleIndex = 0; TriangleIndex < TriangleCount; ++TriangleIndex)
		{
			FTriangle Triangle = { P2UVector(Vertices[Triangles32[TriangleIndex * 3]]),
				P2UVector(Vertices[Triangles32[TriangleIndex * 3 + 1]]),
				P2UVector(Vertices[Triangles32[TriangleIndex * 3 + 2]]) };
			Triangles.Emplace(Triangle);
		}
	}

	return Triangles;
}

// Called every frame
void UBuoyantMeshCompoment::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bNeedsInitialization)
	{
		bNeedsInitialization = false;
		Initialize();
	}

	const auto LocalToWorld = GetComponentTransform();
	for (const auto& Triangle : Triangles)
	{
		const auto WorldTriangle = TransformTriangle(LocalToWorld, Triangle);
		ApplyHydrostaticForces(GetHydrostaticForces(WorldTriangle));
	}
}

FTriangle UBuoyantMeshCompoment::TransformTriangle(const FTransform& Transform,
	const FTriangle& Triangle) const
{
	FTriangle TransformedTriangle = { Transform.TransformPosition(Triangle.A),
		Transform.TransformPosition(Triangle.B),
		Transform.TransformPosition(Triangle.C) };
	return TransformedTriangle;
}