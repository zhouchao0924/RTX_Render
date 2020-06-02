#pragma once
#include "AutoSetToilet.h"
namespace AutoSetToiletHang
{
	class FAutoSetCabinet :public AutoSetToilet::FAutoSetCabinet
	{
	public:
		SINGLE(FAutoSetCabinet);
		virtual bool SetSetEdge(TSetEdge* pSetEdge);
	};
}