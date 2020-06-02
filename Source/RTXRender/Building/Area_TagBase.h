// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once
#include "Blueprint/UserWidget.h"
#include "Area_TagBase.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UArea_TagBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetTextData"))
	void SetLabelTextData(const FString &InLabelText);
};
