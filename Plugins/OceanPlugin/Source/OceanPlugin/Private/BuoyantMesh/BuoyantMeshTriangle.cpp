/*=================================================
* FileName: BuoyantMeshTriangle.cpp
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

#include "BuoyantMesh/BuoyantMeshTriangle.h"
#include "BuoyantMesh/BuoyantMeshSubtriangle.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "CoreUObject.h"


FBuoyantMeshTriangle FBuoyantMeshTriangle::FromClockwiseVertices(const FBuoyantMeshVertex& A,
                                                                 const FBuoyantMeshVertex& B,
                                                                 const FBuoyantMeshVertex& C)
{
	const auto TriangleNormal = FVector::CrossProduct(B.Position - A.Position, C.Position - A.Position).GetSafeNormal();

	const FBuoyantMeshVertex* H;
	const FBuoyantMeshVertex* M;
	const FBuoyantMeshVertex* L;
	SortVerticesByHeight(A, B, C, /*out*/ &H, /*out*/ &M, /*out*/ &L);

	return {*H, *M, *L, TriangleNormal};
}

template <typename T>
void FBuoyantMeshTriangle::SwapPointers(const T*& p, const T*& q)
{
	const T* Temp = p;
	p = q;
	q = Temp;
}

void FBuoyantMeshTriangle::SortVerticesByHeight(const FBuoyantMeshVertex& InA,
                                                const FBuoyantMeshVertex& InB,
                                                const FBuoyantMeshVertex& InC,
                                                const FBuoyantMeshVertex** OutH,
                                                const FBuoyantMeshVertex** OutM,
                                                const FBuoyantMeshVertex** OutL)
{
	auto H = &InA; // Needs to be the sorted so it's the highest vertex.
	auto M = &InB; // Needs to be the sorted so it's the middle vertex.
	auto L = &InC; // Needs to be the sorted so it's the Lowest vertex.

	if (L->Height > H->Height)
	{
		SwapPointers(L, H);
	}
	if (L->Height > M->Height)
	{
		SwapPointers(L, M);
	}
	// Now the L is the lowest vertex. We only need to check M and H.
	if (M->Height > H->Height)
	{
		SwapPointers(M, H);
	}
	*OutH = H;
	*OutM = M;
	*OutL = L;
}

FBuoyantMeshTriangle::FBuoyantMeshTriangle(const FBuoyantMeshVertex& H,
                                           const FBuoyantMeshVertex& M,
                                           const FBuoyantMeshVertex& L,
                                           const FVector& Normal)
    : Normal{Normal}, H{H}, M{M}, L{L}
{
}

FVector FBuoyantMeshTriangle::FindCutOnEdge(const FBuoyantMeshVertex& Start,
                                            const FBuoyantMeshVertex& End,
                                            float const CutDistance)
{
	const auto FullVector = End.Position - Start.Position;
	const auto CutVector = FullVector * CutDistance;
	return Start.Position + CutVector;
}

TArray<FBuoyantMeshSubtriangle> FBuoyantMeshTriangle::GetSubmergedPortion(const UWorld* World,
                                                                          bool bDrawWaterline) const
{
	// Case in which one vertex is above water and the other two are below.
	// See figure Figure 7 in the article in the header.
	if (!H.IsUnderwater() && M.IsUnderwater() && L.IsUnderwater())
	{
		const auto tM = -M.Height / (H.Height - M.Height); // Intersection distance for MH
		const auto Im = FindCutOnEdge(M, H, tM);

		const auto tL = -L.Height / (H.Height - L.Height); // Intersection distance for LH
		const auto Il = FindCutOnEdge(L, H, tL);

		if (bDrawWaterline && IsValid(World))
		{
			// The surface line goes from Im to Il.
			DrawDebugLine(World, Im, Il, FColor::Blue, false, -1.f, 0, 16.f);
		}

		TArray<FBuoyantMeshSubtriangle> CutResult{};
		// First triangle cut
		CutResult.Emplace(M.Position, Im, L.Position);
		// Second triangle cut
		CutResult.Emplace(Im, Il, L.Position);
		return CutResult;
	}
	// Case in which two vertices are above water and one is below.
	// See figure Figure 8 in the article in the header.
	else if (!H.IsUnderwater() && !M.IsUnderwater() && L.IsUnderwater())
	{
		const auto tM = -L.Height / (M.Height - L.Height); // Intersection distance for LM
		const auto Jm = FindCutOnEdge(L, M, tM);

		const auto tH = -L.Height / (H.Height - L.Height); // Intersection distance for LH
		const auto Jh = FindCutOnEdge(L, H, tH);

		if (bDrawWaterline && IsValid(World))
		{
			// The surface line goes from Jm to Jh.
			DrawDebugLine(World, Jm, Jh, FColor::Blue, false, -1.f, 0, 10.f);
		}

		// Return the single triangle cut.
		TArray<FBuoyantMeshSubtriangle> CutResult;
		CutResult.Emplace(FBuoyantMeshSubtriangle{Jh, Jm, L.Position});
		return CutResult;
	}
	//  All three vertices are underwater.
	else if (H.IsUnderwater() && M.IsUnderwater() && L.IsUnderwater())
	{
		// Return the entire triangle.
		TArray<FBuoyantMeshSubtriangle> CutResult{};
		CutResult.Emplace(H.Position, M.Position, L.Position);
		return CutResult;
	}
	// No part is underwater.
	else if (!H.IsUnderwater() && !M.IsUnderwater() && !L.IsUnderwater())
	{
		// No submerged part.
		return {}; 
	}
	else
	{
		// The other cases should cover everything, we should never get to this point.
		checkNoEntry();
		return {};
	}
}
