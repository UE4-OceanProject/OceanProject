#include "OceanPluginPrivatePCH.h"
#include "BuoyantDestructible.h"

// PhysX 			
#include "PhysXIncludes.h" 
#include "PhysicsPublic.h"

ABuoyantDestructible::ABuoyantDestructible(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true; //Duh!

	//Defaults
	ChunkDensity = 600.0f;
	FluidDensity = 1025.0f;
	TestPointRadius = 10.0f;
	FluidLinearDamping = 1.0f;
	FluidAngularDamping = 1.0f;

	VelocityDamper = FVector(0.1, 0.1, 0.1);
	MaxUnderwaterVelocity = 1000.f;

	ChunkSleepThreshold = 50.0f;
	ChunkStabilizationThreshold = 10.0f;

	WaveForceMultiplier = 2.0f;
}

void ABuoyantDestructible::PostLoad()
{
	Super::PostLoad();

	// If no OceanManager is defined auto-detect
	if (!OceanManager)
	{
		for (TActorIterator<AOceanManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			OceanManager = Cast<AOceanManager>(*ActorItr);
			break;
		}
	}

	_baseLinearDamping = DestructibleComponent->GetLinearDamping();
	_baseAngularDamping = DestructibleComponent->GetAngularDamping();
}

void ABuoyantDestructible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OceanManager)
		return;

	float Gravity = DestructibleComponent->GetPhysicsVolume()->GetGravityZ();

	TestPointRadius = abs(TestPointRadius);

	//Signed based on gravity, just in case we need an upside down world
	_SignedRadius = FMath::Sign(Gravity) * TestPointRadius;

#if WITH_PHYSX
	uint32 ChunkCount = DestructibleComponent->ApexDestructibleActor->getNumVisibleChunks();
	const uint16* ChunkIndices = DestructibleComponent->ApexDestructibleActor->getVisibleChunks();
	for (uint32 c = 0; c < ChunkCount; c++)
	{
		PxRigidDynamic* Chunk = DestructibleComponent->ApexDestructibleActor->getChunkPhysXActor(ChunkIndices[c]);
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
					DampingForce += OceanManager->WaveDirection * Chunk->getMass() * waveVelocity * WaveForceMultiplier;
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
}