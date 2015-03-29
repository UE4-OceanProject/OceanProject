// Fill out your copyright notice in the Description page of Project Settings.

#include "OceanPluginPrivatePCH.h"
#include "Terrain/MarchingCubes.h"
#include "Terrain/MarchingCubesLookupTables.h"


UMarchingCubes::UMarchingCubes()
{
	GridSize.X = 0;
	GridSize.Y = 0;
	GridSize.Z = 0;
	m_pVoxels = NULL;
}


void UMarchingCubes::CreateGrid(int32 SizeX, int32 SizeY, int32 SizeZ, float InitialIsoValue)
{
	// First drop the existing grid if it exists
	DestroyGrid();

	// Create new grid
	GridSize.X = SizeX;
	GridSize.Y = SizeY;
	GridSize.Z = SizeZ;

	// Allocate memory for the new grid
	m_pVoxels = new float**[SizeX];
	for (int32 X = 0; X<SizeX; ++X)
	{
		m_pVoxels[X] = new float*[SizeY];
		for (int32 Y = 0; Y < SizeY; ++Y)
		{
			m_pVoxels[X][Y] = new float[SizeZ];
			for (int32 Z = 0; Z < SizeZ; ++Z)
			{
				// Set default values
				m_pVoxels[X][Y][Z] = InitialIsoValue;
			}
		}
	}
}

void UMarchingCubes::ClearGrid(float fValue)
{
	for (int32 X = 0; X < GridSize.X; ++X)
	{
		for (int32 Y = 0; Y < GridSize.Y; ++Y)
		{
			for (int32 Z = 0; Z < GridSize.Z; ++Z)
			{
				m_pVoxels[X][Y][Z] = fValue;
			}
		}
	}
}

void UMarchingCubes::DestroyGrid()
{
	if (m_pVoxels != NULL)
	{
		for (int32 X = 0; X < GridSize.X; ++X)
		{
			if (m_pVoxels[X])
			{
				for (int32 Y = 0; Y < GridSize.Y; ++Y)
				{
					if (m_pVoxels[X][Y])
					{
						delete[] m_pVoxels[X][Y];
						m_pVoxels[X][Y] = NULL;
					}
				}
				delete[] m_pVoxels[X];
				m_pVoxels[X] = NULL;
			}
		}
		delete[] m_pVoxels;
		m_pVoxels = NULL;
	}
	m_pVoxels = NULL;
	GridSize.X = 0;
	GridSize.Y = 0;
	GridSize.Z = 0;
}


FIntVector UMarchingCubes::GetGridSize()
{
	return GridSize;
}

void UMarchingCubes::SetSurfaceCrossOverValue(float fValue)
{
	m_fSurfaceCrossValue = fValue;
}


float UMarchingCubes::GetSurfaceCrossOverValue()
{
	return m_fSurfaceCrossValue;
}

int UMarchingCubes::PolygonizeToTriangles(TArray<FDynamicMeshVertex> *Vertices, TArray<int32> *Indices, TArray<FVector> *Positions, float fScaling, int32 SizeX, int32 SizeY, int32 SizeZ, int32 PosX, int32 PosY, int32 PosZ)
{
	/*
	if (GridSize.X < SizeX || GridSize.Y < SizeY || GridSize.Z < SizeZ)
		return 0;
	*/
	int NumTriangles = 0;
	for (int32 x = 0; x < GridSize.X - 1; ++x)
	{

		for (int32 y = 0; y < GridSize.Y - 1; ++y)
		{

			for (int32 z = 0; z < GridSize.Z - 1; ++z)
			{

				// Get each points of a cube.
				float p0 = GetVoxel(x, y, z);
				float p1 = GetVoxel(x + 1, y, z);
				float p2 = GetVoxel(x, y + 1, z);
				float p3 = GetVoxel(x + 1, y + 1, z);
				float p4 = GetVoxel(x, y, z + 1);
				float p5 = GetVoxel(x + 1, y, z + 1);
				float p6 = GetVoxel(x, y + 1, z + 1);
				float p7 = GetVoxel(x + 1, y + 1, z + 1);

				/*
				Determine the index into the edge table which
				tells us which vertices are inside of the surface
				*/
				int crossBitMap = 0;

				if (p0 < m_fSurfaceCrossValue) crossBitMap |= 1;
				if (p1 < m_fSurfaceCrossValue) crossBitMap |= 2;

				if (p2 < m_fSurfaceCrossValue) crossBitMap |= 8;
				if (p3 < m_fSurfaceCrossValue) crossBitMap |= 4;

				if (p4 < m_fSurfaceCrossValue) crossBitMap |= 16;
				if (p5 < m_fSurfaceCrossValue) crossBitMap |= 32;

				if (p6 < m_fSurfaceCrossValue) crossBitMap |= 128;
				if (p7 < m_fSurfaceCrossValue) crossBitMap |= 64;


				/* Cube is entirely in/out of the surface */
				int edgeBits = edgeTable[crossBitMap];
				if (edgeBits == 0)
					continue;

				float interpolatedCrossingPoint = 0.0f;
				FVector interpolatedValues[12];

				if ((edgeBits & 1) > 0)
				{

					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p0) / (p1 - p0);
					interpolatedValues[0] = FMath::Lerp(FVector(PosX + x, PosY + y, PosZ + z), FVector(PosX + x + 1, PosY + y, PosZ + z), interpolatedCrossingPoint);
				}
				if ((edgeBits & 2) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p1) / (p3 - p1);
					interpolatedValues[1] = FMath::Lerp(FVector(PosX + x + 1, PosY + y, PosZ + z), FVector(PosX + x + 1, PosY + y + 1, PosZ + z), interpolatedCrossingPoint);
				}
				if ((edgeBits & 4) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p2) / (p3 - p2);
					interpolatedValues[2] = FMath::Lerp(FVector(PosX + x, PosY + y + 1, PosZ + z), FVector(PosX + x + 1, PosY + y + 1, PosZ + z), interpolatedCrossingPoint);
				}
				if ((edgeBits & 8) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p0) / (p2 - p0);
					interpolatedValues[3] = FMath::Lerp(FVector(PosX + x, PosY + y, PosZ + z), FVector(PosX + x, PosY + y + 1, PosZ + z), interpolatedCrossingPoint);
				}

				//Top four edges
				if ((edgeBits & 16) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p4) / (p5 - p4);
					interpolatedValues[4] = FMath::Lerp(FVector(PosX + x, PosY + y, PosZ + z + 1), FVector(PosX + x + 1, PosY + y, PosZ + z + 1), interpolatedCrossingPoint);
				}
				if ((edgeBits & 32) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p5) / (p7 - p5);
					interpolatedValues[5] = FMath::Lerp(FVector(PosX + x + 1, PosY + y, PosZ + z + 1), FVector(PosX + x + 1, PosY + y + 1, PosZ + z + 1), interpolatedCrossingPoint);
				}
				if ((edgeBits & 64) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p6) / (p7 - p6);
					interpolatedValues[6] = FMath::Lerp(FVector(PosX + x, PosY + y + 1, PosZ + z + 1), FVector(PosX + x + 1, PosY + y + 1, PosZ + z + 1), interpolatedCrossingPoint);
				}
				if ((edgeBits & 128) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p4) / (p6 - p4);
					interpolatedValues[7] = FMath::Lerp(FVector(PosX + x, PosY + y, PosZ + z + 1), FVector(PosX + x, PosY + y + 1, PosZ + z + 1), interpolatedCrossingPoint);
				}

				//Side four edges
				if ((edgeBits & 256) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p0) / (p4 - p0);
					interpolatedValues[8] = FMath::Lerp(FVector(PosX + x, PosY + y, PosZ + z), FVector(PosX + x, PosY + y, PosZ + z + 1), interpolatedCrossingPoint);
				}
				if ((edgeBits & 512) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p1) / (p5 - p1);
					interpolatedValues[9] = FMath::Lerp(FVector(PosX + x + 1, PosY + y, PosZ + z), FVector(PosX + x + 1, PosY + y, PosZ + z + 1), interpolatedCrossingPoint);
				}
				if ((edgeBits & 1024) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p3) / (p7 - p3);
					interpolatedValues[10] = FMath::Lerp(FVector(PosX + x + 1, PosY + y + 1, PosZ + z), FVector(PosX + x + 1, PosY + y + 1, PosZ + z + 1), interpolatedCrossingPoint);
				}
				if ((edgeBits & 2048) > 0)
				{
					interpolatedCrossingPoint = (m_fSurfaceCrossValue - p2) / (p6 - p2);
					interpolatedValues[11] = FMath::Lerp(FVector(PosX + x, PosY + y + 1, PosZ + z), FVector(PosX + x, PosY + y + 1, PosZ + z + 1), interpolatedCrossingPoint);
				}

				crossBitMap <<= 4;

				int triangleIndex = 0;
				while (triTable[crossBitMap + triangleIndex] != -1)
				{
					// For each triangle in the look up table, create a triangle and add it to the list.
					int index1 = triTable[crossBitMap + triangleIndex];
					int index2 = triTable[crossBitMap + triangleIndex + 1];
					int index3 = triTable[crossBitMap + triangleIndex + 2];

					FDynamicMeshVertex Vertex0;
					Vertex0.Position = (FVector(interpolatedValues[index1].X, interpolatedValues[index1].Y, interpolatedValues[index1].Z) * fScaling);
					FDynamicMeshVertex Vertex1;
					Vertex1.Position = (FVector(interpolatedValues[index2].X, interpolatedValues[index2].Y, interpolatedValues[index2].Z) * fScaling);
					FDynamicMeshVertex Vertex2;
					Vertex2.Position = (FVector(interpolatedValues[index3].X, interpolatedValues[index3].Y, interpolatedValues[index3].Z) * fScaling);
					
					// Calculate Tangents
					const FVector Edge01 = (Vertex1.Position - Vertex0.Position);
					const FVector Edge02 = (Vertex2.Position - Vertex0.Position);
					const FVector TangentX = Edge01.GetSafeNormal();
					const FVector TangentZ = (Edge02 ^ Edge01).GetSafeNormal();
					const FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

					Vertex0.TextureCoordinate.X = Vertex0.Position.X / 100.0f;
					Vertex0.TextureCoordinate.Y = Vertex0.Position.Y / 100.0f;

					Vertex1.TextureCoordinate.X = Vertex1.Position.X / 100.0f;
					Vertex1.TextureCoordinate.Y = Vertex1.Position.Y / 100.0f;

					Vertex2.TextureCoordinate.X = Vertex2.Position.X / 100.0f;
					Vertex2.TextureCoordinate.Y = Vertex2.Position.Y / 100.0f;

					// Fill Index buffer And Vertex buffer with the generated vertices.
					int32 VIndex0 = Positions->Find(Vertex0.Position);
					if (VIndex0 < 0)
					{
						Vertex0.SetTangents(TangentX, TangentY, TangentZ);
						VIndex0 = Positions->Add(Vertex0.Position);
						Indices->Add(VIndex0);
						Vertices->Add(Vertex0);
					}
					else{
						Indices->Add(VIndex0);
					}

					int32 VIndex1 = Positions->Find(Vertex1.Position);
					if (VIndex1 < 0)
					{
						Vertex1.SetTangents(TangentX, TangentY, TangentZ);
						VIndex1 = Positions->Add(Vertex1.Position);
						Indices->Add(VIndex1);
						Vertices->Add(Vertex1);
					}
					else{
						Indices->Add(VIndex1);
					}

					int32 VIndex2 = Positions->Find(Vertex2.Position);
					if (VIndex2 < 0)
					{
						Vertex2.SetTangents(TangentX, TangentY, TangentZ);
						VIndex2 = Positions->Add(Vertex2.Position);
						Indices->Add(VIndex2);
						Vertices->Add(Vertex2);
					}
					else{
						Indices->Add(VIndex2);
					}
					
					++NumTriangles;
					triangleIndex += 3;

				}



			}
		}
	}

	return NumTriangles;
}


float UMarchingCubes::GetVoxel(int32 X, int32 Y, int32 Z)
{
	if (!m_pVoxels)
		return 0.0f;

	if (X >= GridSize.X || X < 0)
		return 0.0f;

	if (Y >= GridSize.Y || Y < 0)
		return 0.0f;

	if (Z >= GridSize.Z || Z < 0)
		return 0.0f;

	if (m_pVoxels[X][Y][Z])
	{
		return m_pVoxels[X][Y][Z];
	}
	return 0.0f;
}


void UMarchingCubes::SetVoxel(int32 X, int32 Y, int32 Z, float IsoValue)
{
	if (!m_pVoxels)
		return;

	if (X >= GridSize.X || X < 0)
		return;

	if (Y >= GridSize.Y || Y < 0)
		return;

	if (Z >= GridSize.Z || Z < 0)
		return;

	if (m_pVoxels[X][Y][Z])
	{
		m_pVoxels[X][Y][Z] = IsoValue;
	}
}

void UMarchingCubes::BeginDestroy()
{
	Super::BeginDestroy();
	DestroyGrid();
}
