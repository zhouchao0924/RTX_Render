
#include <map>
#include "SuiteImpl.h"
#include "Wall.h"
#include "Window.h"
#include "Corner.h"
#include "Room.h"
#include "PinCorner.h"
#include "DoorHole.h"
#include "Surface.h"
#include "BuildingConfig.h"
#include "PlanePrimitive.h"
#include "ModelInstance.h"
#include "Skirting.h"
#include <assert.h>
#include "BuildingConfig.h"
#include <math.h>
#include "IBuildingSDK.h"
#include "ITransaction.h"
#include "MXFile/MXFile.h"
#include "Class/PValue.h"

#define OBJ_CHUNK	1

SuiteImpl::SuiteImpl()
	: _IncrObjectID(1)
	, _Listener(NULL)
	, _bNeedUpdateAreas(false)
	, _ConfigID(INVALID_OBJID)
	, _TransactionSystem(nullptr)
{
	_ClassLibaray.InitClassLibaray();

	BuildingConfig *pConfig = (BuildingConfig *)CreateObject(EBuildingConfig, true);
	if (pConfig)
	{
		_ConfigID = pConfig->GetID();
		OnCreate(pConfig);
	}

	ITransact *Transaction = GetBuildingSDK()->GetTransaction();
	if (Transaction)
	{
		_TransactionSystem = new FDRTransactionSystem(this);
		Transaction->AddSubSystem(_TransactionSystem);
	}
}

SuiteImpl::~SuiteImpl()
{
	ITransact *Transaction = GetBuildingSDK()->GetTransaction();
	if (Transaction && _TransactionSystem)
	{
		Transaction->RemoveSubSystem(_TransactionSystem);
		delete _TransactionSystem;
		_TransactionSystem = nullptr;
	}
}

ObjectID SuiteImpl::GetConfig()
{
	return _ConfigID;
}

BuildingConfig &SuiteImpl::_GetConfig()
{
	return *GET_BUILDING_OBJ(this, _ConfigID, BuildingConfig);
}

ITransact &SuiteImpl::GetTransaction()
{
	return  *GetBuildingSDK()->GetTransaction();
}

BuildingObject *SuiteImpl::CreateObject(EObjectType Class, bool bAllocID /*= true*/)
{
	BuildingObject *NewObj = (BuildingObject *)_ClassLibaray.CreateObject(Class);
	if (NewObj)
	{
		if (bAllocID)
		{
			++_IncrObjectID;
			NewObj->_ID = _IncrObjectID;
		}
		NewObj->_Suite = this;
	}
	return NewObj;
}

void SuiteImpl::OnCreate(BuildingObject *Obj)
{
	if (Obj && Obj->_ID != INVALID_OBJID)
	{
		AddObject(Obj);
		Obj->OnCreate();	
	}
}

void SuiteImpl::AddObject(BuildingObject *Obj)
{
	if (Obj && Obj->_ID != INVALID_OBJID)
	{
		_ObjMap[Obj->_ID] = Obj;
		_PendingAddObjects.push_back(Obj->_ID);
		if (Obj->IsA(ECorner))
		{
			Corner *pCorner = (Corner *)Obj;
			_SparseMap.Add(pCorner);
		}
	}
}

void SuiteImpl::DeleteObj(BuildingObject *Obj)
{
	if (Obj)
	{
		//clear surface
		if (IMeshObject *Mesh = Obj->GetMeshObject(0, false))
		{
			int Num = Mesh->GetSectionCount();
			for (int i = 0; i < Num; ++i)
			{
				ObjectID SurfaceID = Mesh->GetSectionSurfaceID(i);
				Surface *pSurface = GET_BUILDING_OBJ(this, SurfaceID, Surface);
				UnLink(pSurface, Obj->GetID(), i);
			}
		}

		if (Obj->IsA(ECorner))
		{
			Corner *pCorner = (Corner *)Obj;
			_SparseMap.Del(pCorner);
		}

		Obj->OnDestroy();

		if (_Listener && Obj)
		{
			_Listener->OnDeleteObject(Obj);
		}

		int ObjID = Obj->GetID();
		ObjectMap::iterator it = _ObjMap.find(ObjID);
		if (it != _ObjMap.end())
		{
			_ObjMap.erase(it);
		}

		delete Obj;
	}
}

void SuiteImpl::UpdateObj(BuildingObject *Obj)
{
	if (Obj)
	{
		if (_Listener)
		{
			_Listener->OnUpdateObject(Obj);
		}
	}
}

void SuiteImpl::Update()
{
	if (_Listener)
	{
		for (size_t i = 0; i < _PendingAddObjects.size(); ++i)
		{
			BuildingObject *pObj = GetObject(_PendingAddObjects[i]);
			_Listener->OnAddObject(pObj);
		}

		for (ObjectMap::iterator it = _ObjMap.begin(); it != _ObjMap.end(); ++it)
		{
			BuildingObject *pObj = it->second;
			if (pObj && pObj->bNeedUpdate)
			{
				pObj->bNeedUpdate = false;
				_Listener->OnUpdateObject(pObj);
			}
		}
	}

	_PendingAddObjects.clear();
}

bool IsLeftInsert(ISuite *Suite, ObjectID PrePt, ObjectID NextPt)
{
	Corner *Corner0 = GET_BUILDING_OBJ(Suite, PrePt, Corner);
	Corner *Corner1 = GET_BUILDING_OBJ(Suite, NextPt, Corner);
	if (Corner0 && Corner1)
	{
		kPoint Pt0 = Corner0->GetLocation();
		kPoint Pt1 = Corner1->GetLocation();
		if (Pt0.x > Pt1.x || (Pt0.x == Pt1.x && Pt0.y > Pt1.y))
		{
			return true;
		}
	}
	return false;
}

struct FWallContext
{
	FWallContext()
	{
		Area_RL[0] = -1;
		Area_RL[1] = -1;
	}
	int Area_RL[2];
};

struct FCornerContext
{
	FCornerContext()
		:bVisited(false)
	{
	}
	bool bVisited;
};

struct FRoomContext
{
	std::vector<Corner *> CCB;
};

class FSearchContext
{
public:
	FSearchContext()
	{
		bCheckCCW = false;
	}
	int find(ObjectID id)
	{
		for (int k = 0; k < (int)PathCorners.size(); ++k)
		{
			if (id == PathCorners[k]->GetID())
			{
				return k;
			}
		}
		return -1;
	}

	int addRoom(int from)
	{
		FRoomContext *room = new FRoomContext();

		for (int i = from; i < (int)PathCorners.size(); ++i)
		{
			Corner *pCorner = PathCorners[i];
			setVisited(pCorner, true);
			room->CCB.push_back(pCorner);
		}

		int n = (int)Rooms.size();
		Rooms.push_back(room);

		return n;
	}

	void addPath(Corner *pCorner)
	{
		PathCorners.push_back(pCorner);
	}

	void markWall(ISuite *Suite, int roomIndex)
	{
		FRoomContext *room = Rooms[roomIndex];

		for (size_t i = 0; i < room->CCB.size(); ++i)
		{
			Corner *p0 = room->CCB[i];
			Corner *p1 = room->CCB[(i + 1) % room->CCB.size()];

			for (size_t k = 0; k < p0->ConnectedWalls.size(); ++k)
			{
				int foundIndex = -1;

				Wall *pWall = GET_BUILDING_OBJ(Suite, p0->ConnectedWalls[k], Wall);
				for (int j = 0; j < 2; ++j)
				{
					if (pWall->P[j] == p1->GetID())
					{
						foundIndex = (j + 1) % 2;
						break;
					}
				}

				if (foundIndex >= 0)
				{
					FWallContext *wallContext = (FWallContext *)pWall->GetBaton();
					if (wallContext == nullptr)
					{
						wallContext = new FWallContext();
						pWall->SetBaton(wallContext);
					}
					wallContext->Area_RL[foundIndex] = roomIndex;
					break;
				}
			}
		}
	}

	int getRoomID(Wall *pWall, int area_index)
	{
		int roomIndex = -1;
		FWallContext *wallContext = (FWallContext *)pWall->GetBaton();
		if (wallContext && area_index >= 0)
		{
			roomIndex = wallContext->Area_RL[area_index];
		}
		return roomIndex;
	}

	void setVisited(Corner *pCorner, bool bVisited)
	{
		FCornerContext *corner_context = (FCornerContext *)pCorner->GetBaton();

		if (bVisited && corner_context == nullptr)
		{
			corner_context = new FCornerContext();
			pCorner->SetBaton(corner_context);
		}

		if (corner_context)
		{
			corner_context->bVisited = bVisited;
		}
	}

	bool isVisited(Corner *pCorner)
	{
		FCornerContext *corner_context = (FCornerContext *)pCorner->GetBaton();
		return corner_context && corner_context->bVisited;
	}

	~FSearchContext()
	{
		for (size_t i = 0; i < Rooms.size(); ++i)
		{
			delete Rooms[i];
		}
		Rooms.clear();

		for (size_t i = 0; i < PathCorners.size(); ++i)
		{
			setVisited(PathCorners[i], false);
		}
	}

	int getCornerCount() { return (int)PathCorners.size(); }

	struct State
	{
		bool	bSavedCheckCCW;
		int		StartIndex;
	};

	void popState()
	{
		StateStack.pop_back();
	}

	void pushState()
	{
		State state;
		state.bSavedCheckCCW = bCheckCCW;
		state.StartIndex = (int)PathCorners.size();
		StateStack.push_back(state);
	}

	void useState()
	{
		State state = StateStack.back();
		bCheckCCW = state.bSavedCheckCCW;
		PathCorners.resize(state.StartIndex);
	}

	std::vector<FRoomContext *> Rooms;
	std::vector<Corner *>		PathCorners;
	bool						bCheckCCW;
	std::vector<State>			StateStack;
};

void SuiteImpl::SearchShortestArea(Wall *PreWall, Corner *pCorner, FSearchContext &Context)
{
	Context.addPath(pCorner);
	std::vector<ObjectID> walls;
	std::vector<float>    angles;
	std::vector<float>	  projDist;

	if (PreWall)
	{
		ObjectID WallID = PreWall->GetID();
		int index = pCorner->GetWallIndex(WallID);
		int n = (int)pCorner->ConnectedWalls.size();

		if (n > 1)
		{
			walls.resize(n - 1);
			angles.resize(n - 1);
			projDist.resize(n - 1);

			float wallAngle = pCorner->Angles[index];

			kPoint PreWallFoward;
			GetWallForwardFromCorner(PreWall->GetID(), pCorner->GetID(), PreWallFoward);
			PreWallFoward = -PreWallFoward;

			int k = 0;
			for (size_t i = 0; i < pCorner->ConnectedWalls.size(); ++i)
			{
				ObjectID Id = pCorner->ConnectedWalls[i];
				if (Id != WallID)
				{
					kPoint Forward;
					GetWallForwardFromCorner(Id, pCorner->GetID(), Forward);
					float fDot = Forward.Dot(PreWallFoward);
					float fCross = kPoint::CrossProduct(PreWallFoward, Forward);
					if (fCross < -0.001f)
					{
						fDot += 1.0f;
					}
					projDist[k] = fDot;
					walls[k] = Id;
					++k;
				}
			}

			//相对角度排序
			for (size_t i = 0; i < walls.size(); ++i)
			{
				for (size_t j = walls.size() - 1; j > i; --j)
				{
					if (std::abs(projDist[j]) < std::abs(projDist[j - 1]))
					{
						std::swap(projDist[j], projDist[j - 1]);
						std::swap(walls[j], walls[j - 1]);
					}
				}
			}
		}
	}
	else
	{
		walls = pCorner->ConnectedWalls;
	}

	Context.pushState();

	//按照优先级查找封闭的区域
	for (size_t i = 0; i < walls.size(); ++i)
	{
		Context.useState();

		ObjectID WallID = walls[i];
		if (PreWall && WallID == PreWall->GetID())
		{
			continue;
		}

		Wall *Best_Wall = GET_BUILDING_OBJ(this, WallID, Wall);
		if (!Best_Wall)
		{
			continue;
		}

		bool bFoundClosed = false;
		int area_index = Best_Wall->P[0] == pCorner->GetID() ? 0 : 1;
		int room_index = Context.getRoomID(Best_Wall, area_index);
		if (room_index != -1)
		{
			continue;
		}

		ObjectID NextCornerID = Best_Wall->P[(area_index + 1) % 2];
		Corner *NextCorner = GET_BUILDING_OBJ(this, NextCornerID, Corner);
		if (Context.isVisited(NextCorner))
		{
			continue;
		}

		if (Context.PathCorners.size() >= 2)
		{
			int n = (int)Context.PathCorners.size();
			kPoint V0 = (Context.PathCorners[n - 1]->GetLocation() - Context.PathCorners[n - 2]->GetLocation()).Normalize();
			kPoint V1 = (NextCorner->GetLocation() - Context.PathCorners[n - 1]->GetLocation()).Normalize();
			float fDot = kPoint::CrossProduct(V0, V1);

			if (std::abs(fDot) > 0.01f)
			{
				if (!Context.bCheckCCW)
				{
					Context.bCheckCCW = true;
					if (fDot <= 0)
					{
						continue;
					}
				}
				else
				{
					if (fDot <= 0)//再次开启检测
					{
						Context.bCheckCCW = false;
					}
				}
			}
		}

		int FoundIndex = Context.find(NextCornerID);

		if (FoundIndex >= 0)
		{
			if (FoundIndex == 0)
			{
				int newRoom = Context.addRoom(FoundIndex);
				Context.markWall(this, newRoom);
			}
			break;
		}
		else
		{
			SearchShortestArea(Best_Wall, NextCorner, Context);
			if (Context.isVisited(pCorner))
			{
				break;
			}
		}
	}

	Context.popState();
}

class FSavedInfo
{
public:
	struct FWallInfo
	{
		ObjectID RoomLeft;
		ObjectID RoomRight;
	};
	std::vector<ObjectID>			Rooms;
	std::map<Wall*, FWallInfo *>	Walls;

	FSavedInfo(SuiteImpl *InSuite)
		:impl(InSuite)
	{
	}

	~FSavedInfo()
	{
		Clear();
	}

	void Clone()
	{
		for (ObjectMap::iterator it = impl->_ObjMap.begin(); it != impl->_ObjMap.end(); ++it)
		{
			BuildingObject *pObj = it->second;
			if (pObj && pObj->IsA(EWall))
			{
				Wall *pWall = (Wall *)pObj;
				FWallInfo *pWallInfo = new FWallInfo();
				pWallInfo->RoomLeft = pWall->RoomLeft;
				pWallInfo->RoomRight = pWall->RoomRight;
				pWall->SetSolo();
				Walls[pWall] = pWallInfo;
			}
		}
		Rooms.swap(impl->_Rooms);
	}

	ObjectID FindRoomByCorners(std::vector<Corner *> &Corners)
	{
		for (size_t i = 0; i < Rooms.size(); ++i)
		{
			ObjectID roomID = Rooms[i];
			Room *pRoom = GET_BUILDING_OBJ(impl, roomID, Room);
			if (pRoom)
			{
				size_t j = 0;
				for (; j < Corners.size(); ++j)
				{
					if (!pRoom->HasCorner(Corners[j]->GetID()))
					{
						break;
					}
				}

				if (j >= Corners.size())
				{
					return pRoom->GetID();
				}
			}
		}

		return INVALID_OBJID;
	}

	void RestoreRoom(ObjectID roomID)
	{
		size_t i = 0;
		for (; i < Rooms.size() && Rooms[i] != roomID; ++i);

		Room *pRoom = GET_BUILDING_OBJ(impl, roomID, Room);
		if (pRoom)
		{
			for (size_t j = 0; j < pRoom->WallSegments.size(); ++j)
			{
				FWallSegment &Segment = pRoom->WallSegments[j];
				Wall *pWall = GET_BUILDING_OBJ(impl, Segment.WallID, Wall);

				std::map<Wall*, FWallInfo *>::iterator it = Walls.find(pWall);
				if (it != Walls.end())
				{
					FWallInfo *WallInfo = it->second;
					if (WallInfo->RoomLeft == roomID)
					{
						pWall->RoomLeft = roomID;
					}
					else if (WallInfo->RoomRight == roomID)
					{
						pWall->RoomRight = roomID;
					}
				}
			}

			impl->_Rooms.push_back(roomID);
			Rooms.erase(Rooms.begin() + i);

			pRoom->MarkNeedUpdate();
		}
	}

	void Clear()
	{
		for (std::map<Wall*, FWallInfo *>::iterator it = Walls.begin(); it != Walls.end(); ++it)
		{
			FWallInfo *WallInfo = it->second;
			delete WallInfo;
		}

		for (size_t i = 0; i < Rooms.size(); ++i)
		{
			impl->DeleteObject(Rooms[i]);
		}

		Walls.clear();
	}
private:
	SuiteImpl *impl;
};

void SuiteImpl::BuildRooms()
{
	FSavedInfo SavedInfo(this);

	SavedInfo.Clone();

	for (ObjectMap::iterator it = _ObjMap.begin(); it != _ObjMap.end(); ++it)
	{
		FSearchContext Context;
		BuildingObject *pObj = (BuildingObject *)it->second;
		if (pObj && pObj->IsA(ECorner))
		{
			Corner *pCorner = (Corner *)pObj;
			SearchShortestArea(nullptr, pCorner, Context);

			std::vector<ObjectID> room_ids;
			room_ids.resize(Context.Rooms.size());

			for (size_t iroom = 0; iroom < Context.Rooms.size(); ++iroom)
			{
				FRoomContext *room = Context.Rooms[iroom];
				int nCorner = (int)room->CCB.size();
				
				ObjectID FoundRoomID = SavedInfo.FindRoomByCorners(room->CCB);
				if (FoundRoomID != INVALID_OBJID)
				{
					SavedInfo.RestoreRoom(FoundRoomID);
				}
				else
				{
					AddRoom(&room->CCB[0], nCorner);
				}
			}
		}
	}

	ClearBuild();
}

bool SuiteImpl::IsRoomCeilVisible()
{
	return _GetConfig().bCeilVisible;
}

void SuiteImpl::SetRoomCeilVisible(bool bVisible)
{
	_GetConfig().bCeilVisible = bVisible;

	for (size_t i = 0; i < _Rooms.size(); ++i)
	{
		Room *pRoom = GET_BUILDING_OBJ(this, _Rooms[i], Room);
		if (pRoom)
		{
			PlanePrimitive *pPlane = GET_BUILDING_OBJ(this, pRoom->CeilID, CeilPlane);
			if (pPlane)
			{
				pPlane->SetVisible(bVisible);
				pPlane->MarkNeedUpdate();
			}
		}
	}
}

IMeshObject *SuiteImpl::GetMeshObject(ObjectID ID)
{
	BuildingObject *pObj = GetObject(ID);
	if (pObj)
	{
		return pObj->GetMeshObject(0);
	}
	return nullptr;
}

void SuiteImpl::ClearBuild()
{
	for (ObjectMap::iterator it = _ObjMap.begin(); it != _ObjMap.end(); ++it)
	{
		BuildingObject *pObj = it->second;
		if (!pObj->IsA(EWall))
		{
			continue;
		}

		if (Wall *pWall = (Wall *)pObj)
		{
			FWallContext *wall_context = (FWallContext*)pWall->GetBaton();
			if (wall_context)
			{
				delete wall_context;
				pWall->SetBaton(nullptr);
			}
		}
	}
}

ObjectID SuiteImpl::AddCorner(float x, float y)
{
	if (isnan(x) || isnan(y))
	{
		return INVALID_OBJID;
	}

	kPoint BestSnapPos;
	ObjectID BestObj;
	Corner *pCorner = nullptr;
	if (kEPt == FindSnapLocation(kPoint(x, y), BestSnapPos, BestObj, INVALID_OBJID))
	{
		pCorner = GET_BUILDING_OBJ(this, BestObj, Corner);
		assert(pCorner != nullptr);
	}

	if (!pCorner)
	{
		pCorner = _AddCorner(x, y, false);
	}

	return pCorner->GetID();
}

Corner *SuiteImpl::_AddCorner(float x, float y, bool bSnapCheck)
{
	if (isnan(x) || isnan(y))
	{
		return nullptr;
	}

	kPoint BestSnapPos;
	ObjectID BestObj;
	Corner *pCorner = nullptr;

	if (bSnapCheck && kEPt == FindSnapLocation(kPoint(x, y), BestSnapPos, BestObj, INVALID_OBJID))
	{
		pCorner = GET_BUILDING_OBJ(this, BestObj, Corner);
		assert(pCorner != nullptr);
	}
	else
	{
		pCorner = (Corner *)CreateObject(ECorner, true);
		if (pCorner)
		{
			pCorner->Location = kPoint(x, y);
			OnCreate(pCorner);
		}
	}
	return pCorner;
}

ObjectID SuiteImpl::AddModelToObject(ObjectID BaseObjID, const char *ResID, const kVector3D &Location, const kRotation &Rotation, const kVector3D &Scale, int Type /*= -1*/)
{
	Anchor * pAnchor = nullptr;

	if (BaseObjID != INVALID_OBJID)
	{
		BuildingObject *pObj = GetObject(BaseObjID);
		if (pObj && pObj->GetMeshObject(0))
		{
			Primitive *pPrim = (Primitive *)pObj;
			pAnchor = pPrim->CreateAnchor(Location);
			if (pAnchor)
			{
				pPrim->Anchors.push_back(pAnchor->GetID());
			}
		}
	}

	ModelInstance *pModel = ResID ? (ModelInstance *)CreateObject(EModelInstance, true) : nullptr;
	if (pModel)
	{
		if (pAnchor)
		{
			pAnchor->Link(pModel);
		}
		else
		{
			pModel->Location = Location;
			pModel->Scale = Scale;
			pModel->Forward = (*((kVector3D*)&Rotation)).ToDirection();
		}
			
		pModel->Type = Type;
		pModel->ResID = ResID;

		OnCreate(pModel);
	}

	return pModel ? pModel->GetID() : INVALID_OBJID;
}

ObjectID SuiteImpl::AddModelToAnchor(ObjectID AnchorID, const char *ResID, const kVector3D &Location, const kRotation &Rotation, const kVector3D &Scale, int Type /*= -1*/)
{
	ModelInstance *pModel = nullptr;
	Anchor * pAnchor = GET_BUILDING_OBJ(this, AnchorID, Anchor);

	if (pAnchor)
	{
		pModel = ResID ? (ModelInstance *)CreateObject(EModelInstance, true) : nullptr;
		if (pModel)
		{
			pModel->Type = Type;
			pAnchor->Link(pModel);
			pModel->ResID = ResID;
			OnCreate(pModel);
		}
	}

	return pModel ? pModel->GetID() : INVALID_OBJID;
}

void SuiteImpl::GetPropWalls(std::vector<Wall *> &Walls, const kPoint &P0, const kPoint &P1)
{
	for (ObjectMap::iterator it = _ObjMap.end(); it != _ObjMap.end(); ++it)
	{
		BuildingObject *pObj = it->second;
		if (pObj->IsA(EWall))
		{
			Walls.push_back((Wall *)pObj);
		}
	}
}

inline bool IsBetweenPoint(const kPoint &P, const kPoint &P0, const kPoint &P1, float DstSQ)
{
	return (P0 - P).SizeSquared() < DstSQ && (P1 - P).SizeSquared() < DstSQ;
}

Wall *SuiteImpl::InnerAddWall(EObjectType WallType, ObjectID StartCorner, ObjectID EndCorner, float ThickLeft, float ThickRight, float Height, std::vector<FWallHoleInfo> *Holes)
{
	if (StartCorner == EndCorner)
	{
		return NULL;
	}

	ObjectID FoundWallID = GetWallByTwoCorner(StartCorner, EndCorner);
	if (FoundWallID != INVALID_OBJID)
	{
		return NULL;
	}

	Wall * pNewWall = nullptr;

	Corner *s = GET_BUILDING_OBJ(this, StartCorner, Corner);
	Corner *e = GET_BUILDING_OBJ(this, EndCorner, Corner);

	if (s && e)
	{
		pNewWall = (Wall *)CreateObject(WallType, true);

		if (pNewWall)
		{
			pNewWall->P[0] = StartCorner;
			pNewWall->P[1] = EndCorner;
			pNewWall->SetWallInfo(ThickLeft, ThickRight, Height, Height);
			
			if (Holes)
			{
				pNewWall->CopyHoles(*Holes);
			}

			s->AddWall(pNewWall);
			e->AddWall(pNewWall);

			OnCreate(pNewWall);

			BuildingConfig &cfg = _GetConfig();
			if (cfg.WallMaterial.length() > 0)
			{
				SetSurface(pNewWall->GetID(), cfg.WallMaterial.c_str(), cfg.WallMaterialType);
			}
		}
	}

	_bNeedUpdateAreas = true;

	return pNewWall;
}

void SuiteImpl::AddWall(ObjectID StartCorner, ObjectID EndCorner, float ThickLeft, float ThickRight, float Height)
{
	if (StartCorner != EndCorner)
	{
		_AddWall(ESolidWall, StartCorner, EndCorner, ThickLeft, ThickRight, Height);
	}
}

void SuiteImpl::AddVirtualWall(ObjectID StartCorner, ObjectID EndCorner)
{
	if (StartCorner != EndCorner)
	{
		_AddWall(EVirtualWall, StartCorner, EndCorner, 0, 0, 0);
	}
}

void SuiteImpl::_AddWall(EObjectType WallType, ObjectID StartCorner, ObjectID EndCorner, float ThickLeft, float ThickRight, float Height)
{
	Wall   *pNewWall = nullptr;
	Corner *pCornerEnd = nullptr;

	Corner *s = GET_BUILDING_OBJ(this, StartCorner, Corner);
	Corner *e = GET_BUILDING_OBJ(this, EndCorner, Corner);
	if (s && e)
	{
		kPoint P0 = s->GetLocation();
		kPoint P1 = e->GetLocation();

		//得到可选墙体
		std::vector<FWallHoleInfo> Holes;
		std::vector<ObjectID> Splits;
		std::vector<float>	  SplitDists;
		Splits.push_back(EndCorner);
		SplitDists.push_back((P1 - P0).SizeSquared());

		std::vector<Wall *> Walls;
		for (ObjectMap::iterator it = _ObjMap.begin(); it != _ObjMap.end(); ++it)
		{
			BuildingObject *pObj = it->second;
			if (pObj && pObj->IsA(EWall))
			{
				Walls.push_back((Wall *)pObj);
			}
		}

		for (size_t i = 0; i < Walls.size(); ++i)
		{
			Wall *pWall = Walls[i];

			kPoint wP0, wP1;
			pWall->GetLocations(wP0, wP1);

			kPoint wD = wP1 - wP0;
			float wSQL = wD.SizeSquared(); //墙体长度为零
			if (wSQL <= 0)
			{
				continue;
			}

			float  wL = std::sqrt(wSQL);
			kPoint nD = wD*(1.0f / wL);

			//连接线上
			kPoint DL0 = P0 - wP0;
			float  fDot0 = DL0.Dot(nD);
			kPoint Dir = DL0.Normalize();
			if (std::abs(Dir.Dot(nD)) > 0.99f && IsBetweenPoint(P0, wP0, wP1, wSQL))
			{
				kPoint NewPos = wP0 + nD*fDot0;
				s->SetLocation(NewPos);

				ObjectID CornerID = pWall->P[1];
				pWall->SetCorner(1, StartCorner, Holes);
				
				InnerAddWall(pWall->GetType(), StartCorner, CornerID, pWall->GetThickLeft(), pWall->GetThickRight(), pWall->GetHeight(0), &Holes);
				continue;
			}

			kPoint DL1 = P1 - wP0;
			float fDot1 = DL1.Dot(nD);
			Dir = DL1.Normalize();
			if (std::abs(Dir.Dot(nD)) > 0.99f && IsBetweenPoint(P1, wP0, wP1, wSQL))
			{
				kPoint NewPos = wP0 + nD*fDot1;
				e->SetLocation(NewPos);

				ObjectID CornerID = pWall->P[1];
				pWall->SetCorner(1, EndCorner, Holes);

				InnerAddWall(pWall->GetType(), EndCorner, CornerID, pWall->GetThickLeft(), pWall->GetThickRight(), pWall->GetHeight(0), &Holes);
				continue;
			}

			//检测相交
			kPoint wD0 = P0 - wP0;
			kPoint wD1 = P1 - wP0;
			float c0 = kPoint::CrossProduct(wD, wD0);
			float c1 = kPoint::CrossProduct(wD, wD1);

			if (c0*c1 > 0)	//不相交
			{
				continue;
			}

			kPoint wProj0 = wP0 + nD*(wD0.Dot(nD));
			kPoint wProj1 = wP0 + nD*(wD1.Dot(nD));

			float d0 = (wProj0 - P0).Size();
			float d1 = (wProj1 - P1).Size();

			float lr = d0 / (d0 + d1);
			kPoint IntersecPos = (P1 - P0)*lr + P0;
			if (!IsBetweenPoint(IntersecPos, wP0, wP1, wSQL)) //相交点在墙体外部
			{
				continue;
			}

			//分割旧墙体
			ObjectID E = pWall->P[1];
			Corner *NewCorner = _AddCorner(IntersecPos.x, IntersecPos.y, true);
			if (!NewCorner)
			{
				break;
			}

			ObjectID SplitID = NewCorner->GetID();

			if (NewCorner->IsFree())
			{
				pWall->SetCorner(1, SplitID, Holes);
				InnerAddWall(pWall->GetType(), SplitID, E, pWall->GetThickLeft(), pWall->GetThickRight(), pWall->GetHeight(0), &Holes);
			}

			//记录分割点 , 插入合适的位置
			float sp_dist = (IntersecPos - P0).SizeSquared();
			size_t n = SplitDists.size();
			Splits.resize(n + 1);
			SplitDists.resize(n + 1);
			size_t k = n;
			for (; k > 0; --k)
			{
				if (sp_dist > SplitDists[k - 1])
				{
					break;
				}
				else
				{
					Splits[k] = Splits[k - 1];
					SplitDists[k] = SplitDists[k - 1];
				}
			}
			Splits[k] = SplitID;
			SplitDists[k] = sp_dist;
		}

		//生成切割后新的墙体
		ObjectID CornerID = StartCorner;
		for (size_t i = 0; i < Splits.size(); ++i)
		{
			InnerAddWall(WallType, CornerID, Splits[i], ThickLeft, ThickRight, Height, nullptr);
			CornerID = Splits[i];
		}
	}
}

ObjectID SuiteImpl::AddWindow(ObjectID WallID, const kPoint &Location, float zPos, float Width, float Height)
{
	Window *pWindow = nullptr;
	Wall *pWall = GET_BUILDING_OBJ(this, WallID, Wall);

	if (pWall)
	{
		PinCorner *pCorner = (PinCorner *)CreateObject(EPinCorner, true);
		if (pCorner)
		{
			pCorner->Location = Location;
			pCorner->OwnerID = pWall->GetID();

			pWindow = (Window *)CreateObject(EWindow, true);
			pWindow->CornerID = pCorner->GetID();
			pCorner->RelativeID = pWindow->GetID();
			pWall->AddHole(pWindow, Location, zPos, Height, Width);
			pWall->MarkNeedUpdate();

			OnCreate(pCorner);
			OnCreate(pWindow);

			BuildingConfig *Cfg = GET_BUILDING_OBJ(this, _ConfigID, BuildingConfig);
			if (Cfg && Cfg->DefaultWindow.length() > 0)
			{
				AddModelToAnchor(pWindow->GetID(), Cfg->DefaultWindow.c_str(), kVector3D());
			}
		}
	}
	return pWindow ? pWindow->GetID() : INVALID_OBJID;
}

ObjectID SuiteImpl::AddDoor(ObjectID WallID, const kPoint &Location, float Width, float Height, float zPos/*= 0*/)
{
	DoorHole *pDoor = nullptr;
	Wall *pWall = GET_BUILDING_OBJ(this, WallID, Wall);
	if (pWall)
	{
		PinCorner *pCorner = (PinCorner *)CreateObject(EPinCorner, true);
		if (pCorner)
		{
			pCorner->Location = Location;
			pCorner->OwnerID = pWall->GetID();

			pDoor = (DoorHole *)CreateObject(EDoorHole, true);
			pDoor->WallID = WallID;
			pDoor->CornerID = pCorner->GetID();
			pCorner->RelativeID = pDoor->GetID();
			pWall->AddHole(pDoor, Location, zPos, Height, Width);
			pWall->MarkNeedUpdate();

			OnCreate(pCorner);
			OnCreate(pDoor);

			BuildingConfig *Cfg = GET_BUILDING_OBJ(this, _ConfigID, BuildingConfig);
			if (Cfg)
			{
				pDoor->SetModel(Cfg->DefaultDoor, Cfg->DefaultDoorFrame, "");
			}
		}
	}
	return pDoor ? pDoor->GetID() : INVALID_OBJID;
}

Room *SuiteImpl::AddRoom(ObjectID *pCorners, int NumCorner)
{
	Room *pRoom = NULL;

	if (NumCorner >= 3)
	{
		pRoom = (Room *)CreateObject(ERoom, true);
		pRoom->Init(pCorners, NumCorner);
			
		CeilPlane *pCeil = (CeilPlane *)CreateObject(ECeilPlane, true);
		if (pRoom)
		{
			pRoom->CeilID = pCeil->GetID();
			pCeil->RoomID = pRoom->GetID();
			pCeil->SetVisible(_GetConfig().bCeilVisible);
		}

		FloorPlane *pFloor = (FloorPlane *)CreateObject(EFloorPlane, true);
		if (pFloor)
		{
			pRoom->FloorID = pFloor->GetID();
			pFloor->RoomID = pRoom->GetID();
		}

		OnCreate(pCeil);
		OnCreate(pFloor);
		OnCreate(pRoom);

		BuildingConfig &cfg = _GetConfig();
		if (cfg.FloorMaterial.length()>0)
		{
			SetSurface(pFloor->GetID(), cfg.FloorMaterial.c_str(), cfg.FloorMaterialType);
		}

		if (_GetConfig().CeilMaterial.length() > 0)
		{
			SetSurface(pCeil->GetID(), cfg.CeilMaterial.c_str(), cfg.CeilMaterialType);
		}

		if (pRoom)
		{
			AddSkirting(pRoom);
		}

		_Rooms.push_back(pRoom->GetID());
	}

	return pRoom;
}

void SuiteImpl::AddSkirting(Room *pRoom)
{
	BuildingConfig &cfg = _GetConfig();
		
	Skirting *pSkirtingCeil = (Skirting *)CreateObject(ESkirting, true);
	if (pSkirtingCeil)
	{
		pRoom->SkirtingCielID = pSkirtingCeil->GetID();
		pSkirtingCeil->RoomID = pRoom->GetID();
		pSkirtingCeil->Extent = cfg.SkirtingCeilExt;
		pSkirtingCeil->SkirtingType = ESkirtingTop;
		pSkirtingCeil->SkirtingResID = cfg.SkirtingCeil;
		OnCreate(pSkirtingCeil);
	}

	Skirting *pSkirtingFloor = (Skirting *)CreateObject(ESkirting, true);
	if (pSkirtingFloor)
	{
		pRoom->SkirtingFloorID = pSkirtingFloor->GetID();
		pSkirtingFloor->RoomID = pRoom->GetID();
		pSkirtingFloor->Extent = cfg.SkirtingFloorExt;
		pSkirtingFloor->SkirtingType = ESkirtingBottom;
		pSkirtingFloor->SkirtingResID = cfg.SkirtingFloor;
		OnCreate(pSkirtingFloor);
	}
}

Room *SuiteImpl::AddRoom(Corner **ppCorners, int NumCorner)
{
	if (NumCorner >= 3)
	{
		std::vector<ObjectID> Corners;
		Corners.resize(NumCorner);

		for (int i = 0; i < NumCorner; ++i)
		{
			Corners[i] = ppCorners[i]->GetID();
		}

		return AddRoom(&Corners[0], NumCorner);
	}
	return nullptr;
}

void SuiteImpl::RemoveSurface(ObjectID PrimitiveID, int SubModelIndex)
{
	Primitive *pPrim = GET_BUILDING_OBJ(this, PrimitiveID, Primitive);
	if (pPrim)
	{
		IMeshObject *MeshObject = pPrim->GetMeshObject(0);
		ObjectID SurfaceID = MeshObject->GetSectionSurfaceID(SubModelIndex);
		Surface *pSurf = GET_BUILDING_OBJ(this, SurfaceID, Surface);
		if (pSurf)
		{
			UnLink(pSurf, PrimitiveID, SubModelIndex);
		}
		MeshObject->SetSectionSurface(SubModelIndex, INVALID_OBJID);
	}
}

void SuiteImpl::UnLink(Surface *pSurface, ObjectID PrimID, int SubModelIndex)
{
	if (pSurface)
	{
		pSurface->UnLink(PrimID, SubModelIndex);
		if (pSurface->GetRefCount() <= 0)
		{
			DeleteObj(pSurface);
		}
	}
}

BuildingObject *SuiteImpl::GetObject(ObjectID ObjID, EObjectType Class /*= nullptr*/)
{
	BuildingObject *FoundObj = nullptr;
	ObjectMap::iterator it = _ObjMap.find(ObjID);
	if (it != _ObjMap.end())
	{
		BuildingObject *Obj = it->second;
		if (Obj != nullptr)
		{
			if (!Class || Obj->IsA(Class))
			{
				FoundObj = Obj;
			}
		}
	}
	return FoundObj;
}

void SuiteImpl::DeleteObject(ObjectID ObjID, bool bForce /*= false*/)
{
	BuildingObject *pObj = GetObject(ObjID);
	if (pObj && (bForce || pObj->IsDeletable()))
	{
		DeleteObj(pObj);
	}
}

void SuiteImpl::AddCornersToSparseMap(std::vector<Corner *> &Corners)
{
	for (size_t i = 0; i < Corners.size(); ++i)
	{
		_SparseMap.Add(Corners[i]);
	}
}

void SuiteImpl::RemoveCornersFromSparseMap(std::vector<Corner *> &Corners)
{
	for (size_t i = 0; i < Corners.size(); ++i)
	{
		_SparseMap.Del(Corners[i]);
	}
}

bool SuiteImpl::Move(ObjectID ObjID, const kPoint &DeltaMove)
{
	bool bSuccess = false;
	BuildingObject *pObj = GetObject(ObjID);

	if (pObj)
	{
		std::vector<Corner *> Corners;
		pObj->GetCorners(Corners);

		RemoveCornersFromSparseMap(Corners);

		bSuccess = pObj->Move(DeltaMove);

		AddCornersToSparseMap(Corners);
	}

	return bSuccess;
}

int SuiteImpl::GetAllObjects(IObject **&ppObjects, EObjectType InClass /*= EUnkownObject*/)
{
	static std::vector<IObject *> AllObjects;
	AllObjects.clear();

	ppObjects = nullptr;

	for (ObjectMap::iterator it = _ObjMap.begin(); it != _ObjMap.end(); ++it)
	{
		BuildingObject *Obj = it->second;
		if (Obj && (!InClass || Obj->IsA(InClass)))
		{
			AllObjects.push_back(Obj);
		}
	}

	if (AllObjects.size() > 0)
	{
		ppObjects = &AllObjects[0];
	}

	return (int)AllObjects.size();
}

void SuiteImpl::Clean()
{
	IObject ** ppAllObjects;
	int nObjects = GetAllObjects(ppAllObjects, EObjectType::EUnkownObject);

	for (int i = 0; i < nObjects; ++i)
	{
		BuildingObject *Obj = (BuildingObject *)ppAllObjects[i];
		DeleteObj(Obj);
	}

	assert(_ObjMap.size() == 0);
}

void SuiteImpl::SetListener(ISuiteListener *Listener)
{
	_Listener = Listener;
}

void SuiteImpl::Serialize(ISerialize &Ar)
{
	Ar << _IncrObjectID;

	if (Ar.IsSaving())
	{
		size_t Pos = Ar.Tell();

		int nObj = 0;
		Ar << nObj;

		for (ObjectMap::iterator it = _ObjMap.begin(); it != _ObjMap.end(); ++it)
		{
			BuildingObject *Obj = it->second;
			if (Obj)
			{
				int ID = Obj->GetID();
				Ar << ID;
				int ObjType = Obj->GetType();
				Ar << ObjType;

 				Ar.WriteChunk(OBJ_CHUNK);
				Obj->Serialize(Ar);
 				Ar.EndChunk(OBJ_CHUNK);

				++nObj;
			}
		}

		size_t endPos = Ar.Tell();
		Ar.Seek(Pos);

		Ar << nObj;
		Ar.Seek(endPos);
	}
	else if (Ar.IsLoading())
	{
		Clean();

		int nObj = 0;
		Ar << nObj;

		for (int i = 0; i < nObj; ++i)
		{
			int ID = 0;
			int ObjType = 0;
			Ar << ID;
			Ar << ObjType;
			int ChunkID = Ar.ReadChunk();

			BuildingObject *ObjToLoad = CreateObject((EObjectType)ObjType, false);
			if (ObjToLoad)
			{
				if (ChunkID == OBJ_CHUNK)
				{
					ObjToLoad->Serialize(Ar);
					ObjToLoad->SetID(ID);
					OnCreate(ObjToLoad);
					if (ObjToLoad->IsA(EBuildingConfig))
					{
						_ConfigID = ID;
					}
				}
				else
				{
					DeleteObj(ObjToLoad);
					Ar.SkipChunk();
				}
			}
			Ar.EndChunk(ChunkID);
		}
	}
}

void SuiteImpl::Load(const char *Filename)
{
	ISerialize *Reader = GetBuildingSDK()->CreateFileReader(Filename);
	if (Reader)
	{
		Serialize(*Reader);
		Reader->Close();
	}
}

void SuiteImpl::Save(const char *Filename)
{
	ISerialize *Writer = GetBuildingSDK()->CreateFileWriter(Filename);
	if (Writer)
	{
		Serialize(*Writer);
		Writer->Close();
	}
}

ObjectID SuiteImpl::GetWallByTwoCorner(ObjectID CornerID0, ObjectID CornerID1)
{
	ObjectID FoundWallID = INVALID_OBJID;
	Corner *pCorner0 = GET_BUILDING_OBJ(this, CornerID0, Corner);
	if (pCorner0)
	{
		for (size_t i = 0; i < pCorner0->ConnectedWalls.size(); ++i)
		{
			ObjectID WallID = pCorner0->ConnectedWalls[i];
			Wall *pWall = GET_BUILDING_OBJ(this, WallID, Wall);
			if (pWall)
			{
				if (CornerID1 == pWall->GetOtherCorner(CornerID0))
				{
					FoundWallID = WallID;
					break;
				}
			}
		}
	}
	return FoundWallID;
}

int SuiteImpl::GetConnectWalls(ObjectID CorerID, ObjectID *&pConnectedWalls)
{
	static std::vector<ObjectID> s_walls;
	s_walls.clear();

	int nWall = 0;

	Corner *pCorner = GET_BUILDING_OBJ(this, CorerID, Corner);
	if (pCorner)
	{
		nWall = (int)pCorner->ConnectedWalls.size();
		pConnectedWalls = &(pCorner->ConnectedWalls[0]);
	}
	else
	{
		pConnectedWalls = nullptr;
	}

	return nWall;
}

bool SuiteImpl::IsFree(ObjectID CornerID)
{
	Corner *pCorner = GET_BUILDING_OBJ(this, CornerID, Corner);
	if (pCorner)
	{
		return pCorner->IsFree();
	}
	return true;
}

bool SuiteImpl::GetWallVector(ObjectID WallID, kPoint &P0, kPoint &P1, kPoint &Right)
{
	Wall *pWall = GET_BUILDING_OBJ(this, WallID, Wall);
	if (pWall)
	{
		pWall->GetLocations(P0, P1);
		Right = pWall->GetRight();
		return true;
	}
	return false;
}

bool SuiteImpl::GetWallForwardFromCorner(ObjectID WallID, ObjectID CornerID, kPoint &Forward)
{
	Wall *pWall = GET_BUILDING_OBJ(this, WallID, Wall);
	if (pWall && CornerID != INVALID_OBJID)
	{
		Forward = pWall->GetDirection(CornerID);
		return true;
	}
	return false;
}

bool SuiteImpl::GetWallBorderLines(ObjectID WallID, kLine &CenterLine, kLine &LeftLine, kLine &RightLine)
{
	Wall *pWall = GET_BUILDING_OBJ(this, WallID, Wall); 
	if (pWall)
	{
		pWall->GetBorderLines(CenterLine, LeftLine, RightLine);
		return true;
	}
	return false;
}

int SuiteImpl::GetPolygon(ObjectID RoomID, kPoint *&pPolygons, bool bInner)
{
	int Count = 0;
	pPolygons = nullptr;

	Room *pRoom = GET_BUILDING_OBJ(this, RoomID, Room);
	if (pRoom)
	{
		Count = pRoom->GetPolygon(pPolygons, bInner);
	}

	return Count;
}

void SuiteImpl::GetRoomCorners(ObjectID RoomID, std::vector<ObjectID> &Corners)
{
	Room *pRoom = GET_BUILDING_OBJ(this, RoomID, Room);
	if (pRoom)
	{
		pRoom->GetCorners(Corners);
	}
}

ObjectID SuiteImpl::FindCloseWall(const kPoint &Location, float Width, kPoint &BestLoc, float InTolerance /*= -1.0f*/)
{
	kPoint P0, P1;
	float Tolerance = InTolerance<0?  _GetConfig().Tolerance : InTolerance;
	float ToleranceSQ = Tolerance*Tolerance;
	float MinDist = ToleranceSQ*100.0f;
	ObjectID foundWall = INVALID_OBJID;

	for (ObjectMap::iterator it = _ObjMap.begin(); it != _ObjMap.end(); ++it)
	{
		BuildingObject *pObj = it->second;

		if (!pObj || !pObj->IsA(EWall))
		{
			continue;
		}

		Wall *pWall = (Wall *)(pObj);

		pWall->GetLocations(P0, P1);
			
		kPoint P10 = P1 - P0;
		kPoint PL0 = Location - P0;
				
		float Len10 = P10.Size();
		if (Len10 > 0)
		{
			kPoint Dir = P10 / Len10;
			float fDot = Dir.Dot(PL0);

			//在墙体内部,且不在墙洞内
			if ( fDot > 0 && fDot < Len10 && 
					pWall->HitTestHole(fDot)==INVALID_OBJID) 
			{
				kPoint projP = P0 + Dir * fDot;
				float distSQ = (Location - projP).SizeSquared();

				if (distSQ < ToleranceSQ)
				{
					if (distSQ < MinDist)
					{
						MinDist = distSQ;
						foundWall = pWall->GetID();
						BestLoc = projP;
					}
				}
			}
		}
	}

	return foundWall;
}

Corner *FilterCorner(std::vector<Corner *> &Corners, ObjectID IgnoreObjID)
{
	Corner *BestCorner = nullptr;

	for (size_t i = 0; i < Corners.size(); ++i)
	{
		Corner *pCorner = Corners[i];
		if (IgnoreObjID != INVALID_OBJID)
		{
			if (IgnoreObjID != pCorner->GetID())
			{
				BestCorner = pCorner;
				break;
			}
		}
		else
		{
			BestCorner = pCorner;
			break;
		}
	}

	return BestCorner;
}

kESnapType SuiteImpl::FindSnapLocation(const kPoint &TouchPos, kPoint &BestSnapLocation, ObjectID &BestObjID, ObjectID IgnoreObjID, float Tolerance /*= -1.0f*/)
{
	Corner *BestCorner = nullptr;
	if (Tolerance < 0)
	{
		Tolerance = _GetConfig().Tolerance;
	}

	std::vector<Corner *> Corners;
	if (_SparseMap.FindByRadius(TouchPos, 2.0f*Tolerance, Corners, true))
	{
		BestCorner = FilterCorner(Corners, IgnoreObjID);
		if (BestCorner)
		{
			BestObjID = BestCorner->GetID();
			BestSnapLocation = BestCorner->Location;
			return kEPt;
		}
	}

	Corners.clear();
	if (_SparseMap.FindByLine(TouchPos, kPoint(1.0f, 0), Tolerance, Corners))
	{
		BestCorner = FilterCorner(Corners, IgnoreObjID);
		if (BestCorner)
		{
			BestObjID = BestCorner->GetID();
			BestSnapLocation = BestCorner->Location;
			return kEDirY;
		}
	}

	Corners.clear();
	if (_SparseMap.FindByLine(TouchPos, kPoint(0, 1.0f), Tolerance, Corners))
	{
		BestCorner = FilterCorner(Corners, IgnoreObjID);
		if (BestCorner)
		{
			BestObjID = BestCorner->GetID();
			BestSnapLocation = BestCorner->Location;
			return kEDirX;
		}
	}

	return kESnapType::kENone;
}

ObjectID SuiteImpl::GetConnectCorner(ObjectID Wall0, ObjectID Wall1, bool &bWall1InverseConnect)
{		
	Wall *pWall0 = GET_BUILDING_OBJ(this, Wall0, Wall);
	Wall *pWall1 = GET_BUILDING_OBJ(this, Wall1, Wall);
	bWall1InverseConnect = false;

	if (pWall0 && pWall1)
	{
		Corner *pCorner00 = GET_BUILDING_OBJ(this, pWall0->P[0], Corner);
		for (size_t i = 0; i < pCorner00->ConnectedWalls.size(); ++i)
		{
			if (pCorner00->ConnectedWalls[i] == Wall1)
			{
				bWall1InverseConnect = false;
				return pWall0->P[0];
			}
		}

		Corner *pCorner01 = GET_BUILDING_OBJ(this, pWall0->P[1], Corner);
		for (size_t i = 0; i < pCorner01->ConnectedWalls.size(); ++i)
		{
			if (pCorner01->ConnectedWalls[i] == Wall1)
			{
				bWall1InverseConnect = true;
				return pWall0->P[1];
			}
		}
	}

	return INVALID_OBJID;
}

ObjectID SuiteImpl::SetSurface(ObjectID PrimitiveID, const char *MaterialName, int MaterialType, int SubModelIndex /*= -1*/)
{
	Surface *pSurfaceFound = nullptr;
	Primitive *pPrim = GET_BUILDING_OBJ(this, PrimitiveID, Primitive);

	if (pPrim)
	{
		IMeshObject *MeshObject = pPrim->GetMeshObject(0);
		if (MeshObject)
		{
			if (SubModelIndex < 0)
			{
				std::vector<int> EmptyModels;
				int NumModels = MeshObject->GetSectionCount();
				for (int i = 0; i < NumModels; ++i)
				{
					if (MeshObject->GetSectionSurfaceID(i) == INVALID_OBJID)
					{
						EmptyModels.push_back(i);
					}
				}
				if (EmptyModels.size() > 0)
				{
					Surface *pSurface = (Surface *)CreateObject(ESurface, true);
					if (pSurface)
					{
						pSurface->Material = MaterialName;
						pSurface->MaterialType = MaterialType;
						
						for (int i = 0; i < (int)EmptyModels.size(); ++i)
						{
							int ModelIndex = EmptyModels[i];
							pSurface->Link(PrimitiveID, ModelIndex);
							MeshObject->SetSectionSurface(ModelIndex, pSurface->GetID());
						}

						OnCreate(pSurface);
						pSurfaceFound = pSurface;
					}
				}
			}
			else
			{
				if (SubModelIndex < MeshObject->GetSectionCount())
				{
					Surface  *RefSurface = nullptr;
					ObjectID SurfaceID = MeshObject->GetSectionSurfaceID(SubModelIndex);
					if (SurfaceID != INVALID_OBJID)
					{
						Surface *pSurface = GET_BUILDING_OBJ(this, SurfaceID, Surface);
						if (pSurface->GetRefCount() > 1)
						{
							pSurface->UnLink(PrimitiveID, SubModelIndex);
						}
						else
						{
							RefSurface = pSurface;
						}
					}

					if (!RefSurface)
					{
						RefSurface = (Surface *)CreateObject(ESurface, true);
						if (RefSurface)
						{
							MeshObject->SetSectionSurface(SubModelIndex, RefSurface->GetID());
							OnCreate(RefSurface);
							pSurfaceFound = RefSurface;
						}
					}

					if (RefSurface)
					{
						RefSurface->MaterialType = MaterialType;
						RefSurface->Material = MaterialName;
					}
				}
			}
		}
	}

	return pSurfaceFound ? pSurfaceFound->GetID() : INVALID_OBJID;
}

void SuiteImpl::SetSurface(ObjectID PrimitiveID, ObjectID SurfaceID, int SubModelIndex /*= -1*/)
{
	Primitive *pPrim = GET_BUILDING_OBJ(this, PrimitiveID, Primitive);
	if (pPrim)
	{
		IMeshObject *MeshObject = pPrim->GetMeshObject(0);
		if (SubModelIndex < 0)
		{
			std::vector<int> EmptyModels;
			int NumModels = MeshObject->GetSectionCount();
			for (int i = 0; i < NumModels; ++i)
			{
				if (MeshObject->GetSectionSurfaceID(i) == INVALID_OBJID)
				{
					EmptyModels.push_back(i);
				}
			}
			if (EmptyModels.size() > 0)
			{
				Surface *pSurface = GET_BUILDING_OBJ(this, SurfaceID, Surface);
				if (pSurface)
				{
					for (int i = 0; i < (int)EmptyModels.size(); ++i)
					{
						int ModelIndex = EmptyModels[i];
						pSurface->Link(PrimitiveID, ModelIndex);
						MeshObject->SetSectionSurface(ModelIndex, SurfaceID);
					}
				}
			}
		}
		else
		{
			if (SubModelIndex < MeshObject->GetSectionCount())
			{
				ObjectID OldSurfaceID = MeshObject->GetSectionSurfaceID(SubModelIndex);
				Surface  *pOldSurface = GET_BUILDING_OBJ(this, OldSurfaceID, Surface);
				if (pOldSurface)
				{
					pOldSurface->UnLink(PrimitiveID, SubModelIndex);
				}

				Surface *pSurface = GET_BUILDING_OBJ(this, SurfaceID, Surface);
				if (pSurface)
				{
					pSurface->Link(PrimitiveID, SubModelIndex);
				}

				ObjectID NewSurfaceID = pSurface ? pSurface->GetID() : INVALID_OBJID;
				MeshObject->SetSectionSurface(SubModelIndex, NewSurfaceID);
			}
		}
	}

}

ObjectID SuiteImpl::GetSurface(ObjectID PrimitiveID, int SubModelIndex)
{
	Primitive *pPrim = GET_BUILDING_OBJ(this, PrimitiveID, Primitive);
	if (pPrim)
	{
		IMeshObject *MeshObject = pPrim->GetMeshObject(0);
		return MeshObject->GetSectionSurfaceID(SubModelIndex);
	}
	return INVALID_OBJID;
}

ObjectID SuiteImpl::GetRoomByLocation(const kPoint &Location)
{
	ObjectID HitObj = INVALID_OBJID;
	Room *pRoom = _GetRoomByLocation(Location, HitObj);
	if (pRoom && HitObj==INVALID_OBJID)
	{
		return pRoom->GetID();
	}
	return INVALID_OBJID;
}

Room * SuiteImpl::_GetRoomByLocation(const kPoint &Location, ObjectID &HitObj)
{
	if (_Rooms.size() > 0)
	{
		for (size_t i = 0; i < _Rooms.size(); ++i)
		{
			Room *pRoom = GET_BUILDING_OBJ(this, _Rooms[i], Room);
			if (pRoom && pRoom->HitTest(Location, HitObj))
			{
				return pRoom;
			}
		}
	}
	return nullptr;
}

ObjectID SuiteImpl::HitTest(const kPoint &Location)
{
	ObjectID HitObj = INVALID_OBJID;
	kPoint BestLocation;
	ObjectID BestID = INVALID_OBJID;

	if (kEPt == FindSnapLocation(Location, BestLocation, BestID, INVALID_OBJID, 10))
	{
		HitObj = BestID;
	}
	else
	{
		Room *pRoom = _GetRoomByLocation(Location, HitObj);
		if (pRoom && HitObj == INVALID_OBJID)
		{
			HitObj = pRoom->GetID();
		}

		if(HitObj==INVALID_OBJID)
		{
			for (ObjectMap::iterator it = _ObjMap.begin(); it != _ObjMap.end(); ++it)
			{
				BuildingObject *pObj = it->second;
				if (!pObj->IsA(EWall))
				{
					continue;
				}

				Wall *pWall = (Wall* )(pObj);
				if (pWall && pWall->IsSolo() && pWall->HitTest(Location, HitObj))
				{
					if (HitObj == INVALID_OBJID)
					{
						HitObj = pWall->GetID();
					}
					break;
				}
			}
		}
	}

	return HitObj;
}

bool IsLineIntersecBox(const kLine &line, const kPoint &Min, const kPoint &Max)
{
	/*
	-------max
	|		|
	|-------|
	min
	*/

	if (line.end.x == line.start.x)
	{
		float x = line.end.x;
		float minY, maxY;

		if (line.start.y < line.end.y)
		{
			minY = line.start.y;
			maxY = line.end.y;
		}
		else
		{
			minY = line.end.y;
			maxY = line.start.y;
		}

		if ((x > Min.x && x < Max.x) && ((Min.y>minY && Min.y<maxY) || (Max.y>minY && Max.y<maxY)) )
		{
			return true;
		}
	}
	else if (line.end.y == line.start.y)
	{
		float y = line.start.y;

		float minX, maxX;

		if (line.start.x < line.end.x)
		{
			minX = line.start.x;
			maxX = line.end.x;
		}
		else
		{
			minX = line.end.x;
			maxX = line.start.x;
		}

		if ((y > Min.y && y < Max.y) && ((Min.x>minX && Min.x<maxX) || (Max.x>minX && Max.x<maxX)))
		{
			return true;
		}
	}
	else
	{
		float elopse = (line.end.y - line.start.y) / (line.end.x - line.start.x);
		float yMin = elopse * (Min.x - line.start.x) + line.start.y;

		if ((yMin > Min.y && yMin < Max.y) && ((yMin > line.start.y && yMin < line.end.y) || (yMin > line.end.y && yMin < line.start.y)))
		{
			return true;
		}

		float yMax = elopse * (Max.x - line.start.x) + line.start.y;
		if ((yMax > Min.y && yMax < Max.y) && ((yMax > line.start.y && yMax < line.end.y) || (yMax > line.end.y && yMax < line.start.y)))
		{
			return true;
		}

		float invelopse = 1.0f / elopse;

		float xMin = invelopse * (Min.y - line.start.y) + line.start.x;
		if ((xMin > Min.x && xMin < Max.x) && ((xMin > line.start.x && xMin < line.end.x) || (xMin > line.end.x && xMin < line.start.x)))
		{
			return true;
		}

		float xMax = invelopse * (Max.y - line.start.y) + line.start.x;
		if ((xMax > Min.x && xMax < Max.x) && ((xMax > line.start.x && xMax < line.end.x) || (xMax > line.end.x && xMax < line.start.x)))
		{
			return true;
		}
	}

	return false;
}

int SuiteImpl::HitTest(const kPoint &Min, const kPoint &Max, ObjectID *&pResults)
{
	static std::vector<ObjectID> OutResults;
	OutResults.clear();

	std::vector<Corner *> Corners;
	_SparseMap.FindByBox2D(Min, Max, Corners);

	std::set<ObjectID> walls;

	for (int i = 0; i < Corners.size(); ++i)
	{
		Corner *pCorner = Corners[i];
		if (pCorner)
		{
			for (size_t i = 0; i < pCorner->ConnectedWalls.size(); ++i)
			{
				ObjectID WallID = pCorner->ConnectedWalls[i];

				if (walls.find(WallID)==walls.end())
				{
					walls.insert(WallID);
				}
			}
			OutResults.push_back(pCorner->GetID());
		}
	}

	for (ObjectMap::iterator it = _ObjMap.begin(); it != _ObjMap.end(); ++it)
	{
		BuildingObject *pObj = it->second;
		if (pObj->IsA(EWall))
		{
			if (walls.find(pObj->GetID()) == walls.end())
			{
				Wall *pWall = (Wall *)pObj;

				kLine l_left, l_right, l_center;
				pWall->GetBorderLines(l_center, l_left, l_right);

				if (IsLineIntersecBox(l_left, Min, Max))
				{
					walls.insert(pWall->GetID());
				}
				else if(IsLineIntersecBox(l_right, Min, Max))
				{
					walls.insert(pWall->GetID());
				}
			}
		}
	}

	for (std::set<ObjectID>::iterator it = walls.begin(); it!=walls.end(); ++it)
	{
		OutResults.push_back(*it);
	}

	if (!OutResults.empty())
	{
		pResults = &OutResults[0];
	}
	else
	{
		pResults = nullptr;
	}

	return (int)OutResults.size();
}

IValue &SuiteImpl::GetProperty(ObjectID ID, const char *PropertyName)
{
	BuildingObject *pObj = (BuildingObject *)GetObject(ID);
	if (pObj && pObj->ClsDesc)
	{
		ObjectDesc *Desc = pObj->ClsDesc;
		while (Desc)
		{
			IProperty *Prop = Desc->GetProperty(PropertyName);
			if (Prop)
			{
				return *Prop->GetValue(pObj);
			}
			Desc = Desc->SuperClassDesc;
		}

		IValue *propFun = pObj->GetFunctionProperty(PropertyName);
		if (propFun)
		{
			return *propFun;
		}
	}

	return PValue::Nil;
}

void SuiteImpl::SetProperty(ObjectID ID, const char *PropertyName, const IValue *Value)
{
	BuildingObject *pObj = GetObject(ID);
	if (pObj && pObj->ClsDesc)
	{
		ObjectDesc *Desc = pObj->ClsDesc;
		while (Desc)
		{
			IProperty *Prop = Desc->GetProperty(PropertyName);
			if (Prop)
			{
				Prop->SetValue(pObj, Value);
				break;
			}
			Desc = Desc->SuperClassDesc;
		}
	}
}

void SuiteImpl::SetValue(ObjectID ID, const char *PropertyName, IValue *Value)
{
	BuildingObject *pObj = GetObject(ID);
	if (pObj)
	{
		pObj->SetValue(PropertyName, Value);
	}
}

IValue *SuiteImpl::FindValue(ObjectID ID, const char *PropertyName)
{
	BuildingObject *pObj = GetObject(ID);
	if (pObj)
	{
		return pObj->FindValue(PropertyName);
	}
	return nullptr;
}

void SuiteImpl::NotifySurfaceValueChanged(ObjectID PrimID, int SubSection)
{
	if (_Listener)
	{
		IObject *pObj = GetObject(PrimID);
		IMeshObject *meshObj = pObj? pObj->GetMeshObject(0) : nullptr;
		if (meshObj)
		{
			ObjectID SurfaceID = meshObj->GetSectionSurfaceID(SubSection);
			_Listener->OnUpdateSurfaceValue(pObj, SubSection, SurfaceID);
		}
	}
}


