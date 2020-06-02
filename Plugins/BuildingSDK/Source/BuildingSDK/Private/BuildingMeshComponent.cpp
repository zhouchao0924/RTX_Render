
#include "BuildingMeshComponent.h"
#include "BuildingSDKSystem.h"

UBuildingMeshComponent::UBuildingMeshComponent(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
	, ObjID(INVALID_OBJID)
	, ElemID(INVALID_OBJID)
	, Parent(nullptr)
	, ChildIndex(-1)
{
}

void UBuildingMeshComponent::UpdateObject(IObject *Obj, bool bReclusive)
{
	ObjID = Obj->GetID();
	UpdateObjectReclusive(Obj, bReclusive);
}

void UBuildingMeshComponent::UpdateGroup(IGroup *Group)
{
	int nChild = Group->GetChildCount();
	
	for (int i = 0; i < nChild; ++i)
	{
		INode *pNode = Group->GetChild(i);
		IMeshObject *MeshObject = pNode ? pNode->GetMesh() : nullptr;
		if (!MeshObject)
		{
			continue;
		}

		FString Name = ANSI_TO_TCHAR(MeshObject->GetName());
		if (Name.Find(TEXT("aux_")) == 0)
		{
			continue;
		}

		int SectionIndex = 0;
		int nSection = MeshObject->GetSectionCount();

		for (int iSection = 0; iSection < nSection; ++iSection)
		{
			float *pVertices = nullptr;
			float *pNormals = nullptr;
			float *pTangents = nullptr;
			float *pUVs = nullptr;
			float *pLightmapUVs = nullptr;
			int nVerts = 0, nIndices = 0;
			int *pIndices = nullptr;

			if (MeshObject->GetSectionMesh(iSection, pVertices, pNormals, pTangents, pUVs, pLightmapUVs, nVerts, pIndices, nIndices))
			{
				TArray<FVector> Vertices, Normals;
				TArray<int32> Triangles;
				TArray<FVector2D> UVs;
				TArray<FColor> VertexColors;
				TArray<FProcMeshTangent> Tangents;

				UVs.SetNum(nVerts);
				Vertices.SetNum(nVerts);
				Normals.SetNum(nVerts);
				Tangents.SetNum(nVerts);
				VertexColors.SetNum(nVerts);
				Triangles.SetNum(nIndices);

				FMemory::Memcpy(Vertices.GetData(), pVertices, nVerts * 12);
				FMemory::Memcpy(Normals.GetData(), pNormals, nVerts * 12);

				for (int32 iVert = 0; iVert < Tangents.Num(); ++iVert)
				{
					VertexColors[iVert] = FColor::White;
					Tangents[iVert].TangentX = *((FVector *)(pTangents + 3 * iVert));
				}

				FMemory::Memcpy(UVs.GetData(), pUVs, nVerts * 8);
				FMemory::Memcpy(Triangles.GetData(), pIndices, nIndices * 4);

				CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

				ISurfaceObject *pSurface = pNode->GetSurface(iSection);
				if (pSurface)
				{
					UpdateSurface(SectionIndex, pSurface);
				}

				SectionIndex++;
			}
		}
	}
}

void UBuildingMeshComponent::UpdateObjectReclusive(IObject *Obj, bool bReclusive)
{
	int32 MeshCount = Obj->GetMeshCount();

	for (int32 i = 0; i < MeshCount; ++i)
	{
		IMeshObject *MeshObject = Obj->GetMeshObject(i);
		FString Name = ANSI_TO_TCHAR(MeshObject->GetName());
		if (Name.Find(TEXT("aux_"))==0)
		{
			continue;
		}

		int nSection = MeshObject->GetSectionCount();

		int SectionIndex = 0;

		for (int iSection = 0; iSection < nSection; ++iSection)
		{
			float *pVertices = nullptr;
			float *pNormals = nullptr;
			float *pTangents = nullptr;
			float *pUVs = nullptr;
			float *pLightmapUVs = nullptr;
			int nVerts = 0, nIndices = 0;
			int *pIndices = nullptr;

			if (MeshObject->GetSectionMesh(iSection, pVertices, pNormals, pTangents, pUVs, pLightmapUVs, nVerts, pIndices, nIndices))
			{
				TArray<FVector> Vertices, Normals;
				TArray<int32> Triangles;
				TArray<FVector2D> UVs;
				TArray<FColor> VertexColors;
				TArray<FProcMeshTangent> Tangents;

				UVs.SetNum(nVerts);
				Vertices.SetNum(nVerts);
				Normals.SetNum(nVerts);
				Tangents.SetNum(nVerts);
				VertexColors.SetNum(nVerts);
				Triangles.SetNum(nIndices);

				FMemory::Memcpy(Vertices.GetData(), pVertices, nVerts * 12);
				FMemory::Memcpy(Normals.GetData(), pNormals, nVerts * 12);

				for (int32 iVert = 0; iVert < Tangents.Num(); ++iVert)
				{
					VertexColors[iVert] = FColor::White;
					Tangents[iVert].TangentX = *((FVector *)(pTangents + 3 * iVert));
				}

				FMemory::Memcpy(UVs.GetData(), pUVs, nVerts * 8);
				FMemory::Memcpy(Triangles.GetData(), pIndices, nIndices * 4);

				CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

				INode *SurfNode = nullptr;
				if (Parent)
				{
					SurfNode = Parent->GetNode();
				}
				else
				{
					SurfNode = Obj->GetNode();
				}

				ISurfaceObject *pSurface = SurfNode? SurfNode->GetSurface(iSection) : nullptr;
				if (pSurface)
				{
					UpdateSurface(SectionIndex, pSurface);
				}

				SectionIndex++;
			}
		}
	}

	if (bReclusive)
	{
		int32 NumChilds = Obj->GetNumberOfChild();
		for (int32 childIndex = 0; childIndex < NumChilds; ++childIndex)
		{
			IObject *Child = Obj->GetChild(childIndex);
			UpdateObjectReclusive(Child, bReclusive);
		}
	}
}

void UBuildingMeshComponent::UpdateSurface(int iSection, ISurfaceObject *Surface)
{
	UMaterialInstanceDynamic *DynamicMtrl = GetMaterialBySurface(Surface);
	if (DynamicMtrl)
	{
		SetMaterial(iSection, DynamicMtrl);
	}
}

UMaterialInstanceDynamic *UBuildingMeshComponent::GetMaterialBySurface(ISurfaceObject *Surface)
{
	UResourceMgrComponent *ResMgr = UResourceMgrComponent::GetSDKResMgr();
	if (ResMgr)
	{
		return ResMgr->GetMaterialResource(nullptr, Surface);
	}
	return nullptr;
}

void UBuildingMeshComponent::SetParent(IObject *InParent, int32 InChildIndex)
{
	Parent = InParent;
	ChildIndex = InChildIndex;
}

