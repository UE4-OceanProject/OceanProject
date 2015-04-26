#include "OceanPluginPrivatePCH.h"
#include "CustomFunctionLibrary.h"

UCustomFunctionLibrary::UCustomFunctionLibrary(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
}

//Why aren't these exposed by Epic?
//TODO: send a pull request..
void UCustomFunctionLibrary::SetCenterOfMassOffset(UPrimitiveComponent* Target, FVector Offset, FName BoneName)
{
	if (!Target) return;

	Target->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;

	FBodyInstance* BI = Target->GetBodyInstance(BoneName);
	if (BI->IsValidBodyInstance())
	{
		BI->COMNudge = Offset;
		BI->UpdateMassProperties();
	}
}

void UCustomFunctionLibrary::SetCanCharacterStepUpOn(UPrimitiveComponent* Target, ECanBeCharacterBase CanBeCharBase)
{
	if (!Target) return;

	Target->CanCharacterStepUpOn = CanBeCharBase;
}
