/*=================================================
* FileName: BuoyantMeshTriangle.h
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
#include "BuoyantMesh/BuoyantMeshVertex.h"
struct FBuoyantMeshSubtriangle;


/*
This class calculates the buoyancy forces on a triangle.
Of the triangle, only the submerged part is taken into account.

The algorithm is described in "Water interaction model for boats in video
games" by Jacques Kerner.
http://gamasutra.com/view/news/237528/Water_interaction_model_for_boats_in_video_games.php

*/

struct FBuoyantMeshTriangle
{
	// Given three vertices, create a triangle. The vertices need to be in clockwise order.
	static FBuoyantMeshTriangle FromClockwiseVertices(const FBuoyantMeshVertex& A,
	                                                  const FBuoyantMeshVertex& B,
	                                                  const FBuoyantMeshVertex& C);
	// The triangle normal.
	const FVector Normal;

	// Highest vertex above water.
	const FBuoyantMeshVertex H;
	// Middle vertex above water.
	const FBuoyantMeshVertex M;
	// Lowest vertex above water.
	const FBuoyantMeshVertex L;

	// Calculates the submerged part of the triangle.
	// The triangle is cut into smaller triangles if necessary.
	// Returns a list sub-triangles.
	TArray<FBuoyantMeshSubtriangle> GetSubmergedPortion(const UWorld* World = nullptr,
	                                                    bool bDrawWaterline = false) const;

   private:
	// Find the cutting point on a triangle edge, at the determined distance from the start vertex.
	static FVector FindCutOnEdge(const FBuoyantMeshVertex& Start,
	                             const FBuoyantMeshVertex& End,
	                             float const CutDistance);

	static void SortVerticesByHeight(const FBuoyantMeshVertex& InA,
	                                 const FBuoyantMeshVertex& InB,
	                                 const FBuoyantMeshVertex& InC,
	                                 const FBuoyantMeshVertex** OutH,
	                                 const FBuoyantMeshVertex** OutM,
	                                 const FBuoyantMeshVertex** OutL);
	template <typename T>
	static void SwapPointers(const T*& p, const T*& q);

	// Creates a triangle from the ordered vertices and the normal.
	// H is the highest vertex above water, followed by M then by L.
	FBuoyantMeshTriangle(const FBuoyantMeshVertex& H,
		const FBuoyantMeshVertex& M,
		const FBuoyantMeshVertex& L,
		const FVector& Normal);
};
