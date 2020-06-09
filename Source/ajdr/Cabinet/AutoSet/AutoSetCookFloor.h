#pragma once
#include "CabinetSetBase.h"
namespace AutoSetCookFloor
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		FAutoSetCabinet();
		~FAutoSetCabinet();
		bool PreExecute();
		bool PostExecute();
		virtual bool AfterExecute();
		virtual bool GetLayoutResult(FCabinetLayoutResult& LayoutData);
		virtual bool CompleteLayout();
		virtual bool OnReplace();
		bool ModifySkuPos(int type, FVector2D NewPos);
		FCabinetSkuModel SkuData[2];
		TSkuInstance		mSkus[2];//0水槽1灶具
	protected:
		TSetEdge* MaxLenSeg(TArray<TSetEdge*>& ChimerySegs,TSetEdge* pExpacte=nullptr);
		bool SignChimery();
		bool SignSink();
	protected:
		bool SetSetEdge(TSetEdge* pSetEdge);
		int32	mCornerType;//转角柜类型
		bool SignCorner();
		bool SignCorner(TSetEdge* pSetEdge);
		bool FillCorner(TSetEdge* pSetEdge, bool bIs = true);
		bool FillCorner270(TSetEdge* pSetEdge);
		//
		bool SetChimerySinkEdge(TSetEdge* pSetEdge);
		bool SetChimeryEdge(TSetEdge* pSetEdge);
		bool SetSinkEdge(TSetEdge* pSetEdge);
		bool SetOtherEdge(TSetEdge* pSetEdge);

		bool GetEdgePillarSet(TSetEdge* pSetEdge, int& nHeadId, int& nTailId);
		///
	protected:
		bool ResteSku(TSetEdge* pSetEdge0, TSetEdge* pSetEdge1);
		bool SwapSinkPos2Window(TSetEdge* pSetEdge);
		bool SetCabinets(TSetEdge * pSetEdge, FCabinetSet::eAlignType eAlign, int Id0, int id1 = 0, int Id2 = 0, int Id3 = 0, int Id4 = 0, int Id5 = 0, int Id6 = 0, int Id7 = 0, int Id8 = 0);
		bool FillIsolateCorner(TSetEdge * pSetEdge);//孤立的烟道柱子
	protected:
		TSetEdge*		mChimeySeg;
		PolyEdgeLib::TPoint*	mChimeyCorner;
		PolyEdgeLib::TEdge*		mChimeyEdge;//当前选择区域的烟道边
		TSetEdge*		mSinkSeg;
		int32	mWindowType;//0：不含窗口，1：当前的墙含窗洞 2：当前的墙左侧含窗，3：当前的墙右侧含窗
	protected:
		FCabinetRes mChimey;
		FCabinetRes mSink;
		FCabinetRes mDrawer;
		FCabinetRes mSingleDoor;
		FCabinetRes mDoubleDoor;
		FCabinetRes mCornerTurn;
		FCabinetRes mCornerLine;
	};
}
