
#include "CabinetArea.h"
#include "CabinetMgr.h"
#include "base/FrameEdge.h"
#include "base/PolygonTool.h"
#include "../HomeLayout/HouseArchitect/AreaSystem.h"
#include "../HomeLayout/View/GameEditorPawn_Base.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "EditorGameInstance.h"
#include "AutoSet/CabinetSetApi.h"
#include "CabinetSdk.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRWindowHoleAdapter.h"
//////////////////////////////////////////////////////////////////////////
FCabinetArea::FCabinetArea()
{
	for ( int i=0; i< eAutoSetNum; ++i )
	{
		mCabinetGroup[i].mArea = this;
		mCabinetGroup[i].mSetType = (eAutoSetType)i;
	}
	mType = -1;
}
FCabinetArea::~FCabinetArea()
{
	for (int i = 0; i < eAutoSetNum; ++i)
	{
		mCabinetGroup[i].Clear();
	}
}

bool FCabinetArea::AddRawWall(int32 wallId, FVector2D Start, FVector2D End, FVector2D VerticalDir)
{
	TLine2d line2d;
	FVector2D Dir = End - Start;
	Dir.Normalize();
	FVector2D Nor(-Dir.Y, Dir.X);	

	line2d.mStart.X = FMath::TruncToInt(Start.X*10);
	line2d.mStart.Y = FMath::TruncToInt(Start.Y*10);
	line2d.mEnd.X = FMath::TruncToInt(End.X*10);
	line2d.mEnd.Y = FMath::TruncToInt(End.Y*10);


	line2d.Id = wallId;
	line2d.mState = wallId == -1 ? 0 : 1;
	if ( wallId == -1 )
	{
		line2d.Id = -mRawWalls.Num() - 1;
	}
	else
	{
		line2d.Id = wallId;
	}
	if (!Nor.Equals(VerticalDir, 0.1f))
	{
		TinySwap(line2d.mStart, line2d.mEnd);
	}
	mRawWalls.Add(line2d.Id, line2d);
	return true;
}
bool FCabinetArea::AddHole(int Id, FVector2D PosStart, FVector2D PosEnd, EOpeningType Type)
{
	TLine2d hole;
	hole.Id = Id;
	hole.mStart = PosStart * 10;
	hole.mEnd = PosEnd * 10;
	hole.mState = (Type >= EOpeningType::Window) ? eSliptWallWindow : eSliptWallDoor;
	mHoles.Add(hole.Id, hole);
	return true;
}
TRectBase* FCabinetArea::GetChimery()
{
	for (int i=0; i<mPillars.Num(); ++i)
	{
		if (mPillars[i].mUserType == 0 )
		{
			return &mPillars[i];
		}
	}
	return nullptr;
}

bool FCabinetArea::AutoSetCabinets(int nType)
{
	eAutoSetType eType = (eAutoSetType)nType;
	FAutoSetBase* pAutoSet = NewAutoSet(eType);
	CHECK_ERROR(pAutoSet);

	mCabinetGroup[nType].Clear();

	GenOutPath();
	pAutoSet->Execute(this);

	mCabinetGroup[nType].GenExtern();
	return true;
}

bool FCabinetArea::ClearCabinets(TArray<int32>& CabinetsIds, bool bClearCook)
{
	int32 nNumCabinets = CabinetsIds.Num();
	if (mType == 1)
		mCabinetGroup[eAutoSetFloor].Clear();
	else if (mType == 2)
		mCabinetGroup[eAutoSetHang].Clear();

	for ( int g=0; g< eAutoSetNum;++g )
	{
		bool bChang = false;
		for ( int i= 0;i< nNumCabinets;++i )
		{
			bChang |= mCabinetGroup[g].DeleteCabinet(CabinetsIds[i]);
		}
		if( bChang) 
			mCabinetGroup[g].GenExtern();
	}
	return true;
}

bool FCabinetArea::GetSelectWalls(TArray<int32>& RawSelectWalls, TArray<TLine2d>& SelectPaths)
{
	for (auto p : RawSelectWalls)
	{
		TLine2d* pWall = GetWall(p);
		if (pWall)
		{
			TLine2d tline;
			tline.mStart = pWall->mStart;
			tline.mEnd = pWall->mEnd;
			tline.mState = pWall->mState;
			SelectPaths.Add(tline);
		}
	}
	return true;
}
bool FCabinetArea::NewCabinet(FCabinetSet& CabinetSet)
{
	FCabinetShell* pCabinet = new FCabinetShell;
	pCabinet->Id = CabinetSet.mId;
	pCabinet->mPosZ = CabinetSet.mHeight;
	pCabinet->mPos = CabinetSet.mPos;
	pCabinet->mScale = CabinetSet.mScale;
	pCabinet->mSize = CabinetSet.mRes.mSize*0.5f;
	pCabinet->mTeplateId = CabinetSet.mRes.mTeplateId;
	pCabinet->mDir = CabinetSet.mDir;
	pCabinet->mSetType = CabinetSet.mSetType;
	pCabinet->mScaleZ = CabinetSet.mScaleZ;
	pCabinet->mRes = CabinetSet.mRes;
	mCabinetGroup[pCabinet->mSetType].mCabinets.Add(pCabinet->Id, pCabinet);
	return pCabinet->Init();
}
bool FCabinetArea::GenOutPath()
{
	PolyEdgeLib::CPolyMgr Frame;
	Frame.mMinLineDistMerge2 = 550;
	if (mRawWalls.Num() == 0)
		return false;

	Frame.AddPoly(mRawWalls, 0, true);
	Frame.AdjustFrame(mRawAdjustWalls);
	Frame.CombitFrame(mOutPaths, mPillars);
	return true;
}
bool FCabinetArea::AdjustSelect(TLine2d& li)
{
	for (auto wall : mOutPaths)
	{
		FVector2D CrossStart;
		float fDot0;
		float fDistance0 = Point2LineSegVer(wall.mStart, wall.mEnd, li.mStart, fDot0, CrossStart);
		if (fDistance0 > 10)
			continue;
		FVector2D CrossEnd;
		float fDot1;
		float fDistance1 = Point2LineSegVer(wall.mStart, wall.mEnd, li.mEnd, fDot1, CrossEnd);
		if (fDistance1 > 10)
			continue;
		if (FMath::Abs(fDistance0 - fDistance1) > 10)
			continue;

		if (fDot0 > fDot1)
		{
			TinySwap(CrossStart, CrossEnd);
		}
		if (FVector2D::Distance(wall.mStart, CrossStart) < 50 )
		{
			CrossStart = wall.mStart;
		}
		if (FVector2D::Distance(wall.mEnd, CrossEnd) < 50)
		{
			CrossEnd = wall.mEnd;
		}
		if ((CrossStart - CrossEnd).Size() > 100)
		{
			li.mStart = CrossStart;
			li.mEnd = CrossEnd;
			return true;
		}
	}
	return false;
}
TLine2d* FCabinetArea::GetWall(int id)
{
	TLine2d* pRet = mRawAdjustWalls.Find(id);
	if (pRet)
		return pRet;
	return nullptr;
}

bool FCabinetAreaMgr::ChangAreaTag(FString newid, FString oldid)
{
	FCabinetArea** pRet = mAreas.Find(oldid);
	if (!pRet)
		return false;
	(*pRet)->mName = newid;
	mAreas.Remove(oldid);
	mAreas.Add(newid, (*pRet));
	return true;
}

bool FCabinetArea::ClearCabinet(eAutoSetType eType)
{
	mCabinetGroup[eType].Clear();
	return false;
}
bool FCabinetArea::GetFurnitureModel(int nType, TArray<TRectBase>& Rects)
{
	Rects.Empty();

	UCEditorGameInstance* EditorGameInstance = Cast<UCEditorGameInstance>(gCabinetMgr.GetWorld()->GetGameInstance());
	AGameEditorPawn_Base* mypawn = Cast<AGameEditorPawn_Base>(UGameplayStatics::GetPlayerPawn(gCabinetMgr.GetWorld(), 0));
	TArray<AFurnitureModelActor*> FurnitureModels = EditorGameInstance->AreaSystem->CollectRelatedModelsOfRoom(mypawn->HouseCustomizationArea, false);
	for (auto p : FurnitureModels)
	{
		if (nType == 0 && p->BuildingData == nullptr)
		{
			FVector pos;
			FVector Size;
			p->GetActorBounds(true, pos, Size);
			FVector dir = p->GetActorRightVector();
			TRectBase rect;
			rect.mPos = FVector2D(pos.X * 10, pos.Y * 10);
			rect.mDir = FVector2D(1, 0);
			rect.mHeight = pos.Z * 10 + Size.Z * 10;
			rect.mSize = FVector2D(Size.X * 10, Size.Y * 10);
			if (pos.Z * 10 <= 2500 && rect.mHeight <= 2500 && rect.mHeight >= 100 && rect.mSize.X > 100 && rect.mSize.Y > 100 && Size.Z > 10)
			{
				Rects.Add(rect);
			}
		}

		UDRProjData *ProjectManager = UDRProjData::GetProjectDataManager(p);
		if (ProjectManager != nullptr)
		{
			UDRWindowHoleAdapter *WindowHoleAdapter = Cast<UDRWindowHoleAdapter>(ProjectManager->GetAdapter(p->ObjectId));
			if (WindowHoleAdapter)
			{
				if ((nType == 1 || nType == 2) && WindowHoleAdapter->GetHoleType() == EModelType::EDRBayWindow)
				{
					FVector pos;
					FVector Size;
					p->GetActorBounds(true, pos, Size);
					auto Transform = WindowHoleAdapter->GetTransform();
					FVector2D dir2(Transform.GetLocation().X - pos.X, Transform.GetLocation().Y - pos.Y);
					dir2.Normalize();

					 
					TRectBase rect;
					rect.mPos = FVector2D(Transform.GetLocation()) * 10;
					rect.mDir = FVector2D(cos(Transform.Rotator().Yaw*PI / 180), sin(Transform.Rotator().Yaw*PI / 180));
					rect.mPos += dir2 * 100;
					rect.mHeight = WindowHoleAdapter->GetHeight() * 10;
					rect.mPosZ = WindowHoleAdapter->GetGroundHeight() * 10;
					rect.mSize = FVector2D(WindowHoleAdapter->GetWidth() * 5, 5);
					if (rect.mHeight > 300)
						Rects.Add(rect);
				}
			}
		}
	}
	return true;
}

int  FCabinetArea::GetCabinets(TArray<FCabinetShell*>& Cabinets, eAutoSetType eSetType, eSplineType bFloor)
{
	return mCabinetGroup[eSetType].GetCabinets(Cabinets, bFloor);
}

FCabinetShell * FCabinetArea::GetCabinets(eAutoSetType eSetType, int eType)
{
	for (auto p : mCabinetGroup[eSetType].mCabinets)
	{
		if (p.Value->mTeplateId == eType )
		{
			return p.Value;
		}
	}
	return nullptr;
}
//////////////////////////////////////////////////////////////////////////
bool FCabinetArea::SetSplineMaterial(FString MxFileName, eSplineType eType, eAutoSetType eSetType)
{
	mCabinetGroup[eSetType].SetSplineMaterial(MxFileName, eType);
	return true;
}

bool FCabinetArea::SetSplineMesh(FString MxFileName, eSplineType eType, eAutoSetType eSetType)
{
	mCabinetGroup[eSetType].SetSplineMesh(MxFileName, eType);
	return true;
}

bool FCabinetArea::SetSplineVisble(int SplineType, bool bVisble, eAutoSetType eSetType)
{
	mCabinetGroup[eSetType].SetSplineVisble(eSplineType(SplineType) ,bVisble);
	return true;
}
//////////////////////////////////////////////////////////////////////////
FCabinetAreaMgr::FCabinetAreaMgr()
{
	mCurArea = NULL;
}
FCabinetArea* FCabinetAreaMgr::GetArea(FString Id)
{
	FCabinetArea** pRet = mAreas.Find(Id);
	if (pRet)
	{
		mCurArea = *pRet;
		return *pRet;
	}
	else
	{
		
			auto temp = new	FCabinetArea();
			temp->mName = Id;
			mAreas.Add(temp->mName, temp);
		
		return temp;
	}
	return nullptr;
}
FCabinetArea* FCabinetAreaMgr::NewArea(FString Id)
{
	FCabinetArea* pRet = GetArea(Id);
	if (pRet == NULL)
	{
		pRet = new	FCabinetArea();
		pRet->mName = Id;
		mAreas.Add(pRet->mName, pRet);
	}
	mCurArea = pRet;
	return pRet;
}
bool FCabinetAreaMgr::Clear()
{
	for ( auto p:mAreas )
	{
		delete p.Value;
	}
	mAreas.Empty();
	return true;
}
bool FCabinetArea::GetSaveData(FCabinetAreaSave & SaveData)
{
	SaveData.mSkuSink.mSku.mSkuId = mSkus[0].mSku.mSkuId;
	SaveData.mSkuSink.mSku.mCategoryId = mSkus[0].mSku.mCategoryId;
	SaveData.mSkuSink.mSku.mName = mSkus[0].mSku.mName;
	SaveData.mSkuSink.mSku.mMxFileMD5 = mSkus[0].mSku.mMxFileMD5;
	SaveData.mSkuSink.mSku.mSize = mSkus[0].mSku.mSize;

	SaveData.mSkuSink.mPos = mSkus[0].mPos;
	SaveData.mSkuSink.mDir = mSkus[0].mDir;
	SaveData.mSkuSink.mLeftCabinetId = mSkus[0].mLeftCabinetId;
	SaveData.mSkuSink.mRightCabinetId = mSkus[0].mRightCabinetId;
	SaveData.mSkuSink.mHeight = mSkus[0].mHeight;

	SaveData.mSkuChimery.mSku.mSkuId = mSkus[1].mSku.mSkuId;
	SaveData.mSkuChimery.mSku.mCategoryId = mSkus[1].mSku.mCategoryId;
	SaveData.mSkuChimery.mSku.mName = mSkus[1].mSku.mName;
	SaveData.mSkuChimery.mSku.mMxFileMD5 = mSkus[1].mSku.mMxFileMD5;
	SaveData.mSkuChimery.mSku.mSize = mSkus[1].mSku.mSize;

	SaveData.mSkuChimery.mPos = mSkus[1].mPos;
	SaveData.mSkuChimery.mDir = mSkus[1].mDir;
	SaveData.mSkuChimery.mLeftCabinetId = mSkus[1].mLeftCabinetId;
	SaveData.mSkuChimery.mRightCabinetId = mSkus[1].mRightCabinetId;
	SaveData.mSkuChimery.mHeight = mSkus[1].mHeight;

	SaveData.mName = mName;
	for (auto p : mRawWalls)
	{
		TLine2d& l = p.Value;
		FLine2dSave data;
		data.mStart = l.mStart;
		data.mEnd = l.mEnd;
		data.Id = l.Id;
		data.mState = l.mState;
		SaveData.mWalls.Add(data);
	}
	for ( int i=0; i<eAutoSetNum;++i )
	{
		FCabinetSetGroupBlue GroupSave;
		mCabinetGroup[i].GetSaveData(GroupSave);
		SaveData.mCabinetGroups.Add(GroupSave);
	}
	return true;
}
bool FCabinetAreaMgr::DeleteArea(FString sName)
{
	FCabinetArea* pArea = GetArea(sName);
	if ( pArea )
	{
		mAreas.Remove(pArea->mName);
		delete pArea;
	}
	return false;
}
bool FCabinetAreaMgr::SaveCabinetMgr(FCabinetAreaMgrSave & Datas)
{
	Datas.mAreas.Empty();
	Datas.mVersion = TCabibetSaveVersion;
	Datas.mCabinetIdxMax = gCabinetMgr.GenId();

	for (int i = 0; i < eCabinetAuxObjecNum; ++i)
	{
		Datas.mAuxMxs.Add(gCabinetGlobal.mCabinetAuxMx[i]);
	}
	for (auto p : mAreas)
	{
		FCabinetAreaSave data;
		p.Value->GetSaveData(data);
		Datas.mAreas.Add(data);
	}
	return false;
}

bool FCabinetArea::LoadPreSaveSku()
{
	mSkus[0].mSku.mSkuId = mSkuSink.mSku.mSkuId;
	mSkus[0].mSku.mCategoryId = mSkuSink.mSku.mCategoryId;
	mSkus[0].mSku.mName = mSkuSink.mSku.mName;
	mSkus[0].mSku.mMxFileMD5 = mSkuSink.mSku.mMxFileMD5;
	mSkus[0].mSku.mSize = mSkuSink.mSku.mSize;

	mSkus[0].mPos = mSkuSink.mPos;
	mSkus[0].mDir = mSkuSink.mDir;
	mSkus[0].mLeftCabinetId = mSkuSink.mLeftCabinetId;
	mSkus[0].mRightCabinetId = mSkuSink.mRightCabinetId;
	mSkus[0].mHeight = mSkuSink.mHeight;
	mSkus[0].LoadSku();

	mSkus[1].mSku.mSkuId = mSkuChimery.mSku.mSkuId;
	mSkus[1].mSku.mCategoryId = mSkuChimery.mSku.mCategoryId;
	mSkus[1].mSku.mName = mSkuChimery.mSku.mName;
	mSkus[1].mSku.mMxFileMD5 = mSkuChimery.mSku.mMxFileMD5;
	mSkus[1].mSku.mSize = mSkuChimery.mSku.mSize;

	mSkus[1].mPos = mSkuChimery.mPos;
	mSkus[1].mDir = mSkuChimery.mDir;
	mSkus[1].mLeftCabinetId = mSkuChimery.mLeftCabinetId;
	mSkus[1].mRightCabinetId = mSkuChimery.mRightCabinetId;
	mSkus[1].mHeight = mSkuChimery.mHeight;;
	return true;
}
bool FCabinetArea::GetCabinet(eAutoSetType eType, TMap<int32, FCabinetShell*>& Cabinets )
{
	Cabinets = mCabinetGroup[eType].mCabinets;
	if (eType == eAutoSetHang)
	{
		TMap<int32, FCabinetShell*> CabinetFloors = mCabinetGroup[eAutoSetFloor].mCabinets;
		Cabinets.Append(CabinetFloors);
	}
	if (eType == eAutoSetWardorbeHang)
	{
		TMap<int32, FCabinetShell*> CabinetFloors = mCabinetGroup[eAutoSetWardrobe].mCabinets;
		Cabinets.Append(CabinetFloors);
	}
	if (eType == eAutoSetToiletHang)
	{
		TMap<int32, FCabinetShell*> CabinetFloors = mCabinetGroup[eAutoSetToilet].mCabinets;
		Cabinets.Append(CabinetFloors);
	}
	if (eType == eAutoSetCoatroomHang)
	{
		TMap<int32, FCabinetShell*> CabinetFloors = mCabinetGroup[eAutoSetCoatroom].mCabinets;
		Cabinets.Append(CabinetFloors);
	}
	return false;
}
bool FCabinetArea::LoadData(FCabinetAreaSave & SaveData,bool bDNA)
{
	mName = SaveData.mName;
	mSkuSink = SaveData.mSkuSink;
	mSkuChimery = SaveData.mSkuChimery;
	LoadPreSaveSku();

	for (auto w : SaveData.mOutWallLines)
	{
		AddRawWall(w.WallID, w.SegStart, w.SegEnd, w.VerticalDir);
	}
	for ( auto p: SaveData.mWalls)
	{
		TLine2d l;
		FLine2dSave data = p;
		l.mStart  = data.mStart;
		l.mEnd    = data.mEnd;
		l.Id	  = data.Id ;
		l.mState  = data.mState;
		mRawWalls.Add(l.Id,l);
	}
	GenOutPath();
	//
	for ( int i=0; i<SaveData.mCabinetGroups.Num(); ++i )
	{
		mCabinetGroup[i].LoadSaveData(SaveData.mCabinetGroups[i]);
	}
	return true;
}
bool FCabinetArea::CompleteLayout(int nType)
{
	mCabinetGroup[nType].GenExtern();
	return true;
}
bool FCabinetAreaMgr::LoadCabinetMgr(const FCabinetAreaMgrSave & Datas, bool bDNA)
{
	CHECK_ERROR(Datas.mVersion == TCabibetSaveVersion);
	gCabinetMgr.SetId(Datas.mCabinetIdxMax);
	for (int i = 0; i < eCabinetAuxObjecNum; ++i)
	{
		gCabinetGlobal.mCabinetAuxMx[i] = Datas.mAuxMxs[i];
	}
	for (auto p : Datas.mAreas)
	{
		if (!p.mName.IsEmpty())
		{
			FCabinetArea* pArea = NewArea(p.mName);
			if (pArea)
			{
				pArea->LoadData(p, bDNA);
			}
		}
	}
	return true;
}