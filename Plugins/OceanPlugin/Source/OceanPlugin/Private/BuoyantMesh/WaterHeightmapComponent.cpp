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

#include "BuoyantMesh/WaterHeightmapComponent.h"
#include "OceanShaderPlugin/Private/OceanShaderManager.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"


using FTrianglePlane = UWaterHeightmapComponent::FTrianglePlane;
using FIntVector2D = UWaterHeightmapComponent::FIntVector2D;

UWaterHeightmapComponent::UWaterHeightmapComponent()
{
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	UActorComponent::SetComponentTickEnabled(true);
}

// Called every frame
void UWaterHeightmapComponent::TickComponent(float DeltaTime,
                                             ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bGridSizeNeedsUpdate = true;

	if (bDrawHeightmap)
	{
		DrawHeightmap();
	}
}

void UWaterHeightmapComponent::EnsureComponentIsInitialized()
{
	if (!bHasInitialized)
	{
		Initialize();
		bHasInitialized = true;
	}
}

void UWaterHeightmapComponent::EnsureUpToDateGridSize()
{
	if (bGridSizeNeedsUpdate)
	{
		UpdateGridSize();
		bGridSizeNeedsUpdate = false;
	}
}

void UWaterHeightmapComponent::UpdateGridSize()
{
	EnsureComponentIsInitialized();

	const auto Owner = GetOwner();
	check(Owner);
	FVector BoxCenter;
	FVector BoxExtent;
	Owner->GetActorBounds(bOnlyCollidingComponents, /*out*/ BoxCenter, /*out*/ BoxExtent);
	GridSizeInUU = FVector2D{BoxExtent} * 2.f * GridSizeMultiplier;
	GridCenter = FVector2D{BoxCenter};

	LowerLeftGridCorner = FVector2D{BoxCenter - BoxExtent};

	GridSizeInCells = FIntVector2D(FMath::Max(1, FMath::RoundToInt(GridSizeInUU.X / DesiredCellSize)),
	                               FMath::Max(1, FMath::RoundToInt(GridSizeInUU.Y / DesiredCellSize)));

	CellSize = FVector2D{GridSizeInUU.X / GridSizeInCells.X, GridSizeInUU.Y / GridSizeInCells.Y};

	ResetGridData();
}

void UWaterHeightmapComponent::ResetGridData()
{
	const auto GridVertexCount = (GridSizeInCells.X + 1) * (GridSizeInCells.Y + 1);

	VertexHeights.Empty(GridVertexCount);
	VertexHeights.AddDefaulted(GridVertexCount);

	const auto CellCount = GridSizeInCells.X * GridSizeInCells.Y;

	LowerRightTrianglePlanes.Empty(CellCount);
	LowerRightTrianglePlanes.AddDefaulted(CellCount);

	UpperLeftTrianglePlanes.Empty(CellCount);
	UpperLeftTrianglePlanes.AddDefaulted(CellCount);
}

void UWaterHeightmapComponent::DrawHeightmap()
{
	EnsureUpToDateGridSize();

	// Query the height from every triangle in the heightmap so it can be drawn.

	const auto UpperRightGridCorner = LowerLeftGridCorner + GridSizeInUU;
	const auto SampleSize = 0.13f; // Arbitrary value that makes sure every triangle is visited.
	for (float X = LowerLeftGridCorner.X; X < UpperRightGridCorner.X; X += CellSize.X * SampleSize)
	{
		for (float Y = LowerLeftGridCorner.Y; Y < UpperRightGridCorner.Y; Y += CellSize.Y * SampleSize)
		{
			const auto Position = FVector{X, Y, 0.f};
			//  As a side effect, this will draw the triangle when bDrawHeightmap is true.
			const auto Height = GetHeightAtPosition(Position);
			DrawDebugPoint(GetWorld(), FVector{Position.X, Position.Y, Height}, 2.f, FColor::Green);
		}
	}
}

void UWaterHeightmapComponent::Initialize()
{
	for (auto Actor : TActorRange<AOceanShaderManager>(GetWorld()))
	{
		if (IsValid(Actor))
		{
			OceanManager = Actor;
		}
	}
	if (!OceanManager)
	{
		UE_LOG(LogTemp, Error, TEXT("WaterPatchComponent requires an OceanManager in the level."));
	}
}

FVector UWaterHeightmapComponent::GetSurfaceVertex(const FIntVector2D VertexCoordinates)
{
	const auto Position = FVector(LowerLeftGridCorner.X + VertexCoordinates.X * CellSize.X,
	                              LowerLeftGridCorner.Y + VertexCoordinates.Y * CellSize.Y,
	                              0.f);
	const auto VertexIndex = VertexCoordinates.X * (GridSizeInCells.Y + 1) + VertexCoordinates.Y;
	if (const auto HeightMaybe = VertexHeights[VertexIndex])
	{
		// Point is cached, use it
		return FVector{Position.X, Position.Y, HeightMaybe.GetValue()};
	}
	else
	{
		// Point isn't in cache, compute and store it
		const auto Height = OceanManager->GetWaveHeight(Position);
		VertexHeights[VertexIndex] = Height;
		return FVector{Position.X, Position.Y, Height};
	}
}

FTrianglePlane UWaterHeightmapComponent::GetLowerRightTrianglePlane(const FIntVector2D& CellCoordinates)
{
	const auto Vertex1 = FIntVector2D{CellCoordinates.X + 1, CellCoordinates.Y + 0};
	const auto Vertex2 = FIntVector2D{CellCoordinates.X + 0, CellCoordinates.Y + 0};
	const auto Vertex3 = FIntVector2D{CellCoordinates.X + 1, CellCoordinates.Y + 1};
	return GetTrianglePlane(LowerRightTrianglePlanes, CellCoordinates, Vertex1, Vertex2, Vertex3);
}

FTrianglePlane UWaterHeightmapComponent::GetUpperLeftTrianglePlane(const FIntVector2D& CellCoordinates)
{
	const auto Vertex1 = FIntVector2D{CellCoordinates.X + 0, CellCoordinates.Y + 1};
	const auto Vertex2 = FIntVector2D{CellCoordinates.X + 0, CellCoordinates.Y + 0};
	const auto Vertex3 = FIntVector2D{CellCoordinates.X + 1, CellCoordinates.Y + 1};
	return GetTrianglePlane(UpperLeftTrianglePlanes, CellCoordinates, Vertex1, Vertex2, Vertex3);
}

int32 UWaterHeightmapComponent::GetCellIndex(const FIntVector2D CellCoordinates) const
{
	return CellCoordinates.X * GridSizeInCells.Y + CellCoordinates.Y;
}

FTrianglePlane UWaterHeightmapComponent::GetTrianglePlane(TArray<TOptional<FTrianglePlane>> TrianglePlanes,
                                                          const FIntVector2D& CellCoordinates,
                                                          const FIntVector2D& Vertex1GridCoordinates,
                                                          const FIntVector2D& Vertex2GridCoordinates,
                                                          const FIntVector2D& Vertex3GridCoordinates)
{
	const auto CellIndex = GetCellIndex(CellCoordinates);

	if (const auto& TrianglePlaneMaybe = TrianglePlanes[CellIndex])
	{
		// The triangle plane is already cached.
		return TrianglePlaneMaybe.GetValue();
	}
	else
	{
		// Compute and cache the triangle plane.
		const auto SurfaceVertex1 = GetSurfaceVertex(Vertex1GridCoordinates);
		const auto SurfaceVertex2 = GetSurfaceVertex(Vertex2GridCoordinates);
		const auto SurfaceVertex3 = GetSurfaceVertex(Vertex3GridCoordinates);
		const auto TrianglePlane = FTrianglePlane::FromTriangle(SurfaceVertex1, SurfaceVertex2, SurfaceVertex3);

		TrianglePlanes[CellIndex] = TrianglePlane;

		if (bDrawUsedTriangles || bDrawHeightmap)
		{
			DrawDebugLine(GetWorld(), SurfaceVertex1, SurfaceVertex2, FColor::White);
			DrawDebugLine(GetWorld(), SurfaceVertex2, SurfaceVertex3, FColor::White);
			DrawDebugLine(GetWorld(), SurfaceVertex3, SurfaceVertex1, FColor::White);
		}

		return TrianglePlane;
	}
}

FIntVector2D UWaterHeightmapComponent::GetCellCoordinates(const FVector& WorldPosition) const
{
	const auto GridRow = FMath::FloorToInt((WorldPosition.X - LowerLeftGridCorner.X) / CellSize.X);
	const auto GridColumn = FMath::FloorToInt((WorldPosition.Y - LowerLeftGridCorner.Y) / CellSize.Y);
	return FIntVector2D{GridRow, GridColumn};
}

bool UWaterHeightmapComponent::IsCellInBounds(const FIntVector2D CellCoords) const
{
	return (CellCoords.X >= 0) && (CellCoords.X < GridSizeInCells.X) && (CellCoords.Y >= 0) &&
	       (CellCoords.Y < GridSizeInCells.Y);
}

float UWaterHeightmapComponent::GetHeightAtPosition(const FVector& Position)
{
	EnsureUpToDateGridSize();

	const auto CellCoordinates = GetCellCoordinates(Position);

	if (!IsCellInBounds(CellCoordinates))
	{
		return IsValid(OceanManager) ?  OceanManager->GetWaveHeight(Position) : 0.f;
	}

	const auto Position2D = FVector2D{Position};

	const auto CellCoordinatesFloat = FVector2D(CellCoordinates.X, CellCoordinates.Y);

	// Coordinates of the wanted position relative to the heightmap grid.
	FVector2D GridSpacePosition = Position2D - LowerLeftGridCorner;
	// In-cell coordinates
	FVector2D CellSpacePosition = GridSpacePosition - CellCoordinatesFloat * CellSize;

	if (CellSpacePosition.X > CellSpacePosition.Y)
	{
		// Lower right triangle
		const auto TrianglePlane = GetLowerRightTrianglePlane(CellCoordinates);
		return TrianglePlane.GetHeightAtPosition(Position2D);
	}
	else
	{
		// Upper Left Triangle
		const auto TrianglePlane = GetUpperLeftTrianglePlane(CellCoordinates);
		return TrianglePlane.GetHeightAtPosition(Position2D);
	}
}

float FTrianglePlane::GetHeightAtPosition(const FVector2D& Position) const
{
	if (e4 != 0.f)
	{
		return (e1 * Position.Y + e2 * Position.X + e3) / e4;
	}
	else
	{
		return 0.f;
	}
}

FTrianglePlane FTrianglePlane::FromTriangle(const FVector& A, const FVector& B, const FVector& C)
{
	const auto e1 = ((B.X - A.X) * C.Z + (A.Z - B.Z) * C.X + A.X * B.Z - A.Z * B.X);
	const auto e2 = ((A.Y - B.Y) * C.Z + (B.Z - A.Z) * C.Y - A.Y * B.Z + A.Z * B.Y);
	const auto e3 = (A.X * B.Y - A.Y * B.X) * C.Z + (A.Z * B.X - A.X * B.Z) * C.Y + (A.Y * B.Z - A.Z * B.Y) * C.X;
	const auto e4 = (B.X - A.X) * C.Y + (A.Y - B.Y) * C.X + A.X * B.Y - A.Y * B.X;
	return FTrianglePlane(e1, e2, e3, e4);
};
