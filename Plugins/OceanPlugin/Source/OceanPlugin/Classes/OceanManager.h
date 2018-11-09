/*=================================================
* FileName: OceanManager.h
* 
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/03/20
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
#include "Landscape.h"
#include "OceanManager.generated.h"


/*
* Contains the parameters necessary for a single Gerstner wave.
*/
USTRUCT(BlueprintType)
struct OCEANPLUGIN_API FWaveParameter
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float Rotation;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float Length;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float Amplitude;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float Steepness;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float TimeScale;

	FORCEINLINE FWaveParameter(float InRotation, float InLength, float InAmplitude, float InSteepness, float InTimeScale);

	//Default struct values
	FWaveParameter()
	{
		Rotation = 0.45f;
		Length = 1200.f;
		Amplitude = 100.f;
		Steepness = 0.8f;
		TimeScale = 1.f;
	}
};

FORCEINLINE FWaveParameter::FWaveParameter(float InRotation, float InLength, float InAmplitude, float InSteepness, float InTimeScale)
	: Rotation(InRotation), Length(InLength), Amplitude(InAmplitude), Steepness(InSteepness), TimeScale(InTimeScale)
{ }

/*
* Contains the parameters necessary for a set of Gerstner waves.
*/
USTRUCT(BlueprintType)
struct OCEANPLUGIN_API FWaveSetParameters
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	FWaveParameter Wave01;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	FWaveParameter Wave02;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	FWaveParameter Wave03;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	FWaveParameter Wave04;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	FWaveParameter Wave05;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	FWaveParameter Wave06;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	FWaveParameter Wave07;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	FWaveParameter Wave08;

	//Default struct values
	FWaveSetParameters()
	{
		Wave01 = FWaveParameter(0.f, 1.05f, 1.4f, 1.2f, 1.f);
		Wave02 = FWaveParameter(-0.05f, 0.65f, 1.1f, 0.6f, 1.f);
		Wave03 = FWaveParameter(0.045f, 1.85f, 2.1f, 1.35f, 1.f);
		Wave04 = FWaveParameter(0.02f, 0.65f, 0.9f, 0.9f, 1.f);
		Wave05 = FWaveParameter(-0.015f, 1.28f, 1.854f, 1.2f, 1.f);
		Wave06 = FWaveParameter(0.065f, 0.75f, 1.15f, 0.5f, 1.f);
		Wave07 = FWaveParameter(0.01f, 1.15f, 1.55f, 1.15f, 1.f);
		Wave08 = FWaveParameter(-0.04f, 1.45f, 1.75f, 0.45f, 1.f);
	}
};


// Cache for the "dir" variable in CalculateGerstnerWaveHeight
struct FWaveCache
{
	bool GetDir(float rotation, const FVector2D& inDirection, FVector* outDir);
	void SetDir(float rotation, const FVector2D& inDirection, const FVector& inDir);

private:
	float LastRotation = 0.f;
	FVector2D LastDirection;
	FVector MemorizedDir;
};


/**
* OceanManager calculates the Gerstner waves in code, while the Material uses it's own implementation in a MaterialFunction.
* TODO: Investigate whether a single implementation could be used to increase performance.
*/
UCLASS(BlueprintType, Blueprintable)
class OCEANPLUGIN_API AOceanManager : public AActor
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	bool EnableGerstnerWaves;

	// The global direction the waves travel.
	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	FVector2D GlobalWaveDirection;

	// The global speed multiplier of the waves.
	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float GlobalWaveSpeed;

	// The global amplitude multiplier of the waves.
	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float GlobalWaveAmplitude;

	/* Optimization: 
	* If the distance of a buoyant point to base sea level exceeds DistanceCheck,
	* skip the Gerstner calculations and return base sea level.
	*/
	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float DistanceCheck;

	/* Median Gerstner wave settings 
	(only 1 cluster is used in the material by default).*/
	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	TArray<FWaveParameter> WaveClusters;

	/* Individual Gerstner wave settings.
	(leave blank to use the default offsets).*/
	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	TArray<FWaveSetParameters> WaveSetOffsetsOverride;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite)
	float NetWorkTimeOffset;

	UFUNCTION(BlueprintCallable, Category = "Ocean Manager", meta = (HidePin = "World"))
	FVector GetWaveHeightValue(const FVector& location, const UWorld* World = nullptr, bool HeightOnly = true, bool TwoIterations = false);

	// Returns the wave height at a determined location.
	// Same as GetWaveHeightValue, but only returns the vertical component.
	float GetWaveHeight(const FVector& location, const UWorld* World = nullptr) const;

	//Landscape height modulation vars.
	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	bool bEnableLandscapeModulation;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float ModulationStartHeight;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float ModulationMaxHeight;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	float ModulationPower;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	ALandscape* Landscape;

	UPROPERTY(Category = "Ocean", BlueprintReadWrite, EditAnywhere)
	UTexture2D* HeightmapTexture;

	UFUNCTION(BlueprintCallable, Category = "Ocean Manager")
	void LoadLandscapeHeightmap(UTexture2D* Tex2D);

	UFUNCTION(BlueprintCallable, Category = "Ocean Manager")
	FLinearColor GetHeightmapPixel(float U, float V) const;

private:

	virtual void BeginPlay() override;

	TArray<FFloat16Color> HeightmapPixels;
	int32 HeightmapWidth;
	int32 HeightmapHeight;

	mutable TArray<FWaveCache> WaveParameterCache;
	
	// Based on the parameters of the wave sets, the time and the position, computes the wave height.
	// Same as CalculateGerstnerWaveSetVector, but only returns the vertical component.
	float CalculateGerstnerWaveSetHeight(const FVector& position, float time) const;

	FVector CalculateGerstnerWaveSetVector(const FVector& position, float time, bool CalculateXY, bool CalculateZ) const;
	FVector CalculateGerstnerWaveVector(float rotation, float waveLength, float amplitude, float steepness, const FVector2D& direction, const FVector& position, float time, FWaveCache& InWaveCache, bool CalculateXY, bool CalculateZ) const;


	// Gets the time from the argument if it's not null, otherwise use GetWorld()
	float GetTimeSeconds(const UWorld* World) const;
};
