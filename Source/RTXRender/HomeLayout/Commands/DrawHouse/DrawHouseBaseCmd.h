// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "../BaseCmd.h"
#include "../../UserInterface/DrawHouseWidget.h"
#include "DrawHouseBaseCmd.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UDrawHouseBaseCmd : public UBaseCmd
{
	GENERATED_BODY()
	
public:
	virtual void CommandWillStart();

protected:
	virtual void Go2DefaultCommand();

protected:
	UPROPERTY()
	UDrawHouseWidget *DrawHouseWidget;
};
