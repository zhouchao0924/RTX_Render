#pragma once
#include "AutoSetWardrobe.h"
namespace AutoSetWardrobeHang
{
	class FAutoSetCabinet :public AutoSetWardrobe::FAutoSetCabinet
	{
	public:
		SINGLE(FAutoSetCabinet);
	protected:
		virtual bool SetSetEdge(TSetEdge * pSetEdge, FCabinetSet::eAlignType eAlign,FCabinetSet::eFillType eFillType);
	};
}