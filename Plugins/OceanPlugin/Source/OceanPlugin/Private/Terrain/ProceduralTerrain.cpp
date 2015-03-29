// Fill out your copyright notice in the Description page of Project Settings.

#include "OceanPluginPrivatePCH.h"
#include "Terrain/ProceduralTerrain.h"


AProceduralTerrain::AProceduralTerrain(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create Root Component
	SceneRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "SceneRoot");
	RootComponent = SceneRoot;
	
	PrimaryActorTick.bCanEverTick = true;
	gMaterial = NULL;


}

bool AProceduralTerrain::GenerateFromOrigin(int32 X, int32 Y, int32 Z, int32 Size)
{
	int32 StartX = X - Size;
	int32 StartY = Y - Size;
	int32 EndX = X + Size;
	int32 EndY = Y + Size;

	// Clear X Axis
	for (int32 tY = StartY; tY <= EndY; ++tY)
	{
		DestroyChunk(StartX - 2, tY, Z);
		DestroyChunk(EndX + 2, tY, Z);
	}

	// Clear Y Axis
	for (int32 tX = StartX; tX <= EndX; ++tX)
	{
		DestroyChunk(tX, StartY - 2, Z);
		DestroyChunk(tX, EndY + 2, Z);
	}


	for (int32 tX = StartX; tX < EndX; ++tX)
	{
		for (int32 tY = StartY; tY < EndY; ++tY)
		{
			CreateChunk(tX, tY, Z);
		}
	}
	return true;
}


bool AProceduralTerrain::ToggleCollision(int32 X, int32 Y, int32 Z, bool collide)
{
	int32 ComponentNum = TerrainMeshComponents.Num();
	for (int i = 0; i < ComponentNum; ++i)
	{
		if (
			TerrainMeshComponents[i]->WorldPosition.X == X
			&&
			TerrainMeshComponents[i]->WorldPosition.Y == Y
			&&
			TerrainMeshComponents[i]->WorldPosition.Z == Z
			)
		{
			if (collide)
			{
				TerrainMeshComponents[i]->UpdateCollision();
				return true;
			}
		}
		TerrainMeshComponents[i]->RemoveCollision();

	}
	return false;
}

bool AProceduralTerrain::CreateChunk(int32 X, int32 Y, int32 Z)
{
	
	int32 ComponentNum = TerrainMeshComponents.Num();
	for (int i = 0; i < ComponentNum; ++i)
	{
		if (
			TerrainMeshComponents[i]->WorldPosition.X == X
			&&
			TerrainMeshComponents[i]->WorldPosition.Y == Y
			&&
			TerrainMeshComponents[i]->WorldPosition.Z == Z
			)
		{
			return false;
		}
	}
	UMarchingCubes *MarchingCubes = ConstructObject<UMarchingCubes>(UMarchingCubes::StaticClass());
	MarchingCubes->SetSurfaceCrossOverValue(SurfaceCrossOverValue);

	UTerrainMeshComponent *MeshComponent = CreateTerrainComponent();
	MeshComponent->WorldPosition.X = X;
	MeshComponent->WorldPosition.Y = Y;
	MeshComponent->WorldPosition.Z = Z;
	FTerrainGenerationWorker *WorkerThread = new FTerrainGenerationWorker(MarchingCubes, MeshComponent, X, Y, Z);
	WorkerThread->VerticalSmoothing = VerticalSmoothness;
	WorkerThread->Scale = Scale;
	WorkerThread->Start();
	
	WorkerThreads.Add(WorkerThread);
	TerrainMeshComponents.Add(MeshComponent);
	return true;
}

bool AProceduralTerrain::DestroyChunk(int32 X, int32 Y, int32 Z)
{
	int32 ComponentNum = TerrainMeshComponents.Num();
	for (int i = 0; i < ComponentNum; ++i)
	{
		if (
			TerrainMeshComponents[i]->WorldPosition.X == X
			&&
			TerrainMeshComponents[i]->WorldPosition.Y == Y
			&&
			TerrainMeshComponents[i]->WorldPosition.Z == Z
			)
		{
			TerrainMeshComponents[i]->UnregisterComponent();
			TerrainMeshComponents[i]->DestroyComponent();
			TerrainMeshComponents.RemoveAt(i);
			return true;
		}

	}
	return false;
}
UTerrainMeshComponent * AProceduralTerrain::CreateTerrainComponent()
{
	FString ComponentName;
	int32 ID = TerrainMeshComponents.Num();
	ComponentName.Append(TEXT("TerrainMeshComponent"));
	ComponentName.AppendInt(ID);
	FName name;
	name.AppendString(ComponentName);
	
	UTerrainMeshComponent *MeshComponent = ConstructObject<UTerrainMeshComponent>(UTerrainMeshComponent::StaticClass(), this, name);
	MeshComponent->RegisterComponent();
	//MeshComponent->AttachParent = SceneRoot;
	if (!gMaterial)
		gMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
	MeshComponent->SetMaterial(0, gMaterial);

	return MeshComponent;
}

//float AProceduralTerrain::GeneratePoint(int32 X, int32 Y, int32 Z)
//{
//	return -1.0f;
//}

void AProceduralTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	
	int ComponentNum = WorkerThreads.Num();
	for (int i = 0; i < ComponentNum; ++i)
	{
		if (WorkerThreads[i]->IsFinished())
		{
			
			// Update the Mesh Component
			WorkerThreads[i]->MeshComponent->MarkRenderStateDirty();
			WorkerThreads[i]->MeshComponent->UpdateCollision();
			
			

			// Destroy the thread
			delete WorkerThreads[i];
			WorkerThreads.RemoveAt(i);
			return;
		}
	}

}
