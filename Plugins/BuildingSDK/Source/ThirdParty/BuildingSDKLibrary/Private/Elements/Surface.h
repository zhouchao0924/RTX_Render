
#pragma once

#include <set>
#include "BuildingObject.h"
#include "Mesh/SurfaceObject.h"

class Surface :public BuildingObject
{
	DEFIN_CLASS()
public:
	Surface();
	void Serialize(ISerialize &Ar);
	ISurfaceObject *GetObject() { return &Surf; }
	EObjectType GetType() { return ESurface; }
	void Link(ObjectID PrimID, int ModelIndex);
	void UnLink(ObjectID PrimID, int ModelIndex);
	IValue *GetFunctionProperty(const std::string &name);
	bool SetFunctionProperty(const std::string &name, const IValue *Value);
	int  GetRefCount();
protected:
	void NotifySurfaceValueChanged();
public:
	std::set<ModelSectionID>	RefModels;
	std::string					Material;
	int							MaterialType;
	SurfaceObject				Surf;
};


