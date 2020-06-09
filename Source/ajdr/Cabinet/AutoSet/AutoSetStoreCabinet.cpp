
#include "AutoSetStoreCabinet.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetStoreCabinet
{
	FAutoSetCabinet::FAutoSetCabinet()
	{
		mExpactBit = eSliptWallDoor;
	}
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		if (pSetEdge->IsState(eSliptWallDoor))
		{
			float fDistTail = pSetEdge->TailStateDist(eSliptWallDoor);
			float fDistHead = pSetEdge->HeadStateDist(eSliptWallDoor);
			if (fDistHead < FLT_MAX)
			{
				if (fDistHead > fDistTail)
				{
					pSetEdge->SetFillDist(fDistHead/* - 100*/, false);
				}
				else
				{
					pSetEdge->SetFillDist(fDistTail/* - 100*/, true);
				}
			}
			CHECK_ERROR(pSetEdge->GetRemainLen() > 600);
			SetSetEdge(pSetEdge, FCabinetSet::eAlignHead);
			return true;
		}
		else // ²»º¬ÃÅµÄÇ½
		{
			float fHeadLen = 0;
			float fTailLen = 0;
			float fDistHead = pSetEdge->PreSeg()->TailStateDist(eSliptWallDoor);
			float fDistTail = pSetEdge->NextSeg()->HeadStateDist(eSliptWallDoor);
			if (fDistHead < GetRes(200110).mSize.Y && pSetEdge->mStart->GetType() == eCorner90)
			{
				fHeadLen = 1200;
			}
			if (fDistTail < GetRes(200110).mSize.Y && pSetEdge->mEnd->GetType() == eCorner90)
			{
				fTailLen = 1200;
			}
			if( fHeadLen >0 )
				pSetEdge->FillFake(fHeadLen, FCabinetSet::eAlignHead);
			if( fTailLen >0 )
				pSetEdge->FillFake(fTailLen, FCabinetSet::eAlignTail);
			SetSetEdge(pSetEdge, FCabinetSet::eAlignHead);
		}
		return true;
	}
	bool FAutoSetCabinet::SetSetEdge(TSetEdge * pSetEdge, FCabinetSet::eAlignType alignType)
	{
		FCabinetRes mResSingle = GetRes(200110);
		FCabinetRes mResDouble = GetRes(200111);
		float fSizeY = mResDouble.mSize.Y;
		float DistPre = pSetEdge->PreSeg()->TailStateDist(eSliptWallWindow);
		if (DistPre > mResDouble.mSizeMin.Y && DistPre < mResDouble.mSizeMax.Y)
		{
			fSizeY = DistPre;
		}
		float DistNext = pSetEdge->NextSeg()->HeadStateDist(eSliptWallWindow);
		if (DistNext > mResDouble.mSizeMin.Y && DistNext < mResDouble.mSizeMax.Y)
		{
			fSizeY = DistPre;
		}
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111));
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200110));
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111));//0
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200110));//0
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111));//1
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200110));//2
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111));//3.0
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200110));//3.1
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200111));//4.0
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200110));//4.1
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200111, 200111));
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200111, 200110));
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200111, 200111, 200111));
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200111, 200111, 200110));
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200111, 200111, 200111, 200111));
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200111, 200111, 200111, 200110));
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200111, 200111, 200111, 200111, 200111));
		CHECK_OK(FillCabinets(pSetEdge, alignType, 200111, 200111, 200111, 200111, 200111, 200111, 200111, 200111, 200110));
		for (auto p:pSetEdge->mCabinets)
		{
			p.mScale.Y = mResSingle.mSize.Y / fSizeY;
		}
		return true;
	}
}