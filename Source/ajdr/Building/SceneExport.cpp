// Fill out your copyright notice in the Description page of Project Settings.

#include "SceneExport.h"
#include "../EditorGameInstance.h"
#include "../Model/ResourceMgr.h"
#include "BuildingSystem.h"
#include "GeometryFunctionLibrary.h"
#pragma optimize( "", off )

// Sets default values
ASceneExport::ASceneExport()
{

}

// Called when the game starts or when spawned
void ASceneExport::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASceneExport::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


bool ASceneExport::Export(int SolutionID, const FString& strPath /* = "" */)
{
	UGameInstance* pInst = (GetWorld() ? GetWorld()->GetGameInstance() : nullptr);
	//UGameInstance* pInst = GetGameInstance();
	if (nullptr == pInst)
		return false;
	UCEditorGameInstance* pGameInst = Cast<UCEditorGameInstance>(pInst);
	if (nullptr == pGameInst)
		return false;
	UBuildingSystem* pBuild = pGameInst->GetBuildingSystemInstance();
	if (nullptr == pBuild)
		return false;

	IBuildingSDK *pSdk = pBuild->GetBuildingSDK();
	ISuite * pSuite = pBuild->GetSuite();
	if (nullptr == pSdk || nullptr == pSuite)
		return false;
	IDataExchange* pData = pSdk->GetDataExchange();
	if (nullptr == pData)
		return false;
	//
	if (nullptr != pBuild->BuilldActorFactory)
	{
		const TArray<FDRPointLight> vPoint =  IBuildActorFactory::Execute_GetPointLight(pBuild->BuilldActorFactory);
		for (int i = 0; i < vPoint.Num(); i++)
		{
			pBuild->AddPointLight(vPoint[i].Location, vPoint[i].SourceRadius, vPoint[i].SoftSourceRadius, vPoint[i].SourceLength,
				vPoint[i].Intensity, vPoint[i].LightColor, vPoint[i].isCastShadows);
		}
		const TArray<FDRSpotLight> vSpot =  IBuildActorFactory::Execute_GetSpotLight(pBuild->BuilldActorFactory);
		for (int i = 0; i < vSpot.Num(); i++)
		{
			pBuild->AddSpotLight(vSpot[i].Location, vSpot[i].Rotation, vSpot[i].AttenuationRadius, vSpot[i].SourceRadius,
				vSpot[i].SoftSourceRadius, vSpot[i].SourceLength, vSpot[i].InnerConeAngle, vSpot[i].OuterConeAngle, 
				vSpot[i].Intensity, vSpot[i].LightColor, vSpot[i].isCastShadows);
		}
	}
	//
	const FString strDire = FPaths::ProjectSavedDir() / TEXT("Autosaves") / TEXT("SceneDataForAR");
	const FString strFile = strDire / TEXT("SceneDataForAR.json");
	if (FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*strDire))
	{
		FPlatformFileManager::Get().GetPlatformFile().DeleteDirectoryRecursively(*strDire);
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*strDire);
	}
	//
	TSharedPtr<FJsonObject> jsRoot = MakeShareable(new FJsonObject);
	jsRoot->SetStringField("SceneName", "AJ AR Scene");
	jsRoot->SetStringField("Version", "1.0");
	jsRoot->SetNumberField("SolutionID", pGameInst->GetCurrentSolutionID());
	TArray< TSharedPtr<FJsonValue> > nodeList;
	//
	IObject ** objList = nullptr;
	const int cnt = pSuite->GetAllObjects(objList);
	if (cnt <= 0 || nullptr == objList)
		return false;
	FString strResId,strType;
	int num = 0;
	for (int i = 0; i < cnt; i++)
	{
		IObject * pObj = objList[i];
		if (nullptr == pObj)
			continue;
		//
		const EObjectType type = pObj->GetType();
		if (EModelInstance == type)
			strType = "ModelInstance";
		else if (ESolidWall == type)
			strType = "SolidWall";
		else if (EFloorPlane == type)
			strType = "FloorPlane";
		else if (ECeilPlane == type)
			strType = "CeilPlane";
		//
		else if (EPointLight == type)
			strType = "PointLight";
		else if (ESpotLight == type)
			strType = "SpotLight";
		else if (ESkyLight == type)
			strType = "SkyLight";
		//
		else
			strType = "UnknownObject";
		//
		strResId = pData->ExportObjectToMX(pObj, TCHAR_TO_ANSI(*strDire));
		if (strResId.IsEmpty() && (EPointLight != type && ESpotLight != type && ESkyLight != type))
			continue;
		//
		TSharedPtr<FJsonObject> node = MakeShareable(new FJsonObject);
		node->SetStringField("ObjectType", strType);
		if(!strResId.IsEmpty())
			node->SetStringField("ResourceID", strResId);

		switch (type)
		{
		case EModelInstance:
		case ESolidWall:
		case EFloorPlane:
		case ECeilPlane:
		{
			const kXform mat = pObj->GetTransform();
			const kVector3D vPos = mat.Location;
			const kRotation vRot = mat.Rotation;
			const kVector3D vSca = mat.Scale;

			TSharedPtr<FJsonObject> pos = MakeShareable(new FJsonObject);
			pos->SetNumberField("x", vPos.X);
			pos->SetNumberField("y", vPos.Y);
			pos->SetNumberField("z", vPos.Z);

			TSharedPtr<FJsonObject> rot = MakeShareable(new FJsonObject);
			rot->SetNumberField("pitch", vRot.Pitch);
			rot->SetNumberField("yaw", vRot.Yaw);
			rot->SetNumberField("roll", vRot.Roll);

			TSharedPtr<FJsonObject> sca = MakeShareable(new FJsonObject);
			sca->SetNumberField("x", vSca.X);
			sca->SetNumberField("y", vSca.Y);
			sca->SetNumberField("z", vSca.Z);

			node->SetObjectField("Position", pos);
			node->SetObjectField("Rotation", rot);
			node->SetObjectField("Scale", sca);

			break;
		}
		case EPointLight:
		{
			kColor vClr = pObj->GetPropertyValue("LightColor").ColorValue();
			bool vIsCastShadow = pObj->GetPropertyValue("IsCastShadow").BoolValue();
			bool vIsVisible = pObj->GetPropertyValue("IsVisible").BoolValue();
			kVector3D vPos = pObj->GetPropertyValue("Location").Vec3Value();
			float vRadius = pObj->GetPropertyValue("SourceRadius").FloatValue();
			float vSoftRadius = pObj->GetPropertyValue("SoftSourceRadius").FloatValue();
			float vLength = pObj->GetPropertyValue("SourceLength").FloatValue();
			float vAtten = pObj->GetPropertyValue("AttenuationRadius").FloatValue();
			float vIntensity = pObj->GetPropertyValue("Intensity").FloatValue();
			float vShadowResolutionScale = pObj->GetPropertyValue("ShadowResolutionScale").FloatValue();
			bool vIsAutoPoint = pObj->GetPropertyValue("IsAutoPoint").BoolValue();

			TSharedPtr<FJsonObject> pos = MakeShareable(new FJsonObject);
			pos->SetNumberField("x", vPos.X);
			pos->SetNumberField("y", vPos.Y);
			pos->SetNumberField("z", vPos.Z);
			TSharedPtr<FJsonObject> clr = MakeShareable(new FJsonObject);
			clr->SetNumberField("r", vClr.R);
			clr->SetNumberField("g", vClr.G);
			clr->SetNumberField("b", vClr.B);

			node->SetObjectField("Color", clr);
			node->SetBoolField("IsCastShadow", vIsCastShadow);
			node->SetBoolField("IsVisible", vIsVisible);
			node->SetObjectField("Position", pos);
			node->SetNumberField("Radius", vRadius);
			node->SetNumberField("SoftRadius", vSoftRadius);
			node->SetNumberField("Length", vLength);
			node->SetNumberField("Attenuation", vAtten);
			node->SetNumberField("Intensity", vIntensity);
			node->SetNumberField("ShadowResolutionScale", vShadowResolutionScale);
			node->SetBoolField("IsAutoPoint", vIsAutoPoint);

			break;
		}

		case ESpotLight:
		{
			kColor vClr = pObj->GetPropertyValue("LightColor").ColorValue();
			bool vIsCastShadow = pObj->GetPropertyValue("IsCastShadow").BoolValue();
			bool vIsVisible = pObj->GetPropertyValue("IsVisible").BoolValue();
			kVector3D vPos = pObj->GetPropertyValue("Location").Vec3Value();
			kRotation vRot = pObj->GetPropertyValue("Rotation").RotationValue();
			float vRadius = pObj->GetPropertyValue("SourceRadius").FloatValue();
			float vSoftRadius = pObj->GetPropertyValue("SoftSourceRadius").FloatValue();
			float vLength = pObj->GetPropertyValue("SourceLength").FloatValue();
			float vAtten = pObj->GetPropertyValue("AttenuationRadius").FloatValue();
			float vIntensity = pObj->GetPropertyValue("Intensity").FloatValue();
			float vShadowResolutionScale = pObj->GetPropertyValue("ShadowResolutionScale").FloatValue();
			float vInner = pObj->GetPropertyValue("InnerConeAngle").FloatValue();
			float vOuter = pObj->GetPropertyValue("OuterConeAngle").FloatValue();

			TSharedPtr<FJsonObject> pos = MakeShareable(new FJsonObject);
			pos->SetNumberField("x", vPos.X);
			pos->SetNumberField("y", vPos.Y);
			pos->SetNumberField("z", vPos.Z);
			TSharedPtr<FJsonObject> clr = MakeShareable(new FJsonObject);
			clr->SetNumberField("r", vClr.R);
			clr->SetNumberField("g", vClr.G);
			clr->SetNumberField("b", vClr.B);
			TSharedPtr<FJsonObject> dir = MakeShareable(new FJsonObject);
			dir->SetNumberField("x", vRot.Pitch);
			dir->SetNumberField("y", vRot.Yaw);
			dir->SetNumberField("z", vRot.Roll);

			node->SetObjectField("Color", clr);
			node->SetBoolField("IsCastShadow", vIsCastShadow);
			node->SetBoolField("IsVisible", vIsVisible);
			node->SetObjectField("Position", pos);
			node->SetObjectField("Rotation", dir);
			node->SetNumberField("Radius", vRadius);
			node->SetNumberField("SoftRadius", vSoftRadius);
			node->SetNumberField("Length", vLength);
			node->SetNumberField("Attenuation", vAtten);
			node->SetNumberField("Intensity", vIntensity);
			node->SetNumberField("ShadowResolutionScale", vShadowResolutionScale);
			node->SetNumberField("InnerAngle", vInner);
			node->SetNumberField("OuterAngle", vOuter);

			break;
		}
		case ESkyLight:
		{
			kColor vClr = pObj->GetPropertyValue("SkyLightColor").ColorValue();
			bool vIsCastShadow = pObj->GetPropertyValue("IsCastShadow").BoolValue();
			bool vIsVisible = pObj->GetPropertyValue("IsVisible").BoolValue();
			kRotation vRot = pObj->GetPropertyValue("DirectionLightRotation").RotationValue();
			kColor vDirClr = pObj->GetPropertyValue("DirectionLightColor").ColorValue();
			float vIntensity = pObj->GetPropertyValue("SkyLightIntensity").FloatValue();
			float vSkyLightAngle = pObj->GetPropertyValue("SkyLightAngle").FloatValue();

			TSharedPtr<FJsonObject> clr = MakeShareable(new FJsonObject);
			clr->SetNumberField("r", vClr.R);
			clr->SetNumberField("g", vClr.G);
			clr->SetNumberField("b", vClr.B);
			TSharedPtr<FJsonObject> dir = MakeShareable(new FJsonObject);
			dir->SetNumberField("x", vRot.Pitch);
			dir->SetNumberField("y", vRot.Yaw);
			dir->SetNumberField("z", vRot.Roll);
			TSharedPtr<FJsonObject> dirClr = MakeShareable(new FJsonObject);
			clr->SetNumberField("r", vDirClr.R);
			clr->SetNumberField("g", vDirClr.G);
			clr->SetNumberField("b", vDirClr.B);

			node->SetObjectField("SkyLightColor", clr);
			node->SetBoolField("IsCastShadow", vIsCastShadow);
			node->SetBoolField("IsVisible", vIsVisible);
			node->SetObjectField("DirectionLightRotation", dir);
			node->SetObjectField("DirectionLightColor", dirClr);
			node->SetNumberField("Intensity", vIntensity);
			node->SetNumberField("SkyLightAngle", vSkyLightAngle);

			break;
		}
		}
		//
		nodeList.Add(MakeShareable(new FJsonValueObject(node)));
	}
	jsRoot->SetArrayField("Nodes", nodeList);

	//
	FArchive* const FileAr = IFileManager::Get().CreateFileWriter(*strFile, FILEWRITE_EvenIfReadOnly);
	if (FileAr)
	{
		TSharedRef<TJsonWriter< TCHAR, TPrettyJsonPrintPolicy<TCHAR> >> Writer = TJsonWriterFactory< TCHAR, TPrettyJsonPrintPolicy<TCHAR> >::Create(FileAr);
		FJsonSerializer::Serialize(jsRoot.ToSharedRef(), Writer);
		FileAr->Close();
		return true;
	}
	//
	return false;
}

bool ASceneExport::Upload()
{
	return false;
}





#pragma optimize( "", on )