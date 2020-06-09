
#include "AutoSetCookFloor.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
#include "../CabinetGlobal.h"

using namespace PolyEdgeLib;
namespace AutoSetCookFloor
{
	FAutoSetCabinet::FAutoSetCabinet()
	{
		mExpactBit = eSliptWallDoor;
		mReplaceReseat = false;
	}
	FAutoSetCabinet::~FAutoSetCabinet()
	{
	}

	bool FAutoSetCabinet::PreExecute()
	{
		mSingleDoor = GetRes(200030);
		mDoubleDoor = GetRes(200031);
		mSink = GetRes(200032);
		mChimey = GetRes(200033);
		mDrawer = GetRes(200034);
		mCornerLine = GetRes(200035);
		mCornerTurn = GetRes(200036);

		AddPoly(mArea->GetChimery(), eSliptWallChimney);

		for (auto p : mArea->mPillars)
		{
			uint32 nBit = eSliptWallChimney;
			if (p.mUserType == 1)
				nBit = eSliptWallPillar;
			else if (p.mUserType == 2)
				nBit = eSliptWallNull;
			AddPoly(p.mPos, p.mSize*p.mScale, p.mDir, nBit);
		}
		return true;
	}
	bool FAutoSetCabinet::PostExecute()
	{
		for ( auto p:mSelectSegs )
		{
			TSetEdge* pSetSeg = p;
			TSetEdge* pPillarPre = pSetSeg->PreSeg();
			if (pPillarPre->IsState(eSliptWallPillar|eSliptWallChimney) && pPillarPre->mLen < 550 && pPillarPre->mStart->IsTurn90() )
			{
				pSetSeg->MergePre();
			}
			TSetEdge* pPillarNext = pSetSeg->NextSeg();
			if (pPillarNext->IsState(eSliptWallPillar|eSliptWallChimney) && pPillarNext->mLen < 550 && pPillarNext->mEnd->IsTurn90())
			{
				pSetSeg->MergeNext();
			}
		}
		SignChimery();
		SignSink();
		SignCorner();
		return true;
	}
	bool FAutoSetCabinet::OnReplace()
	{
		TSetEdge* pChimeySeg = nullptr;
		TSetEdge* pSinkSeg = nullptr;

		for (auto pset : mSelectSegs)
		{
			TSetEdge* pSetSeg = pset;
			for (auto p : pSetSeg->mCabinets)
			{
				if (p.mRes.mTeplateId == 200033  )
				{
					pChimeySeg = pSetSeg;
				}
				if (p.mRes.mTeplateId == 200032  )
				{
					pSinkSeg = pSetSeg;
				}
			}
		}
		ResteSku(pSinkSeg, pChimeySeg);
		return true;
	}
	bool FAutoSetCabinet::ResteSku(TSetEdge* pSinkSeg, TSetEdge* pChimeySeg)
	{
		CHECK_ERROR(pSinkSeg);
		CHECK_ERROR(pChimeySeg);
		for (auto p : pChimeySeg->mCabinets)
		{
			if (p.mRes.mTeplateId == 200033 || p.mRes.mTeplateId == 200031 || p.mRes.mTeplateId == 200034)
			{
				int32 InstanceId = p.mRes.mSkuDatas.Num() ? p.mRes.mSkuDatas[0].mSkuId : -1;
				TSkuData* pData = gCabinetGlobal.GetSku(p.GetSize(), 722, InstanceId);
				FCabinetSkuModel k;
				k.point = p.mPos;
				k.rotate = atan2(p.mDir.Y, p.mDir.X) * 180 / PI;
				k.type = 0;
				k.size = FVector2D(TinyMax(pData->mSize.X, 200), TinyMax(pData->mSize.Y, 200));
				k.skuid = pData->mSkuId;
				k.wallline.mStart = pChimeySeg->mStart->GetVector() + pChimeySeg->mNor* p.mRes.mSize.Y;
				k.wallline.mEnd = pChimeySeg->mEnd->GetVector() + pChimeySeg->mNor* p.mRes.mSize.Y;
				SkuData[1] = k;
				//
				mSkus[1].mPos = p.mPos;
				mSkus[1].mDir = p.mDir;
				mSkus[1].mSku = *pData;
				mSkus[1].mLeftCabinetId = mSkus[1].mRightCabinetId = p.mId;
				if (p.mRes.mTeplateId == 200033 || p.mRes.mTeplateId == 200034)
					break;
			}
		}
		for (auto p : pSinkSeg->mCabinets)
		{
			if (p.mRes.mTeplateId == 200032 || p.mRes.mTeplateId == 200031)
			{
				int32 InstanceId = p.mRes.mSkuDatas.Num() ? p.mRes.mSkuDatas[0].mSkuId : -1;
				TSkuData* pData = gCabinetGlobal.GetSku(p.GetSize(), 721, InstanceId);

				FCabinetSkuModel k;
				k.point = p.mPos;
				k.rotate = atan2(p.mDir.Y, p.mDir.X) * 180 / PI;
				k.type = 1;
				k.size = FVector2D(TinyMax(pData->mSize.X, 200), TinyMax(pData->mSize.Y, 200));
				k.skuid = pData->mSkuId;
				k.wallline.mStart = pSinkSeg->mStart->GetVector() + pSinkSeg->mNor* p.mRes.mSize.Y;
				k.wallline.mEnd = pSinkSeg->mEnd->GetVector() + pSinkSeg->mNor* p.mRes.mSize.Y;
				SkuData[0] = k;
				//
				mSkus[0].mPos = p.mPos;
				mSkus[0].mDir = p.mDir;
				mSkus[0].mSku = *pData;
				mSkus[0].mLeftCabinetId = mSkus[1].mRightCabinetId = p.mId;
				if (p.mRes.mTeplateId == 200032)
					break;
			}
		}
		return true;
	}
	bool FAutoSetCabinet::AfterExecute()
	{
		return ResteSku(mSinkSeg, mChimeySeg);
	}
	bool FAutoSetCabinet::GetLayoutResult(FCabinetLayoutResult& LayoutData)
	{
		FAutoSetBase::GetLayoutResult(LayoutData);
		LayoutData.mCookExtern.Add(SkuData[0]);
		LayoutData.mCookExtern.Add(SkuData[1]);
		return true;
	}
	bool FAutoSetCabinet::CompleteLayout()
	{
		mArea->mSkus[0] = mSkus[0];
		mArea->mSkus[0].LoadSku();
		mArea->mSkus[1] = mSkus[1];
		FAutoSetBase::CompleteLayout();
		return true;
	}
	bool FAutoSetCabinet::ModifySkuPos(int type, FVector2D NewPos)
	{
		CHECK_ERROR(mSinkSeg);
		CHECK_ERROR(mChimeySeg);
		TSetEdge* pSetSeg = type == 1 ? mChimeySeg : mSinkSeg;
		bool bIs = false;
		for (auto i : pSetSeg->mCabinets)
		{
			if (type == 1 && i.mRes.mTeplateId == 200033)
			{
				bIs = true;
				break;
			}
			else if(type == 0 && i.mRes.mTeplateId == 200032)
			{
				bIs = true;
				break;
			}
		}
		if (!bIs)
		{
			auto temp = pSetSeg == mChimeySeg ? mSinkSeg : mChimeySeg;

			for (auto i : temp->mCabinets)
			{
				if (type == 1 && i.mRes.mTeplateId == 200033)
				{
					bIs = true;
					break;
				}
				else if (type == 0 && i.mRes.mTeplateId == 200032)
				{
					bIs = true;
					break;
				}
			}
			if (bIs)
				pSetSeg = type == 1 ? mSinkSeg : mChimeySeg;
		}


		float fDot;
		FVector2D CrossPos;
		FVector2D posStart = pSetSeg->mStart->GetVector();
		FVector2D posEnd = pSetSeg->mEnd->GetVector();
		Point2LineSegVer(posStart, posEnd, NewPos,fDot,CrossPos);
		float poslocal = (CrossPos - posStart).Size();

		TArray<FCabinetSet> Cabinets = pSetSeg->mCabinets;
		Cabinets.Sort([](const FCabinetSet& A, const FCabinetSet& B) {return A.mPosLocal < B.mPosLocal;});
		int Num = Cabinets.Num();
		for ( int i=0; i< Num;++i )
		{
			FCabinetSet& CabintSet = Cabinets[i];
			int32 nCabinetPos = poslocal - CabintSet.mPosLocal;
			int32 nHalfSize = CabintSet.mScale.X*CabintSet.mRes.mSize.X*0.5f;
			int32 nDepth = IsTurnCabinet(CabintSet.mRes.mTeplateId) ? CabintSet.mRes.mTurnBSize : CabintSet.mRes.mSize.Y;
			if (FMath::Abs(nCabinetPos) <= nHalfSize)
			{
				FCabinetSet* pNeiborLeft = nullptr;
				int32 IdxPre = pSetSeg->PreSeg()->GetCabinet(FCabinetSet::eAlignTail);
				if (IdxPre != -1)
				{
					pNeiborLeft = &pSetSeg->PreSeg()->mCabinets[IdxPre];
				}

				FCabinetSet* pNeiborRight = nullptr;
				int32 IdxRight = pSetSeg->NextSeg()->GetCabinet(FCabinetSet::eAlignHead);
				if (IdxRight != -1)
				{
					pNeiborRight = &pSetSeg->NextSeg()->mCabinets[IdxRight];
				}

				if (i == 0)
				{
					if (CabintSet.mRes.mTeplateId < 0 )
					{
						if (pNeiborLeft)
						{
							SkuData[type].point = posStart + Cabinets[1].mPosLocal*pSetSeg->mDir + pSetSeg->mNor* nDepth*0.5f;
							mSkus[type].mPos = SkuData[type].point;
							mSkus[type].mLeftCabinetId = mSkus[1].mRightCabinetId = Cabinets[1].mId;
							return true;
						}
						if (pNeiborLeft->mRes.mTeplateId == 200035)
						{
							SkuData[type].point = posStart + Cabinets[1].mPosLocal*pSetSeg->mDir + pSetSeg->mNor* nDepth*0.5f;
							mSkus[type].mPos = SkuData[type].point;
							mSkus[type].mLeftCabinetId = mSkus[1].mRightCabinetId = Cabinets[1].mId;
							return true;
						}
					}
				}
				if (i == Num - 1)
				{
					if (CabintSet.mRes.mTeplateId < 0)
					{
						int32 Idx = pSetSeg->NextSeg()->GetCabinet(FCabinetSet::eAlignHead);
						if (Idx == -1)
						{
							SkuData[type].point = posStart + Cabinets[Num-2].mPosLocal*pSetSeg->mDir + pSetSeg->mNor* nDepth*0.5f;	
							mSkus[type].mPos = SkuData[type].point;
							mSkus[type].mLeftCabinetId = mSkus[1].mRightCabinetId = Cabinets[Num - 2].mId;
							return true;
						}
						if (pNeiborRight->mRes.mTeplateId == 200035)
						{
							SkuData[type].point = posStart + Cabinets[Num - 2].mPosLocal*pSetSeg->mDir + pSetSeg->mNor* nDepth*0.5f;
							mSkus[type].mPos = SkuData[type].point;
							mSkus[type].mLeftCabinetId = mSkus[1].mRightCabinetId = Cabinets[Num - 2].mId;
							return true;
						}
					}
				}
				if (nCabinetPos >= nHalfSize * 0.5f && i <= Num - 2)
				{
					SkuData[type].point = posStart + (nHalfSize + CabintSet.mPosLocal)*pSetSeg->mDir + pSetSeg->mNor* nDepth*0.5f;
					mSkus[type].mPos = SkuData[type].point;
					mSkus[type].mLeftCabinetId  = (CabintSet.mRes.mTeplateId > 0) ? CabintSet.mId: pNeiborLeft->mId;
					mSkus[type].mRightCabinetId = (Cabinets[i+1].mRes.mTeplateId > 0) ? Cabinets[i+1].mId: pNeiborRight->mId;
				}
				else if (nCabinetPos <= -nHalfSize * 0.5f && i >=1 )
				{
					SkuData[type].point  = posStart + (-nHalfSize + CabintSet.mPosLocal)*pSetSeg->mDir + pSetSeg->mNor* nDepth*0.5f;
					mSkus[type].mPos = SkuData[type].point;

					mSkus[type].mRightCabinetId = (CabintSet.mRes.mTeplateId > 0) ? CabintSet.mId : pNeiborRight->mId;
					mSkus[type].mLeftCabinetId = (Cabinets[i - 1].mRes.mTeplateId > 0) ? Cabinets[i-1].mId : pNeiborLeft->mId;
				}
				else
				{
					SkuData[type].point = posStart + CabintSet.mPosLocal*pSetSeg->mDir + pSetSeg->mNor* nDepth*0.5f;
					mSkus[type].mPos = SkuData[type].point;
					if (CabintSet.mRes.mTeplateId == -1)
					{
						if (i == 0)
						{
							mSkus[type].mLeftCabinetId = mSkus[1].mRightCabinetId = pNeiborLeft->mId;
						}
						else
						{
							mSkus[type].mLeftCabinetId = mSkus[1].mRightCabinetId = pNeiborRight->mId;
						}
					}
					else
					{
						mSkus[type].mLeftCabinetId = mSkus[1].mRightCabinetId = CabintSet.mId;
					}
				}
				return true;
			}
		}
		return true;
	}
	bool FAutoSetCabinet::FillIsolateCorner(TSetEdge * pSetEdge)
	{
		if ( !pSetEdge->PreSeg()->IsState(eSliptWallSelect) && (pSetEdge->HeadEdge()->IsState(eSliptWallPillar)|| pSetEdge->HeadEdge()->IsState(eSliptWallCabinet)) )
		{
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead,FCabinetSet::eFillDefault, mDoubleDoor);
		}
		if ( !pSetEdge->NextSeg()->IsState(eSliptWallSelect) && (pSetEdge->TailEdge()->IsState(eSliptWallPillar) || pSetEdge->TailEdge()->IsState(eSliptWallCabinet)))
		{
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignTail, FCabinetSet::eFillDefault, mDoubleDoor);
		}
		return true;
	}
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		if (mChimeySeg == mSinkSeg && pSetEdge == mChimeySeg)
		{
			CHECK_ERROR(SetChimerySinkEdge(pSetEdge));
			SwapSinkPos2Window(pSetEdge);
			return true;
		}
		else if (mChimeySeg == pSetEdge)
		{
			CHECK_ERROR(SetChimeryEdge(pSetEdge));
			return true;
		}
		else if (mSinkSeg == pSetEdge)
		{
			CHECK_ERROR(SetSinkEdge(pSetEdge));
			SwapSinkPos2Window(pSetEdge);
			return true;
		}
		else
		{
			CHECK_ERROR(SetOtherEdge(pSetEdge));
			return true;
		}
		return true;
	}
	bool FAutoSetCabinet::SetChimerySinkEdge(TSetEdge * pSetEdge)
	{
		CHECK_ERROR(MinCabinets(pSetEdge, 200032, 200033));

		FCabinetSet::eAlignType alignType = mChimeyCorner == pSetEdge->mEnd ? FCabinetSet::eAlignTail: FCabinetSet::eAlignHead;
		int32 nHeadId = 0;
		int32 TailId = 0;
		GetEdgePillarSet(pSetEdge, nHeadId, TailId);
		if (alignType == FCabinetSet::eAlignTail)
		{
			TinySwap(nHeadId,TailId);
		}
		if (nHeadId)
		{
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200032, TailId));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200032,TailId));

			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, TailId));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200032, TailId));

			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200031, 200032, TailId));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200032, TailId));
		}
		if (TailId)
		{
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200032, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200032, 200030));

			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200032, 200030));

			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200031, 200032, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200032, 200030));
		}
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200032));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200032));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200032));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200032, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200032, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200032, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200032, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200031, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200031, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200031, 200031, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200031, 200031, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200031, 200031, 200031, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200031, 200031, 200031, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200031, 200031, 200031, 200031, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200031, 200032, 200031, 200031, 200031, 200031, 200030));
		return false;
	}
	bool FAutoSetCabinet::SetChimeryEdge(TSetEdge * pSetEdge )
	{
		FCabinetSet::eAlignType alignType = mChimeyCorner == pSetEdge->mEnd ? FCabinetSet::eAlignTail : FCabinetSet::eAlignHead;
		//
		int32 nHeadId = 0;
		int32 TailId = 0;
		GetEdgePillarSet(pSetEdge, nHeadId, TailId);
		if ((alignType == FCabinetSet::eAlignHead && nHeadId) || (alignType == FCabinetSet::eAlignTail && TailId) )
		{
			CHECK_OK(SetCabinets(pSetEdge, alignType,200031,200033));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200034));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030));//0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031));//2
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200030));//1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200031));//3.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200030));//3.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200031, 200031));//4.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200031, 200030));//4.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200033, 200030, 200031, 200031, 200031, 200031, 200030, 200030));

			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200034));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030));//0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031));//2
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200030));//1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200031));//3.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200030));//3.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200031, 200031));//4.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200031, 200030));//4.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200033, 200030, 200031, 200031, 200031, 200031, 200030, 200030));
		}
		else
		{
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200034));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030));//0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200030));//1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031));//2
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031));//3.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200030));//3.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200031));//4.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200030));//4.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200031, 200031, 200030, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200033, 200030, 200031, 200031, 200031, 200031, 200031, 200031, 200030));
		}
		return false;
	}
	bool FAutoSetCabinet::SetSinkEdge(TSetEdge * pSetEdge )
	{
		FCabinetSet::eAlignType alignType = FCabinetSet::eAlignHead;
		int32 nHeadId = 0;
		int32 TailId = 0;
		GetEdgePillarSet(pSetEdge, nHeadId, TailId);
		if (nHeadId)
		{
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031));//0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200030));//0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031));//1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200030));//2
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031, 200031));//3.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031, 200030));//3.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031, 200031, 200031));//4.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031, 200031, 200030));//4.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200032, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200032, 200031, 200031, 200031, 200031, 200031, 200031, 200030));

			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031));//0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200030));//0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031));//1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200030));//2
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200031));//3.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200030));//3.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200031, 200031));//4.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200031, 200030));//4.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200030, 200032, 200031, 200031, 200031, 200031, 200031, 200031, 200030));
		}
		else
		{
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031));//0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200030));//0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031));//1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200030));//2
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031));//3.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200030));//3.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200031));//4.0
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200030));//4.1
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200031, 200031, 200031, 200030));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200031, 200031, 200031, 200031, 200031));
			CHECK_OK(SetCabinets(pSetEdge, alignType, 200032, 200031, 200031, 200031, 200031, 200031, 200031, 200031, 200030));
		}
		return false;
	}
	bool FAutoSetCabinet::SetOtherEdge(TSetEdge * pSetEdge )
	{
		FCabinetSet::eAlignType alignType = FCabinetSet::eAlignHead;
		if ( !pSetEdge->mPreSeg->IsState(eSliptWallSelect) && !pSetEdge->NextSeg()->IsState(eSliptWallSelect) )
		{
			alignType = FCabinetSet::eAlignTail;
		}
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031));//0
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200030));//0
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031));//1
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200030));//2
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031));//3.0
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200030));//3.1
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200031));//4.0
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200030));//4.1
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200031, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200031, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200031, 200031, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200031, 200031, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200031, 200031, 200031, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200031, 200031, 200031, 200030));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200031, 200031, 200031, 200031, 200031));
		CHECK_OK(SetCabinets(pSetEdge, alignType, 200031, 200031, 200031, 200031, 200031, 200031, 200031, 200031, 200030));
		return false;
	}
	bool FAutoSetCabinet::GetEdgePillarSet(TSetEdge * pSetEdge, int & nHeadId, int & nTailId)
	{
		nHeadId = 0;
		nTailId = 0;
		if (!pSetEdge->PreSeg()->IsState(eSliptWallSelect) && (pSetEdge->HeadEdge()->IsState(eSliptWallPillar) || pSetEdge->HeadEdge()->IsState(eSliptWallCabinet)))
		{
			nHeadId = 200031;
			return true;
		}
		if (!pSetEdge->NextSeg()->IsState(eSliptWallSelect) && (pSetEdge->TailEdge()->IsState(eSliptWallPillar) || pSetEdge->TailEdge()->IsState(eSliptWallCabinet)))
		{
			nTailId = 200031;
			return true;
		}
		return false;
	}
	bool FAutoSetCabinet::SignCorner()
	{
		mCornerType = 200036;
		for (auto p : mSelectSegs)
		{
			//FillIsolateCorner(p);
			SignCorner(p);
		}

		TArray<TSetEdge> templist;
		bool bIs = false;
		for (auto p : mSelectSegs)
		{
			auto temp = (*p);
			FillCorner(&temp);
			FillCorner270(&temp);
			if (temp.mCabinets.Num() > 0)
			{
				if (temp.GetRemainLen() < temp.mCabinets[0].mRes.mSizeMin.X)
				{
					bIs = true;
					break;
				}
			}
			templist.Add(temp);
		}

		if (!bIs)
		{
			for (int i = 0; i < templist.Num(); ++i)
			{
				(*mSelectSegs[i]) = templist[i];
			}
		}
		else
		{
			for (auto p : mSelectSegs)
			{
				FillCorner(p, false);
				FillCorner270(p);
			}
		}
		return true;

	}
	bool FAutoSetCabinet::SignCorner(TSetEdge * pSetEdge)
	{
		int32 nLen = pSetEdge->GetRemainLen();
		int32 nTurnSize = GetRes(200036).mSize.X;
		if (pSetEdge->PreSeg()->IsState(eSliptWallSelect) && pSetEdge->mStart->IsTurn90())
		{
			nLen -= nTurnSize;
		}
		if (pSetEdge->NextSeg()->IsState(eSliptWallSelect) && pSetEdge->mEnd->IsTurn90())
		{
			nLen -= nTurnSize;
		}
		if(mChimeySeg == pSetEdge )
		{
			if (nLen  < MinLenCabinets(200033, 200030) )
			{
				mCornerType = 200035;
			}
		}
		if (mSinkSeg == pSetEdge)
		{
			if (nLen < MinLenCabinets(200032, 200030))
			{
				mCornerType = 200035;
			}
		}
		if (nLen < MinLenCabinets(200031, 200030))
		{
			mCornerType = 200035;
		}
		return true;
	}
	bool FAutoSetCabinet::FillCorner(TSetEdge * pSetEdge, bool bIs)
	{
		TSetEdge* PreSeg = pSetEdge->PreSeg();
		TSetEdge* NextSeg = pSetEdge->NextSeg();
		if (mCornerType == 200036)
		{
			if (PreSeg->IsState(eSliptWallSelect) && pSetEdge->mStart->IsTurn90())
			{
				pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillDefault, GetRes(mCornerType));
			}
			if (NextSeg->IsState(eSliptWallSelect) && pSetEdge->mEnd->IsTurn90())
			{
				pSetEdge->FillFake(GetRes(mCornerType).GetTurnFill(), FCabinetSet::eAlignTail);
			}
		}
		else
		{
			if (PreSeg->IsState(eSliptWallSelect) && pSetEdge->mStart->IsTurn90())
			{
				if (bIs)
				{
					if (pSetEdge->mLen > PreSeg->mLen)
					{
						pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillDefault, GetRes(mCornerType));
					}
					else
					{
						pSetEdge->FillFake(GetRes(mCornerType).GetTurnFill(), FCabinetSet::eAlignHead);
					}
				}
				else
				{
					if (pSetEdge->mLen < PreSeg->mLen)
					{
						pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillDefault, GetRes(mCornerType));
					}
					else
					{
						pSetEdge->FillFake(GetRes(mCornerType).GetTurnFill(), FCabinetSet::eAlignHead);
					}
				}
			}
			//
			if (NextSeg->IsState(eSliptWallSelect) && pSetEdge->mEnd->IsTurn90())
			{
				if (bIs)
				{
					if (pSetEdge->mLen > NextSeg->mLen)
					{
						pSetEdge->SetCabinetPos(FCabinetSet::eAlignTail, FCabinetSet::eFillDefault, GetRes(mCornerType));
					}
					else
					{
						pSetEdge->FillFake(GetRes(mCornerType).GetTurnFill(), FCabinetSet::eAlignTail);
					}
				}
				else
				{
					if (pSetEdge->mLen < NextSeg->mLen)
					{
						pSetEdge->SetCabinetPos(FCabinetSet::eAlignTail, FCabinetSet::eFillDefault, GetRes(mCornerType));
					}
					else
					{
						pSetEdge->FillFake(GetRes(mCornerType).GetTurnFill(), FCabinetSet::eAlignTail);
					}
				}
			}
		}
		return true;
	}
	bool FAutoSetCabinet::FillCorner270(TSetEdge * pSetEdge)
	{
		if (pSetEdge->NextSeg()->IsState(eSliptWallSelect) && pSetEdge->mEnd->IsTurn270())
		{
			if ( pSetEdge->mLen >= pSetEdge->NextSeg()->mLen )
			{
				pSetEdge->FillFake(-GetRes(200035).mSize.Y, FCabinetSet::eAlignTail);
			}
			else
			{
				pSetEdge->NextSeg()->FillFake(-GetRes(200035).mSize.Y, FCabinetSet::eAlignHead);
			}
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FAutoSetCabinet::SwapSinkPos2Window(TSetEdge* pSetEdge)
	{
		int SinkSlotIdx = -1;
		for (int32 i = 0; i < pSetEdge->mCabinets.Num(); ++i)
		{
			if (200032 == pSetEdge->mCabinets[i].mRes.mTeplateId )
			{
				SinkSlotIdx = i;
				break;
			}
		}
		CHECK_OK(SinkSlotIdx == -1);
		if (mWindowType == 0)
			return true;
		float WindLocalPos = 0;
		if ( mWindowType == 1 )
		{
			TEdge* pEdge = pSetEdge->HeadEdge()->NextFirstEdge(eSliptWallWindow);
			CHECK_OK(pEdge == nullptr);
			WindLocalPos = (pEdge->GetPos() - pSetEdge->mStart->GetVector()) | pEdge->mDir;
		}
		else if ( mWindowType == 3 )
		{
			WindLocalPos = pSetEdge->mLen;
		}
		float SinkPrePos   = pSetEdge->mCabinets[SinkSlotIdx].mPosLocal;
		float Sink2Wind    = FMath::Abs(SinkPrePos-WindLocalPos);

		int32 Idx = -1;
		float MinLen = Sink2Wind;
		for (int32 i = 0; i < pSetEdge->mCabinets.Num(); ++i)
		{
			if (200030 == pSetEdge->mCabinets[i].mRes.mTeplateId || 200031 == pSetEdge->mCabinets[i].mRes.mTeplateId|| 200034 == pSetEdge->mCabinets[i].mRes.mTeplateId )
			{
				float iLen = FMath::Abs(pSetEdge->mCabinets[i].mPosLocal - WindLocalPos);
				if (iLen < MinLen)
				{
					MinLen = iLen;
					Idx = i;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if( Idx != -1 )
		{
			TinySwap(pSetEdge->mCabinets[SinkSlotIdx].mRes, pSetEdge->mCabinets[Idx].mRes);
			TinySwap(pSetEdge->mCabinets[SinkSlotIdx].mScale, pSetEdge->mCabinets[Idx].mScale);
			pSetEdge->SetCabinetsPos();
		}
		return true;
	}
	bool FAutoSetCabinet::SetCabinets(TSetEdge * pSetEdge, FCabinetSet::eAlignType eAlign, int Id0, int Id1, int Id2, int Id3, int Id4, int Id5, int Id6, int Id7, int Id8)
	{
		CHECK_ERROR(FillCabinets(pSetEdge, eAlign, Id0, Id1, Id2, Id3, Id4, Id5, Id6, Id7, Id8));
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	TSetEdge* FAutoSetCabinet::MaxLenSeg(TArray<TSetEdge *>& ChimerySegs, TSetEdge* pExpacte)
	{
		float maxLen = FLT_MIN;
		TSetEdge* pSeg = nullptr;
		for (auto p : ChimerySegs)
		{
			if (p->mLen > maxLen && pExpacte!= p)
			{
				pSeg = p;
				maxLen = p->mLen;
			}
		}
		return pSeg;
	}
	bool FAutoSetCabinet::SignChimery()
	{
		mSinkSeg = nullptr;
		mChimeySeg = nullptr;
		mChimeyCorner = nullptr;
		if (mArea->GetChimery())//спял╣ю╣д 
		{
			TArray<TSetEdge *> ChimerySegs;
			for (auto p : mSelectSegs)
			{
				if (p->IsState(eSliptWallChimney, eSliptWallWindow))
					ChimerySegs.Add(p);
			}
			mChimeySeg = MaxLenSeg(ChimerySegs);
			if ( mChimeySeg )
			{
				mChimeyCorner = mChimeySeg->mStart;
				if (mChimeySeg->NextSeg()->IsState(eSliptWallChimney) )
					mChimeyCorner = mChimeySeg->mEnd;
				return true;
			}
		}
		///
		SignSink();

		mChimeySeg = MaxLenSeg(mSelectSegs, mSinkSeg);
		if (mChimeySeg == nullptr)
			mChimeySeg = mSinkSeg;
		CHECK_ERROR(mChimeySeg);
		if (mChimeySeg->mNextSeg->IsState(eSliptWallSelect))
		{
			mChimeyCorner = mChimeySeg->mEnd;
		}
		else
		{
			mChimeyCorner = mChimeySeg->mStart;
		}
		return true;
	}
	bool FAutoSetCabinet::SignSink()
	{
		mWindowType = 0;
		mSinkSeg = nullptr;
		if (mChimeySeg)
		{
			if (mChimeySeg->mPreSeg->IsState(eSliptWallSelect | eSliptWallWindow))
			{
				mSinkSeg = mChimeySeg->PreSeg();
				mWindowType = 1;
			}
			else if (mChimeySeg->mNextSeg->IsState(eSliptWallSelect | eSliptWallWindow))
			{
				mSinkSeg = mChimeySeg->NextSeg();
				mWindowType = 1;
			}
			CHECK_OK(mSinkSeg != nullptr);
			{
				TArray<TSetEdge *> ChimerySegs;
				for (auto p : mSelectSegs)
				{
					if (p->IsState(eSliptWallSelect | eSliptWallWindow))
						ChimerySegs.Add(p);
				}
				mSinkSeg = MaxLenSeg(ChimerySegs, mChimeySeg);
				CHECK_OK(mSinkSeg != nullptr);
			}
			//
			if (mChimeySeg->mPreSeg->IsState(eSliptWallSelect) && mChimeySeg->mPreSeg->GetLen() > mSink.mSizeMin.X + GetRes(200035).mSize.X)
			{
				mSinkSeg = mChimeySeg->PreSeg();
				mWindowType = 2;
			}
			else if (mChimeySeg->mNextSeg->IsState(eSliptWallSelect) && mChimeySeg->mPreSeg->GetLen() > mSink.mSizeMin.X + GetRes(200035).mSize.X)
			{
				mSinkSeg = mChimeySeg->NextSeg();
				mWindowType = 3;
			}
			CHECK_OK(mSinkSeg != nullptr);
			{
				TArray<TSetEdge *> ChimerySegs;
				for (auto p : mSelectSegs)
				{
					if (p->mPreSeg->IsState(eSliptWallWindow) || p->mNextSeg->IsState(eSliptWallWindow))
						ChimerySegs.Add(p);
				}
				mSinkSeg = MaxLenSeg(ChimerySegs, mChimeySeg);
				CHECK_OK(mSinkSeg != nullptr);
			}
		}
		{
			TArray<TSetEdge *> ChimerySegs;
			for (auto p : mSelectSegs)
			{
				if (p->IsState(eSliptWallSelect | eSliptWallWindow))
					ChimerySegs.Add(p);
			}
			mSinkSeg = MaxLenSeg(ChimerySegs, mChimeySeg);
			CHECK_OK(mSinkSeg != nullptr);
		}
		{
			TArray<TSetEdge *> ChimerySegs;
			for (auto p : mSelectSegs)
			{
				if (p->mPreSeg->IsState( eSliptWallWindow) || p->mNextSeg->IsState(eSliptWallWindow))
					ChimerySegs.Add(p);
			}
			mSinkSeg = MaxLenSeg(ChimerySegs,mChimeySeg);
			CHECK_OK(mSinkSeg != nullptr);
		}
		if(mChimeySeg!=nullptr)
			mSinkSeg = MaxLenSeg(mSelectSegs, mChimeySeg);
		CHECK_OK(mSinkSeg != nullptr);
		mSinkSeg = mChimeySeg;
		return true;
	}
}