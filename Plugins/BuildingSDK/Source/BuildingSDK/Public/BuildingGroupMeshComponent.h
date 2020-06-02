// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "IBuildingSDK.h"
#include "ProceduralMeshComponent.h"
#include "BuildingGroupMeshComponent.generated.h"

UCLASS(BlueprintType)
class BUILDINGSDK_API UBuildingGroupMeshComponent : public UProceduralMeshComponent
{
	GENERATED_UCLASS_BODY()
public:
	struct FSectionInfo
	{
		FSectionInfo()
			: Node(nullptr)
			, MeshObj(nullptr)
			, MeshSectionIndex(-1)
			, ComponentSectionIndex(-1)
		{
		}
		INode		*Node;
		IMeshObject	*MeshObj;
		int32		MeshSectionIndex;
		int32		ComponentSectionIndex;
	};
	UFUNCTION(BlueprintCallable)
	int32 GetID();
	void UpdateNode(INode *Node);
	IGroup *GetGroup();
	INode *GetNode() { return _Node; }
	void GetNodeByMeshName(const FString &Name, TArray<INode *> &Nodes);
	int32 GetComponentSectionIndex(IMeshObject *MeshObj, int32 iSectionIndex);
	void  GetComponentSections(IMeshObject *meshObj, TArray<int32> &Sections);
	ISurfaceObject *GetSurfaceBySectionIndex(int32 SectionIndex, IObject *&SurfaceOwner);
protected:
	void _UpdateGroup(IGroup *Group);
	void _UpdateNode(INode *Node, int32 &SectionIndex);
	void UpdateSurface(int iSection, ISurfaceObject *Surface);
	UMaterialInstanceDynamic *GetMaterialBySurface(ISurfaceObject *Surface);
public:
	UPROPERTY(Transient, BlueprintReadOnly)
	AActor				 *HoldActor;
protected:
	INode				 *_Node;

	TArray<FSectionInfo> _SectionInfos;
};


