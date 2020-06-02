
#pragma once

#include "BuildingObject.h"


class ModelInstance;
class Anchor :public BuildingObject
{
	DEFIN_CLASS()
public:
	Anchor();
	void Serialize(ISerialize &Ar);
	void OnDestroy();
	EObjectType GetType() { return EAnchor; }
	virtual void Link(ModelInstance *InModel);
	virtual void UnLink(ModelInstance *InModel);
	virtual ObjectID GetModel(int index = 0);
	virtual void UpdateTransform(ModelInstance *pModel);
	virtual void GetPins(std::vector<ObjectID> &OutPins);
	bool SetFunctionProperty(const std::string &name, const IValue *Value) override;
	IValue *GetFunctionProperty(const std::string &name) override;
	ModelInstance *GetModelByType(int type);
	ModelInstance *SetModelByType(int type, const std::string ResID);
	void MarkNeedUpdate() override;
protected:
	kPoint GetLocation();
public:
	ObjectID				CornerID;
	std::vector<ObjectID>	LinkObjects;
};



