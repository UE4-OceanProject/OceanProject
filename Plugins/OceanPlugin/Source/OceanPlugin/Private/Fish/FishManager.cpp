/*=================================================
* FileName: FishManager.cpp
* 
* Created by: Komodoman
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/03/17
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

#include "Fish/FishManager.h"
#include "Fish/FlockFish.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


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
