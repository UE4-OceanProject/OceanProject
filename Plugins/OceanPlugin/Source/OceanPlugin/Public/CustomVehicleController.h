/*=================================================
* FileName: CustomVehicleController.h
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/06/26
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
#include "GameFramework/PlayerController.h"
#include "CustomVehicleController.generated.h"


/**
 * Custom player controller that has custom functionality to help possess vehicles.
 */
UCLASS()
class OCEANPLUGIN_API ACustomVehicleController : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	virtual void EnterVehicle(APawn* Vehicle);

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	virtual void ExitVehicle();

	UFUNCTION(BlueprintPure, Category = "Vehicle")
	virtual bool GetIsDriving();

	//--Custom debugging console commands--

	UFUNCTION(exec)
	virtual void DrawBuoyancyPoints();

	UFUNCTION(exec)
	virtual void EnableBuoyancy();
	
private:

	UPROPERTY(Replicated)
	APawn* PlayerPawn;

	virtual void PawnLeavingGame() override;
	//virtual void Possess(APawn* PawnToPossess) override;
	//virtual void UnPossess() override;
	
};
