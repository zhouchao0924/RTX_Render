
#pragma once

#include "BuildingObject.h"

struct FWallSegment
{
	FWallSegment() { bCW = true; }
	ObjectID WallID;
	ObjectID SourceCornerID;
	bool	 bInverseWall;
	bool	 bCW;
	void Serialize(ISerialize &Ar);
};

struct FSegmentPoint
{
	FSegmentPoint() 
		:ID(INVALID_OBJID)
		,Height(0)
	{ 
	}
	FSegmentPoint(const kPoint &InP, const kPoint &InInner, const kPoint &InForward, float InHeight, bool bInCW, ObjectID InID = INVALID_OBJID)
		:Point(InP)
		,ID(InID)
		,Inner(InInner)
		,Forward(InForward)
		,Height(InHeight)
		,bCW(bInCW)
	{
	}
	float		Height;
	kPoint		Point;
	kPoint		Forward;
	kPoint		Inner;
	bool		bCW;
	ObjectID	ID;
};

class Room :public  BuildingObject
{
	DEFIN_CLASS()
public:
	Room();
	kPoint GetRight(int index);
	void Serialize(ISerialize &Ar);
	void Init(ObjectID *pCorners, int Count);
	int  GetPolygon(kPoint *&pPolygon, bool bInnerPolygon);
	void GetCorners(std::vector<Corner *> &Corners);
	void GetCorners(std::vector<ObjectID> &Corners);
	bool HasCorner(ObjectID CornerID);
	int  GetWallHeight(float *&pHeight);
	bool GetBox2D(kPoint &Orignal, kPoint &Range) override;
	void OnDestroy();
	EObjectType GetType() { return ERoom; }
	bool HitTest(const kPoint &Location, ObjectID &HitObj);
	void MarkNeedUpdate();
	bool HasEmptyWall();
	bool GetSegments(std::vector<int> &Polygons, std::vector<FSegmentPoint> &Points, bool bFloor);
protected:
	void BuildCache();
public:
	std::vector<FWallSegment> WallSegments;
	ObjectID				  FloorID;
	ObjectID				  CeilID;
	ObjectID				  SkirtingCielID;
	ObjectID				  SkirtingFloorID;
	std::vector<kPoint>		  InnerPolygons;
	std::vector<kPoint>		  OuterPolygons;
	std::vector<ObjectID>	  SubRooms;
	std::vector<float>		  Heights;
protected:
	bool					  bCachePolygons;
};


