#pragma once
#include "CabinetSetBase.h"
namespace AutoSetPorchArk
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool SetSetEdge(TSetEdge* pSetEdge);
	};
}