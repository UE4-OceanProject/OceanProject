#include "OceanPluginPrivatePCH.h"
#include "InfiniteSystemComponent.h"
//#include "UnrealEd.h"

UInfiniteSystemComponent::UInfiniteSystemComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	bTickInEditor = true;
	
	//Defaults
	UpdateInEditor = true;
	GridSnapSize = 0;
	MaxLookAtDistance = 20000;
	ScaleByDistance = true;
	ScaleDistanceFactor = 1000;
	ScaleStartDistance = 1;
	ScaleMin = 1;
	ScaleMax = 100;
}

void UInfiniteSystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If disabled or we are not attached to a parent component, return.
	if (!bIsActive || !AttachParent || !GetWorld()) return;

	FVector CamLoc;
	FRotator CamRot;
	FVector PawnLoc;
	FVector NewLoc;

#if WITH_EDITOR
	if (GIsEditor)
	{
		if (!UpdateInEditor) return;

		TArray<FVector> viewLocations = GetWorld()->ViewLocationsRenderedLastFrame;
		if (viewLocations.Num() != 0)
		{
			CamLoc = viewLocations[0];
		}

		//FEditorViewportClient* client = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
		//CamLoc = client->GetViewLocation();
		//CamRot = client->GetViewRotation();

		if (FollowMethod == LookAtLocation || FollowMethod == FollowCamera)
		{
			NewLoc = CamLoc;
			NewLoc = NewLoc.GridSnap(GridSnapSize);
			NewLoc.Z = AttachParent->GetComponentLocation().Z;
			AttachParent->SetWorldLocation(NewLoc);
		}
		else
		{
			AttachParent->SetRelativeLocation(FVector(0, 0, 0)); //Reset location
		}

		if (ScaleByDistance && FMath::Abs(CamLoc.Z - AttachParent->GetComponentLocation().Z) > ScaleStartDistance)
		{
			float DistScale = FMath::Abs(CamLoc.Z - AttachParent->GetComponentLocation().Z) / ScaleDistanceFactor;
			DistScale = FMath::Clamp(DistScale, ScaleMin, ScaleMax);
			AttachParent->SetRelativeScale3D(FVector(DistScale, DistScale, 1)); //Scale only on x & y axis
		}
		else if (!ScaleByDistance)
		{
			AttachParent->SetRelativeScale3D(FVector(1, 1, 1)); //Reset scale
		}
		return;
	}
#endif

	if (GetWorld()->WorldType == EWorldType::Game || GetWorld()->WorldType == EWorldType::PIE)
	{
		if (!GEngine || !GEngine->GetFirstLocalPlayerController(GetWorld())) return;
		
		GEngine->GetFirstLocalPlayerController(GetWorld())->PlayerCameraManager->GetCameraViewPoint(CamLoc, CamRot);

		if (GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn()) //null check
		{
			PawnLoc = GEngine->GetFirstLocalPlayerController(GetWorld())->GetPawn()->GetActorLocation();
		}
		else
		{
			PawnLoc = AttachParent->GetComponentLocation();
		}
	}

	switch (FollowMethod)
	{
		case LookAtLocation:
			if (!FMath::SegmentPlaneIntersection(CamLoc, CamLoc + CamRot.Vector() * MaxLookAtDistance, FPlane(AttachParent->GetComponentLocation(), FVector(0, 0, 1)), NewLoc))
			{
				NewLoc = CamLoc + CamRot.Vector() * MaxLookAtDistance;
			}
			break;
		case FollowCamera:
			NewLoc = CamLoc;
			break;
		case FollowPawn:
			NewLoc = PawnLoc;
			break;
		default:
			break;
	};

	//UE_LOG(LogTemp, Warning, TEXT("Camera Z Distance from Plane: %f"), FMath::Abs(CamLoc.Z - AttachParent->GetComponentLocation().Z));

	if (ScaleByDistance && FMath::Abs(CamLoc.Z - AttachParent->GetComponentLocation().Z) > ScaleStartDistance)
	{
		float DistScale = FMath::Abs(CamLoc.Z - AttachParent->GetComponentLocation().Z) / ScaleDistanceFactor;
		DistScale = FMath::Clamp(DistScale, ScaleMin, ScaleMax);
		AttachParent->SetRelativeScale3D(FVector(DistScale, DistScale, 1)); //Scale only on x & y axis
	}

	if (FollowMethod == Stationary) return;

	NewLoc = NewLoc.GridSnap(GridSnapSize);
	NewLoc.Z = AttachParent->GetComponentLocation().Z;
	AttachParent->SetWorldLocation(NewLoc);
}