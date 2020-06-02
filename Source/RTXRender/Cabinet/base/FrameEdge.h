#pragma once
#include "CoreMinimal.h"
#include "../base/common.h"

namespace PolyEdgeLib
{
	struct TPoint
	{
		int32 x;
		int32 y;

		int64 Id;
		TPoint(float x, float y);
		TPoint(FVector2D& Pos);

		struct TEdge* mPreEdge;
		struct TEdge* mNextEdge;

		eCornerType GetType();
		bool IsHor();
		bool IsTrun();
		bool IsTurn90();
		bool IsTurn270();
		TPoint* NextPoint();
		TPoint* PrePoint();
		float Dist(TPoint* pPoint);
		FVector2D GetVector() { return FVector2D(x, y); }
	};
	//////////////////////////////////////////////////////////////////////////
	struct TEdge
	{
		FString Id;
		int32   mId;
		uint32  mBit; //

		TPoint* mStart;
		TPoint* mEnd;

		FVector2D mDir;
		FVector2D mNor;

		TEdge();
		bool InitPoint(TPoint* pStart, TPoint* pEnd);

		TEdge* PreEdge() { return  mStart->mPreEdge; }
		TEdge* NextEdge() { return mEnd->mNextEdge; }

		FVector2D GetPos() { return (mStart->GetVector() + mEnd->GetVector())*0.5f; }
		float GetLen();
		bool IsState(uint32 nBit, uint32 nExpactBit=0);
		//
		//连续的最前面一个最后面一个（包含当前的）
		TEdge* FirstEdge(uint32 nBit, uint32 nExpactBit = 0);
		TEdge* LastEdge(uint32 nBit, uint32 nExpactBit=0);
		//下一个最早出现的（包含当前的）
		TEdge*  PreFirstEdge(uint32 nBit, uint32 nExpactBit = 0);
		TEdge*  NextFirstEdge(uint32 nBit, uint32 nExpactBit = 0);
		//
		TPoint* PreTurnPoint();
		TPoint* NextTurnPoint();
		struct TPoly* mPoly;
	};
	struct TSegment: public TEdge
	{
		TSegment* mPreSeg;
		TSegment* mNextSeg;
		TEdge* HeadEdge() { return mStart->mNextEdge; }
		TEdge* TailEdge() { return mEnd->mPreEdge; }
		bool Init(TPoint* pStart, TPoint* pEnd);
		virtual bool  Init() { return true; }
	};
	//////////////////////////////////////////////////////////////////////////
	struct TPoly
	{
		uint32 mBit;
		int32 Id;
		TArray<TPoint*>  mPath;
		TArray<TEdge*>   mEdges;
		int32 mHeight;
		int32 mPosZ;
	};
	struct TBox :public TPoly
	{
		FVector2D mPos;
		FVector2D mSizeHalf;
		FVector2D mDir;
	};
	//////////////////////////////////////////////////////////////////////////
	class CPolyMgr
	{
	public:
		CPolyMgr();
		~CPolyMgr();
		bool Clear();
		TPoly* AddPoly(TArray<TLine2d>& Path, uint32 nBit,bool bMain=false);
		bool   AddPoly(TMap<int, TLine2d>& Path, uint32 nBit,int BiasLen=-1);
		TBox*  AddPoly(FVector2D Pos, FVector2D SizeHalf, FVector2D Dir, uint32 nBit);
		TPoly* AddPoly(TRectBase* pRect, uint32 nBit);
		bool   AddPoly(TArray<TRectBase>& Rects, uint32 nBit);
		TPoly* AddPoly(FVector2D Pos, FVector2D Dir, FVector2D Scale, TArray<FVector2D>& Path, uint32 nBit);
		TPoly* AddPoly(TArray<FVector2D>& Path, uint32 nBit = 0, bool bMain = false);
		//////////////////////////////////////////////////////////////////////////
		int	   GetEdges(TArray<TEdge*>& Edges, uint32 nBit, uint32 nExpactBit=0);
		TEdge* GetEdge(uint32 nBit, uint32 nExpactBit=0);
		//////////////////////////////////////////////////////////////////////////
		TSegment* GetSegments(uint32 nBit, uint32 nExpactBit = 0);
		bool	GetOutLine(TArray<FVector2D>& OutLine, int nExtern, uint32 nBit, uint32 nExpactBit = 0);
		bool	GetOutLine(TArray<FVector2D>& OutLineAll, TArray<FVector2D>& PathFloorBottom, TArray<FVector2D>& PathOnWall, TArray<FVector2D>& PathFront, int nExtern,int nExternBotom, uint32 nBit, uint32 nExpactBit = 0);
		//////////////////////////////////////////////////////////////////////////
		TPoly* AddPoly(TMap<int, TLine2d>& Path, uint32 nBit, bool bMain);
		bool   AdjustFrame(TMap<int, TLine2d>& RawAdjustWalls);
		bool   CombitFrame(TArray<TLine2d>& OutPaths, TArray< TRectBase>& Pillars);
		//////////////////////////////////////////////////////////////////////////
	protected:
		TMap<int64,   TPoint*> mPoints;
		TMap<FString, TEdge*>  mEdges;
		TMap<int32,   TPoly*>  mPolys;
		TPoly*	mMainPoly;
		TArray<TSegment*>	  mSegments;
	protected:
		TPoint* FindPoint(FVector2D& Pos, bool bPrecise = false);
		TPoint* NewPoint(FVector2D& Pos,bool bPrecise=false);
		TPoint* NewPointLocal(FVector2D& Anchor, FVector2D& Dir, FVector2D& Scale, FVector2D& LocalPos);
		TEdge*  NewEdge(FVector2D pStart, FVector2D pEnd, TPoly* pPoly,int32 nState = 0);
		TEdge*  NewEdge(TPoint* pStart, TPoint* pEnd, TPoly* pPoly,int32 nState = 0);
		TEdge*  FindEdge(TPoint* pStart, TPoint* pEnd);
		bool	InitBox(TBox* pBox);
		bool	InitPoly(TPoly* pBox);
		bool    AddPoly(TLine2d& Line, uint32 nBit, TPoly* pPoly, int BiasLen);
		//////////////////////////////////////////////////////////////////////////
		TEdge*	SignOnFrame(TEdge* pEdgeFrom);
		bool	SliptEdge(TEdge* pEdgeDet, TEdge* pEdgeOn);
		//
		bool	SignOnFrame(TLine2d& pEdgeFrom);
		bool	SliptEdge(TLine2d& pEdgeFrom, TEdge* pEdgeOn);
		///////////////////////////////////////////////////////////////////////////
		virtual TSegment* NewSegment(TPoint* pStart, TPoint* pEnd);
		//////////////////////////////////////////////////////////////////////////
	public:
		bool	  SignFrame();
		TSegment* GetFirstSegment(uint32 nBit, uint32 nExpactBit = 0);
		TSegment* GetFirstSegment(TSegment* pSeg, uint32 nBit, uint32 nExpactBit = 0);
		TSegment* GetLastSegment(TSegment* pSeg, uint32 nBit, uint32 nExpactBit = 0);
	protected:
		int32	mId;
	public:
		int32	mMinPointDistMerge;//点最小距离合并
		int32	mMinLineDistMerge; //线段距离最小距离认为重合
		int32	mMinLineDistMerge2;
	};
}