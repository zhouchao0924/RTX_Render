
#include "AutoSetCookHang.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
#include "AutoSetCookFloor.h"
using namespace PolyEdgeLib;
//////////////////////////////////////////////////////////////////////////
namespace AutoSetCookHang
{
	FCabinetRes FAutoSetCabinet::GetCorner270Fill()
	{
		FCabinetRes res;
		res.mSize = FVector2D(-GetRes(200045).mSize.Y, GetRes(200045).mSize.Y);
		res.mSizeMax = res.mSize;
		return res;
	}
	FCabinetRes FAutoSetCabinet::GetCorner90Fill()
	{
		FCabinetRes res;
		res.mSize = FVector2D(GetRes(200045).mSize.Y, GetRes(200045).mSize.Y);
		res.mSizeMax = res.mSize;
		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FAutoSetCabinet::PreExecute()
	{
		mChimeyHang = GetRes(200040);
		mSingleDoor = GetRes(200041);
		mDoubleDoor = GetRes(200042);
		mChimeySide = GetRes(200043);
		//mCornerLine = GetRes(200045);

		mHeight = gCabinetGlobal.mHangSetHeight;

		FCabinetSkuModel chimery = AutoSetCookFloor::FAutoSetCabinet::Instance().SkuData[1];
		TRectBase ChimeryRect;
		ChimeryRect.mPos = chimery.point;
		ChimeryRect.mDir = FVector2D(cos(chimery.rotate * PI/180), sin(chimery.rotate* PI / 180));
		ChimeryRect.mSize = FVector2D(mChimeyHang.mSize.X,GetRes(200033).mSize.Y)*0.5f;
		AddPoly(&ChimeryRect, eSliptWallChimeyCabinet);
		return true;
	}
	bool FAutoSetCabinet::PostExecute()
	{
		TSetEdge* pChimeySeg = (TSetEdge*)GetFirstSegment(eSliptWallSelect | eSliptWallChimeyCabinet);
		if (pChimeySeg)
		{
			FCabinetRes res = GetRes(200040);
			res.mHeight = 2350-res.mCabinetHeight;
			pChimeySeg->SetCabinetPos(FCabinetSet::eAlignCentry, FCabinetSet::eFillFull,res);
		}
		return true;
	}
	FAutoSetCabinet::FAutoSetCabinet()
	{
		mReplaceReseat = false;
		mBit = eSliptWallSelect | eSliptWallSolid;
		mExpactBit = eSliptWallDoor | eSliptWallWindow | eSliptWallPillar | eSliptWallChimeyCabinet;
	}

	FAutoSetCabinet::~FAutoSetCabinet()
	{
	}
	////////////////////////////////////////////////////////////////////////
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSeg)
	{
		FCabinetSet::eAlignType alignType = FCabinetSet::eAlignHead;
		//////////////////////////////////////////////////////////////////////////
		if (pSeg->NextSeg()->IsState(mBit, mExpactBit) && pSeg->mEnd->GetType()== eCorner270)
		{
			pSeg->FillFake(-mDoubleDoor.mSize.Y, FCabinetSet::eAlignTail);
		}
		else if (pSeg->NextSeg()->IsState(mBit, mExpactBit) && pSeg->mEnd->GetType() == eCorner90 && pSeg->NextSeg()->mLen > GetRes(200045).mSizeMin.X )
		{
			pSeg->FillFake(mDoubleDoor.mSize.Y, FCabinetSet::eAlignTail);
		}
		//////////////////////////////////////////////////////////////////////////
		if ( pSeg->NextSeg()->IsState(mBit, mExpactBit) && (!pSeg->PreSeg()->IsState(mBit, mExpactBit)) )
			alignType = FCabinetSet::eAlignTail;

		if (pSeg->mPreSeg->IsState(eSliptWallChimeyCabinet))
			alignType = FCabinetSet::eAlignHead;
		else if (pSeg->mNextSeg->IsState(eSliptWallChimeyCabinet))
			alignType = FCabinetSet::eAlignTail;
		CHECK_OK(FillCabinets(pSeg, alignType, 200041));
		CHECK_OK(FillCabinets(pSeg, alignType, 200042));
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042));//0
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200041));//0
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042));//1
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200041));//2
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042));//3.0
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200041));//3.1
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200042));//4.0
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200041));//4.1
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200042, 200042));
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200042, 200041));
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200042, 200042, 200042));
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200042, 200042, 200041));
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200042, 200042, 200042, 200042));
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200042, 200042, 200042, 200041));
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200042, 200042, 200042, 200042, 200042));
		CHECK_OK(FillCabinets(pSeg, alignType, 200042, 200042, 200042, 200042, 200042, 200042, 200042, 200042, 200041));
		return true;
	}
}