
#pragma once

#include "kLine.h"
#include "WallHole.h"

class DoorHole :public WallHole
{
	DEFIN_CLASS()
public:
	DoorHole();
	EObjectType GetType() { return EDoorHole; }
	void Serialize(ISerialize &Ar);
	void UpdateTransform(ModelInstance *pModel) override;
	void SetModel(const std::string &Door, const std::string &DoorFrame, const std::string &DoorSone);
	IValue *GetFunctionProperty(const std::string &name) override;
	bool SetFunctionProperty(const std::string &name, const IValue *Value) override;
};

