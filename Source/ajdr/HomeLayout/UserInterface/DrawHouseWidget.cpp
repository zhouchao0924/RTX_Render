// Copyright© 2017 ihomefnt All Rights Reserved.

#include "DrawHouseWidget.h"
#include "../Commands/CommandMgr.h"


void UDrawHouseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DrawHouseCmdMgr = UCommandMgr::GetInstance();
}


