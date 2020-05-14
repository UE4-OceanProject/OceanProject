/*=================================================
* FileName: BuoyantMeshVertex.h
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


// Associates a position with a height above water.
struct FBuoyantMeshVertex
{
	// Height Above Water
	const float Height;
	const FVector Position;

	bool IsUnderwater() const;

	FBuoyantMeshVertex(const FVector& Position, float HeightAboveWater);
};