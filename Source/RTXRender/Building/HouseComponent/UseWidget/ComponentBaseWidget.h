#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../ComponentBasePrimitive.h"
#include "ComponentBaseWidget.generated.h"

UCLASS(BlueprintType)
class  AJDR_API UComponentBaseWidget:public UUserWidget
{
	GENERATED_BODY()
public:
	UComponentBaseWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct();
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, 
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;
	void CreateComponentPrimitive(EComponenetType type);

	UComponentBasePrimitive* GetComponentBase() const;
	void SetComponentDRActorID(int32 ID);
	void SetComponentManger(class AComponentManagerActor* Manger);
protected:
	TSharedPtr<SCanvas> Canvas;

	//UPROPERTY()
	//EComponenetState ComponenetState;
	UPROPERTY()
	FSlateBrush Brush;
	UPROPERTY()
	UComponentBasePrimitive* ComponentBasePrimitive;
	UPROPERTY()
	class AComponentManagerActor* ComponentManger;
};

