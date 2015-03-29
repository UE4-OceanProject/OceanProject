#pragma once

#include "OceanManager.h"
#include "OceanPluginPrivatePCH.h"
#include "BuoyantDestructible.generated.h"

UCLASS()
class ABuoyantDestructible : public ADestructibleActor
{
	GENERATED_UCLASS_BODY()
 
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void PostLoad() override;

private:
	float _SignedRadius;
	float _baseAngularDamping;
	float _baseLinearDamping;
 
public:

	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Buoyancy Settings")
	AOceanManager* OceanManager;

	/* Density of each chunk */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float ChunkDensity;

	/* Density of water */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float FluidDensity;

	/* Linear damping when chunk is in fluid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float FluidLinearDamping;

	/* Angular damping when chunk is in fluid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float FluidAngularDamping;

	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "Buoyancy Settings")
	FVector VelocityDamper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	bool ClampMaxVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float MaxUnderwaterVelocity;

	/* Radius of the test point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Settings")
	float TestPointRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	bool DrawDebugPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	bool EnableWaveForces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Buoyancy Settings")
	float WaveForceMultiplier;

	/* Use a low value (or even zero) if the chunks sleep too easily on water */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	float ChunkSleepThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	float ChunkStabilizationThreshold;
 
};