// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRDoorAdapter.h"


UDRDoorAdapter::UDRDoorAdapter(const FObjectInitializer & ObjectIntializer)
	:Super(ObjectIntializer)
{
}

FString UDRDoorAdapter::GetName()
{
	return UTF8_TO_TCHAR(Obj->GetString("Name"));
}

void UDRDoorAdapter::SetName(const FString &Name)
{
	Obj->SetString("Name", TCHAR_TO_UTF8(*Name));
}
