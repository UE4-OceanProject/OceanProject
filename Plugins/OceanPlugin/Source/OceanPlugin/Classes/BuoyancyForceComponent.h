/*=================================================
* FileName: BuoyancyForceComponent.h
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.8.3
* Created on: 2015/04/26
*
* Last Edited on: 2015/08/27
* Last Edited by: TK-Master
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

#include "OceanManager.h"
#include "OceanPluginPrivatePCH.h"
#include "BuoyancyForceComponent.generated.h"

//Custom bone density/radius override struct.
USTRUCT(BlueprintType)
struct FStructBoneOverride
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Buoyancy)
	FName BoneName;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Buoyancy)
	float Density;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Buoyancy)
	float TestRadius;

	//Default struct values
	FStructBoneOverride()
	{
		Density = 600.f;
		TestRadius = 10.f;
	}
};

/** 
 *	Applies buoyancy forces to physics objects.
 *	OceanManager is required in the level for this to work.
 */
UCLASS(hidecategories=(Object, Mobility, LOD), ClassGroup=Physics, showcategories=Trigger, MinimalAPI, meta=(BlueprintSpawnableComponent))
class UBuoyancyForceComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

	/* OceanManager used by the component, if unassigned component will auto-detect */
	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Buoyancy Settings")
	AOceanManager* OceanManager;
	
	/* Density of mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float MeshDensity;

	/* Density of water. Typically you don't need to change this. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float FluidDensity;

	/* Linear damping when object is in fluid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float FluidLinearDamping;

	/* Angular damping when object is in fluid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float FluidAngularDamping;

	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Buoyancy Settings")
	FVector VelocityDamper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool ClampMaxVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float MaxUnderwaterVelocity;

	/* Radius of the points. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float TestPointRadius;

	/* Test point array. At least one point is required for buoyancy. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	TArray<FVector> TestPoints;

	/* If skeletal mesh with physics asset, it will apply buoyancy force at the COM of each bone instead of using the test point array. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool ApplyForceToBones;

	/* If object has no physics enabled, snap to water surface. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool SnapToSurfaceIfNoPhysics;

	/* Per-point mesh density override, can be used for half-sinking objects etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	TArray<float> PointDensityOverride;

	/* Density & radius overrides per skeletal bone (ApplyForceToBones needs to be true). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	TArray<FStructBoneOverride> BoneOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	bool DrawDebugPoints;

	/**
	* Stay upright physics constraint (inspired by UDK's StayUprightSpring)
	* -STILL WIP-
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	bool EnableStayUprightConstraint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	float StayUprightStiffness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	float StayUprightDamping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	FRotator StayUprightDesiredRotation;

	/**
	* Waves will push objects towards the wave direction set in the Ocean Manager.
	* -STILL WIP-
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	bool EnableWaveForces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	float WaveForceMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	TEnumAsByte<enum ETickingGroup> TickGroup;

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitializeComponent() override;
	//virtual void PostLoad() override;
	//End UActorComponent Interface

private:

	static FVector GetUnrealVelocityAtPoint(UPrimitiveComponent* Target, FVector Point, FName BoneName = NAME_None);
	void ApplyUprightConstraint(UPrimitiveComponent* BasePrimComp);

	float _baseAngularDamping;
	float _baseLinearDamping;
	
};
