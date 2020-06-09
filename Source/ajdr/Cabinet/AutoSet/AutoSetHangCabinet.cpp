
#include "AutoSetHangCabinet.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetHangBedCabinet
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		mHeight = 1500;
		FCabinetRes DragRes = (mCabinetType) ? GetRes(mCabinetType) : GetRes(200107);
		pSetEdge->SetCabinetPos(FCabinetSet::eAlignCentry, FCabinetSet::eFillDefault, DragRes);
		return true;
	}
}