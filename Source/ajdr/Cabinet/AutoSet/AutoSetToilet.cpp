
#include "AutoSetToilet.h"
#include "Math/LayoutMath.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
//////////////////////////////////////////////////////////////////////////
namespace AutoSetToilet
{
	FCabinetSet::eAlignType FAutoSetCabinet::HaveStemRegion(TSetEdge* pSetEdge)
	{
		TEdge* pEdge = pSetEdge->mStart->mNextEdge;
		TPoint* pNextTurn = pEdge->NextTurnPoint();
		TPoint* pPreTurn = pEdge->PreTurnPoint();
		bool  pDoorNext = pEdge->NextTurnPoint()->mNextEdge->NextFirstEdge(eSliptWallDoor);
		bool  pDoorPre = pEdge->PreTurnPoint()->mPreEdge->PreFirstEdge(eSliptWallDoor);
		for (auto p : mArea->mRawWalls)
		{
			if (p.Value.mState == 0)
			{
				if (pSetEdge->mStart->GetVector().Equals(p.Value.mStart,30) || pSetEdge->mStart->GetVector().Equals(p.Value.mEnd, 30) )
				{
					pDoorPre = true;
					pDoorNext = true;
				}
				if (pSetEdge->mEnd->GetVector().Equals(p.Value.mStart, 30) || pSetEdge->mEnd->GetVector().Equals(p.Value.mEnd, 30))
				{
					pDoorPre = true;
					pDoorNext = true;
				}
			}
		}
		if (pDoorNext && pDoorPre)//ÓÐ¸ÉÇø
		{
			return FCabinetSet::eAlignCentry;
		}
		if (pDoorPre)
			return FCabinetSet::eAlignHead;
		if(pDoorNext)
			return FCabinetSet::eAlignTail;

		return FCabinetSet::eAlignHead;
	}
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		FCabinetRes mToilet = (mCabinetType) ? GetRes(mCabinetType) : GetOptionCabinet(pSetEdge,200053, 200054, 200055);
		if (mToilet.mTeplateId == -1)
			return false;
		FCabinetSet::eAlignType eType = HaveStemRegion(pSetEdge);
		if (eType == FCabinetSet::eAlignCentry)
		{
			pSetEdge->SetCabinetPos(eType, FCabinetSet::eFillFull, mToilet);
		}
		else
		{
			pSetEdge->SetCabinetPos(eType, FCabinetSet::eFillDefault, mToilet);
		}
		return true;
	}

	bool FAutoSetCabinet::AfterExecute()
	{
		for (auto pset : mSegments)
		{
			TSetEdge* pSetEdge = (TSetEdge*)pset;
			if (pSetEdge->mCabinets.Num() > 0 && pSetEdge->GetLen() < pSetEdge->mCabinets[0].mRes.mSize.X)
			{
				FLine templine(pSetEdge->mStart->GetVector(), pSetEdge->mEnd->GetVector());
				auto tempPos = templine.GetMiddlePoint();
				if (pSetEdge->mCabinets[0].mDir.X == 1.f)
					pSetEdge->mCabinets[0].mPos.X = tempPos.X;
				else
					pSetEdge->mCabinets[0].mPos.Y = tempPos.Y;

				pSetEdge->mCabinets[0].mScale.X = pSetEdge->GetLen() / pSetEdge->mCabinets[0].mRes.mSize.X;

			}

		}

		return true;
	}
}