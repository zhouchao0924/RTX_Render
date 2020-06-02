// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AataStructure/ExpansionsDrawingStructure.h"
#include "Public/HouseCustomActor.h"
#include "CustomLayoutFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UCustomLayoutFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Practice", meta = (WorldContext = "WorldContextObject"))
		static TArray<FExpansionTest> CreateExpansionDrawingMesh(UObject * WorldContextObject, const TArray<AHouseCustomActor*>& FurActor, const TArray <FExpansionLine>& WallList, FString mx, TArray<AExpansionDrawingActor*>&TopBoard, AFunctionManage* manage);
	//台面0 后挡水1 前挡水2 踢脚板3
	UFUNCTION(BlueprintCallable, Category = "Practice", meta = (WorldContext = "WorldContextObject"))
		static TArray<FExpansionTest> CreateExpansionDrawingMesh1(UObject * WorldContextObject, const TArray<AHouseCustomActor*>& FurActor, const TArray <FExpansionCabnetPillars>& Fluelist, const TArray <FExpansionLine>& WallList,
			const TArray<FString>& mx, TArray<ADynamicTableboard*>& Tableboardlist, TArray<AExpansionDrawingActor*>& BreakWater, TArray<AExpansionDrawingActor*>& SkirtBoard, AFunctionManage* manage);

	UFUNCTION(BlueprintCallable, Category = "Practice", meta = (WorldContext = "WorldContextObject"))
		static void LoadloftData(UObject * WorldContextObject, const TMap<int32, FString>& mx, const FCustomLoftdata& data, AFunctionManage* manage, TArray<ADynamicTableboard*>& Tableboardlist, TArray<AExpansionDrawingActor*>& BreakWater, TArray<AExpansionDrawingActor*>& SkirtBoard, TArray<AExpansionDrawingActor*>&TopBoard, TMap<int32, AHouseCustomActor*> FurActor);



	static TArray<TArray<FVector>> GetForwardPoints(UObject * WorldContextObject, const TArray<AHouseCustomActor*>& FurActor, const TArray <FExpansionLine>& WallList, FString mx, AFunctionManage* manage);
};