// Fill out your copyright notice in the Description page of Project Settings.
// region data structure

#pragma once

#include "MaterialNode.h"
#include "RegionNode.generated.h"

// 3d segment points struct
USTRUCT(Blueprintable)
struct F3DSegmentPointsStructCPP
{
	GENERATED_BODY()

public:
	TArray<FVector> SegmentPoints;
};

// wall area saved config
USTRUCT(Blueprintable)
struct FRegionNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> PointTagList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> VectorList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bMoadTwo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString TagName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsDelete;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D UV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D UVLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D UVRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bGround;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCeiling;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMaterialNode> Mat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> WallList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString RoomName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomUsageId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString RoomUsageName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Area;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasSGX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasTJX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SGXStyle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 TJXStyle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SGXMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString TJXMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsCW;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> SGXPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<F3DSegmentPointsStructCPP> TJXSegmentsPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bTJXHasDWFlag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Typeid;
};

