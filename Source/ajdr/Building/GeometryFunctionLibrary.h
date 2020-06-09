
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeometryFunctionLibrary.generated.h"

UCLASS(BlueprintType)
class UGeometryFunctionLibrary :public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static bool GetBoundary(const FString &MXFilename, TArray<FVector2D> &Boundary);
};


