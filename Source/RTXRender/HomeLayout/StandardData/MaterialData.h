// Fill out your copyright notice in the Description page of Project Settings.
// data model node for wall
#pragma once

#include "DataHandle.h"
#include "MaterialData.generated.h"

// Material UV
USTRUCT(Blueprintable)
struct FMatUVData : public FDataHandle
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FVector2D UVScale;
	UPROPERTY(BlueprintReadOnly)
	FVector UVOffset;
	UPROPERTY(BlueprintReadOnly)
	float UVAngle;
};


USTRUCT(Blueprintable)
struct FMaterialData : public FDataHandle
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString ResID;
	UPROPERTY(BlueprintReadOnly)
	int32 ModelID;
	UPROPERTY(BlueprintReadOnly)
	FString Url;
};
