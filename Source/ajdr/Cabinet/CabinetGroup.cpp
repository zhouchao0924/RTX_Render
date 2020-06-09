
#include "CabinetGroup.h"
#include "CabinetArea.h"
#include "CabinetMgr.h"
#include "base/FrameEdge.h"
#include "base/PolygonTool.h"
#include "CabinetGlobal.h"
//////////////////////////////////////////////////////////////////////////
bool FCabinetSetGroup::Clear()
{
	for (auto p : mCabinets)
	{
		delete p.Value;
	}
	mCabinets.Empty();

	ClearTable();
	for ( int i=0; i< eSplineTypeNum; ++i )
	{
		ClearSpline(eSplineType(i));
	}
	return true;
}

bool FCabinetSetGroup::ClearExtern()
{
	ClearTable();
	for (int i = 0; i < eSplineTypeNum; ++i)
	{
		ClearSpline(eSplineType(i));
	}
	return true;
}

bool FCabinetSetGroup::ClearTable()
{
	for(auto p: mTables)
	{
		p->Destroy();
	}
	mTables.Empty();
	return true;
}
bool FCabinetSetGroup::ClearSpline(eSplineType eType)
{
	for (auto p : mSplines[eType])
	{
		p->Destroy();
	}
	mSplines[eType].Empty();
	return true;
}
bool FCabinetSetGroup::GenExtern()
{
	CHECK_ERROR(!mArea->mName.IsEmpty());
	ClearTable();
	for (int i = 0; i < eSplineTypeNum; ++i)
	{
		ClearSpline(eSplineType(i));
	}
	GenTable(eSplineFloorBottom);
	GenTable(eSplineTableBack);
	GenTable(eSplineHangTop);
	return true;
}
bool FCabinetSetGroup::GenTable(eSplineType eType)
{
	TArray<FCabinetShell*> CabinetFloors;
	GetCabinets(CabinetFloors, eType);
	CHECK_ERROR(CabinetFloors.Num() > 0);
	float fHeight = GetCabinetTopZ(CabinetFloors);
	if (eType == eSplineTableBack)
		fHeight += 15;
	FXRPolygonTool PolygonTool;
	PolygonTool.SetStandPath(&mArea->mOutPaths);
	TArray<TPath2d> PolyPaths;
	//
	for (auto p : CabinetFloors)
	{
		int nTurnSize = GetTurnSize(p->mTeplateId);
		if (nTurnSize>0)
		{
			PolygonTool.AddTurnPoly(p->mPos, p->mSize*p->mScale, p->mDir, nTurnSize);
		}
		else
		{
			if (p->mRes.mTeplateId == 200105)
			{
				FVector2D PosTable(p->mTablePos.X, p->mTablePos.Y);
				FVector2D Dir = -p->mDir;
				FVector2D Pos = Local2World(p->mPos, Dir, p->mScale, PosTable);
				FVector2D Size = p->mTableSize* p->mScale;
				PolygonTool.AddPoly(Pos, Size, p->mDir);
				fHeight = 880;// p->mTablePos.Z;
			}
			else
			{
				PolygonTool.AddPoly(p->mPos, FVector2D(FMath::CeilToInt(p->mSize.X*p->mScale.X+2), p->mSize.Y*p->mScale.Y), p->mDir);
			}			
			if (mSetType != eAutoSetFloor )
			{
				FVector2D vScale(1.0f, 1.0f);
				FVector2D Dir = -p->mDir;
				FVector2D Pos = Local2World(p->mPos, Dir,p->mScale,p->mSkinPos);
				PolygonTool.AddHole(Pos, p->mDir, vScale, p->mHoles[0]);
			}
		}
		//////////////////////////////////////////////////
	}
	for (auto p : mArea->mPillars)
	{
		PolygonTool.AddHole(p.mPos, p.mSize*p.mScale, p.mDir);
	}
	//if (mSetType == eAutoSetFloor && eType == eSplineTableBack)
	//{
	//	FVector2D vScale(1.0f, 1.0f);
	//	//PolygonTool.AddHole(mArea->mSkus[0].mPos, mArea->mSkus[0].mDir, vScale, mArea->mSkus[0].mHoles[0]);
	//}
	PolygonTool.Execute();
	PolygonTool.GetPaths(PolyPaths);
	AdjustTablePath(PolyPaths, fHeight,eType);
	return true;
}
bool FCabinetSetGroup::AdjustTablePath(TArray<TPath2d>& PolyPaths, float fHegiht, eSplineType eType)
{
	//TPath2d Hole;
	//if (mSetType == eAutoSetFloor && eType == eSplineTableBack)
	//{
	//	for (int i = 0; i < mArea->mSkus[0].mHoles[0].Num(); ++i)
	//	{
	//		FVector2D vScale(1.0f, 1.0f);
	//		FVector2D Posw = Local2World(mArea->mSkus[0].mPos, mArea->mSkus[0].mDir, vScale, mArea->mSkus[0].mHoles[0][i]);
	//		Hole.mOutLine.Push(Posw);
	//	}
	//}
	//for (auto Frame : PolyPaths)
	//{
	//	PolyEdgeLib::CPolyMgr PolyMgr;
	//	PolyMgr.AddPoly(Frame.mOutLine, eSliptWallSelect| eSliptWall, true);
	//	PolyMgr.AddPoly(mArea->mOutPaths, eSliptWallSolid);
	//	PolyMgr.AddPoly(mArea->mPillars, eSliptWallSolid);
	//	PolyMgr.SignFrame();

	//	if (mSetType == eAutoSetFloor && eType == eSplineTableBack)
	//	{
	//		Frame.mHoles.Push(Hole);
	//	}

	//	int nExternBotom = gCabinetGlobal.mSkirtboardElapse;
	//	if ( eType == eSplineHangTop )
	//	{
	//		nExternBotom = 0;
	//	}
	//	TArray<FVector2D> PathFrame, PathOnWall, PathFloorBottom,PathFront;
	//	PolyMgr.GetOutLine(PathFrame, PathFloorBottom, PathOnWall, PathFront, gCabinetGlobal.mTableFrontElapse, nExternBotom, eSliptWallSelect,eSliptWallSolid);

	//	if ( eType == eSplineFloorBottom )
	//	{
	//		ASplineActor* pActorSkirtboard = gCabinetMgr.NewSpline();
	//		pActorSkirtboard->Init(PathFloorBottom, false, mSplinesMesh[eSplineFloorBottom], 0, eSplineFloorBottom);
	//		pActorSkirtboard->SetMaterial(mSplinesMaterial[eSplineFloorBottom]);
	//		mSplines[eSplineFloorBottom].Add(pActorSkirtboard);
	//	}
	//	else if (eType == eSplineHangTop)
	//	{
	//		ASplineActor* pActorSkirtboard = gCabinetMgr.NewSpline();
	//		auto TempList = PathFloorBottom;
	//		PathFloorBottom.Empty();
	//		for (int i = TempList.Num() - 1; i >= 0; --i)
	//			PathFloorBottom.Add(TempList[i]);
	//		
	//		AdjustSplineHangTop(PathFloorBottom);

	//		pActorSkirtboard->Init(PathFloorBottom, false, mSplinesMesh[eSplineHangTop], fHegiht, eSplineHangTop);
	//		pActorSkirtboard->SetMaterial(mSplinesMaterial[eSplineHangTop]);
	//		mSplines[eSplineHangTop].Add(pActorSkirtboard);
	//	}
	//	else if ( eType == eSplineTableBack && (mSetType == eAutoSetFloor || mSetType == eAutoSetToilet || mSetType == eAutoSetWashCabinet) )
	//	{
	//		ASplineActor* pActorBack = gCabinetMgr.NewSpline();
	//		pActorBack->Init(PathOnWall, false, mSplinesMesh[eSplineTableBack], fHegiht + gCabinetGlobal.mTableThickness, eSplineTableBack);
	//		pActorBack->SetMaterial(mTableMeterial);
	//		mSplines[eSplineTableBack].Add(pActorBack);

	//		ASplineActor* pActorFront = gCabinetMgr.NewSpline();
	//		pActorFront->Init(PathFront, true, /*mSplinesMesh[eSplineTableBack]*/mSplinesMesh[eSplineFront], fHegiht -25 /*gCabinetGlobal.mTableThickness*/, eSplineFront);
	//		pActorFront->SetMaterial(mTableMeterial);

	//		mSplines[eSplineFront].Add(pActorFront);

	//		ATableActor* pTable = gCabinetMgr.NewTable();
	//		pTable->Init(PathFrame, Frame.mHoles, gCabinetGlobal.mTableThickness, mTableMeterial, fHegiht);
	//		mTables.Add(pTable);
	//	}
	//}
	return true;
}
int FCabinetSetGroup::GetCabinets(TArray<FCabinetShell*>& Cabinets, eSplineType bFloor)
{
	Cabinets.Empty();
	for (auto p : mCabinets)
	{
		if ( bFloor == eSplineTypeNull )
		{
			Cabinets.Add(p.Value);
			continue;
		}
		else if (p.Value->mRes.mbSpline[bFloor] )
		{
			Cabinets.Add(p.Value);
			continue;
		}
	}
	return Cabinets.Num();
}

float FCabinetSetGroup::GetCabinetTopZ(TArray<FCabinetShell*>& Cabinets)
{
	float fMaxZ = FLT_MIN;
	for ( auto p: Cabinets )
	{
		if ( p->mPosZ+p->mRes.mCabinetHeight > fMaxZ )
		{
			fMaxZ = p->mPosZ + p->mRes.mCabinetHeight;
		}
	}
	return fMaxZ;
}
bool FCabinetSetGroup::SetSplineMaterial(FString MxFileName, eSplineType SplineType)
{
	mSplinesMaterial[SplineType] = MxFileName;
	for (auto p : mSplines[SplineType])
	{
		p->SetMaterial(MxFileName);
	}
	if (SplineType == eSplineTableBack)
	{
		mTableMeterial = MxFileName;
		for (auto p : mTables)
		{
			p->SetMaterial(MxFileName);
		}
		for (auto p : mSplines[eSplineFront])
		{
			p->SetMaterial(MxFileName);
		}
	}
	return true;
}

bool FCabinetSetGroup::SetSplineMesh(FString MxFileName, eSplineType SplineType)
{
	mSplinesMesh[SplineType] = MxFileName;
	for (auto p : mSplines[SplineType])
	{
		p->SetMesh(MxFileName);
	}
	return true;
}

bool FCabinetSetGroup::SetHangTop(float PosZ)
{
	float fMaxZ = FLT_MIN;
	for (auto p: mCabinets )
	{
		if ( p.Value->mPosZ >=500 )
		{
			p.Value->mPosZ = PosZ;
			fMaxZ = TinyMax(fMaxZ, p.Value->mPosZ + p.Value->mRes.mCabinetHeight);
		}
	}
	for ( auto p: mSplines[eSplineHangTop] )
	{
		p->SetPosZ(fMaxZ);
	}
	return true;
}

float FCabinetSetGroup::SetHardModle(FString MxFileName, int32 CabinetId,int CategoryId,int SkuId)
{
	auto ppCabinet = mCabinets.Find(CabinetId);
	if (ppCabinet == nullptr)
		return 0;
	FCabinetShell* pShell = *ppCabinet;
	float PosZ= pShell->ChangeHard(MxFileName, CategoryId, SkuId);
	GenExtern();
	return PosZ;
}

FCabinetShell * FCabinetSetGroup::GetCabinet(int32 CabinetId)
{
	auto ppCabinet = mCabinets.Find(CabinetId);
	if (ppCabinet == nullptr)
		return nullptr;
	FCabinetShell* pShell = *ppCabinet;
	return pShell;
}

bool FCabinetSetGroup::DeleteCabinet(int32 CabinetId)
{
	auto ppCabinet = mCabinets.Find(CabinetId);
	if (ppCabinet == nullptr)
		return false;

	FCabinetShell* pShell = *ppCabinet;
	delete pShell;
	mCabinets.Remove(CabinetId);
	return true;
}

bool FCabinetSetGroup::GetSaveData(FCabinetSetGroupBlue & GroupSave)
{
	GroupSave.mSetType = mSetType;

	for (int i = 0; i < eSplineTypeNum; ++i)
	{
		GroupSave.mSplinesMesh.Add(mSplinesMesh[i]);
	}

	for (int i = 0; i < eSplineTypeNum; ++i)
	{
		GroupSave.mSplineMaterial.Add(mSplinesMaterial[i]);
	}

	GroupSave.mTableMeterial = mTableMeterial;

	for (auto p : mCabinets)
	{
		FCabinetShell* pShell = p.Value;
		FCabinetSave data;
		data.Id = pShell->Id;
		data.mSize = pShell->mSize;
		data.mScale = pShell->mScale;
		data.mDir = pShell->mDir;
		data.mPos = pShell->mPos;
		data.mTemplateId = pShell->mTeplateId;
		data.mPosZ = pShell->mPosZ;
		data.mHeight = pShell->mRes.mCabinetHeight;
		data.mbSpline[0] = pShell->mRes.mbSpline[0];
		data.mbSpline[1] = pShell->mRes.mbSpline[1];
		data.mbSpline[2] = pShell->mRes.mbSpline[2];
		data.mHardSkuMd5 = pShell->mRes.mHardSkuMd5;
		data.mCategoryId = pShell->mRes.mCategoryIds;
		data.mBoyMd5 = pShell->mRes.mBoyMd5;
		data.mMeshScale = pShell->mRes.mMeshScale;
		GroupSave.mCabinets.Add(data);
	}
	mGroupSave = GroupSave;
	return true;
}

bool FCabinetSetGroup::LoadSaveData(FCabinetSetGroupBlue & GroupSave)
{
	if (&GroupSave != &mGroupSave)
		mGroupSave = GroupSave;
	for (int i = 0; i < eSplineTypeNum; ++i)
	{
		mSplinesMesh[i] = GroupSave.mSplinesMesh[i];
	}
	for (int i = 0; i < eSplineTypeNum; ++i)
	{
		mSplinesMaterial[i] = GroupSave.mSplineMaterial[i];
	}
	mTableMeterial = GroupSave.mTableMeterial;

	for (auto p : GroupSave.mCabinets)
	{
		FCabinetShell* pShell = new FCabinetShell;
		FCabinetSave& data = p;
		pShell->mSize.X = data.mSize.X;
		pShell->mSize.Y = data.mSize.Y;
		pShell->mScale.X = data.mScale.X;
		pShell->mScale.Y = data.mScale.Y;
		pShell->mDir = data.mDir;
		pShell->mPos = data.mPos;
		pShell->Id = data.Id;
		pShell->mSetType = (eAutoSetType)GroupSave.mSetType;
		pShell->mTeplateId = data.mTemplateId;
		pShell->mRes.mTeplateId = data.mTemplateId;
		pShell->mRes.mbSpline[0] = data.mbSpline[0];
		pShell->mRes.mbSpline[1] = data.mbSpline[1];
		pShell->mRes.mbSpline[2] = data.mbSpline[2];
		pShell->mPosZ = data.mPosZ;
		pShell->mRes.mCabinetHeight = data.mHeight;
		pShell->mRes.mHardSkuMd5 = data.mHardSkuMd5;
		pShell->mRes.mCategoryIds = data.mCategoryId;
		pShell->mRes.mBoyMd5 = data.mBoyMd5;
		pShell->mRes.mMeshScale = data.mMeshScale;
		pShell->Init();
		mCabinets.Add(pShell->Id, pShell);
	}
	GenExtern();
	return true;
}

bool FCabinetSetGroup::LoadPreSaveData()
{
	if (mGroupSave.mSetType != mSetType)
	{
		return Clear();
	}
	else
	{
		return LoadSaveData(mGroupSave);
	}
}

bool FCabinetSetGroup::SetSplineVisble(eSplineType SplineType, bool bVisble)
{
	for (auto p : mSplines[SplineType])
	{
		p->SetActorHiddenInGame(bVisble);
	}
	if(SplineType == eSplineTableBack )
	{
		for ( auto p: mTables )
		{
			p->SetActorHiddenInGame(bVisble);
		}
		for (auto p : mSplines[eSplineFront])
		{
			p->SetActorHiddenInGame(bVisble);
		}
	}
	return true;
}

void FCabinetSetGroup::AdjustSplineHangTop(TArray<FVector2D> &PathFloorBottom)
{
	if (PathFloorBottom.Num() <= 2)
		return;

	FVector2D &FirLinePt1 = PathFloorBottom[0];
	FVector2D &FirLinePt2 = PathFloorBottom[1];
	FVector2D &LastLinePt1 = PathFloorBottom.Last(0);
	FVector2D &LastLinePt2 = PathFloorBottom.Last(1);
	float Diff = 0;

	for (const auto &Wall : mSelectWalls) 
	{
		if(IfPointOnWall(FirLinePt1, Wall))
		{
			FVector2D LineDir = PathFloorBottom[2] - PathFloorBottom[1];
			if (LineDir.Y == 0)
			{
				Diff = LineDir.X > 0 ? 18 : -18;
				AdjustSplineHangTopLine(FirLinePt1, FirLinePt2, Diff);
			}
			else
			{
				Diff = LineDir.Y > 0 ? 18 : -18;
				AdjustSplineHangTopLine(FirLinePt1, FirLinePt2, Diff);
			}
			break;
		}
	}

	for (const auto &Wall : mSelectWalls) 
	{
		if (IfPointOnWall(LastLinePt1, Wall))
		{
			FVector2D LineDir = PathFloorBottom.Last(2) - PathFloorBottom.Last(1);
			if (LineDir.Y == 0)
			{
				Diff = LineDir.X > 0 ? 18 : -18;
				AdjustSplineHangTopLine(LastLinePt1, LastLinePt2, Diff);
			}
			else
			{
				Diff = LineDir.Y > 0 ? 18 : -18;
				AdjustSplineHangTopLine(LastLinePt1, LastLinePt2, Diff);
			}
			break;
		}
	}
}

void FCabinetSetGroup::FindSelectWall(const TArray<TLine2d> &SelectWalls)
{
	mSelectWalls.Empty();
	  
	for (const auto &Line : SelectWalls) 
	{
		for (const auto &Wall : mArea->mOutPaths) 
		{
			
			if (IfPointOnWall(Line.mStart, Wall) && IfPointOnWall(Line.mEnd, Wall))
			{
				mSelectWalls.Add(Wall);
				break;
			}
		}
	}
}

bool FCabinetSetGroup::IfPointOnWall(const FVector2D &Point, const TLine2d &Wall)
{
	FVector2D Direction = Wall.mStart - Wall.mEnd;
	float UpValue(0);
	float DownValue(0);

	if (Direction.Y == 0) //水平墙体
	{
		UpValue = std::max(Wall.mStart.X, Wall.mEnd.X);
		DownValue = std::min(Wall.mStart.X, Wall.mEnd.X);

		return (abs(Point.Y - Wall.mStart.Y) <= 0.1) && (Point.X >= DownValue && Point.X <= UpValue);
	}
	else //垂直墙体
	{
		UpValue = std::max(Wall.mStart.Y, Wall.mEnd.Y);
		DownValue = std::min(Wall.mStart.Y, Wall.mEnd.Y);

		return (abs(Point.X - Wall.mStart.X) <= 0.1) && (Point.Y >= DownValue && Point.Y <= UpValue);
	}
	
	return false;
}

void FCabinetSetGroup::AdjustSplineHangTopLine(FVector2D &StartPoint, FVector2D &EndPoint, float Value)
{
	FVector2D Dir = EndPoint - StartPoint;

	//水平方向
	if (Dir.Y == 0) 
	{
		StartPoint.Y += Value;
		EndPoint.Y += Value;
	}
	else 
	{
		StartPoint.X += Value;
		EndPoint.X += Value;
	}

}
