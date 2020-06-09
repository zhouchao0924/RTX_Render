// Fill out your copyright notice in the Description page of Project Settings.
// data model node for wall
#pragma once

#include "DataHandle.h"
#include "CornerData.generated.h"

// Corner
USTRUCT(Blueprintable)
struct FCornerData : public FDataHandle
{
	GENERATED_BODY()

public:
	bool operator== (const FCornerData &InCorner) const 
	{
		return ID == InCorner.ID;
	}
	
	UPROPERTY(BlueprintReadOnly)
	FVector2D Position;

	UPROPERTY(BlueprintReadOnly)
	float ZPos;
};
