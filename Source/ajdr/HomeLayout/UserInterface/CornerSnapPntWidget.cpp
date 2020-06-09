
#include "CornerSnapPntWidget.h"
#include "../SceneEntity/CornerActor.h"

void UCornerSnapPntWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bShowInnerCircle = bShowOutterCircle = false;
	RelatedCorner = nullptr;
	//bCanEverTick = true;
}

void UCornerSnapPntWidget::InitCornerActor(class ACornerActor *InCornerActor)
{
	RelatedCorner = InCornerActor;
}

void UCornerSnapPntWidget::ShowInnerCircel(bool bShow /*= false*/)
{
	bShowInnerCircle = bShow;

	ShowInnerCircleImpl();
}

void UCornerSnapPntWidget::ShowOutterCircle(bool bShow /*= false*/)
{
	bShowOutterCircle = bShow;

	ShowOutterCircleImpl();
}

void UCornerSnapPntWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GetVisibility() == ESlateVisibility::Visible)
	{
		if (RelatedCorner)
		{
			FVector ActorPos = RelatedCorner->GetActorLocation();
			FVector2D CornerScreenPos;
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(ActorPos, CornerScreenPos);
			ChangeWidgetPos(CornerScreenPos);
		}
	}
}