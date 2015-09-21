
#include "BandK.h"
#include "BuoyantMeshVertex.h"


bool FBuoyantMeshVertex::IsUnderwater() const
{
	return Height < 0.f;
}

FBuoyantMeshVertex::FBuoyantMeshVertex(FVector Position, float HeightAboveWater) : Position{ Position }, Height{HeightAboveWater}
{

}

FBuoyantMeshVertex::FBuoyantMeshVertex()
{

}

