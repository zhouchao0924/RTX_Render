
#pragma once

class ISystemUtils
{
public:
	virtual ~ISystemUtils() {}
	virtual const char *CreateGUID() = 0;
	virtual const char *GetCachedFilename(const char *SearchPath, const char *ResID, bool bCheckFileExsit) = 0;
	virtual bool  GetQueryURL(const char *ResID, const char *&URL, const char *&JsonPost) = 0;
	virtual const char *GetSaveDirectory() = 0;
	virtual bool Copy(const char *SrcFile, const char *DstFile) = 0;
	virtual const char * GetPluginDirectory() = 0;
	virtual void *LoadDllFunction(const char *DLLFilename, const char *FuncName) = 0;
	virtual int  FindFiles(const char **&OutFiles, const char *Directory, const char *FileExt, bool bReclusive) = 0;
};



