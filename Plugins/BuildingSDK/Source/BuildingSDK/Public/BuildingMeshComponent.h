// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "IBuildingSDK.h"
#include "ProceduralMeshComponent.h"
#include "BuildingMeshComponent.generated.h"

UCLASS(BlueprintType)
class BUILDINGSDK_API UBuildingMeshComponent : public UProceduralMeshComponent
{
	GENERATED_UCLASS_BODY()
public:
	void UpdateGroup(IGroup *Group);
	void UpdateObject(IObject *Obj, bool bReclusive);
	UFUNCTION(BlueprintCallable)
	int32 GetID() { return ObjID; }
	void SetParent(IObject *InParent, int32 InChildIndex);
protected:
	void UpdateObjectReclusive(IObject *Obj, bool bReclusive);
	void UpdateSurface(int iSection, ISurfaceObject *Surface);
	UMaterialInstanceDynamic *GetMaterialBySurface(ISurfaceObject *Surface);
public:
	UPROPERTY(BlueprintReadOnly)
	int32 ObjID;
	
	UPROPERTY(BlueprintReadOnly)
	int32 ElemID;
protected:
	IObject *Parent;
	int32	ChildIndex;
};


