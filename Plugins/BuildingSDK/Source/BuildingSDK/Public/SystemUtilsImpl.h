// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "IBuildingSDK.h"

class BUILDINGSDK_API SystemUtilsImpl :public IBuildingSDK::Dependency
{
public:
	const char *GetCachedFilename(const char *SearchPath, const char *ResID, bool bCheckFileExsit) override;
	const char *GetSaveDirectory() override;
	bool Copy(const char *SrcFile, const char *DstFile) override;
	const char * GetPluginDirectory() override;
	void *LoadDllFunction(const char *DLLFilename, const char *FuncName) override;
	int FindFiles(const char **&OutFiles, const char *Directory, const char *FileExt, bool bReclusive) override;
	bool  GetQueryURL(const char *ResID, const char *&URL, const char *&JsonPost) override;
	bool IsCheckMemory() { return false;  }
	void Log(ELOGLevel Level, const char *msgInfo);
protected:
	std::vector<std::string>	TempStrings;
	std::vector<const char *>	TempStrPtr;
};
