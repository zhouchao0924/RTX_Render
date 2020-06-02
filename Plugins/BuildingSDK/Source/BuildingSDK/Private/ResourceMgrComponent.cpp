
#include "ResourceMgrComponent.h"
#include "BuildingSDKSystem.h"

UResourceMgrComponent::UResourceMgrComponent(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
{
}

int32 UResourceMgrComponent::GetResourceByID(const FString &ResID, bool bLoad)
{
	IBuildingResourceMgr *Mgr = GetSDKResourceMgr();
	if (Mgr)
	{
		IObject *pResource = GetResource(ResID, bLoad);
		return pResource ? pResource->GetID() : INVALID_OBJID;
	}
	return INVALID_OBJID;
}

UResourceMgrComponent *UResourceMgrComponent::GetSDKResMgr()
{
	ABuildingSDKSystem *SDK = ABuildingSDKSystem::GetSDKActor();
	if (SDK)
	{
		return SDK->ResourceMgr;
	}
	return nullptr;
}

int32 UResourceMgrComponent::GetResourceByFilename(const FString &Filename)
{
	return INVALID_OBJID;
}

int32 UResourceMgrComponent::ImportResource(const FString &Filename)
{
	return INVALID_OBJID;
}

int32 UResourceMgrComponent::ExtractSurface(int32 ModelID, int32 SurfaceIndex, const FString &SurfaceName)
{
	return INVALID_OBJID;
}

void UResourceMgrComponent::Clear()
{
}

void UResourceMgrComponent::Delete(int32 ResID)
{
}

IObject *UResourceMgrComponent::GetResource(const FString &ResID, bool bDownloadIfNotExsit)
{
	IBuildingResourceMgr *Mgr = GetSDKResourceMgr();
	if (Mgr)
	{
		std::string AnsiResID = TCHAR_TO_ANSI(*ResID);
		FString Filename = AnsiResID.c_str();
		return Mgr->GetResource(AnsiResID.c_str(), true, nullptr);
	}
	return nullptr;
}

IObject *UResourceMgrComponent::GetResource(int32 ID)
{
	IBuildingResourceMgr *Mgr = GetSDKResourceMgr();
	if (Mgr)
	{
		return Mgr->GetResource(ID);
	}
	return nullptr;
}

int32 UResourceMgrComponent::CreateCompoundResource()
{
	IObject *compundFile = nullptr;
	IBuildingResourceMgr *Mgr = GetSDKResourceMgr();
	if (Mgr)
	{
		compundFile = Mgr->CreateResource(ERCompoundFile, "");
	}
	return compundFile? compundFile->GetID() : INVALID_OBJID;
}

int32 UResourceMgrComponent::CreateTriangulatedPlaneMesh(const TArray<FVector2D> &Polygon, float Thickness)
{
	IBuildingSDK *SDK = GetBuildingSDK();
	ITriangulatedMesh *pMeshEditor = SDK ? SDK->GetTriMeshEditor() : nullptr;
	if (pMeshEditor)
	{
		kArray<kPoint> kPolygon;
		kPolygon.set((kPoint *)Polygon.GetData(), Polygon.Num());
		return pMeshEditor->CreatePlane(kPolygon, Thickness, EAlign_PlaneZLow);
	}
	return INVALID_OBJID;
}

UTexture2D *UResourceMgrComponent::GetTextureResource(int32 TexID)
{
	UTexture2D *Texture = nullptr;
	UObject **ppTex = RemapUE4Resource.Find(TexID);

	if (ppTex)
	{
		Texture = Cast<UTexture2D>(*ppTex);
	}
	else
	{
		IBuildingResourceMgr *Mgr = GetSDKResourceMgr();
		if (Mgr)
		{
			IObject *Obj =  Mgr->GetResource(TexID);
			ISurfaceTexture *TexSurf = Obj->GetInterface<ISurfaceTexture>("SurfaceTexture");
			if (TexSurf)
			{
				Texture = ConvertSurfaceTextureToTexture2D(TexSurf);
				RemapUE4Resource.Add(Obj->GetID(), Texture);
			}
		}
	}

	return Texture;
}

UTexture2D *UResourceMgrComponent::GetTexture(ISurfaceTexture *TexSurf)
{
	UTexture2D *Texture = nullptr;
	IObject *pTexObj = TexSurf? TexSurf->GetObject() : nullptr;
	if (pTexObj)
	{
		Texture = GetTextureResource(pTexObj->GetID());
	}
	return Texture;
}

UMaterialInstanceDynamic *UResourceMgrComponent::GetMaterialResource(IObject *Obj, ISurfaceObject *Surface)
{
	UMaterialInstanceDynamic *DynamicMtrl = nullptr;
	IObject *ResoruceObj = Surface->GetObject();
	if (ResoruceObj)
	{
		int32 ResID = ResoruceObj->GetID();
		UObject **pSurfaceMtrl = RemapUE4Resource.Find(ResID);
		if (pSurfaceMtrl)
		{
			DynamicMtrl = Cast<UMaterialInstanceDynamic>(*pSurfaceMtrl);
		}
		else
		{
			DynamicMtrl = ConvertSurfaceToMaterial(Surface);
			RemapUE4Resource.Add(ResID, DynamicMtrl);
		}
	}
	else
	{
		DynamicMtrl = ConvertSurfaceToMaterial(Surface);
	}
	return DynamicMtrl;
}

UMaterialInstanceDynamic *UResourceMgrComponent::ConvertSurfaceToMaterial(ISurfaceObject *Surface)
{
	int SurfaceType = Surface->GetSurfaceType();
	const char *Uri = Surface->GetUri();

	UMaterialInstanceDynamic *DynamicMtrl = CreateMaterialBySurface(Surface);
	if (DynamicMtrl)
	{
		int n = 0;
		CharPtr ParamName;

		n = Surface->GetScalarCount();
		for (int i = 0; i < n; ++i)
		{
			float Value = Surface->GetScalar(i, ParamName);
			DynamicMtrl->SetScalarParameterValue(FName(ParamName.w_str()), Value);
		}

		n = Surface->GetVectorCount();
		for (int i = 0; i < n; ++i)
		{
			kVector4D Value = Surface->GetVector(i, ParamName);
			DynamicMtrl->SetVectorParameterValue(FName(ParamName.w_str()), FLinearColor(Value.X, Value.Y, Value.Z, Value.W));
		}

		int iDiffuse = Surface->GetTextureIndex(eDiffuse);
		if (iDiffuse != -1)
		{
			ISurfaceTexture *TexSurf = Surface->GetTexture(iDiffuse);
			UTexture2D *texDiffuse = TexSurf? GetTexture(TexSurf) : nullptr;
			if (texDiffuse)
			{
				DynamicMtrl->SetTextureParameterValue(TEXT("Diffuse"), texDiffuse);
			}
		}

		int iNormal = Surface->GetTextureIndex(eNormal);
		if (iNormal != -1)
		{
			ISurfaceTexture *TexSurf = Surface->GetTexture(iDiffuse);
			UTexture2D *texNormal = TexSurf? GetTexture(TexSurf) : nullptr;
			if (texNormal)
			{
				DynamicMtrl->SetTextureParameterValue(TEXT("Normal"), texNormal);
			}
		}
	}

	return DynamicMtrl;
}

UMaterialInstanceDynamic *UResourceMgrComponent::CreateMaterialBySurface(ISurfaceObject *Surface)
{
	UMaterialInstanceDynamic *DynamicMtrl = nullptr;

	if (Surface)
	{
		int SurfaceType = Surface->GetSurfaceType();
		std::string Uri = Surface->GetUri();
		if (Uri.length() > 0)
		{
			if (SurfaceType == ERefMaterial)
			{
				IBuildingResourceMgr *ResMgr = GetSDKResourceMgr();
				IObject *MaterialObj = nullptr;
				MaterialObj = ResMgr->GetResource(Uri.c_str(), false, false);
				if (MaterialObj)
				{
					ISurfaceObject *RefSurface = MaterialObj->GetSurfaceObject(0);
					DynamicMtrl = CreateMaterialBySurface(RefSurface);
				}
			}
			else if (SurfaceType == EUE4Material)
			{
				FString UE4MaterialPath = ANSI_TO_TCHAR(Uri.c_str());
				UMaterialInterface *Material = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL, *UE4MaterialPath));
				DynamicMtrl = UMaterialInstanceDynamic::Create(Material, this);
			}
		}
	}

	return DynamicMtrl;
}

UTexture2D *UResourceMgrComponent::ConvertSurfaceTextureToTexture2D(ISurfaceTexture *TexSurf)
{
	UTexture2D *Texture = nullptr;
	if (TexSurf)
	{
		int32 MipCount = TexSurf->GetMipCount();

		EPFormat Format;
		int32 Width = 0, Height = 0;
		kArray<unsigned char> ImageData;
		if (TexSurf->GetMipInfo(0, Width, Height, ImageData, Format))
		{
			EPixelFormat PixelFormat = (EPixelFormat)Format;
			Texture = UTexture2D::CreateTransient(Width, Height, PixelFormat);

			FTexture2DMipMap &FirstMip = Texture->PlatformData->Mips[0];
			FirstMip.SizeX = Width;
			FirstMip.SizeY = Height;
			Texture->NeverStream = 0;
			Texture->SRGB = 1;
			Texture->PlatformData->PackedData = 1;
			Texture->PlatformData->PixelFormat = PixelFormat;

			if (PixelFormat == PF_BC5)
			{
				Texture->CompressionSettings = TextureCompressionSettings::TC_Normalmap;
				Texture->LODGroup = TextureGroup::TEXTUREGROUP_WorldNormalMap;
				Texture->SRGB = 0;
			}

			void* NewMipData = FirstMip.BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(NewMipData, ImageData.data(), ImageData.size());
			FirstMip.BulkData.Unlock();

			for (int32 i = 1; i < MipCount; ++i)
			{
				if (TexSurf->GetMipInfo(i, Width, Height, ImageData, Format))
				{
					FTexture2DMipMap SubMip;
					SubMip.SizeX = Width;
					SubMip.SizeY = Height;
					SubMip.BulkData.Lock(LOCK_READ_WRITE);
					void *NextNewMipData = SubMip.BulkData.Realloc(ImageData.size());
					FMemory::Memcpy(NextNewMipData, ImageData.data(), ImageData.size());
					SubMip.BulkData.Unlock();

					Texture->PlatformData->Mips.Add(&SubMip);
				}
			}

			Texture->UpdateResource();
		}
	}
	return Texture;
}

void UResourceMgrComponent::ReleaseAll()
{
	for (TMap<int32, UObject *>::TIterator It(RemapUE4Resource); It; ++It)
	{
		UObject *Obj = It.Value();
		if (Obj)
		{
			Obj->ConditionalBeginDestroy();
		}
	}

	RemapUE4Resource.Empty();

	IBuildingResourceMgr *ResMgr = GetSDKResourceMgr();
	if (ResMgr)
	{
		ResMgr->CleanCache();
	}
}


