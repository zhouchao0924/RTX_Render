#pragma once
#include "CabinetSetBase.h"
namespace AutoSetToilet
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		FCabinetSet::eAlignType HaveStemRegion(TSetEdge* pSetEdge);
		virtual bool SetSetEdge(TSetEdge* pSetEdge);

		virtual bool AfterExecute();
	};
}