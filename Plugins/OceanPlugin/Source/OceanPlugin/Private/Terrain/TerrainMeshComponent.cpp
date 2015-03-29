
#include "OceanPluginPrivatePCH.h"
#include "Terrain/TerrainMeshComponent.h"
#include "Runtime/Launch/Resources/Version.h"

#define VBO_SIZE 100002
#define IBO_SIZE 500001

/** Vertex Buffer */
class FTerrainMeshVertexBuffer : public FVertexBuffer
{
	
public:
	int32 BOSize;
	virtual void InitRHI() override
	{
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 3
		FRHIResourceCreateInfo CreateInfo;
		VertexBufferRHI = RHICreateVertexBuffer(BOSize * sizeof(FDynamicMeshVertex), BUF_AnyDynamic, CreateInfo);
#else
		VertexBufferRHI = RHICreateVertexBuffer(BOSize * sizeof(FDynamicMeshVertex), NULL, BUF_AnyDynamic);
#endif
	}

	void AddElements(TArray<FDynamicMeshVertex> &Elements)
	{
		FDynamicMeshVertex* VertexBufferData = (FDynamicMeshVertex*)RHILockVertexBuffer(VertexBufferRHI, 0, BOSize * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, &Elements[0], Elements.Num() * sizeof(FDynamicMeshVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}

};
/** Index Buffer */
class FTerrainMeshIndexBuffer : public FIndexBuffer
{
public:
	int32 BOSize;
	virtual void InitRHI() override
	{
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 3
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), BOSize * sizeof(int32), BUF_AnyDynamic, CreateInfo);
#else
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), BOSize * sizeof(int32), NULL, BUF_AnyDynamic);
#endif
	}

	void AddElements(TArray<int32> &Elements)
	{
		int32* IndexBufferData = (int32*)RHILockIndexBuffer(IndexBufferRHI, 0, BOSize * sizeof(int32), RLM_WriteOnly);
		FMemory::Memcpy(IndexBufferData, &Elements[0], Elements.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

/** Vertex Factory */
class FTerrainMeshVertexFactory : public FLocalVertexFactory
{
public:
	FTerrainMeshVertexFactory()
	{
	}
	/** Initialization */
	void Init(const FTerrainMeshVertexBuffer* VertexBuffer)
	{
		// Commented out to enable building light of a level (but no backing is done for the procedural mesh itself)
		//check(!IsInRenderingThread());
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitTerrainMeshVertexFactory,
			FTerrainMeshVertexFactory*, VertexFactory, this,
			const FTerrainMeshVertexBuffer*, VertexBuffer, VertexBuffer,
			{
				// Initialize the vertex factory's stream components.
				DataType NewData;
				NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
				NewData.TextureCoordinates.Add(
					FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex, TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
					);
				NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
				NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
				NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
				VertexFactory->SetData(NewData);
			});
	}

};
/** Scene proxy */
class FTerrainMeshSceneProxy : public FPrimitiveSceneProxy
{
	UTerrainMeshComponent* TerrainMeshComponent;
public:
	FTerrainMeshSceneProxy(UTerrainMeshComponent* Component)
		: FPrimitiveSceneProxy(Component),
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 5
		 MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
#else
		 MaterialRelevance(Component->GetMaterialRelevance())
#endif
	{
		TerrainMeshComponent = Component;
		VertexBuffer.BOSize = Component->Vertices.Num();
		IndexBuffer.BOSize = Component->Indices.Num();

		// Init vertex factory
		VertexFactory.Init(&VertexBuffer);
		// Enqueue initialization of render resource
		BeginInitResource(&VertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);

		
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			UpdateTerrainVertexCommand,
			FTerrainMeshSceneProxy*, Proxy, (FTerrainMeshSceneProxy*)this,
			{
				Proxy->SetStaticData_RenderThread();
			});
		

		// Grab material
		Material = Component->GetMaterial(0);
		if (Material == NULL)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}
		

	}

	virtual ~FTerrainMeshSceneProxy()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	void SetStaticData_RenderThread()
	{
		if (IsInRenderingThread())
		{
			VertexBuffer.AddElements(TerrainMeshComponent->Vertices);
			IndexBuffer.AddElements(TerrainMeshComponent->Indices);
		}
	}

#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 5
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_TerrainMeshSceneProxy_GetDynamicMeshElements);
		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f)
			);
		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
		FMaterialRenderProxy* MaterialProxy = NULL;
		if (bWireframe)
		{
			MaterialProxy = WireframeMaterialInstance;
		}
		else
		{
			MaterialProxy = Material->GetRenderProxy(IsSelected());
		}
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				// Draw the mesh.
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;
				BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.BOSize / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffer.BOSize - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				//Mesh.ReverseCulling = false;
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}
#endif
	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_TerrainMeshSceneProxy_DrawDynamicElements);
		const bool bWireframe = AllowDebugViewmodes() && View->Family->EngineShowFlags.Wireframe;
		FColoredMaterialRenderProxy WireframeMaterialInstance(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f)
			);
		FMaterialRenderProxy* MaterialProxy = NULL;
		if (bWireframe)
		{
			MaterialProxy = &WireframeMaterialInstance;
		}
		else
		{
			MaterialProxy = Material->GetRenderProxy(IsSelected());
		}
		// Draw the mesh.
		FMeshBatch Mesh;
		FMeshBatchElement& BatchElement = Mesh.Elements[0];
		BatchElement.IndexBuffer = &IndexBuffer;
		Mesh.bWireframe = bWireframe;
		Mesh.VertexFactory = &VertexFactory;
		Mesh.MaterialRenderProxy = MaterialProxy;
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 5
		BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
#else
		BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true);
#endif
		BatchElement.FirstIndex = 0;
		BatchElement.NumPrimitives = IndexBuffer.BOSize / 3;
		BatchElement.MinVertexIndex = 0;
		BatchElement.MaxVertexIndex = VertexBuffer.BOSize - 1;
		Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
		//Mesh.ReverseCulling = false;
		Mesh.Type = PT_TriangleList;
		Mesh.CastShadow = true;
		Mesh.DepthPriorityGroup = SDPG_World;
		PDI->DrawMesh(Mesh);
	}
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View)
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = true; //IsShown(View);
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		return Result;
	}
	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}
	virtual uint32 GetMemoryFootprint(void) const
	{
		return(sizeof(*this) + GetAllocatedSize());
	}
	uint32 GetAllocatedSize(void) const
	{
		return(FPrimitiveSceneProxy::GetAllocatedSize());
	}
private:
	UMaterialInterface* Material;
	FTerrainMeshVertexBuffer VertexBuffer;
	FTerrainMeshIndexBuffer IndexBuffer;
	FTerrainMeshVertexFactory VertexFactory;
	FMaterialRelevance MaterialRelevance;
};
//////////////////////////////////////////////////////////////////////////
UTerrainMeshComponent::UTerrainMeshComponent(const FObjectInitializer& PCIP)
	: Super(PCIP)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);

}

FPrimitiveSceneProxy* UTerrainMeshComponent::CreateSceneProxy()
{
	FPrimitiveSceneProxy *tmp = 0;
	// Only if have enough triangles
	if (Vertices.Num() > 0)
	{
		UE_LOG(LogClass, Log, TEXT("Creating New Scene Proxy"));
		tmp = new FTerrainMeshSceneProxy(this);
	}
	return tmp;
}
int32 UTerrainMeshComponent::GetNumMaterials() const
{
	return 1;
}

FBoxSphereBounds UTerrainMeshComponent::CalcBounds(const FTransform & LocalToWorld) const
{
	if (Positions.Num() > 0)
	{
		UE_LOG(LogClass, Log, TEXT("CalcBounds"));
		// Minimum Vector: It's set to the first vertex's position initially (NULL == FVector::ZeroVector might be required and a known vertex vector has intrinsically valid values)
		FVector vecMin = Positions[0];
		// Maximum Vector: It's set to the first vertex's position initially (NULL == FVector::ZeroVector might be required and a known vertex vector has intrinsically valid values)
		FVector vecMax = Positions[0];
		// Get maximum and minimum X, Y and Z positions of vectors
		for (int32 MeshIdx = 0; MeshIdx < Positions.Num(); MeshIdx++)
		{
			vecMin.X = (vecMin.X > Positions[MeshIdx].X) ? Positions[MeshIdx].X : vecMin.X;
			vecMin.Y = (vecMin.Y > Positions[MeshIdx].Y) ? Positions[MeshIdx].Y : vecMin.Y;
			vecMin.Z = (vecMin.Z > Positions[MeshIdx].Z) ? Positions[MeshIdx].Z : vecMin.Z;

			vecMax.X = (vecMax.X < Positions[MeshIdx].X) ? Positions[MeshIdx].X : vecMax.X;
			vecMax.Y = (vecMax.Y < Positions[MeshIdx].Y) ? Positions[MeshIdx].Y : vecMax.Y;
			vecMax.Z = (vecMax.Z < Positions[MeshIdx].Z) ? Positions[MeshIdx].Z : vecMax.Z;
		}

		FVector vecOrigin = ((vecMax - vecMin) / 2) + vecMin; // Origin = ((Max Vertex's Vector - Min Vertex's Vector) / 2 ) + Min Vertex's Vector
		FVector BoxPoint = vecMax - vecMin; // The difference between the "Maximum Vertex" and the "Minimum Vertex" is our actual Bounds Box
		return FBoxSphereBounds(vecOrigin, BoxPoint, BoxPoint.Size()).TransformBy(LocalToWorld);
	}
	else
	{
		return FBoxSphereBounds();
	}
}

bool UTerrainMeshComponent::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{

	FTriIndices Triangle;
	for (int32 i = 0; i<Indices.Num(); i += 3)
	{
		Triangle.v0 = CollisionData->Vertices.AddUnique(Vertices[Indices[i]].Position);
		Triangle.v1 = CollisionData->Vertices.AddUnique(Vertices[Indices[i + 1]].Position);
		Triangle.v2 = CollisionData->Vertices.AddUnique(Vertices[Indices[i + 2]].Position);
		CollisionData->Indices.Add(Triangle);
		CollisionData->MaterialIndices.Add(i);
	}
	CollisionData->bFlipNormals = true;
	return true;
}

bool UTerrainMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	return (Vertices.Num() > 0);
}


void UTerrainMeshComponent::UpdateBodySetup()
{
	if (ModelBodySetup == NULL)
	{
		ModelBodySetup = ConstructObject<UBodySetup>(UBodySetup::StaticClass(), this);
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = true;
	}
}


void UTerrainMeshComponent::UpdateCollision()
{
	if (bPhysicsStateCreated)
	{
		UE_LOG(LogClass, Log, TEXT("bPhysicsStateCreated"));
		DestroyPhysicsState();
		UpdateBodySetup();
		
		// Works in Packaged build only since UE4.5:
		ModelBodySetup->InvalidatePhysicsData();
		ModelBodySetup->CreatePhysicsMeshes();
		CreatePhysicsState();
	}
}

void UTerrainMeshComponent::RemoveCollision()
{
	if (bPhysicsStateCreated)
	{
		DestroyPhysicsState();
	}
}

UBodySetup* UTerrainMeshComponent::GetBodySetup()
{
	UpdateBodySetup();
	return ModelBodySetup;
}
