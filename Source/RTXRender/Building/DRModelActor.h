
#pragma once
#include "Building/DRStruct.h"
#include "Building/DRActor.h"
#include "Building/BuildingData.h"
#include "Model/ModelFileComponent.h"
#include "DRModelActor.generated.h"

UCLASS()
class ADRModelActor : public ADRActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		UBuildingData * BuildData;
	FDRModelInstance ModelStruct;
	UModelFileComponent * ModelCom;
public:
	ADRModelActor();
	UFUNCTION(BlueprintCallable)
	virtual void  UpdateBuildingData() override;
	virtual void SaveSelf() override;
	virtual void LoadSelf() override;
};