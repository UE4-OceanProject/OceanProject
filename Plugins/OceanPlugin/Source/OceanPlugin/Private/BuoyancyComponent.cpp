/*=================================================
* FileName: BuoyancyComponent.cpp
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/04/26
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

#include "BuoyancyComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/ConstraintInstance.h"


UBuoyancyComponent::UBuoyancyComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

	//Store the world ref.
	World = GetWorld();

	// If no OceanManager is defined, auto-detect
	if (!OceanManager)
	{
		for (TActorIterator<AOceanManager> ActorItr(World); ActorItr; ++ActorItr)
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
		//Store the initial damping values.
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

			//If we have a point density override, use the overridden value instead of MeshDensity
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
				DampingForce += FVector(OceanManager->GlobalWaveDirection.X, OceanManager->GlobalWaveDirection.Y, 0) * UpdatedPrimitive->GetMass() * waveVelocity * WaveForceMultiplier / TotalPoints;
			}

			//Add force for this test point
			UpdatedPrimitive->AddForceAtLocation(FVector(DampingForce.X, DampingForce.Y, DampingForce.Z + BuoyancyForceZ), worldTestPoint);
		}

		if (DrawDebugPoints)
		{
			FColor DebugColor = FLinearColor(0.8, 0.7, 0.2, 0.8).ToRGBE();
			if (isUnderwater) { DebugColor = FLinearColor(0, 0.2, 0.7, 0.8).ToRGBE(); } //Blue color underwater, yellow out of watter
			DrawDebugSphere(World, worldTestPoint, TestPointRadius, 8, DebugColor);
		}
	}

	//Clamp the velocity to MaxUnderwaterVelocity if there is any point underwater
	if (ClampMaxVelocity && PointsUnderWater > 0
		&& UpdatedPrimitive->GetPhysicsLinearVelocity().Size() > MaxUnderwaterVelocity)
	{
		FVector	vVelocity = UpdatedPrimitive->GetPhysicsLinearVelocity().GetSafeNormal() * MaxUnderwaterVelocity;
		UpdatedPrimitive->SetPhysicsLinearVelocity(vVelocity);
	}

	//Update damping based on number of underwater test points
	UpdatedPrimitive->SetLinearDamping(_baseLinearDamping + FluidLinearDamping / TotalPoints * PointsUnderWater);
	UpdatedPrimitive->SetAngularDamping(_baseAngularDamping + FluidAngularDamping / TotalPoints * PointsUnderWater);
}

FVector UBuoyancyComponent::GetVelocityAtPoint(UPrimitiveComponent* Target, FVector Point, FName BoneName)
{
	if (!Target) return FVector::ZeroVector;

	FBodyInstance* BI = Target->GetBodyInstance(BoneName);
	if (BI->IsValidBodyInstance())
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

		ConstraintInstance.SetLinearXMotion(ELinearConstraintMotion::LCM_Free);
		ConstraintInstance.SetLinearYMotion(ELinearConstraintMotion::LCM_Free);
		ConstraintInstance.SetLinearZMotion(ELinearConstraintMotion::LCM_Free);

		//ConstraintInstance.LinearLimitSize = 0;

		//ConstraintInstance.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Limited);
		ConstraintInstance.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Limited);
		ConstraintInstance.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Limited);

		ConstraintInstance.SetOrientationDriveTwistAndSwing(true, true);

		//ConstraintInstance.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0);
		ConstraintInstance.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
		ConstraintInstance.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);

		ConstraintInstance.SetAngularDriveParams(StayUprightStiffness, StayUprightDamping, 0);

		ConstraintInstance.AngularRotationOffset = UpdatedPrimitive->GetComponentRotation().GetInverse() + StayUprightDesiredRotation;

		//UPhysicsConstraintComponent* ConstraintComp = NewObject<UPhysicsConstraintComponent>(UpdatedPrimitive);
		if (ConstraintComp)
		{
			ConstraintComp->ConstraintInstance = ConstraintInstance; //Set instance parameters
			ConstraintComp->SetWorldLocation(UpdatedPrimitive->GetComponentLocation());

			//Attach
			ConstraintComp->AttachToComponent(UpdatedComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
			ConstraintComp->SetConstrainedComponents(UpdatedPrimitive, NAME_None, NULL, NAME_None);
			ConstraintComp->RegisterComponent();
		}
	}
}
