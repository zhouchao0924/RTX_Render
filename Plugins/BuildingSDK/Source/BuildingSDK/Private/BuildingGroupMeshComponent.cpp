
#include "BuildingGroupMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "ResourceMgrComponent.h"
#include "KismetProceduralMeshLibrary.h"

UBuildingGroupMeshComponent::UBuildingGroupMeshComponent(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
	, HoldActor(nullptr)
	, _Node(nullptr)
{
}

int32 UBuildingGroupMeshComponent::GetID()
{
	if (_Node)
	{
		return _Node->GetID();
	}
	return INDEX_NONE;
}

IGroup *UBuildingGroupMeshComponent::GetGroup()
{ 
	if (_Node)
	{
		return _Node->CastToGroup();
	}
	return nullptr; 
}

void UBuildingGroupMeshComponent::UpdateNode(INode *Node)
{
	if (Node)
	{
		_Node = Node;
		_SectionInfos.Empty();
		ClearAllMeshSections();

		IGroup *pGroup = Node->CastToGroup();
		if (pGroup)
		{
			_UpdateGroup(pGroup);
		}
		else
		{
			int32 SectionIndex = 0;
			_UpdateNode(Node, SectionIndex);
		}
	}
}

void UBuildingGroupMeshComponent::_UpdateGroup(IGroup *Group)
{
	int32 SectionIndex = 0;
	int nChild = Group->GetChildCount();

	for (int i = 0; i < nChild; ++i)
	{
		INode *pNode = Group->GetChild(i);
		_UpdateNode(pNode, SectionIndex);
	}
}

void UBuildingGroupMeshComponent::_UpdateNode(INode *pNode, int32 &SectionIndex)
{
	IMeshObject *MeshObject = pNode ? pNode->GetMesh() : nullptr;
	if (!MeshObject)
	{
		return;
	}

	FString Name = ANSI_TO_TCHAR(MeshObject->GetName());
	if (Name.Find(TEXT("aux_")) == 0)
	{
		return;
	}

	if (!pNode->IsVisible())
	{
		return;
	}

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

			FSectionInfo SectionInfo;
			SectionInfo.Node = pNode;
			SectionInfo.MeshObj = MeshObject;
			SectionInfo.MeshSectionIndex = iSection;
			SectionInfo.ComponentSectionIndex = SectionIndex;
			_SectionInfos.Add(SectionInfo);

			ISurfaceObject *pSurface = pNode->GetSurface(iSection);
			if (pSurface)
			{
				UpdateSurface(SectionIndex, pSurface);
			}

			SectionIndex++;
		}
	}
}

void UBuildingGroupMeshComponent::UpdateSurface(int iSection, ISurfaceObject *Surface)
{
	UMaterialInstanceDynamic *DynamicMtrl = GetMaterialBySurface(Surface);
	if (DynamicMtrl)
	{
		SetMaterial(iSection, DynamicMtrl);
	}
}

UMaterialInstanceDynamic *UBuildingGroupMeshComponent::GetMaterialBySurface(ISurfaceObject *Surface)
{
	UResourceMgrComponent *ResMgr = UResourceMgrComponent::GetSDKResMgr();
	if (ResMgr)
	{
		return ResMgr->GetMaterialResource(nullptr, Surface);
	}
	return nullptr;
}

void UBuildingGroupMeshComponent::GetNodeByMeshName(const FString &Name, TArray<INode *> &Nodes)
{
	if (_Node)
	{
		IGroup *pGroup = _Node->CastToGroup();
		if (pGroup)
		{
			int ChildCount = pGroup->GetChildCount();
			for (int i = 0; i < ChildCount; ++i)
			{
				INode *pNode = pGroup->GetChild(i);
				if (Name.Len() > 0)
				{
					IMeshObject *MeshObj = pNode->GetMesh();
					std::string AnsiName = TCHAR_TO_ANSI(*Name);
					if (MeshObj && MeshObj->GetName() == AnsiName)
					{
						Nodes.Add(pNode);
					}
				}
				else
				{
					Nodes.Add(pNode);
				}
			}
		}
		else
		{
			if (Name.Len() > 0)
			{
				IMeshObject *MeshObj = _Node->GetMesh();
				std::string AnsiName = TCHAR_TO_ANSI(*Name);
				if (MeshObj && MeshObj->GetName() == AnsiName)
				{
					Nodes.Add(_Node);
				}
			}
			else
			{
				Nodes.Add(_Node);
			}
		}
	}
}

int32 UBuildingGroupMeshComponent::GetComponentSectionIndex(IMeshObject *MeshObj, int32 iSectionIndex)
{
	for (int32 i = 0; i < _SectionInfos.Num(); ++i)
	{
		FSectionInfo &SectionInfo = _SectionInfos[i];
		if (SectionInfo.MeshObj == MeshObj && SectionInfo.MeshSectionIndex == iSectionIndex)
		{
			return SectionInfo.ComponentSectionIndex;
		}
	}
	return INDEX_NONE;
}

void  UBuildingGroupMeshComponent::GetComponentSections(IMeshObject *meshObj, TArray<int32> &Sections)
{
	Sections.Empty();

	for (int32 i = 0; i < _SectionInfos.Num(); ++i)
	{
		FSectionInfo &SectionInfo = _SectionInfos[i];
		if (SectionInfo.MeshObj == meshObj)
		{
			Sections.Add(SectionInfo.ComponentSectionIndex);
		}
	}
}

ISurfaceObject *UBuildingGroupMeshComponent::GetSurfaceBySectionIndex(int32 SectionIndex, IObject *&SurfaceOwner)
{
	ISurfaceObject *pSurf = nullptr;
	
	for (int32 i = 0; i < _SectionInfos.Num(); ++i)
	{
		FSectionInfo &SectionInfo = _SectionInfos[i];
		if (SectionInfo.ComponentSectionIndex == SectionIndex)
		{
			if (SectionInfo.Node)
			{
				pSurf = SectionInfo.Node->GetSurface(SectionInfo.MeshSectionIndex);
			}
			break;
		}
	}

	return pSurf;
}


