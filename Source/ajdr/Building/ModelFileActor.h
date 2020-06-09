// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DRActor.h"
#include "ModelFileComponent.h"
#include "ModelFileActor.generated.h"

UCLASS(BlueprintType)
class AModelFileActor :public ADRActor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Update(UBuildingData *Data);
	static void UpdateCompoennt(UModelFileComponent *InModelFileComponent, UBuildingData *Data);
	EVisibleChannel GetChannel() override { return EVisibleChannel::EModelChannel; }
protected:
	UPROPERTY(Transient, BlueprintReadWrite)
	UModelFileComponent *ModelComponentFile;
};


