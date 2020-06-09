// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRDoorHoleAdapter.h"


UDRDoorHoleAdapter::UDRDoorHoleAdapter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

/********************************Get Function*************************************/
int32 UDRDoorHoleAdapter::GetDoorObjId()
{
	return Obj->GetPropertyInt("Door");
}

int32 UDRDoorHoleAdapter::GetDoorFrameObjId()
{
	return Obj->GetPropertyInt("DoorFrame");
}

FVector2D UDRDoorHoleAdapter::GetForward()
{
	return ToVector2D(Obj->GetPropertyVector2D("Forward"));
}

/********************************Set Function*************************************/

void UDRDoorHoleAdapter::SetDoorResId(const FString &InResId)
{
	Obj->SetPropertyString("Door", TCHAR_TO_ANSI(*InResId));
}

void UDRDoorHoleAdapter::SetDoorFrameResId(const FString &InResId)
{
	Obj->SetPropertyString("DoorFrame", TCHAR_TO_ANSI(*InResId));
}