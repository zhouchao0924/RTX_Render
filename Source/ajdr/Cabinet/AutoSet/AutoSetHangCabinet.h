#pragma once
#include "CabinetSetBase.h"
namespace AutoSetHangBedCabinet
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		//bool Execute(FCabinetArea* pArea);
		bool SetSetEdge(TSetEdge* pSetEdge);
	};
}