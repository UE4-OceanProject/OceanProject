/*=================================================
* FileName: BuoyantDestructible.cpp
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/04/26
*
* Last Edited on: 2015/08/28
* Last Edited by: TK-Master
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
#include "BuoyantDestructibleComponent.h"
#include "BuoyantDestructible.h"

ABuoyantDestructible::ABuoyantDestructible(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BuoyantDestructibleComponent = CreateDefaultSubobject<UBuoyantDestructibleComponent>(TEXT("DestructibleComponent0"));
	RootComponent = BuoyantDestructibleComponent;
}
