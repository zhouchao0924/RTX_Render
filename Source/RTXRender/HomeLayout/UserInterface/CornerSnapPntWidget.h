// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CornerSnapPntWidget.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UCornerSnapPntWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct();

	void InitCornerActor(class ACornerActor *InCornerActor);

	void ShowInnerCircel(bool bShow = false);

	void ShowOutterCircle(bool bShow = false);

	UFUNCTION(BlueprintImplementableEvent, Category = "Show Corner")
	FVector2D GetWidgetSize();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Show Corner")
	void ShowInnerCircleImpl();

	UFUNCTION(BlueprintImplementableEvent, Category = "Show Corner")
	void ShowOutterCircleImpl();

	UFUNCTION(BlueprintImplementableEvent, Category = "Show Corner")
	void ChangeWidgetPos(const FVector2D &NewPos);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	class ACornerActor *RelatedCorner;

	UPROPERTY(BlueprintReadOnly)
	bool bShowInnerCircle;

	UPROPERTY(BlueprintReadOnly)
	bool bShowOutterCircle;

};
