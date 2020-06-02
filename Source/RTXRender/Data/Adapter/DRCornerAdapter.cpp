// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRCornerAdapter.h"

UDRCornerAdapter::UDRCornerAdapter(const FObjectInitializer & ObjectIntializer)
	:Super(ObjectIntializer)
{
}

/********************************Get Function*************************************/
FVector2D UDRCornerAdapter::GetLocaltion()
{
	return ToVector2D(Obj->GetPropertyVector2D("Location"));
}

TArray<int32> UDRCornerAdapter::GetWalls()
{
	TArray<int32> Local_AllWallId;
	kArray<int32> Local_KAllWallId = Obj->GetPropertyIntArray("Walls");
	for (int32 Index = 0; Index < Local_KAllWallId.size(); ++Index)
	{
		Local_AllWallId.Add(Local_KAllWallId[Index]);
	}
	return Local_AllWallId;
}

