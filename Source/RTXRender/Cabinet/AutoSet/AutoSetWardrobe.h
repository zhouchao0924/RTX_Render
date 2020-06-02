#pragma once
#include "CabinetSetBase.h"
namespace AutoSetWardrobe
{
	class FAutoSetCabinet :public FAutoSetBase
	{
	public:
		SINGLE(FAutoSetCabinet);
		bool PreExecute();
	protected:
		bool SetSetEdge(TSetEdge* pSetEdge);
		virtual bool SetSetEdge(TSetEdge * pSetEdge, FCabinetSet::eAlignType eAlign,FCabinetSet::eFillType eFillType);
	};
}