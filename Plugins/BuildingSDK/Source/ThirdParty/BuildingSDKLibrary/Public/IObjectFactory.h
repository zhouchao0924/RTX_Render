
#pragma once

#include "Math/kArray.h"
#include "ObjectTypes.h"

class IObject;

class IObjectFactoryCallback
{
public:
	virtual void OnFactoryPreCreate(IObject *ObjLoaded) = 0;
	virtual void OnFactoryPostCreate(IObject *ObjLoaded) = 0;
	virtual void OnFactoryDestroy(IObject *ObjLoaded) = 0;
	virtual void OnFactoryLoaded(IObject *ObjLoaded) = 0;
};

class IObjectFactory
{
public:
	virtual IObject *CreateObject(EObjectType ObjType, int ID = INVALID_OBJID) = 0;
	virtual void DeleteObject(IObject *pObj) = 0;
	virtual void Clear() = 0;
	virtual IObject * GetObj(ObjectID ID, EObjectType ObjType = EUnkownObject) = 0;
	virtual kArray<IObject *> GetAllObjects(EObjectType ObjType = EUnkownObject, bool bIncludeDeriveType = true) = 0;
	virtual void Serialize(ISerialize &Ar, unsigned int Ver) = 0;
	virtual void SetCallback(IObjectFactoryCallback *Callback) = 0;
	virtual void PostCreate(IObject *pObj) = 0;
	virtual void Destroy() = 0;
};



