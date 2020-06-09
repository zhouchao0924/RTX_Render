

#include "DataAdapter.h"
#include "DRProjData.h"

UDataAdapter::UDataAdapter(const FObjectInitializer &ObjectIntializer)
	:Super(ObjectIntializer)
{
}

UDRMaterialAdapter *UDataAdapter::GetMaterial(int32 ObjID, int32 SectionIndex)
{
	if (ObjID != INVALID_OBJID)
	{
		return ProjData->GetMaterialAdapter(ObjID, SectionIndex);
	}
	return ProjData->GetMaterial(Obj.Get(), SectionIndex);
}

int32 UDataAdapter::GetNumSection()
{
	IMeshObject *meshObj = Obj->GetMeshObject(0);
	return meshObj ? meshObj->GetSectionCount() : 0;
}

ISurfaceObject *UDataAdapter::GetSurface(int SectionIndex)
{
	IMeshObject *meshObj = Obj->GetMeshObject(0);
	if (meshObj)
	{
		return meshObj->GetSurfaceObject(SectionIndex);
	}
	return nullptr;
}

void UDataAdapter::GetValidMaterialSections(TArray<int32> &MaterialSections)
{
	MaterialSections.Empty();
	IMeshObject *meshObj = Obj->GetMeshObject(0);
	if (meshObj)
	{
		for (int i = 0; i < meshObj->GetSectionCount(); ++i)
		{
			ISurfaceObject *surfObj = meshObj->GetSurfaceObject(i);
			if (surfObj)
			{
				std::string path = surfObj->GetUri();
				if (path.length() > 0)
				{
					MaterialSections.Add(i);
				}
			}
		}
	}
}

void UDataAdapter::SetUVOffset(const FVector2D &UVOffset, int32 SectionIndex, int32 ObjID)
{
	int32 nSection = GetNumSection();
	if (SectionIndex < 0)
	{
		for (int32 i = 0; i < nSection; ++i)
		{
			SetUVOffset(UVOffset, i);
		}
	}
	else
	{
		if (SectionIndex < nSection)
		{
			UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
			Material->SetUVOffset(UVOffset);
		}
	}
}

void UDataAdapter::SetUVScale(const FVector2D &UVScale, int32 SectionIndex, int32 ObjID)
{
	int32 nSection = GetNumSection();
	if (SectionIndex < 0)
	{
		for (int32 i = 0; i < nSection; ++i)
		{
			SetUVScale(UVScale, i);
		}
	}
	else
	{
		if (SectionIndex < nSection)
		{
			UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
			Material->SetUVScale(UVScale);
		}
	}
}

void UDataAdapter::SetUVRot(float UVRot, int32 SectionIndex, int32 ObjID)
{
	int32 nSection = GetNumSection();
	if (SectionIndex < 0)
	{
		for (int32 i = 0; i < nSection; ++i)
		{
			SetUVRot(UVRot, i);
		}
	}
	else
	{
		if (SectionIndex < nSection)
		{
			UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
			Material->SetUVAngle(UVRot);
		}
	}
}

void UDataAdapter::SetCraftID(int32 CraftID, int32 SectionIndex, int32 ObjID)
{
	int32 nSection = GetNumSection();
	if (SectionIndex < 0)
	{
		for (int32 i = 0; i < nSection; ++i)
		{
			SetCraftID(CraftID, i);
		}
	}
	else
	{
		if (SectionIndex < nSection)
		{
			UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
			Material->SetCraftID(CraftID);
		}
	}
}

void UDataAdapter::SetRoomClassID(int32 RoomClassID, int32 SectionIndex, int32 ObjID)
{
	int32 nSection = GetNumSection();
	if (SectionIndex < 0)
	{
		for (int32 i = 0; i < nSection; ++i)
		{
			SetRoomClassID(RoomClassID, i);
		}
	}
	else
	{
		if (SectionIndex < nSection)
		{
			UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
			Material->SetRoomClassID(RoomClassID);
		}
	}
}

FVector2D UDataAdapter::GetUVOffset(int32 SectionIndex, int32 ObjID)
{
	UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
	return Material->GetUVOffset();
}

FVector2D UDataAdapter::GetUVScale(int32 SectionIndex, int32 ObjID)
{
	UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
	return Material->GetUVScale();
}

float UDataAdapter::GetUVRot(int32 SectionIndex, int32 ObjID)
{
	UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
	return Material->GetUVAngle();
}

int32 UDataAdapter::GetCraftID(int32 SectionIndex, int32 ObjID)
{
	UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
	return Material->GetCraftID();
}

int32 UDataAdapter::GetRoomClassID(int32 SectionIndex, int32 ObjID)
{
	UDRMaterialAdapter *Material = GetMaterial(ObjID, SectionIndex);
	return Material->GetRoomClassID();
}













