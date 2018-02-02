/*=================================================
* FileName: FishState.cpp
* 
* Created by: Komodoman
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/03/17
*
* Last Edited on: 2018/01/30
* Last Edited by: SaschaElble
* 
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#include "Fish/FishState.h"
#include "Fish/FlockFish.h"

/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Seek State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

void SeekState::Update(float delta)
{	
	Fish->isFleeing = false;
	Fish->fleeTarget = NULL;
	Fish->preyTarget = NULL;

	if (Fish->isLeader)
	{
		// Seek target
		SeekTarget(delta);
	}
	else
	{
		// School with buddies
		Flock(delta);
	}
}

void SeekState::SeekTarget(float delta)
{	
	// Set Speed
	Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->speed, delta * Fish->SeekDecelerationMultiplier);

	// Set Rotation 
	FVector targetRotation = (Fish->getSeekTarget() - Fish->GetActorLocation() + Fish->AvoidObstacle());
	FRotator leaderRotation = FRotationMatrix::MakeFromX(targetRotation).Rotator();
	leaderRotation = FMath::RInterpTo(Fish->GetActorRotation(), leaderRotation, delta, Fish->turnSpeed);
	Fish->setRotation(leaderRotation);

	// Set Velocity Vector
	FVector leaderVelocity = Fish->GetActorForwardVector() * (delta * Fish->curSpeed);
	Fish->setVelocity(leaderVelocity);
}

void SeekState::Flock(float delta)
{
	// Get a list of Fish neighbors and calculate seperation
	FVector seperation = FVector(0, 0, 0);
	if (Fish->nearbyFriends.IsValidIndex(0))
	{
		TArray<AActor*> neighborList = Fish->nearbyFriends;
		int neighborCount = 0;
		for (int i = 0; i < Fish->nearbyFriends.Num(); i++)
		{
			if (neighborList.IsValidIndex(i))
			{
				seperation += neighborList[i]->GetActorLocation() - Fish->GetActorLocation();
				neighborCount++;
			}

			if (i == Fish->NumNeighborsToEvaluate && i != 0)
			{
				break;
			}
		}
		seperation = ((seperation / neighborCount) * -1);
		seperation.Normalize();
		seperation *= Fish->neighborSeperation;
	}

	// Maintain distance behind Leader
	FVector distBehind = (Cast<AFlockFish>(Fish->leader)->getVelocity() * -1);
	distBehind.Normalize();
	distBehind *= Fish->followDist;

	// Calculate all seperation and distance behind leader into one vector
	FVector leaderLocation = Fish->leader->GetActorLocation();
	FVector flockerVelocity = distBehind + leaderLocation + seperation + Fish->AvoidObstacle();
	FRotator flockerRotation = FRotationMatrix::MakeFromX(flockerVelocity - Fish->GetActorLocation()).Rotator();

	// If fish is too far behind leader, speed up 
	float newSpeed = Fish->speed;
	if (Fish->GetDistanceTo(Fish->leader) > Fish->distBehindSpeedUpRange)
	{
		// Set Speed
		Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->maxSpeed, delta);
	}
	else
	{
		// Set Speed
		Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->speed, delta);
	}


	// Set Velocity
	FVector flockVelocity = Fish->GetActorForwardVector() * (delta * Fish->curSpeed);
	Fish->setVelocity(flockVelocity);

	// Set Rotation
	FRotator flockRotation = FMath::RInterpTo(Fish->GetActorRotation(), flockerRotation, delta, Fish->turnSpeed);
	Fish->setRotation(flockRotation);
}


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Flee State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

void FleeState::Update(float delta)
{
	Fish->isFleeing = true;
	Fish->fleeTarget = Enemy;
	Fish->preyTarget = NULL;
	if (Fish->GetDistanceTo(Enemy) >= Fish->fleeDistance)
	{
		Fish->setState(new SeekState(Fish));
	}
	FleeFromEnemy(delta);
}

void FleeState::FleeFromEnemy(float delta)
{
	// Set Speed
	Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->maxSpeed, (delta * Fish->FleeAccelerationMultiplier));
	
	// Set Velocity
	FVector fleeVelocity = Fish->GetActorForwardVector() * (delta * Fish->curSpeed);
	Fish->setVelocity(fleeVelocity);

	// Set Rotation
	FVector targetRotation = (Fish->GetActorLocation() - Enemy->GetActorLocation()) + Fish->AvoidObstacle();
	FRotator fleeRotation = FRotationMatrix::MakeFromX(targetRotation).Rotator();
	fleeRotation = FMath::RInterpTo(Fish->GetActorRotation(), fleeRotation, delta, Fish->turnSpeed);
	Fish->setRotation(fleeRotation);

}

/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Chase State            /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

void ChaseState::Update(float delta)
{
	Fish->isFleeing = false;
	Fish->fleeTarget = NULL;
	Fish->preyTarget = Prey;
	EatPrey();
	ChasePrey(delta);
}

void ChaseState::EatPrey()
{
	if (Fish->GetDistanceTo(Prey) < 1000)
	{
		float zLoc = Fish->minZ + FMath::Abs(0.25 * Fish->maxZ);
		Prey->SetActorLocation(FVector(FMath::FRandRange(Fish->minX, Fish->maxX), FMath::FRandRange(Fish->minY, Fish->maxX), zLoc));
		Fish->isFull = true;
		Fish->setState(new SeekState(Fish));
	}
}

void ChaseState::ChasePrey(float delta)
{
	// Set Speed
	Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->maxSpeed, (delta * Fish->ChaseAccelerationMultiplier));

	// Set Velocity
	FVector chaseVelocity = Fish->GetActorForwardVector() * (delta * Fish->curSpeed);
	Fish->setVelocity(chaseVelocity);

	// Set Rotation
	FVector seperation = FVector(0, 0, 0);
	if (Fish->nearbyFriends.IsValidIndex(0))
	{
		int neighborCount = 0;
		TArray<AActor*> neighborList = Fish->nearbyFriends;
		for (int i = 0; i < Fish->NumNeighborsToEvaluate; i++)
		{
			if (neighborList.IsValidIndex(i))
			{
				seperation += neighborList[i]->GetActorLocation() - Fish->GetActorLocation();
				neighborCount++;
			}
		}
		seperation = ((seperation / neighborCount) * -1);
		seperation.Normalize();
		seperation *= Fish->neighborSeperation;
	}

	FVector preyLocation = Prey->GetActorLocation();
	FVector flockerVelocity = ((preyLocation + seperation) - Fish->GetActorLocation()) + Fish->AvoidObstacle();
	FRotator flockerRotation = FRotationMatrix::MakeFromX(flockerVelocity).Rotator();
	FRotator chaseRotation = FMath::RInterpTo(Fish->GetActorRotation(), flockerRotation, delta, Fish->turnSpeed);
	Fish->setRotation(chaseRotation);
}
