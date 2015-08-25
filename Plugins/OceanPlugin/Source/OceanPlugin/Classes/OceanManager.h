/*=================================================
* FileName: OceanManager.h
* 
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.8.3
* Created on: 2015/03/20
*
* Last Edited on: 2015/08/09
* Last Edited by: DotCam
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
#include "GameFramework/Actor.h"
#include "OceanManager.generated.h"


/*
* Contains the parameters necessary for a single Gerstner wave.
*/
USTRUCT(BlueprintType)
struct FWaveParameter {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Rotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Length;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Amplitude;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Steepness;
	};

/*
* Contains the parameters necessary for a set of Gerstner waves.
*/
USTRUCT(BlueprintType)
struct FWaveSetParameters {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaveParameter Wave01;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaveParameter Wave02;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaveParameter Wave03;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaveParameter Wave04;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaveParameter Wave05;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaveParameter Wave06;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaveParameter Wave07;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FWaveParameter Wave08;
	};



/**
* OceanManager calculates the Gerstner waves in code, while the Material uses it's own implementation in a MaterialFunction.
* TODO: Investigate whether a single implementation could be used to increase performance.
*/
UCLASS(BlueprintType, Blueprintable)
class AOceanManager : public AActor {
	GENERATED_UCLASS_BODY()

	// The Direction the waves travel
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector WaveDirection;

	// The speed of the waves
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WaveSpeed;

	// The globally applied wave settings
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FWaveParameter GlobalWaveSettings;

	// Individual wave settings for wave set 1
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FWaveSetParameters WaveSet1;

	// Individual wave settings for wave set 2
	//UPROPERTY() //BlueprintReadWrite, EditAnywhere) - REMOVING ACCESS to reduce complexity
	//FWaveSetParameters WaveSet2;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool EnableGerstnerWaves;

	UPROPERTY(BlueprintReadWrite)
	float NetWorkTimeOffset;

	UFUNCTION(BlueprintCallable, Category = "Ocean Manager")
	FVector GetWaveHeightValue(FVector location);

private:
	FVector CalculateGerstnerWaveSet(FWaveParameter global, FWaveSetParameters ws, FVector2D direction, FVector position, float time);

	FVector CalculateGertnerWave(float rotation, float waveLength, float amplitude, float steepness, FVector2D direction, FVector position, float time);
	};
