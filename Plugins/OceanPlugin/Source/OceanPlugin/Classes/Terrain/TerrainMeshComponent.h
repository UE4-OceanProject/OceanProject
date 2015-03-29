// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DynamicMeshBuilder.h"
#include "TerrainMeshComponent.generated.h"

UCLASS(editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class UTerrainMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_UCLASS_BODY()
	
public:
	FIntVector WorldPosition;

	/** Set the geometry to use on this triangle mesh */

	/** Description of collision */
	UPROPERTY(BlueprintReadOnly, Category = "Collision")
	class UBodySetup* ModelBodySetup;
	// Begin Interface_CollisionDataProvider Interface
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override{ return false; }
	// End Interface_CollisionDataProvider Interface
	// Begin UPrimitiveComponent interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	// End UPrimitiveComponent interface.
	// Begin UMeshComponent interface.
	virtual int32 GetNumMaterials() const override;
	// End UMeshComponent interface.

	UFUNCTION(BlueprintCallable, Category = "Components|TerrainMesh")
	void UpdateBodySetup();

	UFUNCTION(BlueprintCallable, Category = "Components|TerrainMesh")
		void UpdateCollision();

	UFUNCTION(BlueprintCallable, Category = "Components|TerrainMesh")
		void RemoveCollision();

	TArray<FVector> Positions;
	TArray<FDynamicMeshVertex> Vertices;
	TArray<int32> Indices;

private:	
	// Begin USceneComponent interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;
	// Begin USceneComponent interface.
	/** */
	friend class FTerrainMeshSceneProxy;
};