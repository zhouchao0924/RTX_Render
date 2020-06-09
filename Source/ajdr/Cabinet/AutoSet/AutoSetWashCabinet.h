#pragma once
#include "CabinetSetBase.h"
namespace AutoSetWashCabinet
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool SetSetEdge(TSetEdge* pSetEdge);
	};
}