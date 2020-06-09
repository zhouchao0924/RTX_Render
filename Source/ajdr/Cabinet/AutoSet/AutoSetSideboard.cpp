
#include "AutoSetSideboard.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetSideboard
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		FCabinetRes res = (mCabinetType) ? GetRes(mCabinetType) : GetRes(200117);// : GetRes(200118);
		pSetEdge->SetCabinetPos(FCabinetSet::eAlignCentry, FCabinetSet::eFillMax, res);
		return true;
	}
}