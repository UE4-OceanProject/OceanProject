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
	float _baseAngularDamping;
	float _baseLinearDamping;
 
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	AOceanManager* OceanManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	float TestPointRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	float Buoyancy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	FVector DirectionalDamping;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy")
	bool DrawDebugSpheres;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	float ChunkSleepThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced")
	float ChunkStabilizationThreshold;
 
};