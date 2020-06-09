// Copyright© 2017 ihomefnt All Rights Reserved.


#include "DrawHouseBaseCmd.h"
#include "../CommandMgr.h"
#include "../../UserInterface/DrawHouseWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UDrawHouseBaseCmd::CommandWillStart()
{
	Super::CommandWillStart();

	DrawHouseWidget = nullptr;
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(),
		FoundWidgets, UDrawHouseWidget::StaticClass());
	if (FoundWidgets.Num() != 0)
	{
		DrawHouseWidget = Cast<UDrawHouseWidget>(FoundWidgets[0]);
	}
}

void UDrawHouseBaseCmd::Go2DefaultCommand()
{
	if (DrawHouseWidget)
	{
		DrawHouseWidget->Turn2ModifyPaperCmd();
	}
}
