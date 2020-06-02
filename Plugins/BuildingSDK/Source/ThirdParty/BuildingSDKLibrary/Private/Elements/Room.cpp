
#include "Room.h"
#include "ISuite.h"
#include "Wall.h"
#include "DoorHole.h"
#include "Corner.h"
#include "Skirting.h"
#include "Class/Property.h"
#include "PlanePrimitive.h"

void FWallSegment::Serialize(ISerialize &Ar)
{
	Ar << WallID;
	Ar << SourceCornerID;
	Ar << bInverseWall;
}

//////////////////////////////////////////////////////////////////////////
BEGIN_DERIVED_CLASS(Room, BuildingObject)
	ADD_PROP_READONLY(CeilID, IntProperty)
	ADD_PROP_READONLY(FloorID, IntProperty)
	ADD_PROP_READONLY(SubRooms, IntArrayProperty)
	ADD_PROP_READONLY(InnerPolygons, Vec2DArrayProperty)
	ADD_PROP_READONLY(OuterPolygons, Vec2DArrayProperty)
END_CLASS()

Room::Room()
	: bCachePolygons(false)
	, FloorID(INVALID_OBJID)
	, CeilID(INVALID_OBJID)
{
}

void Room::Serialize(ISerialize &Ar)
{
	BuildingObject::Serialize(Ar);
	Ar << SkirtingCielID;
	Ar << SkirtingFloorID;

	SERIALIZE_VEC(SubRooms);
	SERIALIZE_COMPLEXVEC(WallSegments);
}

void Room::Init(ObjectID *pCorners, int Count)
{
	WallSegments.resize(Count);

	for (int ic = 0; ic < Count; ++ic)
	{
		ObjectID p0 = pCorners[ic];
		ObjectID p1 = pCorners[(ic + 1) % Count];

		ObjectID wallID = _Suite->GetWallByTwoCorner(p0, p1);
		Wall *pWall = SUITE_GET_BUILDING_OBJ(wallID, Wall);

		if (pWall)
		{
			FWallSegment &Segment = WallSegments[ic];
			Segment.WallID = wallID;
			Segment.SourceCornerID = p0;
			if (pWall->P[0] == p1)
			{
				Segment.bInverseWall = true;
				assert(pWall->RoomRight == INVALID_OBJID);
				pWall->RoomRight = _ID;
			}
			else
			{
				Segment.bInverseWall = false;
				assert(pWall->RoomLeft == INVALID_OBJID);
				pWall->RoomLeft = _ID;
			}
		}
	}

	BuildCache();
}

bool Room::GetBox2D(kPoint &Orignal, kPoint &Range)
{
	if (OuterPolygons.size() > 0)
	{
		kPoint *pPolygon = &OuterPolygons[0];
		kPoint MinPt = pPolygon[0], MaxPt = pPolygon[0];

		for (int i = 1; i < OuterPolygons.size(); ++i)
		{
			if (pPolygon[i].x < MinPt.x)
			{
				MinPt.x = pPolygon[i].x;
			}

			if (pPolygon[i].y < MinPt.y)
			{
				MinPt.y = pPolygon[i].y;
			}

			if (pPolygon[i].x > MaxPt.x)
			{
				MaxPt.x = pPolygon[i].x;
			}

			if (pPolygon[i].y > MaxPt.y)
			{
				MaxPt.y = pPolygon[i].y;
			}
		}
		
		Orignal = MinPt;
		Range = MaxPt - MinPt;
			
		return true;
	}

	return false;
}

int  Room::GetPolygon(kPoint *&pPolygon, bool bInnerPolygon)
{
	BuildCache();
	int Count = (int)InnerPolygons.size();

	if (bInnerPolygon)
	{
		pPolygon = &(InnerPolygons[0]);
	}
	else
	{
		pPolygon = &(OuterPolygons[0]);
	}
		
	return Count;
}

int  Room::GetWallHeight(float *&pHeight)
{
	BuildCache();
	int Count = (int)Heights.size();
	pHeight = &Heights[0];
	return Count;
}

void Room::BuildCache()
{
	if (!bCachePolygons)
	{
		Heights.clear();
		InnerPolygons.clear();
		OuterPolygons.clear();

		for (size_t i = 0; i < WallSegments.size(); ++i)
		{
			FWallSegment &Segment = WallSegments[i];
			Wall *pWall = SUITE_GET_BUILDING_OBJ(Segment.WallID, Wall);
			assert(pWall);
				
			kLine l_center, l_right, l_left;
			pWall->GetBorderLines(l_center, l_left, l_right);

			if (!Segment.bInverseWall)
			{
				OuterPolygons.push_back(kPoint(l_center.start.x, l_center.start.y));//l_center
				InnerPolygons.push_back(kPoint(l_right.start.x, l_right.start.y));
				Heights.push_back(pWall->GetHeight(0));
			}
			else
			{
				OuterPolygons.push_back(kPoint(l_center.end.x, l_center.end.y));//l_center
				InnerPolygons.push_back(kPoint(l_left.end.x, l_left.end.y));
				Heights.push_back(pWall->GetHeight(1));
			}
		}

		int nPoints = (int)WallSegments.size();
		if (nPoints > 0)
		{
			for (size_t i = 0; i < WallSegments.size(); ++i)
			{
				FWallSegment &Segment = WallSegments[i];
				kPoint CurrentPos = InnerPolygons[i];
				kPoint PrePos = InnerPolygons[(i + nPoints - 1) % nPoints];
				kPoint NextPos = InnerPolygons[(i + 1) % nPoints];
				kPoint V0 = (CurrentPos - PrePos).Normalize();
				kPoint V1 = (NextPos - CurrentPos).Normalize();

				float fCross = kPoint::CrossProduct(V0, V1);
				if (fCross < 0)
				{
					Segment.bCW = false;
				}
			}
		}

		bCachePolygons = true;
	}
}

void  Room::GetCorners(std::vector<Corner *> &Corners)
{
	size_t n = WallSegments.size();
	Corners.resize(n);

	for (size_t i = 0; i < WallSegments.size(); ++i)
	{
		FWallSegment &Segment = WallSegments[i];
		Corners[i] = SUITE_GET_BUILDING_OBJ(Segment.SourceCornerID, Corner);
	}
}

void  Room::GetCorners(std::vector<ObjectID> &Corners)
{
	size_t n = WallSegments.size();
	Corners.resize(n);

	for (size_t i = 0; i < WallSegments.size(); ++i)
	{
		FWallSegment &Segment = WallSegments[i];
		Corners[i] = Segment.SourceCornerID;
	}
}

bool Room::HasCorner(ObjectID CornerID)
{
	for (size_t i = 0; i < WallSegments.size(); ++i)
	{
		FWallSegment &Segment = WallSegments[i];
		if (CornerID == Segment.SourceCornerID)
		{
			return true;
		}
	}
	return false;
}

bool Room::HasEmptyWall()
{
	for (size_t i = 0; i < WallSegments.size(); ++i)
	{
		FWallSegment &Segment = WallSegments[i];
		Wall *pWall = SUITE_GET_BUILDING_OBJ(Segment.WallID, Wall);
		if (!pWall)
		{
			return true;
		}
	}
	return false;
}

void Room::MarkNeedUpdate()
{
	bCachePolygons = false;

	FloorPlane *pFloor = SUITE_GET_BUILDING_OBJ(FloorID, FloorPlane);
	if (pFloor)
	{
		pFloor->MarkNeedUpdate();
	}

	CeilPlane *pCeil = SUITE_GET_BUILDING_OBJ(CeilID, CeilPlane);
	if (pCeil)
	{
		pCeil->MarkNeedUpdate();
	}

	Skirting *pSkirtingCiel = SUITE_GET_BUILDING_OBJ(SkirtingCielID, Skirting);
	if (pSkirtingCiel)
	{
		pSkirtingCiel->MarkNeedUpdate();
	}

	Skirting *pSkirtingFloor = SUITE_GET_BUILDING_OBJ(SkirtingFloorID, Skirting);
	if (pSkirtingFloor)
	{
		pSkirtingFloor->MarkNeedUpdate();
	}

	BuildingObject::MarkNeedUpdate();
}

void Room::OnDestroy()
{
	if (FloorID!=INVALID_OBJID)
	{
		_Suite->DeleteObject(FloorID,true);
		FloorID = INVALID_OBJID;
	}
		
	if (CeilID != INVALID_OBJID)
	{
		_Suite->DeleteObject(CeilID,true);
		CeilID = INVALID_OBJID;
	}

	if (SkirtingFloorID != INVALID_OBJID)
	{
		_Suite->DeleteObject(SkirtingFloorID, true);
		SkirtingFloorID = INVALID_OBJID;
	}

	if (SkirtingCielID != INVALID_OBJID)
	{
		_Suite->DeleteObject(SkirtingCielID, true);
		SkirtingCielID = INVALID_OBJID;
	}

	for (size_t i = 0; i < WallSegments.size(); ++i)
	{
		FWallSegment &Segment = WallSegments[i];
		Wall *pWall = SUITE_GET_BUILDING_OBJ(Segment.WallID, Wall);
		if (pWall)
		{
			if (pWall->RoomLeft == _ID)
			{
				pWall->RoomLeft = INVALID_OBJID;
			}
			else if (pWall->RoomRight == _ID)
			{
				pWall->RoomRight = INVALID_OBJID;
			}
		}
	}

	BuildingObject::OnDestroy();
}

bool Room::HitTest(const kPoint &Location, ObjectID &HitObj)
{
	HitObj = INVALID_OBJID;
	int nWall = (int)WallSegments.size();

	for (int i = 0; i < nWall; ++i)
	{
		FWallSegment &Seg = WallSegments[i];
		Wall *pWall = SUITE_GET_BUILDING_OBJ(Seg.WallID, Wall);
			
		if (pWall && pWall->HitTest(Location, HitObj))
		{
			if (HitObj == INVALID_OBJID)
			{
				HitObj = pWall->GetID();
			}
			break;
		}
	}

	int nIntersec = 0;

	if (HitObj == INVALID_OBJID)
	{
		for (int i = 0; i < nWall; ++i)
		{
			kPoint P0 = OuterPolygons[i];
			kPoint P1 = OuterPolygons[(i + 1) % nWall];

			float  slope = (P1.y - P0.y) / (P1.x - P0.x);
			bool   cond1 = (Location.x >= P0.x && Location.x < P1.x) || (Location.x >= P1.x && Location.x < P0.x);
			float  y = slope * (Location.x - P0.x) + P0.y;
			bool above = y >= Location.y;

			if (cond1 && above)
			{
				nIntersec++;
			}
		}
	}

	return HitObj!=INVALID_OBJID || (nIntersec%2)>0;
}

kPoint Room::GetRight(int index)
{
	if (index >= 0 && index < InnerPolygons.size())
	{
		kPoint Right = InnerPolygons[index] - OuterPolygons[index];
		return Right.Normalize();
	}
	return kPoint();
}

bool Room::GetSegments(std::vector<int> &Polygons, std::vector<FSegmentPoint> &Points, bool bFloor)
{
	BuildCache();

	for (int i = 0; i < WallSegments.size(); ++i)
	{
		FWallSegment &Seg = WallSegments[i];
		Wall *pWall = SUITE_GET_BUILDING_OBJ(Seg.WallID, Wall);
		if (!pWall)
		{
			continue;
		}

		kPoint Right = pWall->GetRight();
		if (Seg.bInverseWall)
		{
			Right = -Right;
		}

		float Height = bFloor ? pWall->GetZPos() : (pWall->GetZPos() + pWall->GetHeight(0));

		if (Points.empty())
		{
			Polygons.push_back(0);
		}

		kPoint P0, P1;
		pWall->GetLocations(P0, P1);
		kPoint Forward = (P1 - P0).Normalize();
		kPoint SegForward = Seg.bInverseWall? - Forward : Forward;

		Points.push_back(FSegmentPoint(InnerPolygons[i], Right, SegForward, Height, Seg.bCW));

		std::vector<FWallHoleInfo> *pHoles =pWall->GetHoles();

		if (bFloor && pHoles && pHoles->size() > 0)
		{
			kPoint RightOffset = Right*pWall->GetThickRight();
			if (Seg.bInverseWall)
			{
				for (int iHole = pHoles->size()-1; iHole >=0 ; --iHole)
				{
					FWallHoleInfo &HoleInfo = (*pHoles)[iHole];
					if (HoleInfo.ObjType == EDoorHole)
					{
						kPoint startPos = P0 + Forward*HoleInfo.MaxX + RightOffset;
						kPoint  endPos  = P0 + Forward*HoleInfo.MinX + RightOffset;

						Points.push_back(FSegmentPoint(startPos, Right, SegForward, Height, Seg.bCW, HoleInfo.HoleID));

						//从洞的结束处开始
						int startIndex = (int)Points.size();
						Polygons.push_back(startIndex);
						Points.push_back(FSegmentPoint(endPos, Right, SegForward, Height, Seg.bCW, HoleInfo.HoleID));
					}
				}
			}
			else
			{
				for (int iHole = 0; iHole < pHoles->size(); ++iHole)
				{
					FWallHoleInfo &HoleInfo = (*pHoles)[iHole];
					if (HoleInfo.ObjType == EDoorHole)
					{

						kPoint startPos = P0 + Forward*HoleInfo.MinX + RightOffset;
						kPoint endPos = P0 + Forward*HoleInfo.MaxX + RightOffset;

						Points.push_back(FSegmentPoint(startPos, Right, SegForward, Height, Seg.bCW, HoleInfo.HoleID));

						//从洞的结束处开始
						int startIndex = (int)Points.size();
						Polygons.push_back(startIndex);
						Points.push_back(FSegmentPoint(endPos, Right, SegForward, Height, Seg.bCW, HoleInfo.HoleID));
					}
				}
			}
		}			
	}
	return true;
}



