
#include "BuildingSDKSystem.h"

static ABuildingSDKSystem *SDKActor = nullptr;

ABuildingSDKSystem::ABuildingSDKSystem(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
	, SolutionComponent(nullptr)
	, ResourceMgr(nullptr)
	, SDKComponent(nullptr)
	, BuildingSceneComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = 1;
	PrimaryActorTick.SetTickFunctionEnable(true);

	BGPoolManager = ObjectIntializer.CreateDefaultSubobject<UBGCPoolManager>(this, "BGPoolManager");
	ResourceMgr = ObjectIntializer.CreateDefaultSubobject<UResourceMgrComponent>(this, "ResourceMgr");
	SDKComponent = ObjectIntializer.CreateDefaultSubobject<UBuildingSDKComponent>(this, "SDKComponent");
	BuildingSceneComponent = ObjectIntializer.CreateDefaultSubobject<UBuildingSceneComponent>(this, "BuildingSceneComponent");
}

ABuildingSDKSystem *ABuildingSDKSystem::GetSDKActor()
{
	return SDKActor;
}

ABuildingSDKSystem *ABuildingSDKSystem::CreateInstance(UObject *WorldContextObject)
{
	UWorld *MyWorld = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (!SDKActor)
	{
		SDKActor = MyWorld->SpawnActor<ABuildingSDKSystem>();
	}
	return SDKActor;
}

void ABuildingSDKSystem::DestroyInstance()
{
	if (SDKActor)
	{
		SDKActor->Destroy(true);
		SDKActor = nullptr;
	}
}

void ABuildingSDKSystem::BeginPlay()
{
	Super::BeginPlay();

	IBuildingSDK *pSDK = GetBuildingSDK();
	if (pSDK)
	{
		IBuildingResourceMgr *ResMgr = pSDK->GetResourceMgr();
		if (ResMgr)
		{
			TArray<FString> FoundDirectories;
			FoundDirectories.Add(FPaths::ProjectContentDir() + TEXT("_Basic/Materials/BaseSx/"));
			FoundDirectories.Add(FPaths::ProjectContentDir() + TEXT("Geometry/"));
			FoundDirectories.Add(FPaths::ProjectSavedDir() + TEXT("Download/Resources/"));
			FoundDirectories.Add(FPaths::ProjectSavedDir() + TEXT("Resources/Local/"));

			for (int32 i = 0; i < FoundDirectories.Num(); ++i)
			{
				std::string SearchPath = TCHAR_TO_ANSI(*FoundDirectories[i]);
				ResMgr->AddSearchPath(SearchPath.c_str());
			}
		}

		SDKActor = this;
	}
}

USolutionComponent *ABuildingSDKSystem::CreateSolution(const FString &Filename)
{
	if (!SolutionComponent)
	{
		SolutionComponent = NewObject<USolutionComponent>(this, "SolutionComponent");
	}

	if (Filename.Len()<=0)
	{
		SolutionComponent->CreateNewSuite();
	}

	return SolutionComponent;
}

void ABuildingSDKSystem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	IBuildingSDK *sdk = GetBuildingSDK();
	if (sdk)
	{
		sdk->Tick(DeltaSeconds);
	}
}

void ABuildingSDKSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ResourceMgr)
	{
		ResourceMgr->ReleaseAll();
	}

	if (BGPoolManager)
	{
		BGPoolManager->ReleaseAll();
	}

	SDKActor = nullptr;
	Super::EndPlay(EndPlayReason);
}

int32 ABuildingSDKSystem::GetMemoryUsed()
{
	FMemorySummary Summary;
	IBuildingSDK *pSDK = GetBuildingSDK();
	if (pSDK && pSDK->GetMemorySummary(Summary))
	{
		return Summary.MemoryUsed;
	}
	return 0;
}

int32 ABuildingSDKSystem::GetObjectCount()
{
	FMemorySummary Summary;
	IBuildingSDK *pSDK = GetBuildingSDK();
	if (pSDK && pSDK->GetMemorySummary(Summary))
	{
		return Summary.ObjectCount;
	}
	return 0;
}


