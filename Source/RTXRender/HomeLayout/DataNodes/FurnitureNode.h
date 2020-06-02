// Fill out your copyright notice in the Description page of Project Settings.
// furniture data model for furniture model

#pragma once

#include "OrderFurnitureStruct.h"
#include "GeometryConfig.h"
#include "FurnitureOtherInfoNode.h"
#include "FurnitureNode.generated.h"

// model saved config
USTRUCT(Blueprintable)
struct FFurnitureNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> ActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UMaterialInterface*> Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString InFolder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bSimulatePhysics;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bVisible;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHiddenInGame;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasBeenDeleted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIgnoreDecal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FFurnitureOtherInfoNode Other;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bEditableGeom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGeometryConfigCPP GeometryConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh *StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ModelPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bThermalLoadType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Detailed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture *Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MaterialPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanConvertedToGeometry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsProjectMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ModePath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bProjectModeDefMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsProjectMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ProjectMatPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString TagName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SkuId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ModelCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString PakPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ModelDatas;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString DoorMaterialDatas;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString DoorMaterialPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString DoorMaterialPakPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FOrderFurnitureStructCPP> OrderFurniture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bReceiveShadow;
};

