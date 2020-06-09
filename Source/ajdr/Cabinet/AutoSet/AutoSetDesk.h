#pragma once
#include "CabinetSetBase.h"
namespace AutoSetDesk
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool SetSetEdge(TSetEdge* pSetEdge);
		bool PreExecute();
	};
}