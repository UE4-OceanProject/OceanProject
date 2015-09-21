/*=================================================
* FileName: BuoyantMeshTriangle.h
* 
* Created by: quantumv
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/09/21
*
* Last Edited on: 2015/09/21
* Last Edited by: quantumv
* 
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#pragma once

#include "OceanPluginPrivatePCH.h"
#include "BuoyantMeshVertex.h"
#include "BuoyantMeshTriangle.generated.h"

//For the UE4 Profiler
DECLARE_STATS_GROUP(TEXT("BuoyantMeshTriangle"), STATGROUP_BuoyantMeshTriangle, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("CutTriangle"), STAT_CutTriangle, STATGROUP_BuoyantMeshTriangle);
DECLARE_CYCLE_STAT(TEXT("GetTriangleArea"), STAT_GetTriangleArea, STATGROUP_BuoyantMeshTriangle);
DECLARE_CYCLE_STAT(TEXT("CreateTriangle"), STAT_CreateTriangle, STATGROUP_BuoyantMeshTriangle);
DECLARE_CYCLE_STAT(TEXT("GetForces"), STAT_GetForces, STATGROUP_BuoyantMeshTriangle);

/*
This class calculates the buoyancy forces on a triangle.
Of the triangle, only the submerged part is taken into account.

The easiest way to use this class is to call the static overload of "GetHydrostaticForces".

The algorithm is described in "Water interaction model for boats in video
games" by Jacques Kerner.
http://gamasutra.com/view/news/237528/Water_interaction_model_for_boats_in_video_games.php

*/

USTRUCT()
struct FBuoyantMeshTriangle
{

	GENERATED_BODY()
public:


	// Create a triangle from the supplied vertices. The vertices need to be in clockwise
	// order.
	static FBuoyantMeshTriangle FromVertices(const FBuoyantMeshVertex A, const FBuoyantMeshVertex B,
		const FBuoyantMeshVertex C);

	// Creates a triangle from the ordered vertices and the normal.
	// H is the highest vertex above water, followed by M then by L.
	FBuoyantMeshTriangle(const FBuoyantMeshVertex H, const FBuoyantMeshVertex M, const FBuoyantMeshVertex L,
		const FVector Normal);

	FBuoyantMeshTriangle();

	struct FForceApplicationParameters
	{
		FVector Force;
		FVector ApplicationPoint;
	};

	// Calculates the hydrostatic forces on the submerged part of triangle.
	// Returns a list of force vectors and their application points.
	TArray<FForceApplicationParameters> GetHydrostaticForces(const TArray<FVector>& SubmergedTriangles,
		float const WaterDensity, float const GravityMagnitude,
		TFunction<float(FVector Position)> const GetHeight) const;

	// Calculates the hydrostatic forces on the submerged part of triangle.
	// Returns a list of force vectors and their application points.
	static TArray<FForceApplicationParameters> GetHydrostaticForces(
		const FVector& A, const FVector& B, const FVector& C, const float WaterDensity,
		const float GravityMagnitude, const TFunction<float(FVector Position)> GetHeight, UWorld* World = nullptr, const bool bDrawDebugWaterline = false);

	// The triangle normal.
	FVector Normal;

	// Highest vertex above water.
	FBuoyantMeshVertex H;
	// Middle vertex above water.
	FBuoyantMeshVertex M;
	// Lowest vertex above water.
	FBuoyantMeshVertex L;


	// Calculates the submerged part of the triangle. 
	// The triangle is cut into smaller triangles if necessary.
	// Returns a list of triangle vertices, 3 by 3.
	TArray<FVector> GetSubmergedPortion(const UWorld* World, bool bDrawWaterline) const;

	// Find the cutting point on a triangle edge, at the determined distance from the start vertex.
	static FVector FindCutOnEdge(const FBuoyantMeshVertex& Start, const FBuoyantMeshVertex& End,
		float const CutDistance);
	// Calculate the area of a triangle
	static float GetTriangleArea(const FVector& Vertex1, const FVector& Vertex2, const FVector& Vertex3);
};
