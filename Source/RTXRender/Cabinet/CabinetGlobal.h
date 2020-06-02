#pragma once
#include "CoreMinimal.h"
#include "base/common.h"
#include "CabinetSave.h"
/*柜体全局数据类
*/
class FCabinetGlobal
{
public:
	SINGLE(FCabinetGlobal);
	FCabinetGlobal();
	float GetTableHeight();
	float HangTopHeight() {return mHangHeight + mHangSetHeight; }
	TSkuData* GetSku(FVector2D CabinetSize,int32 CategoryId, int32 InstanceId = -1);
	bool ClearSkuDatas();
	FString GetTableSku(int32 nType );
	TSkuData* GetSkuData(int32 InstanceId);
public:
	TMap<int32,TSkuData*>	mSkuDatas;
	FString mCabinetAuxMx[eCabinetAuxObjecNum];
	FString mTableFrontMx;
	//单位毫米
	float	mFloorDepth; //地柜深度
	float	mFloorHeight; //地柜高度
	float	mFloorSetHeight;//地柜放置高度

	float	mHangSetHeight;	//吊柜放置高度
	float	mHangHeight;	//吊柜高度
	float	mHangDepth;	//吊柜深度

	float	mTableThickness;//台面厚度
	float	mTableFrontElapse;//台面外延
	float	mSkirtboardElapse;//踢脚板内缩
};
#define gCabinetGlobal FCabinetGlobal::Instance()
