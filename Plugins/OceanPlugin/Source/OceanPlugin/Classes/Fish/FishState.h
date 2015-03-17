// Kyle Bryant @Komdoman
// Use wherever your heart desires <3


#pragma once

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
