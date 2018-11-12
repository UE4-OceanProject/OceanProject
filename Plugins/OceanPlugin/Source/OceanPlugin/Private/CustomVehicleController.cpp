/*=================================================
* FileName: CustomVehicleController.cpp
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/04/26
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

#include "CustomVehicleController.h"
#include "UObject/UObjectIterator.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"


#include "BuoyancyForceComponent.h"
#include "BuoyantDestructibleComponent.h"

ACustomVehicleController::ACustomVehicleController(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	PlayerPawn = nullptr;
}

void ACustomVehicleController::EnableBuoyancy()
{
	for (TObjectIterator<UBuoyancyForceComponent> Itr; Itr; ++Itr)
	{
		Itr->ToggleActive();
	}
}

void ACustomVehicleController::DrawBuoyancyPoints()
{
	for (TObjectIterator<UBuoyancyForceComponent> Itr; Itr; ++Itr)
	{
		Itr->DrawDebugPoints = !Itr->DrawDebugPoints;
	}

	for (TObjectIterator<UBuoyantDestructibleComponent> Itr; Itr; ++Itr)
	{
		Itr->DrawDebugPoints = !Itr->DrawDebugPoints;
	}

// 	for (TActorIterator<ABuoyantDestructible> ActorItr(GetWorld()); ActorItr; ++ActorItr)
// 	{
// 		ActorItr->DrawDebugPoints = !ActorItr->DrawDebugPoints;
// 	}
}

void ACustomVehicleController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate
	DOREPLIFETIME(ACustomVehicleController, PlayerPawn);
}

void ACustomVehicleController::EnterVehicle(APawn* Vehicle)
{
	if (!Vehicle->IsValidLowLevel() || !GetPawn()->IsValidLowLevel()) return;

	if (!Vehicle->IsControlled() && !GetIsDriving())
	{
		PlayerPawn = GetPawn();
		Possess(Vehicle);
	}
}

void ACustomVehicleController::ExitVehicle()
{
	if (GetIsDriving())
	{
		//UnPossess();
		Possess(PlayerPawn);
		PlayerPawn = nullptr;
	}
}

bool ACustomVehicleController::GetIsDriving()
{
	return PlayerPawn != nullptr;
}

void ACustomVehicleController::PawnLeavingGame()
{
	if (GetPawn() != nullptr)
	{
		if (GetIsDriving())
		{
			UnPossess();
			PlayerPawn->Destroy();
			PlayerPawn = nullptr;
			return;
		}

		GetPawn()->Destroy();
		SetPawn(nullptr);
	}

	//if (GetPawn() != NULL)
	//{
	//	GetPawn()->Destroy();
	//	SetPawn(NULL);
	//}
}

/*void ACustomVehicleController::Possess(APawn* PawnToPossess)
{
	//if (GetIsDriving()) return;

	Super::Possess(PawnToPossess);
}

void ACustomVehicleController::UnPossess()
{
	//if (GetIsDriving()) return;

	Super::UnPossess();
}*/
