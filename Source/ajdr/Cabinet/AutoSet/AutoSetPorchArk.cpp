
#include "AutoSetPorchArk.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetPorchArk
{
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
					pSetEdge->SetFillDist(fDistHead - 100, false);
				}
				else
				{
					pSetEdge->SetFillDist(fDistTail - 100, true);
				}
			}
		}
		else
		{
			if (!pSetEdge->PreEdge()->IsState(eSliptWallSolid))
				pSetEdge->FillDist(100, true);

			if (!pSetEdge->NextEdge()->IsState(eSliptWallSolid))
				pSetEdge->FillDist(100, false);
		}
		FCabinetRes res = (mCabinetType) ? GetRes(mCabinetType) : GetOptionCabinet(pSetEdge, 200010, 200011, 200012);
		CHECK_ERROR(res.mTeplateId > 0);
		CHECK_ERROR(res.mSizeMin.X <= pSetEdge->mLen);
		pSetEdge->SetCabinetPos(FCabinetSet::eAlignCentry, FCabinetSet::eFillMax, res);
		return true;
	}
}