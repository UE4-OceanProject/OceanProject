/*=================================================
* FileName: OceanManager.cpp
* 
* Created by: DotCam
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/03/05
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

#include "OceanManager.h"
#include <Engine/World.h>
#include <Engine/Texture2D.h>


AOceanManager::AOceanManager(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	EnableGerstnerWaves = true;
	GlobalWaveDirection = FVector2D(0, 1);
	GlobalWaveSpeed = 1.f;
	GlobalWaveAmplitude = 1.f;
	DistanceCheck = 2000.f;
	WaveClusters.AddDefaulted(1);
	WaveParameterCache.AddDefaulted(8);

	ModulationStartHeight = -2000.f;
	ModulationMaxHeight = 200.f;
	ModulationPower = 0.9f;
}

void AOceanManager::BeginPlay()
{
	Super::BeginPlay();

	if (HeightmapTexture)
	{
		LoadLandscapeHeightmap(HeightmapTexture);
	}
}

float AOceanManager::GetTimeSeconds(const UWorld* World) const
{
	if (World == nullptr) World = GetWorld();
	return World->GetTimeSeconds() + NetWorkTimeOffset;
}

float AOceanManager::GetWaveHeight(const FVector& location, const UWorld* World) const
{
	// Flat ocean buoyancy optimization
	if (!EnableGerstnerWaves)
		return RootComponent->GetComponentLocation().Z;

	// GetWorld() can have a significant impact on the performace of this function, so let's give the caller the option to supply a cached result.
	const float time = GetTimeSeconds(World);

	//Landscape height modulation
	float LandscapeModulation = 1.f;
	if (bEnableLandscapeModulation && IsValid(Landscape))
	{
		const FVector LandLoc = Landscape->GetActorLocation();
		const FVector2D LandXY = FVector2D(LandLoc.X, LandLoc.Y);
		const FVector2D LocXY = FVector2D(location.X, location.Y);
		const FVector LandScale = Landscape->GetActorScale3D();
		const FVector2D ScaleXY = FVector2D(LandScale.X * HeightmapWidth, LandScale.Y * HeightmapHeight);

		if (LocXY > LandXY && LocXY < LandXY + ScaleXY) //optimization: don't calculate modulation if outside of landscape bounds
		{
			FVector2D UV = LocXY - (LandXY + ScaleXY / 2.f);
			UV = UV / ScaleXY + 0.5f;

			float height = GetHeightmapPixel(UV.X, UV.Y).R - 0.5f;
			height = height * 512 * LandScale.Z + LandLoc.Z;

			LandscapeModulation = height - RootComponent->GetComponentLocation().Z - ModulationStartHeight;
			LandscapeModulation /= FMath::Abs(ModulationStartHeight - ModulationMaxHeight);
			LandscapeModulation = 1 - FMath::Clamp(LandscapeModulation, 0.f, 1.f);
			LandscapeModulation = FMath::Pow(LandscapeModulation, ModulationPower);
		}
	}

	// Calculate the Gerstner Wave Sets
	return CalculateGerstnerWaveSetHeight(location, time * GlobalWaveSpeed) * LandscapeModulation + RootComponent->GetComponentLocation().Z;
}

void AOceanManager::LoadLandscapeHeightmap(UTexture2D* Tex2D)
{
	if (!Tex2D)
	{
		return;
	}

	Tex2D->SRGB = true;
	//Tex2D->Filter = TF_Trilinear;// TF_Bilinear;
	Tex2D->CompressionSettings = TC_VectorDisplacementmap;
	Tex2D->UpdateResource();

	FTexture2DMipMap* MyMipMap = &Tex2D->PlatformData->Mips[0];
	HeightmapWidth = MyMipMap->SizeX;
	HeightmapHeight = MyMipMap->SizeY;

	HeightmapPixels.Empty();

	FColor* FormatedImageData = static_cast<FColor*>(Tex2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));

// 	HeightmapPixels.SetNum(HeightmapWidth * HeightmapHeight);
// 	uint8* ArrayData = (uint8 *)HeightmapPixels.GetData();
// 	FMemory::Memcpy(ArrayData, FormatedImageData, GPixelFormats[Tex2D->GetPixelFormat()].BlockBytes * HeightmapWidth * HeightmapHeight);

	for (int i = 0; i < HeightmapWidth * HeightmapHeight; i++)
	{
		HeightmapPixels.Add(FLinearColor(FormatedImageData[i]));
	}
	Tex2D->PlatformData->Mips[0].BulkData.Unlock();

// 	UE_LOG(LogTemp, Warning, TEXT("num = %d"), HeightmapPixels.Num());
// 	UE_LOG(LogTemp, Warning, TEXT("numx = %f"), (float)HeightmapPixels[0].R);
}

FLinearColor AOceanManager::GetHeightmapPixel(float U, float V) const
{
	if (HeightmapPixels.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Landscape heightmap data is not loaded! Pixel array is empty."));
		return FLinearColor::Black;
	}
	
	const int32 Width = HeightmapWidth;
	const int32 Height = HeightmapHeight;

	check(Width > 0 && Height > 0 && HeightmapPixels.Num() > 0);

	const float NormalizedU = U >= 0 ? FMath::Fractional(U) : 1.0 + FMath::Fractional(U);
	const float NormalizedV = V >= 0 ? FMath::Fractional(V) : 1.0 + FMath::Fractional(V);

	const int PixelX = NormalizedU * (Width - 1) + 1;
	const int PixelY = NormalizedV * (Height - 1) + 1;

	return FLinearColor(HeightmapPixels[(PixelY - 1) * Width + PixelX - 1]);
}

FVector AOceanManager::GetWaveHeightValue(const FVector& location, const UWorld* World, bool HeightOnly, bool TwoIterations)
{
	const float SeaLevel = RootComponent->GetComponentLocation().Z;

	//optimization: skip gerstner calculations if gerstner waves are disabled or test point is far below/above from sea level
	if (!EnableGerstnerWaves || location.Z - DistanceCheck > SeaLevel || location.Z + DistanceCheck < SeaLevel)
		return FVector(0.f, 0.f, SeaLevel);

	const float time = GetTimeSeconds(World);

	//Landscape height modulation
	float LandscapeModulation = 1.f;
	if (bEnableLandscapeModulation && IsValid(Landscape))
	{
		const FVector LandLoc = Landscape->GetActorLocation();
		const FVector2D LandXY = FVector2D(LandLoc.X, LandLoc.Y);
		const FVector2D LocXY = FVector2D(location.X, location.Y);
		const FVector LandScale = Landscape->GetActorScale3D();
		const FVector2D ScaleXY = FVector2D(LandScale.X * HeightmapWidth, LandScale.Y * HeightmapHeight);

		if (LocXY > LandXY && LocXY < LandXY + ScaleXY) //optimization: don't calculate modulation if outside of landscape bounds
		{
			FVector2D UV = LocXY - (LandXY + ScaleXY / 2.f);
			UV = UV / ScaleXY + 0.5f;

			float height = GetHeightmapPixel(UV.X, UV.Y).R - 0.5f;
			height = height * 512 * LandScale.Z + LandLoc.Z;

			LandscapeModulation = height - SeaLevel - ModulationStartHeight;
			LandscapeModulation /= FMath::Abs(ModulationStartHeight - ModulationMaxHeight);
			LandscapeModulation = 1 - FMath::Clamp(LandscapeModulation, 0.f, 1.f);
			LandscapeModulation = FMath::Pow(LandscapeModulation, ModulationPower);

			//DrawDebugPoint(World, FVector(location.X, location.Y, height), 15.f, FColor::Blue);

// 			//Trace method (too slow)
// 			FHitResult hit(ForceInit);
// 			FVector rayStart = location + FVector(0, 0, 2000);
// 			FVector rayEnd = location - FVector(0, 0, 2000);
// 			FCollisionQueryParams params = FCollisionQueryParams(FName(TEXT("trace")), true, this);
// 			if (Landscape->ActorLineTraceSingle(hit, rayStart, rayEnd, ECollisionChannel::ECC_Visibility, params))
// 			{
// 				LandscapeModulation = hit.Location.Z - RootComponent->GetComponentLocation().Z - ModulationStartHeight;
// 				LandscapeModulation /= FMath::Abs(ModulationStartHeight - ModulationMaxHeight);
// 				LandscapeModulation = 1 - FMath::Clamp(LandscapeModulation, 0.f, 1.f);
// 				LandscapeModulation = FMath::Pow(LandscapeModulation, ModulationPower);
// 				
// 				DrawDebugPoint(World, hit.Location, 15.f, FColor::Blue);
// 			}

		}
	}

	// Calculate the Gerstner Wave Sets
	if (TwoIterations)
	{
		const FVector xy = CalculateGerstnerWaveSetVector(location, time * GlobalWaveSpeed, true, false);
		const float z = CalculateGerstnerWaveSetVector(location - xy, time * GlobalWaveSpeed, false, true).Z;
		return FVector(xy.X * LandscapeModulation, xy.Y * LandscapeModulation, z * LandscapeModulation + SeaLevel);
	}
	return CalculateGerstnerWaveSetVector(location, time * GlobalWaveSpeed, !HeightOnly, true) * LandscapeModulation + FVector(0, 0, SeaLevel);
}

float AOceanManager::CalculateGerstnerWaveSetHeight(const FVector& position, float time) const
{
	return CalculateGerstnerWaveSetVector(position, time, false, true).Z;

//	float sum = 0.f;
// 	// Calculate the Gerstner Waves
// 	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave01.Rotation, global.Length * ws.Wave01.Length,
// 		global.Amplitude * ws.Wave01.Amplitude, global.Steepness * ws.Wave01.Steepness, direction, position, time, WaveParameterCache[0]);
// 	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave02.Rotation, global.Length * ws.Wave02.Length,
// 		global.Amplitude * ws.Wave02.Amplitude, global.Steepness * ws.Wave02.Steepness, direction, position, time, WaveParameterCache[1]);
// 	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave03.Rotation, global.Length * ws.Wave03.Length,
// 		global.Amplitude * ws.Wave03.Amplitude, global.Steepness * ws.Wave03.Steepness, direction, position, time, WaveParameterCache[2]);
// 	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave04.Rotation, global.Length * ws.Wave04.Length,
// 		global.Amplitude * ws.Wave04.Amplitude, global.Steepness * ws.Wave04.Steepness, direction, position, time, WaveParameterCache[3]);
// 	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave05.Rotation, global.Length * ws.Wave05.Length,
// 		global.Amplitude * ws.Wave05.Amplitude, global.Steepness * ws.Wave05.Steepness, direction, position, time, WaveParameterCache[4]);
// 	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave06.Rotation, global.Length * ws.Wave06.Length,
// 		global.Amplitude * ws.Wave06.Amplitude, global.Steepness * ws.Wave06.Steepness, direction, position, time, WaveParameterCache[5]);
// 	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave07.Rotation, global.Length * ws.Wave07.Length,
// 		global.Amplitude * ws.Wave07.Amplitude, global.Steepness * ws.Wave07.Steepness, direction, position, time, WaveParameterCache[6]);
// 	sum += CalculateGerstnerWaveHeight(global.Rotation + ws.Wave08.Rotation, global.Length * ws.Wave08.Length,
// 		global.Amplitude * ws.Wave08.Amplitude, global.Steepness * ws.Wave08.Steepness, direction, position, time, WaveParameterCache[7]);
// 
// 	return sum / 8.f;
}

FVector AOceanManager::CalculateGerstnerWaveSetVector(const FVector& position, float time, bool CalculateXY, bool CalculateZ) const
{
	FVector sum = FVector(0, 0, 0);

	if (WaveClusters.Num() <= 0)
		return sum;

	for (int i = 0; i < WaveClusters.Num(); i++)
	{
		FWaveSetParameters offsets = FWaveSetParameters();
		if (WaveSetOffsetsOverride.IsValidIndex(i))
		{
			offsets = WaveSetOffsetsOverride[i];
		}

		sum += CalculateGerstnerWaveVector(WaveClusters[i].Rotation + offsets.Wave01.Rotation, WaveClusters[i].Length * offsets.Wave01.Length,
			GlobalWaveAmplitude * WaveClusters[i].Amplitude * offsets.Wave01.Amplitude, WaveClusters[i].Steepness * offsets.Wave01.Steepness, GlobalWaveDirection,
			position, WaveClusters[i].TimeScale * offsets.Wave01.TimeScale * time, WaveParameterCache[0], CalculateXY, CalculateZ);

		sum += CalculateGerstnerWaveVector(WaveClusters[i].Rotation + offsets.Wave02.Rotation, WaveClusters[i].Length * offsets.Wave02.Length,
			GlobalWaveAmplitude * WaveClusters[i].Amplitude * offsets.Wave02.Amplitude, WaveClusters[i].Steepness * offsets.Wave02.Steepness, GlobalWaveDirection,
			position, WaveClusters[i].TimeScale * offsets.Wave02.TimeScale * time, WaveParameterCache[1], CalculateXY, CalculateZ);

		sum += CalculateGerstnerWaveVector(WaveClusters[i].Rotation + offsets.Wave03.Rotation, WaveClusters[i].Length * offsets.Wave03.Length,
			GlobalWaveAmplitude * WaveClusters[i].Amplitude * offsets.Wave03.Amplitude, WaveClusters[i].Steepness * offsets.Wave03.Steepness, GlobalWaveDirection,
			position, WaveClusters[i].TimeScale * offsets.Wave03.TimeScale * time, WaveParameterCache[2], CalculateXY, CalculateZ);

		sum += CalculateGerstnerWaveVector(WaveClusters[i].Rotation + offsets.Wave04.Rotation, WaveClusters[i].Length * offsets.Wave04.Length,
			GlobalWaveAmplitude * WaveClusters[i].Amplitude * offsets.Wave04.Amplitude, WaveClusters[i].Steepness * offsets.Wave04.Steepness, GlobalWaveDirection,
			position, WaveClusters[i].TimeScale * offsets.Wave04.TimeScale * time, WaveParameterCache[3], CalculateXY, CalculateZ);

		sum += CalculateGerstnerWaveVector(WaveClusters[i].Rotation + offsets.Wave05.Rotation, WaveClusters[i].Length * offsets.Wave05.Length,
			GlobalWaveAmplitude * WaveClusters[i].Amplitude * offsets.Wave05.Amplitude, WaveClusters[i].Steepness * offsets.Wave05.Steepness, GlobalWaveDirection,
			position, WaveClusters[i].TimeScale * offsets.Wave05.TimeScale * time, WaveParameterCache[4], CalculateXY, CalculateZ);

		sum += CalculateGerstnerWaveVector(WaveClusters[i].Rotation + offsets.Wave06.Rotation, WaveClusters[i].Length * offsets.Wave06.Length,
			GlobalWaveAmplitude * WaveClusters[i].Amplitude * offsets.Wave06.Amplitude, WaveClusters[i].Steepness * offsets.Wave06.Steepness, GlobalWaveDirection,
			position, WaveClusters[i].TimeScale * offsets.Wave06.TimeScale * time, WaveParameterCache[5], CalculateXY, CalculateZ);

		sum += CalculateGerstnerWaveVector(WaveClusters[i].Rotation + offsets.Wave07.Rotation, WaveClusters[i].Length * offsets.Wave07.Length,
			GlobalWaveAmplitude * WaveClusters[i].Amplitude * offsets.Wave07.Amplitude, WaveClusters[i].Steepness * offsets.Wave07.Steepness, GlobalWaveDirection,
			position, WaveClusters[i].TimeScale * offsets.Wave07.TimeScale * time, WaveParameterCache[6], CalculateXY, CalculateZ);

		sum += CalculateGerstnerWaveVector(WaveClusters[i].Rotation + offsets.Wave08.Rotation, WaveClusters[i].Length * offsets.Wave08.Length,
			GlobalWaveAmplitude * WaveClusters[i].Amplitude * offsets.Wave08.Amplitude, WaveClusters[i].Steepness * offsets.Wave08.Steepness, GlobalWaveDirection,
			position, WaveClusters[i].TimeScale * offsets.Wave08.TimeScale * time, WaveParameterCache[7], CalculateXY, CalculateZ);
	}

// 	// Calculate the Gerstner Waves
// 	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave01.Rotation, global.Length * ws.Wave01.Length,
// 		global.Amplitude * ws.Wave01.Amplitude, global.Steepness * ws.Wave01.Steepness, direction, position, time, WaveParameterCache[0], CalculateXY, CalculateZ);
// 	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave02.Rotation, global.Length * ws.Wave02.Length,
// 		global.Amplitude * ws.Wave02.Amplitude, global.Steepness * ws.Wave02.Steepness, direction, position, time, WaveParameterCache[1], CalculateXY, CalculateZ);
// 	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave03.Rotation, global.Length * ws.Wave03.Length,
// 		global.Amplitude * ws.Wave03.Amplitude, global.Steepness * ws.Wave03.Steepness, direction, position, time, WaveParameterCache[2], CalculateXY, CalculateZ);
// 	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave04.Rotation, global.Length * ws.Wave04.Length,
// 		global.Amplitude * ws.Wave04.Amplitude, global.Steepness * ws.Wave04.Steepness, direction, position, time, WaveParameterCache[3], CalculateXY, CalculateZ);
// 	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave05.Rotation, global.Length * ws.Wave05.Length,
// 		global.Amplitude * ws.Wave05.Amplitude, global.Steepness * ws.Wave05.Steepness, direction, position, time, WaveParameterCache[4], CalculateXY, CalculateZ);
// 	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave06.Rotation, global.Length * ws.Wave06.Length,
// 		global.Amplitude * ws.Wave06.Amplitude, global.Steepness * ws.Wave06.Steepness, direction, position, time, WaveParameterCache[5], CalculateXY, CalculateZ);
// 	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave07.Rotation, global.Length * ws.Wave07.Length,
// 		global.Amplitude * ws.Wave07.Amplitude, global.Steepness * ws.Wave07.Steepness, direction, position, time, WaveParameterCache[6], CalculateXY, CalculateZ);
// 	sum += CalculateGerstnerWaveVector(global.Rotation + ws.Wave08.Rotation, global.Length * ws.Wave08.Length,
// 		global.Amplitude * ws.Wave08.Amplitude, global.Steepness * ws.Wave08.Steepness, direction, position, time, WaveParameterCache[7], CalculateXY, CalculateZ);

	return sum / (WaveClusters.Num() * 8);
}

FVector AOceanManager::CalculateGerstnerWaveVector(float rotation, float waveLength, float amplitude, float steepness, const FVector2D& direction, const FVector& position, float time, FWaveCache& InWaveCache, bool CalculateXY, bool CalculateZ) const
{
	float frequency = (2 * PI) / waveLength;

	FVector dir;
	if (!InWaveCache.GetDir(rotation, direction, &dir))
	{
		dir = FVector(direction.X, direction.Y, 0);
		dir = dir.RotateAngleAxis(rotation * 360, FVector(0, 0, 1));
		dir = dir.GetSafeNormal();
		InWaveCache.SetDir(rotation, direction, dir);
	}

	float wavePhase = frequency * FVector::DotProduct(dir, position) + time;
	float c = 0, s = 0, QA = 0;

	//FMath::SinCos(&s, &c, wavePhase);

	if (CalculateXY)
	{
		c = FMath::Cos(wavePhase);
		QA = steepness * amplitude;
	}

	if (CalculateZ)
	{
		s = FMath::Sin(wavePhase);
	}

	return FVector(QA * dir.X * c, QA * dir.Y * c, amplitude * s);
}

bool FWaveCache::GetDir(float rotation, const FVector2D& inDirection, FVector* outDir)
{
	if (rotation == LastRotation && inDirection == LastDirection)
	{
		*outDir = MemorizedDir;
		return true;
	}
	return false;
}

void FWaveCache::SetDir(float rotation, const FVector2D& inDirection, const FVector& inDir)
{
	LastDirection = inDirection;
	LastRotation = rotation;
	MemorizedDir = inDir;
}

