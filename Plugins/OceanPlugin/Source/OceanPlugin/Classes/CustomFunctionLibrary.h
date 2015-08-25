/*=================================================
* FileName: CustomFunctionLibrary.h
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.8.3
* Created on: 2015/04/26
*
* Last Edited on: 2015/04/26
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

#include "CustomFunctionLibrary.generated.h"

UCLASS()
class UCustomFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	/*Custom function until Epic implements it.*/
	UFUNCTION(BlueprintCallable, Category = "Physics")
	static void SetCenterOfMassOffset(UPrimitiveComponent* Target, FVector Offset, FName BoneName = NAME_None);

	/*Custom function until Epic implements it.*/
	UFUNCTION(BlueprintCallable, Category = "Collision")
	static void SetCanCharacterStepUpOn(UPrimitiveComponent* Target, ECanBeCharacterBase CanBeCharBase);

};
