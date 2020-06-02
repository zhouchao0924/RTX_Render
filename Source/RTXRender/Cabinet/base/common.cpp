
#include "common.h"
#include "../Building/BuildingSystem.h"
#include "../CabinetGlobal.h"

float Point2LineSeg(const FVector2D& point_A, const FVector2D& point_B, const FVector2D& point_P, float &fDot, FVector2D &point_C)
{
	FVector2D AP = point_P - point_A;
	FVector2D AB = point_B - point_A;
	fDot = (AP | AB) / (AB | AB);

	FVector2D AC = AB * fDot;
	point_C = AC + point_A;

	if (fDot >= 1)
	{
		FVector2D BP = point_P - point_B;
		point_C = point_B;
		return BP.Size();
	}
	else if (fDot <= 0)
	{
		point_C = point_A;
		return AP.Size();
	}
	else
	{
		FVector2D PC = point_P - point_C;
		return PC.Size();
	}
};
float Point2LineSegVer(const FVector2D& point_A, const FVector2D& point_B, const FVector2D& point_P, float &fDot, FVector2D &point_C)
{
	FVector2D AP = point_P - point_A;
	FVector2D AB = point_B - point_A;
	fDot = (AP | AB) / (AB | AB);

	FVector2D AC = AB * fDot;
	point_C = AC + point_A;
	FVector2D PC = point_P - point_C;

	if (fDot >= 1)
	{
		point_C = point_B;
	}
	else if (fDot <= 0)
	{
		point_C = point_A;
	}
	return PC.Size();
};
float AreaPoly(TArray<FVector2D> &poly)
{
	int size = (int)poly.Num();
	if (size < 3) return 0;

	double a = 0;
	for (int i = 0, j = size - 1; i < size; ++i)
	{
		a += ((double)poly[j].X + poly[i].X) * ((double)poly[j].Y - poly[i].Y);
		j = i;
	}
	return -a * 0.5;
}
bool IsPointOnLine(FVector2D PosStart,FVector2D PosEnd, FVector2D point_P)
{
	if (FMath::Abs(((PosStart.X - point_P.Y)*(PosEnd.Y - point_P.Y) - (PosStart.Y - point_P.Y)*(PosEnd.X - point_P.X))) <= 0.00001f)
	{
		if (FMath::Min(PosStart.X, PosEnd.X) <= point_P.X && point_P.X <= FMath::Max(PosStart.X, PosEnd.X) && FMath::Min(PosStart.Y, PosEnd.Y) <= point_P.Y && point_P.Y <= FMath::Max(PosStart.Y, PosEnd.Y))
			return true;
	}
	return false;
}
FVector2D CeilVector(FVector2D Size)
{
	return FVector2D(FMath::CeilToInt(Size.X), FMath::CeilToInt(Size.Y));
}
int64 Pos2Id(float _x, float _y)
{
	INT32 x = INT32((_x + (_x > 0 ? .5f : -.5f)));
	INT32 y = INT32((_y + (_y > 0 ? .5f : -.5f)));
	return (INT64(x) << 32) + y;
}
int64 Pos2Id(FVector2D Pos)
{
	return Pos2Id(Pos.X, Pos.Y);
}

FString IdId2Str(INT64 IdStart, INT64 IdEnd)
{
	if (IdStart > IdEnd)
		return FString::Printf(TEXT("%llx_%llx"), IdStart, IdEnd);
	else
		return FString::Printf(TEXT("%llx_%llx"), IdEnd, IdStart);
}
FVector2D Local2World(FVector2D& Anchor, FVector2D& Dir, FVector2D& Scale, FVector2D& LocalPos)
{
	FVector2D Pos = LocalPos * Scale;
	Pos = Anchor + Pos.X*Dir + Pos.Y* FVector2D(-Dir.Y, Dir.X);
	return Pos;
}
FVector GetResSize(FString ResID)
{
	FVector Size;
	IBuildingSDK *SDK = UBuildingSystem::GetBuildingSDK();
	if ( SDK==nullptr)
	{
		return Size;
	}
	const char *AnsiResID = TCHAR_TO_ANSI(*ResID);
	IObject *pObj = SDK->GetResourceMgr()->GetResource(AnsiResID, false);
	if (pObj == nullptr)
	{
		return Size;
	}
	IGeometry* pGeometry = SDK->GetGeometryLibrary();
	if (pGeometry == nullptr)
	{
		return Size;
	}
	IMeshObject* pMeshObj = pObj->GetMeshObject(0);
	if (pMeshObj == nullptr)
	{
		return Size;
	}
	kBox3D box = pMeshObj->GetBounds();
	Size.X = box.MaxEdge.x - box.MinEdge.x;
	Size.Y = /*box.MaxEdge.y*/ - box.MinEdge.y*2;
	Size.Z = box.MaxEdge.z - box.MinEdge.z;
	return Size;
}
bool LoadStrim(FString ResID, TArray<FVector2D>& OutLine, int IgnoreXYZ, char* sMeshName, float& Z)
{
	OutLine.Empty();
	Z = 0;
	OutLine.Empty();
	IBuildingSDK *SDK = UBuildingSystem::GetBuildingSDK();
	CHECK_ERROR(SDK);

	const char *AnsiResID = TCHAR_TO_ANSI(*ResID);
	IObject *pObj = SDK->GetResourceMgr()->GetResource(AnsiResID, false,true,false/*,true*/);

	CHECK_ERROR(pObj);
	IGeometry* pGeometry = SDK->GetGeometryLibrary();
	CHECK_ERROR(pGeometry);
	IMeshObject* pMeshObj = pObj->GetMeshObject(sMeshName);
	CHECK_ERROR(pMeshObj);

	kBox3D box = pMeshObj->GetBounds();
	Z = box.MaxEdge.z*10;
	kArray<kPoint>* OutBoundarys = nullptr;;
	unsigned char* OutCloseFlags = nullptr;;
	int OutBoundaryCount;
	bool bSucess= pGeometry->GetMesh2DBoundary(pMeshObj, OutBoundarys, OutCloseFlags, OutBoundaryCount, IgnoreXYZ);
	CHECK_ERROR(bSucess);
	kArray<kPoint>& OutBound = OutBoundarys[0];
	OutLine.SetNum(OutBound.size());
	for (int i = 0; i < OutBound.size(); ++i)
	{
		FVector2D a = FVector2D(OutBound[i].x * 10, OutBound[i].y * 10);
		OutLine[i] = FVector2D(OutBound[i].x * 10, OutBound[i].y * 10);
	}
	return true;
}

bool GetMeshSize(FString ResID, char* sMeshName, FVector& Size, FVector& Pos)
{
	IBuildingSDK *SDK = UBuildingSystem::GetBuildingSDK();
	CHECK_ERROR(SDK);

	const char *AnsiResID = TCHAR_TO_ANSI(*ResID);
	IObject *pObj = SDK->GetResourceMgr()->GetResource(AnsiResID, false);

	CHECK_ERROR(pObj);
	IGeometry* pGeometry = SDK->GetGeometryLibrary();
	CHECK_ERROR(pGeometry);

	IMeshObject* pMeshObj = pObj->GetMeshObject(sMeshName);
	CHECK_ERROR(pMeshObj);

	kBox3D box = pMeshObj->GetBounds();
	kVector3D kSize = box.GetExtent()*0.5f;
	Size.Set(kSize.x, kSize.y, kSize.z);
	kVector3D kPos = box.GetCenter()*10;
	kPos.z = kPos.z -= kSize.z*0.5f;
	Pos.Set(kPos.x, kPos.y, kPos.z);
	return true;
}

eAutoSetType CategoryId2SetType(int32 categoryId, bool bFloor /*= false*/)
{
	switch (categoryId)
	{
	case 200028:
	{
		if (bFloor)
			return eAutoSetFloor;
		return eAutoSetHang;
	}
	case 200051:
	{
		if (bFloor == false)
			return eAutoSetToiletHang;
		return eAutoSetToilet;
	}
	case 200076:
	{
		return eAutoSetTatami;
	}
	case 200015:
	{
		if (bFloor == false)
			return eAutoSetWardorbeHang;
		return eAutoSetWardrobe;
	}
	case 200116:
	{
		return eAutoSetSideboardCabinet;
	}
	case 200115:
	{
		return eAutoSetTelevision;
	}
	case 200063:
	{
		return eAutoSetBookcase;
	}
	case 200008:
	{
		return eAutoSetPorchArk;
	}
	case 200101:
	{
		return eAutoSetWashCabinet;
	}
	case 200109:
	{
		return eAutoSetStoreCabinet;
	}
	case 200119:
	{
		return eAutoSetBedCabinet;
	}
	case 200073:
	{
		return eAutoSetDesk;
	}
	case 200106:
	{
		return eAutoSetHangBedCabinet;
	}
	case 200082:
	{		
		if (bFloor == false)
			return eAutoSetCoatroomHang;
		return eAutoSetCoatroom;
	}
	case 200124:
	{
		return eAutoSetBayWindowCabinet;
	}
	case 200128:
	{
		return eAutoSetTakeInCabinet;
	}
	case 200123:
	{
		return eAutoSetLaminate;
	}
	case 200129:
	{
		return eAutoSetBedside;
	}
	default:
		return eAutoSetNull;
		break;
	}
}

int32 SetType2CategoryId(eAutoSetType eSetType)
{
	switch (eSetType)
	{
	case eAutoSetFloor:
	{
		return 200028;
	}
	break;
	case eAutoSetHang:
	{
		return 200028;
	}
	break;
	case eAutoSetToiletHang:
	case eAutoSetToilet:
	{
		return 200051;
	}
	break;
	case eAutoSetTatami:
	{
		return 200076;
	}
	break;
	case eAutoSetWardorbeHang:
	case eAutoSetWardrobe:
	{
		return 200015;
	}
	break;
	case eAutoSetSideboardCabinet:
	{
		return 200116;
	}
	break;
	case eAutoSetTelevision:
	{
		return 200115;
	}
	break;
	case eAutoSetBookcase:
	{
		return 200063;
	}
	break;
	case eAutoSetPorchArk:
	{
		return 200008;
	}
	break;
	case eAutoSetWashCabinet:
	{
		return 200101;
	}
	break;
	case eAutoSetStoreCabinet:
	{
		return 200109;
	}
	break;
	case eAutoSetBedCabinet:
	{
		return 200119;
	}
	break;
	case eAutoSetDesk:
	{
		return 200073;
	}
	break;
	case eAutoSetHangBedCabinet:
	{
		return 200106;
	}
	break;
	case eAutoSetCoatroomHang:
	case eAutoSetCoatroom:
	{
		return 200082;
	}
	break;
	case eAutoSetBayWindowCabinet:
	{
		return 200124;
	}
	break;
	case eAutoSetTakeInCabinet:
	{
		return 200128;
	}
	break;
	case eAutoSetLaminate:
	{
		return 200123;
	}
	break;
	case eAutoSetBedside:
	{
		return 200129;
	}
	break;	default:
		return eAutoSetNull;
		break;
	}
}

int GetTurnSize(int TemplateId)
{
	if (TemplateId == 200036)
	{
		return 562;
	}
	else if (TemplateId == 200090)
	{
		return 512;
	}
	else if (TemplateId == 200086)
	{
		return 512;
	}
	else if (TemplateId == 200098)
	{
		return 530;
	}
	else if (TemplateId == 200094)
	{
		return 530;
	}
	return 0;
}
bool IsTurnCabinet(int TemplateId)
{
	if (TemplateId == 200036)
	{
		return true;
	}
	else if (TemplateId == 200090)
	{
		return true;
	}
	else if (TemplateId == 200086)
	{
		return true;
	}
	else if (TemplateId == 200098)
	{
		return true;
	}
	else if (TemplateId == 200094)
	{
		return true;
	}
	return false;
}
TRectBase::TRectBase(FVector2D Pos, FVector2D Size, FVector2D Dir, FVector2D Scale, FVector2D Anchor)
{
	mScale = Scale;
	mDir = Dir;
	mSize = FVector2D(Size.X*0.5f, Size.Y*0.5f);
	mPos = FVector2D(Pos.X, Pos.Y) - mSize.X*Anchor.X*mDir - mSize.Y*Anchor.Y*FVector2D(-mDir.Y, mDir.X);
}

bool TRectBase::Collision(TLine2d & Line2d, float fOffset)
{
	FVector2D PosStart = Line2d.mStart-mPos;
	PosStart = FVector2D(PosStart | mDir, PosStart | FVector2D(-mDir.Y, mDir.X) );
	FVector2D PosEnd = Line2d.mEnd-mPos;
	PosEnd = FVector2D(PosEnd | mDir, PosEnd | FVector2D(-mDir.Y, mDir.X) );

	FVector2D SegPos = (PosStart + PosEnd)*0.5f;
	FVector2D HalfD = PosEnd - SegPos;
	float adx = FMath::Abs(HalfD.X);
	if (FMath::Abs(SegPos.X) > mSize.X + 20 + adx)
		return false;
	float ady = FMath::Abs(HalfD.Y);
	if (FMath::Abs(SegPos.Y) > mSize.Y + fOffset + ady)
		return false;
	if (FMath::Abs(SegPos^HalfD) > (mSize.X + 20)*ady + (mSize.Y + fOffset)*adx)
		return false;
	return true;
}

float TSkuInstance::LoadSku()
{
	static char* StrimName[3] = { "aux_trim_1","aux_trim_0","aux_trim_2" };

	for (int i = 0; i < 3; ++i)
	{
		LoadStrim(mSku.mMxFileMD5, mHoles[i], 2, StrimName[i], mAnchor[i]);
	}
	return gCabinetGlobal.mFloorHeight -mAnchor[0]+ gCabinetGlobal.mTableThickness;
}
bool FCabinetRes::InitSinkSku(FVector2D mScale)
{
	FVector2D Size = mScale * mTableSize;
	for (int i = 0; i < mSkuDatas.Num(); ++i)
	{
		if (mSkuDatas[i].mCategoryId == 726 || mSkuDatas[i].mCategoryId == 721)
		{
			TSkuData* pData = gCabinetGlobal.GetSku(Size, mSkuDatas[i].mCategoryId, mSkuDatas[i].mSkuId);
			if (pData == nullptr || pData->mSize.X>Size.X || pData->mSize.Y>Size.Y )
			{
				pData = gCabinetGlobal.GetSku(Size, 726, 162860);
			}
			if( pData )
			{
				mSkuDatas[i] = *pData;
				mCategoryIds[i] = pData->mSkuId;
				mHardSkuMd5[i] = pData->mMxFileMD5;
				return true;
			}
		}
	}
	return true;
}