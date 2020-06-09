
#include "ComponentBaseWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "../../../EditorGameInstance.h"
#include "../../BuildingSystem.h"
#include "../FlueComponentPrimitive.h"
#include "../PillarComponentPrimitive.h"
#include "../BagRiserComponentPrimitive.h"
#include "../SewerComponentPrimitive.h"
#include "../ComponentManagerActor.h"


UComponentBaseWidget::UComponentBaseWidget(const FObjectInitializer& ObjectInitializer)
	:UUserWidget(ObjectInitializer)
	, ComponentManger(nullptr)
{

}

void UComponentBaseWidget::NativeConstruct()
{
	/*UTexture2D* Tex = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/Building/UMG_Component/Res/WhiteBlock.WhiteBlock'"));
	if (Tex)
	{
		Brush.SetResourceObject(Tex);

		Brush.TintColor = FLinearColor::White;
	}*/
	

}


TSharedRef<SWidget> UComponentBaseWidget::RebuildWidget()
{
	Canvas = SNew(SCanvas);

	
	return Canvas.ToSharedRef();
}


int32 UComponentBaseWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, 
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (ComponentManger)
	{
		ComponentManger->Update();
	}
	
	if (ComponentBasePrimitive)
	{
		FPaintContext InContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		ComponentBasePrimitive->Draw(InContext);
	}
	
	return LayerId;
}

void UComponentBaseWidget::CreateComponentPrimitive(EComponenetType type)
{
	if (!ComponentBasePrimitive)
	{
		switch (type)
		{

		case EComponenetType::_FLUE:
			ComponentBasePrimitive = NewObject<UFlueComponentPrimitive>();
			//ComponentBasePrimitive->SetBrush(Brush);
			break;
		case EComponenetType::_BagRiser:
			ComponentBasePrimitive = NewObject<UBagRiserComponentPrimitive>();
			//ComponentBasePrimitive->SetBrush(Brush);
			break;
		case EComponenetType::_Pillar:
			ComponentBasePrimitive = NewObject<UPillarComponentPrimitive>();
			//ComponentBasePrimitive->SetBrush(Brush);
			break;
		case EComponenetType::_Sewer:
			ComponentBasePrimitive = NewObject<USewerComponentPrimitive>();
			//ComponentBasePrimitive->SetBrush(Brush);
			break;
		default:
			break;
		}
		
	}
}

UComponentBasePrimitive* UComponentBaseWidget::GetComponentBase() const
{
	return ComponentBasePrimitive;
}

void UComponentBaseWidget::SetComponentDRActorID(int32 ID)
{
	if (ComponentBasePrimitive)
	{
		ComponentBasePrimitive->SetComponentDRActorID(ID);
	}
}

void UComponentBaseWidget::SetComponentManger(AComponentManagerActor* Manger)
{
	ComponentManger = Manger;
}
