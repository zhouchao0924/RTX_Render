// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DROperationBuildingFunctionLibrary.h"
#include "Data/DRProjData.h"
#include "Adapter/DRPointLightAdapter.h"
#include "Adapter/DRSpotLightAdapter.h"
#include "Adapter/DRModelInstanceAdapter.h"
#include "Adapter/DRSolidWallAdapter.h"
#include "Adapter/DRWindowHoleAdapter.h"
#include "Adapter/DRWindowAdapter.h"
#include "Adapter/DRDoorHoleAdapter.h"
#include "Adapter/DRDoorAdapter.h"
#include "Adapter/DRPointLightAdapter.h"
#include "Adapter/DRSpotLightAdapter.h"
#include "Misc/Guid.h"
#include "EditorGameInstance.h"
#include "DROperationHouseFunctionLibrary.h"

int32 UDROperationBuildingFunctionLibrary::AddPointLight(UObject* WorldContextObject, const FDRPointLightBase& InPointLightConfig)
{
	int32 Local_CurrentId = 0;
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (Local_DRProjData && Local_DRProjData->GetSuite())
	{
		 Local_CurrentId = Local_DRProjData->GetSuite()->AddPointLight(UDRData::ToKVector3D(InPointLightConfig.Location), InPointLightConfig.SourceRadius, 0.f, InPointLightConfig.SourceLength, InPointLightConfig.LightIntensity, UDRData::ToKColor(InPointLightConfig.LightColor), InPointLightConfig.IsCastShadow, InPointLightConfig.Temperature, InPointLightConfig.Highlight);
		UDRPointLightAdapter* Local_PointLightAdapter = Cast<UDRPointLightAdapter>(Local_DRProjData->GetAdapter(Local_CurrentId));
		if (Local_PointLightAdapter)
		{
			Local_PointLightAdapter->SetAttenuationRadius(InPointLightConfig.AttenuationRadius);
			Local_PointLightAdapter->SetShadowResolutionScale(InPointLightConfig.ShadowResolutionScale);
			Local_PointLightAdapter->SetIsAutoPoint(InPointLightConfig.IsAutoPoint);
			Local_PointLightAdapter->SetLightType(InPointLightConfig.LightTyp);
		}
	}
	return Local_CurrentId;
}

int32 UDROperationBuildingFunctionLibrary::AddSpotLight(UObject* WorldContextObject, const FDRSpotLightBase& InSpotLightConfig)
{
	int32 Local_CurrentId = 0;
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (Local_DRProjData && Local_DRProjData->GetSuite())
	{
		 Local_CurrentId = Local_DRProjData->GetSuite()->AddSpotLight(UDRData::ToKVector3D(InSpotLightConfig.Location), UDRData::ToKRotation(InSpotLightConfig.Rotation), InSpotLightConfig.AttenuationRadius, InSpotLightConfig.SourceRadius, 0.f, 0.f, InSpotLightConfig.InnerConeAngle, InSpotLightConfig.OuterConeAngle, InSpotLightConfig.LightIntensity, UDRData::ToKColor(InSpotLightConfig.LightColor), InSpotLightConfig.IsCastShadow, InSpotLightConfig.Temperature, InSpotLightConfig.Highlight);
		UDRSpotLightAdapter* Local_SpotLightAdapter = Cast<UDRSpotLightAdapter>(Local_DRProjData->GetAdapter(Local_CurrentId));
		if (Local_SpotLightAdapter)
		{
			Local_SpotLightAdapter->SetShadowResolutionScale(InSpotLightConfig.ShadowResolutionScale);
			Local_SpotLightAdapter->SetLightType(InSpotLightConfig.LightTyp);
		}
	}
	return Local_CurrentId;
}

int32 UDROperationBuildingFunctionLibrary::AddRectLight(UObject* WorldContextObject, const FDRRectLightBase& InRectLightConfig)
{
	int32 Local_CurrentId = 0;
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (Local_DRProjData && Local_DRProjData->GetSuite())
	{
		//Local_CurrentId = Local_DRProjData->GetSuite()->AddRectLight(UDRData::ToKVector3D(InRectLightConfig.Location), UDRData::ToKRotation(InRectLightConfig.Rotation), InRectLightConfig.AttenuationRadius, InRectLightConfig.SourceWidth, InRectLightConfig.Highlight, InRectLightConfig.BarnDoorAngle, InRectLightConfig.BarnDoorLength, InRectLightConfig.LightIntensity, UDRData::ToKColor(InRectLightConfig.LightColor), InRectLightConfig.IsCastShadow, InRectLightConfig.Temperature, InRectLightConfig.Highlight);
		UDRRectLightAdapter* Local_RectLightAdapter = Cast<UDRRectLightAdapter>(Local_DRProjData->GetAdapter(Local_CurrentId));
		if (Local_RectLightAdapter)
		{
			Local_RectLightAdapter->SetLightType(InRectLightConfig.LightTyp);
		}
	}
	return Local_CurrentId;
}

int32 UDROperationBuildingFunctionLibrary::AddNewModel(UObject* WorldContextObject, const FDNormalModel& InModelInstance)
{
	int32 Local_CurrentId = 0;
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (Local_DRProjData && Local_DRProjData->GetSuite())
	{
		Local_CurrentId = Local_DRProjData->GetSuite()->AddModel(INVALID_OBJID, TCHAR_TO_ANSI(*InModelInstance.ResId), UDRData::ToKVector3D(InModelInstance.Transform.GetLocation()), UDRData::ToKRotation(InModelInstance.Transform.GetRotation().Rotator()), UDRData::ToKVector3D(InModelInstance.Transform.GetScale3D()), static_cast<int32>(InModelInstance.ModelType));
		UDRModelInstanceAdapter* Local_ModelInstanceAdapter = Cast<UDRModelInstanceAdapter>(Local_DRProjData->GetAdapter(Local_CurrentId));
		if (Local_ModelInstanceAdapter)
		{
			Local_ModelInstanceAdapter->SetModelID(InModelInstance.ModelId);
			Local_ModelInstanceAdapter->SetCustomSkuId(InModelInstance.CustomSkuId);
			Local_ModelInstanceAdapter->SetDependsUV(InModelInstance.UV);
			Local_ModelInstanceAdapter->SetApplyShadow(InModelInstance.ApplyShadow);
			Local_ModelInstanceAdapter->SetRoomClassIdByNormalModel(InModelInstance.RoomClassId);
			Local_ModelInstanceAdapter->SetCraftIdByNormalModel(InModelInstance.CraftId);
			Local_ModelInstanceAdapter->SetModelTag(InModelInstance.ModelTag);
			Local_ModelInstanceAdapter->SetDependsMaterialByValidSections(InModelInstance.DependsMat);
		}
	}
	return Local_CurrentId;
}

int32 UDROperationBuildingFunctionLibrary::AddWallByCorner(UObject* WorldContextObject, const FDRSolidWall& InSolidWall)
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (projectDataManager == nullptr || projectDataManager->GetSuite() == nullptr) {
		return INVALID_OBJID;
	}

	ISuite* suite = projectDataManager->GetSuite();

	int32 StartCorner = suite->AddCorner(InSolidWall.StartPos.X, InSolidWall.StartPos.Y);
	int32 EndCorner = suite->AddCorner(InSolidWall.EndPos.X, InSolidWall.EndPos.Y);

	kArray<int32> effectedWalls = suite->AddWall(StartCorner, EndCorner, InSolidWall.LeftThick, InSolidWall.RightThick, InSolidWall.Height);

	if (effectedWalls.size() <= 0) {
		return INVALID_OBJID;
	}

	int32 newWallId = suite->GetWallByTwoCorner(StartCorner, EndCorner);
	if (newWallId == INVALID_OBJID) {
		return INVALID_OBJID;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(newWallId));
	if (solidWallAdapter == nullptr) {
		return newWallId;
	}

	solidWallAdapter->SetLeftRuler(InSolidWall.LeftRuler);
	solidWallAdapter->SetRightRuler(InSolidWall.RightRuler);
	solidWallAdapter->SetMainWall(InSolidWall.MainWall);
	solidWallAdapter->SetRoomWall(InSolidWall.RoomWall);
	solidWallAdapter->SetWallTagName(FGuid::NewGuid().ToString());
	solidWallAdapter->SetSolidWallType(InSolidWall.SolidWallType);

	solidWallAdapter->SetFrontUVScale(InSolidWall.SolidWallUV[0].UVScale);
	solidWallAdapter->SetFrontUVPos(InSolidWall.SolidWallUV[0].UVOffset);
	solidWallAdapter->SetFrontUVAngle(InSolidWall.SolidWallUV[0].UVAngle);

	solidWallAdapter->SetBackUVScale(InSolidWall.SolidWallUV[1].UVScale);
	solidWallAdapter->SetBackUVPos(InSolidWall.SolidWallUV[1].UVOffset);
	solidWallAdapter->SetBackUVAngle(InSolidWall.SolidWallUV[1].UVAngle);

	solidWallAdapter->SetSideUVScale(InSolidWall.SolidWallUV[2].UVScale);
	solidWallAdapter->SetSideUVPos(InSolidWall.SolidWallUV[2].UVOffset);
	solidWallAdapter->SetSideUVAngle(InSolidWall.SolidWallUV[2].UVAngle);

	solidWallAdapter->SetWallMaterials(InSolidWall.SolidWallMaterials);

	return newWallId;
}

int32 UDROperationBuildingFunctionLibrary::AddNewHole(UObject* WorldContextObject, const FDRHole& InHole)
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (projectDataManager == nullptr) {
		return INVALID_OBJID;
	}

	UWorld* currentWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	if (currentWorld == nullptr) {
		return INVALID_OBJID;
	}

	UCEditorGameInstance* editorGameInstance = Cast<UCEditorGameInstance>(currentWorld->GetGameInstance());
	if (editorGameInstance == nullptr) {
		return INVALID_OBJID;
	}

	ISuite* suite = projectDataManager->GetSuite();
	if (suite == nullptr) {
		return INVALID_OBJID;
	}
	
	FVector HoleLocation = InHole.Transform.GetLocation();
	kPoint BestPoint;
	int32 closedWallId = suite->FindCloseWall(kPoint(HoleLocation.X, HoleLocation.Y), InHole.Width, BestPoint, 10.0f);

	FVector2D BestLocation(BestPoint.X, BestPoint.Y);
	float HoleWidth(0.0f);
	bool bFoundBestLocation(false);

	editorGameInstance->WallBuildSystem->FindBestLoc(BestLocation, closedWallId, InHole.Width, HoleWidth, bFoundBestLocation);

	if (bFoundBestLocation == false) {
		return INVALID_OBJID;
	}

	int32 HoleObjectId(INVALID_OBJID);

	if (InHole.HoleType > EModelType::EDRDoorWay) {
		HoleObjectId = suite->AddWindow(closedWallId, kPoint(BestLocation.X, BestLocation.Y), InHole.ZPos, HoleWidth, InHole.Height, "", "");
	}
	else {
		HoleObjectId = suite->AddDoor(closedWallId, kPoint(BestLocation.X, BestLocation.Y), HoleWidth, InHole.Height, InHole.ZPos);
	}

	UDRHoleAdapter* holeAdapter = Cast<UDRHoleAdapter>(projectDataManager->GetAdapter(HoleObjectId));

	if (holeAdapter != nullptr) {
		holeAdapter->SetHorizontalFlip(InHole.HorizontalFlip);
		holeAdapter->SetVerticalFlip(InHole.VerticalFlip);
		holeAdapter->SetSelfTagName(InHole.SelfTagName);
		holeAdapter->SetSlotActorTag(InHole.SlotActorTag);
		holeAdapter->SetHoleMaterials(InHole.HoleMaterials);
		holeAdapter->SetHoleUVs(InHole.HoleUVs);
		holeAdapter->SetHoleType(InHole.HoleType);
	}

	return HoleObjectId;
}

void UDROperationBuildingFunctionLibrary::AddNewDoor(UObject* WorldContextObject, const FDRDoorAndWindow& InDoorData, int32& DoorObjectId, int32& DoorFrameObjectId)
{
	DoorObjectId = INVALID_OBJID;
	DoorFrameObjectId = INVALID_OBJID;

	if (!InDoorData.ModelIds.IsValidIndex(0)) {
		return;
	}

	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (projectDataManager == nullptr) {
		return;
	}

	UWorld* currentWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	if (currentWorld == nullptr) {
		return;
	}

	UCEditorGameInstance* editorGameInstance = Cast<UCEditorGameInstance>(currentWorld->GetGameInstance());
	if (editorGameInstance == nullptr) {
		return;
	}

	ISuite* suite = projectDataManager->GetSuite();
	if (suite == nullptr) {
		return;
	}

	IObject** doorHoleObjects(nullptr);
	int32 doorHoleCount = suite->GetAllObjects(doorHoleObjects, EObjectType::EDoorHole);

	if (doorHoleCount < 1) {
		return;
	}

	for (int32 Index = 0; Index < doorHoleCount; Index++)
	{
		UDRDoorHoleAdapter* doorHoleAdapter = Cast<UDRDoorHoleAdapter>(projectDataManager->GetAdapter(doorHoleObjects[Index]->GetID()));
		if (doorHoleAdapter == nullptr || doorHoleAdapter->GetSlotActorTag() != InDoorData.TagName) {
			continue;
		}

		if (!InDoorData.ModelIds.IsValidIndex(1)) {
			return;
		}

		FString doorFrameResId(editorGameInstance->FindResIDByModelID(InDoorData.ModelIds[1]));

		if (!doorFrameResId.IsEmpty()) {
			doorHoleAdapter->SetDoorFrameResId(doorFrameResId);
			DoorFrameObjectId = doorHoleAdapter->GetDoorFrameObjId();

			UDRModelInstanceAdapter* doorFrameAdapter = Cast<UDRModelInstanceAdapter>(projectDataManager->GetAdapter(DoorFrameObjectId));
			if (doorFrameAdapter) {
				doorFrameAdapter->SetModelID(InDoorData.ModelIds[1]);
				doorFrameAdapter->SetRoomClassID(InDoorData.RoomClassIds[1]);
				doorFrameAdapter->SetCraftID(InDoorData.CraftIDs[1]);
				doorFrameAdapter->SetDependsMat(InDoorData.DependsMats[1], 1);
				doorFrameAdapter->SetModelTag(InDoorData.TagName);
				doorFrameAdapter->SetApplyShadow(true);
				doorFrameAdapter->SetType(EModelType::EDRDoorFram);
			}
		}

		FString doorResId(editorGameInstance->FindResIDByModelID(InDoorData.ModelIds[0]));
		
		if (!doorResId.IsEmpty()) {
			doorHoleAdapter->SetDoorResId(doorResId);
			DoorObjectId = doorHoleAdapter->GetDoorObjId();

			UDRModelInstanceAdapter* doorAdapter = Cast<UDRModelInstanceAdapter>(projectDataManager->GetAdapter(DoorObjectId));
			if (doorAdapter) {
				doorAdapter->SetModelID(InDoorData.ModelIds[0]);
				doorAdapter->SetRoomClassID(InDoorData.RoomClassIds[0]);
				doorAdapter->SetCraftID(InDoorData.CraftIDs[0]);
				doorAdapter->SetDependsMat(InDoorData.DependsMats[0], 0);
				doorAdapter->SetModelTag(InDoorData.TagName);
				doorAdapter->SetApplyShadow(true);
				doorAdapter->SetType(InDoorData.ModelType);
			}
		}

		return;
	}
}

void UDROperationBuildingFunctionLibrary::AddNewWindow(UObject* WorldContextObject, const FDRDoorAndWindow& InWindowData, int32& WindowObjectId)
{
	WindowObjectId = INVALID_OBJID;

	if (!InWindowData.ModelIds.IsValidIndex(0)) {
		return;
	}

	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (projectDataManager == nullptr) {
		return;
	}

	UWorld* currentWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	if (currentWorld == nullptr) {
		return;
	}

	UCEditorGameInstance* editorGameInstance = Cast<UCEditorGameInstance>(currentWorld->GetGameInstance());
	if (editorGameInstance == nullptr) {
		return;
	}

	ISuite* suite = projectDataManager->GetSuite();
	if (suite == nullptr) {
		return;
	}

	IObject** windowHoleObjects(nullptr);
	int32 windowHoleCount = suite->GetAllObjects(windowHoleObjects, EObjectType::EWindow);

	if (windowHoleCount < 1) {
		return;
	}

	for (int32 Index = 0; Index < windowHoleCount; Index++)
	{
		UDRWindowHoleAdapter* windowHoleAdapter = Cast<UDRWindowHoleAdapter>(projectDataManager->GetAdapter(windowHoleObjects[Index]->GetID()));
		if (windowHoleAdapter == nullptr || windowHoleAdapter->GetSlotActorTag() != InWindowData.TagName) {
			continue;
		}

		FString windowResId(editorGameInstance->FindResIDByModelID(InWindowData.ModelIds[0]));
		if (windowResId.IsEmpty())
		{
			switch (InWindowData.ModelType)
			{
			case EModelType::EDRWindow:
				windowResId = TEXT("FD3FF7E34DF874E415944AB8C55E7368");
				break;
			case EModelType::EDRGroundWindow:
				windowResId = TEXT("D8801CC14B61F21FB5190486DF78C8DF");
				break;
			case EModelType::EDRBayWindow:
				windowResId = TEXT("8F2EE01D45E222358E942FB895F141BB");
				break;
			}
		}
		if (!windowResId.IsEmpty()) {
			windowHoleAdapter->SetWindowResId(windowResId);
			WindowObjectId = windowHoleAdapter->GetWindowObjId();

			UDRModelInstanceAdapter* windowAdapter = Cast<UDRModelInstanceAdapter>(projectDataManager->GetAdapter(WindowObjectId));
			if (windowAdapter) {
				windowAdapter->SetModelID(InWindowData.ModelIds[0]);
				windowAdapter->SetRoomClassID(InWindowData.RoomClassIds[0]);
				windowAdapter->SetCraftID(InWindowData.CraftIDs[0]);
				windowAdapter->SetDependsMat(InWindowData.DependsMats[0], 0);
				windowAdapter->SetModelTag(InWindowData.TagName);
				windowAdapter->SetApplyShadow(true);
				windowAdapter->SetType(InWindowData.ModelType);
			}
		}

		return;
	}
}

void UDROperationBuildingFunctionLibrary::UpdateHisMatLists(TArray<FDRMaterial> &InHisMatListRef, FDRMaterial InHisMat)
{
	if (InHisMatListRef.Num() == 0)
	{
		InHisMatListRef.Add(InHisMat);
		InHisMatListRef.Add(InHisMat);
	}
	else
	{
		InHisMatListRef.RemoveAt(0);
		if ((InHisMatListRef[0].ModelID) == (InHisMat.ModelID))
		{
			InHisMatListRef.Insert(InHisMat,0);
		}
		else
		{
			for (int32 i=0;i<InHisMatListRef.Num();i++)
			{
				if ((InHisMatListRef[i].ModelID) == (InHisMat.ModelID))
					InHisMatListRef.RemoveAt(i);
			}
			InHisMatListRef.Insert(InHisMat,0);
			InHisMatListRef.Insert(InHisMat,2);
			if (InHisMatListRef.IsValidIndex(6))
			{
				InHisMatListRef.RemoveAt(6);
			}
		}
	}
}

FTransform UDROperationBuildingFunctionLibrary::CalculateSillTransform(UObject* WorldContextObject, const int32 InObjId, const FVector InModelBounds)
{
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (!Local_DRProjData)
		return FTransform();

	UDRWindowHoleAdapter* Local_WindowHoleAdapter = Cast<UDRWindowHoleAdapter>(Local_DRProjData->GetAdapter(InObjId));
	if (!Local_WindowHoleAdapter)
		return FTransform();

	UDRWindowAdapter* Local_WindowAdapter = Cast<UDRWindowAdapter>(Local_DRProjData->GetAdapter(Local_WindowHoleAdapter->GetWindowObjId()));
	if (!Local_WindowAdapter)
		return FTransform();

	FVector Local_TempPos = Local_WindowHoleAdapter->GetTransform().GetLocation();
	Local_TempPos.Z = Local_WindowHoleAdapter->GetGroundHeight() - InModelBounds.Z + 1;

	FRotator Local_TempRot = Local_WindowAdapter->GetRotation();
	Local_TempRot.Yaw = Local_WindowHoleAdapter->GetVerticalFlip() ? Local_TempRot.Yaw: Local_TempRot.Yaw + 180.f;

	FVector Local_Scale(Local_WindowHoleAdapter->GetWidth() / (InModelBounds.X - 10), Local_WindowHoleAdapter->GetThickness() / (InModelBounds.Y - 1.9), 1);

	return FTransform(Local_TempRot.Quaternion(), Local_TempPos, Local_Scale);

}

FTransform UDROperationBuildingFunctionLibrary::CalculateSillTransformByBayWindow(UObject* WorldContextObject, const int32 InObjId, const FVector InModelBounds, const FVector InBayBounds)
{
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (!Local_DRProjData)
		return FTransform();

	UDRWindowHoleAdapter* Local_WindowHoleAdapter = Cast<UDRWindowHoleAdapter>(Local_DRProjData->GetAdapter(InObjId));
	if (!Local_WindowHoleAdapter)
		return FTransform();

	UDRWindowAdapter* Local_WindowAdapter = Cast<UDRWindowAdapter>(Local_DRProjData->GetAdapter(Local_WindowHoleAdapter->GetWindowObjId()));
	if (!Local_WindowAdapter)
		return FTransform();

	FVector Local_TempPos = Local_WindowHoleAdapter->GetTransform().GetLocation();
	Local_TempPos.Z = Local_WindowHoleAdapter->GetGroundHeight() - InModelBounds.Z + 5.f;

	FRotator Local_TempRot = Local_WindowAdapter->GetRotation();
	Local_TempRot.Yaw = Local_WindowHoleAdapter->GetVerticalFlip() ? Local_TempRot.Yaw: Local_TempRot.Yaw + 180.f;

	Local_TempPos += UKismetMathLibrary::GetRightVector(Local_TempRot)*(-19.f);

	FVector Local_Scale(Local_WindowHoleAdapter->GetWidth() / (InModelBounds.X - 10), InBayBounds.Y / (InModelBounds.Y - 1.9), 1);

	return FTransform(Local_TempRot.Quaternion(), Local_TempPos, Local_Scale);

}

bool UDROperationBuildingFunctionLibrary::ConvertModelAdapterToNormalModel(UObject * WorldContextObject, const int32 InObjId, FDNormalModel & OutNormalModelData)
{
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (!Local_DRProjData)
		return false;
	UDRModelInstanceAdapter* Local_ModelAdapter = Cast<UDRModelInstanceAdapter>(Local_DRProjData->GetAdapter(InObjId));
	if (!Local_ModelAdapter)
		return false;

	OutNormalModelData.Transform = Local_ModelAdapter->GetTransformBySize();
	OutNormalModelData.ModelType = Local_ModelAdapter->GetType();
	OutNormalModelData.ResId = Local_ModelAdapter->GetResID();
	OutNormalModelData.ModelId = Local_ModelAdapter->GetModelID();
	OutNormalModelData.CustomSkuId = Local_ModelAdapter->GetCustomSkuId();
	OutNormalModelData.RoomClassId = Local_ModelAdapter->GetRoomClassIdByNormalModel();
	OutNormalModelData.CraftId = Local_ModelAdapter->GetCraftIdByNormalModel();
	OutNormalModelData.ModelTag = Local_ModelAdapter->GetModelTag();
	OutNormalModelData.ApplyShadow = Local_ModelAdapter->GetApplyShadow();
	OutNormalModelData.DependsMat = Local_ModelAdapter->GetDependsMaterialByValidSections();
	OutNormalModelData.UV = Local_ModelAdapter->GetDependsUV();
	return true;
}

bool UDROperationBuildingFunctionLibrary::ConvertPointAdapterToPointLightBase(UObject * WorldContextObject, const int32 InObjId, FDRPointLightBase & OutPointLightBase)
{
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (!Local_DRProjData)
		return false;
	UDRPointLightAdapter* Local_PointLightAdapter = Cast<UDRPointLightAdapter>(Local_DRProjData->GetAdapter(InObjId));
	if (!Local_PointLightAdapter)
		return false;
	OutPointLightBase.LightTyp = Local_PointLightAdapter->GetLightType();
	OutPointLightBase.Location = Local_PointLightAdapter->GetLocation();
	OutPointLightBase.LightIntensity = Local_PointLightAdapter->GetIntensity();
	OutPointLightBase.LightColor = Local_PointLightAdapter->GetLightColor();
	OutPointLightBase.AttenuationRadius = Local_PointLightAdapter->GetAttenuationRadius();
	OutPointLightBase.SourceRadius = Local_PointLightAdapter->GetSourceRadius();
	OutPointLightBase.Temperature = Local_PointLightAdapter->GetColorTemperature();
	OutPointLightBase.Highlight = Local_PointLightAdapter->GetHighlight();
	OutPointLightBase.ShadowResolutionScale = Local_PointLightAdapter->GetShadowResolutionScale();
	OutPointLightBase.IsCastShadow = Local_PointLightAdapter->GetIsCastShadow();

	OutPointLightBase.IsAutoPoint = Local_PointLightAdapter->GetIsAutoPoint();
	OutPointLightBase.SourceLength = Local_PointLightAdapter->GetSourceLength();
	return true;
}

bool UDROperationBuildingFunctionLibrary::ConvertSpotAdapterToSpotLightBase(UObject* WorldContextObject, const int32 InObjId, FDRSpotLightBase& OutSpotLightBase)
{
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(WorldContextObject);
	if (!Local_DRProjData)
		return false;
	UDRSpotLightAdapter* Local_SpotLightAdapter = Cast<UDRSpotLightAdapter>(Local_DRProjData->GetAdapter(InObjId));
	if (!Local_SpotLightAdapter)
		return false;
	OutSpotLightBase.LightTyp = Local_SpotLightAdapter->GetLightType();
	OutSpotLightBase.Location = Local_SpotLightAdapter->GetLocation();
	OutSpotLightBase.LightIntensity = Local_SpotLightAdapter->GetIntensity();
	OutSpotLightBase.LightColor = Local_SpotLightAdapter->GetLightColor();
	OutSpotLightBase.AttenuationRadius = Local_SpotLightAdapter->GetAttenuationRadius();
	OutSpotLightBase.SourceRadius = Local_SpotLightAdapter->GetSourceRadius();
	OutSpotLightBase.Temperature = Local_SpotLightAdapter->GetColorTemperature();
	OutSpotLightBase.Highlight = Local_SpotLightAdapter->GetHighlight();
	OutSpotLightBase.ShadowResolutionScale = Local_SpotLightAdapter->GetShadowResolutionScale();
	OutSpotLightBase.IsCastShadow = Local_SpotLightAdapter->GetIsCastShadow();

	OutSpotLightBase.Rotation = Local_SpotLightAdapter->GetRotation();
	OutSpotLightBase.InnerConeAngle = Local_SpotLightAdapter->GetInnerConeAngle();
	OutSpotLightBase.OuterConeAngle = Local_SpotLightAdapter->GetOuterConeAngle();
	return true;
}

TArray<FDRMaterial> UDROperationBuildingFunctionLibrary::ResetAreaMaterialIfNeed(const TArray<FDRMaterial>& AreaMaterials)
{
	TArray<FDRMaterial> ResultMaterials(AreaMaterials);

	FDRMaterial FloorMaterial;
	FloorMaterial.ModelID = UDROperationHouseFunctionLibrary::GetConfigModelIDByType(EDConfigType::EDRAreaGroundConfig);;
	FloorMaterial.RoomClassID = 0;
	FloorMaterial.CraftID = 0;

	FDRMaterial CeilMaterial;
	CeilMaterial.ModelID = UDROperationHouseFunctionLibrary::GetConfigModelIDByType(EDConfigType::EDRAreaRoofConfig);
	CeilMaterial.RoomClassID = 0;
	CeilMaterial.CraftID = 0;

	do 
	{
		if (!ResultMaterials.IsValidIndex(0)) {
			ResultMaterials.Add(FloorMaterial);
			ResultMaterials.Add(CeilMaterial);
			ResultMaterials.Add(CeilMaterial);
			break;
		}

		if (!ResultMaterials.IsValidIndex(1)) {
			ResultMaterials.Add(CeilMaterial);
			ResultMaterials.Add(CeilMaterial);
			break;
		}
		if (!ResultMaterials.IsValidIndex(2)) {
			ResultMaterials.Add(CeilMaterial);
			break;
		}
	} while (false);

	return ResultMaterials;
}

TArray<FDRMaterial> UDROperationBuildingFunctionLibrary::FillWallMaterialIfNeed(const TArray<FDRMaterial>& WallMaterials)
{
	TArray<FDRMaterial> ResultMaterials;
	
	FDRMaterial EmptyWallMaterial;
	EmptyWallMaterial.ModelID = UDROperationHouseFunctionLibrary::GetConfigModelIDByType(EDConfigType::EDRWallConfig);
	EmptyWallMaterial.CraftID = 0;
	EmptyWallMaterial.RoomClassID = 0;

	for (int32 Index = 0; Index < 6; Index++)
	{
		if (WallMaterials.IsValidIndex(Index)) {
			ResultMaterials.Add(WallMaterials[Index]);
		}
		else
		{
			ResultMaterials.Add(EmptyWallMaterial);
		}
	}

	return ResultMaterials;
}
