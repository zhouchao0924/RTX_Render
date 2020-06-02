// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DRModelInstanceAdapter.h"
#include "DRWindowAdapter.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UDRWindowAdapter : public UDRModelInstanceAdapter
{
	GENERATED_UCLASS_BODY()
	
	UFUNCTION(BlueprintPure)
	FString GetName();

	UFUNCTION(BlueprintCallable)
	void SetName(const FString &Name);
	
};
