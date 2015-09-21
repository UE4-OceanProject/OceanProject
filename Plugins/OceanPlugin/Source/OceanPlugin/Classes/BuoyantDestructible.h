/*=================================================
* FileName: BuoyantDestructible.h
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.9
* Created on: 2015/03/29
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

#pragma once

#include "OceanPluginPrivatePCH.h"
#include "BuoyantDestructibleComponent.h"
#include "BuoyantDestructible.generated.h"

UCLASS()
class OCEANPLUGIN_API ABuoyantDestructible : public AActor
{
	GENERATED_UCLASS_BODY()
 
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BuoyantDestructible, meta = (ExposeFunctionCategories = "Destruction,Components|Destructible,Buoyancy Settings,Advanced", AllowPrivateAccess = "true"))
	UBuoyantDestructibleComponent* BuoyantDestructibleComponent;
};
