
#pragma once

#include "DRActor.h"
#include "BuildingData.h"
#include "BuildingActor.generated.h"

UCLASS(BlueprintType)
class ABuildingActor : public ADRActor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Update(UBuildingData *Data);
	EVisibleChannel GetChannel() override { return  bTopCeil? EVisibleChannel::EBuildingTopChannel :EVisibleChannel::EBuildingStructChannel; }
public:
	UPROPERTY(Transient)
	UBuildingData *BuildingData;
	UPROPERTY(Transient)
	uint8		  bTopCeil:1;
};


