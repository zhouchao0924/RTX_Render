#pragma once
#include "CabinetSetBase.h"
namespace AutoSetTelevision
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool SetSetEdge(TSetEdge* pSetEdge);
	};
}