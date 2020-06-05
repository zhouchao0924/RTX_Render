
#pragma once

#include "Engine/EngineTypes.h"
#include "ResourceItem.h"
#include "StandardMaterialCollection.h"
#include "ResourceMgr.generated.h"

class UModelFile;
class USurfaceFile;
class UTextureImporter;
class UModelImporter;
class UAsyncHttpFile;

DECLARE_LOG_CATEGORY_EXTERN(LogResMgr, Log, All);

USTRUCT(BlueprintType)
struct FMaterialListItem
{
	GENERATED_BODY()
	FMaterialListItem();
	~FMaterialListItem();
	UPROPERTY(Transient, BlueprintReadOnly)
	int32				Index;
	UPROPERTY(Transient, BlueprintReadOnly)
	USurfaceFile		*Surface;
	UPROPERTY(Transient, BlueprintReadOnly)
	UMaterialInterface  *Material;
	UPROPERTY(Transient, BlueprintReadOnly)
	FString				 DisplayName;
	UPROPERTY(Transient, BlueprintReadOnly)
	UTexture			*PreviewImage;
};

USTRUCT(BlueprintType)
struct FResourceInfo
{
	GENERATED_BODY()

	FResourceInfo():Resource(NULL),ResType(EResType::EResUnknown)
	{
	}

	FResourceInfo(EResType InResType, const FString &InResID, const FString &InFilename, UResource *InResource)
		:ResType(InResType)
		,ResID(InResID)
		,Resource(InResource)
		,Filename(InFilename)
	{
	}
	UPROPERTY()
	TEnumAsByte<EResType>	ResType;
	UPROPERTY()
	FString					ResID;
	UPROPERTY()
	FString					Filename;
	UPROPERTY()
	UResource				*Resource;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResourceListChanged);

UCLASS(BlueprintType)
class UResourceMgr :public UObject
{
	GENERATED_UCLASS_BODY()
public:	
	static UResourceMgr *Create(UObject *Outer);
	static void Destroy();
	
	UFUNCTION(BlueprintPure, Category = "DR|Resource", meta = (DisplayName = "GetResourceManager", WorldContext = "WorldContextObject"))
	static UResourceMgr *GetResourceMgr();

	UFUNCTION(BlueprintCallable, Category = "DR|Resource", meta=(DisplayName="GetResourceManager", WorldContext = "WorldContextObject"))
	static UResourceMgr *Instance(UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource", meta = (WorldContext = "WorldContextObject"))
	static UModelFile *AsyncLoadModel(const FString &InFilename, UObject *WorldContextObject, bool bNotifyIfLoaded = false);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource", meta = (WorldContext = "WorldContextObject"))
	static USurfaceFile *AsyncLoadSurface(const FString &InFilename, UObject *WorldContextObject, bool bNotifyIfLoaded = false);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource", meta = (WorldContext = "WorldContextObject"))
	static UModelFile *AsyncLoadFBX(UModelFile *InUpdateModel, const FString &InFilename, UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource", meta = (WorldContext = "WorldContextObject"))
	static UCompoundModelFile *AsyncLoadCompoundFBX(UCompoundModelFile *InUpdateModel, const FString &InFilename, UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource", meta = (WorldContext = "WorldContextObject"))
	static UResource *AsyncLoadRes(const FString &InResID, UObject *WorldContextObject, bool bNotifyIfLoaded = false);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource", meta = (WorldContext = "WorldContextObject"))
	static void AsyncDownloadRes(const FString &Filename, const FString &Resid, EResType ResType, const FString &URL, const TArray<FDownloadingRes> &DependsResources, FOnDownloadTaskCompeleted Delegate, UObject *WorldContextObject, int32 SrvVersion = 0);
	
	UFUNCTION(BlueprintCallable, Category = "DR|Resource", meta = (WorldContext = "WorldContextObject"))
	static bool IsInLocalHas(const FString &filename);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	USurfaceFile *ExtractSurface(UModelFile *Model, int32 SurfaceIndex, const FSurfaceInfo &ChineseName, const FSurfaceInfo &EnglishName);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	USurfaceFile *CreateSurface(const FMaterialListItem &item,const FSurfaceInfo &ChineseName, const FSurfaceInfo &EnglishName);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	UResource *FindRes(const FString &InResID, bool bNeedHeader = false,int modelid = 0);	

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	UResource *FindResByFilename(const FString &Filename, bool bNeedHeader = false);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	USurfaceFile *ImportFromUE4(UMaterialInstance *MaterialInst);
	
	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	UModelFile *NewModel(bool bTransient = false);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	UCompoundModelFile *NewCompoundModel(bool bTransient = false);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	USurfaceFile *NewSurface();

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	USurfaceFile *NewSurfaceByName(const FString & EnglishName);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void StopDownload();

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void UpdateFromFileSystem();

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void SaveAll();

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	bool RequestSkuid(FString &OutSkuid);


	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void ReleaseAll();

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void ReleaseResource(UResource *InResource);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	int32 GetNumberOfStandardMaterials();

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	FStdMaterialInfo GetStandardMaterial(int32 MaterialIndex);
	UWorld *GetWorld() const override;

	//////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void UploadResource(UResource *Resource, const FString &URL);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void QueryMaterialBrandTree(int32 UserID);
	
	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void QueryMaterialById(int32 MaterialId);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void QueryMaterialByPage(int32 PageIndex, int32 PageSize, const FString &OrderBy, int32 BrandId, const FString &Name, int32 UserId);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void RemoveMaterialById(int32 MaterialId);
	
	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void BeginCook(UResource *Resource, const FCookArgs &InArgs);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void CleanCache();

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void FreeResource(UResource *Resource);

	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void DeleteResource(UResource *Resource);
	
	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void DeleteResByFilename(const FString & InFilename);
	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	void RenameFilenameByModelID(const FString &ResID, const FString &modelID);
	void ConditionalInit();
	UResource *LoadResByID(const FString &Resid, bool bNotifyIfLoaded);
	UResource *LoadResByFile(const FString &Filename, bool bNotifyIfLoaded);
	void DownloadRes(const FString &Filename, const FString &Resid, EResType ResType, const FString &URL, const TArray<FDownloadingRes> &DependsResources, FOnDownloadTaskCompeleted Delegate, int32 SrvVersion);
	void UpdateModelsByCategory(const FName &Level1, const FName &Level2, const FName &Level3, TArray<int32> &Resources);

	void GetMaterialList(TArray<FMaterialListItem> &MaterialList, const FString &CategoryName, const FString &SeachName, bool bOther, bool bAll ,bool isbasesx=true);
	void GetResourceList(TArray<UResource *> &ResourceList);
	UResource *Preload(const FString &Filename, bool bNeedHeader, bool bNotify);

protected:

	bool IsPreloadResFromFileSystem();
	UResource *CreateResByType(EResType ResType);
	void LoadRes(UResource *InResource);
	void AddRes(EResType ResType, const FString &ResID, const FString &Filename, UResource *Resource, bool bNotify);
	bool RenameFilename(UResource *InResource, const FString &NewFilename);
	
	void  LoadHeader(UResource *Resource);
	void  FreeSlot(int32 SlotIndex);
	int32 AllocSlot();
	UFUNCTION(BlueprintCallable, Category = "DR|Resource")
	UResource *GetSlot(int32 SlotIndex, bool bNeedHeader);

	void FinishUpdateFile();
	void AbandonUpdateFile();
	bool IsAbandonUpdateFile();
	friend class UResource;
	friend class FUpdateFileSystemAsyncTask;
	static UResourceMgr *s_Instance;
public:
	UPROPERTY(Transient, BlueprintAssignable, Category = "DR|Resource")
	FOnResourceListChanged					ResourceListChanged;
	UPROPERTY(Transient)
	bool									bImportPreloadResource;
	
	bool												bTickable;	
	FCriticalSection									CriticalSection;

	UPROPERTY(Transient)
	TMap<FString, int32>					ResidResMap;
	UPROPERTY(Transient)
	TMap<FString, int32>					FilenameResMap;
	UPROPERTY(Transient)
	TArray<FResourceInfo>					PooledResource;
	UPROPERTY(Transient)
	TArray<int32>							UnusedResource;
	FThreadSafeCounter						FinishUpdateFileSystemCouter;
	FAutoDeleteAsyncTask<FUpdateFileSystemAsyncTask>	*UpdateFileSystemTask;

	int										RunModeid;//ÐÂ×ÊÔ´µ÷¶È¹²Ïí×ÊÔ´
	FString								    IrayMXDir;//资源共享盘目录
};

