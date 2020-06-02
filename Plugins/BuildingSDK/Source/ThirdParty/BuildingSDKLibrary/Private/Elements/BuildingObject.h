
#pragma once

#include <vector>
#include "Class/Object.h"

class ISuite;
class Corner;
class Anchor;
class BuildingObject : public Object
{
	DEFIN_CLASS()
public:
	BuildingObject();
	~BuildingObject() {}
	ISuite *GetSuite() { return _Suite; }
	void Serialize(ISerialize &Ar)override;
	EObjectType GetType() override { return EBuildingObject; }
	virtual void MarkNeedUpdate() { bNeedUpdate = true; }
	virtual bool GetBox2D(kPoint &Orignal, kPoint &Range) { return false; }
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void GetCorners(std::vector<Corner *> &Corners) {};
	void SetID(ObjectID InID) { _ID = InID; }
	Anchor *CreateAnchor(const kVector3D &Location) { return nullptr; }		
protected:
protected:
	friend class		 SuiteImpl;
	bool				 bNeedUpdate;
	std::vector<ObjectID> Anchors;
	class ISuite		 *_Suite;
};


