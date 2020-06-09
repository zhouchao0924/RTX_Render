// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Resource.h"
#include "ModelCommons.h"
#include "ModelMaterial.h"
#include "SurfaceFile.generated.h"

#define  SURFACE_HEADERVER_3	3  //->4 ImageData
#define  SURFACE_HEADERVER_4	4  //->5 ImageSize
#define  SURFACE_HEADERVER_5	5  //->6 DisplayName;
#define  SURFACE_HEADERVER		6

#define  SURFACE_BODY_VER_1		1	// -> 2  SurfaceObject  { OverrideVec2Parameters  OverrideVec3Parameters }
#define  SURFACE_BODY_VER_2		2	// -> 3  SurfaceObject  Name
#define  SURFACE_BODY_VER		3

UCLASS(BlueprintType)
class USurfaceFile : public UResource
{
	GENERATED_UCLASS_BODY()

	struct FHeader : public FResourceSummary
	{
		FString DisplayName;
		TArray<FChunk>	Textures;
		int32 GetHeadVer() override { return SURFACE_HEADERVER; }
		int32 GetCodeVer() override { return SURFACE_BODY_VER; }
		void Serialize(FArchive &Ar) override;
	};

public:
	//UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	//UMaterialInterface *GetUE4Material();
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	UMaterialInterface *GetUE4Material();
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	int32 GetNumerOfTexureSlot();
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	UTexture2D *GetTexureSlotInfo(int32 iTexSlot, FString &TexSlotName, FString &TexName);
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	UTexture2D *SetMaterialSlotTexture(int32 TextureIndex, const FString &NewTexFile);
	
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	UTexture2D *GetSlotNTexture(int32 TextureIndex, const FString &NewTexFile);

	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	int32 GetNumberOfScalarValues();
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	float GetScalarValue(int32 iSlot, FString &Name);
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	void SetScalarValue(int32 iSlot, float Value);

	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	int32 GetNumberOfVectorValues();
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	FLinearColor GetVectorValue(int32 iSlot, FString &Name);
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	void SetVectorValue(int32 iSlot, const FLinearColor &Value);

	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	UTexture *GetMaterialPreviewImage(int32 SizeX, int32 SizeY);
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	UMaterialInstanceDynamic *GetDynamicMaterialInstance();

	UFUNCTION(BlueprintCallable, Category = "DR|Model")
	void SetMaterialName(const FString &Name);

	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	FString GetMaterialName();

	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	FString GetSurfaceChineseName( );

	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	void SetSurfaceChineseName( const FString&InSurfaceName);

	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
		void SetDisplayName(const FString&Name);

	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
		FString GetDisplayName();
	
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	UMaterialInstanceDynamic* ResetMaterial();

	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
		void UnloadResource();
	UFUNCTION(BlueprintCallable, Category = "DR|Surface")
	EMaterialType GetSurfaceType();

	virtual void BeginDestroy() override;
	void Serialize(FArchive &Ar) override;
	void SerializeHeader(FArchive &Ar) override;
	FResourceSummary *GetSummary() override { return &m_Header; }

	virtual EResType GetResType() { return EResSurface; }
	FModelMaterial		   *GetMaterial();
	FModelMaterialUE4	   *GetMaterialUE4();
	
	void SetType(EMaterialType InType);
	void SetSurface(FModelMaterial *Material, TArray<FModelTexture *> &Textures);
	void UpdateParameters(UMaterialInstanceDynamic *Mtrl);
	void CopyOverrideParameters(FModelMaterial *Material);
	void CopyOverrideTextures(FModelMaterial *Material, TArray<FModelTexture *> &Textures);
	UTexture *GetTexture(int32 iSlot);
	FModelTexture *GetTextureByName(const FName &Name);
protected:
	virtual void Unload() override;
	virtual void CheckResource() override;
	virtual void BuildPreviewImage() override;
	void SerizlizeHistory(FArchive &Ar, uint32 HistoryFileVer);
	void Clean();
public:
	FHeader						m_Header;
	uint32						FileVer;
	bool						bCompressed;
	FModelMaterialUE4			*m_MaterialInfo;
	TArray<FModelTexture*>		m_Textures;
	
};

