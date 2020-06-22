#include "ResourceMgr.h"
#include "ModelFile.h"
#include "SurfaceFile.h"
#include "AsyncTask/ImportTask.h"
#include "AsyncTask/LoadTask.h"
#include "AsyncTask/ProtocalTask.h"
#include "AsyncTask/ResAsyncTaskMgr.h"
#include "DRGameMode.h"
#include "EditorGameInstance.h"
#include "AsyncTask/UploadLoadTask.h"
#include "AsyncTask/CookTask.h"
#include "Model/CompoundModelFile.h"
#include "ResourceMgrComponent.h"
#include "HAL/FileManager.h"
#include <VaRestSubsystem.h>

DEFINE_LOG_CATEGORY(LogResMgr);

UResourceMgr * UResourceMgr::s_Instance = NULL;
FCriticalSection UResourceMgr::CriticalSection;

UResourceMgr::UResourceMgr(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, bTickable(false)
{
}

UWorld *UResourceMgr::GetWorld() const
{
	UObject *Outer = GetOuter();
	if (Outer)
	{
		return Outer->GetWorld();
	}
	return NULL;
}

UResourceMgr *UResourceMgr::Create(UObject *Outer)
{
	if (!s_Instance)
	{
		s_Instance = NewObject<UResourceMgr>(Outer, TEXT("ResourceManager"));
		if (s_Instance)
		{
			s_Instance->ConditionalInit();
		}
	}
	return s_Instance;
}

void UResourceMgr::Destroy()
{
	if (s_Instance)
	{
		s_Instance->ReleaseAll();
		s_Instance->ConditionalBeginDestroy();
		s_Instance = NULL;
	}
}

UResourceMgr *UResourceMgr::Instance(UObject *WorldContextObject) 
{
	return s_Instance;
}

UResourceMgr *UResourceMgr::GetResourceMgr()
{
	return s_Instance;
}

void UResourceMgr::ConditionalInit()
{
	if (!bImportPreloadResource)
	{
		bImportPreloadResource = true;
		UpdateFromFileSystem();
	}
}

UModelFile *UResourceMgr::NewModel(bool bTransient /*= false*/)
{
	UModelFile *Model = NULL;
	FString Skuid;

	if (bTransient)
	{
		Model = NewObject<UModelFile>();
		Model->ResMgr = this;
		Model->bLoaded = 1;
		Model->bHeadLoaded = 1;
	}
	else
	{
		if (RequestSkuid(Skuid))
		{
			Model = NewObject<UModelFile>();
			Model->SetResID(Skuid);
			Model->ResMgr = this;
			Model->bLoaded = 1;
			Model->bHeadLoaded = 1;
			Model->MarkDirty();
			AddRes(EResModel, Skuid, TEXT(""), Model, true);
		}
	}

	return Model;
}

UCompoundModelFile *UResourceMgr::NewCompoundModel(bool bTransient /*= false*/)
{
	UCompoundModelFile *Model = NULL;
	FString Skuid;

	if (bTransient)
	{
		Model = NewObject<UCompoundModelFile>();
		Model->ResMgr = this;
		Model->bLoaded = 1;
		Model->bHeadLoaded = 1;
	}
	else
	{
		if (RequestSkuid(Skuid))
		{
			Model = NewObject<UCompoundModelFile>();
			Model->SetResID(Skuid);
			Model->ResMgr = this;
			Model->bLoaded = 1;
			Model->bHeadLoaded = 1;
			Model->MarkDirty();
			AddRes(EResCompoundModel, Skuid, TEXT(""), Model, true);
		}
	}

	return Model;
}

USurfaceFile *UResourceMgr::NewSurface()
{
	USurfaceFile *Surface = NULL;

	FString Skuid;

	if (RequestSkuid(Skuid))
	{
		Surface = NewObject<USurfaceFile>();
		Surface->SetResID(Skuid);
		Surface->bLoaded = 1;
		Surface->bHeadLoaded = 1;
		Surface->ResMgr = this;
		Surface->MarkDirty();
		AddRes(EResSurface, Skuid, TEXT(""), Surface, true);
	}

	return Surface;
}

USurfaceFile * UResourceMgr::NewSurfaceByName(const FString & EnglishName)
{
	USurfaceFile *Surface = NULL;

	FString Skuid;

	if (RequestSkuid(Skuid))
	{
		Surface = NewObject<USurfaceFile>();
		Surface->SetResID(Skuid);
		Surface->bLoaded = 1;
		Surface->bHeadLoaded = 1;
		Surface->ResMgr = this;
		Surface->MarkDirty();
		FString NewFilename = UResource::GetUserSaveFilename(EnglishName, Skuid, EResSurface, true);
		AddRes(EResSurface, Skuid, NewFilename, Surface, true);
	}

	return Surface;
}


UModelFile *UResourceMgr::AsyncLoadFBX(UModelFile *InUpdateModel, const FString &InFilename, UObject *WorldContextObject)
{
	UModelFile *ModelFile = NULL;
	UResourceMgr *ResMgr = UResourceMgr::Instance(WorldContextObject);

	return InUpdateModel? InUpdateModel : ModelFile;
}

UCompoundModelFile *UResourceMgr::AsyncLoadCompoundFBX(UCompoundModelFile *InUpdateModel, const FString &InFilename, UObject *WorldContextObject)
{
	UCompoundModelFile *CompoundFile = NULL;

	UResourceMgr *ResMgr = UResourceMgr::Instance(WorldContextObject);

	return CompoundFile;
}

UResource *UResourceMgr::AsyncLoadRes(const FString &InSkuid, UObject *WorldContextObject, bool bNotifyIfLoaded /*= false*/)
{
	UResource *FoundRes = NULL;

	UResourceMgr *ResMgr = UResourceMgr::Instance(WorldContextObject);
	if (ResMgr)
	{
		FoundRes = ResMgr->LoadResByID(InSkuid, bNotifyIfLoaded);
	}

	return FoundRes;
}

UResource *UResourceMgr::LoadResByID(const FString &InResID, bool bNotifyIfLoaded)
{
	UResource * FoundRes = FindRes(InResID, true);
	
	if (FoundRes)
	{
		if (FoundRes->IsNeedLoad())
		{
			LoadRes(FoundRes);
		}
		else
		{
			if (bNotifyIfLoaded)
			{
				if (FoundRes->StateChanged.IsBound())
				{
					FoundRes->StateChanged.Broadcast(FoundRes, EResState::EResLoaded, FoundRes->DRInfo);
				}
			}
		}
	}

	return FoundRes;
}

void UResourceMgr::AsyncDownloadRes(const FString &Filename, const FString &Resid, EResType ResType, const FString &URL, const TArray<FDownloadingRes> &DependsResources, FOnDownloadTaskCompeleted Delegate, UObject *WorldContextObject, int32 SrvVersion/*= 0*/)
{
	UResource *DownloadResource = NULL;
	UResourceMgr *ResMgr = UResourceMgr::Instance(WorldContextObject);

	if (ResMgr)
	{
		ResMgr->DownloadRes(Filename, Resid, ResType, URL, DependsResources, Delegate, SrvVersion);
	}
}

bool UResourceMgr::IsInLocalHas(const FString &filename)
{
	FString savepath= FPaths::ProjectSavedDir();
	FString p1 = savepath +"Resources/Local/"+ filename;
	FString p2 = savepath + "Download/Resources/" + filename;
	return IFileManager::Get().FileExists(*p1)|| IFileManager::Get().FileExists(*p2);
		
}

void UResourceMgr::LoadAllSXResources()
{
	TArray<FString> sxFilePaths;
	FString extensionFilter(FPaths::ProjectContentDir() / TEXT("_Basic/Materials/BaseSx/*.sx"));
	IFileManager::Get().FindFiles(sxFilePaths, *extensionFilter, true, false);

	FString fullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("_Basic/Materials/BaseSx/"));

	for (const FString& sxFile : sxFilePaths)
	{
		Preload(fullPath / sxFile, true, false);
	}
}

void UResourceMgr::DownloadRes(const FString &Filename, const FString &Resid, EResType ResType, const FString &URL, const TArray<FDownloadingRes> &DependsResources, FOnDownloadTaskCompeleted Delegate, int32 SrvVersion)
{
	FDRAsyncTaskManager::Get().ExecuteTask(new FAsyncTask<FDownloadAsyncTask>(this, ResType, Resid, URL, Filename, DependsResources, Delegate, SrvVersion));
}

USurfaceFile *UResourceMgr::AsyncLoadSurface(const FString &InFilename, UObject *WorldContextObject, bool bNotifyIfLoaded /*= false*/)
{
	USurfaceFile *SurfaceFile = NULL;
	UResourceMgr *Instance = UResourceMgr::Instance(WorldContextObject);

	if (Instance !=NULL)
	{
		SurfaceFile = Cast<USurfaceFile>(Instance->LoadResByFile(InFilename, bNotifyIfLoaded));
	}

	return SurfaceFile;
}


UModelFile *UResourceMgr::AsyncLoadModel(const FString &InFilename, UObject *WorldContextObject, bool bNotifyIfLoaded /*= false*/)
{
	UModelFile *ModelFile = NULL;
	UResourceMgr *Instance = UResourceMgr::Instance(WorldContextObject);

	if (Instance !=NULL)
	{
		ModelFile = Cast<UModelFile>(Instance->LoadResByFile(InFilename, bNotifyIfLoaded));
	}

	return ModelFile;
}

UResource *UResourceMgr::LoadResByFile(const FString &Filename, bool bNotifyIfLoaded)
{
	UResource *Resource = FindResByFilename(Filename, true);
	
	if (!Resource)
	{
		Resource = Preload(Filename, true, true);
	}
	
	if (Resource)
	{
		if (Resource->IsNeedLoad())
		{
			LoadRes(Resource);
		}
		else
		{
			if (bNotifyIfLoaded)
			{
				if (Resource->StateChanged.IsBound())
				{
					Resource->StateChanged.Broadcast(Resource, EResState::EResLoaded, Resource->DRInfo);
				}
			}
		}
	}

	return Resource;
}

void UResourceMgr::LoadRes(UResource *InResource)
{
	if (InResource && InResource->IsNeedLoad())
	{
		FDRAsyncTaskManager::Get().QueueTask(new FAsyncTask<FResLoadAsyncTask>(InResource));
	}
}

void UResourceMgr::FreeSlot(int32 SlotIndex)
{
	if (PooledResource.IsValidIndex(SlotIndex))
	{
		FResourceInfo &Info = PooledResource[SlotIndex];
		Info = FResourceInfo();
		UnusedResource.Add(SlotIndex);
	}
}

int32 UResourceMgr::AllocSlot()
{
	int32 SlotIndex = INDEX_NONE;
	if (UnusedResource.Num() > 0)
	{
		 SlotIndex = UnusedResource.Pop();
	}
	else
	{
		SlotIndex = PooledResource.AddDefaulted();
	}
	return SlotIndex;
}

void UResourceMgr::AddRes(EResType ResType, const FString &ResID, const FString &Filename, UResource *InResource, bool bNotify)
{		
	{
		FScopeLock Scope(&CriticalSection);
		int32 ResouceIndex =  AllocSlot();

		PooledResource[ResouceIndex] = FResourceInfo(ResType, ResID, Filename, InResource);
		ResidResMap.Add(ResID, ResouceIndex);
		if (Filename.Len() > 0)
		{
			FilenameResMap.Add(Filename, ResouceIndex);
		}
	}

	if (bNotify && ResourceListChanged.IsBound())
	{
		ResourceListChanged.Broadcast();
	}
}

bool UResourceMgr::RenameFilename(UResource *InResource, const FString &NewFilename)
{
	if (InResource)
	{
		FScopeLock Scope(&CriticalSection);

		if (InResource->Filename.Len() > 0)
		{
			if (IFileManager::Get().FileExists(*InResource->Filename))
			{
				if (!IFileManager::Get().Move(*NewFilename, *InResource->Filename))
				{
					return false;
				}
			}

			int32 *SlotIndex = FilenameResMap.Find(InResource->Filename);
			if (SlotIndex)
			{
				FilenameResMap.Remove(InResource->Filename);
				InResource->Filename = NewFilename;
				FilenameResMap.FindOrAdd(NewFilename) = *SlotIndex;
			}

			return true;
		}
		else
		{
			int32 *SlotIndex = ResidResMap.Find(InResource->GetResID());
			if (SlotIndex)
			{
				InResource->Filename = NewFilename;
				FilenameResMap.FindOrAdd(NewFilename) = *SlotIndex;
			}
			return true;
		}
	}
	return false;
}

USurfaceFile *UResourceMgr::ExtractSurface(UModelFile *Model, int32 iSurface, const FSurfaceInfo &ChineseName, const FSurfaceInfo &EnglishName)
{
	USurfaceFile *Surface = NULL;
	
	if (Model)
	{
		Surface = Model->ExtractSurface(iSurface, ChineseName, EnglishName);
		if (Surface)
		{
			Model->MarkDirty();
			Surface->MarkDirty();
		}
	}

	return Surface;
}

USurfaceFile * UResourceMgr::CreateSurface(const FMaterialListItem &item, const FSurfaceInfo &ChineseName, const FSurfaceInfo &EnglishName)
{
	USurfaceFile *pNewSurface = NULL;
	
	UMaterialInstance *ue4MtrlInst = Cast<UMaterialInstance>(item.Material);
	if (!ue4MtrlInst && item.Surface)
	{
		ue4MtrlInst = Cast<UMaterialInstance>(item.Surface->GetUE4Material());
	}

	if (ue4MtrlInst)
	{
		pNewSurface = ImportFromUE4(ue4MtrlInst);
	}

	if (pNewSurface)
	{
		pNewSurface->SetResourceName(EnglishName.Name, true);
		//设置filename
		FString ResID = pNewSurface->GetResID();
		int32 *pSlotIndex = ResidResMap.Find(ResID);
		if (*pSlotIndex)
		{
			PooledResource[*pSlotIndex].Filename = pNewSurface->Filename;
		}
		if (item.Surface) {
			pNewSurface->SetSurface(item.Surface->GetMaterial(), item.Surface->m_Textures);
		}
		else
		{
			FModelMaterialUE4  *ue4Material = pNewSurface->GetMaterialUE4();
			if (ue4Material && item.Surface)
			{
				FModelMaterial *modelMaterial = item.Surface->GetMaterial();
				if (modelMaterial)
				{
					pNewSurface->CopyOverrideParameters(modelMaterial);
					pNewSurface->CopyOverrideTextures(modelMaterial, item.Surface->m_Textures);
				}
			}
		}
		
	}
	pNewSurface->DRInfo = UVaRestSubsystem::StaticConstructVaRestJsonObject();
	pNewSurface->DRInfo->SetStringField("ChineseName", ChineseName.Name);
	return pNewSurface;
}

UResource *UResourceMgr::GetSlot(int32 SlotIndex, bool bNeedHeader)
{
	UResource *FoundRes = NULL;

	if (PooledResource.IsValidIndex(SlotIndex))
	{
		FResourceInfo &Info = PooledResource[SlotIndex];
		if (!Info.Resource)
		{
			Info.Resource = CreateResByType(Info.ResType);
			if (Info.Resource)
			{
				Info.Resource->ResMgr = this;
				Info.Resource->Filename = Info.Filename;
				Info.Resource->SetResID(Info.ResID);
				if (bNeedHeader)
				{
					LoadHeader(Info.Resource);
				}
			}
		}
		else
		{
			if (bNeedHeader && !Info.Resource->bHeadLoaded)
			{
				LoadHeader(Info.Resource);
			}
		}
		FoundRes = Info.Resource;
	}
	return FoundRes;
}

UResource *UResourceMgr::FindRes(const FString &InResID, bool bNeedHeader, int modelid )
{
	UResource *FoundRes = NULL;
	FString Temp = InResID;
	if (true)
	{
		FScopeLock Scope(&CriticalSection);
		int32 *SlotIndex = ResidResMap.Find(Temp);
		if (SlotIndex)
		{
			FoundRes = GetSlot(*SlotIndex, bNeedHeader);
		}
	}

	return FoundRes;
}

UResource *UResourceMgr::FindResByFilename(const FString &Filename, bool bNeedHeader)
{
	UResource *FoundRes = NULL;
	
	if (true)
	{
		FScopeLock Scope(&CriticalSection);
		int32 *SlotIndex = FilenameResMap.Find(Filename);
		if (SlotIndex)
		{
			FoundRes = GetSlot(*SlotIndex, bNeedHeader);
		}
	}

	return FoundRes;
}


void UResourceMgr::UploadResource(UResource *Resource, const FString &URL)
{
	if (Resource)
	{
		FDRAsyncTaskManager::Get().QueueTask<FUploadAsyncTask>(new FAsyncTask<FUploadAsyncTask>(Resource, URL));
	}
}

void UResourceMgr::QueryMaterialBrandTree(int32 UserID)
{
	UProtocalImpl *Protocal = UProtocalImpl::GetProtocal(this);
	UVaRestJsonObject *JsonObj = UVaRestSubsystem::StaticConstructVaRestJsonObject();
	if (Protocal)
	{
		FProtocalDelegate Delegate;
		Protocal->Protocal_QueryMaterialBrandTree(UserID, JsonObj, Delegate);
		FDRAsyncTaskManager::Get().ExecuteTask<FProtocalTask>(new FAsyncTask<FProtocalTask>(nullptr, JsonObj, Delegate));
	}
}

void UResourceMgr::QueryMaterialById(int32 MaterialId)
{
	UProtocalImpl *Protocal = UProtocalImpl::GetProtocal(this);
	UVaRestJsonObject *JsonObj = UVaRestSubsystem::StaticConstructVaRestJsonObject();
	if (Protocal)
	{
		FProtocalDelegate Delegate;
		Protocal->Protocal_QueryMaterialById(MaterialId, JsonObj, Delegate);
		FDRAsyncTaskManager::Get().ExecuteTask<FProtocalTask>(new FAsyncTask<FProtocalTask>(nullptr, JsonObj, Delegate));
	}
}

void UResourceMgr::QueryMaterialByPage(int32 PageIndex, int32 PageSize, const FString &OrderBy, int32 BrandId, const FString &Name, int32 UserId)
{
	UProtocalImpl *Protocal = UProtocalImpl::GetProtocal(this);
	UVaRestJsonObject *JsonObj = UVaRestSubsystem::StaticConstructVaRestJsonObject();
	if (Protocal)
	{
		FProtocalDelegate Delegate;
		Protocal->Protocal_QueryMaterialByPage(PageIndex, PageSize, OrderBy, BrandId, Name, UserId, JsonObj, Delegate);
		FDRAsyncTaskManager::Get().ExecuteTask<FProtocalTask>(new FAsyncTask<FProtocalTask>(nullptr, JsonObj, Delegate));
	}
}

void UResourceMgr::RemoveMaterialById(int32 MaterialId)
{
	UProtocalImpl *Protocal = UProtocalImpl::GetProtocal(this);
	UVaRestJsonObject *JsonObj = UVaRestSubsystem::StaticConstructVaRestJsonObject();
	if (Protocal)
	{
		FProtocalDelegate Delegate;
		Protocal->Protocal_QueryMaterialById(MaterialId, JsonObj, Delegate);
		FDRAsyncTaskManager::Get().ExecuteTask<FProtocalTask>(new FAsyncTask<FProtocalTask>(nullptr, JsonObj, Delegate));
	}
}

USurfaceFile *UResourceMgr::ImportFromUE4(UMaterialInstance *MaterialInst)
{
	USurfaceFile *Surface = NULL;

	if (MaterialInst)
	{
		Surface = NewSurface();
		Surface->SetType(EMaterialUE4);

		FModelMaterialUE4 *materialInfo = Surface->GetMaterialUE4();
		if (materialInfo)
		{
			materialInfo->UE4Path = MaterialInst->GetPathName();
		}

		Surface->MarkDirty();
	}

	return Surface;
}

bool UResourceMgr::RequestSkuid(FString &OutSkuid)
{
	FGuid NewGuid;
	FPlatformMisc::CreateGuid(NewGuid);
	OutSkuid = NewGuid.ToString();
	return true;
}

void UResourceMgr::UpdateFromFileSystem()
{

}

void UResourceMgr::StopDownload()
{
	FDRAsyncTaskManager::Get().ClearDownload();
}

bool UResourceMgr::IsPreloadResFromFileSystem()
{
#if UE_SERVER
	return false;
#else
	return FinishUpdateFileSystemCouter.GetValue() > 0;
#endif
}

UResource * UResourceMgr::CreateResByType(EResType ResType)
{
	UResource *Resource = NULL;
	if (ResType == EResModel)
	{
		Resource = NewObject<UModelFile>();
		Resource->AddToRoot();
	}
	else if (ResType==EResSurface)
	{
		Resource = NewObject<USurfaceFile>();
		Resource->AddToRoot();
	}
	else if (ResType == EResCompoundModel)
	{
		Resource = NewObject<UCompoundModelFile>();
		Resource->AddToRoot();
	}

	if (Resource)
	{
		Resource->ResMgr = this;
	}

	return Resource;
}

void  UResourceMgr::LoadHeader(UResource *Resource)
{
	if (Resource && !Resource->bHeadLoaded)
	{

		FArchive *Reader = IFileManager::Get().CreateFileReader(*Resource->Filename);
		UE_LOG(LogTemp, Log, TEXT("LoadHeader use mx file[%s]"), *Resource->Filename);

		if (Reader)
		{
			TEnumAsByte<EResType> ResType;
			*Reader << ResType;
			if (UResource::CheckMark(*Reader))
			{
				Resource->SerializeHeader(*Reader);
				Resource->bHeadLoaded = 1;
			}
			UE_LOG(LogResMgr, Log, TEXT("load file header :%s, game thread[%s]"), *(Resource->Filename), (IsInGameThread() ? TEXT("true") : TEXT("false")));
			Reader->Close();
			delete Reader;
		}
	}
}

UResource *UResourceMgr::Preload(const FString &Filename, bool bNeedHeader, bool bNotify)
{
	UResource *Resource = NULL;

	FArchive *Reader = IFileManager::Get().CreateFileReader(*Filename);

	if (Reader)
	{
		TEnumAsByte<EResType> ResType;
		*Reader << ResType;
		
		if (UResource::CheckMark(*Reader))
		{
			if (bNeedHeader)
			{
				Resource = CreateResByType(ResType);
				Resource->bHeadLoaded = 1;
				Resource->Filename = Filename;
				if (Resource)
				{
					Resource->SerializeHeader(*Reader);
					AddRes(ResType, Resource->GetResID(), Filename, Resource, bNotify);
				}
			}
			else
			{
				FString BaseName = FPaths::GetBaseFilename(Filename);
				int32 index = 0;
				if (BaseName.FindLastChar(TEXT('_'), index))
				{
					FString ResId = BaseName.Mid(index + 1);
					AddRes(ResType, ResId, Filename, NULL, bNotify);
				}
			}
		}

		UE_LOG(LogResMgr, Log, TEXT("Preload file :%s, game thread[%s]"), *Filename, (IsInGameThread()? TEXT("true"):TEXT("false")));
		Reader->Close();
		delete Reader;
	}
	else
	{
		UE_LOG(LogResMgr, Error, TEXT("Preload file :%s failed! game thread[%s]"), *Filename, (IsInGameThread() ? TEXT("true") : TEXT("false")));
	}

	return Resource;
}


void UResourceMgr::UpdateModelsByCategory(const FName &Level1, const FName &Level2, const FName &Level3, TArray<int32> &Resources)
{
	FScopeLock Scope(&CriticalSection);

	for (int32 i = 0; i < PooledResource.Num(); ++i)
	{
		FResourceInfo &Info = PooledResource[i];
		if (Info.ResType == EResType::EResModel)
		{
			Resources.Add(i);
		}
	}
}

void UResourceMgr::SaveAll()
{
	FScopeLock Scope(&CriticalSection);

	for (int32 i = 0; i < PooledResource.Num(); ++i)
	{
		FResourceInfo &Info = PooledResource[i];
		if (Info.Resource && Info.Resource->IsDirty())
		{
			Info.Resource->Save();
		}
	}
}

void UResourceMgr::FinishUpdateFile()
{ 
}

void UResourceMgr::AbandonUpdateFile()
{
}

bool UResourceMgr::IsAbandonUpdateFile()
{
	return false;
}

void UResourceMgr::ReleaseAll()
{
	FScopeLock Scope(&CriticalSection);

	for (int32 i = 0; i < PooledResource.Num(); ++i)
	{
		FResourceInfo &Info = PooledResource[i];

		if (Info.Resource != nullptr/* && Info.Resource->IsValidLowLevel()*/)
		{
			Info.Resource->RemoveFromRoot();
			Info.Resource->ResMgr = nullptr;
			Info.Resource->ConditionalBeginDestroy();
			Info.Resource = nullptr;
		}
	}
}

void UResourceMgr::ReleaseResource(UResource * InResource)
{
	FScopeLock Scope(&CriticalSection);

	for (int32 i = 0; i < PooledResource.Num(); ++i)
	{
		FResourceInfo &Info = PooledResource[i];

		if (Info.Resource != nullptr && Info.Resource->IsValidLowLevel())
		{
			Info.Resource->ConditionalBeginDestroy();
			Info.Resource = NULL;
		}
	}
}

int32 UResourceMgr::GetNumberOfStandardMaterials()
{
	return 0;
}


void UResourceMgr::BeginCook(UResource *Resource, const FCookArgs &InArgs/*= FCookArgs()*/)
{
	FDRAsyncTaskManager::Get().QueueTask(new FAsyncTask<FCookAsyncTask>(Resource, InArgs));
}

void UResourceMgr::CleanCache()
{
	for (int32 i = 0; i < PooledResource.Num(); ++i)
	{
		FResourceInfo &Info = PooledResource[i];
		if (Info.Resource)
		{
			Info.Resource->ConditionalBeginDestroy();
			Info.Resource = NULL;
		}
	}
}

void UResourceMgr::GetResourceList(TArray<UResource *> &ResourceList)
{
	FScopeLock Scope(&CriticalSection);
	for (int32 i = 0; i < PooledResource.Num(); ++i)
	{
		FResourceInfo &Info = PooledResource[i];
		if (Info.Resource)
		{
			ResourceList.Add(Info.Resource);
		}
	}
}

void UResourceMgr::GetMaterialList(TArray<FMaterialListItem> &MaterialItems,const FString &CategoryName, const FString &SeachName, bool bOther, bool bAll, bool isbasesx)
{

}



void UResourceMgr::FreeResource(UResource *Resource)
{
	if (Resource)
	{
		FString ResID = Resource->GetResID();
		int32 *SlotIndex = ResidResMap.Find(ResID);
		if (SlotIndex)
		{
			FResourceInfo &Info = PooledResource[*SlotIndex];
			if (Info.Resource == Resource)
			{
				Info.Resource->ConditionalBeginDestroy();
				Info.Resource = NULL;
			}
		}
	}
}

void UResourceMgr::DeleteResource(UResource * Resource)
{
	if (IFileManager::Get().FileExists(*Resource->Filename))
	{
		IFileManager::Get().Delete(*Resource->Filename);
	}

	FString ResID = Resource->GetResID();
	int32 *pSlotIndex = ResidResMap.Find(ResID);

	if (pSlotIndex)
	{
		ResidResMap.Remove(Resource->GetResID());
		FilenameResMap.Remove(Resource->Filename);
		FreeResource(Resource);
		FreeSlot(*pSlotIndex);
	}

}

void UResourceMgr::DeleteResByFilename(const FString & InFilename)
{
	int32 *pSlotIndex = FilenameResMap.Find(InFilename);
	
	if (pSlotIndex)
	{
		FResourceInfo &Info = PooledResource[*pSlotIndex];
		if (Info.Resource)
		{
			FreeResource(Info.Resource);
			Info.Resource = NULL;
		}
		ResidResMap.Remove(Info.ResID);
		FilenameResMap.Remove(Info.Filename);
		FreeSlot(*pSlotIndex);
	}

	if (IFileManager::Get().FileExists(*InFilename))
	{
		IFileManager::Get().Delete(*InFilename);
	}
}


void UResourceMgr::RenameFilenameByModelID(const FString & ResID, const FString & modelID)
{
	int32 *pSlotIndex = ResidResMap.Find(ResID);

	if (pSlotIndex)
	{
		FResourceInfo &Info = PooledResource[*pSlotIndex];
		FString leftstr, rightstr;
		if (!Info.Resource) {
			Info.Resource = LoadResByID(Info.ResID, false);
		}
		
		if (Info.Resource&&Info.Resource->Filename.Len() > 0)
		{
			Info.Resource->Filename.Split("/", &leftstr, &rightstr,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
			RenameFilename(Info.Resource, leftstr+"/"+ modelID+"_"+ ResID+".mx");
		}
		
	}


}

