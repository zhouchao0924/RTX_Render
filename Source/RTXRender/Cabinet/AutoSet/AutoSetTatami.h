#pragma once
#include "CabinetSetBase.h"
namespace AutoSetTatami
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		FAutoSetCabinet();
		virtual bool GetLayoutResult(FCabinetLayoutResult& LayoutData);
		bool SetSetEdge(TSetEdge* pSetEdge);
		bool SetnDepth(int32 nDepth);
		bool SetSideType(int32 nSideType);
		int32	mSideType;//0 左边 1: 右边 2：不放置 -1:初始化自动决定
		virtual bool OnRePlaceCabinet(TSetEdge* pSetEdge, int32 CabinetIdx, int32 mcategoryId);
	private:
		TSetEdge*	mSetEdge;
		int32	mRow; //是否三列
		int32	mDepth;
		int32	mCabinetTypes[4];
		bool AutoSetSideType();
		FCabinetTatamiExtern mSideData[2];
	};
}