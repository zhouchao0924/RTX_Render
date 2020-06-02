
#pragma once

#include "BuildingObject.h"

class CompoundObject :public BuildingObject
{
	DEFIN_CLASS()
public:
	EObjectType GetType() { return ECompoundObject; }
};


