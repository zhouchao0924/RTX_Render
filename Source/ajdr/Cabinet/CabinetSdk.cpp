
#include "CabinetSdk.h"
#include "CabinetMgr.h"
#include "../Runtime/Engine/Classes/Kismet/KismetStringLibrary.h"
#include "CabinetArea.h"
#include "VaRestRequestJSON.h"
#include "HouseFunction/ManageObjectC.h"
#include "AutoSet/CabinetSetApi.h"
#include "EditorGameInStance.h"
#include "HouseFunction/HouseFunctionLibrary.h"
#include "AJBlueprintFunctionLibrary.h"
FString GetMd5MaterialFromJson(UVaRestJsonObject* componentData)
{
	FString ret;
	UVaRestJsonObject* attrJson = componentData->GetObjectField("attrJson");
	if (attrJson)
	{
		UVaRestJsonObject* colorjson = attrJson->GetObjectField("color");
		if (colorjson)
		{
			TArray<UVaRestJsonObject*> propertyValueList = colorjson->GetObjectArrayField("propertyValueList");
			for ( int i=0; i< propertyValueList.Num(); ++i )
			{
				if ( propertyValueList[i]->GetBoolField("selectFlag") )
				{
					ret = propertyValueList[i]->GetStringField("mxFileMD5");
				}
			}
		}
	}
	return ret;
}

FCabinetRes Json2CabinetRes(UVaRestJsonObject* p, FCabinetArea* pArea, int nType)
{
	FCabinetRes res;
	res.mInstanceId = p->GetNumberField("templateId");
	res.mTeplateId = p->GetNumberField("categoryId");
	res.categoryName = p->GetStringField("categoryName");

	FVector size;
	bool is;
	FString str = p->GetStringField("templateSize");
	UKismetStringLibrary::Conv_StringToVector(str, size, is);
	res.mSize.Set(Float2Int2(size.X), Float2Int2(size.Y));
	res.mTurnBSize = GetTurnSize(res.mTeplateId);
	res.mSizeMin.X = Float2Int2(p->GetNumberField("minLength"));
	res.mSizeMax.X = Float2Int2(p->GetNumberField("maxLength"));
	res.mSizeMin.Y = Float2Int2(p->GetNumberField("minWidth"));
	res.mSizeMax.Y = Float2Int2(p->GetNumberField("maxWidth"));
	res.mCabinetHeight = size.Z;

	bool settingOutFlag = p->GetBoolField("settingOutFlag");

	TArray<UVaRestJsonObject*> componentList = p->GetObjectArrayField("componentList");
	for (auto cm : componentList)
	{
		int32 categoryId = p->GetNumberField("categoryId");
		FString categoryName = cm->GetStringField("categoryName");
		if (categoryName.Find(TEXT("踢脚板")) != -1)
		{
			pArea->mCabinetGroup[nType].mSplinesMesh[eSplineFloorBottom] = cm->GetStringField("mxFileMD5");
			pArea->mCabinetGroup[nType].mSplinesMaterial[eSplineFloorBottom] = GetMd5MaterialFromJson(cm);
			res.mbSpline[eSplineFloorBottom] = true;
		}
		else if (categoryName.Find(TEXT("顶角线")) != -1)
		{
			pArea->mCabinetGroup[nType].mSplinesMesh[eSplineHangTop] = cm->GetStringField("mxFileMD5");
			pArea->mCabinetGroup[nType].mSplinesMaterial[eSplineHangTop] = GetMd5MaterialFromJson(cm);
			res.mbSpline[eSplineHangTop] = true;
		}
		else if (categoryName.Find(TEXT("台面")) != -1 && settingOutFlag )
		{
			pArea->mCabinetGroup[nType].mSplinesMesh[eSplineTableBack] = cm->GetStringField("mxFileMD5");
			pArea->mCabinetGroup[nType].mSplinesMaterial[eSplineTableBack] = GetMd5MaterialFromJson(cm);
			pArea->mCabinetGroup[nType].mTableMeterial = pArea->mCabinetGroup[nType].mSplinesMaterial[eSplineTableBack];
			res.mbSpline[eSplineTableBack] = true;
		}
		else if (categoryName.Find(TEXT("柜身")) != -1)
		{
			res.mBoyMd5 = cm->GetStringField("mxFileMD5");
			FVector componentSize;
			bool OutIsValid;
			UKismetStringLibrary::Conv_StringToVector(cm->GetObjectField("customJson")->GetStringField("componentSize"), componentSize, OutIsValid);
			if (OutIsValid) 
			{
				res.mSize.Set(Float2Int2(componentSize.X), Float2Int2(componentSize.Y));
				res.mCabinetHeight = componentSize.Z;
			}
			res.mTableSize = res.mSize;

			auto stransform = cm->GetObjectField("customJson")->GetStringField("transform");
			FTransform outtransform;
			if (UAJBlueprintFunctionLibrary::Conv_StringToTransform(stransform, outtransform))
			{
				res.mMeshScale = outtransform.GetScale3D();
				float Z= outtransform.GetLocation().Z * 10;
				res.mCabinetHeight += Z;
			}

			auto meshNameList= cm->GetObjectField("customJson")->GetObjectArrayField("meshNameList");
			for (auto meshName : meshNameList)
			{
				FString  ssName = meshName->GetStringField("meshName");
				if (ssName == "mesh_color1")
				{
					FVector TableSize;
					bool OutIsValidTmp;
					FString sSize = meshName->GetStringField("meshSize");
					UKismetStringLibrary::Conv_StringToVector(sSize, TableSize, OutIsValidTmp);
					if (OutIsValidTmp)
					{
						res.mTableSize.Set(Float2Int2(TableSize.X*res.mMeshScale.X), Float2Int2(TableSize.Y*res.mMeshScale.Y));
					}

				}
			}
		}
	}
	res.mSizeMin.X = TinyMin(res.mSize.X, res.mSizeMin.X);
	res.mSizeMin.Y = TinyMin(res.mSize.Y, res.mSizeMin.Y);
	res.mSizeMax.X = TinyMax(res.mSize.X, res.mSizeMax.X);
	res.mSizeMax.Y = TinyMax(res.mSize.Y, res.mSizeMax.Y);
	TArray<UVaRestJsonObject*> hardSkuList = p->GetObjectArrayField("hardSkuList");
	for (auto sku : hardSkuList)
	{
		TSkuData skuData;
		skuData.mSkuId = sku->GetNumberField("skuId");
		skuData.mCategoryId = sku->GetNumberField("drCategoryId");
		skuData.mName = sku->GetStringField("name");
		skuData.mMxFileMD5 = sku->GetStringField("mxFileMD5");
		res.mSkuDatas.Add(skuData);
		res.mHardSkuMd5.Add(sku->GetStringField("mxFileMD5"));
		res.mCategoryIds.Add(sku->GetNumberField("skuId"));
	}

	return res;
}
//////////////////////////////////////////////////////////////////////////
bool UCabinetSdk::AutoSetCabinets(int nType)
{
	return true;
}
bool UCabinetSdk::SetBreakWaterJson(UVaRestJsonObject* JsonList)
{
	return false;
}
FString UCabinetSdk::GetAreaTag()
{
	FCabinetArea* pArea = gCabinetAreaMgr.GetCurArea();
	if (pArea)
		return pArea->mName;
	return FString();
}
TArray<float> UCabinetSdk::ReplaceCabinet(int32 CabinetId, int nSetType, UVaRestJsonObject* pJsonTemplate, FString RoomTag)
{
	TArray<float> PosZRet;
	FCabinetArea* pArea = gCabinetAreaMgr.GetArea(RoomTag);
	if (pArea == nullptr)
		return PosZRet;
	if (!(nSetType >= 0 && nSetType < eAutoSetNum))
		return PosZRet;
	FCabinetShell* pCabinetSet = pArea->mCabinetGroup[nSetType].GetCabinet(CabinetId);
	if (pCabinetSet == nullptr)
		return PosZRet;
	FCabinetRes res = Json2CabinetRes(pJsonTemplate, pArea, nSetType);
	if (res.mHardSkuMd5.Num()==0)
		return PosZRet;

	FCabinetRes resOld = pCabinetSet->mRes;
	pCabinetSet->mRes = res;
	pCabinetSet->mRes.mSize = resOld.mSize;
	pCabinetSet->mRes.mCabinetHeight = resOld.mCabinetHeight;

	if (nSetType == eAuotSetCook)
	{
		nSetType = eAutoSetFloor;
	}
	for (int i = 0; i < res.mHardSkuMd5.Num(); ++i)
	{
		float fPosZ = pArea->mCabinetGroup[nSetType].SetHardModle(res.mHardSkuMd5[i], CabinetId, res.mCategoryIds[i], res.mCategoryIds[i]);
		PosZRet.Add(fPosZ);
	}
	return PosZRet;
}

bool UCabinetSdk::SetHangTop(int nType, float PosZ, FString Tag)
{
	FCabinetArea* pArea = gCabinetAreaMgr.GetArea(Tag);
	CHECK_ERROR(pArea);
	CHECK_ERROR(nType >= 0 && nType < eAutoSetNum);
	if (nType == eAuotSetCook)
	{
		pArea->mCabinetGroup[eAutoSetHang].SetHangTop(PosZ);
	}
	else
	{
		pArea->mCabinetGroup[nType].SetHangTop(PosZ);
	}
	return true;
}
bool UCabinetSdk::SetSplineMesh(FString MxFileName, ESplineTypeBlue SplineType, int nType, FString Tag)
{
	FCabinetArea* pArea = gCabinetAreaMgr.GetArea(Tag);
	CHECK_ERROR(pArea);
	CHECK_ERROR(nType >= 0 && nType < eAutoSetNum);
	if (nType == eAuotSetCook)
	{
		pArea->SetSplineMesh(MxFileName, (eSplineType)SplineType, eAutoSetFloor);
		pArea->SetSplineMesh(MxFileName, (eSplineType)SplineType, eAutoSetHang);
	}
	else
	{
		pArea->SetSplineMesh(MxFileName, (eSplineType)SplineType, (eAutoSetType)nType);
	}
	return true;
}
float UCabinetSdk::SetHardModle(FString MxFileName,int32 CabinetId, int SkuId, int CategoryId, int nType,int nOldSkuId, FString RoomTag)
{
	FCabinetArea* pArea = gCabinetAreaMgr.GetArea(RoomTag);
	CHECK_ERROR(pArea);
	CHECK_ERROR(nType >= 0 && nType < eAutoSetNum);
	if (nType == eAuotSetCook)
	{
		if (CategoryId == 721 || CategoryId == 726)
		{
			pArea->mSkus[0].mSku.mSkuId = SkuId;
			pArea->mSkus[0].mSku.mMxFileMD5 = MxFileName;
			float Z = pArea->mSkus[0].LoadSku();
			pArea->mCabinetGroup[eAutoSetFloor].GenExtern();
			return Z;
		}
		else if( CategoryId == 722 )
		{
			pArea->mSkus[1].mSku.mSkuId = SkuId;
			pArea->mSkus[0].mSku.mMxFileMD5 = MxFileName;
			auto ppCabinet = pArea->mCabinetGroup[eAutoSetFloor].mCabinets.Find(CabinetId);
			if (ppCabinet == nullptr)
				return (*ppCabinet)->mRes.mCabinetHeight+ gCabinetGlobal.mTableThickness;
			return gCabinetGlobal.mFloorHeight+gCabinetGlobal.mTableThickness;
		}
		return pArea->mCabinetGroup[eAutoSetFloor].SetHardModle(MxFileName, CabinetId, nOldSkuId, SkuId);
	}
	return pArea->mCabinetGroup[nType].SetHardModle(MxFileName, CabinetId, nOldSkuId, SkuId);
}
bool UCabinetSdk::SetSplineMaterial(FString MxFileName, ESplineTypeBlue SplineType, int nType, FString Tag)
{
	FCabinetArea* pArea = gCabinetAreaMgr.GetArea(Tag);
	CHECK_ERROR(pArea);
	CHECK_ERROR(nType >= 0 && nType < eAutoSetNum);
	if ( nType == eAuotSetCook )
	{
		pArea->SetSplineMaterial(MxFileName, (eSplineType)SplineType,eAutoSetFloor );
		pArea->SetSplineMaterial(MxFileName, (eSplineType)SplineType,eAutoSetHang );
	}
	else
	{
		pArea->SetSplineMaterial(MxFileName, (eSplineType)SplineType,(eAutoSetType)nType);
	}
	return true;
}

bool UCabinetSdk::SetSplineVisble(int SplineType, bool bVisble, int nType, FString Tag)
{
	FCabinetArea* pArea = gCabinetAreaMgr.GetArea(Tag);
	CHECK_ERROR(pArea);
	CHECK_ERROR(nType >= 0 && nType < eAutoSetNum);
	if (nType == eAuotSetCook)
	{
		pArea->SetSplineVisble((eSplineType)SplineType, bVisble, eAutoSetFloor);
		pArea->SetSplineVisble((eSplineType)SplineType, bVisble, eAutoSetHang);
	}
	else
	{
		pArea->SetSplineVisble((eSplineType)SplineType, bVisble, eAutoSetType(nType));
	}
	return true;
}

bool UCabinetSdk::DeleteSpline(FString Tag)
{
	return gCabinetAreaMgr.DeleteArea(Tag);
}

bool UCabinetSdk::DeleteSplineType(FString Tag, int nType)
{
	FCabinetArea* pArea = gCabinetAreaMgr.GetArea(Tag);
	CHECK_ERROR(pArea);
	CHECK_ERROR(nType >= 0 && nType < eAutoSetNum);
	if ( nType ==0 )
	{
		pArea->mCabinetGroup[eAutoSetFloor].Clear();
		pArea->mCabinetGroup[eAutoSetHang].Clear();
	}
	else if (nType == eAutoSetToilet)
	{
		pArea->mCabinetGroup[nType].Clear();
		pArea->mCabinetGroup[eAutoSetToiletHang].Clear();

	}
	else if (nType == eAutoSetWardrobe)
	{
		pArea->mCabinetGroup[nType].Clear();
		pArea->mCabinetGroup[eAutoSetWardorbeHang].Clear();
	}
	else if (nType == eAutoSetCoatroom)
	{
		pArea->mCabinetGroup[nType].Clear();
		pArea->mCabinetGroup[eAutoSetCoatroomHang].Clear();
	}
	else
	{
		pArea->mCabinetGroup[nType].Clear();
	}
	return true;
}

bool UCabinetSdk::DeleteActor(FString Tag, int nType, int Actorid)
{
	FCabinetArea* pArea = gCabinetAreaMgr.GetArea(Tag);
	CHECK_ERROR(pArea);
	CHECK_ERROR(nType >= 0 && nType < eAutoSetNum);

	if (nType > 0)
	{	
		for (auto i : pArea->mCabinetGroup[nType].mCabinets)
		{
			if (i.Value->Id == Actorid)
			{
				pArea->mCabinetGroup[nType].mCabinets.Remove(i.Key);
				break;
			}
		}	
	}

	return false;
}

bool  UCabinetSdk::ExchangeActor(FString oldTag, FString newTag, int nType, int Actorid)
{
	FCabinetArea* oldpArea = gCabinetAreaMgr.GetArea(oldTag);
	CHECK_ERROR(oldpArea);
	CHECK_ERROR(nType >= 0 && nType < eAutoSetNum);

	FCabinetArea* newpArea = gCabinetAreaMgr.GetArea(newTag);
	CHECK_ERROR(newpArea);
	FCabinetShell* Temp = nullptr;
	if (nType > 0)
	{
		for (auto i : oldpArea->mCabinetGroup[nType].mCabinets)
		{
			if (i.Value->Id == Actorid)
			{
				Temp = i.Value;
				oldpArea->mCabinetGroup[nType].mCabinets.Remove(i.Key);
				break;
			}
		}
	}
	if(Temp)
		newpArea->mCabinetGroup[nType].mCabinets.Add(Actorid, Temp);

	return true;
}

bool UCabinetSdk::LoadPreSaveData(int nType, FString Tag)
{
	eAutoSetType eType = CategoryId2SetType(nType,true);
	FCabinetArea* pArea = gCabinetAreaMgr.GetArea(Tag);
	CHECK_ERROR(pArea);
	CHECK_ERROR(eType >= 0 && eType < eAutoSetNum);
	if (eType == eAutoSetFloor)
	{
		pArea->LoadPreSaveSku();
		pArea->mCabinetGroup[eAutoSetHang].LoadPreSaveData();
	}
	else if (eType == eAutoSetWardrobe)
	{
		pArea->mCabinetGroup[eAutoSetWardorbeHang].LoadPreSaveData();
	}
	else if (eType == eAutoSetToilet)
	{
		pArea->mCabinetGroup[eAutoSetToiletHang].LoadPreSaveData();
	}
	else if (eType == eAutoSetCoatroom)
	{
		pArea->mCabinetGroup[eAutoSetCoatroomHang].LoadPreSaveData();
	}
	return pArea->mCabinetGroup[eType].LoadPreSaveData();
}
bool UCabinetSdk::SetLoftingandSKU(TMap<int32, FString> Sku, TMap<ESplineTypeBlue, FString> Lofting)
{
	return false;
}



bool UCabinetSdk::SetSkuData(UVaRestJsonObject* JsonList)
{
	return false;
}

int32 UCabinetSdk::NewCabinetId()
{
	return gCabinetMgr.GenId();
}

bool UCabinetSdk::SaveCabinetMgr(FCabinetAreaMgrSave & Datas)
{
	gCabinetAreaMgr.SaveCabinetMgr(Datas);
	return false;
}

bool UCabinetSdk::LoadCabinetMgr(const FCabinetAreaMgrSave & Datas)
{
	gCabinetAreaMgr.LoadCabinetMgr(Datas);
	return false;
}

bool UCabinetSdk::LoadCabinetNDAMgr(const FCabinetAreaMgrSave & Datas)
{
	gCabinetAreaMgr.LoadCabinetMgr(Datas,true);
	return false;
}
bool SetTemplateCabinetJson(TArray<UVaRestJsonObject*>& JsonData, int nType)
{
	FCabinetArea* pArea = gCabinetAreaMgr.GetCurArea();
	CHECK_ERROR(pArea);

	FAutoSetBase* pAutoSet = NewAutoSet(eAutoSetType(nType));
	CHECK_ERROR(pAutoSet);

	for (auto p : JsonData)
	{
		FCabinetRes res = Json2CabinetRes(p,pArea,nType);
		pAutoSet->InitRes(res);
	}
	return true;
}
bool UCabinetSdk::SetCustomFurTypJsonData(TArray<UVaRestJsonObject*> JsonData, int nType)
{
	//FCabinetArea* pArea = gCabinetAreaMgr.GetCurArea();
	//CHECK_ERROR(pArea);

	//if ( nType == 0 )
	//{
	//	SetTemplateCabinetJson(JsonData, eAutoSetFloor);
	//	SetTemplateCabinetJson(JsonData, eAutoSetHang);
	//}
	//else
	//{
		//SetTemplateCabinetJson(JsonData, nType);
	//}
	return true;
}

int32 GetCabinetByCabinetId(TArray<FCabinetShell*>& CabinetFloors, int id)
{
	for (int i = 0; i < CabinetFloors.Num(); ++i)
	{
		if (CabinetFloors[i]->Id == id)
			return i;
	}
	return -1;
}
bool GetCabinets(int nType,TMap<int32, FCabinetShell*>& Cabinets)
{
	if (eAutoSetHang == nType)
		return true;

	FCabinetArea* pArea = gCabinetAreaMgr.GetCurArea();
	CHECK_ERROR(pArea);

	TArray<FCabinetShell*> CabinetFloors;
	if (eAutoSetFloor == nType )
	{
		TArray<FCabinetShell*> CabinetFloors1;
		pArea->GetCabinets(CabinetFloors, (eAutoSetType)eAutoSetFloor);
		pArea->GetCabinets(CabinetFloors1, (eAutoSetType)eAutoSetHang);
		CabinetFloors += CabinetFloors1;
	}
	else if (eAutoSetWardrobe == nType )
	{
		TArray<FCabinetShell*> CabinetFloors1;
		pArea->GetCabinets(CabinetFloors, (eAutoSetType)eAutoSetWardrobe);
		pArea->GetCabinets(CabinetFloors1, (eAutoSetType)eAutoSetWardorbeHang);
		CabinetFloors += CabinetFloors1;
	}
	else if (eAutoSetToilet == nType )
	{
		TArray<FCabinetShell*> CabinetFloors1;
		pArea->GetCabinets(CabinetFloors, (eAutoSetType)eAutoSetToilet);
		pArea->GetCabinets(CabinetFloors1, (eAutoSetType)eAutoSetToiletHang);
		CabinetFloors += CabinetFloors1;
	}
	else if (eAutoSetCoatroom == nType )
	{
		TArray<FCabinetShell*> CabinetFloors1;
		pArea->GetCabinets(CabinetFloors, (eAutoSetType)eAutoSetCoatroom);
		pArea->GetCabinets(CabinetFloors1, (eAutoSetType)eAutoSetCoatroomHang);
		CabinetFloors += CabinetFloors1;
	}
	else
	{
		pArea->GetCabinets(CabinetFloors, (eAutoSetType)nType);
	}

	int SinkRightIdx = -1;
	int ChimeryRightIdx = -1;
	if (eAutoSetFloor == nType)
	{
		SinkRightIdx = GetCabinetByCabinetId(CabinetFloors, pArea->mSkus[0].mRightCabinetId);
		ChimeryRightIdx = GetCabinetByCabinetId(CabinetFloors, pArea->mSkus[1].mRightCabinetId);
	}
	TArray<FCustomData> ListData;
	for (auto p : CabinetFloors)
	{
		if (Cabinets.Find(p->Id))
			continue;
		FVector Scale3d(p->mScale.X, p->mScale.Y, p->mScaleZ);
		float fAngle = atan2(p->mDir.Y, p->mDir.X) * 180 / PI;
		FVector Pos = FVector(p->mPos.X, p->mPos.Y, p->mPosZ)*0.1f;

		FCustomData Data;
		Data.Id = p->Id;
		Data.Categoryid = p->mRes.mTeplateId;
		Data.TeplateId = p->mRes.mInstanceId;
		Data.Pos = Pos;
		Data.Scale = Scale3d;
		Data.fRoteZ = fAngle;
		Data.Meshvisible = 3;
		Data.Type = ECustomFurType(p->mSetType);
		for ( int i=0; i<p->mRes.mCategoryIds.Num();++i )
		{
			if ( eAutoSetFloor == nType && (p->mRes.mSkuDatas[i].mCategoryId ==721 || p->mRes.mSkuDatas[i].mCategoryId == 722) )
				continue;

			FVector  pos(0, 0, 0);
			if (p->mRes.mSkuDatas[i].mCategoryId == 721 || p->mRes.mSkuDatas[i].mCategoryId == 722 || p->mRes.mSkuDatas[i].mCategoryId == 726)
			{
				pos = FVector(p->mSkinPos.X*p->mRes.mMeshScale.X, 0, p->mAnchor[0][i]);
			}
			if (p->mRes.mTeplateId == 200105)
			{
				pos.Z += (880 - 2250);
			}
			if (eAuotSetCook == nType || eAutoSetToilet== nType || eAutoSetWashCabinet==nType)
			{
				pos.Z += 15;
			}
			Data.SKU_Rota.Add(0);
			Data.SKU_Pos.Add(pos);
			Data.SKU_ID.Add(p->mRes.mCategoryIds[i]);
			Data.SKU_PosType.Add(true);
		}
		if (eAutoSetFloor == nType)
		{
			if ( pArea->mSkus[0].mLeftCabinetId == p->Id && pArea->mSkus[0].mRightCabinetId != p->Id)
			{
				Data.Meshvisible = 2;
			}
			if (pArea->mSkus[0].mRightCabinetId == p->Id && pArea->mSkus[0].mLeftCabinetId != p->Id)
			{
				Data.Meshvisible = 1;
			}
			bool SinkAttach = false;
			if (pArea->mSkus[0].mLeftCabinetId == p->Id && pArea->mSkus[0].mRightCabinetId == p->Id)
				SinkAttach = true;
			if(p->mRes.mTeplateId== 200032 && (pArea->mSkus[0].mLeftCabinetId == p->Id || pArea->mSkus[0].mRightCabinetId == p->Id) )
				SinkAttach = true;
			if (pArea->mSkus[0].mLeftCabinetId == p->Id && (SinkRightIdx==-1 || CabinetFloors[SinkRightIdx]->mRes.mTeplateId != 200032))
				SinkAttach = true;
			if(SinkAttach)
			{
				int nDir = pArea->mSkus[0].mLeftCabinetId == p->Id ? -1:1;
				float LocalPos = nDir*FMath::Abs(((pArea->mSkus[0].mPos - p->mPos)|p->mDir)) / p->mScale.X;
				FVector  pos(LocalPos, 0, pArea->mSkus[0].mAnchor[0] );
				if (p->mRes.mTeplateId == 200036)
					pos.Y = 219;
				pos.Z = p->mRes.mCabinetHeight + 15 - pos.Z;
				pos.Z += (p->mRes.mbSpline[eSplineTableBack] ? gCabinetGlobal.mTableThickness : 0);
				float SkuAngle = atan2(pArea->mSkus[0].mDir.Y, pArea->mSkus[0].mDir.X) * 180 / PI - fAngle;
				Data.SKU_Rota.Add(SkuAngle);
				Data.SKU_Pos.Add(pos);
				Data.SKU_ID.Add(pArea->mSkus[0].mSku.mSkuId);
				Data.SKU_PosType.Add(false);
			}
			bool ChimeryAttach = false;
			if (pArea->mSkus[1].mLeftCabinetId == p->Id && pArea->mSkus[1].mRightCabinetId == p->Id)
				ChimeryAttach = true;
			if (p->mRes.mTeplateId == 200033 && (pArea->mSkus[1].mLeftCabinetId == p->Id || pArea->mSkus[1].mRightCabinetId == p->Id))
				ChimeryAttach = true;
			if (pArea->mSkus[1].mLeftCabinetId == p->Id && (ChimeryRightIdx==-1|| CabinetFloors[ChimeryRightIdx]->mRes.mTeplateId != 200033))
				ChimeryAttach = true;
			if (ChimeryAttach)
			{
				int nDir = pArea->mSkus[1].mLeftCabinetId == p->Id ? -1 : 1;
				float LocalPos = nDir*FMath::Abs((pArea->mSkus[1].mPos - p->mPos) | p->mDir)/p->mScale.X;
				FVector  pos(LocalPos,0, p->mRes.mCabinetHeight+15 + gCabinetGlobal.mTableThickness);
				float SkuAngle = atan2(pArea->mSkus[1].mDir.Y, pArea->mSkus[1].mDir.X) * 180 / PI - fAngle;
				if (p->mRes.mTeplateId == 200036)
					pos.Y = 219;
				Data.SKU_Rota.Add(SkuAngle);
				Data.SKU_Pos.Add(pos);
				Data.SKU_ID.Add(pArea->mSkus[1].mSku.mSkuId);
				Data.SKU_PosType.Add(false);
			}
		}
		//

		ListData.Add(Data);
	}
	AManageObjectC::GetInstance()->GetManageFunction(ListData);
	return true;
}
bool UCabinetSdk::SetAutoSetWalls(const TArray<FAreaInnerWall>& outWallLines, const TArray<FCustomizationBoolean>& outBooleanData, int nType, FString Tag)
{
	if ( nType == eAuotSetCook )
	{
		{
			FAutoSetBase* pAutoSet = NewAutoSet(eAutoSetFloor);
			CHECK_ERROR(pAutoSet);
			pAutoSet->Clear();
		}
		{
			FAutoSetBase* pAutoSet = NewAutoSet(eAutoSetHang);
			CHECK_ERROR(pAutoSet);
			pAutoSet->Clear();
		}
	}
	else
	{
		FAutoSetBase* pAutoSet = NewAutoSet(eAutoSetHang);
		CHECK_ERROR(pAutoSet);
		pAutoSet->Clear();
	}
	FCabinetArea* pArea = gCabinetAreaMgr.NewArea(Tag);
	pArea->mRawWalls.Empty();
	pArea->mHoles.Empty();
	for (auto w : outWallLines)
	{
		pArea->AddRawWall(w.WallID, w.SegStart, w.SegEnd, w.VerticalDir);
	}
	for (auto h : outBooleanData)
	{
		pArea->AddHole(h.HoleID, h.PStartLeft, h.PEndLeft, h.Type);
	}
	return true;
}

bool UCabinetSdk::SetAutoSlecctWalls(const TArray<int32>& WallsID, int nType)
{
	FAutoSetBase* pSet = NewAutoSet(eAutoSetType(nType));
	return true;
}
bool UCabinetSdk::SetSlecctWalls(const TArray<FLine2dSelect>& SelectWalls, int nType)
{
	FAutoSetBase* pSet = NewAutoSet(eAutoSetType(nType));
	CHECK_ERROR(pSet);
	pSet->mSelectWalls.Empty();
	for( auto p:SelectWalls )
	{
		TLine2d li;
		li.mStart = p.mStart*10;
		li.mEnd = p.mEnd*10;
		li.Id = p.Id;
		pSet->mSelectWalls.Add(li);
	}
	return true;
}
bool UCabinetSdk::ChangAreaTag(FString newid, FString oldid)
{
	gCabinetAreaMgr.ChangAreaTag(newid, oldid);
	return true;
}
bool UCabinetSdk::SetAutoTypeClass(const TArray<int32>& MultiID, const TArray<int32>& RadioID, int nType)
{
	if ( nType == 0 )
	{
		nType = 1;
	}
	FAutoSetBase* pAutoSet = NewAutoSet(eAutoSetType(nType));
	CHECK_ERROR(pAutoSet);
	return true;
}
//////////////////////////////////////////////////////////////////////////
//web
//////////////////////////////////////////////////////////////////////////

FVector GetJsonVector(UVaRestJsonObject* p, const char* sName)
{
	FVector size;
	UVaRestJsonObject* pValue = p->GetObjectField(sName);
	if (pValue)
	{
		size.X = pValue->GetNumberField("length");
		size.Y = pValue->GetNumberField("width");
		size.Z = pValue->GetNumberField("height");
	}
	//bool is;
	//FString str = p->GetStringField(sName);
	//UKismetStringLibrary::Conv_StringToVector(str, size, is);
	return size;
}

FAutoSetBase* gCurLayout = nullptr;
bool UCabinetSdk::AutoLayout( FCabinetAreaRaw AreaRaw, const FCabinetAutoSet & SelectData)
{
	//return false;
	AreaRaw = UHouseFunctionLibrary::RepairtheDeviation(AreaRaw);
	FCabinetArea* pArea = gCabinetAreaMgr.NewArea(AreaRaw.mName);
	CHECK_ERROR(pArea);

	eAutoSetType eSetType = CategoryId2SetType(SelectData.mCategoryId, SelectData.mFloor==1);
	FAutoSetBase* pSet = NewAutoSet(eSetType);
	CHECK_ERROR(pSet);
	pSet->Clear();
	if (SelectData.mActionType == 1)
	{
		if (eSetType == eAutoSetFloor)
		{
			pArea->mCabinetGroup[eAutoSetHang].Clear();
		}
		if (gCurLayout->mSetType == eAutoSetWardrobe)
		{
			pArea->mCabinetGroup[eAutoSetWardorbeHang].Clear();
		}
		if (gCurLayout->mSetType == eAutoSetToilet)
		{
			pArea->mCabinetGroup[eAutoSetWardorbeHang].Clear();
		}
		if (gCurLayout->mSetType == eAutoSetCoatroom)
		{
			pArea->mCabinetGroup[eAutoSetCoatroomHang].Clear();
		}
		pArea->mCabinetGroup[eSetType].Clear();
	}
	gCabinetGlobal.ClearSkuDatas();
	auto SkuLists = AManageObjectC::GetInstance()->GetSkuList();
	for (auto sku : SkuLists)
	{
		TSkuData* skuData = new TSkuData;
		skuData->mSkuId = sku->GetNumberField("skuId");
		skuData->mCategoryId = sku->GetNumberField("drCategoryId");
		skuData->mName = sku->GetStringField("name");
		skuData->mMxFileMD5 = sku->GetStringField("mxFileMD5");
		skuData->mSize = GetJsonVector(sku, "modelSize");
		gCabinetGlobal.mSkuDatas.Add(skuData->mSkuId, skuData);
	}

	auto Templates = AManageObjectC::GetInstance()->GetTemplateList();
	if (Templates.Num() == 0)
		return true;
	SetTemplateCabinetJson(Templates, eSetType);

	pArea->mRawWalls.Empty();
	pArea->mHoles.Empty();
	for (auto w : AreaRaw.mWallLines)
	{
		pArea->AddRawWall(w.WallID, w.SegStart, w.SegEnd, w.VerticalDir);
	}
	for (auto h : AreaRaw.mHoles)
	{
		pArea->AddHole(h.HoleID, h.PStartLeft, h.PEndLeft, h.Type);
	}
	pArea->GenOutPath();
	for ( auto pillar : AreaRaw.mPillars )
	{
		TRectBase rect;
		rect.mDir = pillar.mDir;
		rect.mUserType = pillar.mType;
		rect.mSize = pillar.mSize*10;
		rect.mPos.Set(pillar.mPos.X * 10.f, pillar.mPos.Y * 10.f);//( Float2Int2(pillar.mPos.X), Float2Int2(pillar.mPos.Y) );
		pArea->mPillars.Add(rect);
	}
	//////////////////////////////////////////////////////////////////////////
	gCurLayout = pSet;

	if (eSetType == eAutoSetTatami )
	{
		AutoSetTatami::FAutoSetCabinet* pTatami = (AutoSetTatami::FAutoSetCabinet*)pSet;
		pTatami->SetnDepth(SelectData.mDepth); 
	}
	for (auto p : SelectData.mSelectWalls)
	{
		TLine2d li;
		li.mStart = p.mStart;
		li.mEnd = p.mEnd;
		li.Id = p.Id;
		pArea->AdjustSelect(li);
		pSet->mSelectWalls.Add(li);
	}
	pSet->LayoutCabinet(pArea);
	FCabinetLayoutResult LayoutData;
	LayoutData.success = pSet->IsLayoutError();
	LayoutData.mFailDatas = pSet->mFailData.mFailDatas;
	LayoutData.msg = pSet->mFailData.msg;

	LayoutData.comtype = SelectData.mActionType;
	pSet->GetLayoutResult(LayoutData);
	LayoutResultBack(&gCabinetMgr, LayoutData);
	return true;
}

bool UCabinetSdk::LayoutResultBack(UObject* WorldContextObject, const FCabinetLayoutResult & Data)
{
	UVaRestJsonObject* MessageJson = UVaRestSubsystem::StaticConstructVaRestJsonObject();
	UVaRestJsonObject* data = UVaRestSubsystem::StaticConstructVaRestJsonObject();
	MessageJson->SetNumberField(TEXT("type"), 6);
	UWorld* MyWorld = WorldContextObject->GetWorld();
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	TArray<UVaRestJsonObject*>skumodellist;
	TArray<UVaRestJsonObject*>tatamiList;
	TArray<UVaRestJsonObject*>failList;

	UVaRestJsonObject* inputParams = UVaRestSubsystem::StaticConstructVaRestJsonObject();

	for (FCabinetWalldata var : Data.mFailDatas)
	{
		TArray<UVaRestJsonObject*>modelList;
		UVaRestJsonObject* tempfail = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* start = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* end = UVaRestSubsystem::StaticConstructVaRestJsonObject();

		tempfail->SetNumberField("success", var.success);
		tempfail->SetStringField("msg", var.msg);
		tempfail->SetNumberField("id", var.wallline.Id);
		start->SetNumberField("x", var.wallline.mStart.X);
		start->SetNumberField("y", var.wallline.mStart.Y);

		end->SetNumberField("x", var.wallline.mEnd.X);
		end->SetNumberField("y", var.wallline.mEnd.Y);

		tempfail->SetObjectField("start", start);
		tempfail->SetObjectField("end", end);

		for (FCabinetModle mod : var.mCabinets)
		{
			UVaRestJsonObject* temp2 = UVaRestSubsystem::StaticConstructVaRestJsonObject();
			UVaRestJsonObject* modelSize = UVaRestSubsystem::StaticConstructVaRestJsonObject();
			UVaRestJsonObject* point = UVaRestSubsystem::StaticConstructVaRestJsonObject();
			UVaRestJsonObject* rotate = UVaRestSubsystem::StaticConstructVaRestJsonObject();
			UVaRestJsonObject* scale = UVaRestSubsystem::StaticConstructVaRestJsonObject();
			temp2->SetNumberField("actorId", mod.actorId);
			temp2->SetNumberField("templateId", mod.templateId);
			temp2->SetNumberField("categoryId", mod.categoryId);
			temp2->SetNumberField("turnBSize", mod.turnBSize);
			temp2->SetStringField("categoryName", mod.categoryName);

			modelSize->SetNumberField("dx", mod.modelSize.X);
			modelSize->SetNumberField("dy", mod.modelSize.Y);
			modelSize->SetNumberField("dz", mod.modelSize.Z);

			point->SetNumberField("x", mod.point.X);
			point->SetNumberField("y", mod.point.Y);
			point->SetNumberField("z", mod.point.Z);

			rotate->SetNumberField("yAxis", mod.rotate.X);
			rotate->SetNumberField("xAxis", mod.rotate.Y);
			rotate->SetNumberField("zAxis", mod.rotate.Z);

			scale->SetNumberField("x", mod.scale.X);
			scale->SetNumberField("y", mod.scale.Y);
			scale->SetNumberField("z", mod.scale.Z);

			temp2->SetObjectField("modelSize", modelSize);
			temp2->SetObjectField("point", point);
			temp2->SetObjectField("rotate", rotate);
			temp2->SetObjectField("scale", scale);
			modelList.Add(temp2);
		}
		tempfail->SetObjectArrayField("modelList", modelList);
		failList.Add(tempfail);
	}

	

	for (FCabinetSkuModel sku : Data.mCookExtern) {
		UVaRestJsonObject* temp3 = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* modelSize = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* point = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* start = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* end = UVaRestSubsystem::StaticConstructVaRestJsonObject();

		start->SetNumberField("x", sku.wallline.mStart.X);
		start->SetNumberField("y", sku.wallline.mStart.Y);

		end->SetNumberField("x", sku.wallline.mEnd.X);
		end->SetNumberField("y", sku.wallline.mEnd.Y);

		modelSize->SetNumberField("dx", sku.size.X);
		modelSize->SetNumberField("dy", sku.size.Y);

		point->SetNumberField("x", sku.point.X);
		point->SetNumberField("y", sku.point.Y);

		temp3->SetNumberField("skuid", sku.skuid);
		temp3->SetNumberField("type", sku.type);
		temp3->SetNumberField("rotate", sku.rotate);
		temp3->SetObjectField("start", start);
		temp3->SetObjectField("end", end);
		temp3->SetObjectField("modelSize", modelSize);
		temp3->SetObjectField("point", point);

		skumodellist.Add(temp3);
	}
	for (FCabinetTatamiExtern tata : Data.mTatamiExtern) {
		UVaRestJsonObject* temp4 = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* point = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* size = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		point->SetNumberField("x", tata.point.X);
		point->SetNumberField("y", tata.point.Y);
		size->SetNumberField("dx", tata.size.X);
		size->SetNumberField("dy", tata.size.Y);

		temp4->SetNumberField("type", tata.type);
		temp4->SetNumberField("selected", tata.selected);
		temp4->SetNumberField("rotate", tata.rotate);
		temp4->SetObjectField("modelSize", size);
		temp4->SetObjectField("point", point);
		tatamiList.Add(temp4);
	}




	inputParams->SetNumberField("success", (float)Data.success);
	inputParams->SetStringField("msg", Data.msg);
	inputParams->SetNumberField("comtype", (float)Data.comtype);
	inputParams->SetObjectArrayField("cabinetwalldata", failList);
	
	inputParams->SetObjectArrayField("skumodellist", skumodellist);
	inputParams->SetObjectArrayField("upperwardrobelayout", tatamiList);

	data->SetNumberField(TEXT("operation"), 3);
	data->SetStringField("functionName", "getLayoutResult");
	data->SetStringField("inputParams", inputParams->EncodeJsonToSingleString());
	MessageJson->SetObjectField(TEXT("data"), data);
	return true;
}

bool UCabinetSdk::OverlayResultBack(UObject* WorldContextObject, const FCabinetOverlayResult & Data)
{
	UVaRestJsonObject* MessageJson= UVaRestSubsystem::StaticConstructVaRestJsonObject();
	MessageJson->SetNumberField(TEXT("type"), 6);
	UWorld* MyWorld = WorldContextObject->GetWorld();
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	TArray<UVaRestJsonObject*>failList;
	UVaRestJsonObject* data = UVaRestSubsystem::StaticConstructVaRestJsonObject();
	UVaRestJsonObject* inputParams = UVaRestSubsystem::StaticConstructVaRestJsonObject();
	for(FCabinetWalldata var : Data.mFailDatas)
	{
		UVaRestJsonObject* temp = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* start = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		UVaRestJsonObject* end = UVaRestSubsystem::StaticConstructVaRestJsonObject();
		temp->SetStringField("msg", var.msg);
		start->SetNumberField("x", var.wallline.mStart.X);
		start->SetNumberField("y", var.wallline.mStart.Y);

		end->SetNumberField("x", var.wallline.mEnd.X);
		end->SetNumberField("y", var.wallline.mEnd.Y);

		temp->SetObjectField("start", start);
		temp->SetObjectField("end", end);
		failList.Add(temp);
	}

	inputParams->SetObjectArrayField("faildata", failList);

	data->SetNumberField(TEXT("operation"), 3);
	data->SetStringField("functionName", "checkModelOverlayResult");
	data->SetStringField("inputParams", inputParams->EncodeJsonToSingleString());
	MessageJson->SetObjectField(TEXT("data"), data);
	return true;
}

bool UCabinetSdk::SelecteDwardrobeLayout(int type)
{
	AutoSetTatami::FAutoSetCabinet::Instance().SetSideType(type);
	FCabinetLayoutResult LayoutData;
	LayoutData.success = 0;
	LayoutData.comtype = 2;
	gCurLayout->GetLayoutResult(LayoutData);
	LayoutResultBack(&gCabinetMgr, LayoutData);
	return false;
}

bool UCabinetSdk::WardrobeUseHang(int nUse)
{
	//AutoSetWardrobe::FAutoSetCabinet::Instance().UseHangCabinets(nUse);
	return true;
}

bool UCabinetSdk::ModifySkuPos(int type, FVector2D NewPos)
{
	type = 1 - type;
	AutoSetCookFloor::FAutoSetCabinet* pCookFloor =  (AutoSetCookFloor::FAutoSetCabinet*)gCurLayout;
	pCookFloor->ModifySkuPos(type, NewPos);
	ModifySkuPosBack(&gCabinetMgr, pCookFloor->SkuData[type]);
	return false;
}

bool UCabinetSdk::ModifySkuPosBack(UObject* WorldContextObject,const FCabinetSkuModel & Data)
{
	return true;
}

bool UCabinetSdk::Complete()
{
	CHECK_ERROR(gCurLayout);
	FAutoSetBase* pFloorSet = gCurLayout;

	TArray<int32> Cabinets;
	TArray<int32> Skus;
	gCurLayout->GetCabinets(Cabinets,Skus);

	if (gCurLayout->mSetType == eAutoSetHang)
	{
		pFloorSet = &AutoSetCookFloor::FAutoSetCabinet::Instance();
		pFloorSet->GetCabinets(Cabinets, Skus);
		Skus.Add(AutoSetCookFloor::FAutoSetCabinet::Instance().mSkus[0].mSku.mSkuId);
		Skus.Add(AutoSetCookFloor::FAutoSetCabinet::Instance().mSkus[1].mSku.mSkuId);
	}
	if (gCurLayout->mSetType == eAutoSetWardorbeHang)
	{
		pFloorSet = &AutoSetWardrobe::FAutoSetCabinet::Instance();
		pFloorSet->GetCabinets(Cabinets, Skus);
	}
	if (gCurLayout->mSetType == eAutoSetToiletHang)
	{
		pFloorSet = &AutoSetToilet::FAutoSetCabinet::Instance();
		pFloorSet->GetCabinets(Cabinets, Skus);
	}
	if (gCurLayout->mSetType == eAutoSetCoatroomHang)
	{
		pFloorSet = &AutoSetCoatroom::FAutoSetCabinet::Instance();
		pFloorSet->GetCabinets(Cabinets, Skus);
	}

	AManageObjectC::GetInstance()->TemplatesDownload(Cabinets, Skus);
	//LayoutUseCabinet(Cabinets, Skus);
	return true;
}

bool UCabinetSdk::LayoutUseCabinet(TArray<int32>& Cabinets,TArray<int32>& Skus)
{

	return true;
}

bool UCabinetSdk::DownComplete()
{
	CHECK_ERROR(gCurLayout);
	TMap<int32, FCabinetShell*> Cabinets;
	gCurLayout->mArea->GetCabinet(gCurLayout->mSetType,Cabinets);

	gCurLayout->CompleteLayout();
	FAutoSetBase* pFloorSet = gCurLayout;
	if (gCurLayout->mSetType == eAutoSetHang)
	{
		gCurLayout->mArea->mType = 2;
		pFloorSet = &AutoSetCookHang::FAutoSetCabinet::Instance();
		pFloorSet->CompleteLayout();

		gCurLayout->mArea->mType = 1;
		pFloorSet = &AutoSetCookFloor::FAutoSetCabinet::Instance();
		pFloorSet->CompleteLayout();

		gCurLayout->mArea->mType = -1;
	}
	if (gCurLayout->mSetType == eAutoSetWardorbeHang)
	{
		pFloorSet = &AutoSetWardrobe::FAutoSetCabinet::Instance();
		pFloorSet->CompleteLayout();
	}
	if (gCurLayout->mSetType == eAutoSetToiletHang)
	{
		pFloorSet = &AutoSetToilet::FAutoSetCabinet::Instance();
		pFloorSet->CompleteLayout();
	}
	if (gCurLayout->mSetType == eAutoSetCoatroomHang)
	{
		pFloorSet = &AutoSetCoatroom::FAutoSetCabinet::Instance();
		pFloorSet->CompleteLayout();
	}
	int32 Categoryid = pFloorSet->mCookOverlay ? 200028 : -1;
	AManageObjectC::GetInstance()->GetClearData(pFloorSet->mArea->mName, pFloorSet->mOverlayCabinets, Categoryid);
	GetCabinets(pFloorSet->mSetType,Cabinets);
	return true;
}

void UCabinetSdk::replaceLayoutTemplate(int macterId, int mcategoryId)
{
	FCabinetLayoutResult LayoutData;
	bool b = gCurLayout->replaceLayoutTemplate(macterId, mcategoryId);
	if (b)
	{
		LayoutData.success =0;
		gCurLayout->OnReplace();
	}
	else
	{
		LayoutData.success = 5;
		LayoutData.msg = TEXT("替换失败");
	}
	LayoutData.comtype = 1;
	gCurLayout->GetLayoutResult(LayoutData);
	LayoutResultBack(&gCabinetMgr, LayoutData);
}
