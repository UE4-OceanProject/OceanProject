/*=================================================
* FileName: BuoyantMeshTriangle.cpp
*
* Created by: quantumv
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/09/22
*
* Last Edited on: 2015/09/22
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

#include "OceanPluginPrivatePCH.h"
#include "BuoyantMesh/BuoyantMeshSubtriangle.h"

FVector FBuoyantMeshSubtriangle::GetCenter() const
{
	return (A + B + C) / 3.0f;
}

float FBuoyantMeshSubtriangle::GetArea() const
{
	return GetTriangleAreaHeron(A, B, C);
}

float FBuoyantMeshSubtriangle::GetTriangleAreaHeron(const FVector& Vertex1,
                                                    const FVector& Vertex2,
                                                    const FVector& Vertex3)
{
	const auto A = FVector::Dist(Vertex1, Vertex2);
	const auto B = FVector::Dist(Vertex2, Vertex3);
	const auto C = FVector::Dist(Vertex3, Vertex1);
	const auto S = (A + B + C) / 2.f;
	return FMath::Sqrt(S * (S - A) * (S - B) * (S - C));
}

FVector FBuoyantMeshSubtriangle::GetHydrostaticForce(float const WaterDensity,
                                                     float const GravityMagnitude,
                                                     const FBuoyantMeshVertex& Center,
                                                     const FVector& Normal) const
{
	const auto Area = GetArea();

	if (FMath::IsNearlyZero(Area))
	{
		return {};
	}

	const auto h = Center.Height;
	const auto g = GravityMagnitude;
	const auto n = Normal;
	const auto rho = WaterDensity;
	const auto A = Area;

	// The hydrostatic force on a submerged triangle
	const auto F = rho * g * h * n * A;
	return F;
}

FBuoyantMeshSubtriangle::FBuoyantMeshSubtriangle(const FVector& A,
                                                 const FVector& B,
                                                 const FVector& C)
    : A{A}, B{B}, C{C}
{
}
