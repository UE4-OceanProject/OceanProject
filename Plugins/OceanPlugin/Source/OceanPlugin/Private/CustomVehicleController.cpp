#include "OceanPluginPrivatePCH.h"
#include "UnrealNetwork.h"
#include "CustomVehicleController.h"

#include "BuoyancyForceComponent.h"
#include "BuoyantDestructible.h"

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

	for (TActorIterator<ABuoyantDestructible> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->DrawDebugPoints = !ActorItr->DrawDebugPoints;
	}
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