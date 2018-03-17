/*=================================================
* FileName: BuoyantDestructible.h
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/03/29
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
