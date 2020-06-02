// Fill out your copyright notice in the Description page of Project Settings.

#include "FeatureWalls.h"
#include "../EditorGameInstance.h"
#include "../Model/ResourceMgr.h"
#include "BuildingSystem.h"
#include "GeometryFunctionLibrary.h"
#pragma optimize( "", off )

// Sets default values
float UFeatureWall::EPSILON0 = 0.1f;
float UFeatureWall::EPSILON1 = 0.1f;
UFeatureWall::TempData UFeatureWall::gpData;

UFeatureWall::UFeatureWall()
{
	gpData.mpSuite = nullptr;
	gpData.mvWallID.empty();
	gpData.mvWallInfo.Empty();
	gpData.mvOutInfo.Empty();
	//
	gpData.mpWorld = nullptr;
}

TArray<UFeatureWall::WallInfo> UFeatureWall::GetWallInfo(UWorld * mpWorld, int RoomID)
{
	TArray<UFeatureWall::WallInfo> v;
	gpData.mpWorld = mpWorld;
	InitData(RoomID);
	v = gpData.mvOutInfo;
	return v;
}

bool UFeatureWall::SelectWall(TArray<UFeatureWall::WallInfo>& vOutInfo, const FVector2D &Location, const TArray<UFeatureWall::WallInfo>& vWallInfo)
{
	return false;
}

UFeatureWall* UFeatureWall::Singleton()
{
	static UFeatureWall* gFeatureWall = NewObject<UFeatureWall>();
	return gFeatureWall;
}

void UFeatureWall::SetWorld(UWorld *pWorld)
{
	gpData.mpWorld = pWorld;
}

bool UFeatureWall::Select(const FVector2D &Location)
{
	if (nullptr == gpData.mpWorld)
		return false;
	bool bOk = false;
	kPoint pt(Location.X, Location.Y);
	const ObjectID id = gpData.mpSuite->HitTest(pt);
	IObject* obj = gpData.mpSuite->GetObject(id);
	if (nullptr != obj && (obj->GetType() == EWall || obj->GetType() == ESolidWall))
	{
		UFeatureWall::WallInfoEx* cur = gpData.mvWallInfo.FindByKey<int>(id);
		if (cur == nullptr)
			return false;
		const UFeatureWall::WallInfo info = cur->GetInfo();
		bool hasEle = false;
		//for (int i = 0; i < mvOutInfo.Num(); i++)
		//{
		//	if (info == mvOutInfo[i])
		//	{
		//		hasEle = true;
		//		break;
		//	}
		//}
		if(!hasEle)
			gpData.mvOutInfo.Add(info);
		//int32 idx = 0;
		//if(!mvOutInfo.Find(info, idx))
		//	mvOutInfo.Add(info);

		return true;
	}
	return false;
}

const TArray<UFeatureWall::WallInfo>& UFeatureWall::GetInfo() const
{
	return gpData.mvOutInfo;
}

bool UFeatureWall::InitData(int RoomID)
{
	//if (nullptr == mpSuite)
	{
		//UWorld *pWorld = GetWorld();
		if (nullptr == gpData.mpWorld)
			return false;
		UGameInstance* pInst = gpData.mpWorld->GetGameInstance();
		//UGameInstance* pInst = (GetWorld() ? GetWorld()->GetGameInstance() : nullptr);
		if (nullptr == pInst)
			return false;
		UCEditorGameInstance* pGameInst = Cast<UCEditorGameInstance>(pInst);
		if (nullptr == pGameInst)
			return false;
		UBuildingSystem* pBuild = pGameInst->GetBuildingSystemInstance();
		if (nullptr == pBuild)
			return false;
		ISuite * pSuite = pBuild->GetSuite();
		if (nullptr == pSuite)
			return false;

		//IBuildingSDK *pSdk = pBuild->GetBuildingSDK();
		//if (nullptr == pSdk)
		//	return false;
		//IDataExchange* pData = pSdk->GetDataExchange();
		//if (nullptr == pData)
		//	return false;
		//IObject ** objList = nullptr;
		//const int cnt = pSuite->GetAllObjects(objList);
		//if (cnt <= 0 || nullptr == objList)
		//	return false;

		gpData.mpSuite = pSuite;
	}

	gpData.mvWallID.empty();
	gpData.mvWallRaw.Empty();
	gpData.mvWallInfo.Empty();
	gpData.mvOutInfo.Empty();

	kArray<ObjectID> Edges;
	kArray<unsigned char> InverseFlags;
	//gpData.mvWallID = gpData.mpSuite->GetEdgeBoundary(RoomID, Edges, InverseFlags);
	const bool bTemp = gpData.mpSuite->GetEdgeBoundary(RoomID, gpData.mvWallID, gpData.mvInverse);
	if (!bTemp || gpData.mvWallID.size() <= 0)
		return false;

	for (int i = 0; i < gpData.mvWallID.size(); i++)
	{
		UFeatureWall::WallInfoEx info;
		if (ProceWall(gpData.mpSuite, info, gpData.mvWallID[i], gpData.mvInverse[i]))
		{
			info.index = gpData.mvWallInfo.Num();
			gpData.mvWallRaw.Add(info);
		}
		else
		{
			continue;
		}
		//
		if (gpData.mvWallInfo.Num() > 0)
		{
			UFeatureWall::WallInfoEx* pre = &gpData.mvWallInfo[gpData.mvWallInfo.Num() - 1];
			if(!Combine(pre,info))
			{
				gpData.mvWallInfo.Add(info);
			}
		}
		else
		{
			gpData.mvWallInfo.Add(info);
		}
		//
		if (i + 1 >= gpData.mvWallID.size())
		{
			const int cnt = gpData.mvWallInfo.Num();
			if (cnt >= 2)
			{
				UFeatureWall::WallInfoEx* w0 = &gpData.mvWallInfo[0];
				UFeatureWall::WallInfoEx w1 = gpData.mvWallInfo[cnt - 1];
				if (Combine(w0, w1))
				{
					gpData.mvWallInfo.Remove(w1);
				}
			}
		}
	}

	//test
	tempCopy();
	
	return true;
}

void UFeatureWall::tempCopy()
{
	gpData.mvOutInfo.Empty();
	for (int i = 0; i < gpData.mvWallInfo.Num(); i++)
	{
		gpData.mvOutInfo.Add(gpData.mvWallInfo[i].GetInfo());
	}
}

bool UFeatureWall::ProceWall(ISuite * pSuite, UFeatureWall::WallInfoEx & info, int id, bool bInverse)
{
	if (nullptr == pSuite)
		return false;

	IObject* pObj = nullptr;
	pObj = pSuite->GetObject(id);
	if (nullptr == pObj)
		return false;
	bool bOk = false;
	info.id = pObj->GetID();
	info.type = pObj->GetType();
	const EObjectType type = pObj->GetType();
	if (ESolidWall == type)
	{
		IValue& vHole = pObj->GetPropertyValue("Holes");
		const int cnt = vHole.GetArrayCount();
		for (int i = 0; i < cnt; i++)
		{
			WallAnnex hole;
			IValue& val = vHole.GetField(i);
			const EObjectType objType = (EObjectType)(val.GetField("ObjType").IntValue());
			const int objID = val.GetField("HoleID").IntValue();
			IObject* obj = pSuite->GetObject(objID);
			if (nullptr == obj)
				continue;
			if (obj->IsA(EWallHole))// objType == EWallHole || objType == EDoorHole || objType == EWindow)
			{
				hole.type = obj->GetType();
				hole.width = obj->GetPropertyValue("Width").FloatValue();
				hole.height = obj->GetPropertyValue("Height").FloatValue();
				hole.thickness = obj->GetPropertyValue("Thickness").FloatValue();
				hole.zPos = obj->GetPropertyValue("ZPos").FloatValue();
				hole.location = obj->GetTransform().Location;
				hole.rotation = obj->GetTransform().Rotation;
				hole.scaling = obj->GetTransform().Scale;
				//
				info.annex.Add(hole);
			}
		}
			
	}
	if (EWall == type || ESolidWall == type)
	{
		kLine2D CenterLine, LeftLine, RightLine;
		const bool b0 = pSuite->GetWallBorderLines(pObj->GetID(), CenterLine, LeftLine, RightLine);
		const bool b1 = pSuite->GetWallVector(pObj->GetID(), info.start, info.end, info.forward);
		if (b0)
		{
			FVector dir(info.end.X - info.start.X, info.end.Y - info.start.Y, 0.f);
			FVector up(0.f, 0.f, 1.f);
			FVector cross = FVector::CrossProduct(up, dir);
			cross.Normalize();
			FVector fw(info.forward.X, info.forward.Y, 0.f);
			fw.Normalize();
			float dot = FVector::DotProduct(cross, fw);
			//if (bInverse)(cross.Z >= 0.f)//(dot>0.f)////cross.Z == 0.f -> right
			//{
			//	info.start = RightLine.start;
			//	info.end = RightLine.end;
			//}
			//else
			//{
			//	info.start = LeftLine.start;
			//	info.end = LeftLine.end;
			//}
			if (bInverse)
			{
				info.start = LeftLine.start;
				info.end = LeftLine.end;
			}
			else
			{
				info.start = RightLine.start;
				info.end = RightLine.end;
			}
			info.inverse = bInverse;
		}
		else
		{
			int iiii = 0;
			return false;
		}
		//bOk = (b0 && b1);
		bOk = b1;
	}
	//
	return bOk;
}

bool UFeatureWall::Combine(UFeatureWall::WallInfoEx* pWall, const UFeatureWall::WallInfoEx& temp)
{
	if (nullptr == pWall)
		return false;
	bool bOk = false;
	int flag = -1;
	if (pWall->IsLine(temp, &flag))
	{
		if (0 == flag)
		{
			pWall->end = temp.end;
		}
		else if(1 == flag)
		{
			pWall->start = temp.start;
		}
		else if (2 == flag)
		{
			pWall->start = temp.end;
		}
		else if (3 == flag)
		{
			pWall->end = temp.start;
		}
		//
		pWall->annex.Append(temp.annex);
		pWall->vID.Add(temp.id);
		pWall->vID.Append(temp.vID);
		//
		bOk = true;
	}
	return bOk;
}

void UFeatureWall::DrawEdge(float Lifetime /* = 10.f */, float Thickness /* = 10.f */) const
{
	if (gpData.mvWallInfo.Num() <= 0 || nullptr == gpData.mpWorld)
		return;
	FColor clrTemp[] = {
		FColor::Yellow,
		FColor::Red,
		FColor::Green,
		FColor::Blue,
		FColor::Purple,
		FColor::Cyan
	};
	const int clrNum = sizeof(clrTemp)/sizeof(clrTemp[0]);
	for (int i = 0; i < gpData.mvWallInfo.Num(); i++)
	{
		const WallInfo info = gpData.mvWallInfo[i];
		FVector v0(info.start.X, info.start.y, 0);
		FVector v1(info.end.X, info.end.y, 0);
		FColor clr = FColor::Red;
		//float f = (float)i / (float)mvWallInfo.Num();
		//clr += FColor(64 * f, 128 * f, 255 * f);
		if (i < clrNum)
			clr = clrTemp[i];
		else
			clr = clrTemp[i%clrNum+1];
		DrawDebugLine(gpData.mpWorld, v0, v1, clr, false, Lifetime, 0, Thickness*2.f);
		FVector cen = (v0 + v1)*0.5f;
		FVector nor = cen + FVector(info.forward.X,info.forward.Y,0.f) * 100.f;
		DrawDebugLine(gpData.mpWorld, cen, nor, clr, false, Lifetime, 0, Thickness);
		
		//char sz[256];
		//char sub[256];
		//printf_s(sz, "\r\nannex num: %d .\r\n", info.annex.Num());
		//FString msg(sz);
		//for (int a = 0; a < info.annex.Num(); a++)
		//{
		//	printf_s(sub, "%d: ", a);
		//	msg += sub;
		//	if (info.annex[a].type == EDoorHole)
		//		msg += "EDoorHole";
		//	else if (info.annex[a].type == EWindow)
		//		msg += "EWindow";
		//	msg += "  ----  ";
		//}
		//msg += "\r\n";
		//UE_LOG(LogTemp, Log, TEXT("%s"), *msg);
	}
}

//bool UFeatureWall::ProceWalls(const TArray<int>& vID)
//{
//	UGameInstance* pInst = (GetWorld() ? GetWorld()->GetGameInstance() : nullptr);
//	if (nullptr == pInst)
//		return false;
//	UCEditorGameInstance* pGameInst = Cast<UCEditorGameInstance>(pInst);
//	if (nullptr == pGameInst)
//		return false;
//	UBuildingSystem* pBuild = pGameInst->GetBuildingSystemInstance();
//	if (nullptr == pBuild)
//		return false;
//
//	IBuildingSDK *pSdk = pBuild->GetBuildingSDK();
//	ISuite * pSuite = pBuild->GetSuite();
//	if (nullptr == pSdk || nullptr == pSuite)
//		return false;
//	IDataExchange* pData = pSdk->GetDataExchange();
//	if (nullptr == pData)
//		return false;
//
//	if (vID.Num() <= 0)
//		return false;
//
//	mvWallInfo.Reset(vID.Num());
//	IObject* pObj = nullptr;
//	for (int i = 0; i < vID.Num(); i++)
//	{
//		pObj = pSuite->GetObject(vID[i]);
//		if (nullptr == pObj)
//			continue;
//		UFeatureWall::WallInfoEx info;
//		info.index = i;
//		info.id = pObj->GetID();
//		info.type = pObj->GetType();
//		const EObjectType type = pObj->GetType();
//		if (ESolidWall == type)
//		{
//			IValue& val = pObj->GetPropertyValue("Holes");
//			info.hasHoles = !val.IsNil();
//			const EVarType valType = val.GetType();
//			//val.GetField()
//		}
//		if (EWall == type || ESolidWall == type)
//		{
//			info.forward = pObj->GetPropertyValue("Forward").Vec3Value();
//			ObjectID idP0 = pObj->GetPropertyValue("P0").GetNumFields();
//			ObjectID idP1 = pObj->GetPropertyValue("P1").GetNumFields();
//			IObject* corner = nullptr;
//			IObject* pStart = pSuite->GetObject(idP0);
//			IObject* pEnd = pSuite->GetObject(idP1);
//			if (nullptr != pStart && nullptr != pEnd)
//			{
//				info.start = pStart->GetPropertyValue("Location").Vec2Value();
//				info.end = pEnd->GetPropertyValue("Location").Vec2Value();
//			}
//
//			////kVector3D vPos = pObj->GetPropertyValue("Location").Vec3Value();
//			//const kXform form = pObj->GetWorldTransform();
//			//info.location = form.Location;
//			//info.rotation = form.Rotation;
//			//info.scaling = form.Scale;
//			//
//			mvWallInfo.Add(info);
//		}
//	}
//	//
//	return true;
//}

//void UFeatureWall::Sort()
//{
//	if (mvWallInfo.Num() <= 0)
//		return;
//	TArray<UFeatureWall::WallInfoEx> vTemp = mvWallInfo;
//	TArray<bool> flagx;
//	for (int i = 0; i < vTemp.Num(); i++)
//		flagx.Add(false);
//	bool flag = false;
//	mvWallInfo.Reset(vTemp.Num());
//	UFeatureWall::WallInfoEx temp = vTemp[0];
//	mvWallInfo.Add(temp);
//	while(vTemp.Num() > 0)//for (int i = 0; i < vTemp.Num(); i++)
//	{
//		flag = false;
//		for (int j = 0; j < vTemp.Num(); j++)
//		{
//			if (temp == vTemp[j])
//				continue;
//			float val = 0.f;
//			if (temp.IsParallel(vTemp[j], val))//(temp.end.Equals(vTemp[j].start)) // (vTemp[i].end == vTemp[j].start)
//			{
//				temp = vTemp[j];
//				UFeatureWall::WallInfoEx tmp = vTemp[j];
//				tmp.index = mvWallInfo.Num();
//				mvWallInfo.Add(tmp);
//				vTemp.Remove(vTemp[j]);
//				flag = true;
//				break;
//			}
//		}
//		if (!flag)
//		{
//
//		}
//	}
//}

// bool UFeatureWall::GetWalls(int RoomID)
// {
//	 UGameInstance* pInst = (GetWorld() ? GetWorld()->GetGameInstance() : nullptr);
//	 if (nullptr == pInst)
//		 return false;
//	 UCEditorGameInstance* pGameInst = Cast<UCEditorGameInstance>(pInst);
//	 if (nullptr == pGameInst)
//		 return false;
//	 UBuildingSystem* pBuild = pGameInst->GetBuildingSystemInstance();
//	 if (nullptr == pBuild)
//		 return false;
//
//	 IBuildingSDK *pSdk = pBuild->GetBuildingSDK();
//	 ISuite * pSuite = pBuild->GetSuite();
//	 if (nullptr == pSdk || nullptr == pSuite)
//		 return false;
//	 IDataExchange* pData = pSdk->GetDataExchange();
//	 if (nullptr == pData)
//		 return false;
//
//
//	 IObject ** objList = nullptr;
//	 const int cnt = pSuite->GetAllObjects(objList);
//	 if (cnt <= 0 || nullptr == objList)
//		 return false;
//	 //IObject* pRoom = pSuite->GetObject(RoomID);
//	 kArray<ObjectID> vWall = pSuite->GetEdgeBoundary(RoomID);
//	 if (vWall.size() <= 0)
//		 return false;
//	 TArray<int> vID;
//	 for (int i = 0; i < vWall.size(); i++)
//		 vID.Add(vWall[i]);
//
//	 return ProceWalls(vID);
//}

// // Called when the game starts or when spawned
// void FeatureWall::BeginPlay()
// {
	// Super::BeginPlay();
// }

// // Called every frame
// void FeatureWall::Tick(float DeltaTime)
// {
	// Super::Tick(DeltaTime);
// }

// void FeatureWall::Open(int SolutionID)
// {

// }

// bool FeatureWall::Export(int SolutionID, const FString& strPath /* = "" */)
// {
	// UGameInstance* pInst = (GetWorld() ? GetWorld()->GetGameInstance() : nullptr);
	// //UGameInstance* pInst = GetGameInstance();
	// if (nullptr == pInst)
		// return false;
	// UCEditorGameInstance* pGameInst = Cast<UCEditorGameInstance>(pInst);
	// if (nullptr == pGameInst)
		// return false;
	// UBuildingSystem* pBuild = pGameInst->GetBuildingSystemInstance();
	// if (nullptr == pBuild)
		// return false;

	// IBuildingSDK *pSdk = pBuild->GetBuildingSDK();
	// ISuite * pSuite = pBuild->GetSuite();
	// if (nullptr == pSdk || nullptr == pSuite)
		// return false;
	// IDataExchange* pData = pSdk->GetDataExchange();
	// if (nullptr == pData)
		// return false;
	// //
	// if (nullptr != pBuild->BuilldActorFactory)
	// {
		// const TArray<FDRPointLight> vPoint =  IBuildActorFactory::Execute_GetPointLight(pBuild->BuilldActorFactory);
		// for (int i = 0; i < vPoint.Num(); i++)
		// {
			// pBuild->AddPointLight(vPoint[i].Location, vPoint[i].SourceRadius, vPoint[i].SoftSourceRadius, vPoint[i].SourceLength,
				// vPoint[i].Intensity, vPoint[i].LightColor, vPoint[i].isCastShadows);
		// }
		// const TArray<FDRSpotLight> vSpot =  IBuildActorFactory::Execute_GetSpotLight(pBuild->BuilldActorFactory);
		// for (int i = 0; i < vSpot.Num(); i++)
		// {
			// pBuild->AddSpotLight(vSpot[i].Location, vSpot[i].Rotation, vSpot[i].AttenuationRadius, vSpot[i].SourceRadius,
				// vSpot[i].SoftSourceRadius, vSpot[i].SourceLength, vSpot[i].InnerConeAngle, vSpot[i].OuterConeAngle, 
				// vSpot[i].Intensity, vSpot[i].LightColor, vSpot[i].isCastShadows);
		// }
	// }
	// //
	// const FString strDire = (strPath.IsEmpty()? (FPaths::GameSavedDir() / TEXT("Autosaves") / TEXT("SceneDataForAR")) : strPath);
	// const FString strFile = strDire / TEXT("SceneDataForAR.json");
	// if (FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*strDire))
	// {
		// FPlatformFileManager::Get().GetPlatformFile().DeleteDirectoryRecursively(*strDire);
		// FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*strDire);
	// }
	// //
	// TSharedPtr<FJsonObject> jsRoot = MakeShareable(new FJsonObject);
	// jsRoot->SetStringField("SceneName", "AJ AR Scene");
	// jsRoot->SetStringField("Version", "1.0");
	// jsRoot->SetNumberField("SolutionID", pGameInst->GetCurrentSolutionID());
	// TArray< TSharedPtr<FJsonValue> > nodeList;
	// //
	// IObject ** objList = nullptr;
	// const int cnt = pSuite->GetAllObjects(objList);
	// if (cnt <= 0 || nullptr == objList)
		// return false;
	// FString strResId,strType;
	// int num = 0;
	// for (int i = 0; i < cnt; i++)
	// {
		// IObject * pObj = objList[i];
		// if (nullptr == pObj)
			// continue;
		// //
		// const EObjectType type = pObj->GetType();
		// if (EModelInstance == type)
			// strType = "ModelInstance";
		// else if (ESolidWall == type)
			// strType = "SolidWall";
		// else if (EFloorPlane == type)
			// strType = "FloorPlane";
		// else if (ECeilPlane == type)
			// strType = "CeilPlane";
		// //
		// else if (EPointLight == type)
			// strType = "PointLight";
		// else if (ESpotLight == type)
			// strType = "SpotLight";
		// else if (ESkyLight == type)
			// strType = "SkyLight";
		// //
		// else
			// strType = "UnknownObject";
		// //
		// strResId = pData->ExportObject(pObj, TCHAR_TO_ANSI(*strDire));
		// if (strResId.IsEmpty() && (EPointLight != type && ESpotLight != type && ESkyLight != type))
			// continue;
		// //
		// TSharedPtr<FJsonObject> node = MakeShareable(new FJsonObject);
		// node->SetStringField("ObjectType", strType);
		// if(!strResId.IsEmpty())
			// node->SetStringField("ResourceID", strResId);

		// switch (type)
		// {
		// case EModelInstance:
		// case ESolidWall:
		// case EFloorPlane:
		// case ECeilPlane:
		// {
			// const kMatrix43 mat = pObj->GetWorldMatrix();
			// const TVector3DTemp<float> vPos = mat.GetOrigin();
			// const TVector3DTemp<float> vRot = mat.GetRotationXYZ();
			// const TVector3DTemp<float> vSca = mat.GetScale();

			// TSharedPtr<FJsonObject> pos = MakeShareable(new FJsonObject);
			// pos->SetNumberField("x", vPos.X);
			// pos->SetNumberField("y", vPos.Y);
			// pos->SetNumberField("z", vPos.Z);

			// TSharedPtr<FJsonObject> rot = MakeShareable(new FJsonObject);
			// rot->SetNumberField("x", vRot.X);
			// rot->SetNumberField("y", vRot.Y);
			// rot->SetNumberField("z", vRot.Z);

			// TSharedPtr<FJsonObject> sca = MakeShareable(new FJsonObject);
			// sca->SetNumberField("x", vSca.X);
			// sca->SetNumberField("y", vSca.Y);
			// sca->SetNumberField("z", vSca.Z);

			// node->SetObjectField("Position", pos);
			// node->SetObjectField("Rotation", rot);
			// node->SetObjectField("Scaling", sca);

			// break;
		// }
		
		// case EPointLight:
		// {
			// kColor vClr = pObj->GetPropertyValue("LightColor").ColorValue();
			// bool vIsCastShadow = pObj->GetPropertyValue("IsCastShadow").BoolValue();
			// bool vIsVisible = pObj->GetPropertyValue("IsVisible").BoolValue();
			// kVector3D vPos = pObj->GetPropertyValue("Location").Vec3Value();
			// float vRadius = pObj->GetPropertyValue("SourceRadius").FloatValue();
			// float vSoftRadius = pObj->GetPropertyValue("SoftSourceRadius").FloatValue();
			// float vLength = pObj->GetPropertyValue("SourceLength").FloatValue();
			// float vAtten = pObj->GetPropertyValue("AttenuationRadius").FloatValue();
			// float vIntensity = pObj->GetPropertyValue("Intensity").FloatValue();
			// float vShadowResolutionScale = pObj->GetPropertyValue("ShadowResolutionScale").FloatValue();
			// bool vIsAutoPoint = pObj->GetPropertyValue("IsAutoPoint").BoolValue();

			// TSharedPtr<FJsonObject> pos = MakeShareable(new FJsonObject);
			// pos->SetNumberField("x", vPos.X);
			// pos->SetNumberField("y", vPos.Y);
			// pos->SetNumberField("z", vPos.Z);
			// TSharedPtr<FJsonObject> clr = MakeShareable(new FJsonObject);
			// clr->SetNumberField("r", vClr.R);
			// clr->SetNumberField("g", vClr.G);
			// clr->SetNumberField("b", vClr.B);

			// node->SetObjectField("Color", clr);
			// node->SetBoolField("IsCastShadow", vIsCastShadow);
			// node->SetBoolField("IsVisible", vIsVisible);
			// node->SetObjectField("Position", pos);
			// node->SetNumberField("Radius", vRadius);
			// node->SetNumberField("SoftRadius", vSoftRadius);
			// node->SetNumberField("Length", vLength);
			// node->SetNumberField("Attenuation", vAtten);
			// node->SetNumberField("Intensity", vIntensity);
			// node->SetNumberField("ShadowResolutionScale", vShadowResolutionScale);
			// node->SetBoolField("IsAutoPoint", vIsAutoPoint);

			// break;
		// }

		// case ESpotLight:
		// {
			// kColor vClr = pObj->GetPropertyValue("LightColor").ColorValue();
			// bool vIsCastShadow = pObj->GetPropertyValue("IsCastShadow").BoolValue();
			// bool vIsVisible = pObj->GetPropertyValue("IsVisible").BoolValue();
			// kVector3D vPos = pObj->GetPropertyValue("Location").Vec3Value();
			// kRotation vRot = pObj->GetPropertyValue("Rotation").RotationValue();
			// float vRadius = pObj->GetPropertyValue("SourceRadius").FloatValue();
			// float vSoftRadius = pObj->GetPropertyValue("SoftSourceRadius").FloatValue();
			// float vLength = pObj->GetPropertyValue("SourceLength").FloatValue();
			// float vAtten = pObj->GetPropertyValue("AttenuationRadius").FloatValue();
			// float vIntensity = pObj->GetPropertyValue("Intensity").FloatValue();
			// float vShadowResolutionScale = pObj->GetPropertyValue("ShadowResolutionScale").FloatValue();
			// float vInner = pObj->GetPropertyValue("InnerConeAngle").FloatValue();
			// float vOuter = pObj->GetPropertyValue("OuterConeAngle").FloatValue();

			// TSharedPtr<FJsonObject> pos = MakeShareable(new FJsonObject);
			// pos->SetNumberField("x", vPos.X);
			// pos->SetNumberField("y", vPos.Y);
			// pos->SetNumberField("z", vPos.Z);
			// TSharedPtr<FJsonObject> clr = MakeShareable(new FJsonObject);
			// clr->SetNumberField("r", vClr.R);
			// clr->SetNumberField("g", vClr.G);
			// clr->SetNumberField("b", vClr.B);
			// TSharedPtr<FJsonObject> dir = MakeShareable(new FJsonObject);
			// dir->SetNumberField("x", vRot.Pitch);
			// dir->SetNumberField("y", vRot.Yaw);
			// dir->SetNumberField("z", vRot.Roll);

			// node->SetObjectField("Color", clr);
			// node->SetBoolField("IsCastShadow", vIsCastShadow);
			// node->SetBoolField("IsVisible", vIsVisible);
			// node->SetObjectField("Position", pos);
			// node->SetObjectField("Rotation", dir);
			// node->SetNumberField("Radius", vRadius);
			// node->SetNumberField("SoftRadius", vSoftRadius);
			// node->SetNumberField("Length", vLength);
			// node->SetNumberField("Attenuation", vAtten);
			// node->SetNumberField("Intensity", vIntensity);
			// node->SetNumberField("ShadowResolutionScale", vShadowResolutionScale);
			// node->SetNumberField("InnerAngle", vInner);
			// node->SetNumberField("OuterAngle", vOuter);

			// break;
		// }
		// case ESkyLight:
		// {
			// kColor vClr = pObj->GetPropertyValue("SkyLightColor").ColorValue();
			// bool vIsCastShadow = pObj->GetPropertyValue("IsCastShadow").BoolValue();
			// bool vIsVisible = pObj->GetPropertyValue("IsVisible").BoolValue();
			// kRotation vRot = pObj->GetPropertyValue("DirectionLightRotation").RotationValue();
			// kColor vDirClr = pObj->GetPropertyValue("DirectionLightColor").ColorValue();
			// float vIntensity = pObj->GetPropertyValue("SkyLightIntensity").FloatValue();
			// float vSkyLightAngle = pObj->GetPropertyValue("SkyLightAngle").FloatValue();

			// TSharedPtr<FJsonObject> clr = MakeShareable(new FJsonObject);
			// clr->SetNumberField("r", vClr.R);
			// clr->SetNumberField("g", vClr.G);
			// clr->SetNumberField("b", vClr.B);
			// TSharedPtr<FJsonObject> dir = MakeShareable(new FJsonObject);
			// dir->SetNumberField("x", vRot.Pitch);
			// dir->SetNumberField("y", vRot.Yaw);
			// dir->SetNumberField("z", vRot.Roll);
			// TSharedPtr<FJsonObject> dirClr = MakeShareable(new FJsonObject);
			// clr->SetNumberField("r", vDirClr.R);
			// clr->SetNumberField("g", vDirClr.G);
			// clr->SetNumberField("b", vDirClr.B);

			// node->SetObjectField("SkyLightColor", clr);
			// node->SetBoolField("IsCastShadow", vIsCastShadow);
			// node->SetBoolField("IsVisible", vIsVisible);
			// node->SetObjectField("DirectionLightRotation", dir);
			// node->SetObjectField("DirectionLightColor", dirClr);
			// node->SetNumberField("Intensity", vIntensity);
			// node->SetNumberField("SkyLightAngle", vSkyLightAngle);

			// break;
		// }
		// }
		// //
		// nodeList.Add(MakeShareable(new FJsonValueObject(node)));
	// }
	// jsRoot->SetArrayField("Nodes", nodeList);

	// //
	// FArchive* const FileAr = IFileManager::Get().CreateFileWriter(*strFile, FILEWRITE_EvenIfReadOnly);
	// if (FileAr)
	// {
		// TSharedRef<TJsonWriter< TCHAR, TPrettyJsonPrintPolicy<TCHAR> >> Writer = TJsonWriterFactory< TCHAR, TPrettyJsonPrintPolicy<TCHAR> >::Create(FileAr);
		// FJsonSerializer::Serialize(jsRoot.ToSharedRef(), Writer);
		// FileAr->Close();
		// return true;
	// }
	// //
	// return false;
// }

// bool FeatureWall::Upload()
// {
	// return false;
// }

// /*
// InUrlΪ�ϴ�URL
// InFilesΪ�����ļ�·��������
// BoundaryΪ�ָ���������ָ��ļ��������ط����ܳ��ָ��ַ�����ÿ���ļ�����"\r\n--"+Boundary+"\r\n"��ʼ����BeginBoundry��
// �ָ��֮�����ļ�ͷ��FileHeader�����ļ�ͷ��nameΪ�ļ���������"file[]"��[]��ʾ�ϴ�����ļ��������һ���ļ���ȥ��[]��filenameΪ�ļ����������ñ����ļ������ļ�ͷ���������з�(\r\n\r\n)����
// �ļ�ͷ֮�����ļ�����
// �����ļ����ݶ�����֮�������"\r\n--"+Boundary+"--\r\n"����
// */
// bool FeatureWall::UploadFiles(const FString& InUrl, const TArray<FString>& InFiles)
// {
	// TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	// HttpRequest->SetURL(InUrl);

	// FString Boundary = "---------------------------" + FString::FromInt(FDateTime::Now().GetTicks());//�ָ��
	// HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary =" + Boundary));//����ͷ�����ݸ�ʽ������multipart/form-data
	// HttpRequest->SetVerb(TEXT("POST"));

	// TArray<uint8> UploadContent;
	// for (const FString& FilePath : InFiles)
	// {
		// TArray<uint8> ArrayContent;
		// if (FFileHelper::LoadFileToArray(ArrayContent, *FilePath))
		// {
			// FString BeginBoundry = "\r\n--" + Boundary + "\r\n";
			// UploadContent.Append((uint8*)TCHAR_TO_ANSI(*BeginBoundry), BeginBoundry.Len());

			// FString FileHeader = "Content-Disposition: form-data;";//�ļ�ͷ
			// FileHeader.Append("name=\"file[]\";");
			// FileHeader.Append("filename=\"" + FPaths::GetCleanFilename(FilePath) + "\"");
			// FileHeader.Append("\r\nContent-Type: \r\n\r\n");
			// UploadContent.Append((uint8*)TCHAR_TO_ANSI(*FileHeader), FileHeader.Len());

			// UploadContent.Append(ArrayContent);
		// }
	// }
	// FString EndBoundary = "\r\n--" + Boundary + "--\r\n";//������
	// UploadContent.Append((uint8*)TCHAR_TO_ANSI(*EndBoundary), EndBoundary.Len());
	// HttpRequest->SetContent(UploadContent);
	// return HttpRequest->ProcessRequest();
// }

// Sets default values
AWallWrapper::AWallWrapper()
{
	mpFeatureWall = nullptr;
}

// Called when the game starts or when spawned
void AWallWrapper::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr == mpFeatureWall)
	{
		UWorld *pWorld = GetWorld();
		mpFeatureWall = NewObject<UFeatureWall>();
		mpFeatureWall->SetWorld(pWorld);
	}
}

// Called every frame
void AWallWrapper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UFeatureWall* AWallWrapper::GetFeatureWall()
{
	return mpFeatureWall;
}



#pragma optimize( "", on )