// Fill out your copyright notice in the Description page of Project Settings.

#include "OceanPluginPrivatePCH.h"
#include "Fish/FishManager.h"
#include "Fish/FlockFish.h"


AFishManager::AFishManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFishManager::Tick(float val)
{
	setup();

	if (attachToPlayer)
	{
		moveToPlayer();
	}
}

void AFishManager::moveToPlayer()
{
	if (player)
		this->SetActorLocation(player->GetActorLocation());
}

float AFishManager::getMinZ()
{
	return minZ;
}

float AFishManager::getMaxZ()
{
	return maxZ;
}

void AFishManager::setup()
{
	if (isSetup == false){
		if (!areFishSpawned)
		{
			maxX = GetActorLocation().X + underwaterBoxLength;
			maxY = GetActorLocation().Y + underwaterBoxLength;
			minX = GetActorLocation().X - underwaterBoxLength;
			minY = GetActorLocation().Y - underwaterBoxLength;

			UWorld* const world = GetWorld();
			int numFlocks = flockTypes.Num();
			for (int i = 0; i < numFlocks; i++)
			{
				FVector spawnLoc = FVector(FMath::FRandRange(minX, maxX), FMath::FRandRange(minY, maxY), FMath::FRandRange(minZ, maxZ));
				AFlockFish *leaderFish = NULL;
				for (int j = 0; j < numInFlock[i]; j++)
				{
					AFlockFish *aFish = Cast<AFlockFish>(world->SpawnActor(flockTypes[i]));
					aFish->isLeader = false;
					aFish->DebugMode = DebugMode;
					aFish->underwaterMax = FVector(maxX, maxY, maxZ);
					aFish->underwaterMin = FVector(minX, minY, minZ);
					aFish->underwaterBoxLength = underwaterBoxLength;
					spawnLoc = FVector(FMath::FRandRange(minX, maxX), FMath::FRandRange(minY, maxY), FMath::FRandRange(minZ, maxZ));
					if (j == 0)
					{
						aFish->isLeader = true;
						leaderFish = aFish;
					}
					else if (leaderFish != NULL)
					{	
						aFish->leader = leaderFish;
					}
					aFish->SetActorLocation(spawnLoc);
				}
			}
			areFishSpawned = true;
		}

		if (attachToPlayer)
		{
			TArray<AActor*> aPlayerList;
			UGameplayStatics::GetAllActorsOfClass(this, playerType, aPlayerList);
			if (aPlayerList.Num() > 0)
			{	
				player = aPlayerList[0];
				isSetup = true;
			}
		} 
		else
		{
			isSetup = true;
		}

	}
}
