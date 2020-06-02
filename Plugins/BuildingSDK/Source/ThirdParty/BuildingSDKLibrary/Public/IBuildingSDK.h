
#pragma once

#include "ISuite.h"
#include "ISerialize.h"
#include "IUndoRedo.h"
#include "ITargetPlatformHandler.h"
#include "IGeometry.h"
#include "IMeshObject.h"
#include "IBuildingResourceMgr.h"
#include "IHttp.h"
#include "IStretchPath.h"
#include "IObjectFactory.h"
#include "IDataExchange.h"
#include "IMeshEditor.h"
#include "IBuildingEditor.h"
#include "IBuildingPlugin.h"
#include "IGroup.h"
#include "IShm.h"
#include "IRemoteCall.h"

enum ELOGLevel
{
	ELOG_Info,
	ELOG_Warning,
	ELOG_Error,
};

struct FMemorySummary
{
	size_t MemoryUsed;
	size_t ObjectCount;
};

class  IBuildingSDK
{
public:
	class Dependency
	{
	public:
		virtual ~Dependency() {}
		virtual const char *GetCachedFilename(const char *SearchPath, const char *ResID, bool bCheckFileExsit) = 0;
		virtual bool  GetQueryURL(const char *ResID, const char *&URL, const char *&JsonPost) = 0;
		virtual const char *GetSaveDirectory() = 0;
		virtual bool Copy(const char *SrcFile, const char *DstFile) = 0;
		virtual const char * GetPluginDirectory() = 0;
		virtual void *LoadDllFunction(const char *DLLFilename, const char *FuncName) = 0;
		virtual int  FindFiles(const char **&OutFiles, const char *Directory, const char *FileExt, bool bReclusive) = 0;
		virtual bool IsCheckMemory() = 0;
		virtual void Log(ELOGLevel Level, const char *msgInfo) = 0;
	};

	virtual bool Initialize(Dependency *Dep = nullptr) = 0;
	virtual void Tick(float Delta) = 0;
	virtual void UnInitialize() = 0;

	virtual ISuite *CreateSuite(const char *Name) = 0;
	virtual ISuite *GetSuite(const char *Name) = 0;
	virtual ISuite *GetSuite(int SuiteID) = 0;
	virtual void DestroySuite(ISuite *Suite) = 0;

	virtual ISerialize *CreateFileWriter(const char *Filename, bool bCompressed = false) = 0;
	virtual ISerialize *CreateFileReader(const char *Filename, bool bCompressed = false) = 0;
	virtual ISerialize *CreateMemoryWriter(char *&OutMemory, size_t &outSize) = 0;
	virtual ISerialize *CreateMemoryReader(const char *Memory, size_t Size) = 0;

	virtual IValueFactory *GetValueFactory() = 0;
	virtual IObjectFactory *CreateObjectFactory() = 0;
	virtual IUndoRedo *GetUndoRedo() = 0;
	virtual void SetUndoRedo(IUndoRedo *UndoRedo) = 0;
	virtual IClassLibrary *GetClassLibrary() = 0;

	virtual IDataExchange *GetDataExchange() = 0;
	virtual ITargetPlatformHandler *GetPlatform(ECookedPlatform PlatformType) = 0;
	virtual void AddPlugin(IBuildingPlugin *Plugin) = 0;
	virtual void *QueryInterface(const char *Name) = 0;

	virtual IShm *GetShm() = 0;
	virtual Dependency *GetDependency() = 0;
	virtual IGeometry *GetGeometryLibrary() = 0;
	virtual IStretchPath *GetStretchPathEditor() = 0;
	virtual ICompoundModel *GetCompoundEditor() = 0;
	virtual ITriangulatedMesh *GetTriMeshEditor() = 0;
	virtual IBuildingEditor *GetBuildingEditor() = 0;

	virtual IObject *LoadFile(const char *Filename, IObject *ObjToLoad = nullptr) = 0;
	virtual bool SaveFile(IObject *pObj, const char *Filename, bool bCompressed = false) = 0;
	virtual IBuildingResourceMgr *GetResourceMgr() = 0;
	virtual IHttp *GetHttp() = 0;
	virtual bool GetMemorySummary(FMemorySummary &OutSummary) = 0;

	virtual IRemoteService *GetRemoteService() = 0;
	virtual IRemoteProxy *GetRemoteProxy() = 0;
};

IBuildingSDK *GetBuildingSDK();
IBuildingResourceMgr *GetSDKResourceMgr();
void DumpLog(ELOGLevel Level, const char *msgInfo);

template <typename T>
T *QueryInterface(IBuildingSDK *SDK, const char *Name)
{
	if (SDK && Name)
	{
		return (T*)SDK->QueryInterface(Name);
	}
	return nullptr;
}

