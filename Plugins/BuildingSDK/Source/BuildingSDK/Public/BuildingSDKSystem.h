// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BGCPoolManager.h"
#include "SolutionComponent.h"
#include "ResourceMgrComponent.h"
#include "BuildingSDKComponent.h"
#include "BuildingSceneComponent.h"
#include "BuildingSDKSystem.generated.h"

UCLASS(BlueprintType)
class BUILDINGSDK_API ABuildingSDKSystem : public AActor
{
	GENERATED_UCLASS_BODY()
public: 
	UFUNCTION(BlueprintCallable)
	static ABuildingSDKSystem *CreateInstance(UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable)
	static void DestroyInstance();

	UFUNCTION(BlueprintCallable)
	static ABuildingSDKSystem *GetSDKActor();
	
	UFUNCTION(BlueprintCallable)
	int32 GetMemoryUsed();
	
	UFUNCTION(BlueprintCallable)
	int32 GetObjectCount();
	
	UFUNCTION(BlueprintCallable)
	USolutionComponent *CreateSolution(const FString &Filename);

	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	TWeakPtr<IBuildingSDK> SDK;

	UPROPERTY(BlueprintReadOnly)
	USolutionComponent	 *SolutionComponent;

	UPROPERTY(BlueprintReadOnly)
	UResourceMgrComponent *ResourceMgr;

	UPROPERTY(BlueprintReadOnly)
	UBGCPoolManager		  *BGPoolManager;

	UPROPERTY(BlueprintReadOnly)
	UBuildingSDKComponent *SDKComponent;

	UPROPERTY(BlueprintReadOnly)
	UBuildingSceneComponent *BuildingSceneComponent;
};

