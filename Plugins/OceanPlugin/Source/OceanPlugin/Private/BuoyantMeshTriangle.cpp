/*=================================================
* FileName: BuoyantMeshTriangle.cpp
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

#include "OceanPluginPrivatePCH.h"
#include "BuoyantMeshTriangle.h"
typedef FBuoyantMeshTriangle::FForceApplicationParameters ForceParams;

FBuoyantMeshTriangle FBuoyantMeshTriangle::FromVertices(const FBuoyantMeshVertex A,
	const FBuoyantMeshVertex B,
	const FBuoyantMeshVertex C)
{
	SCOPE_CYCLE_COUNTER(STAT_CreateTriangle);
	const auto TriangleNormal =
		FVector::CrossProduct(B.Position - A.Position, C.Position - A.Position).GetSafeNormal();

	// Sort the triangle vertices in order to determine which ones are L, M and H.
	TArray<FBuoyantMeshVertex, TInlineAllocator<3>> BuoyantMeshVertices;
	BuoyantMeshVertices.Emplace(A);
	BuoyantMeshVertices.Emplace(B);
	BuoyantMeshVertices.Emplace(C);

	BuoyantMeshVertices.Sort([](const FBuoyantMeshVertex A, const FBuoyantMeshVertex B)
	{
		return A.Height < B.Height;
	});

	const auto L = BuoyantMeshVertices[0];
	const auto M = BuoyantMeshVertices[1];
	const auto H = BuoyantMeshVertices[2];

	return{ H, M, L, TriangleNormal };
}

TArray<ForceParams> FBuoyantMeshTriangle::GetHydrostaticForces(
	const FVector& A, const FVector& B, const FVector& C, const float WaterDensity,
	const float GravityMagnitude, const TFunction<float(FVector Position)> GetHeight, UWorld* World,
	const bool bDrawDebugWaterline)
{
	const auto Triangle = FromVertices({ A, GetHeight(A) }, { B, GetHeight(B) }, { C, GetHeight(C) });
	return Triangle.GetHydrostaticForces(Triangle.GetSubmergedPortion(World, bDrawDebugWaterline),
		WaterDensity, GravityMagnitude, GetHeight);
}

FBuoyantMeshTriangle::FBuoyantMeshTriangle(const FBuoyantMeshVertex H, const FBuoyantMeshVertex M,
	const FBuoyantMeshVertex L, const FVector Normal)
	: H{ H }, M{ M }, L{ L }, Normal{ Normal }
{
}

FBuoyantMeshTriangle::FBuoyantMeshTriangle()
{
}

float FBuoyantMeshTriangle::GetTriangleArea(const FVector& Vertex1, const FVector& Vertex2,
	const FVector& Vertex3)
{
	SCOPE_CYCLE_COUNTER(STAT_GetTriangleArea);
	// Calculate the area of the triangle by using Heron's formula
	const auto A = FVector::Dist(Vertex1, Vertex2);
	const auto B = FVector::Dist(Vertex2, Vertex3);
	const auto C = FVector::Dist(Vertex3, Vertex1);
	const auto S = (A + B + C) / 2.f;
	return FMath::Sqrt(S * (S - A) * (S - B) * (S - C));
}

TArray<ForceParams> FBuoyantMeshTriangle::GetHydrostaticForces(
	const TArray<FVector>& SubmergedTriangleVertices, const float WaterDensity,
	const float GravityMagnitude, const TFunction<float(FVector Position)> GetHeight) const
{
	SCOPE_CYCLE_COUNTER(STAT_GetForces);
	TArray<ForceParams> Forces;
	for (int i = 0; i < SubmergedTriangleVertices.Num(); i += 3)
	{
		const auto Vertex1 = SubmergedTriangleVertices[i];
		const auto Vertex2 = SubmergedTriangleVertices[i + 1];
		const auto Vertex3 = SubmergedTriangleVertices[i + 2];
		const auto TriangleCenter = (Vertex1 + Vertex2 + Vertex3) / 3.0f;

		const auto TriangleCenterHeight = GetHeight(TriangleCenter);
		if (TriangleCenterHeight > 0.f)
		{
			// Because of the way we have simplified cutting, it is possible for the triangle center
			// to be found above water. In this case we do not apply any force.
			continue;
		}

		const auto TriangleArea = GetTriangleArea(Vertex1, Vertex2, Vertex3);

		if (FMath::IsNearlyZero(TriangleArea))
		{
			// Avoid artifacts
			continue;
		}

		const auto h = TriangleCenterHeight;
		const auto g = GravityMagnitude;
		const auto n = Normal;
		const auto rho = WaterDensity;
		const auto A = TriangleArea;

		// The hydrostatic force on a submerged triangle
		const auto F = rho * g * h * n * A;

		ForceParams parameters = { F, TriangleCenter };
		Forces.Add(parameters);
	}
	return Forces;
}

FVector FBuoyantMeshTriangle::FindCutOnEdge(const FBuoyantMeshVertex& Start,
	const FBuoyantMeshVertex& End, float const CutDistance)
{
	const auto FullVector = End.Position - Start.Position;
	const auto CutVector = FullVector * CutDistance;
	return Start.Position + CutVector;
}

TArray<FVector> FBuoyantMeshTriangle::GetSubmergedPortion(const UWorld* World,
	bool bDrawWaterline) const
{
	SCOPE_CYCLE_COUNTER(STAT_CutTriangle);
	// Case in which one vertex is above water and the other two are below.
	// See figure Figure 7 in the article in the header.
	if (!H.IsUnderwater() && M.IsUnderwater() && L.IsUnderwater())
	{
		const auto tM = -M.Height / (H.Height - M.Height);  // Intersection distance for MH
		const auto Im = FindCutOnEdge(M, H, tM);

		const auto tL = -L.Height / (H.Height - L.Height);  // Intersection distance for LH
		const auto Il = FindCutOnEdge(L, H, tL);

		if (bDrawWaterline && IsValid(World))
		{
			// The surface line goes from Im to Il.
			DrawDebugLine(World, Im, Il, FColor::Blue);
		}

		TArray<FVector> CutResult;
		// First triangle cut
		CutResult.Emplace(M.Position);
		CutResult.Emplace(Im);
		CutResult.Emplace(L.Position);

		// Second triangle cut
		CutResult.Emplace(Im);
		CutResult.Emplace(Il);
		CutResult.Emplace(L.Position);
		return CutResult;
	}
	// Case in which two vertices are above water and one is below.
	// See figure Figure 8 in the article in the header.
	else if (!H.IsUnderwater() && !M.IsUnderwater() && L.IsUnderwater())
	{
		const auto tM = -L.Height / (M.Height - L.Height);  // Intersection distance for LM
		const auto Jm = FindCutOnEdge(L, M, tM);

		const auto tH = -L.Height / (H.Height - L.Height);  // Intersection distance for LH
		const auto Jh = FindCutOnEdge(L, H, tH);

		if (bDrawWaterline && IsValid(World))
		{
			// The surface line goes from Jm to Jh.
			DrawDebugLine(World, Jm, Jh, FColor::Blue, false, -1.f, 0, 10.f);
		}

		// Return the single triangle cut.
		TArray<FVector> CutResult;
		CutResult.Emplace(Jh);
		CutResult.Emplace(Jm);
		CutResult.Emplace(L.Position);
		return CutResult;
	}
	else if (H.IsUnderwater() && M.IsUnderwater() && L.IsUnderwater())
	{
		// Return the entire triangle.
		TArray<FVector> CutResult;
		CutResult.Emplace(H.Position);
		CutResult.Emplace(M.Position);
		CutResult.Emplace(L.Position);
		return CutResult;
	}
	else if (!H.IsUnderwater() && !M.IsUnderwater() && !L.IsUnderwater())
	{
		return{};  // No part is underwater.
	}

	else
	{
		// The other cases should cover everything, we should never get to this point.
		verify(false);
		return{};
	}
}
