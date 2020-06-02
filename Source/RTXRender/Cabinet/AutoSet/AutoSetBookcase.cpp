
#include "AutoSetBookcase.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetBookcase
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		int len = pSetEdge->GetRemainLen();
		FCabinetRes res = GetRes(200072);
		if ( mCabinetType== 200068 )
		{
			if (len <= 600)
			{
				res = GetRes(200069);
			}
			else if (len < 1200)
			{
				res = GetRes(200070);
			}
			else
			{
				res = GetRes(200071);
			}
		}
		if (mCabinetType == 200064 || mCabinetType == 0)
		{
			if (len <= 600)
			{
				res = GetRes(200065);
			}
			else if (len < 1200)
			{
				res = GetRes(200066);
			}
			else
			{
				res = GetRes(200067);
			}
		}
		CHECK_ERROR(res.mSizeMin.X <= pSetEdge->mLen);
		pSetEdge->SetCabinetPos(FCabinetSet::eAlignCentry, FCabinetSet::eFillMax, res);
		return true;
	}
}