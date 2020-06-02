// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "IBuildingSDK.h"
#include "BuildingSceneComponent.generated.h"

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class BUILDINGSDK_API UBuildingSceneComponent: public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	bool LoadScene(const FString &Filename);
	
	UFUNCTION(BlueprintCallable)
	class UResourceMgrComponent *GetResourceMgr();

protected:
	AActor * SpawnMXActor(IObject *pObj, const FTransform &WorldTM);
public:
};


