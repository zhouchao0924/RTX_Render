// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "IBuildingSDK.h"
#include "IMeshEditor.h"
#include "BuildingSDKDefs.h"
#include "ResourceMeshComponent.h"
#include "TriangulatedMeshComponent.generated.h"

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class BUILDINGSDK_API UTriangulatedMeshComponent : public UResourceMeshComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void CreatePolygon(const TArray<FVector2D> &Polygon, float Thickness);

	UFUNCTION(BlueprintCallable)
	void AddHole(const TArray<FVector2D> &Hole);
	
	UFUNCTION(BlueprintCallable)
	void SetSurface(const FString &MaterialUri);
};


