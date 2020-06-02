// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DRActor.generated.h"

UENUM(BlueprintType)
enum class EVisibleChannel : uint8
{
	EBuildingStructChannel,
	EBuildingTopChannel,
	EModelChannel,
	EUnkownChannel,
};

UCLASS(BlueprintType)
class ADRActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable)
		virtual EVisibleChannel GetChannel() { return EVisibleChannel::EUnkownChannel; }
public:
	virtual void UpdateBuildingData() {};
	virtual void SaveSelf() {};
	virtual void LoadSelf() {};
};


