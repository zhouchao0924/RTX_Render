// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRAreaAdapter.h"
#include "IBuildingSDK.h"
#include "Data/DRProjData.h"
#include "Adapter/DRMaterialAdapter.h"
#include "Building/BuildingSystem.h"

UDRAreaAdapter::UDRAreaAdapter(const FObjectInitializer & ObjectIntializer)
	:Super(ObjectIntializer)
{
}

/********************************Get Function*************************************/
TArray<FVector2D> UDRAreaAdapter::GetPointList()
{
	TArray<FVector2D> AreaPointList;
	kArray<kPoint> kPointlist;
	if (Obj->IsA(ERoom))
		kPointlist = Obj->GetPropertyVector2DArray("InnerPolygons");
	else if (Obj->IsA(EPolygonArea))
		kPointlist = Obj->GetPropertyVector2DArray("Boundary");
	for (int32 index = 0; index < kPointlist.size(); index++)
	{
		AreaPointList.Add(ToVector2D(kPointlist[index]));
	}
	return AreaPointList;
}

FString UDRAreaAdapter::GetTagName()
{
	return UTF8_TO_TCHAR(Obj->GetString("TagName"));
}

float UDRAreaAdapter::GetHeight()
{
	return Obj->GetInt("Height");
}

int32 UDRAreaAdapter::GetRoomID()
{
	return Obj->GetInt("RoomID");
}

FString UDRAreaAdapter::GetRoomName()
{
	return UTF8_TO_TCHAR(Obj->GetString("RoomName"));
}

int32 UDRAreaAdapter::GetRoomUsageId()
{
	return Obj->GetInt("RoomUsageId");
}

FString UDRAreaAdapter::GetRoomUsageName()
{
	return UTF8_TO_TCHAR(Obj->GetString("RoomUsageName"));
}

float UDRAreaAdapter::GetArea()
{

	TArray<FVector2D> Local_Points = GetPointList();
	int32 Local_Num = Local_Points.Num();
	float Local_tempValue(0.f);
	if (Local_Num < 3)
		return Local_tempValue;

	for (int32 Index = 0; Index < Local_Num; Index++)
	{
		Local_tempValue += FVector2D::CrossProduct(Local_Points[Index], Local_Points[(Index + 1) % Local_Num]);
	}
	return Local_tempValue / 2.f;

}

int32 UDRAreaAdapter::GetTypeId()
{
	return Obj->GetInt("TypeId");
}

int32 UDRAreaAdapter::GetRoomUseIndex()
{
	return Obj->GetInt("RoomUseIndex");
}

TArray<FDRMaterial> UDRAreaAdapter::GetAreaMaterials()
{
/*
	TArray<FDRMaterial> AreaMaterials;
	FDRMaterial Material;

	UDRProjData* Local_ProjectData = UDRProjData::GetProjectDataManager(this);
	UDRMaterialAdapter *Local_MaterialAdapter = Local_ProjectData->GetMaterialAdapter(GetFloorID(), 1);
	Material.ModelID = Local_MaterialAdapter->GetModelID();
	Material.RoomClassID = Local_MaterialAdapter->GetRoomClassID();
	Material.CraftID = Local_MaterialAdapter->GetCraftID();
	AreaMaterials.Add(Material);

	Local_MaterialAdapter = Local_ProjectData->GetMaterialAdapter(GetCeilID(), 0);
	Material.ModelID = Local_MaterialAdapter->GetModelID();
	Material.RoomClassID = Local_MaterialAdapter->GetRoomClassID();
	Material.CraftID = Local_MaterialAdapter->GetCraftID();
	AreaMaterials.Add(Material);
*/
	TArray<FDRMaterial> AreaMaterials;
	FDRMaterial Material;

	IValue* areaMaterialListValue = Obj->FindValue("AreaMaterials");
	if (areaMaterialListValue && areaMaterialListValue->GetArrayCount() > 0)
	{
		for (int32 Index = 0; Index < areaMaterialListValue->GetArrayCount(); ++Index)
		{
			IValue& areaMaterialValue = areaMaterialListValue->GetField(Index);
			Material.ModelID = areaMaterialValue.GetField("ModelID").IntValue();
			Material.RoomClassID = areaMaterialValue.GetField("RoomClassID").IntValue();
			Material.CraftID = areaMaterialValue.GetField("CraftID").IntValue();
			AreaMaterials.Add(Material);
		}
	}

	return AreaMaterials;
}

int32 UDRAreaAdapter::GetCeilID()
{
	return Obj->GetPropertyInt("CeilID");
}

int32 UDRAreaAdapter::GetFloorID()
{
	return Obj->GetPropertyInt("FloorID");
}

FVector2D UDRAreaAdapter::GetFloorUVScale()
{
	return ToVector2D(Obj->GetVector2D("UVScale"));
}

FVector2D UDRAreaAdapter::GetFloorUVOffset()
{
	return ToVector2D(Obj->GetVector2D("UVOffset"));
}

float UDRAreaAdapter::GetFloorUVRot()
{
	return Obj->GetFloat("UVRot");
}

/********************************Set Function*************************************/

void UDRAreaAdapter::SetTagName(const FString &TagName)
{
	Obj->SetString("TagName", TCHAR_TO_UTF8(*TagName));
}

void UDRAreaAdapter::SetHeight(const float &Height)
{
	Obj->SetFloat("Height", Height);
}

void UDRAreaAdapter::SetRoomID(const int32 &RoomID)
{
	Obj->SetInt("RoomID", RoomID);
}

void UDRAreaAdapter::SetRoomName(const FString &RoomName)
{
	Obj->SetString("RoomName", TCHAR_TO_UTF8(*RoomName));
}

void UDRAreaAdapter::SetRoomUsageId(const int32 &RoomUsageId)
{
	Obj->SetInt("RoomUsageId", RoomUsageId);
}

void UDRAreaAdapter::SetRoomUsageName(const FString &RoomUsageName)
{
	Obj->SetString("RoomUsageName", TCHAR_TO_UTF8(*RoomUsageName));
	//去掉空间标识后，使用空间用途名称设置RoomName
	Obj->SetString("RoomName", TCHAR_TO_UTF8(*RoomUsageName));
}

void UDRAreaAdapter::SetArea(const float &Area)
{
	//Obj->SetFloat("Area", Area);
}

void UDRAreaAdapter::SetTypeId(const int32 &TypeId)
{
	Obj->SetInt("TypeId", TypeId);
}

void UDRAreaAdapter::SetRoomUseIndex(const int32 &RoomUseIndex)
{
	Obj->SetInt("RoomUseIndex", RoomUseIndex);
}

void UDRAreaAdapter::SetAreaMaterials(const TArray<FDRMaterial> &AreaMaterials)
{
/*
	UDRProjData* Local_ProjectData = UDRProjData::GetProjectDataManager(this);
	UDRMaterialAdapter *Local_MaterialAdapter = Local_ProjectData->GetMaterialAdapter(GetFloorID(), 1);
	Local_MaterialAdapter->SetModelID(AreaMaterials[0].ModelID);
	Local_MaterialAdapter->SetRoomClassID(AreaMaterials[0].RoomClassID);
	Local_MaterialAdapter->SetCraftID(AreaMaterials[0].CraftID);

	Local_MaterialAdapter = Local_ProjectData->GetMaterialAdapter(GetCeilID(), 0);
	Local_MaterialAdapter->SetModelID(AreaMaterials[1].ModelID);
	Local_MaterialAdapter->SetRoomClassID(AreaMaterials[1].RoomClassID);
	Local_MaterialAdapter->SetCraftID(AreaMaterials[1].CraftID);
*/

	IBuildingSDK* buildingSDK = UBuildingSystem::GetBuildingSDK();
	if (buildingSDK == nullptr) {
		return;
	}

	IValueFactory* valueFactory = buildingSDK->GetValueFactory();
	if (valueFactory == nullptr) {
		return;
	}

	IValue& AreaMaterialValueList = valueFactory->Create();
	for (const FDRMaterial& AreaMaterial : AreaMaterials)
	{
		IValue& CurrentAreaValue = valueFactory->Create();
		CurrentAreaValue.AddField("ModelID", valueFactory->Create(AreaMaterial.ModelID));
		CurrentAreaValue.AddField("RoomClassID", valueFactory->Create(AreaMaterial.RoomClassID));
		CurrentAreaValue.AddField("CraftID", valueFactory->Create(AreaMaterial.CraftID));
		AreaMaterialValueList.AddField(CurrentAreaValue);
	}
	Obj->SetValue("AreaMaterials", &AreaMaterialValueList);
}

void UDRAreaAdapter::SetFloorUVScale(const FVector2D& InUVScale)
{
	Obj->SetVector2D("UVScale", ToKVector2D(InUVScale));
}

void UDRAreaAdapter::SetFloorUVOffset(const FVector2D& InUVOffset)
{
	Obj->SetVector2D("UVOffset", ToKVector2D(InUVOffset));
}

void UDRAreaAdapter::SetFloorUVRot(const float InUVRot)
{
	Obj->SetFloat("UVRot", InUVRot);
}
