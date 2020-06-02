
#pragma once

#include "kLine.h"
#include "ISuite.h"
#include "poly2tri/poly2tri.h"
#include "Primitive.h"
#include "WallHole.h"

enum EPinType
{
	ELeft0,
	ERight0,
	ELeft1,
	ERight1,
	EPinMax
};

enum EAreaType
{
	EInnerArea,
	EOuterArea,
	EAreaMax
};

struct FWallHoleInfo
{
	FWallHoleInfo() { ObjType = EBuildingObject; MinX = MaxX = 0; HoleID = INVALID_OBJID; }
	FWallHoleInfo(ObjectID ID, float InMinX, float InMax, EObjectType InType)
		:HoleID(ID)
		,MinX(InMinX)
		,MaxX(InMax)
		,ObjType(InType)
	{
	}
	void Serialize(ISerialize &Ar);
	ObjectID HoleID;
	float MinX, MaxX;
	EObjectType ObjType;
};

class Wall :public  Primitive
{
	DEFIN_CLASS()
public:
	Wall();
	void Serialize(ISerialize &Ar);
	bool IsDeletable() override { return true; }
	void GetLocations(kPoint &P0, kPoint &P1);
	kPoint GetForward();
	kPoint GetRight();
	void GetBorderLines(kLine &CenterLine, kLine &LeftLine, kLine &RightLine);
	void SetCorner(int CornerIndex, ObjectID NewCorner, std::vector<FWallHoleInfo> &OtherHoles);
	void MarkNeedUpdate() override;
	kPoint GetDirection(ObjectID From);
	ObjectID GetOtherCorner(ObjectID CornerID);
	void OnCreate();
	void OnDestroy();
	bool HitTest(const kPoint &Location, ObjectID &ObjID);
	void SetSolo() { RoomLeft = RoomRight = INVALID_OBJID; }
	bool IsSolo() { return RoomLeft == INVALID_OBJID && RoomRight == INVALID_OBJID; }
	ObjectID HitTestHole(float Dist);
	EObjectType GetType() { return EWall; }
	void SetBaton(void *InBaton) { Baton = InBaton; }
	void *GetBaton() { return Baton; }
	bool Move(const kPoint &DeltaMove) override;
	void GetCorners(std::vector<Corner *> &Corners) override;
	IValue *GetFunctionProperty(const std::string &name);
	virtual void SetWallInfo(float InThickLeft, float InThickRight, float Height0, float Height1) {}
	virtual float GetThickLeft() { return 0; }
	virtual float GetThickRight() { return 0; }
	virtual float GetHeight(int i) { return 0; }
	virtual float GetZPos() { return 0; }
	virtual void CopyHoles(std::vector<FWallHoleInfo> &Holes) {}
	virtual std::vector<FWallHoleInfo> *GetHoles() { return nullptr; }
	virtual void AddHole(WallHole *pHole, const kPoint &Location, float InZPos, float InHeight, float InWidth) {}
protected:
	void BuildCaps();
	void GetOriginalBorderLines(kLine &CenterLine, kLine &LeftLine, kLine &RightLine);
public:
	ObjectID					P[2];
	ObjectID					RoomLeft;
	ObjectID					RoomRight;
	kPoint						Points[EPinMax];
	bool						bCapsBuilded;
	void						*Baton;
};

class VirtualWall :public Wall
{
	DEFIN_CLASS()
public:
	EObjectType GetType() override { return EVirtualWall; }
};

class SolidWall :public Wall
{
	DEFIN_CLASS()
public:
	SolidWall();
	void Serialize(ISerialize &Ar);
	IValue *GetFunctionProperty(const std::string &name);
	void Build() override;
	void OnDestroy() override;
	void RemoveHoles();
	void UpdateWallHoles();
	float GetThickLeft() override { return ThickLeft; }
	float GetThickRight() override { return ThickRight; }
	float GetHeight(int i) override { return Height[i]; }
	float GetZPos() override { return ZPos; }
	EObjectType GetType() override { return ESolidWall; }
	void AddTris(FMeshSection *mesh, std::vector<p2t::Triangle*> &triangles, bool bInverse);
	void AddHole(WallHole *pHole, const kPoint &Location, float InZPos, float InHeight, float InWidth) override;
	void AddWallHoles(p2t::CDT *CDT, std::vector<ObjectID> &Holes, int &Offset, std::vector<p2t::IndexPoint*> &polyline);
	void BrushHoleMeshs(std::vector<ObjectID> &Holes, int &MaterialOffset, const kVector3D &Base, const kVector3D &Forward, const kVector3D &Right, float InThickLeft, float InThickRight);
	void SetWallInfo(float InThickLeft, float InThickRight, float Height0, float Height1) override;
	void CopyHoles(std::vector<FWallHoleInfo> &Holes) override;
	std::vector<FWallHoleInfo> *GetHoles() override { return &Holes; }
	void MarkNeedUpdate() override;
public:
	float						ThickLeft;
	float						ThickRight;
	float						Height[2];
	float						ZPos;
	bool						bMainWall;
	std::vector<FWallHoleInfo>	Holes;
};







