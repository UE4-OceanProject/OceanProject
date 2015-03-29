// Fill out your copyright notice in the Description page of Project Settings.

#include "OceanPluginPrivatePCH.h"
#include "Terrain/Noise.h"
#include "Terrain/SimplexNoise.h"


void UNoise::SetSimplexSeed(int32 seed)
{
	if (seed>0)
	SimplexNoise::CreatePermutationTable(seed);
}

float UNoise::MakeSimplexNoise2D(float x, float y, float scale)
{
	return SimplexNoise::RawNoise2D(x * scale, y * scale);
}

float UNoise::MakeSimplexNoise3D(float x, float y, float z, float scale)
{
	return SimplexNoise::RawNoise3D(x * scale, y * scale, z * scale);
}

float UNoise::MakeSimplexNoise4D(float x, float y, float z, float w, float scale)
{
	return SimplexNoise::RawNoise4D(x * scale, y * scale, z * scale, w * scale);
}

float UNoise::MakeOctaveSimplexNoise2D(const float octaves, const float persistence, const float scale, const float x, const float y)
{
	return SimplexNoise::OctaveNoise2D(octaves, persistence, scale, x, y);
}

float UNoise::MakeOctaveSimplexNoise3D(const float octaves, const float persistence, const float scale, const float x, const float y, const float z)
{
	return SimplexNoise::OctaveNoise3D(octaves, persistence, scale, x, y, z);
}

float UNoise::MakeOctaveSimplexNoise4D(const float octaves, const float persistence, const float scale, const float x, const float y, const float z, const float w)
{
	return SimplexNoise::OctaveNoise4D(octaves, persistence, scale, x, y, z, w);
}