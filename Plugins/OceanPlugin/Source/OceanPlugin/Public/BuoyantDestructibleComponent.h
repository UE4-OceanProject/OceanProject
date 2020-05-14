/*=================================================
* FileName: BuoyantDestructibleComponent.h
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/08/27
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
#include "OceanManager.h"
#include "DestructibleComponent.h"
#include "BuoyantDestructibleComponent.generated.h"


UCLASS(ClassGroup = Physics, hidecategories = (Object, Mesh, "Components|SkinnedMesh", Mirroring, Activation, "Components|Activation"), config = Engine, editinlinenew, meta = (BlueprintSpawnableComponent))
class OCEANPLUGIN_API UBuoyantDestructibleComponent : public UDestructibleComponent
{
	GENERATED_UCLASS_BODY()
 
protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitializeComponent() override;

private:
	float _SignedRadius;
	float _baseAngularDamping;
	float _baseLinearDamping;
 
public:
	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Buoyancy Settings")
	AOceanManager* OceanManager;

	/* Density of each chunk */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float ChunkDensity;

	/* Density of water */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float FluidDensity;

	/* Linear damping when chunk is in fluid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float FluidLinearDamping;

	/* Angular damping when chunk is in fluid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float FluidAngularDamping;

	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Buoyancy Settings")
	FVector VelocityDamper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool ClampMaxVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float MaxUnderwaterVelocity;

	/* Radius of the test point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float TestPointRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	bool DrawDebugPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	bool EnableWaveForces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	float WaveForceMultiplier;

	/*
	* Sets the mass-normalized kinetic energy threshold below which an actor may go to sleep. 
	* Default physx value is ~50.0f (we set it 0 to avoid weird sleeping chunks on water).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	float ChunkSleepThreshold;

	/*
	* Sets the mass-normalized kinetic energy threshold below which an actor may participate in stabilization.
	* (This value has no effect if PxSceneFlag::eENABLE_STABILIZATION was not enabled on the PxSceneDesc)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	float ChunkStabilizationThreshold;
 
};
