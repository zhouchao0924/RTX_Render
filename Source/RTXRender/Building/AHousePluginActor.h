#pragma once

#include "DRActor.h"
#include "ModelFileComponent.h"
#include "AHousePluginActor.generated.h"

UCLASS(BlueprintType)
class AHousePluginActor :public ADRActor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Update(UBuildingData *Data);
	EVisibleChannel GetChannel() override { return  bTopCeil ? EVisibleChannel::EBuildingTopChannel : EVisibleChannel::EBuildingStructChannel; }
	void InitSurface();
	void ChangePillarSurf();
	void SetPluginTopMaterial(bool b);
public:
	UPROPERTY(Transient)
	UBuildingData *BuildingData;
	UPROPERTY(Transient)
	uint8		  bTopCeil : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent *HouseRootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBuildingGroupMeshComponent *GroupComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UModelFileComponent *ModelFileComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 HousePluginID;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 modelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomClassID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CraftID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterial* NewWallMaterial;
	UPROPERTY(EditAnywhere, Category = "Wall")
		UMaterialInterface	*WallMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class AComponentManagerActor* ComponentManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Scale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Angle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D DefaultScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsPillarPlugin;
};

