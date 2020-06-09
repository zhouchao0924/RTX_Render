
#include "AutoSetBedCabinet.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetBedCabinet
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		FCabinetSet slot;
		slot.mAlign = FCabinetSet::eAlignHead;
		slot.mFillType = FCabinetSet::eFillDefault;
		slot.mRes = (mCabinetType) ? GetRes(mCabinetType) : GetRes(200120);// : GetRes(200121);
		float DistNext = pSetEdge->NextSeg()->HeadStateDist(eSliptWallDoor);
		if (DistNext > slot.mRes.mSize.Y)
			slot.mAlign = FCabinetSet::eAlignTail;
		if ( pSetEdge->mStart->IsTrun() && !pSetEdge->mEnd->IsTrun() )
		{
			slot.mAlign = FCabinetSet::eAlignHead;
		}
		if (!pSetEdge->mStart->IsTrun() && pSetEdge->mEnd->IsTrun())
		{
			slot.mAlign = FCabinetSet::eAlignTail;
		}
		pSetEdge->SetCabinetPos(slot);
		return true;
	}
}