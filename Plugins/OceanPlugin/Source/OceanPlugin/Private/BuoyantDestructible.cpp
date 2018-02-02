/*=================================================
* FileName: BuoyantDestructible.cpp
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/04/26
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

#include "BuoyantDestructible.h"
#include "BuoyantDestructibleComponent.h"

ABuoyantDestructible::ABuoyantDestructible(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BuoyantDestructibleComponent = CreateDefaultSubobject<UBuoyantDestructibleComponent>(TEXT("DestructibleComponent0"));
	RootComponent = BuoyantDestructibleComponent;
}
