#pragma once

#include "OceanPluginPrivatePCH.h"
#include "InfiniteSystemComponent.generated.h"

UENUM()
enum EFollowMethod
{
	LookAtLocation,
	FollowCamera,
	FollowPawn,
	Stationary
};

/** 
 *	Infinite Ocean Plane System.
 *	Follows camera and scales by distance to make a plane appear as infinite.
 */
UCLASS(hidecategories=(Object, Mobility, LOD), ClassGroup=Physics, showcategories=Trigger, MinimalAPI, meta=(BlueprintSpawnableComponent))
class UInfiniteSystemComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool UpdateInEditor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<enum EFollowMethod> FollowMethod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float GridSnapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MaxLookAtDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool ScaleByDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ScaleDistanceFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ScaleStartDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ScaleMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ScaleMax;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};