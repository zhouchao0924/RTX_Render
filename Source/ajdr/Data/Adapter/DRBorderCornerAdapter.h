// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DRCornerAdapter.h"
#include "DRBorderCornerAdapter.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API UDRBorderCornerAdapter : public UDRCornerAdapter
{
	GENERATED_UCLASS_BODY()	
public:
	EDR_ObjectType GetObjectType() override { return EDR_ObjectType::EDR_BorderCorner; }
	
	/********************************Get Function*************************************/

};
