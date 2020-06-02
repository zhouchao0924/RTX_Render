
#pragma once

#include "kLine.h"
#include "BuildingObject.h"

class Wall;
class Corner :public BuildingObject
{
	DEFIN_CLASS()
public:
	Corner();
	void Serialize(ISerialize &Ar);
	EObjectType GetType() { return ECorner; }
	bool IsDeletable() { return true; }
	void GetNearByWall(ObjectID WallID, ObjectID &LeftWall, ObjectID &RightWall);
	void GetNearByWallIndex(ObjectID WallID, int &LeftWall, int &RightWall);
	int GetWallIndex(ObjectID WallID);
	void SetLocation(const kPoint &Loc);
	const kPoint &GetLocation() { return Location; }
	void MarkNeedUpdate() override;
	void AddWall(Wall *pWall);
	void RemoveWall(ObjectID WallID);
	bool IsConnect(ObjectID CornerID);
	bool IsFree();
	void SetBaton(void *InBaton) { Baton = InBaton; }
	void *GetBaton() { return Baton; }
	bool Move(const kPoint &DeltaMove) override;
protected:
	bool IsWallDistValid();
	void ReSortConnectWalls();
	bool IsValid();
	bool IsValid(std::vector<float> &newAngles, int &iMin);
	void RecalcAngles(std::vector<float> &newAngles, std::vector<ObjectID> *connCorners);
public:
	kPoint					Location;
	std::vector<ObjectID>	ConnectedWalls;
	std::vector<float>		Angles;
	ObjectID				RelativeID;
	void					*Baton;
};


