// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DrawLineWidget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class AJDR_API UDrawLineWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct()override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	    FVector2D CenterPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Y;

};
