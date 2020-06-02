// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/DRStruct/DRBuildingStruct.h"
#include "DROperationBuildingFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UDROperationBuildingFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SpawnBuildingObject")
	static int32 AddPointLight(UObject* WorldContextObject, const FDRPointLightBase& InPointLightConfig);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SpawnBuildingObject")
	static int32 AddSpotLight(UObject* WorldContextObject, const FDRSpotLightBase& InSpotLightConfig);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SpawnBuildingObject")
	static int32 AddRectLight(UObject* WorldContextObject, const FDRRectLightBase& InRectLightConfig);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SpawnBuildingObject")
	static int32 AddNewModel(UObject* WorldContextObject, const FDNormalModel& InModelInstance);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "SpawnBuildingObject")
	static int32 AddWallByCorner(UObject* WorldContextObject, const FDRSolidWall& InSolidWall);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "SpawnBuildingObject")
	static int32 AddNewHole(UObject* WorldContextObject, const FDRHole& InHole);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "SpawnBuildingObject")
	static void AddNewDoor(UObject* WorldContextObject, const FDRDoorAndWindow& InDoorData, int32& DoorObjectId, int32& DoorFrameObjectId);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "SpawnBuildingObject")
	static void AddNewWindow(UObject* WorldContextObject, const FDRDoorAndWindow& InWindowData, int32& WindowObjectId);

	UFUNCTION(BlueprintCallable, Category = "HisMat")
	static void UpdateHisMatLists(UPARAM(ref)TArray<FDRMaterial> &InHisMatListRef, FDRMaterial InHisMat);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Calculate")
	static	FTransform CalculateSillTransform(UObject* WorldContextObject, const int32 InObjId, const FVector InModelBounds);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Calculate")
	static	FTransform CalculateSillTransformByBayWindow(UObject* WorldContextObject, const int32 InObjId, const FVector InModelBounds, const FVector InBayBounds);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "ConvertAdapterToStruct")
	static	bool ConvertModelAdapterToNormalModel(UObject* WorldContextObject, const int32 InObjId, FDNormalModel& OutNormalModelData);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "ConvertAdapterToStruct")
	static	bool ConvertPointAdapterToPointLightBase(UObject* WorldContextObject, const int32 InObjId, FDRPointLightBase& OutPointLightBase);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "ConvertAdapterToStruct")
	static	bool ConvertSpotAdapterToSpotLightBase(UObject* WorldContextObject, const int32 InObjId, FDRSpotLightBase& OutSpotLightBase);

	UFUNCTION(BlueprintPure)
	static TArray<FDRMaterial> ResetAreaMaterialIfNeed(const TArray<FDRMaterial>& AreaMaterials);

	UFUNCTION(BlueprintPure)
	static TArray<FDRMaterial> FillWallMaterialIfNeed(const TArray<FDRMaterial>& WallMaterials);
};
