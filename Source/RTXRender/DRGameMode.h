#pragma once

#include "GameFramework/GameMode.h"
#include "Building/BuildingConfig.h"
#include "DRGameMode.generated.h"

class UCEditorGameInstance;
class UStandardMaterialCollection;

UCLASS(BlueprintType, abstract)
class ADRGameMode :public AGameMode
{
	GENERATED_UCLASS_BODY()
public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	FBuildingConfig *GetConfig() { return &BuildingConfig; } 
private:
	UPROPERTY(EditAnywhere)
		FBuildingConfig	BuildingConfig;
	class UCEditorGameInstance *MyGameInstance;
};
