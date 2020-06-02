
#include "CabinetSetEdge.h"
#include "../CabinetMgr.h"
///////////////////////////////////////////////////////////////////////////
using namespace PolyEdgeLib;

float TSetEdge::HeadStateDist(uint32 nBit, uint32 nExpactBit /* = 0 */)
{
	TPoint* pPoint = nullptr;
	TEdge* pEdge = mStart->mNextEdge->NextFirstEdge(nBit, nExpactBit);
	if (pEdge)
		pPoint = pEdge->mStart;
	return mStart->Dist(pPoint);
}

float TSetEdge::TailStateDist(uint32 nBit, uint32 nExpactBit /* = 0 */)
{
	TPoint* pPoint = nullptr;
	TEdge* pEdge = mEnd->mPreEdge->PreFirstEdge(nBit, nExpactBit);
	if (pEdge)
		pPoint = pEdge->mEnd;
	return mEnd->Dist(pPoint);
}
bool TSetEdge::AdjustTurnDir(FCabinetSet & Cabinet)
{
	if (IsTurnCabinet(Cabinet.mRes.mTeplateId))
	{
		if(Cabinet.mAlign == FCabinetSet::eAlignTail )
		{
			Cabinet.mDir = FVector2D(-mDir.Y, mDir.X);
			return true;
		}
	}
	return false;
}
bool TSetEdge::SetCabinetsPos()
{
	mHead = 0;
	mTail = mLen;
	for (int32 i = 0; i < mCabinets.Num(); ++i)
	{
		FCabinetSet& CabinetSet = mCabinets[i];
		int32 nHalfLen = CabinetSet.mRes.mSize.X*CabinetSet.mScale.X*0.5f;
		if (CabinetSet.mAlign == FCabinetSet::eAlignHead)
		{
			CabinetSet.mPosLocal = mHead + nHalfLen;
			CabinetSet.mPos = mStart->GetVector() + CabinetSet.mPosLocal*mDir + mNor * CabinetSet.mRes.mSize.Y / 2;
			mHead += nHalfLen * 2;
		}
		else
		{
			CabinetSet.mPosLocal = mTail - nHalfLen;
			CabinetSet.mPos = mStart->GetVector() + CabinetSet.mPosLocal*mDir + mNor * CabinetSet.mRes.mSize.Y / 2;
			mTail -= nHalfLen * 2;
		}
	}
	return true;
}

bool TSetEdge::MergeNext()
{
	TSegment* pPreSeg = PreSeg();
	TSetEdge* pNext = NextSeg();
	TSegment::Init(mStart,pNext->mEnd);
	mPreSeg = pPreSeg;
	mNextSeg = pNext->NextSeg();
	mNextSeg->mPreSeg = this;
	return true;
}

bool TSetEdge::MergePre()
{
	TSetEdge* pPreSeg = PreSeg();
	TSetEdge* pNext = NextSeg();
	TSegment::Init(pPreSeg->mStart,mEnd);
	mPreSeg = pPreSeg->PreSeg();
	mNextSeg = pNext;
	mPreSeg->mNextSeg = this;
	return true;
}

bool TSetEdge::ReSetClear()
{
	mCabinets.Empty();
	ReseatRemain();
	return true;
}
bool TSetEdge::ReSetClearNormal()
{
	TArray<FCabinetSet> Cabinets;
	for (auto p : mCabinets)
	{
		if (p.mRes.mTeplateId == -1 || IsTurnCabinet(p.mRes.mTeplateId))
		{
			Cabinets.Add(p);
		}
	}
	mCabinets = Cabinets;
	SetCabinetsPos();
	return true;
}
bool TSetEdge::AdjustRemainSize()
{
	int nLen = GetRemainLen();
	CHECK_ERROR(nLen > 10);
	for (int32 i = mCabinets.Num() - 1; i >= 0 && nLen>0; --i)
	{
		float fSize = TinyMax(1.0f, mCabinets[i].mScale.X)*mCabinets[i].mRes.mSize.X;
		float MaxLen = TinyMin((nLen + fSize), mCabinets[i].mRes.mSizeMax.X);
		float fScale = MaxLen / mCabinets[i].mRes.mSize.X;
		fScale = FMath::CeilToInt(mCabinets[i].mRes.mSize.X*fScale) / mCabinets[i].mRes.mSize.X;
		nLen -= (MaxLen - mCabinets[i].mRes.mSize.X*mCabinets[i].mScale.X);
		mCabinets[i].mScale.X = fScale;
	}
	SetCabinetsPos();
	return true;
}

int32 TSetEdge::AdjustCabinets()
{
	int32 nTotalSize = 0;
	int32 nMinSize = 0;
	int32 nMaxSize = 0;
	int32 nFixSize = 0;
	int32 nBaseSize = 0;
	TArray<int32> Cabinets;
	for (int32 i=0; i < mCabinets.Num(); ++i)
	{
		FCabinetSet& cabinet = mCabinets[i];
		if (cabinet.mRes.mSizeMin.X < cabinet.mRes.mSizeMax.X)
		{
			Cabinets.Add(i);
			nBaseSize += cabinet.mRes.mSize.X;
		}
		else
		{
			nFixSize += cabinet.mRes.mSize.X;
		}
		nTotalSize +=FMath::CeilToInt(cabinet.mRes.mSize.X*cabinet.mScale.X);
		nMinSize += cabinet.mRes.mSizeMin.X;
		nMaxSize += cabinet.mRes.mSizeMax.X;
	}
	//if (FMath::Abs(nTotalSize - mLen) < 5)
	//{
	//	SetCabinetsPos();
	//	return 0;
	//}
	if (nMinSize > mLen+5)
	{
		return mLen - nMinSize;
	}
	if (mLen-5 > nMaxSize)
	{
		return mLen - nMaxSize;
	}
	AdjustCabinets(Cabinets, nBaseSize, mLen - nFixSize);
	SetCabinetsPos();
	return 0;
}
bool TSetEdge::AdjustCabinets(TArray<int32>& Cabinets, int nBaseSize, int nTotalLen)
{
	float ScaleRef = nTotalLen / ((float)nBaseSize);
	for (int i = 0; i < Cabinets.Num() - 1; ++i)
	{
		for (int j = 0; j < Cabinets.Num() - 1 - i; ++j)
		{
			if (ScaleRef > 1.0f)
			{
				if (mCabinets[Cabinets[i]].mRes.GetMaxScale() > mCabinets[Cabinets[j + 1]].mRes.GetMaxScale() )
				{
					TinySwap(Cabinets[i], Cabinets[j + 1]);
				}
			}
			else
			{
				if (mCabinets[Cabinets[i]].mRes.GetMinScale() < mCabinets[Cabinets[j + 1]].mRes.GetMinScale())
				{
					TinySwap(Cabinets[i], Cabinets[j + 1]);
				}
			}
		}
	}

	for (int32 i = 0; i < Cabinets.Num(); ++i)
	{
		float RefScale = nTotalLen / ((float)nBaseSize);
		FCabinetSet& cabinet = mCabinets[Cabinets[i]];
		RefScale = FMath::CeilToInt(cabinet.mRes.mSize.X*RefScale) / cabinet.mRes.mSize.X;
		if (cabinet.mRes.mSize.X*RefScale < cabinet.mRes.mSizeMin.X)
		{
			RefScale = cabinet.mRes.mSizeMin.X / ((float)cabinet.mRes.mSize.X);
		}
		else if(cabinet.mRes.mSize.X*RefScale > cabinet.mRes.mSizeMax.X)
		{
			RefScale = cabinet.mRes.mSizeMax.X / ((float)cabinet.mRes.mSize.X);
		}
		cabinet.mScale.X = RefScale;
		nTotalLen -= cabinet.mRes.mSize.X*cabinet.mScale.X;
		nBaseSize -= cabinet.mRes.mSize.X;
	}
	return true;
}

FCabinetSet& TSetEdge::SetCabinetPos(FCabinetSet::eAlignType eAlign, FCabinetSet::eFillType eFillType, FCabinetRes Res, float fScaleX, float fScaleY)
{
	FCabinetSet CabinetSet;
	CabinetSet.mId = gCabinetMgr.GenId();
	CabinetSet.mAlign = eAlign;
	CabinetSet.mFillType = eFillType;
	CabinetSet.mRes = Res;
	CabinetSet.mScale.X = fScaleX;
	CabinetSet.mScale.Y = fScaleY;
	return SetCabinetPos(CabinetSet);
}
FCabinetSet & TSetEdge::SetCabinetVer(FCabinetSet::eAlignType eAlign, FCabinetRes Res, int32 XLen)
{
	FCabinetSet CabinetSet;
	if ( eAlign == FCabinetSet::eAlignHead )
	{
		CabinetSet.mPosLocal = mHead + Res.mSize.Y*0.5f;
		CabinetSet.mDir = Rote270(mDir);
		mHead += Res.mSize.Y;
	}
	else if ( eAlign == FCabinetSet::eAlignTail )
	{
		CabinetSet.mPosLocal = mTail - Res.mSize.Y*0.5f;
		CabinetSet.mDir = Rote90(mDir);
		mTail -= Res.mSize.Y;
	}
	CabinetSet.mAlign = eAlign;
	CabinetSet.mFillType = FCabinetSet::eFillFix;
	CabinetSet.mRes = Res;

	if (XLen < Res.mSizeMin.X)
		XLen = Res.mSizeMin.X;
	else if (XLen > Res.mSizeMax.X)
		XLen = Res.mSizeMax.X;

	CabinetSet.mScale.X = XLen/Res.mSize.X;
	CabinetSet.mPos = mStart->GetVector() + CabinetSet.mPosLocal*mDir  + mNor * XLen / 2;
	CabinetSet.mId = gCabinetMgr.GenId();
	mCabinets.Add(CabinetSet);
	return mCabinets.Top();
}
FCabinetSet& TSetEdge::SetCabinetPos(FCabinetSet& CabinetSet )
{
	if ( CabinetSet.mFillType== FCabinetSet::eFillFull)
	{
		CabinetSet.mScale.X = GetRemainLen() / CabinetSet.mRes.mSize.X;
	}

	if (CabinetSet.mFillType == FCabinetSet::eFillMax )
	{
		float fSize = (GetRemainLen() < CabinetSet.mRes.mSizeMax.X) ? GetRemainLen() : CabinetSet.mRes.mSizeMax.X;
		CabinetSet.mScale.X = fSize / CabinetSet.mRes.mSize.X;
	}
	if (CabinetSet.mScale.X* CabinetSet.mRes.mSize.X < CabinetSet.mRes.mSizeMin.X)
	{
		//CabinetSet.mScale.X = CabinetSet.mRes.mSizeMin.X/ CabinetSet.mRes.mSize.X;
		//CabinetSet.mRes.mTeplateId = -1;
	}
	if (CabinetSet.mScale.X* CabinetSet.mRes.mSize.X > CabinetSet.mRes.mSizeMax.X)
	{
		//CabinetSet.mScale.X = CabinetSet.mRes.mSizeMax.X / CabinetSet.mRes.mSize.X;
		//CabinetSet.mRes.mTeplateId = -1;
	}
	int32 nHalfLen = CabinetSet.mRes.mSize.X*CabinetSet.mScale.X*0.5f;
	if (CabinetSet.mAlign == FCabinetSet::eAlignTail)
	{
		CabinetSet.mPosLocal = mTail - nHalfLen;
		mTail -= nHalfLen *2;
	}
	else if ( CabinetSet.mAlign == FCabinetSet::eAlignHead)
	{
		CabinetSet.mPosLocal = mHead + nHalfLen; 
		mHead += nHalfLen * 2;
	}
	else if ( CabinetSet.mAlign == FCabinetSet::eAlignCentry)
	{
		CabinetSet.mPosLocal = GetRemainLen()*0.5f;
		mHead = mTail;
	}
	CabinetSet.mPos = mStart->GetVector() + CabinetSet.mPosLocal*mDir + mNor * CabinetSet.mRes.mSize.Y*CabinetSet.mScale.Y / 2;
	CabinetSet.mDir = mDir;
	CabinetSet.mId  = gCabinetMgr.GenId();
	mCabinets.Add(CabinetSet);
	return mCabinets.Top();
}
bool TSetEdge::FillDist(float len, bool bHead)
{
	if (bHead)
		mHead += len;
	else
		mTail -= len;
	return true;
}

bool TSetEdge::FillFake(float len, bool bHead)
{
	if ( bHead )
	{
		SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillDefault, FCabinetRes(len));
	}
	else
	{
		SetCabinetPos(FCabinetSet::eAlignTail, FCabinetSet::eFillDefault, FCabinetRes(len));
	}
	return true;
}

bool TSetEdge::FillFake(float len, FCabinetSet::eAlignType eAlign)
{
	SetCabinetPos(eAlign, FCabinetSet::eFillDefault, FCabinetRes(len));
	return true;
}

bool TSetEdge::SetFillDist(float len, bool bHead)
{
	if (!bHead)
		mTail = len;
	else
		mHead = (mLen-len);
	return true;
}
bool TSetEdge::Init()
{
	mLen = FVector2D(mEnd->x - mStart->x, mEnd->y - mStart->y).Size();
	mHead = 0;
	mTail = mLen;
	mSetRet = 0;
	return true;
}

int32 TSetEdge::GetCabinet(FCabinetSet::eAlignType eAlign)
{
	if (eAlign == FCabinetSet::eAlignHead)
	{
		for (int32 i =0; i< mCabinets.Num(); --i)
		{
			if (mCabinets[i].mAlign == eAlign)
				return i;
		}
	}
	if (eAlign == FCabinetSet::eAlignTail)
	{
		for (int32 i = mCabinets.Num() - 1; i >= 0; --i)
		{
			if (mCabinets[i].mAlign == eAlign)
				return i;
		}
	}
	return -1;
}