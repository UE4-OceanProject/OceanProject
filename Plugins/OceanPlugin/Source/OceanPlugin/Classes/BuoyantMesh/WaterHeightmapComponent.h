/*=================================================
* FileName: BuoyantMeshComponent.h
*
* Created by: quantumv
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/09/21
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

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WaterHeightmapComponent.generated.h"


class AOceanManager;
// Water heightmap centered on the owning actor.
// Only the heightmap vertices that are actually used trigger an ocean height calculation. Queries between vertices are
// interpolated. Vertex heights are cached within a tick.
UCLASS(editinlinenew, meta = (BlueprintSpawnableComponent))
class OCEANPLUGIN_API UWaterHeightmapComponent : public UActorComponent
{
	GENERATED_BODY()

   public:
	float GetHeightAtPosition(const FVector& Position);

	// Represents the plane defined by the three points in a triangle.
	// Reference: Step 2 in http://codespear.github.io/graphics/2013/09/21/terrain-surface/
	struct FTrianglePlane
	{
		const float e1;
		const float e2;
		const float e3;
		const float e4;

		FTrianglePlane(float e1, float e2, float e3, float e4) : e1{e1}, e2{e2}, e3{e3}, e4{e4} {};
		static FTrianglePlane FromTriangle(const FVector& A, const FVector& B, const FVector& C);
		// Get the height of a world position, using the triangle plane.
		float GetHeightAtPosition(const FVector2D& Position) const;
	};

	// Vector of two int32s.
	struct FIntVector2D
	{
		int32 X;
		int32 Y;
		FIntVector2D(int32 X, int32 Y) : X{X}, Y{Y} {};
	};

	// Desired size for a square cell in the water patch.
	// This value might be rounded by the algorithm to make sure no cells are cut.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Patch")
	float DesiredCellSize = 300.f;

	// Only use colliding components be used to determine the water patch size?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Patch")
	bool bOnlyCollidingComponents = false;

	// How much big should the patch be, relative to the parent actor. 1 is the same.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Patch")
	float GridSizeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawUsedTriangles = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawHeightmap = false;

	UWaterHeightmapComponent();

   protected:
	virtual void TickComponent(float DeltaTime,
	                           ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

   private:
	// Width and height of the grid in Unreal units.
	FVector2D GridSizeInUU = FVector2D::ZeroVector;
	FVector2D GridCenter = FVector2D::ZeroVector;
	FVector2D LowerLeftGridCorner = FVector2D::ZeroVector;
	// Number of rows and columns in the grid.
	FIntVector2D GridSizeInCells = FIntVector2D{0, 0};

	// Effecte width and height of a grid cell (can be different from DesiredCellSize).
	FVector2D CellSize = FVector2D::ZeroVector;

	FIntVector2D GetCellCoordinates(const FVector& WorldPosition) const;
	int32 GetCellIndex(const FIntVector2D CellCoordinates) const;
	bool IsCellInBounds(const FIntVector2D CellCoordinates) const;

	// Height of ocean surface at the the water patch heightmap vertices.
	// The vertices are stored sequencially starting from the bottom left corner.
	TArray<TOptional<float>> VertexHeights;
	// Array containing the plane of the lower right triangle in each cell.
	TArray<TOptional<FTrianglePlane>> LowerRightTrianglePlanes;
	// Array containing the plane of the upper left triangle in each cell.
	TArray<TOptional<FTrianglePlane>> UpperLeftTrianglePlanes;

	void ResetGridData();

	void EnsureComponentIsInitialized();
	void Initialize();
	bool bHasInitialized = false;


	void EnsureUpToDateGridSize();
	void UpdateGridSize();
	bool bGridSizeNeedsUpdate = true;

	FVector GetSurfaceVertex(const FIntVector2D VertexCoordinates);
	FTrianglePlane GetTrianglePlane(TArray<TOptional<FTrianglePlane>> TrianglePlanes,
	                                const FIntVector2D& CellCoordinates,
	                                const FIntVector2D& Vertex1GridCoordinates,
	                                const FIntVector2D& Vertex2GridCoordinates,
	                                const FIntVector2D& Vertex3GridCoordinates);
	FTrianglePlane GetLowerRightTrianglePlane(const FIntVector2D& CellCoordinates);
	FTrianglePlane GetUpperLeftTrianglePlane(const FIntVector2D& CellCoordinates);

	void DrawHeightmap();

	UPROPERTY()
	AOceanManager* OceanManager = nullptr;
};
