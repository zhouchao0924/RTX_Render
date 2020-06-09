
#include "PolygonTool.h"
#include "common.h"
FXRPolygonTool::FXRPolygonTool()
{
	mStandPaths = nullptr;
}
FXRPolygonTool::~FXRPolygonTool()
{
	Clear();
}
void FXRPolygonTool::Clear()
{
	for (auto p : mPathNodes)
		delete(p);
	mPathNodes.Empty();
}
bool FXRPolygonTool::SetStandPath(TArray<TLine2d>* StandPaths)
{
	mStandPaths = StandPaths;
	return true;
}
bool FXRPolygonTool::AdjustPoint(FVector2D& p)
{
	if (!mStandPaths)
		return false;
	for (auto wall : *mStandPaths)
	{
		if ((wall.mStart - p).SizeSquared() < 100)
		{
			p = wall.mStart;
			return true;
		}
		FVector2D CrossStart;
		float fDot0;
		float fDistance0 = Point2LineSegVer(wall.mStart, wall.mEnd, p, fDot0, CrossStart);
		if (fDistance0 < 10 && fDot0>0 && fDot0<1  )
		{
			p = CrossStart;
			return true;
		}
	}
	return false;
}

bool FXRPolygonTool::AddPoly(TArray<FVector2D>& vPath)
{
	AddPoly();
	for (auto p:vPath)
	{
		ClipperLib::IntPoint P(ValueToClip(p.X), ValueToClip(p.Y));
		mPolys.rbegin()->push_back(P);
	}
	return true;
}

bool FXRPolygonTool::AddPoly(FVector2D WorldPos, FVector2D SizeHalf, FVector2D Dir)
{
	static FVector2D Pos[4] ={ FVector2D(1,-1),FVector2D(1,1),FVector2D(-1,1),FVector2D(-1,-1) };
	AddPoly();
	for (int i = 0; i < 4; ++i)
	{
		FVector2D p = Local2World(WorldPos,Dir, SizeHalf, Pos[i]);
		AdjustPoint(p);
		ClipperLib::IntPoint P(ValueToClip(p.X), ValueToClip(p.Y));
		mPolys.rbegin()->push_back(P);
	}
	return true;
}

bool FXRPolygonTool::AddTurnPoly(FVector2D WorldPos, FVector2D SizeHalf, FVector2D Dir, float fWidth)
{
	float fPos = fWidth / SizeHalf.X - 1;
	FVector2D Pos[6] = { FVector2D(1,-1),FVector2D(1,fPos),FVector2D(fPos,fPos),FVector2D(fPos,1),FVector2D(-1,1),FVector2D(-1,-1) };
	AddPoly();
	for (int i = 0; i < 6; ++i)
	{
		FVector2D p = Local2World(WorldPos, Dir, SizeHalf, Pos[i]);
		AdjustPoint(p);
		ClipperLib::IntPoint P(ValueToClip(p.X), ValueToClip(p.Y));
		mPolys.rbegin()->push_back(P);
	}
	return true;
}
bool FXRPolygonTool::AddHole(FVector2D WorldPos, FVector2D SizeHalf, FVector2D Dir)
{
	static FVector2D Pos[4] = { FVector2D(1,-1),FVector2D(1,1),FVector2D(-1,1),FVector2D(-1,-1) };
	AddHole();
	for (int i = 0; i < 4; ++i)
	{
		FVector2D p = Local2World(WorldPos, Dir, SizeHalf, Pos[i]);
		ClipperLib::IntPoint P(ValueToClip(p.X), ValueToClip(p.Y));
		mHoles.rbegin()->push_back(P);
	}
	return true;
}
bool FXRPolygonTool::AddHole(FVector2D Pos, FVector2D Dir, FVector2D Scale, TArray<FVector2D>& vPath)
{
	if (vPath.Num()<3 )
		return false;
	AddHole();
	for (auto p : vPath)
	{
		FVector2D pos = Local2World(Pos, Dir, Scale, p);
		ClipperLib::IntPoint P(ValueToClip(pos.X), ValueToClip(pos.Y));
		mHoles.rbegin()->push_back(P);
	}
	return true;
}
bool FXRPolygonTool::AddHole( TArray<FVector2D>& vPath)
{
	if (vPath.Num() < 3)
		return false;
	AddHole();
	for (auto p : vPath)
	{
		ClipperLib::IntPoint P(ValueToClip(p.X), ValueToClip(p.Y));
		mHoles.rbegin()->push_back(P);
	}
	return true;
}
bool FXRPolygonTool::Execute()
{
	m2tVers.clear();
	Clip();
	return true;
}

bool FXRPolygonTool::GetPaths(TArray<TPath2d>& Path2ds)
{
	Path2ds.Empty();
	for (auto& pNode : mPathNodes)
	{
		if (pNode->IsPoly)
		{
			TPath2d OutLine;
			for (auto& pv : pNode->mOutLine)
			{
				FVector2D v(pv->X, pv->Y);
				OutLine.mOutLine.Add(v);
			}
			for ( auto p: pNode->mHoles )
			{
				TPath2d HoleLine;
				for (auto& pv : p->mOutLine)
				{
					FVector2D v(pv->X, pv->Y);
					HoleLine.mOutLine.Add(v);
				}
				OutLine.mHoles.Add(HoleLine);
			}
			Path2ds.Add(OutLine);
		}
	}
	return true;
}
bool FXRPolygonTool::Clip()
{
	ClipperLib::Clipper c;
	c.AddPaths(mPolys, ClipperLib::ptSubject, true);
	c.AddPaths(mHoles, ClipperLib::ptClip, true);
	ClipperLib::PolyTree RetPolys;
	c.Execute(ClipperLib::ctDifference, RetPolys, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	INT32 NumVer = 0;
	TMap<ClipperLib::PolyNode*, TPathNode*> TempNodes;
	for (ClipperLib::PolyNode* p = RetPolys.GetFirst(); p != NULL; p = p->GetNext())
	{
		NumVer += p->Contour.size();
		TPathNode* pNode = new TPathNode;
		pNode->mNode = p;
		pNode->IsPoly = !p->IsHole();
		mPathNodes.Add(pNode);
		TempNodes.Add(p, pNode);
	}
	m2tVers.resize(NumVer);

	int PointIdx = 0;
	for ( auto& pNode:mPathNodes )
	{
		ClipperLib::Path& ph = pNode->mNode->Contour;
		for (ClipperLib::Path::iterator p = ph.begin(); p != ph.end(); ++p)
		{
			TCorner2d& pt = m2tVers[PointIdx];
			pt.Id = PointIdx;
			++PointIdx;
			pt.X = ClipToValue(p->X);
			pt.Y = ClipToValue(p->Y);
			pNode->mOutLine.push_back(&pt);
		}
		for (auto& p : pNode->mNode->Childs )
		{ 
			auto pChild = *TempNodes.Find(p);
			pNode->mHoles.push_back(pChild);
		}
	}
	return true;
}
