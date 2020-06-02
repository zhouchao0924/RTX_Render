#pragma once
#include "./base/common.h"
//#include "./Actor/CabinetNode.h"
struct TCabinetBase: public TRectBase
{
	int				Id;
	int32			mTeplateId;
	eAutoSetType	mSetType;
	float			mScaleZ;
	FCabinetRes	mRes;
};

class FCabinetShell: public TCabinetBase
{
public:
	bool Init();
	FVector2D	mTableSize;
	FVector2D	mSkinPos;
	FVector		mTablePos;
	TArray<FVector2D>	mHoles[2];
	TArray<float>		mAnchor[2];
	float ChangeHard(FString Sku, int CategoryId, int SkuId);
};
