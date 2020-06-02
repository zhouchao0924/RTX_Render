// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "../DataNodes/UnknownNode.h"
#include "GameFramework/SaveGame.h"
#include "MaterialSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API UMaterialSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMatChannelInfo Mat;
	
};
