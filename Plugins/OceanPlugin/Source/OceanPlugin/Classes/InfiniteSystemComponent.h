/*=================================================
* FileName: InfiniteSystemComponent.h
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
#include <Components/SceneComponent.h>
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
UCLASS(hidecategories=(Object, Mobility, LOD), ClassGroup=Physics, showcategories=Trigger, meta=(BlueprintSpawnableComponent))
class OCEANPLUGIN_API UInfiniteSystemComponent : public USceneComponent
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
	virtual void BeginPlay() override;

protected:
	UWorld* World;
};
