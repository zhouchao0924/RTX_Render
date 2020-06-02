
#include "CabinetSetBase.h"
#include "../CabinetMgr.h"
#include "../CabinetSdk.h"
#include "../CabinetArea.h"
#include "../CabinetGroup.h"
#include "HouseFunction/HouseFunctionLibrary.h"
using namespace PolyEdgeLib;
FAutoSetBase::FAutoSetBase()
{
	mArea = nullptr;
	mHeight = 0.0f;
	mBit = eSliptWallSelect | eSliptWall;
	mExpactBit = eSliptWallNull;

	mMinPointDistMerge = 10;
	mMinLineDistMerge = 100;
	mMinLineDistMerge2 = 50;
	mReplaceReseat = true;
}
FAutoSetBase::~FAutoSetBase()
{
	Clear();
}
bool FAutoSetBase::Clear()
{
	CPolyMgr::Clear();
	mSetEdgeHead = nullptr;
	mSelectWalls.Empty();
	mCabinetType = 0;
	mSelectSegs.Empty();
	mFailData.mFailDatas.Empty();
	mFailData.msg = "";
	return true;
}
bool FAutoSetBase::Execute(FCabinetArea* pArea)
{
	mArea = pArea;
	CHECK_ERROR(ExecuteBase());
	CHECK_ERROR(ExecuteSelete());
	return true;
}

bool FAutoSetBase::OtherExecute(FCabinetArea* pArea)
{
	mArea = pArea;
	CHECK_ERROR(ExecuteBase());
	CHECK_ERROR(ExecuteSelete());
	return true;
}


bool FAutoSetBase::LayoutCabinet(class FCabinetArea* pArea)
{
	mErrorCodeId = 0;
	mCabinetType = 0;
	mCabinetHangType = 0;
	mArea = pArea;
	CheckOverlay();
	Execute(pArea);
	return true;
}

bool FAutoSetBase::ReLayoutCabinet(TSetEdge * pSetEdge, int32 CabinetIdx, int32 mcategoryId)
{
	TArray<FCabinetSet> OldCabinets = pSetEdge->mCabinets;
	FCabinetSet& Cabinet= pSetEdge->mCabinets[CabinetIdx];
	if ((IsTurnCabinet(Cabinet.mRes.mTeplateId) || IsTurnCabinet(mcategoryId)))
		return ReLayoutTurn(pSetEdge, CabinetIdx, mcategoryId);
	float XSize = Cabinet.mScale.X* Cabinet.mRes.mSize.X;
	Cabinet.mRes = GetRes(mcategoryId);
	Cabinet.mScale.X = XSize / Cabinet.mRes.mSize.X;
	if (GetSuitable(Cabinet))
		return true;
	if (mSetType == eAutoSetTatami)
		return true;
	if (pSetEdge->AdjustCabinets() != 0)
	{
		pSetEdge->mCabinets = OldCabinets;
		return false;
		//pSetEdge->ReSetClearNormal();
		//SetSetEdge(pSetEdge);
	}
	return true;
}

bool FAutoSetBase::ReLayoutTurn(TSetEdge * pSetEdge, int32 CabinetIdx, int32 mcategoryId)
{
	TArray<FCabinetSet> OldCabinte1 = pSetEdge->mCabinets;

	FCabinetSet& Cabinet = pSetEdge->mCabinets[CabinetIdx];
	CHECK_ERROR(IsTurnCabinet(Cabinet.mRes.mTeplateId) || IsTurnCabinet(mcategoryId));
	float XSize = Cabinet.mScale.X* Cabinet.mRes.mSize.X;
	Cabinet.mRes = GetRes(mcategoryId);
	pSetEdge->AdjustCabinets();
	//Cabinet.mScale.X = XSize / Cabinet.mRes.mSize.X;
	//Cabinet.mPos = pSetEdge->mStart->GetVector() + Cabinet.mPosLocal*pSetEdge->mDir + pSetEdge->mNor * Cabinet.mRes.mSize.Y / 2;
	GetSuitable(Cabinet);
	if (IsTurnCabinet(Cabinet.mRes.mTeplateId))
	{
		pSetEdge->AdjustTurnDir(Cabinet);
	}
	else
	{
		Cabinet.mDir = pSetEdge->mDir;
	}
	TSetEdge* pAdjustSeg = nullptr;
	int32 FuckIdx = -1;
	if (Cabinet.mAlign == FCabinetSet::eAlignHead)
	{
		pAdjustSeg = pSetEdge->PreSeg();
		for (int i = 0; i < pAdjustSeg->mCabinets.Num(); ++i)
		{
			if (pAdjustSeg->mCabinets[i].mAlign == FCabinetSet::eAlignTail && pAdjustSeg->mCabinets[i].mRes.mTeplateId == -1)
			{
				FuckIdx = i;
				break;
			}
		}
	}
	if (Cabinet.mAlign == FCabinetSet::eAlignTail)
	{
		pAdjustSeg = pSetEdge->NextSeg();
		for (int i = 0; i < pAdjustSeg->mCabinets.Num(); ++i)
		{
			if (pAdjustSeg->mCabinets[i].mAlign == FCabinetSet::eAlignHead && pAdjustSeg->mCabinets[i].mRes.mTeplateId == -1)
			{
				FuckIdx = i;
				break;
			}
		}
	}
	CHECK_ERROR(pAdjustSeg && FuckIdx != -1);

	TArray<FCabinetSet> OldCabinte2 = pAdjustSeg->mCabinets;

	pAdjustSeg->mCabinets[FuckIdx].mRes = FCabinetRes(Cabinet.mRes.GetTurnFill());

	int32 Len = pAdjustSeg->AdjustCabinets();
	if (Len != 0)
	{
		pAdjustSeg->ReSetClearNormal();
		bool b = SetSetEdge(pAdjustSeg);
		if (!b)
		{
			pSetEdge->mCabinets = OldCabinte1;
			pAdjustSeg->mCabinets = OldCabinte2;
			return false;
		}
	}
	//AfterExecute();
	return true;
}

bool FAutoSetBase::ChangeTurn(TSetEdge * pSetEdge)
{
	int32 Len = pSetEdge->AdjustCabinets();
	if (Len != 0)
		pSetEdge->ReSetClearNormal();
	bool b = SetSetEdge(pSetEdge);
	//AfterExecute();
	return b;
}

bool FAutoSetBase::replaceLayoutTemplate(int macterId, int mcategoryId)
{
	for (auto pset : mSegments)
	{
		TSetEdge* pSetEdge = (TSetEdge*)pset;
		TArray<FCabinetSet> Cabinets = pSetEdge->mCabinets;
		for ( int i=0; i<pSetEdge->mCabinets.Num(); ++i )
		{
			if (pSetEdge->mCabinets[i].mId == macterId)
			{
				if (mReplaceReseat)
				{
					OnRePlaceCabinet(pSetEdge, i, mcategoryId);
					pSetEdge->ReSetClear();
					if (mcategoryId==200091 || mcategoryId == 200095 || mcategoryId == 200062 || mcategoryId == 200059 || mcategoryId == 200130 || mcategoryId == 200131)
					{
						mCabinetHangType = mcategoryId;
					}
					else
					{
						mCabinetType = mcategoryId;
					}
					bool b = SetSetEdge(pSetEdge);
					if (!b)
						pSetEdge->mCabinets = Cabinets;
					SuitableSegCabinets();
					return b;
				}
				else
				{
					bool b= ReLayoutCabinet(pSetEdge, i, mcategoryId);
					SuitableSegCabinets();
					return b;
				}
			}
		}
	}
	return false;
}

bool FAutoSetBase::CompleteLayout()
{
	mArea->ClearCabinets(mOverlayCabinets, mCookOverlay);

	NenSetEdgeCabinets();
	mArea->mCabinetGroup[mSetType].FindSelectWall(mSelectWalls);
	mArea->mCabinetGroup[mSetType].GenExtern();
	return true;
}

bool FAutoSetBase::CheckOverlay()
{
	mOverlayCabinets.Empty();
	mFailData.mFailDatas.Empty();
	mCookOverlay = false;
	if (mSetType == eAutoSetLaminate || mSetType == eAutoSetHangBedCabinet)
		return true;
	//
	if (eAutoSetHang == mSetType)
		return true;

	if (eAutoSetWardorbeHang == mSetType)
		return true;

	if (eAutoSetCoatroomHang == mSetType)
		return true;

	if (eAutoSetToiletHang == mSetType)
		return true;

	if (mSetType == eAutoSetFloor )
	{
		if (mArea->mCabinetGroup[eAutoSetFloor].mCabinets.Num())
		{
			mCookOverlay = true;
			mFailData.msg = TEXT("所添加区域与橱柜产生重合，确定清空已有橱柜进行重新布局吗？");
		}
	}

	for (int i = 0; i < eAutoSetNum; ++i)
	{
		if (mArea->mCabinetGroup[i].mCabinets.Num()==0 )
			continue;

		if ((mSetType == eAutoSetFloor || i == eAutoSetHang) && (i == eAutoSetFloor || mSetType == eAutoSetHang))
		{
			continue;
		}
		if (i == eAutoSetLaminate || i == eAutoSetHangBedCabinet)
		{
			continue;
		}

		if (mSetType == eAutoSetBayWindowCabinet && i == eAutoSetTakeInCabinet)
		{
			continue;
		}
		if (mSetType == eAutoSetTakeInCabinet  && i == eAutoSetBayWindowCabinet)
		{
			continue;
		}
		if ( i == eAutoSetBayWindowCabinet && mSetType != eAutoSetBayWindowCabinet)
		{
			continue;
		}
		if (i == eAutoSetTakeInCabinet && mSetType != eAutoSetTakeInCabinet)
		{
			continue;
		}

		for (auto line2d : mSelectWalls)
		{
			bool bCabient = false;
			for (auto cabient: mArea->mCabinetGroup[i].mCabinets )
			{
				bool bOverLay = cabient.Value->Collision(line2d, 200);
				if (bOverLay)
				{
					mOverlayCabinets.Add(cabient.Key);
					bCabient = true;
				}
			}
			if ( bCabient )
			{
				FCabinetWalldata data;
				if(i == eAutoSetFloor || i == eAutoSetHang)
				{
					mCookOverlay = true;
					mFailData.msg = TEXT("所添加区域与橱柜产生重合，确定清空已有橱柜进行重新布局吗？");

					data.success = 1;
					data.msg = TEXT("所添加区域与橱柜产生重合，确定清空已有橱柜进行重新布局吗？");
				}
				else
				{
					data.success = 2;
					data.msg = TEXT("所添加区域已经存在定制柜，确定清空进行布局吗?");
				}
				data.wallline.Id = line2d.Id;
				data.wallline.mStart = line2d.mStart;
				data.wallline.mEnd = line2d.mEnd;
				mFailData.mFailDatas.Add(data);
			}
		}
	}
	return mOverlayCabinets.Num() || mCookOverlay;
}
bool FAutoSetBase::FailSegSet(TSetEdge* pSetEdge)
{
	FCabinetWalldata data;
	data.success = 3;
	data.msg = TEXT("所添加区域无法放下定制柜，请调整区域后重试");
	data.wallline.mStart = pSetEdge->mStart->GetVector();
	data.wallline.mEnd = pSetEdge->mEnd->GetVector();
	data.wallline.Id = pSetEdge->mStart->mNextEdge->mId;
	mFailData.mFailDatas.Add(data);
	return true;
}
bool FAutoSetBase::AddCabinets(TMap<int32, FCabinetShell*>& Rects)
{
	for (auto p : Rects)
	{
		AddPoly(p.Value, eSliptWallCabinet);
	}
	return true;
}
bool FAutoSetBase::ExecuteBase()
{
	mSegId = -1;
	AddPoly(mArea->mOutPaths, eSliptWall, true);

	AddPoly(mSelectWalls, eSliptWallSelect);

	AddPoly(mArea->mHoles, 0, 500);
	AddPoly(mArea->mPillars, eSliptWallPillar);

	CHECK_ERROR(PreExecute());

	SignFrame();

	mSetEdgeHead = (TSetEdge*)GetSegments(mBit, mExpactBit);
	CHECK_ERROR(mSetEdgeHead);
	GetSegs(mSelectSegs, mBit, mExpactBit);

	if ( !(mSetType == eAutoSetFloor|| mSetType == eAutoSetHang || mSetType== eAutoSetCoatroom || mSetType == eAutoSetCoatroomHang) )
	{
		for (auto p : mSelectSegs)
		{
			TSetEdge* pSetEdge = p;
			if (pSetEdge->mEnd->IsTurn90() && pSetEdge->mNextSeg->IsState(eSliptWallSelect) )
			{
				FCabinetWalldata data;
				data.success = 4;
				data.msg = TEXT("L型区域放置柜体将导致重合，请调整区域后重试");
				data.wallline.mStart = pSetEdge->mStart->GetVector();
				data.wallline.mEnd = pSetEdge->mEnd->GetVector();
				mFailData.mFailDatas.Add(data);
			}
		}
	}
	CHECK_ERROR(PostExecute());
	return true;
}
bool FAutoSetBase::ExecuteSelete()
{
	for (auto p: mSelectSegs)
	{
		p->mSetRet = 0;
		if (!SetSetEdge(p))
		{
			if (p->mLen >= 300)
			{
				p->mSetRet = 3;
				p->mErrorMsg = TEXT("所添加区域无法放下定制柜，请调整区域后重试");
				if( mErrorCodeId == 0)
					mErrorCodeId = 1;
			}
		}
	}
	if (mSetType == eAutoSetHang)
	{
		TArray<int>temp1, temp2;
		int len = mSelectSegs.Num();
		for (int i = 0; i < len; ++i)
		{
			int len1 = mSelectSegs[i]->mCabinets.Num();
			for (int j = 0; j < len; ++j)
			{
				if (i == j)
					continue;

				for (int c = 0; c < len1; ++c)
				{
					int m = mSelectSegs[j]->mCabinets.Num();

					for (int g = 0; g < m; ++g)
					{
										
						if (UHouseFunctionLibrary::IsRect(mSelectSegs[i]->mCabinets[c].mPos, (mSelectSegs[i]->mCabinets[c].mRes.mSize * mSelectSegs[i]->mCabinets[c].mScale),
							mSelectSegs[j]->mCabinets[g].mPos) && !mSelectSegs[j]->mCabinets[g].mRes.categoryName.IsEmpty() && !mSelectSegs[i]->mCabinets[c].mRes.categoryName.IsEmpty())
						{
							if (mSelectSegs[i]->mCabinets.Num() == 1)
							{
								temp1.Add(i);
								temp2.Add(c);
							}
							else
							{
								temp1.Add(j);
								temp2.Add(g);
							}
						}
					}
				}
			}
		}

		for (int i = 0; i < temp1.Num(); ++i)
		{
			if(mSelectSegs[temp1[i]]->mCabinets.Num() >= temp2[i] + 1)
			 mSelectSegs[temp1[i]]->mCabinets.RemoveAt(temp2[i]);
		}
	}

	SuitableSegCabinets();
	AfterExecute();
	return true;
}
TSegment* FAutoSetBase::NewSegment(PolyEdgeLib::TPoint * pStart, PolyEdgeLib::TPoint * pEnd)
{
	CHECK_ERROR(pStart != pEnd);
	TSegment* pSegment = new TSetEdge;
	pSegment->mId = ++mSegId;
	pSegment->Init(pStart, pEnd);
	mSegments.Add(pSegment);
	return pSegment;
}
bool FAutoSetBase::InitRes(FCabinetRes res)
{
	mInstances.Add(res.mInstanceId, res);
	auto p = mRess.Find(res.mTeplateId);
	if (p)
	{
		p->mSizeMin.X = TinyMin(p->mSizeMin.X, res.mSizeMin.X);
		p->mSizeMax.X = TinyMax(p->mSizeMax.X, res.mSizeMax.X);
		return true;
	}
	else
	{
		mRess.Add(res.mTeplateId, res);
	}
	return true;
}
bool FAutoSetBase::GetSuitable(FCabinetSet & res)
{
	float XSize = res.mScale.X* res.mRes.mSize.X;
	for (auto p : mInstances)
	{
		if (res.mRes.mTeplateId == p.Value.mTeplateId /*&& XSize >= p.Value.mSizeMin.X && XSize<=p.Value.mSizeMax.X*/)
		{
			res.mScale.X = XSize / p.Value.mSize.X;
			res.mRes = p.Value;
			return true;
		}
	}
	return false;
}
FCabinetRes FAutoSetBase::GetRes(int id)
{
	auto p = mRess.Find(id);
	if (p)
	{
		return *p;
	}
	return FCabinetRes();
}
bool FAutoSetBase::NenSetEdgeCabinets()
{
	mArea->mCabinetGroup[mSetType].mSplinesMesh[eSplineTableBack] = gCabinetGlobal.GetTableSku(2);
	mArea->mCabinetGroup[mSetType].mSplinesMesh[eSplineFront] = gCabinetGlobal.GetTableSku(1);
	for ( auto pset: mSegments)
	{
		TSetEdge* pSetEdge = (TSetEdge*)pset;
		NenSetEdgeCabinets(pSetEdge);
	}
	return true;
}
bool FAutoSetBase::NenSetEdgeCabinets(TSetEdge * pSetEdge)
{
	for (auto& p : pSetEdge->mCabinets)
	{
		if (p.mRes.mTeplateId != -1)
		{
			p.mSetType = mSetType;
			mArea->NewCabinet(p);
		}
	}
	return true;
}

bool FAutoSetBase::SuitableSegCabinets()
{
	for (auto pset : mSegments)
	{
		TSetEdge* pSetEdge = (TSetEdge*)pset;
		for (auto& p : pSetEdge->mCabinets)
		{
			p.mSetType = mSetType;
			p.mHeight = TinyMax(mHeight, p.mRes.mHeight);
			if ( p.mRes.mTurnBSize && p.mAlign== FCabinetSet::eAlignTail )
			{
				p.mDir =FVector2D(-pSetEdge->mDir.Y,pSetEdge->mDir.X);
			}
			if (p.mRes.mTeplateId != -1)
			{
				GetSuitable(p);
			}
			if (mSetType != eAutoSetFloor)
			{
				p.mRes.InitSinkSku(p.mScale);
			}
		}
	}
	return true;
}

FCabinetRes FAutoSetBase::GetOptionCabinet(TSetEdge * pSetEdge, int Res0, int Res1, int Res2, int Res3)
{
	FCabinetRes res1 = GetRes(Res0);
	FCabinetRes res2 = GetRes(Res1);
	FCabinetRes res3 = GetRes(Res2);
	FCabinetRes res4 = GetRes(Res3);
	if (Res3 && pSetEdge->GetRemainLen() >= res4.mSizeMin.X && pSetEdge->GetRemainLen() <= res4.mSizeMax.X )
	{
		return res4;
	}
	else if (Res2 && pSetEdge->GetRemainLen() >= res3.mSizeMin.X && pSetEdge->GetRemainLen() <= res3.mSizeMax.X)
	{
		return res3;
	}
	else if (Res1 && pSetEdge->GetRemainLen() >= res2.mSizeMin.X&& pSetEdge->GetRemainLen() <= res2.mSizeMax.X)
	{
		return res2;
	}
	else if (Res0 && pSetEdge->GetRemainLen() >= res1.mSizeMin.X&& pSetEdge->GetRemainLen() <= res1.mSizeMax.X)
	{
		return res1;
	}

	if (Res3 && pSetEdge->GetRemainLen() >= res4.mSizeMin.X  )
	{
		return res4;
	}
	else if (Res2 && pSetEdge->GetRemainLen() >= res3.mSizeMin.X  )
	{
		return res3;
	}
	else if (Res1 && pSetEdge->GetRemainLen() >= res2.mSizeMin.X )
	{
		return res2;
	}
	else if (Res0 && pSetEdge->GetRemainLen() >= res1.mSizeMin.X )
	{
		return res1;
	}

	return FCabinetRes();
}

int32 FAutoSetBase::MinLenCabinets(int Id0, int Id1, int Id2, int Id3, int Id4, int Id5, int Id6, int Id7, int Id8)
{
	int32 nMinLen = GetRes(Id0).mSizeMin.X;
	if (Id1)
		nMinLen += GetRes(Id1).mSizeMin.X;
	if (Id2)
		nMinLen += GetRes(Id2).mSizeMin.X;
	if (Id3)
		nMinLen += GetRes(Id3).mSizeMin.X;
	if (Id4)
		nMinLen += GetRes(Id4).mSizeMin.X;
	if (Id5)
		nMinLen += GetRes(Id5).mSizeMin.X;
	if (Id6)
		nMinLen += GetRes(Id6).mSizeMin.X;
	if (Id7)
		nMinLen += GetRes(Id7).mSizeMin.X;
	if (Id8)
		nMinLen += GetRes(Id8).mSizeMin.X;
	return nMinLen;
}
int32 FAutoSetBase::MaxLenCabinets(int Id0, int Id1, int Id2, int Id3, int Id4, int Id5, int Id6, int Id7, int Id8)
{
	int32 nMaxLen = GetRes(Id0).mSizeMax.X;
	if (Id1)
		nMaxLen += GetRes(Id1).mSizeMax.X;
	if (Id2)
		nMaxLen += GetRes(Id2).mSizeMax.X;
	if (Id3)
		nMaxLen += GetRes(Id3).mSizeMax.X;
	if (Id4)
		nMaxLen += GetRes(Id4).mSizeMax.X;
	if (Id5)
		nMaxLen += GetRes(Id5).mSizeMax.X;
	if (Id6)
		nMaxLen += GetRes(Id6).mSizeMax.X;
	if (Id7)
		nMaxLen += GetRes(Id7).mSizeMax.X;
	if (Id8)
		nMaxLen += GetRes(Id8).mSizeMax.X;
	return nMaxLen ;
}
bool FAutoSetBase::MinCabinets(TSetEdge * pSetEdge, int Id0, int Id1, int Id2, int Id3, int Id4, int Id5, int Id6, int Id7, int Id8)
{
	return MinLenCabinets(Id0, Id1, Id2, Id3, Id4, Id5, Id6, Id7, Id8) <=pSetEdge->GetRemainLen();
}
bool FAutoSetBase::MaxCabinets(TSetEdge * pSetEdge, int Id0, int Id1, int Id2, int Id3, int Id4, int Id5, int Id6, int Id7, int Id8)
{
	return MaxLenCabinets(Id0, Id1, Id2, Id3, Id4, Id5, Id6, Id7, Id8) >= pSetEdge->GetRemainLen();
}
bool FAutoSetBase::CanFillCabinets(TSetEdge * pSetEdge, int Id0, int Id1, int Id2, int Id3, int Id4,int Id5, int Id6, int Id7, int Id8)
{
	CHECK_ERROR(MinCabinets(pSetEdge, Id0, Id1, Id2, Id3, Id4, Id5, Id6, Id7, Id8));
	CHECK_ERROR(MaxCabinets(pSetEdge, Id0, Id1, Id2, Id3, Id4, Id5, Id6, Id7, Id8));
	return true;
}
bool FAutoSetBase::FillCabinets(TSetEdge * pSetEdge, FCabinetSet::eAlignType eAlign, int Id0, int Id1, int Id2, int Id3, int Id4, int Id5, int Id6, int Id7, int Id8)
{
	CHECK_ERROR(CanFillCabinets(pSetEdge, Id0, Id1, Id2, Id3, Id4, Id5, Id6, Id7, Id8));
	if (Id0)
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillDefault, GetRes(Id0));
	if (Id1)
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillDefault, GetRes(Id1));
	if (Id2)
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillDefault, GetRes(Id2));
	if (Id3)
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillDefault, GetRes(Id3));
	if (Id4)
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillDefault, GetRes(Id4));
	if (Id5)
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillDefault, GetRes(Id5));
	if (Id6)
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillDefault, GetRes(Id6));
	if (Id7)
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillDefault, GetRes(Id7));
	if (Id8)
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillDefault, GetRes(Id8));
	pSetEdge->AdjustCabinets();
	return true;
}
int FAutoSetBase::OptionSetCabinets(TSetEdge * pSetEdge, FCabinetSet::eAlignType eAlign, int nRes2, int nRes1)
{
	FCabinetRes res2 = GetRes(nRes2);
	FCabinetRes res1 = GetRes(nRes1);
	int NumCabinet = pSetEdge->GetRemainLen() / res2.mSize.X;
	for (int i = 0; i < NumCabinet; ++i)
	{
		pSetEdge->SetCabinetPos(eAlign,FCabinetSet::eFillDefault,res2);
	}
	if (pSetEdge->GetRemainLen() >= res2.mSizeMin.X && pSetEdge->GetRemainLen() <= res2.mSizeMax.X)
	{
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillFull, res2);
		return NumCabinet + 1;
	}
	else if (pSetEdge->GetRemainLen() >= res1.mSizeMin.X && pSetEdge->GetRemainLen() <= res1.mSizeMax.X)
	{
		pSetEdge->SetCabinetPos(eAlign, FCabinetSet::eFillFull, res1);
		return NumCabinet + 1;
	}
	pSetEdge->AdjustRemainSize();
	return NumCabinet;
}
TArray<FCabinetRes> FAutoSetBase::GetCategorys(int categoryId)
{
	TArray<FCabinetRes> Ress;
	for ( auto p: mInstances)
	{
		if ( categoryId == p.Value.mTeplateId )
		{
			Ress.Add(p.Value);
		}
	}
	return Ress;
}
bool FAutoSetBase::GetSegs(TArray<TSetEdge *>& Segs, uint32 nBit, uint32 nExpactBit)
{
	Segs.Empty();
	for (TSetEdge* pSetEdge = mSetEdgeHead; true; )
	{
		if (pSetEdge->IsState(nBit, nExpactBit))
		{
			Segs.Add(pSetEdge);
		}
		pSetEdge = (TSetEdge*)pSetEdge->mNextSeg;
		if (pSetEdge == mSetEdgeHead)
			break;
	}
	return Segs.Num() > 0;
}



bool FCabinetSet2Modle(FCabinetModle& modle, FCabinetSet& cabinetSet)
{
	CHECK_ERROR(cabinetSet.mRes.mTeplateId != -1);
	modle.actorId = cabinetSet.mId;
	modle.templateId = cabinetSet.mRes.mInstanceId;
	modle.categoryId = cabinetSet.mRes.mTeplateId;
	modle.categoryName = cabinetSet.mRes.categoryName;
	modle.modelSize = FVector(cabinetSet.mRes.mSize.X, cabinetSet.mRes.mSize.Y, cabinetSet.mRes.mCabinetHeight);
	modle.point = FVector(cabinetSet.mPos.X, cabinetSet.mPos.Y, cabinetSet.mHeight);
	modle.rotate = FVector(0, 0, atan2(cabinetSet.mDir.Y, cabinetSet.mDir.X) * 180 / PI);
	modle.scale = FVector(cabinetSet.mScale.X, cabinetSet.mScale.Y, cabinetSet.mScaleZ);
	modle.turnBSize = cabinetSet.mRes.mTurnBSize;
	return true;
}

bool FAutoSetBase::GetLayoutResult(FCabinetLayoutResult& LayoutData)
{
	for (auto pset : mSegments)
	{
		TSetEdge* pSetEdge = (TSetEdge*)pset;
		if (pSetEdge->mCabinets.Num() || pSetEdge->mSetRet!=0 )
		{
			FCabinetWalldata data;
			data.mCabinets.Empty();
			data.success = pSetEdge->mSetRet;
			data.msg = pSetEdge->mErrorMsg;
			data.id = pSetEdge->mStart->mNextEdge->mId;
			data.wallline.mStart = pSetEdge->mStart->GetVector();
			data.wallline.mEnd = pSetEdge->mEnd->GetVector();
			for (auto& p : pSetEdge->mCabinets)
			{
				p.mSetType = mSetType;
				p.mHeight = TinyMax(mHeight, p.mRes.mHeight); 
				FCabinetModle modle;
				if (FCabinetSet2Modle(modle, p))
					data.mCabinets.Add(modle);
			}
			LayoutData.mFailDatas.Add(data);
		}
	}
	return true;
}

bool FAutoSetBase::GetCabinets(TArray<int32>& Cabinets, TArray<int32>& Skus)
{
	for (auto pset : mSegments)
	{
		TSetEdge* pSetEdge = (TSetEdge*)pset;
		for (auto p : pSetEdge->mCabinets)
		{
			Cabinets.Add(p.mRes.mInstanceId);
			for (auto sku : p.mRes.mSkuDatas)
			{
				Skus.Add(sku.mSkuId);
			}
		}
	}
	return true;
}

int32 FAutoSetBase::IsLayoutError()
{
	if (mCookOverlay)
		return 7;
	if (mOverlayCabinets.Num() || mFailData.mFailDatas.Num() )
		return 1;
	return mErrorCodeId;
}
