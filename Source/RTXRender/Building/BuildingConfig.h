#pragma once

#include "CoreMinimal.h"
#include "BuildingConfig.generated.h"

USTRUCT(BlueprintType)
struct  FBuildingConfig
{
	GENERATED_BODY()

	FBuildingConfig();

	UPROPERTY(EditAnywhere, Category = "Wall")
	UMaterialInterface	*WallMaterial;

	UPROPERTY(EditAnywhere, Category = "Wall")
	int32 WallMaterialType;

	UPROPERTY(EditAnywhere, Category = "Floor")
	UMaterialInterface	* FloorMaterial;

	UPROPERTY(EditAnywhere, Category = "Floor")
	int32 FloorMaterialType;

	UPROPERTY(EditAnywhere, Category = "Ceil")
	UMaterialInterface	*CeilMaterial;

	UPROPERTY(EditAnywhere, Category = "Ceil")
	int32 CeilMaterialType;

	UPROPERTY(EditAnywhere, Category = "Ceil")
	bool bCeilVisible;

	UPROPERTY(EditAnywhere, Category = "Ceil")
	float Tolerance;

	UPROPERTY(EditAnywhere, Category = "Ceil")
	FString SkirtingCeil;

	UPROPERTY(EditAnywhere, Category = "Ceil")
	FVector SkirtingCeilExt;

	UPROPERTY(EditAnywhere, Category = "Floor")
	FString SkirtingFloor;

	UPROPERTY(EditAnywhere, Category = "Floor")
	FVector SkirtingFloorExt;

	UPROPERTY(EditAnywhere, Category = "Door")
	FString DefaultDoor;

	UPROPERTY(EditAnywhere, Category = "Door")
	FString DefaultDoorFrame;
	
	UPROPERTY(EditAnywhere, Category = "Window")
	FString DefaultWindow;

};


