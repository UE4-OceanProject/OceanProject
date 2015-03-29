#pragma once
#include "Noise.generated.h"

/**
 * Utility class for Simplex/Perlin Noise Generation
 */

UCLASS()
class UNoise : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Utility|SimplexNoise")
	static void SetSimplexSeed(int32 seed);

	UFUNCTION(BlueprintCallable, Category = "Utility|SimplexNoise")
	static float MakeSimplexNoise2D(float x, float y, float scale);

	UFUNCTION(BlueprintCallable, Category = "Utility|SimplexNoise")
	static float MakeSimplexNoise3D(float x, float y, float z, float scale);

	UFUNCTION(BlueprintCallable, Category = "Utility|SimplexNoise")
	static float MakeSimplexNoise4D(float x, float y, float z, float w, float scale);

	UFUNCTION(BlueprintCallable, Category = "Utility|SimplexNoise")
	static float MakeOctaveSimplexNoise2D(const float octaves, const float persistence, const float scale, const float x, const float y);

	UFUNCTION(BlueprintCallable, Category = "Utility|SimplexNoise")
	static float MakeOctaveSimplexNoise3D(const float octaves, const float persistence, const float scale, const float x, const float y, const float z);

	UFUNCTION(BlueprintCallable, Category = "Utility|SimplexNoise")
	static float MakeOctaveSimplexNoise4D(const float octaves, const float persistence, const float scale, const float x, const float y, const float z, const float w);

};
