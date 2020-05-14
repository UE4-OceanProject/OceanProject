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

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PhysXIncludes.h"
#include "BuoyantMeshComponent.generated.h"


struct BuoyantMeshVertex;
struct FBuoyantMeshTriangle;
struct FBuoyantMeshSubtriangle;
class AOceanManager;
class UWaterHeightmapComponent;

struct FTriangleMesh
{
	const TArray<FVector> Vertices;
	const TArray<int32> TriangleVertexIndices;

	FTriangleMesh(const TArray<FVector>& Vertices, const TArray<int32>& TriangleVertexIndices)
	    : Vertices{Vertices}, TriangleVertexIndices{TriangleVertexIndices}
	{
	}
};

/*

This component applies to the root component buoyancy forces modeled from a static mesh.
The algorithm used is described in "Water interaction model for boats in video
games" by Jacques Kerner.
http://gamasutra.com/view/news/237528/Water_interaction_model_for_boats_in_video_games.php

In addition, support for dynamic (drag) forces is included.

*/

UCLASS(ClassGroup = Physics, config = Engine, editinlinenew, meta = (BlueprintSpawnableComponent))
class OCEANPLUGIN_API UBuoyantMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

   public:
	// Sets default values for this component's properties
	UBuoyantMeshComponent();

	// The component on which the forces will be applied. If this is null at startup, it will
	// automatically select the first UPrimitiveComponent parent, or itself if one is not found.
	UPrimitiveComponent* UpdatedComponent;

	// Only use the vertical component of the buoyancy forces.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool bVerticalForcesOnly = false;

	// Use a WaterPatchComponent to possibly improve performance.
	// This actor needs a WaterPatchComponent for this to work.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool bUseWaterPatch = true;

	// Use hydrostatic (buoyancy) forces if true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool bUseStaticForces = true;

	// Use hydrodynamic (drag) forces if true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool bUseDynamicForces = true;

	// OceanManager used by the component, if unassigned component will auto-detect.
	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Buoyancy Settings")
	AOceanManager* OceanManager = nullptr;

	// Draw arrows representing the buoyancy forces pushing on the mesh?
	// The length is proportional to the force magnitude.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawForceArrows = false;

	// Draw the waterline on the mesh?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawWaterline = false;

	// Draw the mesh vertices?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawVertices = false;

	// Draw the original mesh triangles?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawTriangles = false;

	// Draw the submerged triangles?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawSubtriangles = false;

	// Force arrow size multiplier.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float ForceArrowSize = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mass Settings")
	bool bOverrideMeshDensity = false;

	// Density of the mesh in kg/uu^3. The object will sink if it's higher than the water density.
	// Does nothing if bOverrideMeshDensity is false.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mass Settings")
	float MeshDensity = 0.000800f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mass Settings")
	bool bOverrideMass = false;

	// Mass of the rigidbody in kg.
	// Does nothing if bOverrideMass is false.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mass Settings")
	float Mass = 10000.f;

	// Density of the water in kg/uu^3. It is around 0.001027 if 1 unreal unit is 1 cm.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mass Settings")
	float WaterDensity = 0.001027f;

	struct FForce
	{
		FVector Vector;
		// Application point of the force
		FVector Point;
		FForce(const FVector& Vector, const FVector& Point) : Vector{Vector}, Point{Point}
		{
		}
	};

   protected:
	virtual void TickComponent(float DeltaTime,
	                           enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

   private:
	bool bHasInitialized = false;
	void Initialize();

	void SetMassProperties();

	UWorld* World = nullptr;
	float GravityMagnitude = 0.f;

	AOceanManager* FindOceanManager() const;
	UWaterHeightmapComponent* FindWaterHeightmap() const;

	FForce GetSubmergedTriangleForce(const FBuoyantMeshSubtriangle& Subtriangle, const FVector& TriangleNormal) const;

	void ApplyMeshForces();

	void ApplyMeshForce(const FForce& Force);

	void SetupTickOrder();

	TArray<FTriangleMesh> TriangleMeshes;

	// Attempts to get the height above the water of a point.
	float GetHeightAboveWater(const FVector& Position) const;

	static void DrawDebugTriangle(UWorld* const World,
	                              const FVector& A,
	                              const FVector& B,
	                              const FVector& C,
	                              const FColor& Color,
	                              const float Thickness);

	// Returns the parent component as a UPrimitiveComponent, if it is one.
	UPrimitiveComponent* GetParentPrimitive() const;

	UPROPERTY()
	UWaterHeightmapComponent* WaterHeightmap = nullptr;
};

class TMeshUtilities
{
   public:
	static TArray<FTriangleMesh> GetTriangleMeshes(UStaticMeshComponent* StaticMeshComponent);

   private:
	static TArray<FVector> GetVertices(const PxTriangleMesh* TriangleMesh);
	static TArray<int32> GetTriangleVertexIndices(const PxTriangleMesh* TriangleMesh);
};

class TMathUtilities
{
   public:
	// Calculates the volume of a triangle mesh.
	static float MeshVolume(UStaticMeshComponent* StaticMeshComponent);

   private:
	/*
	The signed volume of a triangle in a triangle mesh.
	The magnitude of this value is the volume of the tetrahedron formed by the triangle and the origin, while the
	sign of the value is determined by checking the position of the origin with respect to the edge and the direction
	of the normal.
	Used in the triangle mesh volume calculation.
	Reference: http://research.microsoft.com/en-us/um/people/chazhang/publications/icip01_ChaZhang.pdf
	*/
	static float SignedVolumeOfTriangle(const FVector& p1, const FVector& p2, const FVector& p3);
};
