/*=================================================
* FileName: BuoyancyComponent.h
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/03/29
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
#include "GameFramework/MovementComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "OceanManager.h"
#include "BuoyancyComponent.generated.h"


/**
 *	Buoyancy component
 *	OceanManager is required in the level for this to work.
 */
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), HideCategories = (PlanarMovement, "Components|Movement|Planar", Velocity))
class OCEANPLUGIN_API UBuoyancyComponent : public UMovementComponent
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

	/* Per-point mesh density override, can be used for half-sinking objects etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	TArray<float> PointDensityOverride;

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

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitializeComponent() override;
	//End UActorComponent Interface

private:

	static FVector GetVelocityAtPoint(UPrimitiveComponent* Target, FVector Point, FName BoneName = NAME_None);

	void ApplyUprightConstraint();
	UPhysicsConstraintComponent* ConstraintComp;
	bool _hasTicked;

	float _SignedRadius;
	float _baseAngularDamping;
	float _baseLinearDamping;

	UWorld* World;
	
};
