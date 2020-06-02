
#include "FrameEdge.h"
#include "../base/common.h"
namespace PolyEdgeLib
{
	TPoint::TPoint(float _x, float _y)
	{
		x = INT32((_x + (_x > 0 ? .5f : -.5f)));
		y = INT32((_y + (_y > 0 ? .5f : -.5f)));
		Id = (INT64(x) << 32) + y;
	}
	TPoint::TPoint(FVector2D& Pos)
	{
		x = INT32((Pos.X + (Pos.X > 0 ? .5f : -.5f)));
		y = INT32((Pos.Y + (Pos.Y > 0 ? .5f : -.5f)));
		Id = (INT64(x) << 32) + y;
	}
	bool TPoint::IsHor()
	{ 
		return GetType() == eCorner180;
	}
	bool TPoint::IsTrun()
	{
		return GetType() != eCorner180;
	}
	bool TPoint::IsTurn90()
	{
		return GetType()== eCorner90;
	}
	bool TPoint::IsTurn270()
	{
		return GetType()== eCorner270;
	}
	eCornerType TPoint::GetType()
	{
		if (mPreEdge == NULL)
			return eCorner0;
		if (mNextEdge == NULL)
			return eCorner360;
		if ((-mNextEdge->mDir).Equals(mPreEdge->mNor, 0.1f))
			return eCorner270;
		if ((mNextEdge->mDir).Equals(mPreEdge->mDir, 0.1f))
			return eCorner180;
		if ((mPreEdge->mNor).Equals(mNextEdge->mDir, 0.1f))
			return eCorner90;
		return eCornerNull;
	}
	float TPoint::Dist(TPoint* pPoint)
	{
		if (pPoint == nullptr)
			return FLT_MAX;
		return FVector2D(x - pPoint->x, y - pPoint->y).Size();
	}
	TPoint* TPoint::NextPoint()
	{
		return mNextEdge->mEnd;
	}
	TPoint* TPoint::PrePoint()
	{
		return mPreEdge->mStart;
	}
	//////////////////////////////////////////////////////////////////////////
	TEdge::TEdge()
	{
		mPoly = nullptr;
	}
	bool TEdge::InitPoint(TPoint* pStart, TPoint* pEnd)
	{
		mStart = pStart;
		mStart->mNextEdge = this;
		mEnd = pEnd;
		mEnd->mPreEdge = this;
		Id = IdId2Str(mStart->Id, mEnd->Id);
		mDir = FVector2D(mEnd->x - mStart->x, mEnd->y - mStart->y);
		mDir.Normalize();
		mNor = FVector2D(-mDir.Y, mDir.X);
		return true;
	}
	float TEdge::GetLen()
	{
		return FVector2D::Distance(mStart->GetVector(), mEnd->GetVector());
	}
	bool TEdge::IsState(uint32 nBit, uint32 nExpactBit)
	{
		return (((mBit&nBit)==nBit) && !(mBit&nExpactBit));
	}
	TEdge* TEdge::FirstEdge(uint32 nBit, uint32 nExpactBit)
	{
		TEdge* pEdge = this;
		for (; pEdge->PreEdge()->IsState(nBit, nExpactBit) && pEdge->mStart->IsHor(); pEdge = pEdge->PreEdge() )
		{
		}
		return pEdge;
	}
	TEdge* TEdge::LastEdge(uint32 nBit, uint32 nExpactBit)
	{
		TEdge* pEdge = this;
		for (; pEdge->NextEdge()->IsState(nBit, nExpactBit) && pEdge->mEnd->IsHor(); pEdge = pEdge->NextEdge() )
		{
		}
		return pEdge;
	}
	TEdge* TEdge::NextFirstEdge(uint32 nBit, uint32 nExpactBit)
	{
		for (TEdge* pEdge = this; ; pEdge = pEdge->NextEdge() )
		{
			if (pEdge->IsState(nBit, nExpactBit))
				return pEdge;
			if (!pEdge->mEnd->IsHor())
				return nullptr;
		}
		return nullptr;
	}
	TEdge* TEdge::PreFirstEdge(uint32 nBit, uint32 nExpactBit)
	{
		for (TEdge* pEdge = this; ; pEdge = pEdge->PreEdge())
		{
			if (pEdge->IsState(nBit, nExpactBit))
				return pEdge;
			if (!pEdge->mStart->IsHor())
				return nullptr;
		}
		return nullptr;
	}
	TPoint* TEdge::PreTurnPoint()
	{
		for (TEdge* pEdge = this; ; pEdge = pEdge->PreEdge())
		{
			if (!pEdge->mStart->IsHor())
				return pEdge->mStart;
		}
		return nullptr;
	}
	TPoint* TEdge::NextTurnPoint()
	{
		for (TEdge* pEdge = this; ; pEdge = pEdge->NextEdge())
		{
			if (!pEdge->mEnd->IsHor())
				return pEdge->mEnd;
		}
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TSegment::Init(TPoint * pStart, TPoint * pEnd)
	{
		mStart = pStart;
		mEnd = pEnd;
		mPreSeg = nullptr;
		mNextSeg = nullptr;
		mDir = pStart->mNextEdge->mDir;
		mNor = pStart->mNextEdge->mNor;
		mBit = 0;
		for (TPoint* p = mStart; p != mEnd; p = p->NextPoint())
			mBit |= p->mNextEdge->mBit;
		Init();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	CPolyMgr::CPolyMgr()
	{
		mMinPointDistMerge = 10;
		mMinLineDistMerge = 10;
		mMinLineDistMerge2 = 10;
		mMainPoly = nullptr;
	}
	CPolyMgr::~CPolyMgr()
	{
		Clear();
	}
	bool CPolyMgr::Clear()
	{
		mId = 0;
		for ( auto p: mPoints )
		{
			delete(p.Value);
		}
		mPoints.Empty();

		for (auto p : mEdges)
		{
			delete(p.Value);
		}
		mEdges.Empty();

		for (auto p : mPolys)
		{
			delete(p.Value);
		}
		mPolys.Empty();
		SAFE_DELETE(mMainPoly);

		for ( auto p: mSegments)
		{
			delete p;
		}
		mSegments.Empty();
		return true;
	}
	TBox* CPolyMgr::AddPoly(FVector2D vPos, FVector2D vSizeHalf, FVector2D vDir, uint32 nBit)
	{
		TBox* pBox = new TBox;
		pBox->mPos = vPos;
		pBox->mSizeHalf = vSizeHalf;
		pBox->mDir = vDir;
		pBox->Id = ++mId;
		pBox->mBit = nBit;
		InitBox(pBox);
		return pBox;
	}
	//////////////////////////////////////////////////////////////////////////
	TPoint* CPolyMgr::FindPoint(FVector2D& Pos, bool bPrecise)
	{
		TPoint p(Pos);
		TPoint** pRet = mPoints.Find(p.Id);
		if (pRet)
			return *pRet;
		if (!bPrecise)
		{
			for (auto pIndex : mPoints)
			{
				if (pIndex.Value->GetVector().Equals(Pos, mMinPointDistMerge))
					return pIndex.Value;
			}
		}
		return nullptr;
	}
	TPoint* CPolyMgr::NewPoint(FVector2D& Pos, bool bPrecise)
	{
		TPoint* pRet = FindPoint(Pos);
		if (pRet)
			return pRet;
		pRet = 	new	TPoint(Pos);
		mPoints.Add(pRet->Id, pRet);
		return pRet;
	}
	TPoint* CPolyMgr::NewPointLocal(FVector2D& Anchor, FVector2D& Dir, FVector2D& Scale, FVector2D& LocalPos)
	{
		FVector2D Pos = LocalPos * Scale;
		Pos = Anchor + Pos.X*Dir + Pos.Y* FVector2D(-Dir.Y, Dir.X);
		return NewPoint(Pos);
	}
	TEdge* CPolyMgr::FindEdge(TPoint* pStart, TPoint* pEnd)
	{
		FString Id = IdId2Str(pStart->Id, pEnd->Id);
		TEdge** pRet = mEdges.Find(Id);
		if (pRet)
		{
			return *pRet;
		}
		return nullptr;
	}
	TEdge*  CPolyMgr::NewEdge(FVector2D StartPos, FVector2D EndPos, TPoly* pPoly, int32 nState)
	{
		TPoint* pStart = NewPoint(StartPos);
		TPoint* pEnd = NewPoint(EndPos);
		return NewEdge(pStart, pEnd, pPoly, nState);
	}
	TEdge* CPolyMgr::NewEdge(TPoint* pStart, TPoint* pEnd, TPoly* pPoly, int32 nState)
	{
		CHECK_ERROR(pStart != pEnd);
		TEdge* pEdge = FindEdge(pStart, pEnd);
		if (pEdge == nullptr)
		{
			pEdge = new TEdge;
			pEdge->Id = IdId2Str(pStart->Id, pEnd->Id);
			pEdge->mBit = nState;
			pEdge->mStart = pStart;
			pEdge->mEnd = pEnd;

			pEdge->mDir = FVector2D(pEdge->mEnd->x - pEdge->mStart->x, pEdge->mEnd->y - pEdge->mStart->y);
			pEdge->mDir.Normalize();
			pEdge->mNor = FVector2D(-pEdge->mDir.Y, pEdge->mDir.X);
			mEdges.Add(pEdge->Id, pEdge);
		}
		if ( pPoly )
		{
			if (pPoly == mMainPoly)
			{
				pStart->mNextEdge = pEdge;
				pEnd->mPreEdge = pEdge;
			}
			pEdge->mBit |= pPoly->mBit;
			pEdge->mPoly = pPoly;
			pPoly->mEdges.Add(pEdge);
		}
		pEdge->mBit |= nState;
		return pEdge;
	}
	bool CPolyMgr::InitPoly(TPoly* pPoly)
	{
		int NumPath = pPoly->mPath.Num();
		for (int i = 0; i < NumPath; ++i)
		{
			NewEdge(pPoly->mPath[i], pPoly->mPath[(i + 1) % NumPath], pPoly);
		}
		if ( pPoly != mMainPoly )
		{
			mPolys.Add(pPoly->Id, pPoly);
		}
		return true;
	}
	bool CPolyMgr::InitBox(TBox* pBox)
	{
		FVector2D pos0 = pBox->mSizeHalf.X*pBox->mDir;
		FVector2D pos1 = pBox->mSizeHalf.Y* FVector2D(-pBox->mDir.Y, pBox->mDir.X);
		FVector2D Pos[4] = { pBox->mPos + pos0 - pos1,pBox->mPos + pos0 + pos1,pBox->mPos -pos0 + pos1,pBox->mPos - pos0 - pos1 };
		for (int i = 0; i < 4; ++i)
		{
			pBox->mPath.Add(NewPoint(Pos[i]));
		}
		return InitPoly(pBox);
	}
	TPoly* CPolyMgr::AddPoly(FVector2D Pos, FVector2D Dir, FVector2D Scale, TArray<FVector2D>& Path, uint32 nBit)
	{
		TPoly* pPoly = new TPoly();
		pPoly->mBit = nBit;
		pPoly->Id = ++mId;

		int NumPath = Path.Num();
		for (int i = 0; i < NumPath; ++i)
		{
			pPoly->mPath.Add(NewPointLocal(Pos,Dir,Scale,Path[i]));
		}
		InitPoly(pPoly);
		return pPoly;
	}
	TPoly* CPolyMgr::AddPoly(TArray<TLine2d>& Path, uint32 nBit, bool bMain)
	{
		CHECK_ERROR(Path.Num() > 0);
		TPoly* pPoly = new TPoly();
		pPoly->mBit = nBit;
		pPoly->Id = ++mId;
		if ( bMain)
			mMainPoly = pPoly;
		else
			mPolys.Add(pPoly->Id, pPoly);

		int NumPath = Path.Num();
		for (int i = 0; i < NumPath; ++i)
		{
			pPoly->mPath.Add(NewPoint(Path[i].mStart));
		}
		for (auto p: Path )
		{
			TEdge* pEdge = NewEdge(p.mStart, p.mEnd, pPoly,p.mState);
			if(p.Id!=-1)
				pEdge->mId = p.Id;
		}
		return pPoly;
	}
	TPoly* CPolyMgr::AddPoly(TMap<int, TLine2d>& Path, uint32 nBit, bool bMain)
	{
		CHECK_ERROR(Path.Num() > 0);
		TPoly* pPoly = new TPoly();
		pPoly->mBit = nBit;
		pPoly->Id = ++mId;

		if (bMain)
			mMainPoly = pPoly;
		else
			mPolys.Add(pPoly->Id, pPoly);

		int NumPath = Path.Num();
		for (auto p: Path)
		{
			pPoly->mPath.Add(NewPoint(p.Value.mStart));
		}
		for (auto p : Path)
		{
			TEdge* pEdge = NewEdge(p.Value.mStart, p.Value.mEnd, pPoly, p.Value.mState);
			if (pEdge)
				pEdge->mId = p.Value.Id;
		}
		return pPoly;
	}
	bool CPolyMgr::AddPoly(TArray<TRectBase>& Rects, uint32 nBit)
	{
		for (auto p: Rects)
		{
			TPoly* pPoly = AddPoly(p.mPos, p.mSize*p.mScale, p.mDir,nBit);
			pPoly->mHeight = p.mHeight;
			pPoly->mPosZ = p.mPosZ;
		}
		return true;
	}
	TPoly* CPolyMgr::AddPoly(TRectBase* pRect, uint32 nBit)
	{
		CHECK_ERROR(pRect);
		TPoly* pPoly = AddPoly(pRect->mPos, pRect->mSize*pRect->mScale, pRect->mDir, nBit);
		pPoly->mHeight = pRect->mHeight;
		pPoly->mPosZ = pRect->mPosZ;
		return pPoly;
	}
	TPoly* CPolyMgr::AddPoly(TArray<FVector2D>& Path, uint32 nBit, bool bMain)
	{
		CHECK_ERROR(Path.Num() > 0);
		TPoly* pPoly = new TPoly();
		pPoly->mBit = nBit;
		pPoly->Id = ++mId;
		if (bMain)
			mMainPoly = pPoly;
		else
			mPolys.Add(pPoly->Id, pPoly);

		int NumPath = Path.Num();
		for (int i = 0; i < NumPath; ++i)
		{
			pPoly->mPath.Add(NewPoint(Path[i]));
		}
		InitPoly(pPoly);
		return pPoly;
	}
	bool CPolyMgr::AddPoly(TMap<int, TLine2d>& Path, uint32 nBit, int BiasLen)
	{
		CHECK_ERROR(Path.Num() > 0);
		TPoly* pPoly = new TPoly();
		pPoly->mBit = nBit;
		pPoly->Id = ++mId;
		for (auto p : Path)
		{
			AddPoly(p.Value, nBit, pPoly, BiasLen);
		}
		mPolys.Add(pPoly->Id, pPoly);
		return true;
	}
	bool CPolyMgr::AddPoly(TLine2d& Line, uint32 nBit, TPoly* pPoly, int BiasLen)
	{
		if ( BiasLen >0 )
		{
			FVector2D CrossPos0;
			float fDot0;
			FVector2D CrossPos1;
			float fDot1;
			for (auto p : mEdges)
			{
				TEdge* pSeg = p.Value;
				FVector2D vStart = pSeg->mStart->GetVector();
				FVector2D vEnd = pSeg->mEnd->GetVector();
				if (Point2LineSeg(vStart, vEnd, Line.mStart, fDot0, CrossPos0) < BiasLen && Point2LineSeg(vStart, vEnd, Line.mEnd, fDot1, CrossPos1) < BiasLen)
				{
					if (fDot0 >= 0 && fDot0 <= 1 && fDot1 >= 0 && fDot1 <= 1)
					{
						NewEdge(CrossPos0, CrossPos1, pPoly, Line.mState);
						return true;
					}
				}
			}
		}
		else
		{
			NewEdge(Line.mStart, Line.mEnd, pPoly, Line.mState);
		}
		return false;
	}
	bool CPolyMgr::SignFrame()
	{
		for (auto p : mPolys )
		{
			TArray<TEdge*>& pEdges = p.Value->mEdges;
			for ( auto E:pEdges)
			{
				SignOnFrame(E);
			}
		}
		return true;
	}
	TEdge*	CPolyMgr::SignOnFrame(TEdge* pEdgeFrom)
	{
		TArray<TEdge *> Edges;
		GetEdges(Edges, mMainPoly->mBit);
		for (auto p:Edges)
		{
			if (SliptEdge(p, pEdgeFrom))
				return p;
		}
		return NULL;
	}
	bool CPolyMgr::SliptEdge(TEdge* pEdgeFrame, TEdge* pEdgeOther)
	{
		if (pEdgeFrame == pEdgeOther)
			return true;
		if (FMath::Abs(pEdgeFrame->mDir | pEdgeOther->mDir) < 0.9f)
			return false;

		FVector2D PosStart = pEdgeFrame->mStart->GetVector();
		FVector2D PosEnd = pEdgeFrame->mEnd->GetVector();

		FVector2D Pos0 = pEdgeOther->mStart->GetVector();
		FVector2D Pos1 = pEdgeOther->mEnd->GetVector();

		FVector2D CrossStart;
		float fDot0;
		float fDistance0 = Point2LineSegVer(PosStart, PosEnd, Pos0, fDot0, CrossStart);
		if (pEdgeOther->IsState(4))
		{
			CHECK_ERROR(fDistance0 <= mMinLineDistMerge2);
		}
		else
		{
			CHECK_ERROR(fDistance0 < mMinLineDistMerge);
		}

		FVector2D CrossEnd;
		float fDot1;
		float fDistance1 = Point2LineSegVer(PosStart, PosEnd, Pos1, fDot1, CrossEnd);	
		if (pEdgeOther->IsState(4))
		{
			CHECK_ERROR(fDistance1 <= mMinLineDistMerge2);
		}
		else
		{
			CHECK_ERROR(fDistance1 < mMinLineDistMerge);
		}

		CHECK_ERROR(FMath::Abs(fDistance0 - fDistance1) < mMinLineDistMerge);

		float fMinDot = fDot0;
		float fMaxDot = fDot1;
		if (fDot0 > fDot1 )
		{
			TinySwap(fDot0, fDot1);
			TinySwap(fDot0, fDot1);
			TinySwap(fMinDot, fMaxDot);
			TinySwap(CrossStart, CrossEnd);
		}
		if (fMinDot >= 1)
			return false;
		if (fMaxDot <=0)
			return false;
		if (FVector2D::Distance(PosStart, CrossStart)< mMinLineDistMerge2)//mMinLineDistMerge)
		{
			CrossStart = PosStart;
			fMinDot = 0;
			fDot0 = 0;
		}
		if (FVector2D::Distance(PosEnd, CrossEnd) < mMinLineDistMerge2)//mMinLineDistMerge)
		{
			CrossEnd = PosEnd;
			fMaxDot = 1;
			fDot1 = 1;
		}
		bool bInLine = (fDot0 >= -0.00001f && fDot1 <= 1.00001f);

		if ( fMinDot <= 0 && fMaxDot >= 1 )
		{
			pEdgeFrame->mBit |= pEdgeOther->mBit;
			pEdgeFrame->mPoly = pEdgeOther->mPoly;
			return bInLine;
		}
		TPoint* pStart = NewPoint(CrossStart);
		TPoint* pEnd = NewPoint(CrossEnd);
		CHECK_ERROR(pStart != pEnd);

		if ( fMinDot <= 0 )
		{
			TEdge* pEdge = NewEdge(pEdgeFrame->mStart, pEnd, pEdgeFrame->mPoly, pEdgeOther->mBit | pEdgeFrame->mBit);
			pEdge->mPoly = pEdgeOther->mPoly;
			pEdge->InitPoint(pEdgeFrame->mStart, pEnd);
			mEdges.Remove(pEdgeFrame->Id);
			pEdgeFrame->InitPoint(pEnd, pEdgeFrame->mEnd);
			mEdges.Add(pEdgeFrame->Id,pEdgeFrame);
			return bInLine;
		}
		if (fMaxDot >= 1)
		{
			TEdge* pEdge = NewEdge(pStart, pEdgeFrame->mEnd, pEdgeFrame->mPoly, pEdgeOther->mBit | pEdgeFrame->mBit );
			pEdge->mPoly = pEdgeOther->mPoly;
			pEdge->InitPoint(pStart, pEdgeFrame->mEnd);
			mEdges.Remove(pEdgeFrame->Id);
			pEdgeFrame->InitPoint(pEdgeFrame->mStart, pStart);
			mEdges.Add(pEdgeFrame->Id, pEdgeFrame);
			return bInLine;
		}
		else
		{
			TEdge* pEdgeStart = NewEdge(pStart, pEnd, pEdgeFrame->mPoly, pEdgeOther->mBit | pEdgeFrame->mBit);
			pEdgeStart->InitPoint(pStart, pEnd);

			pEdgeStart->mPoly = pEdgeOther->mPoly;
			pEdgeStart->InitPoint(pStart, pEnd);

			TEdge* pEdge = NewEdge(pEnd, pEdgeFrame->mEnd, pEdgeFrame->mPoly, pEdgeFrame->mBit);
			pEdge->InitPoint(pEnd, pEdgeFrame->mEnd);

			mEdges.Remove(pEdgeFrame->Id);
			pEdgeFrame->InitPoint(pEdgeFrame->mStart, pStart);
			mEdges.Add(pEdgeFrame->Id, pEdgeFrame);
			return true;
		}
		return false;
	}
	bool CPolyMgr::SignOnFrame(TLine2d & pEdgeFrom)
	{
		return false;
	}
	bool CPolyMgr::SliptEdge(TLine2d & pEdgeFrom, TEdge * pEdgeOn)
	{
		return false;
	}
	TSegment * CPolyMgr::NewSegment(TPoint * pStart, TPoint * pEnd)
	{
		CHECK_ERROR(pStart != pEnd);
		TSegment* pSegment = new TSegment;
		pSegment->Init(pStart, pEnd);
		mSegments.Add(pSegment);
		return pSegment;
	}
	int CPolyMgr::GetEdges(TArray<TEdge *>& Edges, uint32 nBit, uint32 nExpactBit)
	{
		Edges.Empty();
		for ( auto p: mEdges )
		{
			if ( p.Value->IsState(nBit, nExpactBit) )
				Edges.Add(p.Value);
		}
		return Edges.Num();
	}
	TEdge* CPolyMgr::GetEdge(uint32 nBit, uint32 nExpactBit)
	{
		for ( auto p: mEdges )
		{
			if (p.Value->IsState(nBit, nExpactBit))
				return p.Value;
		}
		return nullptr;
	}
	TSegment* CPolyMgr::GetFirstSegment(uint32 nBit, uint32 nExpactBit)
	{
		TEdge* pEdge = GetEdge(nBit| eSliptWall, nExpactBit);
		CHECK_ERROR(pEdge);
		TPoint* pStar = pEdge->FirstEdge(nBit, nExpactBit)->mStart;
		TPoint* pEnd = pEdge->LastEdge(nBit, nExpactBit)->mEnd;
		TSegment* pStartSeg = NewSegment(pStar, pEnd);
		return pStartSeg;
	}
	TSegment* CPolyMgr::GetSegments(uint32 nBit, uint32 nExpactBit)
	{
		TSegment* pStartSeg = GetFirstSegment( nBit, nExpactBit);
		CHECK_ERROR(pStartSeg);
		for (TSegment* pNowSeg = pStartSeg; true; )
		{
			TSegment* pSeg = nullptr;
			TPoint* pPointEnd = nullptr;
			TEdge* pEdge = pNowSeg->NextEdge()->NextFirstEdge(nBit, nExpactBit);
			if ( pEdge == nullptr )
			{
				if ( pNowSeg->mEnd->IsTrun() )
					pPointEnd = pNowSeg->NextEdge()->NextTurnPoint();
				else
					pPointEnd = pNowSeg->TailEdge()->NextTurnPoint();
			}
			else
			{
				pPointEnd = pEdge->mStart;
			}
			if (pNowSeg->mEnd != pPointEnd)
			{
				TSegment* pSegTmp = NewSegment(pNowSeg->mEnd, pPointEnd);
				pSegTmp->mPreSeg = pNowSeg;
				pNowSeg->mNextSeg = pSegTmp;
				pNowSeg = pSegTmp;
			}
			if (pNowSeg->mEnd == pStartSeg->mStart)
			{
				pNowSeg->mNextSeg = pStartSeg;
				pStartSeg->mPreSeg = pNowSeg;
				break;
			}
			if ( pEdge )
			{
				pEdge = pEdge->LastEdge(nBit, nExpactBit);
				pSeg = NewSegment(pNowSeg->mEnd, pEdge->mEnd);
				pSeg->mPreSeg = pNowSeg;
				pNowSeg->mNextSeg = pSeg;
				pNowSeg = pSeg;
			}
		}
		return pStartSeg;
	}
	TSegment * CPolyMgr::GetFirstSegment(TSegment * pSeg,uint32 nBit, uint32 nExpactBit )
	{
		for (TSegment* pStartSeg = pSeg; true; )
		{
			if (pStartSeg->IsState(nBit, nExpactBit) && !pStartSeg->mPreSeg->IsState(nBit, nExpactBit) )
			{
				return pStartSeg;
			}
			pStartSeg = pStartSeg->mNextSeg;
			if (pStartSeg == pSeg)
				return nullptr;
		}
		return nullptr;
	}
	TSegment * CPolyMgr::GetLastSegment(TSegment * pSeg, uint32 nBit, uint32 nExpactBit )
	{
		for (TSegment* pStartSeg = pSeg; true; )
		{
			if (pStartSeg->IsState(nBit, nExpactBit) && !pStartSeg->mNextSeg->IsState(nBit, nExpactBit))
			{
				return pStartSeg;
			}
			pStartSeg = pStartSeg->mNextSeg;
			if (pStartSeg == pSeg)
				return nullptr;
		}
		return nullptr;
	}
	bool CPolyMgr::GetOutLine(TArray<FVector2D>& OutLine, int nExtern, uint32 nBit, uint32 nExpactBit)
	{
		OutLine.Empty();
		TSegment* pStartSeg = GetSegments(nBit, nExpactBit);
		CHECK_ERROR(pStartSeg);
		TSegment* pHeadSeg = GetFirstSegment(pStartSeg, nBit, nExpactBit);
		CHECK_ERROR(pHeadSeg);
		TSegment* pTailSeg = GetLastSegment(pStartSeg, nBit, nExpactBit);
		CHECK_ERROR(pTailSeg);

		for (TSegment* pSeg = pHeadSeg; true; pSeg = pSeg->mNextSeg )
		{
			FVector2D Pos = pSeg->mStart->GetVector();
			if (pSeg==pHeadSeg)
			{
				Pos -= pSeg->mNor*nExtern;
			}
			else
			{
				Pos -= (pSeg->mNor*nExtern + pSeg->mPreSeg->mNor*nExtern);
			}
			OutLine.Add(Pos);
			if (pSeg == pTailSeg)
			{
				Pos = pSeg->mEnd->GetVector() - pSeg->mNor*nExtern;
				OutLine.Add(Pos);
				break;
			}
		}
		return true;
	}

	bool CPolyMgr::GetOutLine(TArray<FVector2D>& OutLineAll, TArray<FVector2D>& PathFloorBottom, TArray<FVector2D>& PathOnWall, TArray<FVector2D>& PathFront, int nExtern, int nExternBotom, uint32 nBit, uint32 nExpactBit)
	{
		OutLineAll.Empty();
		PathOnWall.Empty();
		PathFloorBottom.Empty();
		PathFront.Empty();

		TSegment* pStartSeg = GetSegments(nBit, nExpactBit);
		CHECK_ERROR(pStartSeg);
		TSegment* pHeadSeg = GetFirstSegment(pStartSeg, nBit, nExpactBit);
		CHECK_ERROR(pHeadSeg);
		TSegment* pTailSeg = GetLastSegment(pStartSeg, nBit, nExpactBit);
		CHECK_ERROR(pTailSeg);

		FVector2D Pos;
		for (TSegment* pSeg = pHeadSeg; true; pSeg = pSeg->mNextSeg)
		{
			Pos = pSeg->mStart->GetVector();
			if (pSeg == pHeadSeg)
			{
				FVector2D PosBottom = Pos-pSeg->mNor*nExternBotom;
				PathFloorBottom.Add(PosBottom);
				Pos -= pSeg->mNor*nExtern;
				PathFront.Add(Pos);
			}
			else
			{
				FVector2D PosBottom = Pos - (pSeg->mNor+pSeg->mPreSeg->mNor)*nExternBotom;
				PathFloorBottom.Add(PosBottom);
				Pos -= (pSeg->mNor+pSeg->mPreSeg->mNor)*nExtern;
				PathFront.Add(Pos);
			}
			OutLineAll.Add(Pos);
			if (pSeg == pTailSeg)
			{
				FVector2D PosBottom = pSeg->mEnd->GetVector() - pSeg->mDir*nExternBotom;
				PathFloorBottom.Add(PosBottom);

				Pos = pSeg->mEnd->GetVector() - pSeg->mNor*nExtern;
				PathFront.Add(Pos);

				OutLineAll.Add(Pos);
				break;
			}
		}
		TSegment* pRemainHead = pTailSeg->mNextSeg;
		TSegment* pRemainTail = pHeadSeg->mPreSeg;
		PathOnWall.Add(Pos);
		for (TSegment* pSeg = pRemainHead; pSeg != pRemainTail; pSeg = pSeg->mNextSeg)
		{
			FVector2D PosTmp = pSeg->mEnd->GetVector();
			PathOnWall.Add(PosTmp);
			OutLineAll.Add(PosTmp);
		}
		PathOnWall.Add(OutLineAll[0]);
		return true;
	}
	
	bool CPolyMgr::AdjustFrame(TMap<int, TLine2d>& RawAdjustWalls)
	{
		RawAdjustWalls.Empty();
		for (auto p : mEdges)
		{
			TLine2d AdjustLine;
			AdjustLine.mStart = p.Value->mStart->GetVector();
			AdjustLine.mEnd = p.Value->mEnd->GetVector();
			AdjustLine.mState = p.Value->mBit;
			AdjustLine.Id = p.Value->mId;
			RawAdjustWalls.Add(AdjustLine.Id, AdjustLine);
		}
		TPoint* pStarPoint = NULL;
		for (TPoint* Cur = mPoints.CreateIterator().Value(); Cur != pStarPoint;)
		{
			TPoint* pNext = Cur->NextPoint();
			if (Cur->GetType() == eCorner270 && Cur->mPreEdge->GetLen() <= 550 && Cur->mPreEdge->mBit == Cur->mNextEdge->mBit &&Cur->mNextEdge->mBit==pNext->mNextEdge->mBit )
			{
				if (pNext->mNextEdge->GetLen() <= 550 && pNext->GetType() == eCorner270)
				{
					RawAdjustWalls[Cur->mNextEdge->mId].mStart = Cur->PrePoint()->GetVector();
					RawAdjustWalls[Cur->mNextEdge->mId].mEnd = pNext->NextPoint()->GetVector();
					Cur = pNext->mNextEdge->mEnd;
					continue;
				}
				else if (Cur->mNextEdge->GetLen() <= 550 && pNext->GetType() == eCorner90)
				{
					FVector2D pos = Cur->mPreEdge->mStart->GetVector() + (pNext->GetVector() - Cur->GetVector());

					RawAdjustWalls[Cur->mPreEdge->mStart->mPreEdge->mId].mEnd = pos;
					RawAdjustWalls[pNext->mNextEdge->mId].mStart = pos;
					Cur = pNext;
					continue;
				}
			}
			if (pStarPoint == NULL)
				pStarPoint = Cur;
			Cur = pNext;
		}
		return true;
	}
	bool ChangeEnd(TEdge* pEdge, TPoint* pEnd)
	{
		pEnd->mPreEdge = pEdge;
		pEdge->mEnd = pEnd;
		return true;
	}
	bool ChangeStart(TEdge* pEdge, TPoint* pStart)
	{
		pStart->mNextEdge = pEdge;
		pEdge->mStart = pStart;
		return true;
	}
	bool CPolyMgr::CombitFrame(TArray<TLine2d>& OutPaths, TArray< TRectBase>& Pillars)
	{
		OutPaths.Empty();
		Pillars.Empty();
		TPoint* pStarPoint = NULL;
		for (TPoint* Cur = mPoints.CreateIterator().Value(); true; Cur = Cur->NextPoint())
		{
			if (Cur->GetType() != eCorner180 && Cur->mPreEdge->GetLen() > 550 && Cur->mNextEdge->GetLen() > 550)
			{
				pStarPoint = Cur;
				break;
			}
		}
		for (TPoint* Cur = pStarPoint->NextPoint(); Cur != pStarPoint;)
		{
			TPoint* pNext = Cur->NextPoint();
			if (Cur->GetType() == eCorner270 && Cur->mPreEdge->GetLen() <= 550 && Cur->mPreEdge->mBit == Cur->mNextEdge->mBit &&Cur->mNextEdge->mBit == pNext->mNextEdge->mBit)
			{
				if (pNext->mNextEdge->GetLen() <= 550 && pNext->GetType() == eCorner270)
				{
					TRectBase Pillar;
					Pillar.mDir = Cur->mNextEdge->mDir;
					Pillar.mPos = (Cur->GetVector() + pNext->mNextEdge->mEnd->GetVector())*0.5f;
					Pillar.mSize.X = Cur->mPreEdge->GetLen()*0.5f;
					Pillar.mSize.Y = Cur->mNextEdge->GetLen()*0.5f;
					Pillar.mUserType = 1;
					Pillars.Add(Pillar);
					ChangeEnd(Cur->mPreEdge->PreEdge(),pNext->mNextEdge->mEnd);
					Cur = pNext->mNextEdge->mEnd;
					continue;
				}
				else if (Cur->mNextEdge->GetLen() <= 550 && pNext->GetType() == eCorner90)
				{
					FVector2D pos = Cur->mPreEdge->mStart->GetVector() + (pNext->GetVector() - Cur->GetVector());

					TRectBase Pillar;
					Pillar.mDir = Cur->mNextEdge->mDir;
					Pillar.mPos = (Cur->GetVector() + pos)*0.5f;
					Pillar.mSize.X = Cur->mNextEdge->GetLen()*0.5f;
					Pillar.mSize.Y = Cur->mPreEdge->GetLen()*0.5f;
					Pillar.mUserType = 0;
					Pillars.Add(Pillar);

					TPoint* pPoint = NewPoint(pos);
					ChangeEnd(Cur->mPreEdge->PreEdge(),pPoint);
					ChangeStart(pNext->mNextEdge,pPoint);
					Cur = pPoint;
					continue;
				}
			}
			else if (Cur->GetType() == eCorner180 && Cur->mPreEdge->mBit == Cur->mNextEdge->mBit )
			{
				ChangeEnd(Cur->mPreEdge,pNext);
			}
			Cur = pNext;
		}
		for (TEdge* pLine = pStarPoint->mNextEdge; true; )
		{
			TLine2d line;
			line.mStart = pLine->mStart->GetVector();
			line.mEnd = pLine->mEnd->GetVector();
			line.mState = pLine->mBit;
			line.Id = pLine->mId;
			OutPaths.Add(line);
			if (pLine == pStarPoint->mPreEdge)
				break;
			pLine = pLine->NextEdge();
		}
		return true;
	}
}