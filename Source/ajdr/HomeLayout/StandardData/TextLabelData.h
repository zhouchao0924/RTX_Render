// Fill out your copyright notice in the Description page of Project Settings.
// data model node for wall
#pragma once

#include "DataHandle.h"
#include "TextLabelData.generated.h"

// Corner
USTRUCT(Blueprintable)
struct FTextLabelData : public FDataHandle
{
	GENERATED_BODY()

public:
	FTextLabelData()
	{
		RulerPixelInPic = 1.0f;
		RulerLength = 10.0f;
	}

	UPROPERTY(BlueprintReadOnly)
	float RulerLength;

	UPROPERTY(BlueprintReadOnly)
	float RulerPixelInPic;
};
