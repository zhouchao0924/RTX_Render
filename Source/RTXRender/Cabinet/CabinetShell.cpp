
#include "CabinetShell.h"
#include "CabinetSdk.h"
#include "CabinetGlobal.h"
#include "ResourceMgr.h"

char* StrimName[2] = { "aux_trim_1","aux_trim_0" };

bool FCabinetShell::Init()
{
	mAnchor[0].Empty();
	mAnchor[1].Empty();
	if (mSetType == eAutoSetFloor)
	{
		for (int i = 0; i < mRes.mHardSkuMd5.Num(); ++i)
		{
			if (mRes.mCategoryIds[i] == 721 || mRes.mCategoryIds[i] == 722)
			{
				mRes.mCategoryIds.RemoveAt(i);
				mRes.mHardSkuMd5.RemoveAt(i);
				break;
			}
		}
	}

	for ( auto md5name: mRes.mHardSkuMd5 )
	{
		if (md5name.IsEmpty())
		{
			mAnchor[0].Add(mRes.mCabinetHeight);
			mAnchor[1].Add(mRes.mCabinetHeight);
			continue;
		}
		for (int i = 0; i < 2; ++i)
		{
			float Z = 0;
			TArray<FVector2D>	Holes;
			bool b=LoadStrim(md5name, Holes, 2, StrimName[i], Z);
			Z = mRes.mCabinetHeight- Z;
			if (mRes.mbSpline[eSplineTableBack])
			{
				Z +=gCabinetGlobal.mTableThickness;
			}
			mAnchor[i].Add(Z);
			if ( b )
			{
				mHoles[i] = Holes;
			}
		}
	}
	mTableSize = mSize;
	mSkinPos = FVector2D(0, 0);
	if (mRes.mTeplateId == 200102 || mRes.mTeplateId == 200103 || mRes.mTeplateId == 200104 || mRes.mTeplateId == 200105)
	{
		FVector SinkPos, SinkSize;
		FVector TablePos, TableSize;
		if (GetMeshSize(mRes.mBoyMd5, "mesh_color1", SinkSize, SinkPos))
		{
			mSkinPos.Set(SinkPos.X*mRes.mMeshScale.X, 0);
		}
		if (mRes.mTeplateId==200105 && GetMeshSize(mRes.mBoyMd5, "table", TableSize, mTablePos))
		{
			mTableSize.Set(TableSize.X*10, TableSize.Y*10);
		}
	}
	return true;
}

float FCabinetShell::ChangeHard(FString Sku, int CategoryId, int SkuId)
{
	int Idx = -1;
	for ( int i= 0; i<mRes.mCategoryIds.Num(); ++i )
	{
		if ( CategoryId == mRes.mCategoryIds[i] )
		{
			mRes.mHardSkuMd5[i] = Sku;
			mRes.mCategoryIds[i] = SkuId;
			Idx = i;
			break;
		}
	}
	CHECK_ERROR(Idx != -1);

	float fPosZ = 0;
	for (int i = 0; i < 2; ++i)
	{
		float Z = 0;
		TArray<FVector2D>	Holes;
		bool b = LoadStrim(mRes.mHardSkuMd5[Idx], Holes, 2, StrimName[i], Z);
		Z =  - Z;
		if (mRes.mbSpline[eSplineTableBack])
		{
			Z += gCabinetGlobal.mTableThickness;
		}
		mAnchor[i][Idx] = Z;
		if (b)
		{
			mHoles[i] = Holes;
		}
		if (i == 0)
			fPosZ = Z;
	}
	return fPosZ;
}
