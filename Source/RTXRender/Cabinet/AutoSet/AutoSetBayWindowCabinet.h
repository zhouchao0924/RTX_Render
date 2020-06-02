#pragma once
#include "CabinetSetBase.h"
namespace AutoSetBayWindowCabinet
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		FAutoSetCabinet();
		~FAutoSetCabinet();

		bool Execute(FCabinetArea* pArea);
		bool SetSetEdge(TSetEdge* pSetEdge);
		bool SetSetEdge();
	};
}