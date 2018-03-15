/*=================================================
* FileName: FlockFish.h
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

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Fish/FishManager.h"
#include "FlockFish.generated.h"


/**
 * 
 */
UCLASS()
class AFlockFish : public APawn
{
	GENERATED_BODY()

	// Current state of the Fish
	class FishState* currentState;

	/** Static mesh component */
	class UStaticMeshComponent* base;

	/** Fish interaction sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* FishInteractionSphere;

public:
	// Is this fish a leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	bool isLeader;

	// Enemies that fish will flee from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	TArray<UClass*> enemyTypes;

	// Prey that the fish will attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	TArray<UClass*> preyTypes;

	// Neighbor type of the fish ( typically itself, but needed for code atm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	UClass* neighborType;

	// Distance to that fish will try to remain behind leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float followDist;

	// Defualt Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float speed;

	// Max Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float maxSpeed;

	// Speed at which the fish turns (Try to have predators be slightly faster)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float turnSpeed;

	// Frequency of turning ( turn every "turnFrequency" seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float turnFrequency;

	// time after eating until it is hungry again
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float hungerResetTime;

	// Go faster when fish is this distance behind leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float distBehindSpeedUpRange;

	// Multiplies With Radius of Fish Interaction Sphere for neighbor seperation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float SeperationDistanceMultiplier;

	// Multiplies With Radius of Fish Interaction Sphere for Flee Distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float FleeDistanceMultiplier;

	// Multiplies with delta time since last tick when lerping to max speed (While Fleeing)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float FleeAccelerationMultiplier;

	// Multiplies with delta time since last tick when lerping to max speed (While Chasing)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float ChaseAccelerationMultiplier;

	// Multiplies with delta time since last tick when lerping to regular speed (While Seeking)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float SeekDecelerationMultiplier;

	// Avoid Distance Multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float AvoidForceMultiplier;

	// Avoidance force
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float AvoidanceForce;

	// Player the fish will avoid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	UClass* playerType;

	// volume that is considered underwater
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	FVector underwaterMin;

	// volume that is considered underwater
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	FVector underwaterMax;

	// Custom Z range for targeting (NULL will use full range of min/max)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	float CustomZSeekMin;

	// Custom Z range for targeting (NULL will use full range of min/max)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	float CustomZSeekMax;

	// Max number of neighbors to evaluate on a Tick (0 for all neighbors)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization")
	float NumNeighborsToEvaluate;

	// When set to 0, update every tick, otherwise update after specified time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization")
	float UpdateEveryTick;

	// This is the target the fish will path to
	FVector target;

	float InteractionSphereRadius;

	// Run fish in debug mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TESTING")
	bool DebugMode;

	// Flee distance
	// NOTE: CURRENTLY SET IN CODE
	float fleeDistance;

	// Weight to multiply with cohesion
	// NOTE: CURRENTLY SET IN CODE
	float neighborSeperation;

	// current speed
	float curSpeed;

	// This fish's leader
	AActor *leader;

	// current prey in world
	//TArray<AActor*> prey;

	// Enemies that fish will flee from
	//TArray<AActor*> enemies;

	// These are the fish's flocking buddies
	TArray<AActor*> neighbors;

	// Nearby Enemies
	TArray<AActor*> nearbyEnemies;

	// Nearby Prey
	TArray<AActor*> nearbyPrey;

	// Nearby Friends (non-threatning fish and neighbors)
	TArray<AActor*> nearbyFriends;

	// Flee target
	AActor *fleeTarget;

	// current prey
	AActor *preyTarget;

	// is fish currently fleeing
	// Current state of the Fish
	bool isFleeing;

	// is the fish currently full?
	bool isFull;

	/* bounds of underwater box*/
	float underwaterBoxLength;

	FVector AvoidObstacle();

	float AvoidanceDistance;

	// Max bounds of box
	float maxX;
	float maxY;
	float maxZ;
	float minX;
	float minY;
	float minZ;

	/** Constructor */
	AFlockFish(const FObjectInitializer& ObjectInitializer);

	// Getters and Setters
	FVector getSeekTarget()
	{
		return target;
	}

	void setState(class FishState* newState)
	{
		currentState = newState;
	}

	void setRotation(FRotator newRotation)
	{
		curRotation = newRotation;
	};

	void setVelocity(FVector newVelocity)
	{
		curVelocity = newVelocity;
	};

	FRotator getRotation()
	{
		return curRotation;
	};

	FVector getVelocity()
	{
		return curVelocity;
	};

protected:
	/** Overrided tick function */
	virtual void Tick(float delta) override;

	/* setup the game on first tick */
	void Setup();

	/* Manage the various necessary timers*/
	void ManageTimers(float delta);

	/* Choose what state to be in*/
	void ChooseState();

	/* Move Bounds */
	void MoveBounds(float delta);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* activatedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* activatedComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	/* update the current state*/
	void UpdateState(float delta);

	/* places the target randomly within the postprocessvolume*/
	void spawnTarget();

	/* Does a bunch of debug stuff if debug mode is active*/
	void Debug();

	// current velocity
	FVector curVelocity;

	// current rotation
	FRotator curRotation;

	// turning timer
	float turnTimer;

	// Are the array's setup?
	bool isSetup;

	// current hunger timer
	float hungerTimer;

	// Tick Timer
	float updateTimer;

	// player holder
	//AActor* player;

	// has fish manager?
	bool hasFishManager;

	// fish manager
	AActor* fishManager;


};
