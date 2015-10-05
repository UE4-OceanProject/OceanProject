
/*=================================================
* FileName: BuoyantMeshVertex.cpp
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
#include "BuoyantMesh/BuoyantMeshVertex.h"

bool FBuoyantMeshVertex::IsUnderwater() const
{
	return Height < 0.f;
}

FBuoyantMeshVertex::FBuoyantMeshVertex(const FVector& Position, float HeightAboveWater)
    : Height{HeightAboveWater}, Position{Position}
{
}