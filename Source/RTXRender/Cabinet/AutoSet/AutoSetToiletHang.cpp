
#include "AutoSetToiletHang.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
//////////////////////////////////////////////////////////////////////////
namespace AutoSetToiletHang
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		FCabinetRes mMirror = (mCabinetHangType) ? GetRes(mCabinetHangType) : GetRes(200062);// GetRes(200059);
		mHeight = 1300;
		FCabinetSet::eAlignType eType = HaveStemRegion(pSetEdge);
		//if (eType == FCabinetSet::eAlignCentry)
		//{
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignCentry, FCabinetSet::eFillMax, mMirror);
		//}
		//else
		//{
		//	pSetEdge->SetCabinetPos(eType, FCabinetSet::eFillDefault, mMirror);
		//}
		return true;
	}
}