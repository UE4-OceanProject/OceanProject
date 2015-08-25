/*=================================================
* FileName: CustomFunctionLibrary.cpp
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.8.3
* Created on: 2015/04/26
*
* Last Edited on: 2015/06/29
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
#include "CustomFunctionLibrary.h"

UCustomFunctionLibrary::UCustomFunctionLibrary(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
}

//Why aren't these exposed by Epic?
//TODO: send a pull request..
void UCustomFunctionLibrary::SetCenterOfMassOffset(UPrimitiveComponent* Target, FVector Offset, FName BoneName)
{
	if (!Target) return;

	Target->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;

	FBodyInstance* BI = Target->GetBodyInstance(BoneName);
	if (BI->IsValidBodyInstance())
	{
		BI->COMNudge = Offset;
		BI->UpdateMassProperties();
	}
}

void UCustomFunctionLibrary::SetCanCharacterStepUpOn(UPrimitiveComponent* Target, ECanBeCharacterBase CanBeCharBase)
{
	if (!Target) return;

	Target->CanCharacterStepUpOn = CanBeCharBase;
}
