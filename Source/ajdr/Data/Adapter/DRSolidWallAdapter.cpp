// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRSolidWallAdapter.h"
#include "ISuite.h"
#include "IBuildingSDK.h"
#include "Building/BuildingSystem.h"

UDRSolidWallAdapter::UDRSolidWallAdapter(const FObjectInitializer & ObjectIntializer)
	:Super(ObjectIntializer)
{
}

/********************************Get Function*************************************/
bool UDRSolidWallAdapter::GetRoomWall()
{
	return Obj->GetPropertyBool("bRoomWall");
}

ESolidWallType UDRSolidWallAdapter::GetSolidWallType()
{
	return static_cast<ESolidWallType>(Obj->GetInt("SolidWallType"));
}

bool UDRSolidWallAdapter::GetMainWall()
{
	return Obj->GetPropertyBool("bMainWall");
}

FDRWallPosition UDRSolidWallAdapter::GetWallPositionData()
{
	FDRWallPosition Local_DRWallPos;
	kLine2D Local_CenterLine;
	kLine2D Local_LeftLine;
	kLine2D Local_RightLine;
	if (Obj->GetSuite() && Obj->GetSuite()->GetWallBorderLines(Obj->GetID(), Local_CenterLine, Local_LeftLine, Local_RightLine))
	{
		Local_DRWallPos.StartPos = ToVector(Local_CenterLine.start);
		Local_DRWallPos.EndPos = ToVector(Local_CenterLine.end);
		Local_DRWallPos.LeftStartPos = ToVector(Local_LeftLine.start);
		Local_DRWallPos.LeftEndPos = ToVector(Local_LeftLine.end);
		Local_DRWallPos.RightStartPos = ToVector(Local_RightLine.start);
		Local_DRWallPos.RightEndPos = ToVector(Local_RightLine.end);
	}
	return Local_DRWallPos;
}

float UDRSolidWallAdapter::GetLeftThick()
{
	return Obj->GetPropertyFloat("ThickLeft");
}

float UDRSolidWallAdapter::GetRightThick()
{
	return Obj->GetPropertyFloat("ThickRight");
}

float UDRSolidWallAdapter::GetHeight()
{
	return Obj->GetFloat("Height");
}

//GetHeightByProperty此方法仅限ZTB使用
float UDRSolidWallAdapter::GetHeightByProperty()
{
	return Obj->GetPropertyFloat("Height") ? Obj->GetPropertyFloat("Height") : 280.f;
}

float UDRSolidWallAdapter::GetGroundHeight()
{
	return 0.0f;//暂时不需要底层还不支持
}

FVector2D UDRSolidWallAdapter::GetFrontUVScale()
{
	return ToVector2D(Obj->GetVector2D("FrontUVScale"));
}

FVector2D UDRSolidWallAdapter::GetSideUVScale()
{
	return ToVector2D(Obj->GetVector2D("SideUVScale"));
}

FVector2D UDRSolidWallAdapter::GetBackUVScale()
{
	return ToVector2D(Obj->GetVector2D("BackUVScale"));
}

bool UDRSolidWallAdapter::GetLeftRuler()
{
	return Obj->GetBool("LeftRuler");
}

bool UDRSolidWallAdapter::GetRightRuler()
{
	return Obj->GetBool("RightRuler");
}

float UDRSolidWallAdapter::GetFrontUVAngle()
{
	return Obj->GetFloat("FrontUVAngle");
}

float UDRSolidWallAdapter::GetSideUVAngle()
{
	return Obj->GetFloat("SideUVAngle");
}

float UDRSolidWallAdapter::GetBackUVAngle()
{
	return Obj->GetFloat("BackUVAngle");
}

FVector2D UDRSolidWallAdapter::GetFrontUVPos()
{
	return ToVector2D(Obj->GetVector2D("FrontUVPos"));
}

FVector2D UDRSolidWallAdapter::GetSideUVPos()
{
	return ToVector2D(Obj->GetVector2D("SideUVPos"));
}

FVector2D UDRSolidWallAdapter::GetBackUVPos()
{
	return ToVector2D(Obj->GetVector2D("BackUVPos"));
}

TArray<FDRMaterial> UDRSolidWallAdapter::GetWallMaterials()
{
	TArray<FDRMaterial> Local_WallMaterials;
	IValue* Local_WallMaterialValues = Obj->FindValue("WallMaterials");
	if (Local_WallMaterialValues && Local_WallMaterialValues->GetArrayCount() > 0)
	{
		FDRMaterial Local_WallMaterial;
		for (int32 Index = 0; Index < Local_WallMaterialValues->GetArrayCount(); ++Index)
		{
			IValue& Local_WallMaterialValue = Local_WallMaterialValues->GetField(Index);
			Local_WallMaterial.ModelID = Local_WallMaterialValue.GetField("ModelID").IntValue();
			Local_WallMaterial.RoomClassID = Local_WallMaterialValue.GetField("RoomClassID").IntValue();
			Local_WallMaterial.CraftID = Local_WallMaterialValue.GetField("CraftID").IntValue();
			Local_WallMaterials.Add(Local_WallMaterial);
		}
	}
	return Local_WallMaterials;
}

TArray<int32> UDRSolidWallAdapter::GetHoles()
{
	TArray<int32> Local_Holes;
	IValue& Local_HolesInfo = Obj->GetPropertyValue("Holes");
	if (&Local_HolesInfo && Local_HolesInfo.GetArrayCount() > 0)
	{
		for (int32 Index = 0; Index < Local_HolesInfo.GetArrayCount(); ++Index)
		{
			IValue& Local_HoleInfo = Local_HolesInfo.GetField(Index);
			Local_Holes.Add(Local_HoleInfo.GetField("HoleID").IntValue());
		}
	}
	return Local_Holes;
}

FString UDRSolidWallAdapter::GetTagName()
{
	return ANSI_TO_TCHAR(Obj->GetString("TagName"));
}

ENewWallType UDRSolidWallAdapter::GetNewWallType()
{
	return static_cast<ENewWallType>(Obj->GetInt("NewWallType"));
}

/********************************Set Function*************************************/

void UDRSolidWallAdapter::SetRoomWall(const bool bRoomWall)
{
	Obj->SetPropertyBool("bRoomWall", bRoomWall);
}

void UDRSolidWallAdapter::SetSolidWallType(const ESolidWallType InSolidWallType)
{
	Obj->SetInt("SolidWallType", static_cast<int32>(InSolidWallType));
}

void UDRSolidWallAdapter::SetMainWall(const bool bMainWall)
{
	Obj->SetPropertyBool("bMainWall", bMainWall);
}

void UDRSolidWallAdapter::SetLeftThick(const float InLeftThick)
{
	Obj->SetPropertyFloat("ThickLeft", InLeftThick);
}

void UDRSolidWallAdapter::SetRightThick(const float InRightThick)
{
	Obj->SetPropertyFloat("ThickRight", InRightThick);
}

void UDRSolidWallAdapter::SetHeight(const float InHeight)
{
	Obj->SetFloat("Height", InHeight);
}

void UDRSolidWallAdapter::SetGroundHeight(const float GroundHeight)
{

}

void UDRSolidWallAdapter::SetFrontUVScale(const FVector2D& InFrontUVScale)
{
	Obj->SetVector2D("FrontUVScale", ToKVector2D(InFrontUVScale));
}

void UDRSolidWallAdapter::SetSideUVScale(const FVector2D& InSideUVScale)
{
	Obj->SetVector2D("SideUVScale", ToKVector2D(InSideUVScale));
}

void UDRSolidWallAdapter::SetBackUVScale(const FVector2D& InBackUVScale)
{
	Obj->SetVector2D("BackUVScale", ToKVector2D(InBackUVScale));
}

void UDRSolidWallAdapter::SetLeftRuler(const bool bLeftRuler)
{
	Obj->SetBool("LeftRuler", bLeftRuler);
}

void UDRSolidWallAdapter::SetRightRuler(const bool bRightRuler)
{
	Obj->SetBool("RightRuler", bRightRuler);
}

void UDRSolidWallAdapter::SetFrontUVAngle(const float InFrontUVAngle)
{
	Obj->SetFloat("FrontUVAngle", InFrontUVAngle);
}

void UDRSolidWallAdapter::SetSideUVAngle(const float InSideUVAngle)
{
	Obj->SetFloat("SideUVAngle", InSideUVAngle);
}

void UDRSolidWallAdapter::SetBackUVAngle(const float InBackUVAngle)
{
	Obj->SetFloat("BackUVAngle", InBackUVAngle);
}

void UDRSolidWallAdapter::SetFrontUVPos(const FVector2D& InFrontUVPos)
{
	Obj->SetVector2D("FrontUVPos", ToKVector2D(InFrontUVPos));
}

void UDRSolidWallAdapter::SetSideUVPos(const FVector2D& InSideUVPos)
{
	Obj->SetVector2D("SideUVPos", ToKVector2D(InSideUVPos));
}

void UDRSolidWallAdapter::SetBackUVPos(const FVector2D& InBackUVPos)
{
	Obj->SetVector2D("BackUVPos", ToKVector2D(InBackUVPos));
}

void UDRSolidWallAdapter::SetWallMaterials(const TArray<FDRMaterial>& InWallMaterials)
{
	IBuildingSDK *Local_SDK = UBuildingSystem::GetBuildingSDK();
	IValueFactory *Local_VF = Local_SDK->GetValueFactory();
	IValue& Local_WallMaterialValues = Local_VF->Create();
	for (const FDRMaterial& CurrentWallMaterial : InWallMaterials)
	{
		IValue& Local_CurrentWallValue = Local_VF->Create();
		Local_CurrentWallValue.AddField("ModelID", Local_VF->Create(CurrentWallMaterial.ModelID));
		Local_CurrentWallValue.AddField("RoomClassID", Local_VF->Create(CurrentWallMaterial.RoomClassID));
		Local_CurrentWallValue.AddField("CraftID", Local_VF->Create(CurrentWallMaterial.CraftID));
		Local_WallMaterialValues.AddField(Local_CurrentWallValue);
	}
	Obj->SetValue("WallMaterials", &Local_WallMaterialValues);
}

void UDRSolidWallAdapter::SetWallTagName(const FString & TagName)
{
	Obj->SetString("TagName", TCHAR_TO_ANSI(*TagName));
}

void UDRSolidWallAdapter::SetNewWallType(const ENewWallType NewWallType)
{
	Obj->SetInt("NewWallType", static_cast<int32>(NewWallType));
}