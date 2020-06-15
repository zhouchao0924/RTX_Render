
#include "DRGameMode.h"
#include "ResourceMgr.h"
#include "EditorGameInstance.h"
#include "BuildingSDKSystem.h"
#include "Building/LoadingActor.h"
#include "Building/BuildingSystem.h"
#include "Building/DRModelFactory.h"
#include "Cabinet/CabinetMgr.h"

ADRGameMode::ADRGameMode(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}
void ADRGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ADRGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);	
}

void ADRGameMode::InitAllSubsystem()
{
	IBuildingSDK *buildSdk = UBuildingSystem::GetBuildingSDK();
	UResourceMgr::Create(this);

	ABuildingSDKSystem::CreateInstance(this);

	if (!MyGameInstance)
	{
		MyGameInstance = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	}
	if (MyGameInstance && MyGameInstance->GetBuildingSystemInstance())
	{
		MyGameInstance->GetBuildingSystemInstance()->LoadingConfig(GetConfig());
		MyGameInstance->GetBuildingSystemInstance()->AddToWorld(GetWorld());

	}
	UCabinetMgr *CabinetMgr = UCabinetMgr::CreateInstance(this);
	if (CabinetMgr)
	{
		CabinetMgr->Init();
	}
}

void ADRGameMode::ShutdownAllSubsystem()
{
	UBuildingSystem *BuildingSystem = MyGameInstance ? MyGameInstance->GetBuildingSystemInstance() : nullptr;
	if (BuildingSystem)
	{
		BuildingSystem->RemoveFromWorld(GetWorld());
		BuildingSystem->ClearSuite();
	}

	UCabinetMgr *CabinetMgr = UCabinetMgr::Get();
	if (CabinetMgr)
	{
		CabinetMgr->UnInit();
	}

	UResourceMgr::Destroy();

	ABuildingSDKSystem::DestroyInstance();
}

