#pragma once
#include "CabinetSetBase.h"
namespace AutoSetStoreCabinet
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		FAutoSetCabinet();
		bool SetSetEdge(TSetEdge* pSetEdge);
		bool SetSetEdge(TSetEdge * pSetEdge, FCabinetSet::eAlignType eAlign);
	};
}