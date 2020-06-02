// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "WLink.h"
#include "SolutionComponent.h"
#include "BuildingMeshComponent.h"
#include "ResourceMeshComponent.generated.h"

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class BUILDINGSDK_API UResourceMeshComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetResource(int32 ResourceID);
	
	UFUNCTION(BlueprintCallable)
	UBuildingGroupMeshComponent *FindComponentByElementID(int32 InElemID);
	
	virtual void OnRegisterComponent(IObject *pObj);
	virtual void OnUnregister() override;
protected:
	void OnUpdateResource(IObject *RawObj, EFuncType FuncType, IValue *pValue);
	void OnUpdateObject(IObject *RawObj, EFuncType FuncType, IValue *pValue);
	UBuildingGroupMeshComponent *AddComponentToWorld(USceneComponent *ParentMeshComponent, IGroup *Group);
	void UpdateComponents(IObject *RawObj);
	void DestoryComponents();
	IBuildingResourceMgr *GetResourceMgr();
	friend class AResourceActor;
protected:
	UPROPERTY(BlueprintReadOnly)
	int32 InstanceID;
	
	UPROPERTY(BlueprintReadOnly)
	USolutionComponent *SolutionComponent;

	UPROPERTY(BlueprintReadOnly)
	TArray<UBuildingGroupMeshComponent*> MeshComponents;

	WLink	Resource;
	WLink	Instance;
};


