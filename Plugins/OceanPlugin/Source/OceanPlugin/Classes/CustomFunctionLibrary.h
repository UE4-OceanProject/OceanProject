#pragma once

#include "CustomFunctionLibrary.generated.h"

UCLASS()
class UCustomFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:

	/*Custom function until Epic implements it.*/
	UFUNCTION(BlueprintCallable, Category = "Physics")
	static void SetCenterOfMassOffset(UPrimitiveComponent* Target, FVector Offset, FName BoneName = NAME_None);

	/*Custom function until Epic implements it.*/
	UFUNCTION(BlueprintCallable, Category = "Collision")
	static void SetCanCharacterStepUpOn(UPrimitiveComponent* Target, ECanBeCharacterBase CanBeCharBase);

};