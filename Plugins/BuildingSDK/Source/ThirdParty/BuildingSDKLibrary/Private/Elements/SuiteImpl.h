
#pragma once

#include <memory>
#include "ISuite.h"
#include "SparseMap.h"
#include <unordered_map>
#include "ITransaction.h"
#include "Class/ClassLibaray.h"
#include "Transaction/DRTransactionSystem.h"

typedef std::unordered_map<ObjectID, BuildingObject*>		ObjectMap;

class Corner;
class Room;
class Wall;
class Surface;
class BuildingConfig;

class SuiteImpl :public ISuite
{
public:
	SuiteImpl();
	~SuiteImpl();
	virtual ObjectID GetConfig();
	virtual ObjectID AddCorner(float x, float y);
	virtual ObjectID AddModelToObject(ObjectID BaseObjID, const char *ResID, const kVector3D &Location, const kRotation &Rotation = kRotation(), const kVector3D &Scale = kVector3D(1.0f), int Type = -1);
	virtual ObjectID AddModelToAnchor(ObjectID AnchorID, const char *ResID, const kVector3D &Location, const kRotation &Rotation = kRotation(), const kVector3D &Scale = kVector3D(1.0f), int Type = -1);

	virtual void AddWall(ObjectID StartCorner, ObjectID EndCorner, float ThickLeft, float ThickRight, float Height);
	virtual void AddVirtualWall(ObjectID StartCorner, ObjectID EndCorner);
	virtual ObjectID AddWindow(ObjectID WallID, const kPoint &Location, float zPos, float Width, float Height);
	virtual ObjectID AddDoor(ObjectID WallID, const kPoint &Location, float Width, float Height, float zPos = 0);

	virtual ObjectID GetConnectCorner(ObjectID Wall0, ObjectID Wall1, bool &bWall1InverseConnect);
	virtual BuildingObject *GetObject(ObjectID ObjID, EObjectType Class = EUnkownObject);
	virtual void DeleteObject(ObjectID ObjID, bool bForce = false);
	virtual int GetAllObjects(IObject **&ppObjects, EObjectType InClass = EUnkownObject);
	virtual void SetListener(ISuiteListener *Listener);
	virtual bool Move(ObjectID ObjID, const kPoint &DeltaMove);
	virtual void Serialize(ISerialize &Ar);
		
	virtual void Load(const char *Filename);
	virtual void Save(const char *Filename);

	virtual ObjectID GetWallByTwoCorner(ObjectID CornerID0, ObjectID CornerID1);
	virtual int  GetConnectWalls(ObjectID CorerID, ObjectID *&pConnectedWalls);
	virtual bool IsFree(ObjectID CornerID);
	virtual bool GetWallVector(ObjectID WallID, kPoint &P0, kPoint &P1, kPoint &Right);
	virtual bool GetWallForwardFromCorner(ObjectID WallID, ObjectID CornerID, kPoint &Forward);
	virtual bool GetWallBorderLines(ObjectID WallID, kLine &CenterLine, kLine &LeftLine, kLine &RightLine);
	virtual int  GetPolygon(ObjectID RoomID, kPoint *&pPolygons, bool bInner);
	virtual void GetRoomCorners(ObjectID RoomID, std::vector<ObjectID> &Corners);
	virtual kESnapType FindSnapLocation(const kPoint &TouchPos, kPoint &BestSnapLocation, ObjectID &BestObjID, ObjectID IgnoreObjID, float Tolerance = -1.0f);
	virtual ObjectID FindCloseWall(const kPoint &Location, float Width, kPoint &BestLoc, float Tolerance = -1.0f);

	virtual void Update();
	virtual void BuildRooms();
	virtual bool IsRoomCeilVisible();
	virtual void SetRoomCeilVisible(bool bVisible);
	virtual IMeshObject *GetMeshObject(ObjectID ID);
	virtual ObjectID SetSurface(ObjectID PrimitiveID, const char *MaterialName, int MaterialType, int SubModelIndex = -1);
	virtual void SetSurface(ObjectID PrimitiveID, ObjectID SurfaceID, int SubModelIndex = -1);
	virtual ObjectID GetSurface(ObjectID PrimitiveID, int SubModelIndex);
	virtual void RemoveSurface(ObjectID PrimitiveID, int SubModelIndex);
	virtual ObjectID HitTest(const kPoint &Location);
	virtual ObjectID GetRoomByLocation(const kPoint &Location);
	virtual int HitTest(const kPoint &Min, const kPoint &Max, ObjectID *&pResults);
	virtual IValue &GetProperty(ObjectID ID, const char *PropertyName);
	virtual void SetProperty(ObjectID ID, const char *PropertyName, const IValue *Value);
	virtual void SetValue(ObjectID ID, const char *PropertyName, IValue *Value);
	virtual IValue *FindValue(ObjectID ID, const char *PropertyName);
	void NotifySurfaceValueChanged(ObjectID PrimID, int SubSection);
	void Clean();
protected:
	BuildingConfig &_GetConfig();
	ITransact &GetTransaction();
	Corner *_AddCorner(float x, float y, bool bSnapCheck);
	Room *_GetRoomByLocation(const kPoint &Location, ObjectID &HitObj);
	void _AddWall(EObjectType WallType, ObjectID StartCorner, ObjectID EndCorner, float ThickLeft, float ThickRight, float Height);
	Room *AddRoom(Corner **ppCorners, int NumCorner);
	Room *AddRoom(ObjectID *pCorners, int NumCorner);
	void  AddSkirting(Room *pRoom);
	void UnLink(Surface *pSurface, ObjectID PrimID, int SubModelIndex);
	Wall *InnerAddWall(EObjectType WallType, ObjectID StartCorner, ObjectID EndCorner, float ThickLeft, float ThickRight, float Height, std::vector<struct FWallHoleInfo> *Holes);
	BuildingObject *CreateObject(EObjectType Class, bool bAllocID = true);
	void OnCreate(BuildingObject *Obj);
	void AddObject(BuildingObject *Obj);
	void DeleteObj(BuildingObject *Obj);
	void UpdateObj(BuildingObject *Obj);
	void GetPropWalls(std::vector<Wall *> &Walls, const kPoint &P0, const kPoint &P1);
	void SearchShortestArea(Wall *PreWall, Corner *pCorner, class FSearchContext &Context);
	void AddCornersToSparseMap(std::vector<Corner *> &Corners);
	void RemoveCornersFromSparseMap(std::vector<Corner *> &Corners);
	void ClearBuild();
	friend class FAreaComputeContext;
	friend class FAddWallTask;
	friend class FAddCornerTask;
	friend class FAddTask;
	friend class FDeleteTask;
	friend class FSavedInfo;
private:
	FClassLibaray				_ClassLibaray;
	ObjectID					_ConfigID;
	ObjectID					_IncrObjectID;
	ObjectMap					_ObjMap;
	ISuiteListener				*_Listener;
	FDRTransactionSystem		*_TransactionSystem;
	SparseMap					_SparseMap;
	std::vector<ObjectID>		_Rooms;
	std::vector<ObjectID>		_PendingAddObjects;
	bool						_bNeedUpdateAreas;
};


