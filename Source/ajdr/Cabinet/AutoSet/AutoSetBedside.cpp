
#include "AutoSetBedside.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetBedside
{
	FAutoSetCabinet::FAutoSetCabinet()
	{
		
	}

	FAutoSetCabinet::~FAutoSetCabinet()
	{
	}
	bool FAutoSetCabinet::Execute(FCabinetArea* pArea)
	{
		mHeight = 0;
		mArea = pArea;

		AddPoly(mArea->mOutPaths, eSliptWall, true);
		AddPoly(mSelectWalls, eSliptWallSelect);

		TArray<TRectBase> Rects;
		pArea->GetFurnitureModel(0, Rects);
		AddPoly(Rects, eSliptWallObject);

		SignFrame();

		mSetEdgeHead = (TSetEdge*)GetSegments(eSliptWallSelect | eSliptWallObject, mExpactBit);
		if (mSetEdgeHead == nullptr)
		{
			mSetEdgeHead = (TSetEdge*)GetSegments(eSliptWallSelect, mExpactBit);
			if (mSetEdgeHead)
			{
				mSetEdgeHead->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillDefault, GetRes(200129));
				mSetEdgeHead->SetCabinetPos(FCabinetSet::eAlignTail, FCabinetSet::eFillDefault, GetRes(200129));
				SuitableSegCabinets();
			}
		}
		else
		{
			mSetEdgeHead->NextSeg()->SetCabinetPos(FCabinetSet::eAlignHead, FCabinetSet::eFillDefault, GetRes(200129));
			mSetEdgeHead->PreSeg()->SetCabinetPos(FCabinetSet::eAlignTail, FCabinetSet::eFillDefault, GetRes(200129));
			SuitableSegCabinets();
		}
		return true;
	}

}