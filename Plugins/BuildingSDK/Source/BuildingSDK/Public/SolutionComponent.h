// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SolutionComponent.generated.h"

UCLASS(BlueprintType)
class USolutionComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public: 
	UFUNCTION(BlueprintCallable)
	void CreateNewSuite();

	UFUNCTION(BlueprintCallable)
	void DestroySuite();

	UFUNCTION(BlueprintCallable)
	void Delete(int32 InstanceID);

	UFUNCTION(BlueprintCallable)
	int32 AddModelByGUID(const FString &ResID, FTransform Transform, int32 BaseID = -1);
	
	UFUNCTION(BlueprintCallable)
	int32 AddModelByObjectID(int32 ResourceObjID, FTransform Transform, int32 BaseID = -1);

	UFUNCTION(BlueprintCallable)
	AResourceActor *SpawnResourceActor(int32 ResourceID, FTransform Transform);

	IObject *GetSDKObject(int32 ObjID);

	void OnUnregister() override;
protected:
	ISuite *Suite;
};

