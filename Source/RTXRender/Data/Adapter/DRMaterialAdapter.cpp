

#include "DRMaterialAdapter.h"
#include "IMeshObject.h"

UDRMaterialAdapter::UDRMaterialAdapter(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
	, SectionIndex(INDEX_NONE)
{
}

ISurfaceObject *UDRMaterialAdapter::GetSurface()
{
	if (Obj.IsValid())
	{
		IMeshObject *meshObj = Obj->GetMeshObject(0);
		if (meshObj)
		{
			return meshObj->GetSurfaceObject(SectionIndex);
		}
	}
	return nullptr;
}

void UDRMaterialAdapter::SetMaterial(const FString &ResID)
{
	if (Obj.IsValid())
	{
		std::string uri = TCHAR_TO_ANSI(*ResID);
		Obj->SetSurfaceUri(nullptr, 0, uri.c_str(), SectionIndex);
	}
}

FString UDRMaterialAdapter::GetMaterial()
{
	ISurfaceObject *Surf = GetSurface();
	if (Surf)
	{
		const char *uri = Surf->GetUri();
		FString ResID = ANSI_TO_TCHAR(uri);
		return ResID;
	}
	return TEXT("");
}

void UDRMaterialAdapter::SetModelID(const int32 &ModelID)
{
	IObject *surfObj = GetSurfObj();
	if (surfObj)
	{
		surfObj->SetInt("ModelID", ModelID);
	}
}

int32 UDRMaterialAdapter::GetModelID()
{
	IObject *surfObj = GetSurfObj();
	if (surfObj)
	{
		return surfObj->GetInt("ModelID");
	}
	return 0;
}

IObject *UDRMaterialAdapter::GetSurfObj()
{
	ISurfaceObject *Surf = GetSurface();
	if (Surf)
	{
		return Surf->GetObject();
	}
	return nullptr;
}

void UDRMaterialAdapter::SetRoomClassID(int32 RoomClassID)
{
	IObject *surfObj = GetSurfObj();
	if (surfObj)
	{
		surfObj->SetInt("RoomClassID", RoomClassID);
	}
}

int32 UDRMaterialAdapter::GetRoomClassID()
{
	IObject *surfObj = GetSurfObj();
	if (surfObj)
	{
		return surfObj->GetInt("RoomClassID");
	}
	return 0;
}

void UDRMaterialAdapter::SetCraftID(int32 CraftID)
{
	IObject *surfObj = GetSurfObj();
	if (surfObj)
	{
		surfObj->SetInt("CraftID", CraftID);
	}
}

int32 UDRMaterialAdapter::GetCraftID()
{
	IObject *surfObj = GetSurfObj();
	if (surfObj)
	{
		return surfObj->GetInt("CraftID");
	}
	return 0;
}

void UDRMaterialAdapter::SetUVOffset(const FVector2D &UVOffset)
{
	ISurfaceObject *Surf = GetSurface();
	if (Surf)
	{
		Surf->SetScalar(TEXT("位移X"), UVOffset.X);
		Surf->SetScalar(TEXT("位移Y"), UVOffset.Y);
	}
}

FVector2D UDRMaterialAdapter::GetUVOffset()
{
	ISurfaceObject *Surf = GetSurface();
	if (Surf)
	{
		float X = Surf->GetScalar(TEXT("位移X"));
		float Y = Surf->GetScalar(TEXT("位移Y"));
		return FVector2D(X, Y);
	}
	return FVector2D::ZeroVector;
}

void UDRMaterialAdapter::SetUVScale(const FVector2D &UVScale)
{
	ISurfaceObject *Surf = GetSurface();
	if (Surf)
	{
		Surf->SetScalar(TEXT("密度U"), UVScale.X);
		Surf->SetScalar(TEXT("密度V"), UVScale.Y);
		Surf->SetScalar(TEXT("法线U"), UVScale.X);
		Surf->SetScalar(TEXT("法线V"), UVScale.Y);
	}
}

FVector2D UDRMaterialAdapter::GetUVScale()
{
	ISurfaceObject *Surf = GetSurface();
	if (Surf)
	{
		float X = Surf->GetScalar(TEXT("密度U"));
		float Y = Surf->GetScalar(TEXT("密度V"));
		return FVector2D(X, Y);
	}
	return FVector2D(1.0f, 1.0f);
}

void UDRMaterialAdapter::SetUVAngle(float UVAngle)
{
	ISurfaceObject *Surf = GetSurface();
	if (Surf)
	{
		Surf->SetScalar(TEXT("贴图角度"), UVAngle);
		Surf->SetScalar(TEXT("法线角度"), UVAngle);
	}
}

float UDRMaterialAdapter::GetUVAngle()
{
	ISurfaceObject *Surf = GetSurface();
	if (Surf)
	{
		return Surf->GetScalar(TEXT("贴图角度"));
	}
	return 0;
}




