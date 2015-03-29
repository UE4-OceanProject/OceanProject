#pragma once

#include "MarchingCubes.h"
#include "TerrainMeshComponent.h"

class FTerrainGenerationWorker : public FRunnable
{	
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;
	bool bIsFinished;
public:
	int32 XPos;
	int32 YPos;
	int32 ZPos;

	int32 Width;
	int32 Length;
	int32 Height;

	float Scale;
	float VerticalScaling;
	float VerticalSmoothing;

	UMarchingCubes *MarchingCubes;
	

	UTerrainMeshComponent *MeshComponent;
	TArray<FDynamicMeshVertex> Vertices;
	TArray<int32> Indices;
	TArray<FVector> Positions;


	bool IsFinished() const
	{
		return false;
	}
	

	FTerrainGenerationWorker(UMarchingCubes *pMarchingCubes, UTerrainMeshComponent *pMeshComponent, int32 XPos, int32 YPos, int32 ZPos);
	virtual ~FTerrainGenerationWorker();
 
	bool Start();

	virtual uint32 Run();

 
	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();
 
 
	bool IsFinished();
 
};