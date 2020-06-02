// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DRWallAdapter.h"
#include "DRVirtualWallAdapter.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API UDRVirtualWallAdapter : public UDRWallAdapter
{
	GENERATED_UCLASS_BODY()
public:
	/********************************Get Function*************************************/
	EDR_ObjectType GetObjectType()override { return EDR_ObjectType::EDR_Edge; }

	UFUNCTION(BlueprintPure)
	int32 GetAssociationID();

	/********************************Set Function*************************************/
	UFUNCTION(BlueprintCallable)
	void SetAssociationID(const int32 InAssociationID);

};
