/*=================================================
* FileName: FlockFish.cpp
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

#include "Fish/FlockFish.h"
#include "Fish/FishState.h"
#include "Fish/FishManager.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


#define COLLISION_TRACE ECC_GameTraceChannel4

AFlockFish::AFlockFish(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	base = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("FishMesh"));
	RootComponent = base;

	// Fish interaction sphere
	FishInteractionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("FishInteractionSphere"));
	FishInteractionSphere->SetSphereRadius(10);
	FishInteractionSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	FishInteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlockFish::OnBeginOverlap);
	FishInteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AFlockFish::OnEndOverlap);

	if (isLeader == true)
	{
		spawnTarget();
	}

	//Defaults
	followDist = 50.0;
	speed = 1200.0;
	maxSpeed = 2400.0;
	turnSpeed = 3.0;
	turnFrequency = 1.0;
	turnFrequency = 1.0;
	hungerResetTime = 20.0;
	distBehindSpeedUpRange = 3000.0;
	SeperationDistanceMultiplier = 0.75;
	FleeDistanceMultiplier = 5.0;
	FleeAccelerationMultiplier = 2.0;
	ChaseAccelerationMultiplier = 2.0;
	SeekDecelerationMultiplier = 1.0;
	AvoidForceMultiplier = 1.0;
	AvoidanceForce = 20000.0;
	underwaterMin = FVector(-40000, -40000, -9000);
	underwaterMax = FVector(40000, 40000, -950);
	CustomZSeekMin = 0.0;
	CustomZSeekMax = 0.0;
	NumNeighborsToEvaluate = 5.0;
	UpdateEveryTick = 0.0;
	DebugMode = true;
	fleeDistance = 0.0;
	neighborSeperation = 300.0;
	curSpeed = speed;
	isFleeing = false;
	isFull = false;
	underwaterBoxLength = 10000.0;
	AvoidanceDistance = 5000.0;
	curVelocity = FVector(0, 0, 0);
	curRotation = FRotator(0, 0, 0);
	turnTimer = 0.0;
	isSetup = false;
	hungerTimer = 0.0;
	updateTimer = 0.0;
	hasFishManager = false;
}

void AFlockFish::Tick(float delta)
{

	// Setup the fish (happens on first tick only)
	// NOTE TO SELF: consider creating a beginplay event that does this stuff (although beginplay is buggy as hell x.x)
	Setup();

	// If debug mode true, draw interaction sphere and avoiddistance
	Debug();

	// Move Bounds based on location of FishManager (if applicable)
	MoveBounds(delta);

	// Manage Timers (hungerTimer, updateTimer, and turnTimer)
	ManageTimers(delta);

	// Decide what state to be in
	ChooseState();

	// Update curVelocity and curRotation through current state
	UpdateState(delta);

	// Update world rotation and velocity
	this->SetActorRotation(curRotation);
	this->AddActorWorldOffset(curVelocity);

	Super::Tick(delta);
}

void AFlockFish::Debug()
{
	if (DebugMode)
	{
		FVector actorLocation = this->GetActorLocation();
		FVector forwardVector = (this->GetActorForwardVector() * AvoidanceDistance) + actorLocation;
		FVector forwardVector2 = (this->GetActorForwardVector() * (AvoidanceDistance * 0.1)) + actorLocation;

		DrawDebugLine(
			GetWorld(),
			actorLocation,
			forwardVector,
			FColor::Magenta,
			false, -1, 0,
			10
			);

		FColor indicatorColor = FColor::Cyan;
		if (nearbyEnemies.IsValidIndex(0))
		{
			indicatorColor = FColor::Red;
		}
		else if (nearbyPrey.IsValidIndex(0) && isFull == false)
		{
			indicatorColor = FColor::Green;
		}
		DrawDebugSphere(
			GetWorld(),
			actorLocation,
			FishInteractionSphere->GetScaledSphereRadius(),
			20,
			indicatorColor
			);
		DrawDebugLine(
			GetWorld(),
			actorLocation,
			forwardVector2,
			indicatorColor,
			true, 10, 0,
			20
			);
	}

}

FVector AFlockFish::AvoidObstacle()
{
	FVector actorLocation = this->GetActorLocation();
	FVector forwardVector = (this->GetActorForwardVector() * AvoidanceDistance) + actorLocation;

	FHitResult OutHitResult;
	FCollisionQueryParams Line(FName("Collision param"), true);
	bool const bHadBlockingHit = GetWorld()->LineTraceSingleByChannel(OutHitResult, actorLocation, forwardVector, COLLISION_TRACE, Line);
	FVector returnVector = FVector(0, 0, 0);
	float distanceToBound = distanceToBound = (this->GetActorLocation() - OutHitResult.ImpactPoint).Size();
	if (bHadBlockingHit)
	{
		if (OutHitResult.ImpactPoint.Z > this->GetActorLocation().Z + FishInteractionSphere->GetScaledSphereRadius())
		{	
			returnVector.Z += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * -1;
		}
		else if (OutHitResult.ImpactPoint.Z < this->GetActorLocation().Z - FishInteractionSphere->GetScaledSphereRadius())
		{
			returnVector.Z += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * 1;
		}

		if (OutHitResult.ImpactPoint.X > this->GetActorLocation().X)
		{
			returnVector.X += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * -1;
		}
		else if (OutHitResult.ImpactPoint.X < this->GetActorLocation().X)
		{
			
			returnVector.X += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * 1;
		}

		if (OutHitResult.ImpactPoint.Y > this->GetActorLocation().Y)
		{
			returnVector.Y += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * -1;
		}
		else if (OutHitResult.ImpactPoint.Y < this->GetActorLocation().Y)
		{

			returnVector.Y  += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * 1;
		}

		returnVector.Normalize();
		FVector avoidance = returnVector * AvoidanceForce;
		return avoidance;
	}
	return FVector(0, 0, 0);
}

void AFlockFish::UpdateState(float delta)
{
	if (UpdateEveryTick == 0)
	{
		currentState->Update(delta);
	}
	else if (updateTimer >= UpdateEveryTick)
	{
		currentState->Update(delta);
		updateTimer = 0;
	}
}


void AFlockFish::OnBeginOverlap(UPrimitiveComponent* activatedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	// Is overlapping with enemy?
	if (enemyTypes.Find(otherActor->GetClass()) >= 0)
	{	
		nearbyEnemies.Add(otherActor);
	}
	else if (preyTypes.Find(otherActor->GetClass()) >= 0)
	{	
		if (otherActor->GetClass() == this->GetClass())
		{
			if (!Cast<AFlockFish>(otherActor)->isLeader)
			{
				nearbyPrey.Add(otherActor);
			}
		}
		else
		{
			nearbyPrey.Add(otherActor);
		}
	}
	else if (otherActor->GetClass() == this->GetClass())
	{
		nearbyFriends.Add(otherActor);
	}
}

void AFlockFish::OnEndOverlap(UPrimitiveComponent* activatedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (nearbyEnemies.Find(otherActor) >= 0)
	{
		nearbyEnemies.Remove(otherActor);
	}
	else if (nearbyPrey.Find(otherActor) >= 0)
	{
		nearbyPrey.Remove(otherActor);
	}
	else if (nearbyFriends.Find(otherActor) >= 0)
	{
		nearbyFriends.Remove(otherActor);
	}
}

void AFlockFish::ChooseState()
{
	if (nearbyEnemies.IsValidIndex(0))
	{
		currentState = new FleeState(this, nearbyEnemies[0]);
	}
	else if (nearbyPrey.IsValidIndex(0) && isFull == false)
	{
		currentState = new ChaseState(this, nearbyPrey[0]);
	}
	else
	{
		currentState = new SeekState(this);
	}
}

void AFlockFish::ManageTimers(float delta)
{
	// Check if the fish is full or not
	if (isFull)
	{
		hungerTimer += delta;

		if (hungerTimer >= hungerResetTime)
		{
			hungerTimer = 0.0f;
			isFull = false;
		}
	}

	// decide on wether to move target this tick
	if (turnTimer >= turnFrequency && isLeader == true)
	{
		spawnTarget();
		turnTimer = 0.0;
	}

	updateTimer += delta;
	turnTimer += delta;
}



void AFlockFish::MoveBounds(float delta)
{
	if (hasFishManager)
	{
		FVector fishManagerPosition = fishManager->GetActorLocation();
		maxX = fishManagerPosition.X + underwaterBoxLength;
		minX = fishManagerPosition.X - underwaterBoxLength;
		maxY = fishManagerPosition.Y + underwaterBoxLength;
		minY = fishManagerPosition.Y - underwaterBoxLength;

		FVector actorLocation = this->GetActorLocation();
		if (actorLocation.Z > underwaterMax.Z)
		{	
			actorLocation.Z = underwaterMin.Z + FMath::Abs((0.999 * underwaterMax.Z));
		}
		else if (actorLocation.Z < underwaterMin.Z)
		{
			actorLocation.Z = underwaterMin.Z + FMath::Abs((0.001 * underwaterMax.Z));
		}

		if (actorLocation.X > maxX)
		{
			actorLocation.X = minX + FMath::Abs((0.1 * maxX));
		}
		else if (actorLocation.X < minX)
		{
			actorLocation.X = maxX - FMath::Abs((0.1 * maxX));
		}

		if (actorLocation.Y > maxY)
		{
			actorLocation.Y = minY + FMath::Abs((0.1 * maxY));
		}
		else if (actorLocation.Y < minY)
		{
			actorLocation.Y = maxY - FMath::Abs((0.1 * maxY));
		}

		this->SetActorLocation(actorLocation);
	}
}

void AFlockFish::spawnTarget()
{
	target = FVector(FMath::FRandRange(minX, maxX), FMath::FRandRange(minY, maxY), FMath::FRandRange(minZ, maxZ));
}


void AFlockFish::Setup()
{
	// Setup the enemies list on first tick
	if (isSetup == false)
	{
		maxX = underwaterMax.X;
		maxY = underwaterMax.Y;
		minX = underwaterMin.X;
		minY = underwaterMin.Y;

		InteractionSphereRadius = FishInteractionSphere->GetScaledSphereRadius();

		if (CustomZSeekMax == 0.0)
		{
			minZ = underwaterMin.Z;
			maxZ = underwaterMax.Z;
		}
		else
		{
			minZ = CustomZSeekMin;
			maxZ = CustomZSeekMax;
		}

		fleeDistance = FishInteractionSphere->GetScaledSphereRadius() * FleeDistanceMultiplier;
		neighborSeperation = FishInteractionSphere->GetScaledSphereRadius() * SeperationDistanceMultiplier;
		AvoidanceDistance = FishInteractionSphere->GetScaledSphereRadius() * 2;

		currentState = new SeekState(this);

		TArray<AActor*> aFishManagerList;
		UGameplayStatics::GetAllActorsOfClass(this, AFishManager::StaticClass(), aFishManagerList);
		if (aFishManagerList.Num() > 0)
		{
			hasFishManager = true;
			fishManager = aFishManagerList[0];
		}

		// Setup Neighbors
		if (!fishManager)
		{
			TArray<AActor*> aNeighborList;
			UGameplayStatics::GetAllActorsOfClass(this, neighborType, aNeighborList);
			neighbors.Append(aNeighborList);
			for (int i = 0; i < neighbors.Num(); i++)
			{
				if (Cast<AFlockFish>(neighbors[i])->isLeader)
				{
					leader = neighbors[i];
					break;
				}
			}
		}
		//nearbyFriends.Append(neighbors);


		isSetup = true;
	}
}
