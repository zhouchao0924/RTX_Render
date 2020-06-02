// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ModelTexture.h"
#include "TextureImporter.generated.h"

class FTextureSourceInfo;

UCLASS(BlueprintType)
class UTextureImporter : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	FTextureSourceInfo *ImportFromFile(UObject *InParent, ETexSlot TexSlot, const FString &Filename);
	FTextureSourceInfo *ImportTexture(const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd);
	UFUNCTION(BlueprintCallable, Category="DR|Importer")
	static bool ExportTGA(int32 SizeX, int32 SizeY, const TArray<uint8> &Data, const FString &Filename, ETextureSourceFormat Format = TSF_BGRA8);
	UFUNCTION(BlueprintCallable, Category = "DR|Importer")
	static bool ExportTextureTGA(UTexture2D *TexImage, const FString &Filename);
	static bool ExportTGA(struct FModelTexture *Texture, const FString &Filename);
	static bool ExportTGA(int32 SizeX, int32 SizeY, uint8 *Data, bool bRGB16, const FString &Filename);
protected:
	bool IsImportResolutionValid(int32 Width, int32 Height, bool bAllowNonPowerOfTwo);
	UTexture2D* CreateTexture2D(UObject* InParent, FName Name, EObjectFlags Flags);
	UTextureCube* CreateTextureCube(UObject* InParent, FName Name, EObjectFlags Flags);
	void FillZeroAlphaPNGData(const FTextureSourceInfo& TextureSource, uint8* SourceData);
	bool DecompressTGA(const struct FTGAFileHeader* TGA, FTextureSourceInfo &TextureSourceInfo);
protected:
	TEnumAsByte<enum TextureMipGenSettings> MipGenSettings;
};

