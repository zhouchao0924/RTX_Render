#pragma once
#include "CabinetSetBase.h"
namespace AutoSetBedCabinet
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool SetSetEdge(TSetEdge* pSetEdge);
	};
}