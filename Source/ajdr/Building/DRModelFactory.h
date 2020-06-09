
#pragma once

#include "Building/DRStruct.h"
#include "Building/BuildingData.h"
#include "Building/BuildingActor.h"
#include "DRComponentModel.h"
#include "Building/BuildingActorFactory.h"
#include "DRModelFactory.generated.h"


UCLASS(BlueprintType)
class UDRModelFactory : public UObject//, public IBuildActorFactory
{
	GENERATED_BODY()
public://Buleprint Propety 1.0 -> 2.0
	UPROPERTY(BlueprintReadWrite)
		bool isLoading;
	//UPROPERTY(BlueprintReadWrite)
	//	TMap<int32, FTempComModelData> ComModelData;
	UPROPERTY(BlueprintReadWrite)
		TMap<int32, FDRModelInstance> ModelData;
	UPROPERTY(BlueprintReadWrite)
		TMap<int32, FDRHoleStruct> HoleData;
	UPROPERTY(BlueprintReadWrite)
		TMap<int32, FVector2D> CornerData;
	UPROPERTY(BlueprintReadWrite)
		FDRHouseImage HouseImageData;

public://Buleprint
	UFUNCTION(BlueprintNativeEvent)
		ADRActor * CreateActorByData(UBuildingData *BuildingData);
private:
	UWorld * World;
	TMap<int32, ADRActor*> ModelActor;
	TMap<int32, ADRActor*> ComModelActor;
private:
	
	ADRActor * CreateComponentModel(UBuildingData *& Data, int32 & AnchorID);
	
	ADRActor * CreateModel(UBuildingData *& Data);

public://C++
	void SetWorld(UWorld * _World);
};
