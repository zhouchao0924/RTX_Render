
#pragma once

#include "kLine.h"
#include "WallHole.h"

class Window :public WallHole
{
	DEFIN_CLASS()
public:
	Window();
	EObjectType GetType() { return EWindow; }
	void UpdateTransform(ModelInstance *pModel) override;
};

