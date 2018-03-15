/*=================================================
* FileName: BuoyantMeshTriangle.cpp
*
* Created by: quantumv
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/09/22
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


struct FBuoyantMeshVertex;

// Represents a submerged part of a BuoyantMeshTriangle.
struct FBuoyantMeshSubtriangle
{
	const FVector A;
	const FVector B;
	const FVector C;

	// Calculate the barycenter of the triangle.
	FVector GetCenter() const;

	// Calculate the area of the triangle.
	float GetArea() const;

	// Calculates the hydrostatic forces on the submerged part of triangle.
	// Returns a force vector and its application point.
	static FVector GetHydrostaticForce(const float WaterDensity,
	                                   const float GravityMagnitude,
	                                   const FBuoyantMeshVertex& Center,
	                                   const FVector& TriangleNormal,
	                                   const float TriangleArea);
	// Calculates the hydrodynamic forces on the submerged part of triangle.
	// Returns a force vector and its application point.
	static FVector GetHydrodynamicForce(const float WaterDensity,
	                                    const FVector& TriangleCenter,
	                                    const FVector& TriangleCenterVelocity,
	                                    const FVector& TriangleNormal,
	                                    float const TriangleArea);

	FBuoyantMeshSubtriangle(const FVector& A, const FVector& B, const FVector& C);

   private:
	// Calculate the area of the triangle by using Heron's formula
	static float GetTriangleAreaHeron(const FVector& Vertex1, const FVector& Vertex2, const FVector& Vertex3);
};
