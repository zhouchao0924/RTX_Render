// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DRHoleAdapter.h"
#include "DRDoorHoleAdapter.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UDRDoorHoleAdapter : public UDRHoleAdapter
{
	GENERATED_UCLASS_BODY()
public:
	/********************************Get Function*************************************/
	EDR_ObjectType GetObjectType()override { return EDR_ObjectType::EDR_DoorHole; }
	
	UFUNCTION(BlueprintPure)
	int32 GetDoorObjId();

	UFUNCTION(BlueprintPure)
	int32 GetDoorFrameObjId();

	UFUNCTION(BlueprintPure)
	FVector2D GetForward();

	/********************************Set Function*************************************/
	UFUNCTION(BlueprintCallable)
	void SetDoorResId(const FString &InResId);

	UFUNCTION(BlueprintCallable)
	void SetDoorFrameResId(const FString &InResId);
};
