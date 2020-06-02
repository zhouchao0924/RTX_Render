
#include "Private/BuildingSDK.h"

#if defined _WIN32 || defined _WIN64
#include "AllowWindowsPlatformTypes.h"
    #include <Windows.h>
#include "HideWindowsPlatformTypes.h"
    #define DLLEXPORT __declspec(dllexport)
#else
    #include <stdio.h>
#endif

#ifndef DLLEXPORT
    #define DLLEXPORT
#endif


DLLEXPORT void* GetBuildingSDKFunction()
{
	static BuildingSDK sdk;
	return &sdk;
}

