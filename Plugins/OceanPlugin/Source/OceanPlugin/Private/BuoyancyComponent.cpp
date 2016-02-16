/*=================================================
* FileName: BuoyancyComponent.cpp
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
#include "BuoyancyComponent.h"


UBuoyancyComponent::UBuoyancyComponent(const class FObjectInitializer& PCIP)
	: Super(PCIP) 
{
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

void UBuoyancyComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// If no OceanManager is defined auto-detect
	if (!OceanManager)
	{
		for (TActorIterator<AOceanManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			OceanManager = Cast<AOceanManager>(*ActorItr);
			break;
		}
	}

	ApplyUprightConstraint();

	TestPointRadius = FMath::Abs(TestPointRadius);

	//Signed based on gravity, just in case we need an upside down world
	_SignedRadius = FMath::Sign(GetGravityZ()) * TestPointRadius;

	if (UpdatedPrimitive->IsValidLowLevel())
	{
		_baseLinearDamping = UpdatedPrimitive->GetLinearDamping();
		_baseAngularDamping = UpdatedPrimitive->GetAngularDamping();
	}
}

void UBuoyancyComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (!OceanManager || !UpdatedComponent || !UpdatedPrimitive) return;

	if (!UpdatedComponent->IsSimulatingPhysics())
	{
		FVector waveHeight = OceanManager->GetWaveHeightValue(UpdatedComponent->GetComponentLocation());
		UpdatedPrimitive->SetWorldLocation(FVector(UpdatedComponent->GetComponentLocation().X, UpdatedComponent->GetComponentLocation().Y, waveHeight.Z), true);
		return;
	}

	//ApplyUprightConstraint is apparently needed again at first tick for BP-updated components. 
	//TODO: there has to be a better way than this(?), PostInitialize(?)
	if (!_hasTicked)
	{
		_hasTicked = true;
		ApplyUprightConstraint();
	}

	float TotalPoints = TestPoints.Num();
	if (TotalPoints < 1) return;

	int PointsUnderWater = 0;
	for (int pointIndex = 0; pointIndex < TotalPoints; pointIndex++)
	{
		if (!TestPoints.IsValidIndex(pointIndex)) return; //Array size changed during runtime

		bool isUnderwater = false;
		FVector testPoint = TestPoints[pointIndex];
		FVector worldTestPoint = UpdatedComponent->GetComponentTransform().TransformPosition(testPoint);
		float waveHeight = OceanManager->GetWaveHeightValue(worldTestPoint).Z;

		//If test point radius is touching water add buoyancy force
		if (waveHeight > (worldTestPoint.Z + _SignedRadius))
		{
			PointsUnderWater++;
			isUnderwater = true;

			float DepthMultiplier = (waveHeight - (worldTestPoint.Z + _SignedRadius)) / (TestPointRadius * 2);
			DepthMultiplier = FMath::Clamp(DepthMultiplier, 0.f, 1.f);

			//If we have a point density override, use the overriden value insted of MeshDensity
			float PointDensity = PointDensityOverride.IsValidIndex(pointIndex) ? PointDensityOverride[pointIndex] : MeshDensity;

			/**
			* --------
			* Buoyancy force formula: (Volume(Mass / Density) * Fluid Density * -Gravity) / Total Points * Depth Multiplier
			* --------
			*/
			float BuoyancyForceZ = UpdatedPrimitive->GetMass() / PointDensity * FluidDensity * -GetGravityZ() / TotalPoints * DepthMultiplier;

			//Experimental velocity damping using GetUnrealWorldVelocityAtPoint!
			FVector DampingForce = -GetVelocityAtPoint(UpdatedPrimitive, worldTestPoint) * VelocityDamper * UpdatedPrimitive->GetMass() * DepthMultiplier;

			//Wave push force
			if (EnableWaveForces)
			{
				float waveVelocity = FMath::Clamp(GetVelocityAtPoint(UpdatedPrimitive, worldTestPoint).Z, -20.f, 150.f) * (1 - DepthMultiplier);
				DampingForce += OceanManager->WaveDirection * UpdatedPrimitive->GetMass() * waveVelocity * WaveForceMultiplier / TotalPoints;
			}

			//Add force for this test point
			UpdatedPrimitive->AddForceAtLocation(FVector(DampingForce.X, DampingForce.Y, DampingForce.Z + BuoyancyForceZ), worldTestPoint);
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
		&& UpdatedPrimitive->GetPhysicsLinearVelocity().Size() > MaxUnderwaterVelocity)
	{
		FVector	Velocity = UpdatedPrimitive->GetPhysicsLinearVelocity().GetSafeNormal() * MaxUnderwaterVelocity;
		UpdatedPrimitive->SetPhysicsLinearVelocity(Velocity);
	}

	//Update damping based on number of underwater test points
	UpdatedPrimitive->SetLinearDamping(_baseLinearDamping + FluidLinearDamping / TotalPoints * PointsUnderWater);
	UpdatedPrimitive->SetAngularDamping(_baseAngularDamping + FluidAngularDamping / TotalPoints * PointsUnderWater);
}

FVector UBuoyancyComponent::GetVelocityAtPoint(UPrimitiveComponent* Target, FVector Point, FName BoneName)
{
	FBodyInstance* BI = Target->GetBodyInstance(BoneName);
	if (BI != NULL && BI->IsValidBodyInstance())
	{
		return BI->GetUnrealWorldVelocityAtPoint(Point);
	}
	return FVector::ZeroVector;
}

void UBuoyancyComponent::ApplyUprightConstraint()
{
	//Stay upright physics constraint (inspired by UDK's StayUprightSpring)
	if (EnableStayUprightConstraint)
	{
		if (!ConstraintComp) ConstraintComp = NewObject<UPhysicsConstraintComponent>(UpdatedPrimitive);

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

		ConstraintInstance.AngularRotationOffset = UpdatedPrimitive->GetComponentRotation().GetInverse() + StayUprightDesiredRotation;

		//UPhysicsConstraintComponent* ConstraintComp = NewObject<UPhysicsConstraintComponent>(UpdatedPrimitive);
		if (ConstraintComp)
		{
			ConstraintComp->ConstraintInstance = ConstraintInstance; //Set instance parameters
			ConstraintComp->SetWorldLocation(UpdatedPrimitive->GetComponentLocation());

			//Attach
			ConstraintComp->AttachTo(UpdatedComponent, NAME_None, EAttachLocation::KeepRelativeOffset);
			ConstraintComp->SetConstrainedComponents(UpdatedPrimitive, NAME_None, NULL, NAME_None);
		}
	}
}
