// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TerrainMeshComponent.h"
#include "MarchingCubes.h"
#include "TerrainGenerationWorker.h"
#include "ProceduralTerrain.generated.h"

/**
 * 
 */


UCLASS()
class AProceduralTerrain : public AActor
{
	GENERATED_BODY()
private:
	class USceneComponent* SceneRoot;

	TArray<FTerrainGenerationWorker *> WorkerThreads;
public:

	/*
	*	PROPERTIES
	*/
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Terrain Generation")
	float SurfaceCrossOverValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Terrain Generation")
	float VerticalSmoothness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Terrain Generation")
	float Scale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Terrain Generation")
	UMaterialInterface *gMaterial;

	AProceduralTerrain(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
	bool GenerateFromOrigin(int32 X, int32 Y, int32 Z, int32 Size);

	UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
	bool ToggleCollision(int32 X, int32 Y, int32 Z, bool collide);

	UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
	bool CreateChunk(int32 X, int32 Y, int32 Z);

	UFUNCTION(BlueprintCallable, Category = "Terrain Generation")
	bool DestroyChunk(int32 X, int32 Y, int32 Z);


	virtual void Tick(float DeltaTime) override;
	

	TArray<class UTerrainMeshComponent *> TerrainMeshComponents;
private:
	UTerrainMeshComponent *CreateTerrainComponent();
};
