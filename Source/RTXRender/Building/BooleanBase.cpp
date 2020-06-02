// Copyright? 2017 ihomefnt All Rights Reserved.


#include "BooleanBase.h"


void UBooleanBase::NativeConstruct()
{
	Super::NativeConstruct();
	bIsMove = bIsSelect = bFlip = Test = Verticalflip = horizontalflip = false;
	HoleID = WallID = NewWallID= -1;
	WallID_2 = -1;
	Width = 0;
	Width_1 = Width_2 = 0;
	length = 60;
	IsFirst = false;
	Deltavector = FVector2D::ZeroVector;
	bISet = true;
	IsRemoveType = false;
	Thickness_1 = 0;
	Thickness_2 = 0;
	angle_1 = 0;
	angle_2 = 0;
	HolestatusType= EHolestatusType::EDRNormalHole;
}
