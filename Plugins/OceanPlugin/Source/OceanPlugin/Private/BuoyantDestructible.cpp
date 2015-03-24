#include "OceanPluginPrivatePCH.h"
#include "BuoyantDestructible.h"
 
// PhysX 			
#include "PhysXIncludes.h" 
#include "PhysicsPublic.h"
 
ABuoyantDestructible::ABuoyantDestructible(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true; //Duh!

	TestPointRadius = 10.0f;
	Buoyancy = 1.0f;

	ChunkSleepThreshold = 50.0f;
	ChunkStabilizationThreshold = 10.0f;
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

	#if WITH_PHYSX
	for(FDestructibleChunkInfo& Each : DestructibleComponent->ChunkInfos)
	{
		physx::PxRigidDynamic* Chunk = Each.Actor;
 
		if (Chunk)
		{    
			PxTransform Trans = Chunk->getGlobalPose();
			PxTransform MassTrans = Chunk->getCMassLocalPose();
			PxVec3 PxLoc = Trans.p + Trans.rotate(MassTrans.p);

			FVector Location = P2UVector(PxLoc);

			if (DrawDebugSpheres)
				DrawDebugSphere(GetWorld(), Location, TestPointRadius, 8, FColor::Blue);

			FVector waveHeight = OceanManager->GetWaveHeightValue(Location);

			float delta = fabs(Location.Z - waveHeight.Z) / TestPointRadius;
			float force = 0;
			if (Location.Z < waveHeight.Z)
			{
				force = Buoyancy / 100 * -Gravity;

				if (delta >= 1)
				{
					force = force;
				}
				else
				{
					force = (force * delta * delta * delta);
				}

				Chunk->addForce(PxVec3(0, 0, force) * Chunk->getMass(), PxForceMode::eIMPULSE, true);
			}

			Chunk->setSleepThreshold(ChunkSleepThreshold);
			Chunk->setStabilizationThreshold(ChunkStabilizationThreshold);

			Chunk->setLinearDamping(_baseLinearDamping);
			Chunk->setAngularDamping(_baseAngularDamping);

			//Apply Directional Damping
			//PxVec3 rotatedDamping = Trans.q.rotate(PxVec3(DirectionalDamping.X, DirectionalDamping.Y, DirectionalDamping.Z)).abs();
			PxVec3 rotatedDamping = Trans.rotate(PxVec3(DirectionalDamping.X, DirectionalDamping.Y, DirectionalDamping.Z)).abs();
			FVector conv = P2UVector(Chunk->getLinearVelocity()) * P2UVector(rotatedDamping);

			Chunk->addForce(-PxVec3(conv.X, conv.Y, conv.Z), PxForceMode::eIMPULSE, true);
		}
	}
	#endif // WITH_PHYSX 
}