// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRWindowAdapter.h"

UDRWindowAdapter::UDRWindowAdapter(const FObjectInitializer & ObjectIntializer)
	:Super(ObjectIntializer)
{
}

FString UDRWindowAdapter::GetName()
{
	return UTF8_TO_TCHAR(Obj->GetString("Name"));
}

void UDRWindowAdapter::SetName(const FString &Name)
{
	Obj->SetString("Name", TCHAR_TO_UTF8(*Name));
}


