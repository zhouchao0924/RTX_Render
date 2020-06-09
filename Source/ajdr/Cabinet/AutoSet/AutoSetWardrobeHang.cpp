
#include "AutoSetWardrobeHang.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetWardrobeHang
{
	bool FAutoSetCabinet::SetSetEdge(TSetEdge * pSetEdge, FCabinetSet::eAlignType eAlign, FCabinetSet::eFillType eFillType)
	{
		if (pSetEdge->PreSeg()->IsState(eSliptWallSelect))
			pSetEdge->mHead += GetRes(200026).mSize.Y;

		int32 nlen = pSetEdge->GetRemainLen();
		int32 nHead = pSetEdge->mHead;
		int32 nTail = pSetEdge->mTail;
		FCabinetRes hangSingle = GetRes(200025);
		FCabinetRes hangDouble = GetRes(200026);

		FCabinetRes yinmen2 = GetRes(200017);
		FCabinetRes yinmen3 = GetRes(200018);

		FCabinetRes kaimen2 = GetRes(200021);
		FCabinetRes kaimen3 = GetRes(200022);
		FCabinetRes kaimen4 = GetRes(200023);

		mHeight = yinmen2.mCabinetHeight;

		hangSingle.mHeight = yinmen2.mCabinetHeight;
		hangDouble.mHeight = yinmen2.mCabinetHeight;

		if ( nlen <=1600 )
		{
			float fScale = (nlen / (hangDouble.mSize.X + hangSingle.mSize.X));
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangDouble, fScale);
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangSingle, fScale);
		}
		else if ( nlen <= 2200 )
		{
			float fScale = (nlen / (hangDouble.mSize.X * 2));
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangDouble, fScale);
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangDouble, fScale);
		}
		else if (nlen<=3200)
		{
			float fScale = (nlen / (hangDouble.mSize.X*2 + hangSingle.mSize.X));
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangSingle, fScale);
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangDouble, fScale);
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangDouble, fScale);
		}
		else
		{
			float fScale = (nlen / (hangDouble.mSize.X * 2 + hangSingle.mSize.X));
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangDouble, fScale);
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangDouble, fScale);
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, hangSingle, fScale);
		}
		return true;
	}
}