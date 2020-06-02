
#include "CabinetGlobal.h"
#include "VaRestRequestJSON.h"
#include "HouseFunction/ManageObjectC.h"

FCabinetGlobal::FCabinetGlobal()
{
	mFloorDepth = 600;
	mHangDepth = 350;
	mFloorSetHeight = 0;
	mFloorHeight = 805;
	mHangSetHeight = 1500;
	mTableFrontElapse = 15;
	mSkirtboardElapse = 0;
	mTableThickness = 15;
	mHangHeight = 770;

	mCabinetAuxMx[eCabinetAuxObjecSink] = "E76CF3264A20BDC5165DCC974E220BCC";//784D010647332CA84CCEDEBD2611239A
	mTableFrontMx = "62775768479C65958E4CDBB89F49A2C8";
}
float FCabinetGlobal::GetTableHeight()
{
	return mFloorHeight+ mTableThickness;
}

TSkuData* FCabinetGlobal::GetSku(FVector2D CabinetSize, int32 CategoryId, int32 InstanceId)
{
	TSkuData* pData = nullptr;
	for (auto p : mSkuDatas)
	{
		TSkuData* Data = p.Value;
		if (Data->mCategoryId == CategoryId && Data->mSize.Y < (CabinetSize.Y-20) && Data->mSize.X < (CabinetSize.X-20) )
		{
			if (InstanceId==Data->mSkuId)
			{
				return Data;
			}
			if((pData == nullptr)||(pData->mSize.X > Data->mSize.X && pData->mSize.Y > Data->mSize.Y) )
			{
				pData = Data;
			}
		}
	}
	if (pData == nullptr)
	{
		auto p = mSkuDatas.Find(InstanceId);
		if (p)
			return *p;
		for (auto curp : mSkuDatas)
		{
			TSkuData* Data = curp.Value;
			if (Data->mCategoryId == CategoryId)
				return Data;
		}
	}
	return pData;
}

bool FCabinetGlobal::ClearSkuDatas()
{
	for (auto p : mSkuDatas)
	{
		delete p.Value;
	}
	mSkuDatas.Empty();
	return false;
}
FString FCabinetGlobal::GetTableSku(int32 nType)
{
	FString Md5;
	TArray<UVaRestJsonObject*> SinkList = AManageObjectC::GetInstance()->GetBakewaterList();
	for (auto sku : SinkList)
	{
		int32 eType = sku->GetNumberField("type");
		if (eType == nType)
		{
			Md5 = sku->GetStringField("mxFileMD5");
			break;
		}
	}
	return Md5;
}

TSkuData * FCabinetGlobal::GetSkuData(int32 InstanceId)
{
	TSkuData** pRet = mSkuDatas.Find(InstanceId);
	if (pRet)
	{
		return *pRet;
	}
	return nullptr;
}
