// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRWallAdapter.h"
#include "DRCornerAdapter.h"
#include "Data/DRProjData.h"
//#include "ISuite.h"

UDRWallAdapter::UDRWallAdapter(const FObjectInitializer & ObjectIntializer)
	:Super(ObjectIntializer)
{
}

/********************************Get Function*************************************/
int32 UDRWallAdapter::GetRoomLeftId()
{
	return Obj->GetPropertyFloat("AreaLeft");
}

int32 UDRWallAdapter::GetRoomRightId()
{
	return Obj->GetPropertyFloat("AreaRight");
}

FVector UDRWallAdapter::GetForward()
{
	return ToVector(Obj->GetPropertyValue("Forward").Vec3Value());
}

FVector UDRWallAdapter::GetRight()
{
	return ToVector(Obj->GetPropertyValue("Right").Vec3Value());
}

int32 UDRWallAdapter::GetP0()
{
	return Obj->GetPropertyValue("P0").IntValue();
}

int32 UDRWallAdapter::GetP1()
{
	return Obj->GetPropertyValue("P1").IntValue();
}

TArray<int32> UDRWallAdapter::GetAllWallByP0()
{
	/*TArray<int32> Local_AllWallId;
	if (Obj->GetSuite())
	{
		IObject* Local_Corner = Obj->GetSuite()->GetObject(GetP0(), EObjectType::ECorner);
		if (Local_Corner)
		{
			kArray<int32> Local_KAllWallId = Local_Corner->GetPropertyIntArray("Walls");
			for (int32 Index = 0; Index < Local_KAllWallId.size(); ++Index)
			{
				Local_AllWallId.Add(Local_KAllWallId[Index]);
			}
		}
	}
	return Local_AllWallId;*/
	UDRCornerAdapter* Local_CornerAdapter = Cast<UDRCornerAdapter>(ProjData->GetAdapter(GetP0()));
	if (Local_CornerAdapter)
	{
		return Local_CornerAdapter->GetWalls();
	}
	return TArray<int32>();
}

TArray<int32> UDRWallAdapter::GetAllWallByP1()
{
	/*TArray<int32> Local_AllWallId;
	if (Obj->GetSuite())
	{
		IObject* Local_Corner = Obj->GetSuite()->GetObject(GetP1(), EObjectType::ECorner);
		if (Local_Corner)
		{
			kArray<int32> Local_KAllWallId = Local_Corner->GetPropertyIntArray("Walls");
			for (int32 Index = 0; Index < Local_KAllWallId.size(); ++Index)
			{
				Local_AllWallId.Add(Local_KAllWallId[Index]);
			}
		}
	}
	return Local_AllWallId;*/
	UDRCornerAdapter* Local_CornerAdapter = Cast<UDRCornerAdapter>(ProjData->GetAdapter(GetP1()));
	if (Local_CornerAdapter)
	{
		return Local_CornerAdapter->GetWalls();
	}
	return TArray<int32>();
}

FVector2D UDRWallAdapter::GetRange()
{
	return ToVector2D(Obj->GetPropertyValue("Range").Vec2Value());
}

int32 UDRWallAdapter::GetBottomSection()
{
	return Obj->GetPropertyValue("BottomSection").IntValue();
}

int32 UDRWallAdapter::GetTopSection()
{
	return Obj->GetPropertyValue("TopSection").IntValue();
}

int32 UDRWallAdapter::GetFrontSection()
{
	return Obj->GetPropertyValue("FrontSection").IntValue();
}

int32 UDRWallAdapter::GetBackSection()
{
	return Obj->GetPropertyValue("BackSection").IntValue();
}

int32 UDRWallAdapter::GetRightSection()
{
	return Obj->GetPropertyValue("RightSection").IntValue();
}

int32 UDRWallAdapter::GetLeftSection()
{
	return Obj->GetPropertyValue("LeftSection").IntValue();
}

FVector2D UDRWallAdapter::GetOriginalSartPos()
{
	if (Obj->GetSuite() && Obj->GetSuite()->GetObject(GetP0()))
	{
		return ToVector2D(Obj->GetSuite()->GetObject(GetP0())->GetPropertyValue("Location").Vec2Value());
	}
	return FVector2D(0, 0);
}

FVector2D UDRWallAdapter::GetOriginalEndtPos()
{
	if (Obj->GetSuite() && Obj->GetSuite()->GetObject(GetP1()))
	{
		return ToVector2D(Obj->GetSuite()->GetObject(GetP1())->GetPropertyValue("Location").Vec2Value());
	}
	return FVector2D(0, 0);
}