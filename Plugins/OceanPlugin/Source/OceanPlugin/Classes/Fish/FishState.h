/*=================================================
* FileName: FishState.h
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
#include "Fish/FlockFish.h"


class AFlockFish;

/**
 * 
 */

/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish State		          /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////
class FishState
{
protected:
	AFlockFish* Fish;

public:
	// Constructors
	FishState(){}
	FishState(AFlockFish* aFish)
	{
		Fish = aFish;
	};
	virtual void Update(float val){};
	virtual void HandleOverlapEvent(AActor *otherActor, UPrimitiveComponent *otherComponent, FString aColliderString){};
};


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Seek State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

class SeekState : public FishState
{
public:

	SeekState(AFlockFish* aFish) : FishState(aFish){};
	virtual void Update(float delta) override;

protected:

	virtual void SeekTarget(float delta);
	virtual void Flock(float delta);
};


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Flee State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

class FleeState : public FishState
{
protected:
	AActor* Enemy;

public:

	FleeState(AFlockFish* aFish, AActor* aEnemy) : FishState(aFish)
	{
		Enemy = aEnemy;
	};
	virtual void Update(float delta) override;

protected:

	virtual void FleeFromEnemy(float delta);

};


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Chase State            /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

class ChaseState : public FishState
{
protected:
	AActor* Prey;

public:

	ChaseState(AFlockFish* aFish, AActor* aPrey) : FishState(aFish)
	{
		Prey = aPrey;
	};
	virtual void Update(float delta) override;

protected:

	virtual void ChasePrey(float delta);
	virtual void EatPrey();
};
