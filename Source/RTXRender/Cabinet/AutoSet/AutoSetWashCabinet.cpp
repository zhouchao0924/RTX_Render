
#include "AutoSetWashCabinet.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetWashCabinet
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		FCabinetRes res = (mCabinetType) ? GetRes(mCabinetType) : GetOptionCabinet(pSetEdge,200105, 200104, 200103, 200102);
		CHECK_ERROR(res.mTeplateId > 0);
		CHECK_ERROR(res.mSizeMin.X <= pSetEdge->mLen);
		pSetEdge->SetCabinetPos(FCabinetSet::eAlignCentry, FCabinetSet::eFillMax, res);
		return true;
	}
}