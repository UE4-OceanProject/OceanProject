/*=================================================
* FileName: BuoyantMeshTriangle.cpp
*
* Created by: quantumv
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/09/22
*
* Last Edited on: 2018/01/30
* Last Edited by: SaschaElble
*
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#include "BuoyantMesh/BuoyantMeshSubtriangle.h"
#include "BuoyantMesh/BuoyantMeshVertex.h"

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


// Calculates hydrodynamic forces based on Game Programming Gems 8, Chapter 2.7
FVector FBuoyantMeshSubtriangle::GetHydrodynamicForce(const float WaterDensity,
                                                      const FVector& TriangleCenter,
                                                      const FVector& TriangleCenterVelocity,
                                                      const FVector& TriangleNormal,
                                                      float const TriangleArea)
{
	const auto LocalVelocity = -TriangleCenterVelocity;
	const auto VelocityNormal = LocalVelocity.GetSafeNormal();

	// Nondimensional pressure coefficent.
	// Implements equation 6 in the chapter.
	const auto Cp = TriangleNormal | VelocityNormal;

	// Triangle center speed.
	const auto V = LocalVelocity.Size();
	const auto rho = WaterDensity;

	// Dynamic pressure exerted on a segment of the surface.
	// Implements equation 7 in the chapter.
	const auto DynamicPressure = rho * Cp * V * V / 2.f;

	// Dynamic pressure exterted on the triangle.
	// Implements equation 2 in the chapter.
	const auto DynamicForce = TriangleArea * (DynamicPressure * TriangleNormal);

	// Correction of dymamic pressure based on the heuristic parameter Cp.
	// Implements equation 13 in the chapter.
	const auto DynamicForceCorrected = 2.2f * DynamicForce - VelocityNormal * 1.6f * (VelocityNormal | DynamicForce);
	return DynamicForceCorrected;
}

FVector FBuoyantMeshSubtriangle::GetHydrostaticForce(const float WaterDensity,
                                                     const float GravityMagnitude,
                                                     const FBuoyantMeshVertex& Center,
                                                     const FVector& TriangleNormal,
                                                     const float TriangleArea)
{
	const auto h = Center.Height;
	const auto g = GravityMagnitude;
	const auto n = TriangleNormal;
	const auto rho = WaterDensity;
	const auto A = TriangleArea;

	// The hydrostatic force on a submerged triangle.
	const auto F = rho * g * h * A * n;
	return F;
}

FBuoyantMeshSubtriangle::FBuoyantMeshSubtriangle(const FVector& A, const FVector& B, const FVector& C)
    : A{A}, B{B}, C{C}
{
}
