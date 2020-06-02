
#include "Wall.h"
#include "Corner.h"
#include "ISuite.h"
#include "WallHole.h"
#include "ModelInstance.h"
#include "Class/Property.h"

WallHole::WallHole()
	:WallID(INVALID_OBJID)
	,ZPos(0)
	,Width(0)
	,Height(0)
	,Thickness(0)
	,bFlip(false)
{
}

BEGIN_DERIVED_CLASS(WallHole, Anchor)
	ADD_PROP_READONLY(WallID, IntProperty)
	ADD_PROP(ZPos, FloatProperty)
	ADD_PROP(Width, FloatProperty)
	ADD_PROP(Height, FloatProperty)
	ADD_PROP(Thickness, FloatProperty)
	ADD_PROP(bFlip, BoolProperty)
END_CLASS()

void WallHole::GetPolygon(kPoint *&pPolygon, int &Num)
{
	if (polygons.size() <= 0)
	{
		kPoint P0, P1;
		Corner *pCorner = SUITE_GET_BUILDING_OBJ(CornerID, Corner);
		Wall *pWall = SUITE_GET_BUILDING_OBJ(WallID, Wall);
		pWall->GetLocations(P0, P1);
		float OffsetX = (pCorner->Location - P0).Size();
		float HalfWidth = Width / 2.0f;
		polygons.push_back(kPoint(OffsetX - HalfWidth, ZPos));
		polygons.push_back(kPoint(OffsetX - HalfWidth, ZPos + Height));
		polygons.push_back(kPoint(OffsetX + HalfWidth, ZPos + Height));
		polygons.push_back(kPoint(OffsetX + HalfWidth, ZPos));
	}
	pPolygon = &polygons[0];
	Num = (int)polygons.size();
}

void WallHole::OnDestroy()
{
	Wall *pWall = SUITE_GET_BUILDING_OBJ(WallID, Wall);
	if (pWall)
	{
		pWall->MarkNeedUpdate();
	}
	Anchor::OnDestroy();
}


