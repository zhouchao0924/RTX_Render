// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Building/DRActor.h"
#include "Building/BuildingData.h"
#include "DRPointLightActor.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API ADRPointLightActor : public ADRActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		UBuildingData * BuildData;

public:
	ADRPointLightActor();
	UFUNCTION(BlueprintCallable)
		virtual void  UpdateBuildingData() override;
	virtual void SaveSelf() override;
	virtual void LoadSelf() override;
	
	
	
};
