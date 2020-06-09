
#pragma once
#include "UObject/Interface.h"
#include "./Building/DRStruct.h"
#include "BuildingActorFactory.generated.h"

UINTERFACE(BlueprintType)
class UBuildActorFactory : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IBuildActorFactory
{
	GENERATED_IINTERFACE_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		ADRActor * CreateActorByData(UBuildingData *BuildingData,bool & isJump);
	UFUNCTION(BlueprintImplementableEvent)
		TArray<FDRPointLight> GetPointLight();
	UFUNCTION(BlueprintImplementableEvent)
		TArray<FDRSpotLight> GetSpotLight();
};


