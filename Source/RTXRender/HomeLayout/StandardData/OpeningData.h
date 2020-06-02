// Fill out your copyright notice in the Description page of Project Settings.
// opening data structure for opening model

#pragma once

#include "../DataNodes/NodeTypes.h"
#include "DataHandle.h"
#include "OpeningData.generated.h"

// wall boolean saved config
USTRUCT(Blueprintable)
struct FOpeningData : public FDataHandle
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FVector2D Position;
	UPROPERTY(BlueprintReadOnly)
	float ZPos;
	UPROPERTY(BlueprintReadOnly)
	float LeftThick;
	UPROPERTY(BlueprintReadOnly)
	float RightThick;
	UPROPERTY(BlueprintReadOnly)
	float Width;
	UPROPERTY(BlueprintReadOnly)
	float Height;
	UPROPERTY(BlueprintReadOnly)
	int32 OwnerWallIndex;
	UPROPERTY(BlueprintReadOnly)
	bool bXFlip;
	UPROPERTY(BlueprintReadOnly)
	bool bYFlip;
	UPROPERTY(BlueprintReadOnly)
	EOpeningType OpeningType;

	FString OwnerWallID;
};

