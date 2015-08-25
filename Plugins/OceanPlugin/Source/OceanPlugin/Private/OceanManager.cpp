/*=================================================
* FileName: OceanManager.cpp
* 
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.8.3
* Created on: 2015/03/05
*
* Last Edited on: 2015/03/29
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
}

FVector AOceanManager::GetWaveHeightValue(FVector location)
{
	//FVector sum = FVector(0, 0, 0);

	// Flat ocean buoyancy optimization
	if (!EnableGerstnerWaves)
		return FVector(location.X, location.Y, RootComponent->GetComponentLocation().Z);

	float time = GetWorld()->GetTimeSeconds() + NetWorkTimeOffset;

	// Calculate the Gerstner Wave Sets
	return CalculateGerstnerWaveSet(GlobalWaveSettings, WaveSet1, FVector2D(WaveDirection.X, WaveDirection.Y), location, time * WaveSpeed) + FVector(0,0,RootComponent->GetComponentLocation().Z);
	//sum +=
	// Removing this to reduce complexity, not needed
	//sum += CalculateGerstnerWaveSet(GlobalWaveSettings, WaveSet2, FVector2D(WaveDirection.X, WaveDirection.Y), location, time * WaveSpeed);

	//return sum;
}

FVector AOceanManager::CalculateGerstnerWaveSet(FWaveParameter global, FWaveSetParameters ws, FVector2D direction, FVector position, float time)
{
	FVector sum = FVector(0, 0, 0);

	// Calculate the Gerstner Waves
	sum += CalculateGertnerWave(global.Rotation + ws.Wave01.Rotation, global.Length * ws.Wave01.Length,
		global.Amplitude * ws.Wave01.Amplitude, global.Steepness * ws.Wave01.Steepness, direction, position, time);
	sum += CalculateGertnerWave(global.Rotation + ws.Wave02.Rotation, global.Length * ws.Wave02.Length,
		global.Amplitude * ws.Wave02.Amplitude, global.Steepness * ws.Wave02.Steepness, direction, position, time);
	sum += CalculateGertnerWave(global.Rotation + ws.Wave03.Rotation, global.Length * ws.Wave03.Length,
		global.Amplitude * ws.Wave03.Amplitude, global.Steepness * ws.Wave03.Steepness, direction, position, time);
	sum += CalculateGertnerWave(global.Rotation + ws.Wave04.Rotation, global.Length * ws.Wave04.Length,
		global.Amplitude * ws.Wave04.Amplitude, global.Steepness * ws.Wave04.Steepness, direction, position, time);
	sum += CalculateGertnerWave(global.Rotation + ws.Wave05.Rotation, global.Length * ws.Wave05.Length,
		global.Amplitude * ws.Wave05.Amplitude, global.Steepness * ws.Wave05.Steepness, direction, position, time);
	sum += CalculateGertnerWave(global.Rotation + ws.Wave06.Rotation, global.Length * ws.Wave06.Length,
		global.Amplitude * ws.Wave06.Amplitude, global.Steepness * ws.Wave06.Steepness, direction, position, time);
	sum += CalculateGertnerWave(global.Rotation + ws.Wave07.Rotation, global.Length * ws.Wave07.Length,
		global.Amplitude * ws.Wave07.Amplitude, global.Steepness * ws.Wave07.Steepness, direction, position, time);
	sum += CalculateGertnerWave(global.Rotation + ws.Wave08.Rotation, global.Length * ws.Wave08.Length,
		global.Amplitude * ws.Wave08.Amplitude, global.Steepness * ws.Wave08.Steepness, direction, position, time);

	return sum / 8;
}


FVector AOceanManager::CalculateGertnerWave(float rotation, float waveLength, float amplitude, float steepness, FVector2D direction, FVector position, float time)
{
	float frequency = (2 * PI) / waveLength;

	FVector dir = FVector(direction.X, direction.Y, 0);
	dir = dir.RotateAngleAxis(rotation * 360, FVector(0, 0, 1));

	float wavePhase = frequency * FVector::DotProduct(dir, position) + time;

	float c = FMath::Cos(wavePhase);
	float s = FMath::Sin(wavePhase);

	float QA = steepness * amplitude;

	// Leaving this as a FVector to possibly extend it's usefulness to the BuoyancyMovementComponent (dir.X/.Y)
	return FVector(QA * dir.X * c, QA * dir.Y * c, amplitude * s);
}
