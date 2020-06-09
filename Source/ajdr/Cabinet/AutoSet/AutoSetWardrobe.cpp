
#include "AutoSetWardrobe.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetWardrobe
{
	bool FAutoSetCabinet::PreExecute()
	{
		TArray<TRectBase> Rects;
		mArea->GetFurnitureModel(0, Rects);
		AddPoly(Rects, eSliptWallObject);
		return true;
	}
	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		if (pSetEdge->IsState(eSliptWallDoor))
		{
			float fDistTail= pSetEdge->TailStateDist(eSliptWallDoor);
			float fDistHead= pSetEdge->HeadStateDist(eSliptWallDoor);
			if (fDistHead < FLT_MAX)//�ں��ŵ�ǽ�����¹�
			{
				if (fDistHead > fDistTail)
				{
					pSetEdge->SetFillDist(fDistHead/*-100*/, false);
				}
				else
				{
					pSetEdge->SetFillDist(fDistTail/*-100*/, true);
				}
			}
			SetSetEdge(pSetEdge, FCabinetSet::eAlignHead, FCabinetSet::eFillFull);
			return true;
		}
		else // �����ŵ�ǽ
		{
			float fHeadLen = 0;
			float fTailLen = 0;
			float fDistHead = pSetEdge->PreSeg()->TailStateDist(eSliptWallDoor);
			float fDistTail = pSetEdge->NextSeg()->HeadStateDist(eSliptWallDoor);
			if (fDistHead < GetRes(200017).mSize.Y && pSetEdge->mStart->GetType()== eCorner90)
			{
				fHeadLen = 1200;
			}
			if (fDistTail < GetRes(200017).mSize.Y && pSetEdge->mEnd->GetType() == eCorner90 )
			{
				fTailLen = 1200;
			}
			pSetEdge->FillDist(fHeadLen, true);
			pSetEdge->FillDist(fTailLen, false);

			if (pSetEdge->IsState(eSliptWallObject))
			{
				float fDistTailObject = pSetEdge->TailStateDist(eSliptWallObject);
				float fDistHeadObject = pSetEdge->HeadStateDist(eSliptWallObject);
				if (fDistTailObject > fDistHeadObject)
				{
					fHeadLen = fDistTailObject - 200;
					pSetEdge->ReseatRemain(pSetEdge->GetRemainLen()-fHeadLen,pSetEdge->mTail);
				}
				else
				{
					fTailLen = fDistHeadObject - 200;
					pSetEdge->ReseatRemain(0, fTailLen);
				}
			}
			return SetSetEdge(pSetEdge,FCabinetSet::eAlignHead, FCabinetSet::eFillFull);
		}
		return false;
	}
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

		hangSingle.mHeight = yinmen2.mCabinetHeight;
		hangDouble.mHeight = yinmen2.mCabinetHeight;

		if ( nlen <=1600 )
		{
			if ( mCabinetType == 200016 || mCabinetType == 0)
			{
				CHECK_ERROR(yinmen2.mSizeMin.X <= pSetEdge->mLen);
				pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, yinmen2);
			}
			else
			{
				CHECK_ERROR(kaimen3.mSizeMin.X <= pSetEdge->mLen);
				pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, kaimen3);

			}		
		}
		else if ( nlen <= 2200 )
		{
			if (mCabinetType == 200016 || mCabinetType == 0)
			{
				CHECK_ERROR(yinmen2.mSizeMin.X <= pSetEdge->mLen);
				pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, yinmen2);
			}
			else
			{
				CHECK_ERROR(kaimen4.mSizeMin.X <= pSetEdge->mLen);
				pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, kaimen4);
			}
		}
		else if (nlen<=3200)
		{
			if (mCabinetType == 200016 || mCabinetType == 0)
			{
				CHECK_ERROR(yinmen3.mSizeMin.X <= pSetEdge->mLen);
				pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, yinmen3);
			}
			else
			{
				float fScale = (nlen / (kaimen3.mSize.X*2));
				pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, kaimen3, fScale);
				pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFull, kaimen2, fScale);
			}
		}
		else
		{
			if (mCabinetType == 200016 || mCabinetType == 0)
			{
				if (nlen>yinmen2.mSize.X*2 )
				{
					float fScale = (nlen / (yinmen2.mSize.X + yinmen3.mSize.X));
					pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, yinmen3, fScale);
					pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, yinmen2, fScale);
				}
				else
				{
					float fScale = (nlen / (yinmen2.mSize.X + yinmen3.mSize.X));
					pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, yinmen3, fScale);
					pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, yinmen2, fScale);
				}
			}
			else
			{
				//float fScale = (nlen / (kaimen3.mSize.X + kaimen2.mSize.X));
				//pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, kaimen3, fScale);
				//pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, kaimen2, fScale);

				int num = (nlen / kaimen2.mSize.X + 0.5f);
				float fScale = (nlen / (float)num) / kaimen2.mSize.X;
				for(int i = 0; i < num; ++i)
					pSetEdge->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillFix, kaimen2, fScale);
			}
		}
		return true;
	}
}