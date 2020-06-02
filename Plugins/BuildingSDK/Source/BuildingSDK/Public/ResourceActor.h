// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ResourceMeshComponent.h"
#include "ResourceActor.generated.h"

UCLASS(BlueprintType)
class BUILDINGSDK_API AResourceActor : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetResource(class USolutionComponent *SolutionComponent, int32 ID);

	UFUNCTION(BlueprintCallable)
	void LoadFile(const FString &Filename);
public:
	UPROPERTY(Transient, BlueprintReadOnly)
	UResourceMeshComponent *ResourceMeshComp;
};


