#pragma once

#include "OceanPluginPrivatePCH.h"
#include "CustomVehicleController.generated.h"

/**
 * Custom player controller that has custom functionality to help possess vehicles.
 */
UCLASS()
class ACustomVehicleController : public APlayerController
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

	UFUNCTION(exec, BlueprintImplementableEvent, meta = (FriendlyName = "Console Command ~ nogerstrnershader"))
	virtual void noGerstnerShader();

	UFUNCTION(exec)
	virtual void noGerstner();

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
