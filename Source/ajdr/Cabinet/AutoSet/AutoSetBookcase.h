#pragma once
#include "CabinetSetBase.h"
namespace AutoSetBookcase
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool SetSetEdge(TSetEdge* pSetEdge);
	};
}