#pragma once
#include "CabinetSetBase.h"
namespace AutoSetCoatroom
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool PreExecute();
		virtual bool AfterExecute();
	protected:
		bool SetSetEdge(TSetEdge* pSetEdge);
		bool SetSetEdge2(TSetEdge* pSetEdge);
		bool InitCabinetRes();

		FCabinetRes mResFake;
		FCabinetRes mResTurn;
		FCabinetRes mResDouble;
		FCabinetRes mResSingle;
	};
}
namespace AutoSetCoatroomHang
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool PreExecute();
	protected:
		bool SetSetEdge(TSetEdge* pSetEdge);
		bool InitCabinetRes();

		FCabinetRes mResFake;
		FCabinetRes mResTurn;
		FCabinetRes mResDouble;
		FCabinetRes mResSingle;
	};
}