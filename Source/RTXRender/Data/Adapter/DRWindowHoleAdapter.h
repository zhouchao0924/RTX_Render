// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DRHoleAdapter.h"
#include "DRWindowHoleAdapter.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UDRWindowHoleAdapter : public UDRHoleAdapter
{
	GENERATED_UCLASS_BODY()
public:
	/********************************Get Function*************************************/
	EDR_ObjectType GetObjectType()override { return EDR_ObjectType::EDR_Window; }

	UFUNCTION(BlueprintPure)
	int32 GetWindowObjId();
	
	UFUNCTION(BlueprintCallable)
	void SetWindowResId(const FString &ResId);
};
