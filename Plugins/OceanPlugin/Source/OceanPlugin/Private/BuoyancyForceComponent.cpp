/*=================================================
* FileName: BuoyancyForceComponent.cpp
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.8.3
* Created on: 2015/04/26
*
* Last Edited on: 2015/06/29
* Last Edited by: TK-Master
* 
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#include "OceanPluginPrivatePCH.h"
#include "BuoyancyForceComponent.h"

UBuoyancyForceComponent::UBuoyancyForceComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TickGroup;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	
	//Defaults
	MeshDensity = 600.0f;
	FluidDensity = 1025.0f;
	TestPointRadius = 10.0f;
	FluidLinearDamping = 1.0f;
	FluidAngularDamping = 1.0f;

	VelocityDamper = FVector(0.1, 0.1, 0.1);
	MaxUnderwaterVelocity = 1000.f;

	StayUprightStiffness = 50.0f;
	StayUprightDamping = 5.0f;

	WaveForceMultiplier = 2.0f;
}

/*void UBuoyancyForceComponent::PostLoad()
{
	Super::PostLoad();
}*/

void UBuoyancyForceComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//UE_LOG(LogTemp, Warning, TEXT("We're initializing..."));

	// If no OceanManager is defined, auto-detect
	if (!OceanManager)
	{
		for (TActorIterator<AOceanManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			OceanManager = Cast<AOceanManager>(*ActorItr);
			break;
		}
	}

	TestPointRadius = FMath::Abs(TestPointRadius);

	UPrimitiveComponent* BasePrimComp = Cast<UPrimitiveComponent>(AttachParent);
	if (BasePrimComp)
	{
		ApplyUprightConstraint(BasePrimComp);

		//Store the initial damping values.
		_baseLinearDamping = BasePrimComp->GetLinearDamping();
		_baseAngularDamping = BasePrimComp->GetAngularDamping();
	}
}

void UBuoyancyForceComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// If disabled or we are not attached to a parent component, return.
	if( !bIsActive || !AttachParent ) return;

	if (!OceanManager) return;

	UPrimitiveComponent* BasePrimComp = Cast<UPrimitiveComponent>(AttachParent);
	if (!BasePrimComp) return;

	if (!BasePrimComp->IsSimulatingPhysics())
	{
		if (!SnapToSurfaceIfNoPhysics) return;

		FVector waveHeight = OceanManager->GetWaveHeightValue(BasePrimComp->GetComponentLocation());
		BasePrimComp->SetWorldLocation(FVector(BasePrimComp->GetComponentLocation().X, BasePrimComp->GetComponentLocation().Y, waveHeight.Z), true);
		return;
	}

	//Get gravity
	float Gravity = BasePrimComp->GetPhysicsVolume()->GetGravityZ();

	//--------------- If Skeletal ---------------
	USkeletalMeshComponent* SkeletalComp = Cast<USkeletalMeshComponent>(AttachParent);
	if (SkeletalComp && ApplyForceToBones)
	{
		TArray<FName> BoneNames;
		SkeletalComp->GetBoneNames(BoneNames);

		for (int32 Itr = 0; Itr < BoneNames.Num(); Itr++)
		{
			FBodyInstance* BI = SkeletalComp->GetBodyInstance(BoneNames[Itr], false);
			if (BI && BI->IsValidBodyInstance()
				&& BI->bEnableGravity) //Buoyancy doesn't exist without gravity
			{
				bool isUnderwater = false;
				//FVector worldBoneLoc = SkeletalComp->GetBoneLocation(BoneNames[Itr]);
				FVector worldBoneLoc = BI->GetCOMPosition(); //Use center of mass of the bone's physics body instead of bone's location
				float waveHeight = OceanManager->GetWaveHeightValue(worldBoneLoc).Z;

				float BoneDensity = MeshDensity;
				float BoneTestRadius = FMath::Abs(TestPointRadius);
				float SignedBoneRadius = FMath::Sign(Gravity) * TestPointRadius; //Direction of radius (test radius is actually a Z offset, should probably rename it!). Just in case we need an upside down world.

				//Get density & radius from the override array, if available.
				for (int pointIndex = 0; pointIndex < BoneOverride.Num(); pointIndex++)
				{
					FStructBoneOverride Override = BoneOverride[pointIndex];

					if (Override.BoneName.IsEqual(BoneNames[Itr]))
					{
						BoneDensity = Override.Density;
						BoneTestRadius = FMath::Abs(Override.TestRadius);
						SignedBoneRadius = FMath::Sign(Gravity) * BoneTestRadius;
					}
				}

				//If test point radius is below water surface, add buoyancy force.
				if (waveHeight > (worldBoneLoc.Z + SignedBoneRadius))
				{
					isUnderwater = true;

					float DepthMultiplier = (waveHeight - (worldBoneLoc.Z + SignedBoneRadius)) / (BoneTestRadius * 2);
					DepthMultiplier = FMath::Clamp(DepthMultiplier, 0.f, 1.f);

					float Mass = SkeletalComp->CalculateMass(BoneNames[Itr]); //Mass of this specific bone's physics body

					/**
					* --------
					* Buoyancy force formula: (Volume(Mass / Density) * Fluid Density * -Gravity) / Total Points * Depth Multiplier
					* --------
					*/
					float BuoyancyForceZ = Mass / BoneDensity * FluidDensity * -Gravity * DepthMultiplier;

					//Velocity damping.
					FVector DampingForce = -BI->GetUnrealWorldVelocity() * VelocityDamper * Mass * DepthMultiplier;

					//Experimental wave push force
					if (EnableWaveForces)
					{
						float waveVelocity = FMath::Clamp(BI->GetUnrealWorldVelocity().Z, -20.f, 150.f) * (1 - DepthMultiplier);
						DampingForce += OceanManager->WaveDirection * Mass * waveVelocity * WaveForceMultiplier;
					}

					//Add force to this bone
					BI->AddForce(FVector(DampingForce.X, DampingForce.Y, DampingForce.Z + BuoyancyForceZ));
					//BasePrimComp->AddForceAtLocation(FVector(DampingForce.X, DampingForce.Y, DampingForce.Z + BuoyancyForceZ), worldBoneLoc, BoneNames[Itr]);
				}

				//Apply fluid damping & clamp velocity
				if (isUnderwater)
				{
					BI->SetLinearVelocity(-BI->GetUnrealWorldVelocity() * (FluidLinearDamping / 10), true);
					BI->SetAngularVelocity(-BI->GetUnrealWorldAngularVelocity() * (FluidAngularDamping / 10), true);

					//Clamp the velocity to MaxUnderwaterVelocity
					if (ClampMaxVelocity && BI->GetUnrealWorldVelocity().Size() > MaxUnderwaterVelocity)
					{
						FVector	Velocity = BI->GetUnrealWorldVelocity().GetSafeNormal() * MaxUnderwaterVelocity;
						BI->SetLinearVelocity(Velocity, false);
					}
				}

				if (DrawDebugPoints)
				{
					FColor DebugColor = FLinearColor(0.8, 0.7, 0.2, 0.8).ToRGBE();
					if (isUnderwater) { DebugColor = FLinearColor(0, 0.2, 0.7, 0.8).ToRGBE(); } //Blue color underwater, yellow out of watter
					DrawDebugSphere(GetWorld(), worldBoneLoc, BoneTestRadius, 8, DebugColor);
				}
			}
		}
		return;
	}
	//--------------------------------------------------------

	float TotalPoints = TestPoints.Num();
	if (TotalPoints < 1) return;

	int PointsUnderWater = 0;
	for (int pointIndex = 0; pointIndex < TotalPoints; pointIndex++)
	{
		if (!TestPoints.IsValidIndex(pointIndex)) return; //Array size changed during runtime

		bool isUnderwater = false;
		FVector testPoint = TestPoints[pointIndex];
		FVector worldTestPoint = BasePrimComp->GetComponentTransform().TransformPosition(testPoint);
		float waveHeight = OceanManager->GetWaveHeightValue(worldTestPoint).Z;

		//Direction of radius (test radius is actually a Z offset, should probably rename it!). Just in case we need an upside down world.
		float SignedRadius = FMath::Sign(BasePrimComp->GetPhysicsVolume()->GetGravityZ()) * TestPointRadius;

		//If test point radius is below water surface, add buoyancy force.
		if (waveHeight > (worldTestPoint.Z + SignedRadius) 
			&& BasePrimComp->IsGravityEnabled()) //Buoyancy doesn't exist without gravity
		{
			PointsUnderWater++;
			isUnderwater = true;

			float DepthMultiplier = (waveHeight - (worldTestPoint.Z + SignedRadius)) / (TestPointRadius * 2);
			DepthMultiplier = FMath::Clamp(DepthMultiplier, 0.f, 1.f);

			//If we have a point density override, use the overridden value instead of MeshDensity
			float PointDensity = PointDensityOverride.IsValidIndex(pointIndex) ? PointDensityOverride[pointIndex] : MeshDensity;

			/**
			* --------
			* Buoyancy force formula: (Volume(Mass / Density) * Fluid Density * -Gravity) / Total Points * Depth Multiplier
			* --------
			*/
			float BuoyancyForceZ = BasePrimComp->GetMass() / PointDensity * FluidDensity * -Gravity / TotalPoints * DepthMultiplier;

			//Experimental velocity damping using VelocityAtPoint.
			FVector DampingForce = -GetUnrealVelocityAtPoint(BasePrimComp, worldTestPoint) * VelocityDamper * BasePrimComp->GetMass() * DepthMultiplier;

			//Experimental wave push force
			if (EnableWaveForces)
			{
				float waveVelocity = FMath::Clamp(GetUnrealVelocityAtPoint(BasePrimComp, worldTestPoint).Z, -20.f, 150.f) * (1 - DepthMultiplier);
				DampingForce += OceanManager->WaveDirection * BasePrimComp->GetMass() * waveVelocity * WaveForceMultiplier / TotalPoints;
			}

			//Add force for this test point
			BasePrimComp->AddForceAtLocation(FVector(DampingForce.X, DampingForce.Y, DampingForce.Z + BuoyancyForceZ), worldTestPoint);
		}

		if (DrawDebugPoints)
		{
			FColor DebugColor = FLinearColor(0.8, 0.7, 0.2, 0.8).ToRGBE();
			if (isUnderwater) { DebugColor = FLinearColor(0, 0.2, 0.7, 0.8).ToRGBE(); } //Blue color underwater, yellow out of watter
			DrawDebugSphere(GetWorld(), worldTestPoint, TestPointRadius, 8, DebugColor);
		}
	}

	//Clamp the velocity to MaxUnderwaterVelocity if there is any point underwater
	if (ClampMaxVelocity && PointsUnderWater > 0
		&& BasePrimComp->GetPhysicsLinearVelocity().Size() > MaxUnderwaterVelocity)
	{
		FVector	Velocity = BasePrimComp->GetPhysicsLinearVelocity().GetSafeNormal() * MaxUnderwaterVelocity;
		BasePrimComp->SetPhysicsLinearVelocity(Velocity);
	}

	//Update damping based on number of underwater test points
	BasePrimComp->SetLinearDamping(_baseLinearDamping + FluidLinearDamping / TotalPoints * PointsUnderWater);
	BasePrimComp->SetAngularDamping(_baseAngularDamping + FluidAngularDamping / TotalPoints * PointsUnderWater);
}

FVector UBuoyancyForceComponent::GetUnrealVelocityAtPoint(UPrimitiveComponent* Target, FVector Point, FName BoneName)
{
	if (!Target) return FVector::ZeroVector;

	FBodyInstance* BI = Target->GetBodyInstance(BoneName);
	if (BI->IsValidBodyInstance())
	{
		return BI->GetUnrealWorldVelocityAtPoint(Point);
	}

	return FVector::ZeroVector;
}

void UBuoyancyForceComponent::ApplyUprightConstraint(UPrimitiveComponent* BasePrimComp)
{
	//Stay upright physics constraint (inspired by UDK's StayUprightSpring)
	if (EnableStayUprightConstraint)
	{
		UPhysicsConstraintComponent* ConstraintComp = NewObject<UPhysicsConstraintComponent>(BasePrimComp);

		//Settings
		FConstraintInstance ConstraintInstance;

		ConstraintInstance.LinearXMotion = ELinearConstraintMotion::LCM_Free;
		ConstraintInstance.LinearYMotion = ELinearConstraintMotion::LCM_Free;
		ConstraintInstance.LinearZMotion = ELinearConstraintMotion::LCM_Free;

		//ConstraintInstance.LinearLimitSize = 0;

		//ConstraintInstance.AngularSwing1Motion = EAngularConstraintMotion::ACM_Limited;
		ConstraintInstance.AngularSwing2Motion = EAngularConstraintMotion::ACM_Limited;
		ConstraintInstance.AngularTwistMotion = EAngularConstraintMotion::ACM_Limited;

		ConstraintInstance.bSwingLimitSoft = true;
		ConstraintInstance.bTwistLimitSoft = true;

		//ConstraintInstance.Swing1LimitAngle = 0;
		ConstraintInstance.Swing2LimitAngle = 0;
		ConstraintInstance.TwistLimitAngle = 0;

		ConstraintInstance.SwingLimitStiffness = StayUprightStiffness;
		ConstraintInstance.SwingLimitDamping = StayUprightDamping;
		ConstraintInstance.TwistLimitStiffness = StayUprightStiffness;
		ConstraintInstance.TwistLimitDamping = StayUprightDamping;

		ConstraintInstance.AngularRotationOffset = BasePrimComp->GetComponentRotation().GetInverse() + StayUprightDesiredRotation;

		//UPhysicsConstraintComponent* ConstraintComp = NewObject<UPhysicsConstraintComponent>(BasePrimComp);
		if (ConstraintComp)
		{
			ConstraintComp->ConstraintInstance = ConstraintInstance; //Set instance parameters
			ConstraintComp->SetWorldLocation(BasePrimComp->GetComponentLocation());

			//Attach
			ConstraintComp->AttachTo(BasePrimComp, NAME_None, EAttachLocation::KeepRelativeOffset);
			ConstraintComp->SetConstrainedComponents(BasePrimComp, NAME_None, NULL, NAME_None);
		}
	}
}
