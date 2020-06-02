// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PaveMethodFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class BUILDINGSDK_API UPaveMethodFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


	UFUNCTION(BlueprintCallable, Category = CraftPaving)
	static	bool PaveArea(class UProceduralMeshComponent* PaveComponent, class UCraftPavingComponent* PatternComponent, const TArray<FVector>& Area);
	
	UFUNCTION(BlueprintCallable, Category = CraftPaving)
		static bool GetResourceLocalVersion(const FString& FilePath, int32& LocalVersion);
};
