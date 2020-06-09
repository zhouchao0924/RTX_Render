// Fill out your copyright notice in the Description page of Project Settings.
// material node
#pragma once

#include "../DataNodes/NodeTypes.h"
#include "../DataNodes/MaterialNode.h"
#include "../DataNodes/SkirtingNode.h"
#include "../SceneEntity/LineWallActor.h"
#include "AreaCollection.generated.h"

USTRUCT(Blueprintable)
struct FWallCollectionHandle
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FMaterialNode WallMaterial;

	//UFUNCTION(BlueprintPure)
	float GetAreaOfWall() const;

	UPROPERTY(BlueprintReadWrite)
	EWallSurfaceType FaceType;

	FWallCollectionHandle& operator+=(const FWallCollectionHandle &OtherCollection);

public:
	double WallArea;
};

USTRUCT(Blueprintable)
struct FSkirtingCollectionHandle
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FString SkirtingModelID;

	UPROPERTY(BlueprintReadOnly)
	FString SkirtingMaterialID;

	UPROPERTY(BlueprintReadOnly)
	FSkirtingSegNode SkirtingSegNode;

	UPROPERTY(BlueprintReadOnly)
	ESkirtingType SkirtingType;

	//UFUNCTION(BlueprintPure)
	float GetSkirtingLength() const;

	FSkirtingCollectionHandle& operator+=(const FSkirtingCollectionHandle &OtherCollection);

public:
	double SkirtingLength;
};

USTRUCT(Blueprintable)
struct FRegionCollection
{
	GENERATED_BODY()
public:
	const TArray<FWallCollectionHandle>& CalculateWallCollections();

	const TArray<FSkirtingCollectionHandle>& CalculateSkirtingCollections();

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FWallCollectionHandle> WallCollections;
	UPROPERTY(BlueprintReadOnly)
	TArray<FSkirtingCollectionHandle> SkirtingCollections;
};
