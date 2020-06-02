// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AutoLayoutFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UAutoLayoutFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable,Category = "FunctionLibrary")
		static FDateTime GetFileCreatedTime(const FString filePath);

	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "fileList"), Category = "FunctionLibrary")
		static bool DeleteOldestFile(UPARAM(Ref)TArray<FString> &fileList, FString &deletedFile, FString AppendPath = "");

};
