// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "BuildingGroupMeshComponent.h"
#include "BGCPoolManager.generated.h"

UCLASS(BlueprintType)
class BUILDINGSDK_API UBGCPoolManager : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static UBGCPoolManager *Instance();

	UFUNCTION(BlueprintCallable)
	UBuildingGroupMeshComponent *AllocComponent(AActor *Owner);

	UFUNCTION(BlueprintCallable)
	static void FreeComponent(UBuildingGroupMeshComponent *Component);

	UFUNCTION(BlueprintCallable)
	void ReleaseAll();
protected:
	void Free(UBuildingGroupMeshComponent *Component);
protected:
	UPROPERTY(Transient)
	TArray<UBuildingGroupMeshComponent *> Components;
};


