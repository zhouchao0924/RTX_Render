
#include "AutoSetCoatroom.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetCoatroom
{
	bool FAutoSetCabinet::PreExecute()
	{
		mExpactBit = eSliptWallDoor | eSliptWallWindow;
		AddPoly(mSelectWalls, eSliptWallSelectHang);
		return true;
	}
	bool FAutoSetCabinet::AfterExecute()
	{
		float fScaleZ = 2100.0f / mResTurn.mCabinetHeight;
		for (auto pset : mSegments)
		{
			TSetEdge* pSetEdge = (TSetEdge*)pset;
			for (int i = 0; i < pSetEdge->mCabinets.Num(); ++i)
			{
				pSetEdge->mCabinets[i].mScaleZ = fScaleZ;
			}
		}
		return	true;
	}
	bool FAutoSetCabinet::SetSetEdge2(TSetEdge* pSetEdge)
	{
		InitCabinetRes();
		FCabinetSet::eAlignType alignType = FCabinetSet::eAlignHead;
		if (pSetEdge->mStart->GetType() == eCorner90 && pSetEdge->mPreSeg->IsState(eSliptWallSelect))
		{
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillDefault, mResTurn);
		}
		if (pSetEdge->mEnd->GetType() == eCorner90 && pSetEdge->mNextSeg->IsState(eSliptWallSelect))
		{
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignTail, FCabinetSet::eFillDefault, mResFake);
			alignType = FCabinetSet::eAlignTail;
		}
		int32 Id2 = mResDouble.mTeplateId;
		int32 Id1 = mResSingle.mTeplateId;
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id1));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2));//0
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id1));//0
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2));//1
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id1));//2
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2));//3.0
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id1));//3.1
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2));//4.0
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id1));//4.1
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id1));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id1));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id1));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id1));

		return true;
	}
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		bool b = SetSetEdge2(pSetEdge);
		CHECK_ERROR(b);
		float fScaleZ = 2100.0f / mResTurn.mCabinetHeight;
		for (int i = 0; i < pSetEdge->mCabinets.Num(); ++i)
		{
			pSetEdge->mCabinets[i].mScaleZ = fScaleZ;
		}
		return true;
	}
	bool FAutoSetCabinet::InitCabinetRes()
	{
		if (mCabinetType == 0 || mCabinetType == 200083)
		{
			mResSingle = GetRes(200084);
			mResDouble = GetRes(200085);
			mResTurn = GetRes(200086);
			mResFake.mSize.X = mResTurn.mSize.Y;
		}
		else
		{
			mResSingle = GetRes(200088);
			mResDouble = GetRes(200089);
			mResTurn = GetRes(200090);
			mResFake.mSize.X = mResTurn.mSize.Y;
		}
		return true;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////

namespace AutoSetCoatroomHang
{
	bool FAutoSetCabinet::PreExecute()
	{
		mHeight = 2100;
		mExpactBit = eSliptWallDoor | eSliptWallWindow;
		AddPoly(mSelectWalls, eSliptWallSelectHang);
		return true;
	}
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		InitCabinetRes();

		FCabinetSet::eAlignType alignType = FCabinetSet::eAlignHead;
		if (pSetEdge->mStart->GetType() == eCorner90 && pSetEdge->mPreSeg->IsState(eSliptWallSelect))
		{
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillDefault, mResTurn);
		}
		if (pSetEdge->mEnd->GetType() == eCorner90 && pSetEdge->mNextSeg->IsState(eSliptWallSelect))
		{
			pSetEdge->SetCabinetPos(FCabinetSet::eAlignTail, FCabinetSet::eFillDefault, mResFake);
			alignType = FCabinetSet::eAlignTail;
		}
		int32 Id2 = mResDouble.mTeplateId;
		int32 Id1 = mResSingle.mTeplateId;
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id1));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2));//0
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id1));//0
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2));//1
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id1));//2
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2));//3.0
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id1));//3.1
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2));//4.0
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id1));//4.1
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id1));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id1));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id1));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id2));
		CHECK_OK(FillCabinets(pSetEdge, alignType, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id2, Id1));

		return true;
	}
	bool FAutoSetCabinet::InitCabinetRes()
	{
		if (mCabinetHangType == 0 || mCabinetType == 200091)
		{
			mResSingle = GetRes(200092);
			mResDouble = GetRes(200093);
			mResTurn = GetRes(200094);
			mResFake.mSize.X = mResTurn.mSize.Y;
		}
		else
		{
			mResSingle = GetRes(200096);
			mResDouble= GetRes(200097);
			mResTurn = GetRes(200098);
			mResFake.mSize.X = mResTurn.mSize.Y;
		}
		return true;
	}
}