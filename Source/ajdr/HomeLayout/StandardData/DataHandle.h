// Fill out your copyright notice in the Description page of Project Settings.
// data model node for wall
#pragma once

#include "DataHandle.generated.h"

USTRUCT(Blueprintable)
struct FDataHandle
{
	GENERATED_BODY()

public:
	FDataHandle();
	
	UPROPERTY(BlueprintReadOnly)
	FString ID;

	UPROPERTY(BlueprintReadOnly)
	bool bValid;
};
