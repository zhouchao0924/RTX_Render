// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRHoleAdapter.h"
#include "DRProjData.h"
#include "DRSolidWallAdapter.h"
#include "IBuildingSDK.h"
#include "Building/BuildingSystem.h"

UDRHoleAdapter::UDRHoleAdapter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

/********************************Get Function*************************************/
EModelType UDRHoleAdapter::GetHoleType()
{
	return static_cast<EModelType>(Obj->GetInt("HoleType"));
}

FVector UDRHoleAdapter::GetLocation()
{
	return ToVector(Obj->GetPropertyVector3D("Location"));
}

FVector UDRHoleAdapter::GetSize()
{
	return ToVector(Obj->GetPropertyVector3D("Size"));
}

FRotator UDRHoleAdapter::GetRotation()
{
	return ToRotator(Obj->GetPropertyRotator("Rotation"));
}

FTransform UDRHoleAdapter::GetTransform()
{
	return ToTransform(Obj->GetTransform());
}

int32 UDRHoleAdapter::GetDependWallId()
{
	return Obj->GetPropertyInt("WallID");
}

float UDRHoleAdapter::GetWallThickLeft()
{
	UDRSolidWallAdapter* Local_RSolidWallAdapter = Cast<UDRSolidWallAdapter>(ProjData->GetAdapter(GetDependWallId()));
	return Local_RSolidWallAdapter ? Local_RSolidWallAdapter->GetLeftThick() : 0.f;
}

float UDRHoleAdapter::GetWallThickRight()
{
	UDRSolidWallAdapter* Local_RSolidWallAdapter = Cast<UDRSolidWallAdapter>(ProjData->GetAdapter(GetDependWallId()));
	return Local_RSolidWallAdapter ? Local_RSolidWallAdapter->GetRightThick() : 0.f;
}

float UDRHoleAdapter::GetWidth()
{
	return Obj->GetPropertyFloat("Width");
}

float UDRHoleAdapter::GetThickness()
{
	return Obj->GetPropertyFloat("Thickness");
}

float UDRHoleAdapter::GetHeight()
{
	return Obj->GetPropertyFloat("Height");
}

float UDRHoleAdapter::GetGroundHeight()
{
	return Obj->GetPropertyFloat("ZPos");
}

bool UDRHoleAdapter::GetHorizontalFlip()
{
	return Obj->GetBool("HorizonalFlip");
}

bool UDRHoleAdapter::GetVerticalFlip()
{
	return Obj->GetBool("VerticalFlip");
}

FString UDRHoleAdapter::GetSelfTagName()
{
	return UTF8_TO_TCHAR(Obj->GetString("SelfTagName"));
}

FString UDRHoleAdapter::GetSlotActorTag()
{
	return UTF8_TO_TCHAR(Obj->GetString("SlotActorTag"));
}

TArray<FVector2D> UDRHoleAdapter::GetPoints()
{
	TArray<FVector2D> Local_Points;
	kArray<kPoint> Local_KPoints = Obj->GetPropertyVector2DArray("polygons");
	for (int32 Index = 0; Index < Local_KPoints.size(); ++Index)
	{
		Local_Points.Add(ToVector2D(Local_KPoints[Index]));
	}
	return Local_Points;
}

TArray<FDRMaterial> UDRHoleAdapter::GetHoleMaterials()
{
	//TArray<FDRMaterial> Local_HoleMaterials;
	//for(int32 SectionId = 0; SectionId <= 3; SectionId++)
	//{
	//	FDRMaterial Local_HoleMaterial;
	//	UDRMaterialAdapter *MaterialAdapter = GetMaterial(Obj->GetID(), SectionId);
	//	Local_HoleMaterial.ModelID = MaterialAdapter->GetModelID();
	//	Local_HoleMaterial.CraftID = MaterialAdapter->GetCraftID();
	//	Local_HoleMaterial.RoomClassID = MaterialAdapter->GetRoomClassID();
	//	Local_HoleMaterials.Add(Local_HoleMaterial);
	//}
	//return Local_HoleMaterials;

	TArray<FDRMaterial> Local_HoleMaterials;
	IValue* Local_HoleMatValue = Obj->FindValue("HoleMaterials");
	if (Local_HoleMatValue && Local_HoleMatValue->GetArrayCount() > 0)
	{
		FDRMaterial Local_HoleMaterial;
		for (int32 Index = 0; Index < Local_HoleMatValue->GetArrayCount(); ++Index)
		{
			IValue & Local_CurrentAreaValue = Local_HoleMatValue->GetField(Index);
			Local_HoleMaterial.ModelID = Local_CurrentAreaValue.GetField("ModelID").IntValue();
			Local_HoleMaterial.RoomClassID = Local_CurrentAreaValue.GetField("RoomClassID").IntValue();
			Local_HoleMaterial.CraftID = Local_CurrentAreaValue.GetField("CraftID").IntValue();
			Local_HoleMaterials.Add(Local_HoleMaterial);
		}
	}
	return Local_HoleMaterials;
}

TArray<FDRUV> UDRHoleAdapter::GetHoleUVs()
{
	//TArray<FDRUV> Local_HoleUVs;
	//for(int32 SectionId = 0; SectionId <= 3; SectionId++)
	//{
	//	FDRUV Local_HoleUV;
	//	UDRMaterialAdapter *MaterialAdapter = GetMaterial(Obj->GetID(), SectionId);
	//	Local_HoleUV.UVScale = MaterialAdapter->GetUVScale();
	//	Local_HoleUV.UVAngle = MaterialAdapter->GetUVAngle();
	//	Local_HoleUV.UVOffset = MaterialAdapter->GetUVOffset();
	//	Local_HoleUVs.Add(Local_HoleUV);
	//}
	//return Local_HoleUVs;

	TArray<FDRUV> Local_HoleUVs;
	IValue* Local_HoleUVValue = Obj->FindValue("HoleUVs");
	if (Local_HoleUVValue && Local_HoleUVValue->GetArrayCount() > 0)
	{
		FDRUV Local_HoleUV;
		for (int32 Index = 0; Index < Local_HoleUVValue->GetArrayCount(); ++Index)
		{
			IValue & Local_CurrentAreaValue = Local_HoleUVValue->GetField(Index);
			Local_HoleUV.UVScale = ToVector2D(Local_CurrentAreaValue.GetField("UVScale").Vec2Value());
			Local_HoleUV.UVOffset = ToVector2D(Local_CurrentAreaValue.GetField("UVOffset").Vec2Value());
			Local_HoleUV.UVAngle = Local_CurrentAreaValue.GetField("UVAngle").FloatValue();
			Local_HoleUVs.Add(Local_HoleUV);
		}
	}
	return Local_HoleUVs;
}

UDRMaterialAdapter *UDRHoleAdapter::GetMaterial(int32 ObjID, int32 SectionIndex)
{
	if (ObjID != INVALID_OBJID)
	{
		return ProjData->GetMaterialAdapter(ObjID, SectionIndex);
	}
	else
	{
		ISuite *Suite = Obj->GetSuite();
		if (Suite)
		{
			ObjectID WallID = Obj->GetPropertyInt("OwnerID");
			IObject *pWallObj = Suite->GetObject(WallID);
			if (pWallObj)
			{
				int SectionBaseIndex = Obj->GetPropertyInt("SectionBaseIndex");
				int HoleSectionIndex = SectionBaseIndex + SectionIndex;
				return ProjData->GetMaterialAdapter(pWallObj->GetID(), HoleSectionIndex);
			}
		}
	}
	return nullptr;
}

EHolestatusType UDRHoleAdapter::GetHoleStatusType()
{
	return static_cast<EHolestatusType>(Obj->GetInt("HoleStatusType"));
}

/********************************Set Function*************************************/

void UDRHoleAdapter::SetHoleType(const EModelType InHoleType)
{
	Obj->SetInt("HoleType", static_cast<int32>(InHoleType));
}

void UDRHoleAdapter::SetGroundHeight(const float InGroundHeight)
{
	Obj->SetPropertyFloat("ZPos", InGroundHeight);
}

void UDRHoleAdapter::SetHeight(const float InHeight)
{
	Obj->SetPropertyFloat("Height", InHeight);
}

void UDRHoleAdapter::SetWidth(const float InWidth)
{
	Obj->SetPropertyFloat("Width", InWidth);
}

void UDRHoleAdapter::SetHorizontalFlip(const bool bInFlip)
{
	Obj->SetBool("HorizonalFlip", bInFlip);
}

void UDRHoleAdapter::SetVerticalFlip(const bool bInFlip)
{
	Obj->SetBool("VerticalFlip", bInFlip);
}

void UDRHoleAdapter::SetSelfTagName(const FString& InSelfTagName)
{
	Obj->SetString("SelfTagName", TCHAR_TO_UTF8(*InSelfTagName));
}

void UDRHoleAdapter::SetSlotActorTag(const FString& InSlotActorTag)
{
	Obj->SetString("SlotActorTag", TCHAR_TO_UTF8(*InSlotActorTag));
}

void UDRHoleAdapter::SetHoleMaterials(const TArray<FDRMaterial>& InHoleMaterials)
{
	if (InHoleMaterials.Num() != 4)
		return;
	//for(int32 SectionId = 0; SectionId <= 3; SectionId++)
	//{
	//	FDRMaterial Local_HoleMaterial = InHoleMaterials[SectionId];
	//	UDRMaterialAdapter *MaterialAdapter = GetMaterial(Obj->GetID(), SectionId);
	//	MaterialAdapter->SetModelID(Local_HoleMaterial.ModelID);
	//	MaterialAdapter->SetCraftID(Local_HoleMaterial.CraftID);
	//	MaterialAdapter->SetRoomClassID(Local_HoleMaterial.RoomClassID);
	//}
	IBuildingSDK *Local_SDK = UBuildingSystem::GetBuildingSDK();
	IValueFactory *Local_VF = Local_SDK->GetValueFactory();
	IValue& Local_HoleMaterialValues = Local_VF->Create();
	for(const FDRMaterial& CurrentHoleMaterial : InHoleMaterials)
	{
		IValue& Local_CurrentHoleValue = Local_VF->Create();
		Local_CurrentHoleValue.AddField("ModelID", Local_VF->Create(CurrentHoleMaterial.ModelID));
		Local_CurrentHoleValue.AddField("RoomClassID", Local_VF->Create(CurrentHoleMaterial.RoomClassID));
		Local_CurrentHoleValue.AddField("CraftID", Local_VF->Create(CurrentHoleMaterial.CraftID));
		Local_HoleMaterialValues.AddField(Local_CurrentHoleValue);
	}
	Obj->SetValue("HoleMaterials", &Local_HoleMaterialValues);
}

void UDRHoleAdapter::SetHoleUVs(const TArray<FDRUV>& InHoleUVs)
{
	if (InHoleUVs.Num() != 4)
		return;
	//for(int32 SectionId = 0; SectionId <= 3; SectionId++)
	//{
	//	FDRUV Local_HoleUV = InHoleUVs[SectionId];
	//	UDRMaterialAdapter *MaterialAdapter = GetMaterial(Obj->GetID(), SectionId);
	//	MaterialAdapter->SetUVAngle(Local_HoleUV.UVAngle);
	//	MaterialAdapter->SetUVOffset(Local_HoleUV.UVOffset);
	//	MaterialAdapter->SetUVScale(Local_HoleUV.UVScale);
	//}
	IBuildingSDK *Local_SDK = UBuildingSystem::GetBuildingSDK();
	IValueFactory *Local_VF = Local_SDK->GetValueFactory();
	IValue& LocalHoleUVValues = Local_VF->Create();
	for(const FDRUV& CurrentHoleUV : InHoleUVs)
	{
		IValue& Local_CurrentHoleUVValue = Local_VF->Create();
		kPoint Local_Point = ToKVector2D(CurrentHoleUV.UVScale);
		Local_CurrentHoleUVValue.AddField("UVScale", Local_VF->Create(&Local_Point, false));
		Local_Point = ToKVector2D(CurrentHoleUV.UVOffset);
		Local_CurrentHoleUVValue.AddField("UVOffset", Local_VF->Create(&Local_Point, false));
		Local_CurrentHoleUVValue.AddField("UVAngle", Local_VF->Create(CurrentHoleUV.UVAngle));
		LocalHoleUVValues.AddField(Local_CurrentHoleUVValue);
	}
	Obj->SetValue("HoleUVs", &LocalHoleUVValues);
}

void UDRHoleAdapter::SetHoleStatusType(const EHolestatusType HoleStatusType)
{
	Obj->SetInt("HoleStatusType", static_cast<int32>(HoleStatusType));
}