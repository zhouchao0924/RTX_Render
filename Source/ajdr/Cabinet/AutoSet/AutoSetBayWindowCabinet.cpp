
#include "AutoSetBayWindowCabinet.h"
#include "../CabinetMgr.h"
#include "../CabinetArea.h"
using namespace PolyEdgeLib;
namespace AutoSetBayWindowCabinet
{
	FAutoSetCabinet::FAutoSetCabinet()
	{

	}

	FAutoSetCabinet::~FAutoSetCabinet()
	{
	}
	bool FAutoSetCabinet::Execute(FCabinetArea* pArea)
	{
		OtherExecute(pArea);
		mArea = pArea;
		mHeight = 0;
		AddPoly(mArea->mOutPaths, eSliptWall, true);
		AddPoly(mSelectWalls, eSliptWallSelect);

		TArray<TRectBase> Rects;
		pArea->GetFurnitureModel(1, Rects);
		AddPoly(Rects, eSliptWallObject);

		SignFrame();

		TEdge* pEdge = GetEdge(eSliptWallSelect | eSliptWallObject);

		if (pEdge == nullptr)
		{
			SetSetEdge();
			return true;
		}

		FVector2D start(pEdge->mStart->mPreEdge->mStart->x, pEdge->mStart->mPreEdge->mStart->y);
		FVector2D end(pEdge->mEnd->mNextEdge->mEnd->x, pEdge->mEnd->mNextEdge->mEnd->y);
		

		//mSetEdgeHead = (TSetEdge*)NewSegment(pEdge->mStart, pEdge->mEnd);
		//SetSetEdge(mSetEdgeHead);
		
		if (pEdge && mSelectWalls.Num() > 1)
		{
			//mArea = pArea;
			//CHECK_ERROR(ExecuteBase());
			auto TempCabinet = mRess.Find(200127);

			if (!TempCabinet)
				return false;
			for (int i = 0; i< mSelectSegs.Num(); ++i)
			{
				FVector2D start1(mSelectSegs[i]->mStart->x, mSelectSegs[i]->mStart->y);
				FVector2D end1(mSelectSegs[i]->mEnd->x, mSelectSegs[i]->mEnd->y);
				if (start.Equals(start1,0.1f) && end.Equals(end1,0.1f))
				{
					mSelectSegs[i]->mCabinets.Empty();
					mSetEdgeHead = (TSetEdge*)NewSegment(pEdge->mStart, pEdge->mEnd);
					SetSetEdge(mSetEdgeHead);
				}
				else
				{
					mSelectSegs[i]->mCabinets.Empty();
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
							CabinetSet.mScale.X = currentX / CabinetSet.mRes.mSize.X;
							CabinetSet.mSetType = mSetType;
							mSelectSegs[i]->mCabinets.Add(CabinetSet);
							currentpos = currentpos + CabinetSet.mPosLocal * 2.f * mSelectSegs[i]->mDir;
						}
				}
			}
		}
		else
		{
			for (auto pset : mSegments)
			{
				TSetEdge* pSetEdge = (TSetEdge*)pset;
				pSetEdge->mCabinets.Empty();
			}
			mSetEdgeHead = (TSetEdge*)NewSegment(pEdge->mStart, pEdge->mEnd);
			SetSetEdge(mSetEdgeHead);
		}

		return true;
	}

	bool  FAutoSetCabinet::SetSetEdge()
	{
		auto TempCabinet = mRess.Find(200127);

		if (!TempCabinet)
			return false;

		for (int i = 0; i < mSelectSegs.Num(); ++i)
		{
			mSelectSegs[i]->mCabinets.Empty();
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
				CabinetSet.mScale.X = currentX / CabinetSet.mRes.mSize.X;
				CabinetSet.mSetType = mSetType;
				mSelectSegs[i]->mCabinets.Add(CabinetSet);
				currentpos = currentpos + CabinetSet.mPosLocal * 2.f * mSelectSegs[i]->mDir;
			}
		}
		return true;
	}

	bool FAutoSetCabinet::SetSetEdge(TSetEdge* pSetEdge)
	{
		TEdge* pEdge = pSetEdge->HeadEdge();

		FCabinetSet CabinetSet;
		CabinetSet.mId = gCabinetMgr.GenId();
		CabinetSet.mPosLocal = pSetEdge->GetRemainLen()*0.5f;
		CabinetSet.mRes = mCabinetType == 0? GetOptionCabinet(pSetEdge, 200125, 200126, 200127):GetRes(mCabinetType);
		CabinetSet.mPos = pSetEdge->mStart->GetVector() + CabinetSet.mPosLocal*pSetEdge->mDir - pSetEdge->mNor * (CabinetSet.mRes.mSize.Y / 2 - 60);
		CabinetSet.mDir = pSetEdge->mDir;
		CabinetSet.mScaleZ = 1.f;//(float(pEdge->mPoly->mPosZ)) / CabinetSet.mRes.mCabinetHeight;
		CabinetSet.mScale.X = pSetEdge->GetRemainLen() / CabinetSet.mRes.mSize.X;
		CabinetSet.mSetType = mSetType;
		pSetEdge->mCabinets.Add(CabinetSet);
		//pEdge->mca
		//dge->
		//mSetEdgeHead->mCabinets.Add(CabinetSet);
		SuitableSegCabinets();
		return true;
	}

}