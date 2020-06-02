// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Building/DRActor.h"
#include "Building/BuildingData.h"
#include "DRSpotLightActor.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API ADRSpotLightActor : public ADRActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
		UBuildingData * BuildData;

public:
	ADRSpotLightActor();
	UFUNCTION(BlueprintCallable)
		virtual void  UpdateBuildingData() override;
	virtual void SaveSelf() override;
	virtual void LoadSelf() override;
	
	
};
