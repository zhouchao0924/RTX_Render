
#pragma once

#include "kLine.h"
#include "Anchor.h"

enum ESkirtingType
{
	ESkirtingTop,
	ESkirtingBottom,
	SKirtingNone
};

class Skirting :public Anchor
{
	DEFIN_CLASS()
public:
	Skirting();
	virtual void Serialize(ISerialize &Ar);
	virtual void OnCreate();
	virtual void OnDestroy();
	EObjectType GetType() { return ESkirting; }
	virtual void Link(ModelInstance *InModel) override;
protected:
	void AddSkirtingModel(const kPoint &Right, const kPoint &P0, const kPoint &P1, const kVector3D &Normal0, const kVector3D &Normal1, float Height0, float Height1);
public:
	kVector3D				Extent;
	ObjectID				RoomID;
	std::string				SkirtingResID;
	ESkirtingType			SkirtingType;
	std::vector<ObjectID>	SkirtingModels;
};


