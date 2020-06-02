
#pragma once

#include <vector>
#include "Math/kLine.h"
#include "Math/kVector2D.h"
#include "Math/kBox.h"
#include "ISerialize.h"
#include "IProperty.h"
#include "IClass.h"
#include "IObjectFactory.h"
#include "ISceneManager.h"

class ISuiteListener
{
public:
	virtual void OnAddObject(IObject *RawObj) = 0;
	virtual void OnDeleteObject(IObject *RawObj) = 0;
	virtual int  GetExternalChunkID() = 0;
	virtual void Serialize(ISerialize &Ar) = 0;
	virtual void OnUpdateObject(IObject *RawObj, unsigned int ChannelMask) = 0;
	virtual void OnUpdateSurfaceValue(IObject *RawObj, int SectionIndex, ObjectID Surface) = 0;
	virtual void OnSwitchLayer(ISuite *Suite, int LayerIndex) = 0;
};

enum kESnapType
{
	kENone = 0,
	kEDirX = 0x1,
	kEDirY = 0x2,
	kEWallBorder = 0x4,
	kEPt = 0x8,
};

enum ESnapFilter
{
	ESFCorner = 0x1,
	ESFBorderCorner = 0x2,
	ESFAllCorner = 0x3,
	ESFWallBorder = 0x8,
	ESFWallCenterLine = 0x10,
	ESFDirectionX = 0x20,
	ESFDirectionY = 0x40,
	ESFAll = 0xffffffff
};

#undef GetObject

class ISuite
{
public:
	virtual ~ISuite() { } 
	virtual int GetID() = 0;
	virtual IObject *GetConfig() = 0;
	virtual ObjectID AddCorner(float x, float y) = 0;
	virtual ObjectID AddModel(int ObjType, const kXform &Transform = kXform()) = 0;
	virtual ObjectID AddModel(ObjectID BaseObjID, const char *ResID, const kVector3D &Location, const kRotation &Rotation = kRotation(), const kVector3D &Size = kVector3D(0.0f), int Type = -1, bool bAutoLoad = false) = 0;

	virtual kArray<ObjectID> AddWall(ObjectID StartCorner, ObjectID EndCorner, float ThickLeft, float ThickRight, float Height = 0) = 0;
	virtual kArray<ObjectID> AddEdge(bool bHole, const kPoint &P0, const kPoint &P1, ObjectID SnapObj0 = INVALID_OBJID, ObjectID SnapObj1 = INVALID_OBJID, bool bBreak = false) = 0;
	virtual kArray<ObjectID> AddVirtualWall(ObjectID StartCorner, ObjectID EndCorner) = 0;
	virtual ObjectID AddWindow(ObjectID WallID, const kPoint &Location, float zPos, float Width, float Height, const char *WindowResID, const char *SillsResID, float Depth = 0) = 0;
	virtual ObjectID AddCornerWindow(ObjectID WallID0, ObjectID Wall1, float zPos, float Width, float Height, const char *WindowResID, const char *SillsResID, float Depth = 60.0f) = 0;
	virtual ObjectID AddDoor(ObjectID WallID, const kPoint &Location, float Width, float Height, float zPos = 0) = 0;
	virtual ObjectID AddPointLight(const kVector3D &Location, float SourceRadius, float SoftSourceRadius, float SourceLength, float Intensity, kColor LightColor, bool bCastShadow, float ColorTemperature = 6500.0f, float Highlight = 1.0f) = 0;
	virtual ObjectID AddSpotLight(const kVector3D &Location, const kRotation &Rotationn, float AttenuationRadius,  float SourceRadius, float SoftSourceRadius, float SourceLength, float InnerConeAngle, float OuterConeAngle, float Intensity, kColor LightColor, bool bCastShadow, float ColorTemperature = 6500.0f, float Highlight = 1.0f) = 0;
	virtual ObjectID AddStretchMesh(const char *MXShapeFilename, const kArray<kVector3D> &Path, const kVector3D &StartForward = kVector3D(0), const kVector3D &EndFoward = kVector3D(0)) = 0;

	virtual ObjectID Break(ObjectID EdgeID,const kPoint &Location) = 0;
	virtual bool GetEdgeBoundary(ObjectID EdgeID, kArray<ObjectID> &Edges, kArray<unsigned char> &InverseFlags) = 0;
	virtual ObjectID GetConnectCorner(ObjectID Wall0, ObjectID Wall1, bool &bWall0InverseConnect) = 0;
	virtual ObjectID FindCloseWall(const kPoint &Location, float Width, kPoint &BestLoc, float Tolerance = -1.0f) = 0;	
	virtual bool FindWallCorner(const kPoint &Location, float Width, ObjectID &Wall0, ObjectID &Wall1, float Tolerance = -1.0f) = 0;

	virtual IObject *GetObject(ObjectID ObjID, EObjectType InClass = EUnkownObject) = 0;
	virtual int  GetAllObjects(IObject **&ppObjects, EObjectType InClass = EUnkownObject, bool bIncludeDeriveType = true) = 0;
	virtual bool Move(ObjectID ObjID, const kPoint &DeltaMove) = 0;
	virtual void DeleteObject(ObjectID ObjID, bool bForce = false) = 0;
	virtual bool IsFree(ObjectID CornerID) = 0;
	virtual ObjectID GetWallByTwoCorner(ObjectID CornerID0, ObjectID CornerID1) = 0;
	virtual int  GetConnectWalls(ObjectID CorerID, ObjectID *&pConnectedWalls) = 0;		
	virtual bool GetWallForwardFromCorner(ObjectID WallID, ObjectID CornerID, kPoint &Forward) = 0;
	virtual bool GetWallVector(ObjectID WallID, kPoint &P0, kPoint &P1, kPoint &Right) = 0;
	virtual bool GetWallBorderLines(ObjectID WallID, kLine2D &CenterLine, kLine2D &LeftLine, kLine2D &RightLine) = 0;
	virtual int  GetPolygon(ObjectID RoomID, kPoint *&pPolygons, bool bInner) = 0;
	virtual void GetRoomCorners(ObjectID RoomID, std::vector<ObjectID> &Corners) = 0;
	virtual void SetListener(ISuiteListener *Listener) = 0;
	virtual void Serialize(ISerialize &Ar) = 0;

	virtual bool Load(const char *Filename) = 0;
	virtual void Save(const char *Filename) = 0;
	virtual IObjectFactory *GetObjectFactory() = 0;
	
	virtual unsigned int FindSnapLocation(const kPoint &TouchPos, kPoint &BestLocation, ObjectID &BestObjID, ObjectID IgnoreObjID, ESnapFilter Filter = ESFAll, float Tolerance = -1.0f, float ToleranceEdge = -1.0f) = 0;
	virtual void Clean() = 0;
	virtual bool Flush() = 0;
	virtual void BuildRooms() = 0;
	virtual bool IsRoomCeilVisible() = 0;
	virtual void SetRoomCeilVisible(bool bVisible) = 0;
	virtual IMeshObject *GetMeshObject(ObjectID ID) = 0;
	virtual ObjectID SetSurface(ObjectID PrimitiveID, const char *MaterialName, int MaterialType, int SectionIndex = -1) = 0;
	virtual void SetSurface(ObjectID PrimitiveID, ObjectID SurfaceID, int SectionIndex = -1) = 0;
	virtual ObjectID GetSurface(ObjectID PrimitiveID, int SubModelIndex) = 0;
	virtual void RemoveSurface(ObjectID PrimitiveID, int SubModelIndex) = 0;
	virtual ObjectID HitTest(const kPoint &Location) = 0;
	virtual ObjectID GetAreaByLocation(const kPoint &Location) = 0;
	virtual int HitTest(const kPoint &Min, const kPoint &Max, ObjectID *&pResults) = 0;
	virtual IValue &GetProperty(ObjectID ID, const char *PropertyName) = 0;
	virtual void SetProperty(ObjectID ID, const char *PropertyName, const IValue *Value) = 0;
	virtual void SetValue(ObjectID ID, const char *PropertyName, IValue *Value) = 0;
	virtual IValue *FindValue(ObjectID ID, const char *PropertyName) = 0;
	virtual bool IsMoveValid(ObjectID CornerID, const kPoint &MoveDelta) = 0;
	virtual ObjectID FindBestAnchor(const kVector3D &Original, const kVector3D &Direction, float RayDist = 10000.0f) = 0;

	virtual kBox3D GetBounds() = 0;
	virtual ObjectID SwitchToLayer(int LayerIndex) = 0;
	virtual ObjectID AddLayer(int InsertPos = -1) = 0;
	virtual void RemoveLayer(int LayerIndex) = 0;
	virtual int GetLayerCount() = 0;
	virtual ObjectID GetLayer(int LayerIndex) = 0;
	virtual ISceneManager *GetSceneManager() = 0;
};

#define SUITE_GET_BUILDING_OBJ(ID, classtype) (classtype *)_Suite->GetObject(ID, E##classtype);
#define GET_BUILDING_OBJ(suite,ID, classtype) (classtype *)suite->GetObject(ID, E##classtype);

typedef void * (*LoadSDKFunctionType)();
#define  Export_SDKFunaction  ("LoadSDKFunction")
