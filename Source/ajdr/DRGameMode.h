#pragma once

#include "GameFramework/GameMode.h"
#include "Building/BuildingConfig.h"
#include "DRGameMode.generated.h"

class UCEditorGameInstance;
class UStandardMaterialCollection;

UCLASS(BlueprintType, abstract, Config=Game)
class ADRGameMode :public AGameMode
{
	GENERATED_UCLASS_BODY()
public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	FBuildingConfig *GetConfig() { return &BuildingConfig; }

	UFUNCTION(BlueprintCallable)
		void InitAllSubsystem();

	UFUNCTION(BlueprintCallable)
		void ShutdownAllSubsystem();
public:

	UPROPERTY(GlobalConfig, BlueprintReadOnly)
		FString mExtendedResourcePath;

private:
	UPROPERTY(EditAnywhere)
		FBuildingConfig	BuildingConfig;
	class UCEditorGameInstance *MyGameInstance;
};
