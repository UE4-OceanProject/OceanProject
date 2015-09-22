/*=================================================
* FileName: OceanManager.h
* 
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/03/20
*
* Last Edited on: 2015/21/09
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

// Cache for the "dir" variable in CalculateGerstnerWaveHeight
struct FWaveCache
{
	bool GetDir(float rotation, const FVector2D& inDirection, FVector* outDir);
	void SetDir(float rotation, const FVector2D& inDirection, const FVector& inDir);

private:
	float LastRotation = 0.f;
	FVector2D LastDirection;
	FVector MemoizedDir;
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
	FVector GetWaveHeightValue(const FVector& location, const UWorld* World = nullptr) const;


	// Returns the wave height at a determined location.
	// Same as GetWaveHeightValue, but only returns the vertical component.
	float GetWaveHeight(const FVector& location, const UWorld* World = nullptr) const;

private:

	mutable TArray<FWaveCache> WaveParameterCache;
	
	// Based on the parameters of the wave sets, the time and the position, computes the wave height.
	// Same as CalculateGerstnerWaveSetVector, but only returns the vertical component.
	float CalculateGerstnerWaveSetHeight(const FWaveParameter& global, const FWaveSetParameters& ws, const FVector2D& direction, const FVector& position, float time) const;

	// Based on the wave parameters, time and position, computes the wave height.
	// Same as CalculateGerstnerWaveVector, but only returns the vertical component.
	float CalculateGerstnerWaveHeight(float rotation, float waveLength, float amplitude, float steepness, const FVector2D& direction, const FVector& position, float time, FWaveCache& InWaveCache) const;


	FVector CalculateGerstnerWaveSetVector(const FWaveParameter& global, const FWaveSetParameters& ws, const FVector2D& direction, const FVector& position, float time) const;
	FVector CalculateGerstnerWaveVector(float rotation, float waveLength, float amplitude, float steepness, const FVector2D& direction, const FVector& position, float time, FWaveCache& InWaveCache) const;


	// Gets the time from the argument if it's not null, otherwise use GetWorld()
	float GetTimeSeconds(const UWorld* World) const;
};
