// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExpansionsDrawingStructure.h"
#include "ExpansionDrawingFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMLAYOUT_API UExpansionDrawingFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, Category = "Practice", meta = (WorldContext = "WorldContextObject"))
	void CreateExpansionDrawingMesh(UObject * WorldContextObject,const TArray<AActor*>& FurActor, const TArray <FExpansionLine>& WallList,FString mx);
	
	
};
