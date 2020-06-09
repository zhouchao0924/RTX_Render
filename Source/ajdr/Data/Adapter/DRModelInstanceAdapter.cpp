

#include "DRModelInstanceAdapter.h"
#include "Data/DRProjData.h"
#include "Adapter/DRMaterialAdapter.h"
#include "EditorGameInstance.h"

UDRModelInstanceAdapter::UDRModelInstanceAdapter(const FObjectInitializer &ObjectIntializer)
	:Super(ObjectIntializer)
{
}

EModelType UDRModelInstanceAdapter::GetType()
{
	return static_cast<EModelType>(Obj->GetPropertyInt("Type"));
}

FString UDRModelInstanceAdapter::GetResID()
{
	std::string resID = Obj->GetPropertyString("ResID");
	return FString(ANSI_TO_TCHAR(resID.c_str()));
}

int32 UDRModelInstanceAdapter::GetAnchorID()
{
	return Obj->GetPropertyInt("AnchorID");
}

int32 UDRModelInstanceAdapter::GetAnchorOwnerID()
{
	return Obj->GetPropertyInt("AnchorOwnerID");
}

FVector UDRModelInstanceAdapter::GetLocation()
{
	return ToVector(Obj->GetPropertyVector3D("Location"));
}

FVector UDRModelInstanceAdapter::GetSize()
{
	return ToVector(Obj->GetPropertyVector3D("Size"));
}

FRotator UDRModelInstanceAdapter::GetRotation()
{
	return ToRotator(Obj->GetPropertyRotator("Rotation"));
}

int32 UDRModelInstanceAdapter::GetAlignType()
{
	return Obj->GetPropertyInt("AlignType");
}

FVector UDRModelInstanceAdapter::GetClipBase0()
{
	return ToVector(Obj->GetPropertyVector3D("ClipBase0"));
}

FVector UDRModelInstanceAdapter::GetClipNormal0()
{
	return ToVector(Obj->GetPropertyVector3D("ClipNormal0"));
}

FVector UDRModelInstanceAdapter::GetClipBase1()
{
	return ToVector(Obj->GetPropertyVector3D("ClipBase1"));
}

FVector UDRModelInstanceAdapter::GetClipNormal1()
{
	return ToVector(Obj->GetPropertyVector3D("ClipNormal1"));
}

int32 UDRModelInstanceAdapter::GetModelID()
{
	return Obj->GetInt("modelid");
}

int32 UDRModelInstanceAdapter::GetCustomSkuId()
{
	return Obj->GetInt("CustomSkuId");
}

int32 UDRModelInstanceAdapter::GetRoomClassIdByNormalModel()
{
	return Obj->GetInt("RoomClassId");
}

int32 UDRModelInstanceAdapter::GetCraftIdByNormalModel()
{
	return Obj->GetInt("CraftId");
}

FTransform UDRModelInstanceAdapter::GetTransform()
{
	kXform xForm = Obj->GetTransform();
	return ToTransform(xForm);
}

FTransform UDRModelInstanceAdapter::GetTransformBySize()
{
	return  FTransform(GetRotation().Quaternion(), GetLocation(), GetSize());
}

FDRMaterial UDRModelInstanceAdapter::GetDependsMat(const int32 SectionID)
{
	/*FDRMaterial DependsMaterial;
	UDRMaterialAdapter* Local_MaterialAdapter = GetMaterial(Obj->GetID(), SectionID);
	if (Local_MaterialAdapter)
	{
		DependsMaterial.ModelID = Local_MaterialAdapter->GetModelID();
		DependsMaterial.RoomClassID = Local_MaterialAdapter->GetRoomClassID();
		DependsMaterial.CraftID = Local_MaterialAdapter->GetCraftID();
	}
	return DependsMaterial;*/
	TMap<int32, FDRMaterial> Local_AllDependsMaterial = GetDependsMaterialByValidSections();
	FDRMaterial* Local_DependMaterial = Local_AllDependsMaterial.Find(SectionID);
	return	Local_DependMaterial ? *Local_DependMaterial : FDRMaterial();
}

TMap<int32, FDRMaterial> UDRModelInstanceAdapter::GetDependsMaterialByValidSections()
{
	/*TArray<int32> Local_AllSectionIds;
	TMap<int32, FDRMaterial> Local_DependsMaterialMap;
	GetValidMaterialSections(Local_AllSectionIds);

	for (int32 CurrentSection : Local_AllSectionIds)
	{
		Local_DependsMaterialMap.Add(CurrentSection, GetDependsMat(CurrentSection));
	}
	return Local_DependsMaterialMap;*/
	TMap<int32, FDRMaterial> Local_DependsMaterialMap;
	IValue* Local_DependMaterialValues = Obj->FindValue("DependsMaterial");
	if (Local_DependMaterialValues && Local_DependMaterialValues->GetArrayCount() > 0)
	{
		int32 Local_CurrentSection = 0;
		FDRMaterial Local_CurrentDependMaterial;
		for (int32 Index=0; Index< Local_DependMaterialValues->GetArrayCount();++Index)
		{
			IValue& Local_DependMaterialValue = Local_DependMaterialValues->GetField(Index);
			Local_CurrentSection = Local_DependMaterialValue.GetField("SectionID").IntValue();
			Local_CurrentDependMaterial.ModelID= Local_DependMaterialValue.GetField("ModelID").IntValue();
			Local_CurrentDependMaterial.RoomClassID = Local_DependMaterialValue.GetField("RoomClassID").IntValue();
			Local_CurrentDependMaterial.CraftID = Local_DependMaterialValue.GetField("CraftID").IntValue();
			Local_DependsMaterialMap.Add(Local_CurrentSection, Local_CurrentDependMaterial);
		}
	}
	return	Local_DependsMaterialMap;
}

bool UDRModelInstanceAdapter::GetApplyShadow()
{
	return Obj->GetBool("ApplyShadow");
}

FString UDRModelInstanceAdapter::GetModelTag()
{
	return UTF8_TO_TCHAR(Obj->GetString("ModelTag"));
}

FDRUV UDRModelInstanceAdapter::GetDependsUV()
{
	/*FDRUV Local_UV;
	Local_UV.UVScale = GetUVScale();
	Local_UV.UVOffset = GetUVOffset();
	Local_UV.UVAngle = GetUVRot();
	return Local_UV;*/
	FDRUV Local_UV;
	Local_UV.UVScale = ToVector2D(Obj->GetVector2D("UVScale"));
	Local_UV.UVOffset = ToVector2D(Obj->GetVector2D("UVOffset"));
	Local_UV.UVAngle = Obj->GetFloat("UVAngle");
	return Local_UV;
}

void UDRModelInstanceAdapter::SetModelID(int32 ModelID)
{
	Obj->SetInt("modelid", ModelID);
}

void UDRModelInstanceAdapter::SetCustomSkuId(const int32 InSkuId)
{
	Obj->SetInt("CustomSkuId", InSkuId);
}

void UDRModelInstanceAdapter::SetRoomClassIdByNormalModel(const int32 InRoomClassId)
{
	Obj->SetInt("RoomClassId", InRoomClassId);
}

void UDRModelInstanceAdapter::SetCraftIdByNormalModel(const int32 InCraftId)
{
	Obj->SetInt("CraftId", InCraftId);
}

void UDRModelInstanceAdapter::SetResId(const FString& InResId)
{
	Obj->SetPropertyString("ResID", TCHAR_TO_ANSI(*InResId));
}

void UDRModelInstanceAdapter::SetDependsMat(const FDRMaterial &DependsMat, const int32 SectionID)
{
	/*UDRMaterialAdapter *Local_MaterialAdapter = GetMaterial(Obj->GetID(), SectionID);
	if (Local_MaterialAdapter)
	{
		Local_MaterialAdapter->SetModelID(DependsMat.ModelID);
		Local_MaterialAdapter->SetRoomClassID(DependsMat.RoomClassID);
		Local_MaterialAdapter->SetCraftID(DependsMat.CraftID);
		UCEditorGameInstance* Local_GameInstance = GetGameInstance();
		if (Local_GameInstance)
		{
			Local_MaterialAdapter->SetMaterial(Local_GameInstance->FindResIDByModelID(DependsMat.ModelID));
		}
	}	*/
	TMap<int32, FDRMaterial> Local_AllDependsMaterial = GetDependsMaterialByValidSections();
	Local_AllDependsMaterial.Add(SectionID, DependsMat);
	SetDependsMaterialByValidSections(Local_AllDependsMaterial);
}

void UDRModelInstanceAdapter::SetDependsMaterialByValidSections(const TMap<int32, FDRMaterial>& InDependsMaterial)
{
	/*TArray<int32> Local_AllSections;
	InDependsMaterial.GenerateKeyArray(Local_AllSections);
	for (int32 CurrentSection : Local_AllSections)
	{
		FDRMaterial const* Local_CurrentMat = InDependsMaterial.Find(CurrentSection);
		SetDependsMat(*Local_CurrentMat, CurrentSection);
	}*/
	TArray<int32> Local_AllSections;
	IBuildingSDK *Local_SDK = UBuildingSystem::GetBuildingSDK();
	IValueFactory *Local_VF = Local_SDK->GetValueFactory();
	IValue& Local_DependMaterialValues = Local_VF->Create();
	InDependsMaterial.GenerateKeyArray(Local_AllSections);
	for (int32 CurrentSection : Local_AllSections)
	{
		FDRMaterial const* Local_CurentDependMaterial = InDependsMaterial.Find(CurrentSection);
		if (Local_CurentDependMaterial)
		{
			IValue& Local_DependMaterialValue = Local_VF->Create();
			Local_DependMaterialValue.AddField("SectionID", Local_VF->Create(CurrentSection));
			Local_DependMaterialValue.AddField("ModelID", Local_VF->Create(Local_CurentDependMaterial->ModelID));
			Local_DependMaterialValue.AddField("RoomClassID", Local_VF->Create(Local_CurentDependMaterial->CraftID));
			Local_DependMaterialValues.AddField(Local_DependMaterialValue);
		}
	}
	Obj->SetValue("DependsMaterial", &Local_DependMaterialValues);
}

void UDRModelInstanceAdapter::SetApplyShadow(const bool &ApplyShadow)
{
	Obj->SetBool("ApplyShadow", ApplyShadow);
}

void UDRModelInstanceAdapter::SetModelTag(const FString &ModelTag)
{
	Obj->SetString("ModelTag", TCHAR_TO_UTF8(*ModelTag));
}

void UDRModelInstanceAdapter::SetLocation(const FVector &Location)
{
	Obj->SetPropertyVector3D("Location", ToKVector3D(Location));
}

void UDRModelInstanceAdapter::SetSize(const FVector &Size)
{
	Obj->SetPropertyVector3D("Size", ToKVector3D(Size));
}

void UDRModelInstanceAdapter::SetRotation(const FRotator &Rotation)
{
	Obj->SetPropertyRotator("Rotation", ToKRotation(Rotation));
}

void UDRModelInstanceAdapter::SetTransformBySize(const FTransform &ModelTransform)
{
	SetLocation(ModelTransform.GetLocation());
	SetSize(ModelTransform.GetScale3D());
	SetRotation(ModelTransform.GetRotation().Rotator());
}

void UDRModelInstanceAdapter::SetDependsUV(const FDRUV& InUV)
{
	/*SetUVScale(InUV.UVScale);
	SetUVOffset(InUV.UVOffset);
	SetUVRot(InUV.UVAngle);*/
	Obj->SetVector2D("UVScale", ToKVector2D(InUV.UVScale));
	Obj->SetVector2D("UVOffset", ToKVector2D(InUV.UVOffset));
	Obj->SetFloat("UVAngle", InUV.UVAngle);
}

void UDRModelInstanceAdapter::SetType(EModelType ModelType)
{
	Obj->SetPropertyInt("Type", static_cast<int32>(ModelType));
}

UCEditorGameInstance* UDRModelInstanceAdapter::GetGameInstance()
{
	UWorld* Local_CurrentWorld = GEngine->GetWorldFromContextObject(this);
	if (Local_CurrentWorld)
	{
		return Cast<UCEditorGameInstance>(Local_CurrentWorld->GetGameInstance());
	}
	return nullptr;
}