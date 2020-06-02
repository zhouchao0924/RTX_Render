// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IBuildingSDK.h"
#include "Modules/ModuleManager.h"

class IBuildingSDK;
class BUILDINGSDK_API FBuildingSDKModule : public IModuleInterface
{
public:
	FBuildingSDKModule();
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	IBuildingSDK *GetSDK();
private:
	/** Handle to the test dll we will load */
	void*	BuildingSDKLibraryHandle;
	IBuildingSDK *SDK;
};

#define  FORCE_TYPE(type, v)  (*((type*)&(v)))
#define  FORCE_POINTER(type, v)  ((type)(v))

extern IBuildingResourceMgr *GetSDKResourceMgr();

