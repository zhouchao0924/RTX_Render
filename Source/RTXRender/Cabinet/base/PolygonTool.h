#pragma once
#include "CoreMinimal.h"
#include "Clipper/clipper.h"
#include "common.h"
//////////////////////////////////////////////////////////////////////////
class FXRPolygonTool
{

struct TPathNode
{
	bool IsPoly;
	std::vector<TCorner2d*>	mOutLine;
	std::vector<TPathNode*>		mHoles;
	ClipperLib::PolyNode*		mNode;
};
public:
	FXRPolygonTool();
	~FXRPolygonTool();
	void Clear();
	bool SetStandPath(TArray<TLine2d>* StandPaths);
	bool AddPoly(TArray<FVector2D>& vPath);
	bool AddPoly(FVector2D Pos, FVector2D SizeHalf, FVector2D Dir);
	bool AddTurnPoly(FVector2D Pos, FVector2D SizeHalf, FVector2D Dir, float fWidth);
	bool AddHole(FVector2D Pos, FVector2D SizeHalf, FVector2D Dir); 
	bool AddHole(TArray<FVector2D>& vPath);
	bool AddHole(FVector2D Pos, FVector2D Dir, FVector2D Scale, TArray<FVector2D>& vPath);

	bool Execute();
	bool GetPaths(TArray<TPath2d>& Path2ds);
private:
	void AddPoly() { mPolys.resize(mPolys.size() + 1); }
	void AddHole() { mHoles.resize(mHoles.size() + 1); }
	bool Clip();
protected:
	std::vector< ClipperLib::Path > mPolys;
	std::vector< ClipperLib::Path > mHoles;

	TArray<TPathNode*> mPathNodes;

	std::vector<TCorner2d>			 m2tVers;

	TArray<TLine2d>*    mStandPaths;//标准的路径点，修正到此路径
	bool AdjustPoint(FVector2D& p);
protected:
	inline INT32 ValueToClip(float Value) { return INT32((Value + (Value > 0 ? .5 : -.5))); }
	inline float ClipToValue(INT32 Value) { return Value; }
};