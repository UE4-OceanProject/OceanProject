/*=================================================
* FileName: AdvancedBuoyancyComponent.cpp
*
* Created by: Burnrate (Justin Beales)
* Project name: OceanProject
* Unreal Engine version: 4.19
* Created on: 2017/01/01
*
* Last Edited on: 2018/03/15
* Last Edited by: Reapazor (Matthew Davey)
*
* -------------------------------------------------
* Created with Misc. Games and Intelligent Procedure for:
* Fishing: Barents Sea
* http://miscgames.no/
* http://www.IntelligentProcedure.com 
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#include "AdvancedBuoyancyComponent/AdvancedBuoyancyComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"	// According to UE4's IWYU Reference Guide, this is the correct Engine.h to include


// Constructor
UAdvancedBuoyancyComponent::UAdvancedBuoyancyComponent()
{
	// Tick
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(true);
	bAutoActivate = true;
	// Init please
	bWantsInitializeComponent = true;
}


// Initialize
void UAdvancedBuoyancyComponent::InitializeComponent()
{
	Super::InitializeComponent();
	World = GetWorld();
	GetOcean();
	CorrectedMeshDensity = MeshDensity / 1000000.f; // kg / m^3 to cm^3
	CorrectedWaterDensity = WaterDensity / 1000000.f; // kg / m^3 to cm^3
	Gravity = World->GetGravityZ();

	// Get the static mesh component we are checking for buoyancy and ensure it has a valid static mesh assigned
	if (!BuoyantMesh) { BuoyantMesh = Cast<UStaticMeshComponent>(GetAttachParent()); }
	if (!BuoyantMesh) { GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Mesh Missed"))); return; }
	if (!BuoyantMesh->GetStaticMesh()) { GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Mesh Mesh Missed"))); return; }

	// Check again to prevent crash incase was added to an actor without a mesh
	if (BuoyantMesh != nullptr) {
		BuoyantMesh->GetLocalBounds(MinBound, MaxBound);

		if (BuoyantMesh->LODData.Num() == 0) {
			BuoyantMesh->SetLODDataCount(1, BuoyantMesh->LODData.Num());
		}
		PopulateTrianglesFromStaticMesh();
	}

	ForceC = -CorrectedWaterDensity * Gravity;
}


// Called every frame
void UAdvancedBuoyancyComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// Advanced buoyancy using a static mesh triangles

	if (!TheOcean) {
		GetOcean();
		return;
	}

	AdvancedBuoyancy();

}


TArray<FForceTriangle> UAdvancedBuoyancyComponent::SplitTriangle(FBuoyancyVertex H, FBuoyancyVertex M, FBuoyancyVertex L, FVector OutArrow)
{
	TArray<FForceTriangle> ReturnTriangles;
	FBuoyancyVertex CutVertex; 
	CutVertex.Position = ((L.Position - H.Position) * (H.Position.Z - M.Position.Z) / (H.Position.Z - L.Position.Z) + H.Position); CutVertex.Depth = GetOceanDepthFromGrid(CutVertex.Position);

	ReturnTriangles.Emplace(L, CutVertex, M, OutArrow, true);
	ReturnTriangles.Last().Center.Position = (L.Position + CutVertex.Position + M.Position) / 3.f;
	ReturnTriangles.Last().Center.Depth = GetOceanDepthFromGrid(ReturnTriangles.Last().Center.Position);
	ReturnTriangles.Last().CanSetForce = true;

	SubmergedTris.Add(ReturnTriangles.Last());
	SubmergedVolume += SubmergedTris.Last().TriArea;

	ReturnTriangles.Emplace(H, CutVertex, M, OutArrow, false);
	ReturnTriangles.Last().Center.Position = (H.Position + CutVertex.Position + M.Position) / 3.f;
	ReturnTriangles.Last().Center.Depth = GetOceanDepthFromGrid(ReturnTriangles.Last().Center.Position);
	ReturnTriangles.Last().CanSetForce = true;

	SubmergedTris.Add(ReturnTriangles.Last());
	SubmergedVolume += SubmergedTris.Last().TriArea;

	return ReturnTriangles;
}

void UAdvancedBuoyancyComponent::ApplyForce(FForceTriangle TriForce)
{
	float FLDM = 1.f;  // Force Line Debug Multiplier

	if (TriForce.TriArea < .001f) { return; }
	// Static buoyancy

	// The range for this can be set in the initial mesh analysis
	float PitchBuoyancyReductionOffset = 1 - BuoyancyPitchReductionCoefficient * FMath::Clamp(BuoyantMesh->GetComponentTransform().InverseTransformPosition(TriForce.Center.Position).X + 400.f, 0.f, 800.f) / 800.f;
	TriForce.SetForce();

	FVector StaticBuoyancyForce = TriForce.Force * ForceC * FVector(1.f, 1.f, 1.f - BuoyancyReductionCoefficient) * FVector(1.f, 1.f, PitchBuoyancyReductionOffset) * DensityCorrectionModifier;

	if (!FMath::IsNearlyZero(StaticBuoyancyForce.Size())) {
		BuoyantMesh->AddForceAtLocation(StaticBuoyancyForce, TriForce.ForceCenter);
	}
	if (bDebugOn) {
		FLDM = .0001f;
		DrawDebugLine(World, TriForce.ForceCenter - StaticBuoyancyForce * FLDM, TriForce.ForceCenter - StaticBuoyancyForce * .1f * FLDM, FColor::Red, false, -1.f, 0, 4.f);
		DrawDebugLine(World, TriForce.ForceCenter - StaticBuoyancyForce * .1f * FLDM, TriForce.ForceCenter, FColor::Orange, false, -1.f, 0, 4.f);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Be careful with coefficients, if they are too high a reaction force will send the boat flying

	FVector FinalForce = FVector::ZeroVector;
	// Viscosity
	if (bUseDrag) {
		// Project velocity onto triangle plane using normal
		//ViscousDragCoefficient = .005f;
		FVector PointVelocity = BuoyantMesh->GetBodyInstance()->GetUnrealWorldVelocityAtPoint(TriForce.Center.Position);
		// Tangential Velocity = Point Velocity - (Point Velocity | Surface Normal) * Surface Normal
		FVector TangentialVelocity = PointVelocity - (PointVelocity | TriForce.Normal) * TriForce.Normal;
		// Viscous Drag = Triangle Area * Tangential Velocity * Viscous Coefficient
		FVector ViscousForce = -TangentialVelocity * TriForce.TriArea * ViscousDragCoefficient * FMath::Max(51.4444f - TangentialVelocity.Size(), 0.f); // max value corrects for turbulence
		FinalForce += ViscousForce;
		if (bDebugOn) {
			FLDM = .01f;
			DrawDebugLine(World, TriForce.Center.Position, TriForce.Center.Position + ViscousForce * .9f * FLDM, FColor::Red, false, -1.f, 0, 4.f);
			DrawDebugLine(World, TriForce.Center.Position + ViscousForce * .9f * FLDM, TriForce.Center.Position + ViscousForce * FLDM, FColor::Orange, false, -1.f, 0, 4.f);
		}
	}

	if (bUseDrag) {
		// Drag

		float RefV = 500.f;  // about 10 knots
		FVector PointVelocity = BuoyantMesh->GetBodyInstance()->GetUnrealWorldVelocityAtPoint(TriForce.Center.Position);
		if (FMath::IsNearlyZero(PointVelocity.Size())) { return; }
		float VelocityProjection = FVector::DotProduct(PointVelocity.GetSafeNormal(), TriForce.Normal);
		float SpeedModified = PointVelocity.Size() / RefV;
		
		FVector DragForce = FVector::ZeroVector;
		if (VelocityProjection >= 0) {
			// Push Drag
			DragForce = -TriForce.TriArea * FMath::Pow(FMath::Abs(VelocityProjection), DragCoefficient.Z) * TriForce.Normal * (DragCoefficient.X * SpeedModified + DragCoefficient.Y * FMath::Pow(SpeedModified, 2.f));

			if (bDebugOn) {
				FLDM = .003f;
				DrawDebugLine(World, TriForce.Center.Position - DragForce * FLDM, TriForce.Center.Position - DragForce * .1f * FLDM, FColor::Red, false, -1.f, 0, 4.f);
				DrawDebugLine(World, TriForce.Center.Position - DragForce * .1f * FLDM, TriForce.Center.Position, FColor::Orange, false, -1.f, 0, 4.f);
			}
		}
		else {
			// Pull Drag
			DragForce = TriForce.TriArea * FMath::Pow(FMath::Abs(VelocityProjection), SuctionCoefficient.Z) * TriForce.Normal * (SuctionCoefficient.X * SpeedModified + SuctionCoefficient.Y * FMath::Pow(SpeedModified, 2.f));

			if (bDebugOn) {
				FLDM = .003f;
				DrawDebugLine(World, TriForce.Center.Position, TriForce.Center.Position + DragForce * .9f * FLDM, FColor::Red, false, -1.f, 0, 4.f);
				DrawDebugLine(World, TriForce.Center.Position + DragForce * .9f * FLDM, TriForce.Center.Position + DragForce * FLDM, FColor::Green, false, -1.f, 0, 4.f);
			}
		}
		FinalForce += DragForce;
	}
	
	// Non static buoyancy forces are applied to the centroid of the triangle as they do not vary with depth (assuming a constant density)
	if (!FMath::IsNearlyZero(FinalForce.Size())) {
		BuoyantMesh->AddForceAtLocation(FinalForce, TriForce.Center.Position);
	}

}

void UAdvancedBuoyancyComponent::DrawDebugStuff(FForceTriangle TriForce, FColor DebugColor)
{
	if (FMath::IsNearlyZero(TriForce.TriArea)) { return; }

	float DebugLineSize = 5.f;
	// Debug tris (shrunk a little bit so no overlap)
	FVector HS = (TriForce.Center.Position - TriForce.B.Position).GetSafeNormal() * 15.f + TriForce.B.Position;
	FVector MS = (TriForce.Center.Position - TriForce.C.Position).GetSafeNormal() * 15.f + TriForce.C.Position;
	FVector LS = (TriForce.Center.Position - TriForce.A.Position).GetSafeNormal() * 15.f + TriForce.A.Position;
	DrawDebugPoint(World, HS, 10.f, FColor::Red, false, -1.f, 0);
	DrawDebugPoint(World, MS, 10.f, FColor::Green, false, -1.f, 0);
	DrawDebugPoint(World, LS, 10.f, FColor::Blue, false, -1.f, 0);
	DrawDebugLine(World, HS, MS, DebugColor, false, -1.f, 0, DebugLineSize);  // triforce outline
	DrawDebugLine(World, MS, LS, DebugColor, false, -1.f, 0, DebugLineSize);  // triforce outline
	DrawDebugLine(World, HS, LS, DebugColor, false, -1.f, 0, DebugLineSize);  // triforce outline
}

void UAdvancedBuoyancyComponent::PopulateTrianglesFromStaticMesh()
{
	int32 NumLODs = BuoyantMesh->GetStaticMesh()->RenderData->LODResources.Num();
	FStaticMeshLODResources& LODResource = BuoyantMesh->GetStaticMesh()->RenderData->LODResources[NumLODs - 1];

	int numIndices = LODResource.IndexBuffer.IndexBufferRHI->GetSize() / sizeof(uint16);
	uint16* Indices = new uint16[numIndices];
	int numVertices = LODResource.VertexBuffers.PositionVertexBuffer.VertexBufferRHI->GetSize() / (sizeof(float) * 3);
	float* Vertices = new float[numVertices * 3];


	FRawStaticIndexBuffer* IndexBuffer = &LODResource.IndexBuffer;
	uint16* Indices0 = Indices;
	FPositionVertexBuffer*  PositionVertexBuffer = &LODResource.VertexBuffers.PositionVertexBuffer;
	float* Vertices0 = Vertices;
	ENQUEUE_RENDER_COMMAND(GetMyBuffers)
		(
		[IndexBuffer, Indices0, PositionVertexBuffer, Vertices0](FRHICommandListImmediate& RHICmdList)

		{
			uint16* indices1 = (uint16*)RHILockIndexBuffer(IndexBuffer->IndexBufferRHI, 0, IndexBuffer->IndexBufferRHI->GetSize(), RLM_ReadOnly);
			float* indices2 = (float*)RHILockVertexBuffer(PositionVertexBuffer->VertexBufferRHI, 0, PositionVertexBuffer->VertexBufferRHI->GetSize(), RLM_ReadOnly);

			memcpy(Indices0, indices1, IndexBuffer->IndexBufferRHI->GetSize());
			memcpy(Vertices0, indices2, PositionVertexBuffer->VertexBufferRHI->GetSize());

			RHIUnlockIndexBuffer(IndexBuffer->IndexBufferRHI);
			RHIUnlockVertexBuffer(PositionVertexBuffer->VertexBufferRHI);
		}
	);
	
	FlushRenderingCommands();

	int32 NumVerts;
	int32 NumTris;

	if (!BuoyantMesh) { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, GetOwner()->GetName() + ": Where did the mesh go?!"); return; }
	if (!BuoyantMesh->GetStaticMesh()) { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, GetOwner()->GetName() + ": Where did the mesh go?!"); return; }

	NumVerts = LODResource.GetNumVertices();
	NumTris = LODResource.GetNumTriangles();


	FPositionVertexBuffer* PosVertexBuffer;
	PosVertexBuffer = &LODResource.VertexBuffers.PositionVertexBuffer;
	FIndexArrayView IndexBufferArray;
	IndexBufferArray = LODResource.IndexBuffer.GetArrayView();
	uint32 Stride = LODResource.VertexBuffers.PositionVertexBuffer.GetStride();

	uint8* VertexBufferContent = (uint8*)Vertices;


	FVector A; FVector B; FVector C;
	for (int32 TriIndex = 0; TriIndex < NumTris; TriIndex++) {
		TArray<FVector> Tri;
		
		int32 ia = Indices[TriIndex * 3 + 0];
		int32 ib = Indices[TriIndex * 3 + 1];
		int32 ic = Indices[TriIndex * 3 + 2];

		FVector va = ((FPositionVertex*)(VertexBufferContent + ia*Stride))->Position;
		FVector vb = ((FPositionVertex*)(VertexBufferContent + ib*Stride))->Position;
		FVector vc = ((FPositionVertex*)(VertexBufferContent + ic*Stride))->Position;

		Tri.Add(va);
		Tri.Add(vb);
		Tri.Add(vc);

		Triangles.Add(Tri);

		FVector SideAB = vb - va;
		FVector SideAC = vc - va;
		float LengthMult = SideAB.Size() * SideAC.Size();
		float TriSize = LengthMult * .5 * FGenericPlatformMath::Sin(FGenericPlatformMath::Acos(FVector::DotProduct(SideAB.GetSafeNormal(), SideAC.GetSafeNormal())));
		FalseVolume += TriSize;
		TriSizes.Add(TriSize);
		TriSubmergedArea.Add(0.f);
	}
	
}


void UAdvancedBuoyancyComponent::GetOcean()
{
	for (auto Actor : TActorRange<AOceanShaderManager>(GetWorld()))
	{
		TheOcean =  Actor;
	}
}

float UAdvancedBuoyancyComponent::GetOceanDepthFromGrid(FVector Position, bool bJustGetHeightAtLocation)
{
	// Localized position is local X and Y but global Z
	
	FVector LocalPosition;

	LocalPosition = MeshTransform.InverseTransformPosition(Position);

	float InterDepth = 0.f;
	if (bJustGetHeightAtLocation) {
		InterDepth = TheOcean->GetWaveHeightValue(Position, World, false, false).Z;
	}
	else {
		// Non interpolation method
		int32 column = FMath::Clamp<int32>(FMath::RoundToInt((LocalPosition.Y - MinBound.Y) / (MaxBound.Y - MinBound.Y) * (4 - 1)), 0, 3);
		int32 row = FMath::Clamp<int32>(FMath::RoundToInt((LocalPosition.X - MinBound.X) / (MaxBound.X - MinBound.X) * (5 - 1)), 0, 4);
		InterDepth = AdvancedGridHeight[4 * row + column].Z;
	}

	return (Position.Z - InterDepth) / 1.f; // in cm
}

float UAdvancedBuoyancyComponent::TriangleArea(FVector A, FVector B, FVector C)
{
	FVector SideAB = B - A;
	FVector SideAC = C - A;
	float LengthMult = SideAB.Size() * SideAC.Size();
	return LengthMult * .5 * FGenericPlatformMath::Sin(FGenericPlatformMath::Acos(FVector::DotProduct(SideAB.GetSafeNormal(), SideAC.GetSafeNormal())));  // cm^2
}

void UAdvancedBuoyancyComponent::ApplySlamForce(FVector SlamForce, FVector TriCenter)
{
	BuoyantMesh->AddForceAtLocation(SlamForce, TriCenter);
}

void UAdvancedBuoyancyComponent::AdvancedBuoyancy()
{
	if (!BuoyantMesh) { BuoyantMesh = Cast<UStaticMeshComponent>(GetAttachParent()); }
	if (!BuoyantMesh) { GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Mesh Missed"))); return; }
	if (!BuoyantMesh->GetStaticMesh()) { GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Mesh Mesh Missed"))); return; }

	// Create the grid for ocean height sampling based on the size of the boat mesh
	AdvancedGridHeight.Empty();
	int32 rows = 5; int32 columns = 4;
	AdvancedGridHeight.SetNum(rows*columns);
	float columnSize = (MaxBound.Y - MinBound.Y) / (float)(columns - 1);
	FVector GridPoint;
	for (int32 i = 0; i < rows * columns; i++) {
		GridPoint = FVector((float)(i / columns) * (MaxBound.X - MinBound.X) / (float)(rows - 1) + MinBound.X,
			(float)(i % columns) * (MaxBound.Y - MinBound.Y) / (float)(columns - 1) + MinBound.Y, 0.f);
		GridPoint = BuoyantMesh->GetComponentTransform().TransformPosition(GridPoint);
		AdvancedGridHeight[i] = FVector(GridPoint.X, GridPoint.Y, TheOcean->GetWaveHeightValue(GridPoint, World, false, true).Z);
	}


	if (bDebugOn) {
		for (int32 i = 0; i < rows * columns; i++) {
			DrawDebugSphere(World, AdvancedGridHeight[i], 5.f, 3, FColor::Red, false, -1.f, 0, 10.f);
		}
	}

	FBuoyancyVertex TempVertex;

	// TODO ***OPTIMIZATION***
	// Send this to another thread (get data, do work, pass back forces to apply to boat)
	// the three vertices of a triangle
	SubmergedTris.Empty();
	SubmergedVolume = 0.f;
	for (int32 TriIndex = 0; TriIndex < Triangles.Num(); TriIndex++)
	{
		FVector v0; FVector v1; FVector v2;
		FBuoyancyVertex H; FBuoyancyVertex M; FBuoyancyVertex L;


		// TODO ***HIGH PRIORITY OPTIMIZATION*** 
		// Figure out how this list of vertices relates to each single vertex so we can find the depth and location for each vertex one time instead of three times per vertex.
		// Basically get all the location information for each vertex and then pull from the known list to analyze the triangles
		MeshTransform = BuoyantMesh->GetComponentTransform();
		v0 = MeshTransform.TransformPosition(Triangles[TriIndex][0]);
		v1 = MeshTransform.TransformPosition(Triangles[TriIndex][1]);
		v2 = MeshTransform.TransformPosition(Triangles[TriIndex][2]);

		// Approximate the amount of each triangle that is underwater

		// Order by height

		H.Position = v0;
		M.Position = v1;
		L.Position = v2;
		float a; float b; float c;

		a = GetOceanDepthFromGrid(v0);
		b = GetOceanDepthFromGrid(v1);
		c = GetOceanDepthFromGrid(v2);

		H.Depth = a;
		M.Depth = b;
		L.Depth = c;


		if (M.Depth > H.Depth) { TempVertex = H; H = M; M = TempVertex; }
		if (L.Depth > H.Depth) { TempVertex = H; H = L; L = TempVertex; }
		if (L.Depth > M.Depth) { TempVertex = M; M = L; L = TempVertex; }

		float slamforcemult = 5000.f;

		if (BuoyantMesh) {
			slamforcemult = BuoyantMesh->GetMass() / 2.f * ImpactCoefficient;
		}

		// If no vertices are underwater
		if (L.Depth > 0) { continue; }
		// If one vertex is under water
		if (L.Depth < 0 && M.Depth > 0) {

			FVector InterSectPointOne = -L.Depth / (M.Depth - L.Depth) * (M.Position - L.Position) + L.Position;
			FVector InterSectPointTwo = -L.Depth / (H.Depth - L.Depth) * (H.Position - L.Position) + L.Position;

			FVector OutArrow = -FVector::CrossProduct(v1 - v0, v2 - v0); // Outward facing normal
			OutArrow.Normalize();

			// New vertices
			FBuoyancyVertex NewH; FBuoyancyVertex NewM; FBuoyancyVertex NewL;
			NewH.Position = InterSectPointOne; NewH.Depth = GetOceanDepthFromGrid(InterSectPointOne);
			NewM.Position = InterSectPointTwo; NewM.Depth = GetOceanDepthFromGrid(InterSectPointTwo);
			NewL = L;

			if (NewM.Position.Z > NewH.Position.Z) { TempVertex = NewH; NewH = NewM; NewM = TempVertex; }
			if (NewL.Position.Z > NewH.Position.Z) { TempVertex = NewH; NewH = NewL; NewL = TempVertex; }
			if (NewL.Position.Z > NewM.Position.Z) { TempVertex = NewM; NewM = NewL; NewL = TempVertex; }

			// Get new triangles
			TArray<FForceTriangle> SplitTris = SplitTriangle(NewH, NewM, NewL, OutArrow);
			for (FForceTriangle TriForce : SplitTris) {
				if (bDebugOn) {
					// Waterline
					DrawDebugLine(World, InterSectPointOne, InterSectPointTwo, FColor::Blue, false, -1.f, 0, 10.f);
					DrawDebugStuff(TriForce, FColor::Orange);
				}
			}


			// Slamming force
			float NewSubmergedArea = TriangleArea(NewH.Position, NewM.Position, NewL.Position);
			float dS = FMath::Max(0.f, NewSubmergedArea - TriSubmergedArea[TriIndex]);
			FVector TriCenter = (H.Position + M.Position + L.Position) / 3.f;
			FVector v = BuoyantMesh->GetBodyInstance()->GetUnrealWorldVelocityAtPoint(TriCenter);
			float acceleration = v.Size() / World->DeltaTimeSeconds; // cm / s^2
			float cT = FMath::Clamp(OutArrow | v.GetSafeNormal(), 0.f, 1.f); // face velocity value
			float SlamRampValue = 2.f;
			FVector StopForce = -OutArrow * BuoyantMesh->GetMass() * slamforcemult * dS / FalseVolume;  // Make sure the force won't push the boat in the opposite direction
			FVector SlamForce = FMath::Pow(FMath::Clamp(acceleration / MaxSlamAcceleration, 0.f, 1.f), SlamRampValue) * cT * StopForce;
			TriSubmergedArea[TriIndex] = NewSubmergedArea;
			if (!FMath::IsNearlyZero(SlamForce.Size())) {
				ApplySlamForce(SlamForce, TriCenter);
				if (bDebugOn) {
					float FLDM = .1f;
					DrawDebugLine(World, TriCenter - SlamForce * FLDM, TriCenter - SlamForce * .1f * FLDM, FColor(175, 10, 10, 255), false, -1.f, 0, 5.f);
					DrawDebugLine(World, TriCenter - SlamForce * .1f * FLDM, TriCenter, FColor::Orange, false, -1.f, 0, 5.f);
				}
			}


			continue;
		}
		// If two vertices are underwater
		if (M.Depth < 0 && H.Depth > 0) {

			FVector InterSectPointOne = -L.Depth / (H.Depth - L.Depth) * (H.Position - L.Position) + L.Position;
			FVector InterSectPointTwo = -M.Depth / (H.Depth - M.Depth) * (H.Position - M.Position) + M.Position;

			FVector OutArrow = -FVector::CrossProduct(v1 - v0, v2 - v0); // Outward facing normal
			OutArrow.Normalize();

			// Detrapezoidation

			// New vertices 1
			FBuoyancyVertex NewH1; FBuoyancyVertex NewM1; FBuoyancyVertex NewL1;
			NewH1.Position = InterSectPointOne; NewH1.Depth = GetOceanDepthFromGrid(InterSectPointOne);
			NewM1.Position = InterSectPointTwo; NewM1.Depth = GetOceanDepthFromGrid(InterSectPointTwo);
			NewL1 = M;
			if (NewM1.Position.Z > NewH1.Position.Z) { TempVertex = NewH1; NewH1 = NewM1; NewM1 = TempVertex; }
			if (NewL1.Position.Z > NewH1.Position.Z) { TempVertex = NewH1; NewH1 = NewL1; NewL1 = TempVertex; }
			if (NewL1.Position.Z > NewM1.Position.Z) { TempVertex = NewM1; NewM1 = NewL1; NewL1 = TempVertex; }
			// New vertices 2
			FBuoyancyVertex NewH2; FBuoyancyVertex NewM2; FBuoyancyVertex NewL2;
			NewH2.Position = InterSectPointOne; NewH2.Depth = GetOceanDepthFromGrid(InterSectPointOne);
			NewM2 = M;
			NewL2 = L;
			if (NewM2.Position.Z > NewH2.Position.Z) { TempVertex = NewH2; NewH2 = NewM2; NewM2 = TempVertex; }
			if (NewL2.Position.Z > NewH2.Position.Z) { TempVertex = NewH2; NewH2 = NewL2; NewL2 = TempVertex; }
			if (NewL2.Position.Z > NewM2.Position.Z) { TempVertex = NewM2; NewM2 = NewL2; NewL2 = TempVertex; }

			// Get new triangles
			TArray<FForceTriangle> SplitTris = SplitTriangle(NewH1, NewM1, NewL1, OutArrow);
			for (FForceTriangle TriForce : SplitTris) {
				if (bDebugOn) {
					// Waterline
					DrawDebugLine(World, InterSectPointOne, InterSectPointTwo, FColor::Blue, false, -1.f, 0, 10.f);
					DrawDebugStuff(TriForce, FColor::Purple);
				}
			}
			// Get new triangles
			SplitTris.Empty();
			SplitTris = SplitTriangle(NewH2, NewM2, NewL2, OutArrow);
			for (FForceTriangle TriForce : SplitTris) {
				if (bDebugOn) {
					DrawDebugStuff(TriForce, FColor::Turquoise);
				}
			}

			// Slamming force
			float NewSubmergedArea = TriangleArea(NewH1.Position, NewM1.Position, NewL1.Position);
			NewSubmergedArea += TriangleArea(NewH2.Position, NewM2.Position, NewL2.Position);
			float dS = FMath::Max(0.f, NewSubmergedArea - TriSubmergedArea[TriIndex]);
			FVector TriCenter = (H.Position + M.Position + L.Position) / 3.f;
			FVector v = BuoyantMesh->GetBodyInstance()->GetUnrealWorldVelocityAtPoint(TriCenter);
			float acceleration = v.Size() / World->DeltaTimeSeconds; // cm / s^2
			float cT = FMath::Clamp(OutArrow | v.GetSafeNormal(), 0.f, 1.f); // face velocity value
			float SlamRampValue = 2.f;
			FVector StopForce = -OutArrow * BuoyantMesh->GetMass() * slamforcemult * dS / FalseVolume;  // Make sure the force won't push the boat in the opposite direction
			FVector SlamForce = FMath::Pow(FMath::Clamp(acceleration / MaxSlamAcceleration, 0.f, 1.f), SlamRampValue) * cT * StopForce;
			TriSubmergedArea[TriIndex] = NewSubmergedArea;
			if (!FMath::IsNearlyZero(SlamForce.Size())) {
				ApplySlamForce(SlamForce, TriCenter);
				if (bDebugOn) {
					float FLDM = .1f;
					DrawDebugLine(World, TriCenter - SlamForce * FLDM, TriCenter - SlamForce * .1f * FLDM, FColor(175, 10, 10, 255), false, -1.f, 0, 5.f);
					DrawDebugLine(World, TriCenter - SlamForce * .1f * FLDM, TriCenter, FColor::Orange, false, -1.f, 0, 5.f);
				}
			}

			continue;
		}
		// If three vertices are underwater
		if (H.Depth < 0) {

			FVector OutArrow = -FVector::CrossProduct(v1 - v0, v2 - v0); // Outward facing normal
			OutArrow.Normalize();

			if (M.Position.Z > H.Position.Z) { TempVertex = H; H = M; M = TempVertex; }
			if (L.Position.Z > H.Position.Z) { TempVertex = H; H = L; L = TempVertex; }
			if (L.Position.Z > M.Position.Z) { TempVertex = M; M = L; L = TempVertex; }

			// Get new triangles
			TArray<FForceTriangle> SplitTris = SplitTriangle(H, M, L, OutArrow);
			for (FForceTriangle TriForce : SplitTris) {

				if (bDebugOn) {
					DrawDebugStuff(TriForce, FColor::Green);
				}
			}

			// Slamming force
			float NewSubmergedArea = TriangleArea(H.Position, M.Position, L.Position);
			float dS = FMath::Max(0.f, NewSubmergedArea - TriSubmergedArea[TriIndex]);
			FVector TriCenter = (H.Position + M.Position + L.Position) / 3.f;
			FVector v = BuoyantMesh->GetBodyInstance()->GetUnrealWorldVelocityAtPoint(TriCenter);
			float acceleration = v.Size() / World->DeltaTimeSeconds; // cm / s^2
			float cT = FMath::Clamp(OutArrow | v.GetSafeNormal(), 0.f, 1.f); // face velocity value
			float SlamRampValue = 2.f;
			FVector StopForce = -OutArrow * BuoyantMesh->GetMass() * slamforcemult * dS / FalseVolume;  // Make sure the force won't push the boat in the opposite direction
			FVector SlamForce = FMath::Pow(FMath::Clamp(acceleration / MaxSlamAcceleration, 0.f, 1.f), SlamRampValue) * cT * StopForce;
			TriSubmergedArea[TriIndex] = NewSubmergedArea;
			if (!FMath::IsNearlyZero(SlamForce.Size())) {
				ApplySlamForce(SlamForce, TriCenter);
				if (bDebugOn) {
					float FLDM = .1f;
					DrawDebugLine(World, TriCenter - SlamForce * FLDM, TriCenter - SlamForce * .1f * FLDM, FColor(175, 10, 10, 255), false, -1.f, 0, 5.f);
					DrawDebugLine(World, TriCenter - SlamForce * .1f * FLDM, TriCenter, FColor::Orange, false, -1.f, 0, 5.f);
				}
			}

			continue;
		}
	}

	float totalupforce = 0.f;
	for (FForceTriangle TriForce : SubmergedTris) {
		ApplyForce(TriForce);
		totalupforce += TriForce.Force.Z;
	}
}

void UAdvancedBuoyancyComponent::SetMeshDensity(float NewMeshDensity, float NewWaterDensity)
{
	MeshDensity = NewMeshDensity;
	CorrectedMeshDensity = MeshDensity / 1000000.f;
	CorrectedWaterDensity = NewWaterDensity / 1000000.f;
}
