
#include "AutoSetTakeInCabinet.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetTakeInCabinet
{
	bool FAutoSetCabinet::Execute(FCabinetArea* pArea)
	{
		OtherExecute(pArea);

		auto TempCabinet = mRess.Find(200128);

		if (!TempCabinet)
			return false;

		for (int i = 0; i < mSelectSegs.Num(); ++i)
		{
			float len = mSelectSegs[i]->GetLen();

			if (len < TempCabinet->mSizeMin.X)
				return false;
			float bi = (len / TempCabinet->mSizeMin.X);
			int index = bi;
			float lo = bi - (float)index;
			float ofsizeX = lo * TempCabinet->mSizeMin.X / (float)index;
			float currentX = TempCabinet->mSizeMin.X + ofsizeX;

			FVector2D currentpos = mSelectSegs[i]->mStart->GetVector();
			for (int j = 0; j < index; ++j)
			{
				FCabinetSet CabinetSet;
				CabinetSet.mId = gCabinetMgr.GenId();
				CabinetSet.mPosLocal = (currentX)*0.5f;
				CabinetSet.mRes = *TempCabinet;
				CabinetSet.mPos = currentpos + CabinetSet.mPosLocal* mSelectSegs[i]->mDir + mSelectSegs[i]->mNor * (CabinetSet.mRes.mSize.Y / 2 - 10);
				CabinetSet.mDir = mSelectSegs[i]->mDir;
				CabinetSet.mScaleZ = 1.f;
				CabinetSet.mScale.X =  currentX / CabinetSet.mRes.mSize.X;
				CabinetSet.mSetType = mSetType;
				mSelectSegs[i]->mCabinets.Add(CabinetSet);
				currentpos = currentpos + CabinetSet.mPosLocal * 2.f * mSelectSegs[i]->mDir;
			}
		}


		/*mHeight = 0;
		mArea = pArea;
		AddPoly(mArea->mOutPaths, eSliptWall, true);
		AddPoly(mSelectWalls, eSliptWallSelect);

		TArray<TRectBase> Rects;
		pArea->GetFurnitureModel(2, Rects);
		AddPoly(Rects, eSliptWallObject);

		SignFrame();

		TEdge* pEdge = GetEdge(eSliptWallSelect | eSliptWallObject);
		if (pEdge == nullptr)
		{
			mErrorCodeId = 6;
			return false;
		}
		CHECK_ERROR(pEdge);
		mHeight = pEdge->mPoly->mPosZ;
		mSetEdgeHead = (TSetEdge*)NewSegment(pEdge->mStart, pEdge->mEnd);

		{
			FCabinetSet CabinetSet;
			CabinetSet.mId = gCabinetMgr.GenId();
			CabinetSet.mPosLocal = (CabinetSet.mRes.mSize.X + 32)*0.5f;
			CabinetSet.mRes = GetRes(200128);
			CabinetSet.mPos = mSetEdgeHead->mStart->GetVector() + CabinetSet.mPosLocal*mSetEdgeHead->mDir - mSetEdgeHead->mNor * (CabinetSet.mRes.mSize.Y / 2 - 10);
			CabinetSet.mDir = mSetEdgeHead->mDir;
			CabinetSet.mScaleZ = ((float)(pEdge->mPoly->mHeight)) / CabinetSet.mRes.mCabinetHeight;
			CabinetSet.mSetType = mSetType;
			mSetEdgeHead->mCabinets.Add(CabinetSet);
		}
		{
			FCabinetSet CabinetSet;
			CabinetSet.mId = gCabinetMgr.GenId();
			CabinetSet.mRes = GetRes(200128);
			CabinetSet.mPosLocal = mSetEdgeHead->mLen - (CabinetSet.mRes.mSize.X + 32)*0.5f;
			CabinetSet.mPos = mSetEdgeHead->mStart->GetVector() + CabinetSet.mPosLocal*mSetEdgeHead->mDir - mSetEdgeHead->mNor * (CabinetSet.mRes.mSize.Y / 2 - 10);
			CabinetSet.mDir = mSetEdgeHead->mDir;
			CabinetSet.mScaleZ = ((float)(pEdge->mPoly->mHeight)) / CabinetSet.mRes.mCabinetHeight;
			CabinetSet.mSetType = mSetType;
			mSetEdgeHead->mCabinets.Add(CabinetSet);
		}
		SuitableSegCabinets();*/
		return true;
	}
}