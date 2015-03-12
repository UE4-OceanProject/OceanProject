// OceanPlugin version

#pragma once

#include "OceanPluginPrivatePCH.h"
#include "GameFramework/MovementComponent.h"
#include "OceanManager.h"
#include "BuoyancyMovementComponent.generated.h"

/*
* A custom MovementComponent that enables Buoyancy when used with an ocean surface.
*/
UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), HideCategories = (PlanarMovement, "Components|Movement|Planar", Velocity))
class UBuoyancyMovementComponent : public UMovementComponent {
	GENERATED_UCLASS_BODY()

	/* OceanManager used by the component, if unassign component will auto-detect */
	UPROPERTY(BlueprintReadOnly)
	AOceanManager* OceanManager;

	/* Buoyancy force multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
		float Buoyancy;

	/* Points used to test the water level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
		TArray<FVector> TestPoints;

	/* Radius of the volume represented by the points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
		float TestPointVolumeRadius;

	/* Directional Damping */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
		FVector DirectionalDamping;

	/* Set the location used when not simulating physics */
	UFUNCTION(BlueprintCallable, Category = "Buoyancy")
		void SetAnchoredLocation(FVector location);

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitializeComponent() override;
	virtual bool IsInWater() const override;
	//End UActorComponent Interface


	/* TODO - Generating Wake */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean")
	//bool IsGeneratingWake;

	/* TODO - Wake Size */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ocean")
	//float WakeSize;

private:
	bool _pointsInWater;

	float _baseAngularDamping;
	float _baseLinearDamping;

	// Location used when not simulating physics and just snapping to surface of water.
	FVector _anchoredLocation;

	float _buoyantForce;

	// Specific Gravity of water in Grams per Cubic Centimeter
	float _specificGravity;

	};

