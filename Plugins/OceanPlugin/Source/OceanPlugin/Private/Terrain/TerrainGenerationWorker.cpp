
#include "OceanPluginPrivatePCH.h"
#include "Terrain/TerrainGenerationWorker.h"
#include "Terrain/Noise.h"

FTerrainGenerationWorker::FTerrainGenerationWorker(UMarchingCubes *pMarchingCubes, UTerrainMeshComponent *pMeshComponent, int32 pXPos, int32 pYPos, int32 pZPos)
{
	MarchingCubes = pMarchingCubes;
	MeshComponent = pMeshComponent;

	XPos = pXPos * 15;
	YPos = pYPos * 15;
	ZPos = pZPos * 31;

	Width = 16;
	Length = 16;
	Height = 32;
	Scale = 1000.0f;
	VerticalScaling = 0.05f;
	VerticalSmoothing = 10.0f;
	bIsFinished = false;
}

bool FTerrainGenerationWorker::Start()
{
	if (FPlatformProcess::SupportsMultithreading())
	{
		const bool bAutoDeleteSelf = false;
		const bool bAutoDeleteRunnable = false;
		bIsFinished = false;
		Thread = FRunnableThread::Create(this, TEXT("FTerrainGenerationWorker"), bAutoDeleteSelf, bAutoDeleteRunnable, 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
		return true;
	}
	else{
		bIsFinished = true;
		return false;
	}
}

FTerrainGenerationWorker::~FTerrainGenerationWorker()
{
	Thread->WaitForCompletion();
	delete Thread;
	delete MarchingCubes;
	Thread = NULL;
}
 
//Run
uint32 FTerrainGenerationWorker::Run()
{
	//Initial wait before starting
	//FPlatformProcess::Sleep(0.03);
	MarchingCubes->CreateGrid(16, 16, 32, 1.0f);
	
	// Hills And shit
	for (int32 x = 0; x < Width; ++x)
	{
		for (int32 y = 0; y < Length; ++y)
		{
			float zer = 0.0f;

			

			// Simplex Noise Height map
			float Density = UNoise::MakeSimplexNoise2D(XPos + x, YPos + y, VerticalScaling);

			//Density -= FMath::Sin(((float)y) * VerticalScaling);
			for (int32 z = 16; z <= Height; ++z)
			{
				float tmp = Density + ((float)zer / Height);
				MarchingCubes->SetVoxel(x, y, z, tmp);
				zer += VerticalSmoothing;
			}

		}
	}

	// Ground!
	for (int32 x = 0; x < Width; ++x)
	{
		for (int32 y = 16; y < Length; ++y)
		{
			MarchingCubes->SetVoxel(x, y, 0, -1.0f);
		}

	}

	// Caves
	for (int32 x = 0; x < Width; ++x)
	{
		for (int32 y = 0; y < Length; ++y)
		{
			for (int32 z = 0; z <= 16; ++z)
			{
				//float Density = UNoise::MakeOctaveNoise3D(CaveOctaves, CavePersistence, CaveScale, (float)x*SimplexScale, (float)y*SimplexScale, (float)z*SimplexScale);
				float Density = UNoise::MakeSimplexNoise2D(XPos + x + z, YPos + y, 0.02f) - UNoise::MakeSimplexNoise2D(XPos + x, YPos + y + z, 0.02f);
				Density += -0.5f;
				MarchingCubes->SetVoxel(x, y, z, Density);
			}
		}
	}
	
	// Polygonize!
	MarchingCubes->PolygonizeToTriangles(&MeshComponent->Vertices, &MeshComponent->Indices, &MeshComponent->Positions, Scale, 16, 16, 32, XPos, YPos, ZPos);

	bIsFinished = true;
	return 0;
}
 
void FTerrainGenerationWorker::EnsureCompletion()
{
	Thread->WaitForCompletion();
}
 
bool FTerrainGenerationWorker::IsFinished()
{
	return bIsFinished;
}
