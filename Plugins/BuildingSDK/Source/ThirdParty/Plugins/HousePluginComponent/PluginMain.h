
#pragma once

#include "IBuildingSDK.h"

#define PI 3.1415926

class PluginMain :public IBuildingPlugin
{
public:
	bool Init(IBuildingSDK *SDK) override;
	bool Tick(float DeltaTime) override;
	void UnInitialize() override;
	void *QueryInterface(const char *Name) override;
};

extern IBuildingSDK *GSDK;

