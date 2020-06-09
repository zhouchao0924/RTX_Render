#pragma once

#include "OrderFurnitureStruct.generated.h"

// order furniture struct
USTRUCT(Blueprintable)
struct FOrderFurnitureStructCPP
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 FaceIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString OrderFurnitureDatas;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString OrderFurniturePath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString OrderFurniturePakPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface *Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ComponentName;
};