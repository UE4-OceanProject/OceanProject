/*=================================================
* FileName: OceanManager.cpp
* 
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/03/05
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


#include "OceanPluginPrivatePCH.h"
#include "OceanManager.h"


AOceanManager::AOceanManager(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	WaveDirection = FVector(0, 1, 0);
	WaveSpeed = 1.0f;
	GlobalWaveSettings = FWaveParameter();
	WaveSet1 = FWaveSetParameters();
	//WaveSet2 = FWaveSetParameters();
	PrimaryActorTick.bCanEverTick = true;
	EnableGerstnerWaves = true;
	WaveParameterCache.AddDefaulted(8);
}

float AOceanManager::GetWaveHeight(const FVector& location)
{
	// Flat ocean buoyancy optimization
	if (!EnableGerstnerWaves)
		return RootComponent->GetComponentLocation().Z;

	float time = GetWorld()->GetTimeSeconds() + NetWorkTimeOffset;

	// Calculate the Gerstner Wave Sets
	return CalculateGerstnerWaveSetHeight(GlobalWaveSettings, WaveSet1, FVector2D(WaveDirection.X, WaveDirection.Y), location, time * WaveSpeed) + RootComponent->GetComponentLocation().Z;
}

FVector AOceanManager::GetWaveHeightValue(FVector location)
{
	//FVector sum = FVector(0, 0, 0);

	// Flat ocean buoyancy optimization
	if (!EnableGerstnerWaves)
		return FVector(location.X, location.Y, RootComponent->GetComponentLocation().Z);

	float time = GetWorld()->GetTimeSeconds() + NetWorkTimeOffset;

	// Calculate the Gerstner Wave Sets
	return CalculateGerstnerWaveSetVector(GlobalWaveSettings, WaveSet1, FVector2D(WaveDirection.X, WaveDirection.Y), location, time * WaveSpeed) + FVector(0,0,RootComponent->GetComponentLocation().Z);
	//sum +=
	// Removing this to reduce complexity, not needed
	//sum += CalculateGerstnerWaveSet(GlobalWaveSettings, WaveSet2, FVector2D(WaveDirection.X, WaveDirection.Y), location, time * WaveSpeed);

	//return sum;
}

float AOceanManager::CalculateGerstnerWaveSetHeight(const FWaveParameter& global, const FWaveSetParameters& ws, const FVector2D& direction, const FVector& position, float time)
{
	float sum = 0.f;

	// Calculate the Gerstner Waves
	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave01.Rotation, global.Length * ws.Wave01.Length,
		global.Amplitude * ws.Wave01.Amplitude, global.Steepness * ws.Wave01.Steepness, direction, position, time, WaveParameterCache[0]);
	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave02.Rotation, global.Length * ws.Wave02.Length,
		global.Amplitude * ws.Wave02.Amplitude, global.Steepness * ws.Wave02.Steepness, direction, position, time, WaveParameterCache[1]);
	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave03.Rotation, global.Length * ws.Wave03.Length,
		global.Amplitude * ws.Wave03.Amplitude, global.Steepness * ws.Wave03.Steepness, direction, position, time, WaveParameterCache[2]);
	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave04.Rotation, global.Length * ws.Wave04.Length,
		global.Amplitude * ws.Wave04.Amplitude, global.Steepness * ws.Wave04.Steepness, direction, position, time, WaveParameterCache[3]);
	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave05.Rotation, global.Length * ws.Wave05.Length,
		global.Amplitude * ws.Wave05.Amplitude, global.Steepness * ws.Wave05.Steepness, direction, position, time, WaveParameterCache[4]);
	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave06.Rotation, global.Length * ws.Wave06.Length,
		global.Amplitude * ws.Wave06.Amplitude, global.Steepness * ws.Wave06.Steepness, direction, position, time, WaveParameterCache[5]);
	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave07.Rotation, global.Length * ws.Wave07.Length,
		global.Amplitude * ws.Wave07.Amplitude, global.Steepness * ws.Wave07.Steepness, direction, position, time, WaveParameterCache[6]);
	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave08.Rotation, global.Length * ws.Wave08.Length,
		global.Amplitude * ws.Wave08.Amplitude, global.Steepness * ws.Wave08.Steepness, direction, position, time, WaveParameterCache[7]);

	return sum / 8.f;
}

FVector AOceanManager::CalculateGerstnerWaveSetVector(const FWaveParameter& global, const FWaveSetParameters& ws, const FVector2D& direction, const FVector& position, float time)
{
	FVector sum = FVector(0, 0, 0);

	// Calculate the Gerstner Waves
	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave01.Rotation, global.Length * ws.Wave01.Length,
		global.Amplitude * ws.Wave01.Amplitude, global.Steepness * ws.Wave01.Steepness, direction, position, time, WaveParameterCache[0]);
	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave02.Rotation, global.Length * ws.Wave02.Length,
		global.Amplitude * ws.Wave02.Amplitude, global.Steepness * ws.Wave02.Steepness, direction, position, time, WaveParameterCache[1]);
	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave03.Rotation, global.Length * ws.Wave03.Length,
		global.Amplitude * ws.Wave03.Amplitude, global.Steepness * ws.Wave03.Steepness, direction, position, time, WaveParameterCache[2]);
	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave04.Rotation, global.Length * ws.Wave04.Length,
		global.Amplitude * ws.Wave04.Amplitude, global.Steepness * ws.Wave04.Steepness, direction, position, time, WaveParameterCache[3]);
	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave05.Rotation, global.Length * ws.Wave05.Length,
		global.Amplitude * ws.Wave05.Amplitude, global.Steepness * ws.Wave05.Steepness, direction, position, time, WaveParameterCache[4]);
	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave06.Rotation, global.Length * ws.Wave06.Length,
		global.Amplitude * ws.Wave06.Amplitude, global.Steepness * ws.Wave06.Steepness, direction, position, time, WaveParameterCache[5]);
	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave07.Rotation, global.Length * ws.Wave07.Length,
		global.Amplitude * ws.Wave07.Amplitude, global.Steepness * ws.Wave07.Steepness, direction, position, time, WaveParameterCache[6]);
	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave08.Rotation, global.Length * ws.Wave08.Length,
		global.Amplitude * ws.Wave08.Amplitude, global.Steepness * ws.Wave08.Steepness, direction, position, time, WaveParameterCache[7]);

	return sum / 8;
}

float AOceanManager::CalculateGerstnerWaveHeight(float rotation, float waveLength, float amplitude, float steepness, const FVector2D& direction, const FVector& position, float time, FWaveCache& InWaveCache)
{
	float frequency = (2 * PI) / waveLength;

	FVector dir;
	if (!InWaveCache.GetDir(rotation, direction, &dir))
	{
		dir = FVector(direction.X, direction.Y, 0);
		dir = dir.RotateAngleAxis(rotation * 360, FVector(0, 0, 1));
		InWaveCache.SetDir(rotation, direction, dir);
	}

	float wavePhase = frequency * FVector::DotProduct(dir, position) + time;

	float s = FMath::Sin(wavePhase);

	return amplitude * s;
}

FVector AOceanManager::CalculateGerstnerWaveVector(float rotation, float waveLength, float amplitude, float steepness, const FVector2D& direction, const FVector& position, float time, FWaveCache& InWaveCache)
{
	float frequency = (2 * PI) / waveLength;

	FVector dir;
	if (!InWaveCache.GetDir(rotation, direction, &dir))
	{
		dir = FVector(direction.X, direction.Y, 0);
		dir = dir.RotateAngleAxis(rotation * 360, FVector(0, 0, 1));
		InWaveCache.SetDir(rotation, direction, dir);
	}

	float wavePhase = frequency * FVector::DotProduct(dir, position) + time;

	float c;
	float s;
	FMath::SinCos(&s, &c, wavePhase);

	float QA = steepness * amplitude;

	// Leaving this as a FVector to possibly extend it's usefulness to the BuoyancyMovementComponent (dir.X/.Y)
	return FVector(QA * dir.X * c, QA * dir.Y * c, amplitude * s);
}

bool FWaveCache::GetDir(float rotation, const FVector2D& inDirection, FVector* outDir)
{
	if (rotation == LastRotation && inDirection == LastDirection)
	{
		outDir = &MemoizedDir;
		return true;
	}
	return false;
}

void FWaveCache::SetDir(float rotation, const FVector2D& inDirection, const FVector& inDir)
{
	LastDirection = inDirection;
	LastRotation = rotation;
	MemoizedDir = inDir;
}

