#pragma once
#include "CabinetSetBase.h"
namespace AutoSetBedside
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		FAutoSetCabinet();
		~FAutoSetCabinet();
		bool Execute(FCabinetArea* pArea);
	};
}