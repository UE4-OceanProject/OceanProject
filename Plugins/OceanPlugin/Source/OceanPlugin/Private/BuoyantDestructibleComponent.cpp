/*=================================================
* FileName: BuoyantDestructibleComponent.cpp
* 
* Created by: TK-Master
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/08/27
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

#include "BuoyantDestructibleComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "PhysXIncludes.h" 
#include "PhysXPublic.h"


UBuoyantDestructibleComponent::UBuoyantDestructibleComponent(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;

	//Defaults
	ChunkDensity = 600.0f;
	FluidDensity = 1025.0f;
	TestPointRadius = 10.0f;
	FluidLinearDamping = 1.0f;
	FluidAngularDamping = 1.0f;

	VelocityDamper = FVector(0.1, 0.1, 0.1);
	MaxUnderwaterVelocity = 1000.f;

	ChunkSleepThreshold = 0.0f; //Default physx value is 50.0f
	ChunkStabilizationThreshold = 10.0f;

	WaveForceMultiplier = 2.0f;
}

void UBuoyantDestructibleComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// If no OceanManager is defined auto-detect
	if (!OceanManager)
	{
		for (TActorIterator<AOceanShaderManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			OceanManager = Cast<AOceanShaderManager>(*ActorItr);
			break;
		}
	}
	
	_baseLinearDamping = GetLinearDamping();
	_baseAngularDamping = GetAngularDamping();
}

void UBuoyantDestructibleComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OceanManager)
		return;

	float Gravity = GetPhysicsVolume()->GetGravityZ();
	TestPointRadius = FMath::Abs(TestPointRadius);

	//Signed based on gravity, just in case we need an upside down world
	_SignedRadius = FMath::Sign(Gravity) * TestPointRadius;

#if WITH_PHYSX
	uint32 ChunkCount = ApexDestructibleActor->getNumVisibleChunks();
	const uint16* ChunkIndices = ApexDestructibleActor->getVisibleChunks();
	for (uint32 c = 0; c < ChunkCount; c++)
	{
		PxRigidDynamic* Chunk = ApexDestructibleActor->getChunkPhysXActor(ChunkIndices[c]);
		check(Chunk);

		if (Chunk)
		{
			PxTransform Trans = Chunk->getGlobalPose();
			PxTransform MassTrans = Chunk->getCMassLocalPose();
			PxVec3 PxLoc = Trans.p + Trans.rotate(MassTrans.p);

			FVector Location = P2UVector(PxLoc);

			float waveHeight = OceanManager->GetWaveHeightValue(Location).Z;
			bool isUnderwater = false;

			//If test point radius is touching water add buoyancy force
			if (waveHeight > (Location.Z + _SignedRadius))
			{
				isUnderwater = true;

				float DepthMultiplier = (waveHeight - (Location.Z + _SignedRadius)) / (TestPointRadius * 2);
				DepthMultiplier = FMath::Clamp(DepthMultiplier, 0.f, 1.f);

				/**
				* --------
				* Buoyancy force formula: (Volume(Mass / Density) * Fluid Density * -Gravity) * Depth Multiplier
				* --------
				*/
				float BuoyancyForceZ = Chunk->getMass() / ChunkDensity * FluidDensity * -Gravity * DepthMultiplier;

				//Velocity damping
				FVector DampingForce = -P2UVector(Chunk->getLinearVelocity()) * VelocityDamper * Chunk->getMass() * DepthMultiplier;

				//Wave push force
				if (EnableWaveForces)
				{
					float waveVelocity = FMath::Clamp(P2UVector(Chunk->getLinearVelocity()).Z, -20.f, 150.f) * (1 - DepthMultiplier);
					DampingForce += FVector(OceanManager->GlobalWaveDirection.X, OceanManager->GlobalWaveDirection.Y, 0) * Chunk->getMass() * waveVelocity * WaveForceMultiplier;
				}

				//Add force for this chunk
				Chunk->addForce(PxVec3(DampingForce.X, DampingForce.Y, DampingForce.Z + BuoyancyForceZ), PxForceMode::eFORCE, true);
			}

			if (DrawDebugPoints)
			{
				FColor DebugColor = FLinearColor(0.8, 0.7, 0.2, 0.8).ToRGBE();
				if (isUnderwater) { DebugColor = FLinearColor(0, 0.2, 0.7, 0.8).ToRGBE(); } //Blue color underwater, yellow out of watter
				DrawDebugSphere(GetWorld(), Location, TestPointRadius, 8, DebugColor);
			}

			//Advanced
			Chunk->setSleepThreshold(ChunkSleepThreshold);
			Chunk->setStabilizationThreshold(ChunkStabilizationThreshold);

			//Update damping based on isUnderwater
			Chunk->setLinearDamping(_baseLinearDamping + FluidLinearDamping * isUnderwater);
			Chunk->setAngularDamping(_baseAngularDamping + FluidAngularDamping * isUnderwater);

			//Clamp the chunk's velocity to MaxUnderwaterVelocity if chunk is underwater
			if (ClampMaxVelocity && isUnderwater
				&& Chunk->getLinearVelocity().magnitude() > MaxUnderwaterVelocity)
			{
				FVector	Velocity = P2UVector(Chunk->getLinearVelocity()).GetSafeNormal() * MaxUnderwaterVelocity;
				Chunk->setLinearVelocity(PxVec3(Velocity.X, Velocity.Y, Velocity.Z));
			}
		}
	}
#endif // WITH_PHYSX 

// 	for (const FName& BoneName : GetAllSocketNames())
// 	{
// 		FBodyInstance* ChunkBI = GetBodyInstance(BoneName);
// 
// 		if (!ChunkBI)
// 			continue;
// 
// 		FVector Location = ChunkBI->GetUnrealWorldTransform().GetLocation();
// 
// 		float waveHeight = OceanManager->GetWaveHeightValue(Location).Z;
// 		bool isUnderwater = false;
// 
// 		//ChunkBI->GetBodyMass()
// 
// 		//If test point radius is touching water add buoyancy force
// 		if (waveHeight > (Location.Z + _SignedRadius))
// 		{
// 			isUnderwater = true;
// 
// 			float DepthMultiplier = (waveHeight - (Location.Z + _SignedRadius)) / (TestPointRadius * 2);
// 			DepthMultiplier = FMath::Clamp(DepthMultiplier, 0.f, 1.f);
// 
// 			/**
// 			* --------
// 			* Buoyancy force formula: (Volume(Mass / Density) * Fluid Density * -Gravity) * Depth Multiplier
// 			* --------
// 			*/
// 			float BuoyancyForceZ = ChunkBI->GetBodyMass() / ChunkDensity * FluidDensity * -Gravity * DepthMultiplier;
// 
// 			//Velocity damping
// 			FVector DampingForce = ChunkBI->GetUnrealWorldVelocity() * VelocityDamper * ChunkBI->GetBodyMass() * DepthMultiplier;
// 
// 			//Wave push force
// 			if (EnableWaveForces)
// 			{
// 				float waveVelocity = FMath::Clamp(ChunkBI->GetUnrealWorldVelocity().Z, -20.f, 150.f) * (1 - DepthMultiplier);
// 				DampingForce += OceanManager->WaveDirection * ChunkBI->GetBodyMass() * waveVelocity * WaveForceMultiplier;
// 			}
// 
// 			//Add force for this chunk
// 			ChunkBI->AddForce(FVector(DampingForce.X, DampingForce.Y, DampingForce.Z + BuoyancyForceZ));
// 		}
// 
// 		if (DrawDebugPoints)
// 		{
// 			FColor DebugColor = FLinearColor(0.8, 0.7, 0.2, 0.8).ToRGBE();
// 			if (isUnderwater) { DebugColor = FLinearColor(0, 0.2, 0.7, 0.8).ToRGBE(); } //Blue color underwater, yellow out of watter
// 			DrawDebugSphere(GetWorld(), Location, TestPointRadius, 8, DebugColor);
// 		}
// 
// 		//Advanced
// 		//Chunk->setSleepThreshold(ChunkSleepThreshold);
// 		//Chunk->setStabilizationThreshold(ChunkStabilizationThreshold);
// 
// 		//Update damping based on isUnderwater
// 		ChunkBI->LinearDamping = _baseLinearDamping + FluidLinearDamping * isUnderwater;
// 		ChunkBI->AngularDamping = _baseAngularDamping + FluidAngularDamping * isUnderwater;
// 		ChunkBI->UpdateDampingProperties();
// 
// 		//Clamp the chunk's velocity to MaxUnderwaterVelocity if chunk is underwater
// 		if (ClampMaxVelocity && isUnderwater
// 			&& ChunkBI->GetUnrealWorldVelocity().Size() > MaxUnderwaterVelocity)
// 		{
// 			FVector	Velocity = ChunkBI->GetUnrealWorldVelocity().GetSafeNormal() * MaxUnderwaterVelocity;
// 			ChunkBI->SetLinearVelocity(Velocity, false);
// 		}
// 	}
}
