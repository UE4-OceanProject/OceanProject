/*=================================================
* FileName: BuoyantMeshCompoment.h
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
#include "BuoyantMeshTriangle.h"
#include "Components/StaticMeshComponent.h"
#include "PhysXIncludes.h"
#include "PhysXPublic.h"
#include "OceanManager.h"
#include "BuoyantMeshCompoment.generated.h"

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

The component can request the wave height up to 8 times per triangle per tick, so meshes should be
kept fairly simple.
If performance is required, a water heightmap can be implemented as described in the article.
Alternatively, the water heights could be cached so shared vertices won't cause the height to be
computed
multiple times.

*/

UCLASS(ClassGroup = Physics, config = Engine, editinlinenew, meta = (BlueprintSpawnableComponent))
class UBuoyantMeshCompoment : public UStaticMeshComponent
{
	GENERATED_BODY()

   public:
	// Sets default values for this component's properties
	UBuoyantMeshCompoment();

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

	// Draws arrows representing the buoyancy forces pushing on the mesh.
	// The length is proportional to the force magnitude.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawForceArrows;

	// Draws the waterline on the mesh.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebugWaterline;

	// Force arrow size multiplier.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float ForceArrowSize = 1.e-7f;

	// Only use the vertical component of the buoyancy forces.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool bVerticalForcesOnly;

	// Density of the fluid in kg/uu^3. It is around 0.001027 if 1 unreal unit is 1 cm.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float WaterDensity;

	// OceanManager used by the component, if unassigned component will auto-detect.
	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Buoyancy Settings")
	AOceanManager* OceanManager;

	struct FTriangle
	{
		FVector A;
		FVector B;
		FVector C;
	};

#pragma region UActorComponent Interface

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

#pragma endregion

   private:
	bool bNeedsInitialization = true;
	void Initialize();
	TArray<FTriangle> Triangles;

	// Gets the height above the water at a determined position.
	// The wave height is given by the AOceanManager if available, otherwise it
	// is 0.
	float GetHeightAboveWater(const FVector& Position) const;

	void ApplyHydrostaticForces(
		const TArray<FBuoyantMeshTriangle::FForceApplicationParameters>& ForceParams);

	// Retrieves the triangles in the static mesh.
	// The method is described here:
	// https://wiki.unrealengine.com/Accessing_mesh_triangles_and_vertex_positions_in_build
	TArray<FTriangle> GetTriangles(const UBodySetup& BodySetup) const;

	// Retrieves the triangles in a PhysX mesh.
	TArray<FTriangle> GetTriangles(const PxTriangleMesh& TriangleMesh) const;

	// Transforms the points in a triangle.
	// Returns a transformed copy of the triangle.
	FTriangle TransformTriangle(const FTransform& Transform, const FTriangle& Triangle) const;

	// Gets the buoyancy forces affecting a triangle.
	TArray<FBuoyantMeshTriangle::FForceApplicationParameters> GetHydrostaticForces(
		const FTriangle& Triangle) const;
};
