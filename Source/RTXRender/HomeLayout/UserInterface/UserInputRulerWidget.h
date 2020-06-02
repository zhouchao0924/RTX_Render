// Copyright? 2017 ihomefnt All Rights Reserved.



#pragma once



#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "UserInputRulerWidget.generated.h"



/**

 * 

 */

UCLASS()

class AJDR_API UUserInputRulerWidget : public UUserWidget

{

	GENERATED_BODY()

	

public:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetLabelTextLengthData"))

	void SetLabelTextLengthData(const float &Length);

	

public:

	UPROPERTY(BlueprintReadWrite)

		float DisLength = 0;

	UPROPERTY(BlueprintReadWrite)

		bool bUseDisLengthFlag = false;

	

};

