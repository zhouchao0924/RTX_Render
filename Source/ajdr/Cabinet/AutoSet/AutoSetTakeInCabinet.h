#pragma once
#include "CabinetSetBase.h"
namespace AutoSetTakeInCabinet
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool Execute(FCabinetArea* pArea);
	};
}