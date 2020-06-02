// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "RulerLabelWidget.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API URulerLabelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetLabelText"))
	void SetLabelText(const FString &InLabelText);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "SetLabelTextData"))
	void SetLabelTextData(const float &Length);

};
