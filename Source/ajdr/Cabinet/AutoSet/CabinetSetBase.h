#pragma once
#include "CabinetSetEdge.h"
#include "../CabinetStruct.h"

class FAutoSetBase : public PolyEdgeLib::CPolyMgr
{
public:
	FAutoSetBase();
	~FAutoSetBase();
	bool Clear();
public:
	virtual bool LayoutCabinet(class FCabinetArea* pArea);
	virtual bool ChangeTurn(TSetEdge* pSetEdge);
	virtual bool ReLayoutCabinet(TSetEdge* pSetEdge, int32 CabinetIdx, int32 mcategoryId);
	virtual bool OnRePlaceCabinet(TSetEdge* pSetEdge, int32 CabinetIdx, int32 mcategoryId) { return true; }
	virtual bool ReLayoutTurn(TSetEdge* pSetEdge, int32 CabinetIdx, int32 mcategoryId);
	virtual bool replaceLayoutTemplate(int macterId, int mcategoryId);
	virtual bool CompleteLayout();
	virtual bool OnReplace() { return true; }
public:
	virtual bool Execute(class FCabinetArea* pArea);
	virtual bool GetLayoutResult(FCabinetLayoutResult& LayoutData);
	bool GetCabinets(TArray<int32>& Cabinets, TArray<int32>& Skus);
	int32 IsLayoutError();
protected:
	bool CheckOverlay();
	bool FailSegSet(TSetEdge* pSetEdge);

	bool OtherExecute(FCabinetArea* pArea);
protected:
	virtual bool PreExecute() { return true; }
	virtual bool ExecuteBase();
	virtual bool PostExecute() { return true; }
	virtual bool SetSetEdge(TSetEdge* pSetEdge) { return true; }
	virtual bool ExecuteSelete();
	virtual bool AfterExecute() { return true; }
	virtual PolyEdgeLib::TSegment* NewSegment(PolyEdgeLib::TPoint* pStart, PolyEdgeLib::TPoint* pEnd);
public:
	bool InitRes(FCabinetRes res);
	bool GetSuitable(FCabinetSet& res);
public:
	FCabinetArea* mArea;
	eAutoSetType  mSetType;
	TArray<TLine2d>	  mSelectWalls;
	TMap<int, FCabinetRes> mRess;//
	TMap<int, FCabinetRes> mInstances;//
	//
	bool	mForceSet;
	bool	mCookOverlay;//橱柜重叠
	TArray<int32>	mOverlayCabinets;//重叠的其他柜子
	FCabinetOverlayResult mFailData;
	//
	bool	mReplaceReseat;
	int32	mCabinetType;
	int32	mErrorCodeId;
	int32	mCabinetHangType;
protected:
	bool   AddCabinets(TMap<int32, FCabinetShell*>& Rects);
	FCabinetRes GetRes(int id);
	bool GetSegs(TArray<TSetEdge *>& Segs, uint32 nBit, uint32 nExpactBit=0);
	bool NenSetEdgeCabinets();
	bool NenSetEdgeCabinets(TSetEdge* pSeg);
	//
	bool SuitableSegCabinets();

	int32 MinLenCabinets(int Id0, int Id1 = 0, int Id2 = 0, int Id3 = 0, int Id4 = 0, int Id5 = 0, int Id6 = 0, int Id7 = 0, int Id8 = 0);
	int32 MaxLenCabinets(int Id0, int Id1 = 0, int Id2 = 0, int Id3 = 0, int Id4 = 0, int Id5 = 0, int Id6 = 0, int Id7 = 0, int Id8 = 0);
	bool MinCabinets(TSetEdge * pSetEdge, int Id0, int Id1 = 0, int Id2 = 0, int Id3 = 0, int Id4 = 0, int Id5 = 0, int Id6 = 0, int Id7 = 0, int Id8 = 0);
	bool MaxCabinets(TSetEdge * pSetEdge, int Id0, int Id1 = 0, int Id2 = 0, int Id3 = 0, int Id4 = 0, int Id5 = 0, int Id6 = 0, int Id7 = 0, int Id8 = 0);
	bool CanFillCabinets(TSetEdge * pSetEdge, int Id0, int Id1 = 0, int Id2 = 0, int Id3 = 0, int Id4 = 0, int Id5 = 0, int Id6 = 0, int Id7 = 0, int Id8 = 0);
	bool FillCabinets(TSetEdge * pSetEdge, FCabinetSet::eAlignType eAlign, int Id0, int id1 = 0, int Id2 = 0, int Id3 = 0, int Id4 = 0, int Id5 = 0, int Id6 = 0, int Id7 = 0, int Id8 = 0);

	FCabinetRes GetOptionCabinet(TSetEdge* pSeg, int Res0, int Res1, int Res2 = 0, int Res3 = 0);
	int  OptionSetCabinets(TSetEdge* pSeg, FCabinetSet::eAlignType eAlign, int Res0, int Res1);
	//
	TArray<FCabinetRes> GetCategorys(int categoryId);
protected:
	float	mHeight;
	uint32 mBit;
	uint32 mExpactBit;
	TSetEdge*     mSetEdgeHead;
	TArray<TSetEdge*> mSelectSegs;
	int32	mSegId;
};