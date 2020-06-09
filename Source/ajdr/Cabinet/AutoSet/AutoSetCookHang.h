#pragma once
#include "CabinetSetBase.h"
/*
吊柜柜体自动排列
*/
namespace AutoSetCookHang
{
	class  FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		FAutoSetCabinet();
		~FAutoSetCabinet();
		bool PreExecute();
		bool PostExecute();
	protected:
		bool SetSetEdge(TSetEdge* pSetEdge);
	protected:
		FCabinetRes mChimeyHang;
		FCabinetRes mChimeySide;
		FCabinetRes mSingleDoor;
		FCabinetRes mDoubleDoor;
		FCabinetRes mOpenDoor;
		FCabinetRes mCornerLine;
		//
		FCabinetRes GetCorner90Fill();
		FCabinetRes GetCorner270Fill();
	};
}