#pragma once
#include "DynamicMeshBuilder.h"
#include "MarchingCubes.generated.h"


/**
 * Utility Class for extracting Iso Surfaces
 */

UCLASS()
class UMarchingCubes : public UObject
{
	GENERATED_BODY()

private:
	FIntVector GridSize;
	float ***m_pVoxels;
	float m_fSurfaceCrossValue;
public:

	UMarchingCubes();
	// Returns the number of triangles generated.
	int PolygonizeToTriangles(TArray<FDynamicMeshVertex> *Vertices, TArray<int32> *Indices, TArray<FVector> *Positions, float fScaling, int32 SizeX, int32 SizeY, int32 SizeZ, int32 PosX, int32 PosY, int32 PosZ);

	/*
	 * Blueprintable Functions
	 */
	UFUNCTION(BlueprintCallable, Category = "Utility|IsoSurface")
		void CreateGrid(int32 SizeX, int32 SizeY, int32 SizeZ, float InitialIsoValue = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Utility|IsoSurface")
	void ClearGrid(float fValue);

	UFUNCTION(BlueprintCallable, Category = "Utility|IsoSurface")
	void DestroyGrid();

	UFUNCTION(BlueprintCallable, Category = "Utility|IsoSurface")
	void SetSurfaceCrossOverValue(float fValue);

	UFUNCTION(BlueprintCallable, Category = "Utility|IsoSurface")
	float GetSurfaceCrossOverValue();
	
	
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Utility|IsoSurface")
	float GetVoxel(int32 X, int32 Y, int32 Z);

	UFUNCTION(BlueprintCallable, Category = "Utility|IsoSurface")
	void SetVoxel(int32 X, int32 Y, int32 Z, float IsoValue);


	UFUNCTION(BlueprintCallable, Category = "Utility|IsoSurface")
	FIntVector GetGridSize();

};

