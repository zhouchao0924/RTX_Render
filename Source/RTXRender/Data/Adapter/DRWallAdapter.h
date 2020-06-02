// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DataAdapter.h"
#include "DRWallAdapter.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API UDRWallAdapter : public UDataAdapter
{
	GENERATED_UCLASS_BODY()
public:
	EDR_ObjectType GetObjectType() override { return EDR_ObjectType::EDR_Wall; }

	/********************************Get Function*************************************/
	UFUNCTION(BlueprintPure)
	int32 GetRoomLeftId();

	UFUNCTION(BlueprintPure)
	int32 GetRoomRightId();

	UFUNCTION(BlueprintPure)
	FVector GetForward();

	UFUNCTION(BlueprintPure)
	FVector GetRight();

	UFUNCTION(BlueprintPure)
	int32 GetP0();

	UFUNCTION(BlueprintPure)
	int32 GetP1();

	UFUNCTION(BlueprintPure)
	TArray<int32> GetAllWallByP0();

	UFUNCTION(BlueprintPure)
	TArray<int32> GetAllWallByP1();

	UFUNCTION(BlueprintPure)
	FVector2D GetRange();

	UFUNCTION(BlueprintPure)
	int32 GetBottomSection();

	UFUNCTION(BlueprintPure)
	int32 GetTopSection();

	UFUNCTION(BlueprintPure)
	int32 GetFrontSection();

	UFUNCTION(BlueprintPure)
	int32 GetBackSection();

	UFUNCTION(BlueprintPure)
	int32 GetRightSection();

	UFUNCTION(BlueprintPure)
	int32 GetLeftSection();

	UFUNCTION(BlueprintPure)
		FVector2D GetOriginalSartPos();

	UFUNCTION(BlueprintPure)
		FVector2D GetOriginalEndtPos();
};
