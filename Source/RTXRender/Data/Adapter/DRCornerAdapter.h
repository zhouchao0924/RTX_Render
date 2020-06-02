// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DataAdapter.h"
#include "DRCornerAdapter.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UDRCornerAdapter : public UDataAdapter
{
	GENERATED_UCLASS_BODY()	
public:
	EDR_ObjectType GetObjectType() override { return EDR_ObjectType::EDR_Corner; }
	
	/********************************Get Function*************************************/
	UFUNCTION(BlueprintPure)
	FVector2D GetLocaltion();

	UFUNCTION(BlueprintPure)
	TArray<int32> GetWalls();

};
