// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "DrawHouseWidget.generated.h"

/**
*
*/
UCLASS()
class RTXRENDER_API UDrawHouseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct();

	UFUNCTION(BlueprintImplementableEvent, Category = "Command")
	void Turn2ModifyPaperCmd();

protected:
	UPROPERTY()
	class UCommandMgr *DrawHouseCmdMgr;
};
