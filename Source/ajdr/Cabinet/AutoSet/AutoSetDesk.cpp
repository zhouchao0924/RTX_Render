
#include "AutoSetDesk.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetDesk
{
	bool FAutoSetCabinet::PreExecute()
	{
		TArray<TRectBase> Rects;
		mArea->GetFurnitureModel(0, Rects);
		AddPoly(Rects, eSliptWallObject);
		return true;
	}
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		FCabinetSet slot;
		slot.mAlign = FCabinetSet::eAlignCentry;
		slot.mFillType = FCabinetSet::eFillMax;
		slot.mRes = (mCabinetType) ? GetRes(mCabinetType) : GetRes(200074);//GetRes(200075);
		if (!pSetEdge->IsState(eSliptWallObject))
		{
			float DistNext = pSetEdge->NextSeg()->HeadStateDist(eSliptWallDoor);
			if (DistNext > slot.mRes.mSize.Y)
				slot.mAlign = FCabinetSet::eAlignTail;
		}
		else
		{
			if (pSetEdge->mStart->mNextEdge->IsState(eSliptWallObject))
			{
				pSetEdge->FillDist(pSetEdge->mStart->mNextEdge->GetLen() + 100, true);
			}
			if (pSetEdge->mEnd->mPreEdge->IsState(eSliptWallObject))
			{
				pSetEdge->FillDist(pSetEdge->mEnd->mPreEdge->GetLen() + 100, false);
			}
		}
		CHECK_ERROR(slot.mRes.mSizeMin.X <= pSetEdge->GetRemainLen());
		pSetEdge->SetCabinetPos(slot);
		return true;
	}
}