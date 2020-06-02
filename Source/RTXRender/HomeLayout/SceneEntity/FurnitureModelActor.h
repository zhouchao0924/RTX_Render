#pragma once

#include "Struct.h"
#include "GameFramework/Actor.h"
#include "Building/DRActor.h"
#include "Building/BuildingData.h"
#include "Building/DRStruct.h"
#include "Model/ModelFileComponent.h"
//#include "./Wall_Boolean_Base.h"
#include "FurnitureModelActor.generated.h"

class UBooleanBase;
UCLASS(BlueprintType)
class AJDR_API AFurnitureModelActor : public ADRActor
{
	GENERATED_BODY()
private:
	bool isInit;
	bool isLoading;
public:
	// Sets default values for this actor's properties
	AFurnitureModelActor();

	UFUNCTION(BlueprintNativeEvent)
	TArray<FMatOfMX> GetDependMats();

	UFUNCTION(BlueprintNativeEvent)
	TArray<FMatOfMX> GetMeshMats(UModelFileComponent* MeshCom);

	UFUNCTION(BlueprintCallable, Category = "SaveData")
		void UpdateHoleData();
	UFUNCTION(BlueprintCallable, Category = "SaveData")
		void UpdateModelData(UModelFileComponent* MeshCom);
	UFUNCTION(BlueprintCallable, Category = "SaveData")
		void UpdateBuildResModel(int32 ModelID,FString ResID ,  UModelFileComponent* MeshCom);
	UFUNCTION(BlueprintCallable, Category = "SaveData")
		void UpdateBuildTranfrom(FTransform Location, UModelFileComponent* MeshCom);
	UFUNCTION(BlueprintCallable, Category = "SaveData")
		FVector SetModelFileComponetScale(UModelFileComponent* MeshCom,FVector Scale);

		void UpdateBuildingData(UBuildingData * Data);
	UFUNCTION(BlueprintImplementableEvent, Category = "SaveData")
		 void BPUpdateBuildingData(UBuildingData *Data);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SaveData")
		void GetModelCategory(int& CategoryID);
	UFUNCTION(BlueprintCallable, Category = "SaveData")
		FString NewUUID();
public:
	UPROPERTY(BlueprintReadWrite)
		UBuildingData* BuildingData;
	UPROPERTY(BlueprintReadWrite)
		class AWall_Boolean_Base * Hole;
	UPROPERTY(BlueprintReadOnly)
		FDRPointLight PointLightStruct;
	UPROPERTY(BlueprintReadOnly)
		FDRSpotLight SpotLightStruct;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
		int32 ObjectId;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;
	
};
