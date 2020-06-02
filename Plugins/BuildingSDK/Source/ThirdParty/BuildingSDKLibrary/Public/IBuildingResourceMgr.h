
#pragma once

#include "IClass.h"
#include "Math/kString.h"
#include "IObjectFactory.h"
#include "ITargetPlatformHandler.h"

enum ERType
{
	ERSurface,
	ERModel,
	ERScene,
	ERFbxFile,
	ERCompoundFile,
	ERStretchFile,
	ERTriangulatedMeshFile,
	ERPackageFile,
	ERTexture,
	ERSurfaceObject,
	ERDynamicSurfaceObj,
	ERPaveStyle,
	ERPluginObject = 65535,
	ERUnknown
};

enum EResLifeState
{
	EResPending,
	EResLoading,
	EResLost,
	EResDownloading,
	EResFileBad,
	EResLoaded
};

class IResource
{
public:
	virtual ERType   GetResType() = 0;
	virtual const char *GetResID() = 0;
	virtual const char *GetFilename() = 0;
	virtual EResLifeState GetState() = 0;
	virtual void SetState(EResLifeState State) = 0;
	
	virtual bool IsModified() = 0;
	virtual void SetLocalVersion(int LocalVersion) = 0;
	virtual int	 GetHeadVersion() = 0;
	virtual int  GetLocalVersion() = 0;
	virtual int  GetModifyVersion() = 0;
	virtual void SetModifyFlag() = 0;
	virtual void ResetModifyFlag() = 0;
	virtual bool IsNeedUpdate(int SrvVersion) = 0;
	
	virtual kArray<kString> GetDepends() = 0;
};

class IResourceMgrCallback
{
public:
	virtual void OnResourceProgress(const char *ResID, float Progress) = 0;
	virtual void OnResourceStateChanged(const char *ResID, EResLifeState State, IObject *Resource) = 0;
};

class IBuildingResourceMgr
{
public:
	virtual IObject *GetResource(ObjectID ID) = 0;
	virtual IObject *CreateResource(ERType ResType, const char *ResID = "") = 0;
	virtual void DestroyResource(ObjectID ID, bool bFlush) = 0;
	virtual IObjectFactory *GetObjectFactory() = 0;
	virtual void SetUseCompressedFile(bool bUseCompressed) = 0;
	virtual bool IsUseCompressedFile() = 0;
	virtual const char *GetCachedFilename(const char *ResID, bool bCheckFileExsit) =  0;
	virtual IObject *LoadResource(const char *Filename, bool Cache = true) = 0;
	virtual IObject *GetResource(const char *ResID, bool bAyncLoad, bool bCache = true, bool bForceRetry = false, const char *Url = nullptr) = 0;
	virtual void CleanCache() = 0;
	virtual void RegisterCallback(IResourceMgrCallback *Callback) = 0;
	virtual void UnRegisterCallback(IResourceMgrCallback *Callback) = 0;
	virtual void AddSearchPath(const char *Path) = 0;
	virtual void RemoveSearchPath(const char *Path) = 0;
	virtual bool LoadHeader(const char *Filename, unsigned int *LocalVersion, unsigned int *ModVersion, unsigned int *ObjType) = 0;
};


