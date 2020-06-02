
#include "AutoSetLaminate.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
namespace AutoSetLaminate
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		mHeight = 1500;
		pSetEdge->SetCabinetPos(FCabinetSet::eAlignCentry, FCabinetSet::eFillMax, GetRes(200123));
		return true;
	}
}