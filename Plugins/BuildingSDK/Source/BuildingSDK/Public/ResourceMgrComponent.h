// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "IBuildingSDK.h"
#include "BuildingSDKDefs.h"
#include "ResourceMgrComponent.generated.h"

UCLASS(BlueprintType)
class BUILDINGSDK_API UResourceMgrComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static UResourceMgrComponent *GetSDKResMgr();

	UFUNCTION(BlueprintCallable)
	int32 GetResourceByID(const FString &ResID, bool bLoad);

	UFUNCTION(BlueprintCallable)
	int32 CreateCompoundResource();

	UFUNCTION(BlueprintCallable)
	int32 CreateTriangulatedPlaneMesh(const TArray<FVector2D> &Polygon, float Thickness);

	UFUNCTION(BlueprintCallable)
	int32 GetResourceByFilename(const FString &Filename);

	UFUNCTION(BlueprintCallable)
	int32 ImportResource(const FString &Filename);

	UFUNCTION(BlueprintCallable)
	int32 ExtractSurface(int32 ModelID, int32 SurfaceIndex, const FString &SurfaceName);

	UFUNCTION(BlueprintCallable)
	void Clear();

	UFUNCTION(BlueprintCallable)
	void Delete(int32 ResID);

	UFUNCTION(BlueprintCallable)
	void ReleaseAll();
public:
	IObject *GetResource(int32 ID);
	IObject *GetResource(const FString &ResID, bool bDownloadIfNotExsit);
	UTexture2D *GetTexture(ISurfaceTexture *TexSurf);
	UTexture2D *GetTextureResource(int32 TexID);
	UMaterialInstanceDynamic *GetMaterialResource(IObject *Obj, ISurfaceObject *Surface);
protected:
	UMaterialInstanceDynamic *CreateMaterialBySurface(ISurfaceObject *Surface);
	UTexture2D *ConvertSurfaceTextureToTexture2D(ISurfaceTexture *TexSurf);
	UMaterialInstanceDynamic *ConvertSurfaceToMaterial(ISurfaceObject *Surface);
protected:
	UPROPERTY(Transient)
	TMap<int32, UObject *> RemapUE4Resource;
};


