
#include "AutoSetTelevision.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetTelevision
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		pSetEdge->SetCabinetPos(FCabinetSet::eAlignCentry, FCabinetSet::eFillMax, GetRes(200115));
		return true;
	}
}