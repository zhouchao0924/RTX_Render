// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRWindowHoleAdapter.h"

UDRWindowHoleAdapter::UDRWindowHoleAdapter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

int32 UDRWindowHoleAdapter::GetWindowObjId()
{
	return Obj->GetPropertyInt("Window");
}

void UDRWindowHoleAdapter::SetWindowResId(const FString &ResId)
{
	Obj->SetPropertyString("Window", TCHAR_TO_ANSI(*ResId));
}

