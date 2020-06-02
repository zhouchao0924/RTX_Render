// Copyright© 2017 ihomefnt All Rights Reserved.


#include "AutoBuildAreaMsgBox.h"


bool UAutoBuildAreaMsgBox::bReserveRoom = false;

UAutoBuildAreaMsgBox::UAutoBuildAreaMsgBox(const FObjectInitializer& ObjectInitializer)
	:UUserWidget(ObjectInitializer)
{
	
}

void UAutoBuildAreaMsgBox::ReserveRoom(bool bInAcceptedValue)
{
	bReserveRoom = bInAcceptedValue;
}

bool UAutoBuildAreaMsgBox::DoReserveRoom()
{
	return bReserveRoom;
}
