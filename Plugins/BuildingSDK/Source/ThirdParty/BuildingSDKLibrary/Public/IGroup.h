
#pragma once

#include "IClass.h"
#include "Math/kBox.h"
#include "Math/kArray.h"
#include "Math/kXform.h"

class IGroup;
class IObject;
class IMeshObject;
class ISurfaceObject;

class INode
{
public:
	virtual ~INode() {}
	virtual int GetID() = 0;
	virtual IMeshObject *GetMesh() = 0;
	virtual ISurfaceObject *GetSurface(int iSection) = 0;
	virtual void SetSurface(int iSection, ObjectID SurfaceID) = 0;
	virtual ObjectID SetSurfaceUri(int iSection, int Type, const char *Uri) = 0;
	virtual IGroup *GetParent() = 0;
	virtual IGroup *CastToGroup() = 0;
	virtual IObject *GetOwner() = 0;
	virtual IObject *GetResource() = 0;
	virtual void SetOwner(IObject *Owner) = 0;
	virtual kBox3D GetBoundingBox() = 0;
	virtual kXform  GetLocalTranform() = 0;
	virtual bool IsVisible() = 0;
	virtual void SetVisible(bool bVisible) = 0;
};

class IGroup :public INode
{
public:
	virtual int GetChildCount() = 0;
	virtual INode *GetChild(int ChildIndex) = 0;
	virtual INode *AddChild(INode *Node) = 0;
	virtual void RemoveChild(INode *Node) = 0;
	virtual INode *FindChild(int ID) = 0;
	virtual INode *FindNode(const char *Name) = 0;
	virtual kArray<INode*> GetChildByMeshName(const char *Name) = 0;
	virtual bool GetChildByMaterialName(const char *Name, kArray<INode*> &RefNodes, kArray<int> &Sections) = 0;
};


