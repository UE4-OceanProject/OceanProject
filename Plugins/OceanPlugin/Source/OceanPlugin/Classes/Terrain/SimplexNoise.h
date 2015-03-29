#pragma once

#include "OceanPluginPrivatePCH.h"

#ifndef H_SIMPLEXNOISE
#define H_SIMPLEXNOISE

namespace SimplexNoise
{
	// Simplex noise
	float OctaveNoise2D(const float octaves, const float persistence, const float scale, const float x, const float y);
	// Simplex noise
	float OctaveNoise3D(const float octaves, const float persistence, const float scale, const float x, const float y, const float z);
	// Simplex noise
	float OctaveNoise4D(const float octaves, const float persistence, const float scale, const float x, const float y, const float z, const float w);

	// Scaled Simplex noise
	float ScaledNoise2D(const float octaves, const float persistence, const float scale, const float loBound, const float hiBound, const float x, const float y);
	// Scaled Simplex noise
	float ScaledNoise3D(const float octaves, const float persistence, const float scale, const float loBound, const float hiBound, const float x, const float y, const float z);
	// Scaled Simplex noise
	float ScaledNoise4D(const float octaves, const float persistence, const float scale, const float loBound, const float hiBound, const float x, const float y, const float z, const float w);

	// Scaled Raw Simplex noise
	float ScaledRawNoise2D(const float loBound, const float hiBound, const float x, const float y);
	// Scaled Raw Simplex noise
	float ScaledRawNoise3D(const float loBound, const float hiBound, const float x, const float y, const float z);
	// Scaled Raw Simplex noise
	float ScaledRawNoise4D(const float loBound, const float hiBound, const float x, const float y, const float z, const float w);


	// Raw Simplex noise - a single noise value.
	float RawNoise2D(const float x, const float y);
	// Raw Simplex noise - a single noise value.
	float RawNoise3D(const float x, const float y, const float z);
	// Raw Simplex noise - a single noise value.
	float RawNoise4D(const float x, const float y, const float z, const float w);

	void CreatePermutationTable(int seed);

	int fastfloor(const float x);

	float dot(const int* g, const float x, const float y);
	float dot(const int* g, const float x, const float y, const float z);
	float dot(const int* g, const float x, const float y, const float z, const float w);
};



#endif
