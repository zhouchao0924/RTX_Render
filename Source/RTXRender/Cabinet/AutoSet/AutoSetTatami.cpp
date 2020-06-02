
#include "AutoSetTatami.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
//////////////////////////////////////////////////////////////////////////
namespace AutoSetTatami
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		mSetEdge = pSetEdge;
		CHECK_ERROR(!pSetEdge->IsState(eSliptWallDoor));
		CHECK_ERROR(AutoSetSideType());

		FCabinetRes NormalRes = GetRes(200078);
		FCabinetRes DragRes = (mCabinetType) ? GetRes(mCabinetType) : GetRes(200079);
		FCabinetRes WardrobeRes = (mCabinetHangType) ? GetRes(mCabinetHangType) : GetRes(200131);

		if ( mSideType == 0 )
		{
			mSetEdge->SetCabinetVer(FCabinetSet::eAlignHead, GetRes(mCabinetTypes[0]), mDepth);
		}
		else if ( mSideType == 1 )
		{
			mSetEdge->SetCabinetVer(FCabinetSet::eAlignTail, GetRes(mCabinetTypes[0]), mDepth);
		}
		float Depth1 = mDepth / ((float)mRow);

		int32 nHead = pSetEdge->mHead;
		int32 nTail = pSetEdge->mTail;
		if ( mRow == 1 )
		{
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, GetRes(mCabinetTypes[1]), 1.0f, Depth1 / DragRes.mSize.Y);
		}
		else if (mRow==2)
		{
			pSetEdge->ReseatRemain(nHead, nTail);
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, GetRes(mCabinetTypes[1]), 1.0f, Depth1 / NormalRes.mSize.Y);

			pSetEdge->ReseatRemain(nHead, nTail);
			FCabinetSet&  CabinetSet = pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, GetRes(mCabinetTypes[2]), 1.0f, Depth1 / DragRes.mSize.Y);
			CabinetSet.mPos += pSetEdge->mNor*Depth1;
		}
		else if ( mRow == 3 )
		{
			pSetEdge->ReseatRemain(nHead, nTail);
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, GetRes(mCabinetTypes[1]), 1.0f, Depth1 / NormalRes.mSize.Y);

			pSetEdge->ReseatRemain(nHead, nTail);
			FCabinetSet&  CabinetSet = pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, GetRes(mCabinetTypes[2]), 1.0f, Depth1 / NormalRes.mSize.Y);
			CabinetSet.mPos += pSetEdge->mNor*Depth1;

			pSetEdge->ReseatRemain(nHead, nTail);
			FCabinetSet&  CabinetSet2 = pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, GetRes(mCabinetTypes[3]), 1.0f, Depth1 / DragRes.mSize.Y);
			CabinetSet2.mPos += pSetEdge->mNor*Depth1*2;
		}
		return true;
	}
	bool FAutoSetCabinet::OnRePlaceCabinet(TSetEdge* pSetEdge, int32 CabinetIdx, int32 mcategoryId)
	{
		mCabinetTypes[CabinetIdx] = mcategoryId;
		return true;
	}
	bool FAutoSetCabinet::SetnDepth(int32 nDepth)
	{
		mDepth = nDepth;
		mSideType = 100;
		FCabinetRes NormalRes = GetRes(200078);
		FCabinetRes DragRes = GetRes(200079);

		mCabinetTypes[0] = 200131;
		if ((NormalRes.mSizeMin.Y + NormalRes.mSizeMin.Y + DragRes.mSizeMin.Y) <= mDepth )
		{
			int32 nMax = (NormalRes.mSizeMax.Y + NormalRes.mSizeMax.Y + DragRes.mSizeMax.Y);
			mDepth = TinyMin(nMax, mDepth);
			mRow = 3;
			mCabinetTypes[1] = 200078;
			mCabinetTypes[2] = 200078;
			mCabinetTypes[3] = 200079;
		}
		else if ((NormalRes.mSizeMin.Y + DragRes.mSizeMin.Y) <= mDepth )
		{
			int32 nMax = (NormalRes.mSizeMax.Y + DragRes.mSizeMax.Y);
			mDepth = TinyMin(nMax, mDepth);
			mRow = 2;
			mCabinetTypes[1] = 200078;
			mCabinetTypes[2] = 200079;
			mCabinetTypes[3] = 200079;
		}
		else
		{
			int32 nMax = (NormalRes.mSizeMax.Y);
			mDepth = TinyMin(nMax, mDepth);
			mRow = 1;
			mCabinetTypes[1] = 200078;
			mCabinetTypes[2] = 200078;
			mCabinetTypes[3] = 200079;
		}
		return true;
	}

	bool FAutoSetCabinet::GetLayoutResult(FCabinetLayoutResult& LayoutData)
	{
		FAutoSetBase::GetLayoutResult(LayoutData);
		mSideData[0].selected = mSideType == 0;
		mSideData[1].selected = mSideType == 1;
		LayoutData.mTatamiExtern.Add(mSideData[0]);
		LayoutData.mTatamiExtern.Add(mSideData[1]);
		return true;
	}
	FAutoSetCabinet::FAutoSetCabinet()
	{
		mReplaceReseat = true; 
	}
	bool FAutoSetCabinet::SetSideType(int32 nSideType)
	{
		mSideType = nSideType;
		mSetEdge->ReSetClear();
		SetSetEdge(mSetEdge);
		return true;
	}

	bool FAutoSetCabinet::AutoSetSideType()
	{
		CHECK_OK(mSideType != 100);
		FCabinetRes NormalRes = GetRes(200078);
		FCabinetRes DragRes = GetRes(200079);
		FCabinetRes WardrobeRes = GetRes(200131);

		if (mSetEdge->mStart->GetType() == eCorner90 && mSetEdge->mEnd->GetType() != eCorner90)
		{
			mSideType = 1;
		}
		else if (mSetEdge->mStart->GetType() != eCorner90 && mSetEdge->mEnd->GetType() == eCorner90)
		{
			mSideType = 0;
		}
		else if ( mSetEdge->HeadStateDist(eSliptWallWindow)<= WardrobeRes.mSize.Y )
		{
			mSideType = 1;
		}
		else if (mSetEdge->TailStateDist(eSliptWallWindow) <= WardrobeRes.mSize.Y)
		{
			mSideType = 0;
		}
		else if (mSetEdge->PreSeg()->TailStateDist(eSliptWallWindow) <= mDepth )
		{
			mSideType = 0;
		}
		else if (mSetEdge->NextSeg()->HeadStateDist(eSliptWallWindow) <= mDepth)
		{
			mSideType = 1;
		}
		else if (mSetEdge->PreSeg()->TailStateDist(eSliptWallDoor) <= mDepth)
		{
			mSideType = 0;
		}
		else if (mSetEdge->NextSeg()->HeadStateDist(eSliptWallDoor) <= mDepth)
		{
			mSideType = 1;
		}
		else
		{
			mSideType = 0;
		}
		FVector2D DirPre = Rote270(mSetEdge->mDir);
		FVector2D DirNext = Rote90(mSetEdge->mDir);

		mSideData[0].size.X = mDepth;
		mSideData[0].size.Y = WardrobeRes.mSize.Y;
		mSideData[0].type = 0;
		mSideData[1].size.X = mDepth;
		mSideData[1].size.Y = WardrobeRes.mSize.Y;
		mSideData[1].type = 1;
		mSideData[0].rotate= Dir2Rotate(DirPre);
		mSideData[0].point = mSetEdge->mStart->GetVector() - mDepth * 0.5f *DirPre + mSetEdge->mDir* WardrobeRes.mSize.Y / 2;
		mSideData[1].rotate = Dir2Rotate(DirNext);
		mSideData[1].point = mSetEdge->mEnd->GetVector() + mDepth * 0.5f *DirNext - mSetEdge->mDir* WardrobeRes.mSize.Y / 2;
		return true;
	}
}