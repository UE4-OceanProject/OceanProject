/*=================================================
* FileName: BuoyantMeshComponent.h
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

#pragma once

#include "OceanPluginPrivatePCH.h"
#include "BuoyantMesh/BuoyantMeshVertex.h"
#include "BuoyantMesh/BuoyantMeshTriangle.h"
#include "BuoyantMesh/BuoyantMeshSubtriangle.h"
#include "Components/StaticMeshComponent.h"
#include "PhysXIncludes.h"
#include "PhysXPublic.h"
#include "OceanManager.h"
#include "BuoyantMeshComponent.generated.h"

// For the UE4 Profiler
DECLARE_STATS_GROUP(TEXT("BuoyantMeshComponent"), STATGROUP_BuoyantMeshComponent, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("GetHydrostaticForces"), STAT_GetHydrostaticForces,
				   STATGROUP_BuoyantMeshComponent);
DECLARE_CYCLE_STAT(TEXT("GetHeightAboveWater"), STAT_GetHeightAboveWater,
				   STATGROUP_BuoyantMeshComponent);
/*

This component applies to the root component buoyancy forces modeled from a static mesh.
The algorithm used is described in "Water interaction model for boats in video
games" by Jacques Kerner.
http://gamasutra.com/view/news/237528/Water_interaction_model_for_boats_in_video_games.php

It does not implement the water heightmap or the rotation-free triangle centers in the appendix.

Use simplified hull-shaped meshes to keep performance acceptable.

*/

UCLASS(ClassGroup = Physics, config = Engine, editinlinenew, meta = (BlueprintSpawnableComponent))
class OCEANPLUGIN_API UBuoyantMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

   public:
	// Sets default values for this component's properties
	UBuoyantMeshComponent();

	/**
	* The component we move and update.
	* If this is null at startup, the owning Actor's root component will automatically be set as our
	* UpdatedComponent at startup.
	* @see UpdatedPrimitive
	*/
	UPROPERTY(BlueprintReadOnly, DuplicateTransient, Category = MovementComponent)
	USceneComponent* UpdatedComponent;

	/**
	* UpdatedComponent, cast as a UPrimitiveComponent. May be invalid if UpdatedComponent was null
	* or not a
	* UPrimitiveComponent.
	*/
	UPROPERTY(BlueprintReadOnly, DuplicateTransient, Category = MovementComponent)
	UPrimitiveComponent* UpdatedPrimitive;

	// Draw arrows representing the buoyancy forces pushing on the mesh?
	// The length is proportional to the force magnitude.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawForceArrows;

	// Draw the waterline on the mesh?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawWaterline;

	// Draw the mesh vertices?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawVertices;

	// Draw the original mesh triangles?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawTriangles;

	// Draw the submerged triangles?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawSubtriangles;

	// Force arrow size multiplier.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float ForceArrowSize = 1.f;

	// Only use the vertical component of the buoyancy forces.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool bVerticalForcesOnly;

	// Density of the fluid in kg/uu^3. It is around 0.001027 if 1 unreal unit is 1 cm.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float WaterDensity;

	// OceanManager used by the component, if unassigned component will auto-detect.
	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Buoyancy Settings")
	AOceanManager* OceanManager;

#pragma region UActorComponent Interface

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

#pragma endregion

	struct FForce
	{
		FVector Vector;
		// Application point of the force
		FVector Point;
	};

   private:
	bool bNeedsInitialization = true;
	void Initialize();

	// Triangles are stored as indices to vertices in the vertex array.
	// This function gets those indices from the triangles array.
	static void GetTriangleVertexIndices(const TArray<FVector>& WorldVertexPositions,
										 const void* const VertexIndices, const PxU32 TriangleIndex,
										 const bool b16BitIndices, int32* OutIndex1,
										 int32* OutIndex2, int32* OutIndex3);

	// Triangles are stored as indices to vertices in the vertex array.
	// This function gets those indices from the triangles array.
	template <class T>
	static void GetTriangleVertexIndices(const TArray<FVector>& WorldVertexPositions,
										 const T* const VertexIndices, const PxU32 TriangleIndex,
										 int32* OutIndex1, int32* OutIndex2, int32* OutIndex3);

	// Adds the hydrostatic force pressing on a submerged triangle to an array of forces.
	void GetSubtriangleForces(const UWorld& World, TArray<FForce>& InOutForces,
							  const float GravityMagnitude, const FVector& TriangleNormal,
							  const FBuoyantMeshSubtriangle& Subtriangle) const;
	// Adds the hydrostatic forces pressing on PhysX triangle mesh to an array of forces.
	void GetTriangleMeshForces(TArray<FForce>& InOutForces, UWorld& InWorld,
							   const PxTriangleMesh& TriangleMesh) const;

	// Adds the hydrostatic forces pressing on a static mesh to an array of forces.
	void GetStaticMeshForces(TArray<FForce>& InOutForces, UWorld& InWorld,
							 const UBodySetup& BodySetup) const;

	// Applies the buoyancy forces to UpdatedPrimitive.
	void ApplyHydrostaticForce(UWorld& World, const FForce& Force);
	// Gets the height above the water at a determined position.
	// The wave height is given by the AOceanManager if available, otherwise it
	// is 0.
	float GetHeightAboveWater(const UWorld& World, const FVector& Position) const;

	static void DrawDebugTriangle(const UWorld& World, const FVector& A, const FVector& B,
								  const FVector& C, const FColor& Color, const float Thickness);
};
