
#include "BuildingSystem.h"
#include "ModelFile.h"
#include "ResourceMgr.h"
#include "SlateMaterialBrush.h"
#include "BuildingSDK.h"
#include "IBuildingSDK.h"
#include "IPoleEditor.h"
#include "ITubeEditor.h"
#include "IPillarEditor.h"
#include "ISewerEditor.h"
#include "Math/kMatrix43.h"
#include "Building.h"
#include "BuildingActor.h"
#include "ModelFileActor.h"
#include "AHousePluginActor.h"
#include "BuildingComponent.h"
#include <vector>
#include "BuildingActorFactory.h"
#include "SceneViewport.h"
#include "DRComponentModel.h"
#include "EditorGameInstance.h"
#include "HomeLayout/SceneEntity/FurnitureModelActor.h"
#include "HomeLayout/SceneEntity/LineWallActor.h"
#include "HomeLayout/SceneEntity/RoomActor.h"
#include "HomeLayout/SceneEntity/Wall_Boolean_Base.h"
#include "HomeLayout/DataNodes/MaterialNode.h"
#include "DRFunLibrary.h"
#include "IProperty.h"
#include "BuildingGroupMeshComponent.h"
#include "HouseComponent/ComponentManagerActor.h"
#include "IGroup.h"
#include "DRProjData.h"
#include "Data/Adapter/DRDoorAdapter.h"
#include "Data/Adapter/DRAreaAdapter.h"
#include "Data/Adapter/DRDoorHoleAdapter.h"
#include "Data/Adapter/DRWindowHoleAdapter.h"
#include "Data/Adapter/DRModelInstanceAdapter.h"
#include "Data/Adapter/DRVirtualWallAdapter.h"
#include "Data/FunctionLibrary/DROperationHouseFunctionLibrary.h"
IBuildingSDK *UBuildingSystem::BuildingSDK = nullptr;

IBuildingSDK *UBuildingSystem::GetBuildingSDK()
{
	if (!BuildingSDK)
	{
		FBuildingSDKModule &SDKModule = FModuleManager::LoadModuleChecked<FBuildingSDKModule>(FName("BuildingSDK"));
		BuildingSDK = SDKModule.GetSDK();
	}
	return BuildingSDK;
}

IValueFactory *UBuildingSystem::GetValueFactory()
{
	IBuildingSDK *SDK = GetBuildingSDK();
	if (SDK)
	{
		return SDK->GetValueFactory();
	}
	return nullptr;
}

UBuildingSystem::UBuildingSystem(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
	, Suite(nullptr)
	, Visitor(nullptr)
{
}


UBuildingSystem *UBuildingSystem::LoadNewSuite(UObject *Outer, const FString &InFilename)
{
	UBuildingSystem *NewSuite = NewObject<UBuildingSystem>(Outer);
	if (NewSuite)
	{
		NewSuite->LoadFile(InFilename);
	}
	return NewSuite;
}

UBuildingSystem *UBuildingSystem::CreateNewSuite(UObject *Outer, const FString &InFilename)
{
	IBuildingSDK *pSDK = GetBuildingSDK();
	UBuildingSystem *NewSuite = nullptr;
	if (pSDK)
	{
		NewSuite = NewObject<UBuildingSystem>(Outer);
		if (NewSuite)
		{
			NewSuite->Filename = InFilename;
			NewSuite->Suite = pSDK->CreateSuite("");
			NewSuite->Suite->SetListener(NewSuite);
		}
	}
	return NewSuite;
}

bool UBuildingSystem::LoadFile(const FString &InFilename)
{
	if (!IFileManager::Get().FileExists(*InFilename))
	{
		return false;
	}
	if (Suite)
	{
		ClearSuite();
	}

	const char *strFilename = TCHAR_TO_ANSI(*InFilename);
	IBuildingSDK *pSDK = GetBuildingSDK();
	if (pSDK)
	{
		Suite = pSDK->CreateSuite("");
		if (Suite)
		{
			Suite->Load(strFilename);
			Filename = InFilename;
			Suite->SetListener(this);
			LoadObjInfo();
			return true;
		}
	}
	return false;
}

void UBuildingSystem::SaveFile(const FString &InFilename)
{
	if (Suite)
	{
		const char *strFilename = TCHAR_TO_ANSI(*InFilename);
		Suite->Save(strFilename);
	}
}

void UBuildingSystem::ExportScene(const FString &InFilename)
{
	IBuildingSDK *pSDK = GetBuildingSDK();
	if (Suite && pSDK)
	{
		IDataExchange *pExchange = pSDK->GetDataExchange();
		if (pExchange)
		{
			const char *strFilename = TCHAR_TO_ANSI(*InFilename);
			pExchange->ExportScene(Suite, strFilename);
		}		
	}
}

void UBuildingSystem::ClearSuite()
{
	IBuildingSDK *pSDK = GetBuildingSDK();
	if (Suite && pSDK)
	{
		pSDK->DestroySuite(Suite);
		Suite = NULL;
	}

	for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
	{
		FObjectInfo &ObjInfo = It.Value();
		ClearObjInfo(&ObjInfo);
	}
	
	ObjMap.Empty();

	UDRProjData::ClearProjectDataManager();
}

void UBuildingSystem::ForceUpdateSuit()
{
	if (Suite)
	{
		Suite->Flush();
	}
}

int32 UBuildingSystem::FindHostWorld(UWorld *World)
{
	int32 FoundHostIndex = INDEX_NONE;
	if (World)
	{
		for (int32 i = HostWorlds.Num() - 1; i >= 0; --i)
		{
			UObject *Obj = HostWorlds[i].Get(true);
			if (Obj == NULL)
			{
				HostWorlds.RemoveAt(i);
			}
			else if (Obj == World)
			{
				FoundHostIndex = i;
				break;
			}
		}
	}
	return FoundHostIndex;
}

void UBuildingSystem::AddToWorld(UObject *WorldContextObject)
{
	UWorld *MyWorld = WorldContextObject ? WorldContextObject->GetWorld() : NULL;
	if (MyWorld)
	{
		int32 HostIndex = FindHostWorld(MyWorld);
		if (HostIndex==INDEX_NONE && Suite)
		{
			for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
			{
				FObjectInfo &ObjInfo = It.Value();
				SpawnActorByObject(MyWorld, ObjInfo);
			}
			HostWorlds.Add(MyWorld);
		}
	}
}

void UBuildingSystem::RemoveFromWorld(UObject *WorldContextObject)
{
	UWorld *MyWorld = WorldContextObject ? WorldContextObject->GetWorld() : NULL;
	if (MyWorld)
	{
		int32 HostIndex = FindHostWorld(MyWorld);
		if (HostIndex != INDEX_NONE)
		{
			for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
			{
				FObjectInfo &ObjInfo = It.Value();
				for (int32 i = ObjInfo.Actorts.Num()-1; i>=0; --i)
				{
					ADRActor *Actor = ObjInfo.Actorts[i];
					if (Actor)
					{
						if (Actor->GetWorld() == MyWorld)
						{
							//Actor->Destroy();
							ObjInfo.Actorts.RemoveAt(i);
						}
					}
				}
			}
			HostWorlds.Remove(MyWorld);
		}
	}
}

void UBuildingSystem::SetRoomCeilVisible(bool bVisible)
{
	if (Suite)
	{
		Suite->SetRoomCeilVisible(bVisible);
	}
}

bool UBuildingSystem::IsRoomCeilVisible()
{
	if (Suite)
	{
		return Suite->IsRoomCeilVisible();
	}
	return false;
}

void UBuildingSystem::LoadObjInfo()
{
	IObject **ppObjects = nullptr;
	int nObjs = Suite->GetAllObjects(ppObjects);

	for (int i = 0; i < nObjs; ++i)
	{
		IObject *Obj = ppObjects[i];
		if (Obj)
		{
			int32 ID = Obj->GetID();
			check(ObjMap.Find(ID) == NULL);
			NewSuiteData(Obj);
		}
	}
}

FObjectInfo *UBuildingSystem::NewSuiteData(IObject *RawObj)
{
	FObjectInfo *ObjInfo = nullptr;

	if (RawObj)
	{
		UBuildingData *Data = NewObject<UBuildingData>(this);
		if (Data)
		{
			int32 ID = RawObj->GetID();
			Data->SetRawObj(ID);
			Data->BuildingSystem = this;
			ObjInfo = &ObjMap.Add(ID, FObjectInfo(Data));
		}
	}

	return ObjInfo;
}

void UBuildingSystem::SetSelected(ObjectID ObjID, bool bSelected)
{
	FObjectInfo *pObjInfo = ObjMap.Find(ObjID);
	if (pObjInfo)
	{
	}
}

void UBuildingSystem::OnAddObject(IObject *RawObj)
{
	if (Suite)
	{
		FObjectInfo *ObjInfo = NewSuiteData(RawObj);
		if (ObjInfo)
		{
			for (int32 i = HostWorlds.Num() - 1; i >= 0; --i)
			{
				UWorld *World = (UWorld *)HostWorlds[i].Get(true);
				if (World)
				{
					SpawnActorByObject(World, *ObjInfo);
				}
				else
				{
					HostWorlds.RemoveAt(i);
				}
			}
			if (RawObj->GetType() == EObjectType::EEdge && GetWallBuildSystem())
			{
				GetWallBuildSystem()->AddVirtualWallUMG(RawObj->GetID());
			}
		}
	}
}

void UBuildingSystem::OnDeleteObject(IObject *RawObj)
{
	if (Suite == nullptr) {
		return;
	}

	int32 ObjID = RawObj->GetID();

	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (GetObjectType(ObjID) != EDR_ObjectType::EDR_PluginObject&&projectDataManager != nullptr) {
		EDR_ObjectType ObjectType = static_cast<EDR_ObjectType>(RawObj->GetType());
		UDRDoorAdapter* DoorAdapter = Cast<UDRDoorAdapter>(projectDataManager->GetAdapter(ObjID));
		UDRWindowAdapter* WindowAdapter = Cast<UDRWindowAdapter>(projectDataManager->GetAdapter(ObjID));
		if (DoorAdapter != nullptr)
		{
			int32 HoleObjId = -1;
			EModelType ModelType = DoorAdapter->GetType();
			if (ModelType == EModelType::EDRDoorFram)
			{
				HoleObjId = DoorAdapter->GetAnchorID();
			}
			else
			{
				UDRDoorAdapter* DoorFrameAdapter = Cast<UDRDoorAdapter>(projectDataManager->GetAdapter(DoorAdapter->GetAnchorOwnerID()));
				if (DoorFrameAdapter != nullptr)
				{
					HoleObjId = DoorFrameAdapter->GetAnchorID();
				}
			}
			projectDataManager->RemoveDoorAndWindowContext(HoleObjId);
		}
		else if (WindowAdapter != nullptr) {
			int32 HoleObjId = WindowAdapter->GetAnchorID();
			projectDataManager->RemoveDoorAndWindowContext(HoleObjId);
		}
		else if (ObjectType == EDR_ObjectType::EDR_FloorPlane)
		{
			TMap<int32, UObject*> Local_AreaMap = projectDataManager->AreaMap;
			for (TMap<int32, UObject*>::TIterator It(Local_AreaMap); It; ++It)
			{
				int32 AreaID = It.Key();
				UDRAreaAdapter* AreaAdapter = Cast<UDRAreaAdapter>(projectDataManager->GetAdapter(AreaID));
				if (AreaAdapter->GetFloorID() == ObjID)
				{
					projectDataManager->RemoveObjectContext(AreaID, EDR_ObjectType::EDR_Room);
					break;
				}
			}
			projectDataManager->RemoveObjectContext(ObjID, static_cast<EDR_ObjectType>(RawObj->GetType()));
		}
		else if (ObjectType == EDR_ObjectType::EDR_PolygonArea)
		{
			UWallBuildSystem *WallSystem = GetWallBuildSystem();
			if (WallSystem)
			{
				WallSystem->CollectRoomInfoAfterDelete();
			}
			projectDataManager->RemoveObjectContext(ObjID, static_cast<EDR_ObjectType>(RawObj->GetType()));
		}
		else if (ObjectType == EDR_ObjectType::EDR_BorderCorner)
		{
			UWallBuildSystem *WallSystem = GetWallBuildSystem();
			if (WallSystem) {
				WallSystem->CheckEdgeCorner(ObjID);
			}
			projectDataManager->RemoveObjectContext(ObjID, static_cast<EDR_ObjectType>(RawObj->GetType()));
		}
		else {
			projectDataManager->RemoveObjectContext(ObjID, static_cast<EDR_ObjectType>(RawObj->GetType()));
		}
	}
	FObjectInfo *ObjInfo = ObjMap.Find(ObjID);
	if (ObjInfo == nullptr) {
		return;
	}
	if (RawObj->GetType() == EObjectType::EEdge) {
		UWallBuildSystem *WallSystem = GetWallBuildSystem();
		if (WallSystem) {
			WallSystem->DeleteVirtualWallUMG(ObjID);
		}
	}
	ClearObjInfo(ObjInfo);
	ObjMap.Remove(ObjID);
}

void UBuildingSystem::ClearObjInfo(FObjectInfo *ObjInfo)
{
	for (int32 i = 0; i < ObjInfo->Actorts.Num(); ++i)
	{
		ADRActor *Actor = ObjInfo->Actorts[i];
		UWorld *MyWorld = Actor ? Actor->GetWorld() : nullptr;
		if (MyWorld)
		{
			MyWorld->DestroyActor(Actor);
		}
	}

	ObjInfo->Actorts.Empty();

	if (ObjInfo->Data)
	{
		ObjInfo->Data->ConditionalBeginDestroy();
	}
}

void UBuildingSystem::OnUpdateObject(IObject *RawObj, unsigned int ChannelMask)
{
	if (Suite)
	{
		int32 ObjID = RawObj->GetID();
		FObjectInfo *ObjInfo = ObjMap.Find(ObjID);
		EObjectType TestType = RawObj->GetType();
		EChannelMask TestChannel = (EChannelMask)ChannelMask;

		UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
		if (projectDataManager == nullptr) {
			return;
		}

		if (ObjInfo)
		{
			if (RawObj->GetType() == EObjectType::ESolidWall)
			{
				UObject** solidWallObject = projectDataManager->SolidWallMap.Find(ObjID);
				if (solidWallObject == nullptr || *solidWallObject == nullptr) {
					return;
				}

				ALineWallActor* solidWallActor = Cast<ALineWallActor>(*solidWallObject);
				if (solidWallActor == nullptr) {
					return;
				}

				solidWallActor->UpdateBuildingData(ObjInfo->Data);
			}
			else if (RawObj->IsA(EObjectType::EPluginObject))
			{
				INode* pNode = RawObj->GetNode();
				kXform Xfm = pNode->GetLocalTranform();
				FTransform Transform(FORCE_TYPE(FRotator, Xfm.Rotation), FORCE_TYPE(FVector, Xfm.Location), FORCE_TYPE(FVector, Xfm.Scale));
				if (isnan(Xfm.Location.X) || isinf(Xfm.Location.X) || isnan(Xfm.Location.Y) || isinf(Xfm.Location.Y) ||
					isnan(Xfm.Location.Z) || isinf(Xfm.Location.Z))
				{
					return;
				}

				for (int32 i = 0; i < ObjInfo->Actorts.Num(); ++i)
				{
					AHousePluginActor *Actor = (AHousePluginActor*)ObjInfo->Actorts[i];
					if (Actor)
					{
						//set transform value
						TArray<AActor*> OutActor;
						UGameplayStatics::GetAllActorsOfClass(this, AComponentManagerActor::StaticClass(), OutActor);
						for (int i = 0; i < OutActor.Num(); ++i)
						{
							AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(OutActor[i]);
							if (ComponentManager)
							{
								if (Actor->HousePluginID == ComponentManager->GetHouseComponentInfo().DRActorID)
								{
									EComponenetType CompType = (ComponentManager->GetComponenetInfo()).ComponenetType;
									if (CompType == EComponenetType::_Sewer)
									{
										(Actor)->SetActorTransform(Transform);
										//Actor->SetLoactionAndRotation(Transform.Location, Transform.Rotation);
									}
									else
									{
										(Actor->GroupComponent)->SetWorldTransform(Transform);
										(Actor->GroupComponent)->UpdateNode(pNode);
									}
									break;
								}
							}

						}
						//end operation
					}
				}
			}

			if (RawObj->GetType() == EObjectType::EEdge)
			{
				UWallBuildSystem *WallSystem = GetWallBuildSystem();
				if (WallSystem)
				{
					int32 ObjID = RawObj->GetID();
					WallSystem->UpdateVirtualWallUMG(ObjID);
				}
			}
			if (RawObj->GetType() == EObjectType::EModelInstance)
			{
				UObject** modelInstanceObject = projectDataManager->NormalModelMap.Find(ObjID);
				if (modelInstanceObject == nullptr) {
					for (TPair<int32, UObject*>& currentPair : projectDataManager->DoorMap)
					{
						UDRDoorHoleAdapter* currentDoorHoleAdapter = Cast<UDRDoorHoleAdapter>(projectDataManager->GetAdapter(currentPair.Key));
						if (currentDoorHoleAdapter == nullptr) {
							continue;
						}

						if (currentDoorHoleAdapter->GetDoorFrameObjId() == ObjID || currentDoorHoleAdapter->GetDoorObjId() == ObjID) {
							modelInstanceObject = &(currentPair.Value);
							break;
						}
					}

					if (modelInstanceObject == nullptr) {
						for (TPair<int32, UObject*>& currentPair : projectDataManager->WindowMap)
						{
							UDRWindowHoleAdapter* currentWindowHoleAdapter = Cast<UDRWindowHoleAdapter>(projectDataManager->GetAdapter(currentPair.Key));
							if (currentWindowHoleAdapter == nullptr) {
								continue;
							}

							if (currentWindowHoleAdapter->GetWindowObjId() == ObjID) {
								modelInstanceObject = &(currentPair.Value);
								break;
							}
						}
					}
				}

				if (modelInstanceObject == nullptr || *modelInstanceObject == nullptr) {
					OnAddObject(RawObj);
					return;
				}

				AFurnitureModelActor* normalModelActor = Cast<AFurnitureModelActor>(*modelInstanceObject);
				if (normalModelActor == nullptr) {
					return;
				}

				normalModelActor->UpdateBuildingData(ObjInfo->Data);
			}
			if (RawObj->GetType() == EObjectType::EFloorPlane || RawObj->GetType() == EObjectType::ECeilPlane)
			{
				UObject* areaObject = FindAreaByChildId(ObjID, RawObj->GetType() == EObjectType::EFloorPlane);
				if (areaObject == nullptr) {
					return;
				}

				ARoomActor* areaActor = Cast<ARoomActor>(areaObject);
				if (areaActor == nullptr) {
					return;
				}

				areaActor->UpdateBuildingData(ObjInfo->Data);
			}
			if (RawObj->GetType() == EObjectType::EDoorHole || RawObj->GetType() == EObjectType::EWindow) {
				UObject** holeObject = projectDataManager->DoorHoleMap.Find(ObjID);
				if (holeObject == nullptr) {
					holeObject = projectDataManager->WindowHoleMap.Find(ObjID);
				}
				if (holeObject == nullptr || *holeObject == nullptr) {
					return;
				}

				AWall_Boolean_Base* holeActor = Cast<AWall_Boolean_Base>(*holeObject);
				if (holeActor == nullptr) {
					return;
				}

				holeActor->UpdateBuildingData(ObjInfo->Data);
			}
		}
	}
}

ADRActor *UBuildingSystem::SpawnActorByObject(UWorld *World, FObjectInfo &ObjInfo)
{
	bool isIgnore = false;
	ADRActor *pActor = nullptr;
	IObject *Obj = ObjInfo.Data->GetRawObj();

	if (Obj)
	{
		if (BuilldActorFactory)
		{
			pActor = IBuildActorFactory::Execute_CreateActorByData(BuilldActorFactory, ObjInfo.Data, isIgnore);
			if (isIgnore) {
				return nullptr;
			}
		}		
	}

	if (!pActor)
	{
		if (Obj->IsA(EPrimitive))
		{
			pActor = SpawnPrimitiveComponent(World, ObjInfo, Obj->GetType());
		}
		else if (Obj->IsA(EModelInstance))
		{
			pActor = SpawnModelComponent(World, ObjInfo);
		}
		else if (Obj->IsA(EPluginObject))
		{
			pActor = SpawnPluginComponent(World, ObjInfo, Obj->GetType());
		}
	}

	if (Visitor)
	{
		Visitor->OnCheckObjectVisible(ObjInfo);
	}
	return pActor;
}

//@liff add for loading houseplugin component
void UBuildingSystem::LoadMxModel(UModelFileComponent *InModelFileComponent, UBuildingData *Data)
{
	if (InModelFileComponent && Data)
	{
		FString ResID = "35499A40475F564F116E6B9C8F0F7D46";
		int32   ModelID = Data->GetInt(TEXT("ModelID"));

		UResourceMgr *ResMgr = UResourceMgr::GetResourceMgr();
		if (ResMgr)
		{
			UModelFile *ModelFile = Cast<UModelFile>(ResMgr->FindRes(ResID));
			if (ModelFile)
			{
				FVector Location = Data->GetVector(TEXT("Location"));
				FVector ModLoc = Location;
				//Location.Z = 0.55;
				FVector Forward = FVector(1.0,0.0,0.0);
				int32  AlignType = 0;
				//ModelFile->SetOffset(ModLoc);

				AActor *pActor = Cast<AActor>(InModelFileComponent->GetOwner());
				if (pActor)
				{
					pActor->SetActorLocationAndRotation(ModLoc, Forward.ToOrientationRotator(), false);
				}
				else
				{
					InModelFileComponent->SetWorldLocationAndRotation(ModLoc, Forward.ToOrientationRotator());
				}

				FVector ClipNormal0 = Data->GetVector(TEXT("ClipNormal0"));
				if (!ClipNormal0.IsNearlyZero())
				{
					FVector ClipBase0 = Data->GetVector(TEXT("ClipBase0"));
					InModelFileComponent->AddClipPlane(FPlane(ClipBase0, ClipNormal0));
				}

				FVector ClipNormal1 = Data->GetVector(TEXT("ClipNormal1"));
				if (ClipNormal1.IsNearlyZero())
				{
					FVector ClipBase1 = Data->GetVector(TEXT("ClipBase1"));
					//InModelFileComponent->AddClipPlane(FPlane(ClipBase1, ClipNormal1));
				}

				ModelFile->ForceLoad();
				
				InModelFileComponent->SetModelAlignType(ECenterAdjustType(AlignType));
				InModelFileComponent->UpdateModel(ModelFile);
				
				ModelFile->SetOffset(ModLoc);
				InModelFileComponent->UpdateModel(ModelFile);
			}
		}
	}

}


ADRActor *UBuildingSystem::SpawnPluginComponent(UWorld *MyWorld, FObjectInfo &ObjInfo, int ObjectType)
{
	AHousePluginActor *Actor = (AHousePluginActor *)MyWorld->SpawnActor(AHousePluginActor::StaticClass(), &FTransform::Identity);
	if (Actor)
	{
		Actor->BuildingData = ObjInfo.Data;
		Actor->HousePluginID = (ObjInfo.Data)->GetID();

		//search the component actor type
		bool IsSewerType = false;
		TArray<AActor*> OutActor;
		UGameplayStatics::GetAllActorsOfClass(this, AComponentManagerActor::StaticClass(), OutActor);
		for (int i = 0; i < OutActor.Num(); ++i)
		{
			AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(OutActor[i]);
			if (ComponentManager)
			{
				if (Actor->HousePluginID == ComponentManager->GetHouseComponentInfo().DRActorID)
				{
					ComponentManager->SetHousePlugin(Actor);
					EComponenetType CompType = (ComponentManager->GetComponenetInfo()).ComponenetType;
					if (CompType == EComponenetType::_Pillar)
					{
						Actor->IsPillarPlugin = true;
						/*Actor->ChangePillarSurf();*/
					}
					else if (CompType == EComponenetType::_Sewer)
					{
						IsSewerType = true;
					}
					break;
				}
			}

		}

		if (IsSewerType)
		{	
			UModelFileComponent *GroupComp = Actor->ModelFileComponent;
			Actor->HouseRootComponent = (USceneComponent*)(Actor->ModelFileComponent);
			LoadMxModel(GroupComp, ObjInfo.Data);
			//Actor->HouseRootComponent = (USceneComponent*)(Actor->ModelFileComponent);
		}
		else
		{
			UBuildingGroupMeshComponent *GroupComp = Actor->GroupComponent;
			if (GroupComp)
			{
				GroupComp->RegisterComponentWithWorld(MyWorld);
				IObject *pObj = ObjInfo.Data->GetRawObj();
				INode *pNode = pObj->GetNode();
				kXform Xfm = pNode->GetLocalTranform();
				FTransform Transform(FORCE_TYPE(FRotator, Xfm.Rotation), FORCE_TYPE(FVector, Xfm.Location), FORCE_TYPE(FVector, Xfm.Scale));
				(Actor->GroupComponent)->SetWorldTransform(Transform);
				(Actor->GroupComponent)->UpdateNode(pNode);				
			}

		}
		
		Actor->ChangePillarSurf();
		ObjInfo.Actorts.Add(Actor);
	}
	return Actor;
}
//@liff add end

ADRActor * UBuildingSystem::SpawnModelComponent(UWorld *MyWorld, FObjectInfo &ObjInfo)
{
	AModelFileActor *Actor = (AModelFileActor *)MyWorld->SpawnActor(AModelFileActor::StaticClass(), &FTransform::Identity);
	if (Actor)
	{
		Actor->Update(ObjInfo.Data);
		ObjInfo.Actorts.Add(Actor);
	}
	return Actor;
}

ADRActor * UBuildingSystem::SpawnPrimitiveComponent(UWorld *MyWorld, FObjectInfo &ObjInfo, int ObjectType)
{
	ABuildingActor *Actor = (ABuildingActor *)MyWorld->SpawnActor(ABuildingActor::StaticClass(), &FTransform::Identity);
	if (Actor)
	{
		Actor->bTopCeil = ObjectType == ECeilPlane ? 1 : 0;
		Actor->BuildingData = ObjInfo.Data;
		UBuildingComponent *WallComp = NewObject<UBuildingComponent>(Actor);
		if (WallComp)
		{		
			WallComp->RegisterComponentWithWorld(MyWorld);
			WallComp->SetData(ObjInfo.Data);		
		}
		ObjInfo.Actorts.Add(Actor);
	}
	return Actor;
}

bool UBuildingSystem::IsFree(int32 ObjID)
{
	return Suite ? Suite->IsFree(ObjID) : false;
}

TArray<int32> UBuildingSystem::GetAllTypedObjects(EDR_ObjectType InClass, bool bIncludeDeriveType)
{
	IObject** objectArray = nullptr;

	int32 objectNum(0);

	objectNum = GetAllObjects(objectArray, static_cast<EObjectType>(InClass), bIncludeDeriveType);

	TArray<int32> resultIds;

	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager == nullptr) {
		return resultIds;
	}

	for (int32 Index = 0; Index < objectNum; Index++)
	{
		int32 currentObjectId = objectArray[Index]->GetID();
		
		if (InClass == EDR_ObjectType::EDR_Room) {
			UDRAreaAdapter* areaAdapter = Cast<UDRAreaAdapter>(projectDataManager->GetAdapter(currentObjectId));
			if (areaAdapter == nullptr) {
				continue;
			}
			
			if (areaAdapter->GetArea() <= 5000.f) {
				continue;
			}
		}

		if (InClass == EDR_ObjectType::EDR_PolygonArea) {
			UDRAreaAdapter* areaAdapter = Cast<UDRAreaAdapter>(projectDataManager->GetAdapter(currentObjectId));
			if (areaAdapter == nullptr) {
				continue;
			}

			if (areaAdapter->GetArea() <= 5000.f) {
				continue;
			}
		}

		resultIds.Add(currentObjectId);
		
	}

	return resultIds;
}

int32 UBuildingSystem::GetAllObjects(IObject** &ppOutObject, EObjectType InClass,bool bIncludeDeriveType)
{
	if (Suite)
	{
		return Suite->GetAllObjects(ppOutObject, InClass, bIncludeDeriveType);
	}
	return 0;
}

IObject *UBuildingSystem::GetObject(int32 ObjID)
{

	if (Suite)
	{
		return Suite->GetObject(ObjID);
	}

	/*UBuildingData *pData = GetData(ObjID);
	if (pData)
	{
		return pData->GetRawObj();
	}*/
	return nullptr;
}

EDR_ObjectType UBuildingSystem::GetObjectType(int32 ObjectId)
{
	if (Suite == nullptr) {
		return EDR_ObjectType::EDR_UnkownObject;
	}

	IObject* RawObject = Suite->GetObject(ObjectId);

	if (RawObject == nullptr) {
		return EDR_ObjectType::EDR_UnkownObject;
	}

	if (RawObject->IsA(EObjectType::EPluginObject)) {
		return EDR_ObjectType::EDR_PluginObject;
	}

	return static_cast<EDR_ObjectType>(RawObject->GetType());
}

void UBuildingSystem::DeleteObject(int32 ObjID)
{
	if (Suite)
	{
		Suite->DeleteObject(ObjID);
	}
}

int32 UBuildingSystem::Snap(const FVector2D &Loc, int32 Ignore, FVector2D &BestLoc, int32 &BestID, float Torlerance, float ToleranceEdge)
{
	if (Suite)
	{
		const unsigned int Op_AddWallFilter = ESFCorner | ESFWallCenterLine | ESFDirectionX | ESFDirectionY;
		return Suite->FindSnapLocation(FORCE_TYPE(kPoint, Loc), FORCE_TYPE(kPoint, BestLoc), BestID, Ignore, ESnapFilter(Op_AddWallFilter), Torlerance, ToleranceEdge);
	}
	return kESnapType::kENone;
}

void UBuildingSystem::Tick(float DeltaTime)
{
	if (BuildingSDK)
	{
		BuildingSDK->Tick(DeltaTime);
	}
}

void AddTri(const FVector2D &S, const FVector2D &E, const FVector2D &Offset2D, FCanvasUVTri *Triangles, const FLinearColor &Color)
{
	Triangles[0].V0_Pos = S - Offset2D;
	Triangles[0].V1_Pos = S + Offset2D;
	Triangles[0].V2_Pos = E + Offset2D;

	Triangles[0].V0_Color = Color;
	Triangles[0].V1_Color = Color;
	Triangles[0].V2_Color = Color;

	Triangles[1].V0_Pos = E + Offset2D;
	Triangles[1].V1_Pos = E - Offset2D;
	Triangles[1].V2_Pos = S - Offset2D;

	Triangles[1].V0_Color = Color;
	Triangles[1].V1_Color = Color;
	Triangles[1].V2_Color = Color;
}

void UBuildingSystem::OnPropertyChanged(UBuildingData *Data, const FString &PropName, const IValue &NewValue)
{
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*PropName);
		Suite->SetProperty(Data->GetID(), AnsiName, &NewValue);
		if (EventPropertyChangedHandler.IsBound())
		{
			EventPropertyChangedHandler.Broadcast(Data, PropName);
		}
	}
}

UBuildingData *UBuildingSystem::GetData(int32 ID)
{
	FObjectInfo *pObjInfo = ObjMap.Find(ID);
	if (pObjInfo)
	{
		return pObjInfo->Data;
	}
	return NULL;
}

int32  UBuildingSystem::AddCorner(const FVector2D &Location)
{
	if (Suite)
	{
		kPoint Loc = ToBuildingPosition(Location);
		return Suite->AddCorner(Loc.X, Loc.Y);
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::GetWallObjectIDByTwoCorner(const int32& CornerStart, const int32& CornerEnd)
{
	if (Suite)
	{
		return 	Suite->GetWallByTwoCorner(CornerStart, CornerEnd);
	 }
	else
	{
		return INVALID_OBJID;
	}
}

void UBuildingSystem::SetWallInitData(FWallNode WallNode, const int32& WallObjID)
{
	if (Suite && WallObjID != INVALID_OBJID)
	{
		IObject* WallObj = Suite->GetObject(WallObjID);
		
		if(WallObj)
		{ 
			IValueFactory* VF = UBuildingSystem::GetValueFactory();
			//WallMaterial����������8������3dģ�͵�6�����2dģ�͵�������
			int num = WallObj->GetSurfaceCount();
			UE_LOG(LogTemp, Log, TEXT("Surface count = %d"), num);
			IValue& ValueArray = VF->Create();
			for (int i = 0; i < WallNode.WallMaterial.Num(); i++)
			{
				FMaterialNode MatNode = WallNode.WallMaterial[i];
				int _ModelID = MatNode.ModelID;
				if(_ModelID == 0)
				{ 
					_ModelID = UDROperationHouseFunctionLibrary::GetConfigModelIDByType(EDConfigType::EDRWallConfig);
				}
				IValue& Value = VF->Create();
				Value.AddField("ModelID", VF->Create(_ModelID));
				Value.AddField("RoomClassID", VF->Create(MatNode.RoomClassID));
				Value.AddField("CraftID", VF->Create(MatNode.CraftID));
				Value.AddField("ResID", VF->Create(FStringToConstChar(*MatNode.ResID)));
				Value.AddField("Url", VF->Create(FStringToConstChar(*MatNode.Url)));
				ValueArray.AddField(Value);
				//FString MatName = TEXT("Mat") + FString::FromInt(i);
				//Suite->SetValue(WallObjID, FStringToConstChar(MatName), &Value);
			}
			Suite->SetValue(WallObjID, "WallMaterials", &ValueArray);

		}
	}
}

int32 UBuildingSystem::AddModelToObject(int32 BaseObjID, const FString &ResID, const FVector &Location)
{
	if (Suite)
	{
		const char *AnsiResID = TCHAR_TO_ANSI(*ResID);
		kVector3D Loc = ToBuildingPosition(Location);
		return Suite->AddModel(BaseObjID, AnsiResID, Loc);
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddNewModel(const FString & ResId, const FTransform & Transform, EModelType Type)
{
	if (Suite == nullptr) {
		return INVALID_OBJID;
	}

	return Suite->AddModel(INVALID_OBJID, TCHAR_TO_ANSI(*ResId),
		ToBuildingPosition(Transform.GetLocation()), ToBuildingRotation(Transform.GetRotation().Rotator()), ToBuildingPosition(Transform.GetScale3D()), static_cast<int>(Type));
}

int32 UBuildingSystem::AddModelToAnchor(int32 AnchorID, const FString &ResID, const FVector &Location)
{
	if (Suite)
	{
		const char *AnsiResID = TCHAR_TO_ANSI(*ResID);
		kVector3D Loc = ToBuildingVector(Location);
		return Suite->AddModel(AnchorID, AnsiResID, Loc);
	}
	return INVALID_OBJID;
}

TArray<int32> UBuildingSystem::AddWall(int32 StartCorner, int32 EndCorner, float ThickLeft, float ThickRight, float Height)
{
	TArray<int32> WallIDs;
	if (Suite)
	{
		kArray<int>  Temp = Suite->AddWall(StartCorner, EndCorner, ThickLeft, ThickRight, Height);
		for (int i = 0;i < Temp.size();++i)
		{
			WallIDs.Add(Temp[i]);
		}
	}
	return WallIDs;
}

int32 UBuildingSystem::AddWindow(int32 WallID, const FVector2D &Location, float zPos, float Width, float Height, const FString &WindowResID, const FString &SillsResID)
{
	if (Suite)
	{
		kPoint Loc = ToBuildingPosition(Location);
		return Suite->AddWindow(WallID, Loc, zPos, Width, Height, TCHAR_TO_ANSI(*WindowResID), TCHAR_TO_ANSI(*SillsResID));
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddDoor(int32 WallID, const FVector2D &Location, float Width, float Height, float zPos /*= 0*/)
{
	if (Suite)
	{
		kPoint Loc = ToBuildingPosition(Location);
		return Suite->AddDoor(WallID, Loc, Width, Height, zPos);
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddPointLight(const FVector &Location, float SourceRadius, float SoftSourceRadius, float SourceLength, float Intensity, FLinearColor LightColor, bool bCastShadow)
{
	if (Suite)
	{
		kVector3D Loc = ToBuildingVector(Location);
		kColor Color = ToBuildingColor(LightColor);
		return (int32)Suite->AddPointLight(Loc, SourceRadius, SoftSourceRadius, SourceLength, Intensity, Color, bCastShadow);
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddSpotLight(const FVector &Location, const FRotator &Rotationn, float AttenuationRadius, float SourceRadius, float SoftSourceRadius, float SourceLength, float InnerConeAngle, float OuterConeAngle, float Intensity, FLinearColor LightColor, bool bCastShadow, float ColorTemperature, float Highlight)\
{
	if (Suite)
	{
		kVector3D Loc = ToBuildingVector(Location);
		kRotation Rot = ToBuildingRotation(Rotationn);
		kColor Color =  ToBuildingColor(LightColor);
		return (int32)Suite->AddSpotLight(Loc, Rot, AttenuationRadius, SourceRadius, SoftSourceRadius, SourceLength, InnerConeAngle, OuterConeAngle, Intensity, Color, bCastShadow, ColorTemperature, Highlight);
	}
	return INVALID_OBJID;
}

//HouseComponent operation function start
int32 UBuildingSystem::AddFlue(const FVector2D &Location, float Width, float Length, float Height)
{
	if (Suite)
	{

		FVector2D Forward = FVector2D(1.0, 0.0);
		kPoint Loc = ToBuildingPosition(Location);
		kPoint Forwd = ToBuildingPosition(Forward);
		if (IsNaN(Loc.x) || IsNaN(Loc.y))
		{
			return INVALID_OBJID;
		}
		void* PoleEditor = BuildingSDK->QueryInterface("PoleEditor");
		if (PoleEditor)
		{
			IPoleEditor* PoleEdit = (IPoleEditor*)PoleEditor;
			int flueId = PoleEdit->AddPole(Height, Width, Length, Loc, Forwd);
			return flueId;
		}

		return INVALID_OBJID;
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddTube(const FVector2D &Location, float Width, float Length, float Height)
{
	if (Suite)
	{

		FVector2D Forward = FVector2D(1.0, 0.0);
		kPoint Loc = ToBuildingPosition(Location);
		kPoint Forwd = ToBuildingPosition(Forward);
		if (IsNaN(Loc.x) || IsNaN(Loc.y))
		{
			return INVALID_OBJID;
		}

		void* TubeEditor = BuildingSDK->QueryInterface("TubeEditor");
		if (TubeEditor)
		{
			ITubeEditor* TubeEdit = (ITubeEditor*)TubeEditor;
			int TubeID = TubeEdit->AddTube(Height, Width, Length, Loc, Forwd);

			return TubeID;

		}

		return INVALID_OBJID;
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddPillar(const FVector2D &Location, float Width, float Length, float Height)
{
	if (Suite)
	{
		FVector2D Forward = FVector2D(1.0, 0.0);
		bool IsAnch = true;

		//coordinate convert
		kPoint Loc = ToBuildingPosition(Location);
		kPoint Forwd = ToBuildingPosition(Forward);
		if (IsNaN(Loc.x) || IsNaN(Loc.y))
		{
			return INVALID_OBJID;
		}

		void* PillarEditor = BuildingSDK->QueryInterface("PillarEditor");
		if (PillarEditor)
		{
			IPillarEditor* PillarEdit = (IPillarEditor*)PillarEditor;
			int PillarID = PillarEdit->AddPillar(IsAnch, Height, Width, Length, Loc, Forwd);

			return PillarID;
		}

		return INVALID_OBJID;
	}
	return INVALID_OBJID;
}

int32 UBuildingSystem::AddSewer(const FVector2D &Location, float Diameter)
{
	if (Suite)
	{
		//coordinate convert
		kPoint Loc = ToBuildingPosition(Location);
		if (IsNaN(Loc.x) || IsNaN(Loc.y))
		{
			return INVALID_OBJID;
		}

		void* SewerEditor = BuildingSDK->QueryInterface("SewerEditor");
		if (SewerEditor)
		{
			ISewerEditor* SewerEdit = (ISewerEditor*)SewerEditor;
			int SewID = SewerEdit->AddSewer(Diameter, Loc);

			return SewID;
		}
		/*FString ResId = "35499A40475F564F116E6B9C8F0F7D46";
		FVector LocVal = FVector(Location, 0.0);
		int SewID = AddModelToObject(-1, ResId, LocVal);*/

		return INVALID_OBJID;
	}
	return INVALID_OBJID;
}

/*update function*/
bool UBuildingSystem::UpdateFlueProperty(int32 ObjID, const FVector2D &Location, float Angle, float Width, float Length)
{

	if (Suite)
	{
		IObject* Obj = GetObject(ObjID);
		if (!Obj)
		{
			return false;
		}
		kPoint Loc = ToBuildingPosition(Location);

		void* FlueEditor = BuildingSDK->QueryInterface("PoleEditor");
		if (FlueEditor)
		{
			IPoleEditor* FlueEdit = (IPoleEditor*)FlueEditor;
			bool UpdateBool = FlueEdit->UpdatePole(ObjID, Width, Length, Loc, Angle);
			return UpdateBool;
		}
		
		return false;
	}

	return false;
}

bool UBuildingSystem::UpdateTubeProperty(int32 ObjID, const FVector2D &Location, float Angle, float Width, float Length)
{
	if (Suite)
	{
		IObject* Obj = GetObject(ObjID);
		if (!Obj)
		{
			return false;
		}

		kPoint Loc = ToBuildingPosition(Location);

		void* TubeEditor = BuildingSDK->QueryInterface("TubeEditor");
		if (TubeEditor)
		{
			ITubeEditor* TubeEdit = (ITubeEditor*)TubeEditor;
			return TubeEdit->UpdateTubeAttr(ObjID, Width, Length, Loc, Angle);
		}

		return false;
	}

	return false;
}

bool UBuildingSystem::UpdatePillarProperty(int32 ObjID, const FVector2D &Location, float Angle, float Width, float Length)
{
	if (Suite)
	{
		IObject* Obj = GetObject(ObjID);
		if (!Obj)
		{
			return false;
		}
		kPoint Loc = ToBuildingPosition(Location);
		void* PillarEditor = BuildingSDK->QueryInterface("PillarEditor");
		if (PillarEditor)
		{
			IPillarEditor* PillarEdit = (IPillarEditor*)PillarEditor;
			return PillarEdit->UpdatePillarAttr(ObjID, Width, Length, Loc, Angle);
		}

		return false;
	}

	return false;
}

bool UBuildingSystem::UpdateSewerProperty(int32 ObjID, const FVector2D &LocationVal, float Diameter)
{
	IObject* Obj = GetObject(ObjID);
	if (!Obj)
	{
		return false;
	}
	kPoint Loc = ToBuildingPosition(LocationVal);
	void* SewerEditor = BuildingSDK->QueryInterface("SewerEditor");
	if (SewerEditor)
	{
		ISewerEditor* SewerEdit = (ISewerEditor*)SewerEditor;
		return SewerEdit->UpdateSewerAttr(ObjID, Diameter, Loc);
	}
	return false;
}

/*update component by attaching wall object*/
bool UBuildingSystem::UpdateFlue(int32 ObjID, const FVector2D &LocationVal, float RotateAng)
{
	if (Suite && ObjID != INVALID_OBJID)
	{
		void* PoleEditor = BuildingSDK->QueryInterface("PoleEditor");
		if (PoleEditor)
		{
			IPoleEditor* PoleEdit = (IPoleEditor*)PoleEditor;
			kPoint Loc = ToBuildingPosition(LocationVal);
			return PoleEdit->UpdatePole(ObjID, Loc, RotateAng);
		}
	}

	return false;
}

bool UBuildingSystem::UpdateTube(int32 ObjID, const FVector2D &LocationVal, float RotateAng)
{
	if (Suite && ObjID != INVALID_OBJID)
	{
		void* TubeEditor = BuildingSDK->QueryInterface("TubeEditor");
		if (TubeEditor)
		{
			ITubeEditor* TubeEdit = (ITubeEditor*)TubeEditor;
			kPoint Loc = ToBuildingPosition(LocationVal);
			return TubeEdit->UpdateTube(ObjID, Loc, RotateAng);
		}
	}

	return false;
}

bool UBuildingSystem::UpdatePillar(int32 ObjID, const FVector2D &LocationVal, float RotateAng)
{
	if (Suite && ObjID != INVALID_OBJID)
	{
		void* PillarEditor = BuildingSDK->QueryInterface("PillarEditor");
		if (PillarEditor)
		{
			IPillarEditor* PillarEdit = (IPillarEditor*)PillarEditor;
			kPoint Loc = ToBuildingPosition(LocationVal);
			return PillarEdit->UpdatePillar(ObjID, Loc, RotateAng);
		}
	}

	return false;
}

/*material setting function*/
bool UBuildingSystem::SetHousePluginSurface(int32 ObjID, int32 MaterialChannel, const FString &MaterialUri, int32 MaterialType)
{
	bool IsSet = false;

	if (Suite && ObjID != INVALID_OBJID)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);
		if (PlugObj)
		{
			int FlueType, TubeType, PillarType;
			FlueType = EPluginObject + 1;
			TubeType = EPluginObject + 2;
			PillarType = EPluginObject + 3;

			if (PlugObj->IsA((EObjectType)FlueType))
			{
				IsSet = SetFlueSurface(ObjID, MaterialChannel, MaterialUri, MaterialType);
			}
			else if (PlugObj->IsA((EObjectType)TubeType))
			{
				IsSet = SetTubeSurface(ObjID, MaterialChannel, MaterialUri, MaterialType);
			}
			else if (PlugObj->IsA((EObjectType)PillarType))
			{
				IsSet = SetPillarSurface(ObjID, MaterialChannel, MaterialUri, MaterialType);
			}
		}
	}

	return IsSet;
}

bool UBuildingSystem::SetFlueSurface(int32 ObjID, int32 SectionIndex, const FString &MaterialUri, int32 MaterialType)
{
	void* PoleEditor = BuildingSDK->QueryInterface("PoleEditor");
	if (PoleEditor)
	{
		IPoleEditor* PoleEdit = (IPoleEditor*)PoleEditor;

		std::string MaterUri(TCHAR_TO_UTF8(*MaterialUri));

		return PoleEdit->SetPoleSurface(ObjID, SectionIndex, MaterUri.c_str(), MaterialType);
	}

	return false;
}

bool UBuildingSystem::SetTubeSurface(int32 ObjID, int32 SectionIndex, const FString &MaterialUri, int32 MaterialType)
{
	void* TubeEditor = BuildingSDK->QueryInterface("TubeEditor");
	if (TubeEditor)
	{
		ITubeEditor* TubeEdit = (ITubeEditor*)TubeEditor;

		std::string MaterUri(TCHAR_TO_UTF8(*MaterialUri));

		return TubeEdit->SetTubeSurface(ObjID, SectionIndex, MaterUri.c_str(), MaterialType);
	}

	return false;
}

bool UBuildingSystem::SetPillarSurface(int32 ObjID, int32 SectionIndex, const FString &MaterialUri, int32 MaterialType)
{
	void* PillarEditor = BuildingSDK->QueryInterface("PillarEditor");
	if (PillarEditor)
	{
		IPillarEditor* PillarEdit = (IPillarEditor*)PillarEditor;

		std::string MaterUri(TCHAR_TO_UTF8(*MaterialUri));

		return PillarEdit->SetPillarSurface(ObjID, SectionIndex, MaterUri.c_str(), MaterialType);
	}

	return false;
}

/*set houseComponent height*/
bool UBuildingSystem::SetHousePluginHeight(int32 ObjID, float height)
{
	bool IsChanged = false;

	if (Suite && ObjID != INVALID_OBJID)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);
		if (PlugObj)
		{
			int FlueType, TubeType, PillarType;
			FlueType = EPluginObject + 1;
			TubeType = EPluginObject + 2;
			PillarType = EPluginObject + 3;

			if (PlugObj->IsA((EObjectType)FlueType))
			{
				void* PoleEditor = BuildingSDK->QueryInterface("PoleEditor");
				if (PoleEditor)
				{
					IPoleEditor* PoleEdit = (IPoleEditor*)PoleEditor;
					IsChanged = PoleEdit->SetPoleHeightVal(ObjID,height);
				}
			}
			else if (PlugObj->IsA((EObjectType)TubeType))
			{
				void* TubeEditor = BuildingSDK->QueryInterface("TubeEditor");
				if (TubeEditor)
				{
					ITubeEditor* TubeEdit = (ITubeEditor*)TubeEditor;
					IsChanged = TubeEdit->SetTubeHeightVal(ObjID, height);
				}
			}
			else if (PlugObj->IsA((EObjectType)PillarType))
			{
				void* PillarEditor = BuildingSDK->QueryInterface("PillarEditor");
				if (PillarEditor)
				{
					IPillarEditor* PillarEdit = (IPillarEditor*)PillarEditor;
					IsChanged = PillarEdit->SetPillarHeightVal(ObjID, height);
				}
			}
		}
	}

	return IsChanged;
}

/*get wall height*/
float UBuildingSystem::GetWallHeight()
{
	if (Suite)
	{
		IObject *Cfg = Suite->GetConfig();
		if (Cfg)
		{
			float WallHeight = Cfg->GetPropertyFloat("WallHeight");
			return WallHeight;
		}

	}

	return 280.0;
}

/*delete houseComponent functions*/
void UBuildingSystem::DeleteFlueObj(int32 ObjID)
{
	if (Suite && ObjID != INVALID_OBJID)
	{
		void* PoleEditor = BuildingSDK->QueryInterface("PoleEditor");
		if (PoleEditor)
		{
			IPoleEditor* PoleEdit = (IPoleEditor*)PoleEditor;
			PoleEdit->DeletePole(ObjID);
		}

	}
}

void UBuildingSystem::DeleteTubeObj(int32 ObjID)
{
	if (Suite && ObjID != INVALID_OBJID)
	{
		void* TubeEditor = BuildingSDK->QueryInterface("TubeEditor");
		if (TubeEditor)
		{
			ITubeEditor* TubeEdit = (ITubeEditor*)TubeEditor;
			TubeEdit->DeleteTube(ObjID);
		}

	}
}

void UBuildingSystem::DeletePillarObj(int32 ObjID)
{
	if (Suite && ObjID != INVALID_OBJID)
	{
		void* PillarEditor = BuildingSDK->QueryInterface("PillarEditor");
		if (PillarEditor)
		{
			IPillarEditor* PillarEdit = (IPillarEditor*)PillarEditor;
			PillarEdit->DeletePillar(ObjID);
		}
	}
}

void UBuildingSystem::DeleteSewerObj(int32 ObjID)
{
	if (Suite && ObjID != INVALID_OBJID)
	{
		void* SewerEditor = BuildingSDK->QueryInterface("SewerEditor");
		if (SewerEditor)
		{
			ISewerEditor* SewerEdit = (ISewerEditor*)SewerEditor;
			SewerEdit->DeleteSewer(ObjID);
		}

	}
}

/*transport 3d attributes value to 2d primitive*/
int UBuildingSystem::GetPluginTrans(int32 ObjID, float* ValArr)
{
	//float Points[5];
	if (Suite &&ObjID != INVALID_OBJID)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);
		if (PlugObj)
		{
			int ObjType = PlugObj->GetType();
			int FlueType, TubeType, PillarType;
			FlueType = EObjectType::EPluginObject + 1;
			TubeType = EObjectType::EPluginObject + 2;
			PillarType = EObjectType::EPluginObject + 3;
			bool IsHousePlug = false;
			if ((ObjType == FlueType) || (ObjType == TubeType) || (ObjType == PillarType))
			{
				IsHousePlug = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("The object is not housePlugin type.\n"));
			}

			if (PlugObj&&IsHousePlug)
			{
				kXform PlugTransf = PlugObj->GetTransform();
				kRotation RotVect = PlugTransf.Rotation;
				//Points[0]=RotVect.Yaw;
				ValArr[0] = RotVect.Yaw;

				IValue *WidthIV, *LenIV, *LocIV;
				WidthIV = &PlugObj->GetPropertyValue("Width");
				LenIV = &PlugObj->GetPropertyValue("Length");
				LocIV = &PlugObj->GetPropertyValue("Location");
				float WidVal, LenVal, LocX, LocY;
				WidVal = WidthIV->FloatValue();
				LenVal = LenIV->FloatValue();
				kPoint LocPt = LocIV->Vec2Value();
				LocX = LocPt.x;
				LocY = LocPt.y;

				ValArr[1] = WidVal;
				ValArr[2] = LenVal;
				ValArr[3] = LocPt.X;
				ValArr[4] = LocPt.Y;
				return 5;
			}

			return -1;
		}
	}
	return -1;
}

void UBuildingSystem::GetHousePluginBorder(int32 ObjID, kVector3D* BorderArray)
{
	if (ObjID != INVALID_OBJID)
	{
		IObject* PlugObj = Suite->GetObject(ObjID);
		if (PlugObj)
		{
			int FlueType, TubeType, PillarType;
			FlueType = EPluginObject + 1;
			TubeType = EPluginObject + 2;
			PillarType = EPluginObject + 3;

			if (PlugObj->IsA((EObjectType)FlueType))
			{

				void* PoleEditor = BuildingSDK->QueryInterface("PoleEditor");
				if (PoleEditor)
				{
					IPoleEditor* PoleEdit = (IPoleEditor*)PoleEditor;
					bool IsGet = PoleEdit->GetPoleBorder(ObjID, BorderArray);

				}
			}
			else if (PlugObj->IsA((EObjectType)TubeType))
			{
				void* TubeEditor = BuildingSDK->QueryInterface("TubeEditor");
				if (TubeEditor)
				{
					ITubeEditor* TubeEdit = (ITubeEditor*)TubeEditor;
					bool IsGet = TubeEdit->GetTubeBorder(ObjID, BorderArray);
				}

			}
			else if (PlugObj->IsA((EObjectType)PillarType))
			{
				void* PillarEditor = BuildingSDK->QueryInterface("PillarEditor");
				if (PillarEditor)
				{
					IPillarEditor* PillarEdit = (IPillarEditor*)PillarEditor;
					bool IsGet = PillarEdit->GetPillarBorder(ObjID, BorderArray);
				}

			}
		}
	}
}

/*calculate the distance is width or not*/
bool UBuildingSystem::IsWallWidth(int32 WallID, const FVector2D &LocVal, float ThickVal, float Width, float Length)
{
	if (WallID != INVALID_OBJID)
	{
		FVector2D StartP0, EndP1, WallRight;
		bool GetRightBool = GetWallVector(WallID, StartP0, EndP1, WallRight);
		if (GetRightBool)
		{
			bool WidIsWall = false;
			FVector2D WallDir, LocDir;
			WallDir = EndP1 - StartP0;
			LocDir = LocVal - StartP0;
			float CosVal, SinVal;
			CosVal = FVector2D::DotProduct(WallDir, LocDir) / (WallDir.Size()*LocDir.Size());
			SinVal = Sqrt(1.0 - Square(abs(CosVal)));

			float WallDist = LocDir.Size()*SinVal;
			bool DiffIsBig = false;
			if (abs(Width - Length) < 5.1)
			{
				DiffIsBig = true;
			}
			//UE_LOG(LogTemp, Warning, TEXT("Wall distance value is: x=%f, y=%f.\n"), WallDist, (WallDist - ThickVal - (Width / 2)));
			if (DiffIsBig)
			{
				if ((WallDist - ThickVal - Width / 2) < 1.5)
				{
					WidIsWall = true;
				}
			}
			else
			{
				if ((WallDist - ThickVal - Width / 2) < 2.5)
				{
					WidIsWall = true;
				}
			}


			return WidIsWall;
		}
		return false;
	}

	return false;
}
//HouseComponent operation function end

void UBuildingSystem::SetSkyLight(const FDRSkyLight & Sky, const FDirectionLight & Direction)
{
	if (Suite)
	{
		IObject *Cfg = Suite->GetConfig();
		IValue & _Sky = Cfg->GetPropertyValue("SkyLight");
		int32 SkyID = _Sky.IntValue();
		UBuildingData * SkyData = GetData(SkyID);
		if (SkyData)
		{
			//Direction
			SkyData->SetVector(TEXT("DirectionLightRotation"), Direction.Rotation.Vector());
			SkyData->SetVector(TEXT("DirectionLightColor"), FVector(Direction.Color.R, Direction.Color.G, Direction.Color.B));
			SkyData->SetFloat(TEXT("DirectionLightIntensity"), Direction.Intensity);
			//Sky
			SkyData->SetFloat(TEXT("SkyLightIntensity"), Sky.Intensity);
			SkyData->SetFloat(TEXT("SkyLightAngle"), Sky.Angle);
			SkyData->SetVector(TEXT("SkyLightColor"), FVector(Sky.Color.R, Sky.Color.G, Sky.Color.B));
		}
	}
}

void UBuildingSystem::SetPostProcess(const FPostProcess & Post)
{
	if (Suite)
	{
		IObject *Cfg = Suite->GetConfig();
		IValue & _Pose = Cfg->GetPropertyValue("PostProcess");
		int32 PostID = _Pose.IntValue();
		UBuildingData * PostData =  GetData(PostID);
		if (PostData)
		{
			//Post.ID = PostID;
			PostData->SetFloat(TEXT("Saturation"), Post.Saturation);
			PostData->SetFloat(TEXT("BloomIntensity"), Post.BloomIntensity);
			PostData->SetFloat(TEXT("AmbientOcclusion"), Post.AmbientOcclusion);
			PostData->SetFloat(TEXT("AmbientOcclusionRadius"), Post.AmbientOcclusionRadius);
		}
	}
}

int32 UBuildingSystem::FindCloseWall(const FVector2D & Location, float Width, FVector2D & BestLoc, float Tolerance)
{
	if (Suite)
	{
		int32 ID = - 1;
		kPoint Loc = ToBuildingPosition(Location);
		kPoint kBestLoc;
		ID = Suite->FindCloseWall(Loc, Width, kBestLoc, Tolerance);
		BestLoc = ToVector2D(kBestLoc);
		return ID;
	}
	return int32();
}

void UBuildingSystem::Build()
{
	if (Suite)
	{
		Suite->BuildRooms();
	}
}

UWorld *UBuildingSystem::GetWorld() const
{
	UObject *Outer = GetOuter();
	return Outer ? Outer->GetWorld() : nullptr;
}

void UBuildingSystem::SetChannelVisbile(EVisibleChannel Channel, bool bVisible)
{
	for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
	{
		FObjectInfo &ObjInfo = It.Value();
		for (int32 i = 0; i < ObjInfo.Actorts.Num(); ++i)
		{
			ADRActor *pActor = ObjInfo.Actorts[i];
			if (pActor && pActor->GetChannel()==Channel)
			{
				pActor->SetActorHiddenInGame(!bVisible);
			}
		}
	}
}

void UBuildingSystem::SetVisitor(IBuildingVisitor *InVisitor)
{
	Visitor = InVisitor;
}

void UBuildingSystem::UpdateChannelVisible()
{
	if (Visitor)
	{
		for (TMap<int32, FObjectInfo>::TIterator It(ObjMap); It; ++It)
		{
			FObjectInfo &ObjInfo = It.Value();
			Visitor->OnCheckObjectVisible(ObjInfo);
		}
	}
}

void UBuildingSystem::SetObjIntValue(const int32& ObjID, FString& ValueName, const int32& IntValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			IValue &Value = VF->Create(IntValue);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	 }
}

void UBuildingSystem::SetObjFloatValue(const int32& ObjID, FString& ValueName, const int32& FloatValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			IValue &Value = VF->Create(FloatValue);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UBuildingSystem::SetObjFStringValue(const int32& ObjID, FString& ValueName, FString& FStringValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			const char* valuechar = FStringToConstChar(FStringValue);
			IValue &Value = VF->Create(valuechar);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UBuildingSystem::SetObjFVector(const int32& ObjID, FString& ValueName, FVector& FVectorValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			kVector3D KValue = ToBuildingVector(FVectorValue);
			IValue &Value = VF->Create(&KValue, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UBuildingSystem::SetObjFVector2D(const int32& ObjID, FString& ValueName, FVector2D& FVectorValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			kPoint KValue = ToBuildingPosition(FVectorValue);
			IValue &Value = VF->Create(&KValue, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UBuildingSystem::SetObjFVector4D(const int32& ObjID, FString& ValueName, FVector4& FVectorValue)
{
	if (Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			kVector4D KValue = ToBuildingVector4D(FVectorValue);
			IValue &Value = VF->Create(&KValue, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

UObject* UBuildingSystem::FindAreaByChildId(int32 ObjectId, bool bIsFloor)
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager == nullptr) {
		return nullptr;
	}

	for (const TPair<int32, UObject*>& currentPair : projectDataManager->AreaMap)
	{
		if (currentPair.Value == nullptr) {
			continue;
		}

		UDRAreaAdapter* areaAdapter = Cast<UDRAreaAdapter>(projectDataManager->GetAdapter(currentPair.Key));
		if (areaAdapter == nullptr) {
			continue;
		}

		int32 childId(bIsFloor ? areaAdapter->GetFloorID() : areaAdapter->GetCeilID());

		if (childId == ObjectId) {
			return currentPair.Value;
		}
	}

	return nullptr;
}

void UBuildingSystem::GetAllCornerActorLoction(TArray<FVector2D> &OutAllCornerActorLoction, int32 objectID)
{
	if (Suite)
	{
		IObject **ppAllCornerObject = nullptr;
		int nObjs = Suite->GetAllObjects(ppAllCornerObject, EObjectType::ECorner,false);
		for (int i = 0; i < nObjs; ++i)
		{
			if (ppAllCornerObject[i]&& ppAllCornerObject[i]->GetID()!= objectID)
				OutAllCornerActorLoction.Add(ToVector2D(ppAllCornerObject[i]->GetPropertyValue("Location").Vec2Value()));
		}
	}
}

void UBuildingSystem::GetWallBorderLineLocation(TArray<FVector2D> &OutAllCornerActorLoction)
{
	if (Suite)
	{
		TArray<int32> WallIDs;
		UWallBuildSystem *WallSystem = GetWallBuildSystem();
		WallSystem->GetAllObject(WallIDs, EObjectType::ESolidWall, false);

		for (int i = 0; i < WallIDs.Num()-1; ++i)
		{
			UBuildingData *WallData = GetData(WallIDs[i]);
			
			if (!WallData)
			{
				continue;
			}

			int P0 = WallData->GetInt("P0");
			int P1 = WallData->GetInt("P1");

			IObject *CornerData1 = GetObject(P0);
			IObject *CornerData2 = GetObject(P1);
			if (CornerData1 && CornerData2)
			{
				FVector2D CornerPos1 = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
				FVector2D CornerPos2 = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());

				FVector2D Forworld = (CornerPos2 - CornerPos1).GetSafeNormal();
				FVector2D Dir = Forworld.GetRotated(90);
				float Wallthick = WallSystem ? WallSystem->WallThickness : 10;
				OutAllCornerActorLoction.Add(CornerPos1 - Wallthick * Dir);
				OutAllCornerActorLoction.Add(CornerPos1 + Wallthick * Dir);
				OutAllCornerActorLoction.Add(CornerPos2 - Wallthick * Dir);
				OutAllCornerActorLoction.Add(CornerPos2 + Wallthick * Dir);
			}
		}
	}
}

ObjectID UBuildingSystem::HitTest(const FVector2D &Location)
{
	ObjectID num = INDEX_NONE;
	if (Suite)
	{
		kPoint Loc = ToBuildingPosition(Location);
		num = Suite->HitTest(Loc);
	}
	return num;
}

ObjectID UBuildingSystem::IsHitCornner(const FVector2D &Location)
{
	ObjectID HitObj = INVALID_OBJID;
	if (Suite)
	{
		ObjectID BestID = INVALID_OBJID;
		kPoint BestLocation;
		kPoint Loc = ToBuildingPosition(Location);
		if (kEPt == Suite->FindSnapLocation(Loc, BestLocation, BestID, INVALID_OBJID, ESFCorner, 5.0f))
		{
			HitObj = BestID;
		}
	}
	return HitObj;
}

bool UBuildingSystem::Move(ObjectID objID, const FVector2D &DeltaMove)
{
	if (Suite)
	{
		kPoint Loc= ToBuildingPosition(DeltaMove);
		return Suite->Move(objID, Loc);
	}
	return false;
}

void UBuildingSystem::SetWallHightConfig(float Height)
{
	if (Suite)
	{
		IObject *Cfg = Suite->GetConfig();
		if (Cfg)
		{
			Cfg->SetPropertyValue("WallHeight", &GetValueFactory()->Create(Height));
		}
	}
}

void UBuildingSystem::LoadingConfig(FBuildingConfig * Config)
{
	if (!Suite)
	{
		IBuildingSDK *pSDK = GetBuildingSDK();
		if (pSDK)
		{
			Suite = pSDK->CreateSuite("");
			Suite->SetListener(this);
		}
	}
	if (Suite)
	{
		IObject *Cfg = Suite->GetConfig();
		Cfg->SetPropertyValue("WallMaterial", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->WallMaterial->GetPathName())));
		Cfg->SetPropertyValue("WallMaterialType", &GetValueFactory()->Create(Config->WallMaterialType));
		Cfg->SetPropertyValue("FloorMaterial", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->FloorMaterial->GetPathName())));
		Cfg->SetPropertyValue("FloorMaterialType", &GetValueFactory()->Create(Config->FloorMaterialType));
		Cfg->SetPropertyValue("CeilMaterial", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->WallMaterial->GetPathName())));
		Cfg->SetPropertyValue("CeilMaterialType", &GetValueFactory()->Create(Config->CeilMaterialType));
		Cfg->SetPropertyValue("bCeilVisible", &GetValueFactory()->Create(Config->bCeilVisible));

		Cfg->SetPropertyValue("Tolerance", &GetValueFactory()->Create(Config->Tolerance));
		//Suite->SetProperty(ConfigID, "SkirtingCeil", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->SkirtingCeil)));
		//Suite->SetProperty(ConfigID, "SkirtingCeilExt", &GetValueFactory()->Create(&ToBuildingPosition(Config->SkirtingCeilExt)));
		//Suite->SetProperty(ConfigID, "SkirtingFloor", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->SkirtingFloor)));
		//Suite->SetProperty(ConfigID, "SkirtingFloorExt", &GetValueFactory()->Create(&ToBuildingPosition(Config->SkirtingFloorExt)));
		Cfg->SetPropertyValue("DefaultDoor", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->DefaultDoor)));
		Cfg->SetPropertyValue("DefaultDoorFrame", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->DefaultDoorFrame)));
		Cfg->SetPropertyValue("DefaultWindow", &GetValueFactory()->Create(TCHAR_TO_ANSI(*Config->DefaultWindow)));
	}
}

void UBuildingSystem::SetConfigTolerance(const float Num)
{
	if (Suite)
	{
		IObject *Cfg = Suite->GetConfig();
		if (Cfg)
		{
			Cfg->SetPropertyValue("Tolerance", &GetValueFactory()->Create(Num));
		}
	}
}

void UBuildingSystem::SetConfigWallHeight(float WallHeight)
{
	if (Suite)
	{
		IObject *Cfg = Suite->GetConfig();
		if (Cfg)
		{
			Cfg->SetPropertyFloat("WallHeight", WallHeight);
		}

	}
}

void UBuildingSystem::SetHouseImage(FDRHouseImage _HI)
{
	if (Suite)
	{
		IObject *Cfg = Suite->GetConfig();
		if (Cfg)
		{
			IValue & HI = GetValueFactory()->Create();
			/*CopyMode,SpinValue,SceneWorldDistance,Path,Scene0Location,SLocation,PlaneLocation,PlaneRotation,PlaneScale*/
			HI.AddField("CopyMode", GetValueFactory()->Create(_HI.CopyMode));
			HI.AddField("SpinValue", GetValueFactory()->Create(_HI.SpinValue));
			HI.AddField("SceneWorldDistance", GetValueFactory()->Create(_HI.SceneWorldDistance));
			HI.AddField("Path", GetValueFactory()->Create(TCHAR_TO_ANSI(*(_HI.Path))));
			kVector3D Scene0Location = ToBuildingVector(_HI.Scene0Location);
			HI.AddField("Scene0Location", GetValueFactory()->Create(&Scene0Location));
			kVector3D SLocation = ToBuildingVector(_HI.SLocation);
			HI.AddField("SLocation", GetValueFactory()->Create(&SLocation));
			kVector3D PlaneLocation = ToBuildingVector(_HI.PlaneLocation);
			HI.AddField("PlaneLocation", GetValueFactory()->Create(&PlaneLocation));
			kVector3D PlaneRotation = ToBuildingVector(_HI.PlaneRotation);
			HI.AddField("PlaneRotation", GetValueFactory()->Create(&PlaneRotation));
			kVector3D PlaneScale = ToBuildingVector(_HI.PlaneScale);
			HI.AddField("PlaneScale", GetValueFactory()->Create(&PlaneScale));
			Cfg->SetValue("HouseImage", &HI);
		}
	}
}


void UBuildingSystem::GetHouseImage(FDRHouseImage & HouseImage)
{
	if (Suite)
	{
		IObject *Cfg = Suite->GetConfig();
		if (Cfg)
		{
			IValue* HIData = Cfg->FindValue("HouseImage");
			if (HIData)
			{
				HouseImage.CopyMode = HIData->GetField("CopyMode").BoolValue();
				HouseImage.SpinValue = HIData->GetField("SpinValue").FloatValue();
				HouseImage.SceneWorldDistance = HIData->GetField("SceneWorldDistance").FloatValue();
				HouseImage.Path = HIData->GetField("Path").StrValue();
				HouseImage.Scene0Location = ToUE4Vector(HIData->GetField("Scene0Location").Vec3Value());
				HouseImage.SLocation = ToUE4Vector(HIData->GetField("SLocation").Vec3Value());
				HouseImage.PlaneLocation = ToUE4Vector(HIData->GetField("PlaneLocation").Vec3Value());
				HouseImage.PlaneRotation = ToUE4Vector(HIData->GetField("PlaneRotation").Vec3Value());
				HouseImage.PlaneScale = ToUE4Vector(HIData->GetField("PlaneScale").Vec3Value());
			}
		}
	}
}


int32 UBuildingSystem::GetPolygon(int32 objID, TArray<FVector2D>& TPolygons, bool binner)
{
	int32 num = INVALID_OBJID;
	TPolygons.Empty();
	if (Suite)
	{
		if (objID != INVALID_OBJID)
		{
			IObject* Obj = GetObject(objID);
			if (Obj == nullptr)
				return num;
			kArray<kPoint> Polygons = Obj->GetPropertyVector2DArray("Boundary");
			num = Polygons.size();
			for (int i = 0; i < num; ++i)
			{
				TPolygons.Add(ToVector2D(Polygons[i]));
			}
		}
	}
	return num;
}

int	UBuildingSystem::HitTestMinMax(const FVector2D &Min, const FVector2D &Max, int32 *&pResults)
{
	int32 num = INVALID_OBJID;
	if (Suite)
	{
		pResults = nullptr;
		kPoint LocMin= ToBuildingPosition(Min);
		kPoint LocMax = ToBuildingPosition(Max);
		num = Suite->HitTest(LocMin, LocMax, pResults);
	}
	return num;
}

int32 UBuildingSystem::GetRoomByLocation(const FVector2D &Loc)
{
	int32 Objid= INVALID_OBJID;
	if (Suite)
	{
		kPoint KPLoc= ToBuildingPosition(Loc);
		Objid=Suite->GetAreaByLocation(KPLoc);
	}
	return Objid;
}

TArray<int32>  UBuildingSystem::AddVirtualWall(int32 StartCorner, int32 EndCorner)
{
	TArray<int32> VirWallIDS;
	if (Suite)
	{
		kArray<int>  Temp = Suite->AddVirtualWall(StartCorner, EndCorner);
		for (int i = 0;i < Temp.size();++i)
		{
			VirWallIDS.Add(Temp[i]);
		}
	}
	return VirWallIDS;
}

bool UBuildingSystem::GetWallVector(int32 WallID, FVector2D &P0, FVector2D &P1, FVector2D &Right)
{
	bool IsSuccess = false;
	if (Suite)
	{
		kPoint KP0 = ToBuildingPosition(P0);
		kPoint KP1 = ToBuildingPosition(P1);
		kPoint KPR = ToBuildingPosition(Right);
		IsSuccess = Suite->GetWallVector(WallID, KP0, KP1, KPR);
		if(IsSuccess)
		{
			P0 = ToVector2D(KP0);
			P1 = ToVector2D(KP1);
			Right = ToVector2D(KPR);
		}
	}
	return IsSuccess;
}

int32 UBuildingSystem::GetConnectWalls(int32 objID, int32 *&pConnectedWalls)
{
	int32 num = INVALID_OBJID;
	if (Suite&&objID != INVALID_OBJID)
	{
		pConnectedWalls = nullptr;
		num = Suite->GetConnectWalls(objID, pConnectedWalls);
	}
	return num;
}

UPrimitiveComponent *UBuildingSystem::HitTestPrimitiveByCursor(UObject *WorldContextObject)
{
	UWorld *MyWorld = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	UGameViewportClient *GameViewportClient = MyWorld ? MyWorld->GetGameViewport() : nullptr;
	FSceneViewport *Viewport = GameViewportClient? GameViewportClient->GetGameViewport() : nullptr;
	if (Viewport)
	{
		int32 MouseX = Viewport->GetMouseX();
		int32 MouseY = Viewport->GetMouseY();

		HActor *HitActor = HitProxyCast<HActor>(Viewport->GetHitProxy(MouseX, MouseY));
		if (HitActor)
		{
			return const_cast<UPrimitiveComponent *>(HitActor->PrimComponent);
		}
	}
	return nullptr;
}

// int32 UBuildingSystem::HitTestSurfaceByCursor(UObject *WorldContextObject)
// {
// 	UWorld *MyWorld = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
// 	UGameViewportClient *GameViewportClient = MyWorld ? MyWorld->GetGameViewport() : nullptr;
// 	FSceneViewport *Viewport = GameViewportClient ? GameViewportClient->GetGameViewport() : nullptr;
// 	if (Viewport)
// 	{
// 		int32 MouseX = Viewport->GetMouseX();
// 		int32 MouseY = Viewport->GetMouseY();
// 
// 		GameViewportClient->DePr
// 
// 		HActor *HitActor = HitProxyCast<HActor>(Viewport->GetHitProxy(MouseX, MouseY));
// 		if (HitActor)
// 		{
// 			UPrimitiveComponent *Comp =  const_cast<UPrimitiveComponent *>(HitActor->PrimComponent);
// 
// 		}
// 	}
// 	return nullptr;
// }

bool UBuildingSystem::GetWallBorderLines(int32 WallID, TArray<FVector>&WallNodes, float &ZPos)
{
	bool bSuccess = false;
	if (Suite&&WallID != INVALID_OBJID)
	{
		UBuildingData *WallData = GetData(WallID);
		if (WallData)
		{
			ZPos = WallData->GetFloat("Height");
			kLine2D CenterLine;
			kLine2D LeftLine;
			kLine2D RightLine;
			bSuccess=Suite->GetWallBorderLines(WallID, CenterLine, LeftLine, RightLine);
			if (bSuccess)
			{
				WallNodes.Empty();
				WallNodes.Add(ToUE4Vector(CenterLine.start));
				WallNodes.Add(ToUE4Vector(LeftLine.start));
				WallNodes.Add(ToUE4Vector(LeftLine.end));
				WallNodes.Add(ToUE4Vector(CenterLine.end));
				WallNodes.Add(ToUE4Vector(RightLine.end));
				WallNodes.Add(ToUE4Vector(RightLine.start));
			}
		}
	}
	return bSuccess;
}

void UBuildingSystem::OnUpdateSurfaceValue(IObject *RawObj, int SectionIndex, ObjectID SurfaceID)
{
	int32 ObjID = RawObj->GetID();
	FObjectInfo *ObjInfo = ObjMap.Find(ObjID);
	if (ObjInfo)
	{
		for (int32 i = 0; i < ObjInfo->Actorts.Num(); ++i)
		{
			ADRActor *Actor = Cast<ADRActor>(ObjInfo->Actorts[i]);
			UBuildingComponent *BuildingComp = Actor ? Actor->FindComponentByClass<UBuildingComponent>() : nullptr;
			if (BuildingComp)
			{
				BuildingComp->UpdateSurface(SectionIndex, SurfaceID);
			}
		}
	}
}

UWallBuildSystem* UBuildingSystem::GetWallBuildSystem()
{
	UWorld *MyWorld = GetWorld();
	UWallBuildSystem *WallSystem=nullptr;
	if (MyWorld)
	{
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
		if (GameInst)
			WallSystem = GameInst->WallBuildSystem;
	}
	return WallSystem;
}

IValue* UBuildingSystem::GetProperty(ObjectID ID, const char *PropertyName)
{
	IValue *v = nullptr;
	if (Suite)
	{
		v=&Suite->GetProperty(ID, PropertyName);
	}
	return v;
}

FVector2D UBuildingSystem::CalculateCrossoverpoint(FVector2D Line1Point1, FVector2D Line1Point2, FVector2D Line2Point1, FVector2D Line2Point2)
{
	float k1;
	float b1;
	if ((Line1Point1.X - Line1Point2.X)> -0.0001 && (Line1Point1.X - Line1Point2.X) < 0.0001)
	{
		k1 = -9999;
		b1 = Line1Point1.X;
	}
	else
	{
		k1 = (Line1Point1.Y - Line1Point2.Y) / (Line1Point1.X - Line1Point2.X);
		b1 = Line1Point1.Y - k1 * Line1Point1.X;
	}
	
	float k2;
	float b2;
	if ((Line2Point1.X - Line2Point2.X) > -0.0001 && (Line2Point1.X - Line2Point2.X) < 0.0001)
	{
		k2 = -9999;
		b2 = Line2Point1.X;
	}
	else
	{
		k2 = (Line2Point1.Y - Line2Point2.Y) / (Line2Point1.X - Line2Point2.X);
		b2 = Line2Point1.Y - k2 * Line2Point1.X;
	}

	float x = -9999;
	float y = -9999;

	if (k1 == -9999)
	{
		x = b1;
		if (k2 == 0)
		{
			y = b2;
		}
		else
		{
			y = x * k2 + b2;
		}
	}
	else if (k2 == -9999)
	{
		x = b2;
		if (k1 == 0)
		{
			y = b1;
		}
		else
		{
			y = x * k1 + b1;
		}
	}
	else if (k1 == 0)
	{
		if (k2 != -9999)
		{
			y = b1;
			x = (b2 - b1) / (k1 - k2);
		}
	}
	else if (k2 == 0)
	{
		if (k1 != -9999)
		{
			y = b2;
			x = (b2 - b1) / (k1 - k2);
		}
	}

	if (k1 != -9999 && k2 != -9999 && k1 != 0 && k2 != 0)
	{
		x = (b2 - b1) / (k1 - k2);
		y = (b2 * k1 - b1 * k2) / (k1 - k2);
	}

	return FVector2D(x, y);
}

TArray<int32> UBuildingSystem::AddEdge(const FVector2D &P0, const FVector2D &P1, int32 SnapObj0, int32 SnapObj1, bool bBreak)
{
	TArray<int32> VirWallIDS;
	if (Suite)
	{
		kPoint Loc1 = ToBuildingPosition(P0);
		kPoint loc2 = ToBuildingPosition(P1);
		if (SnapObj0 == SnapObj1 && SnapObj0 != -1)
		{
			SnapObj0 = -1;
			SnapObj1 = -1;
		}
		IObject * HitObj_0 = GetObject(SnapObj0);
		int TempType_0 = -1;
		if (HitObj_0)
		{
			TempType_0 = HitObj_0->GetType();
			if (TempType_0 != EObjectType::ESolidWall)
				SnapObj0 = -1;
			int HitObjId = HitTest(P0);
			IObject * HitObj = GetObject(HitObjId);
			int TempType = -1;
			if (HitObj)
			{
				TempType = HitObj->GetType();
				if (TempType == EObjectType::ESolidWall &&HitObjId != SnapObj0)
					SnapObj0 = -1;
			}
		}
		IObject * HitObj_1 = GetObject(SnapObj1);
		int TempType_1 = -1;
		if (HitObj_1)
		{
			TempType_1 = HitObj_1->GetType();
			if (TempType_1 != EObjectType::ESolidWall)
				SnapObj1 = -1;
			int HitObjId = HitTest(P1);
			IObject * HitObj = GetObject(HitObjId);
			int TempType = -1;
			if (HitObj)
			{
				TempType = HitObj->GetType();
				if (TempType == EObjectType::ESolidWall &&HitObjId != SnapObj1)
					SnapObj1 = -1;
			}
		}
		kArray<int>  Temp = Suite->AddEdge(false, Loc1, loc2, SnapObj0, SnapObj1, bBreak);
		if (Temp.size() <= 0 || !Temp.data())
		{
			return VirWallIDS;
		}
		for (int i = 0; i < Temp.size(); ++i)
		{
			VirWallIDS.Add(Temp[i]);
		}

	}
	return VirWallIDS;
}

int32 UBuildingSystem::CutAreaSnap(const FVector2D &Loc, int32 Ignore, FVector2D &BestLoc, int32 &BestID, float Torlerance, float ToleranceEdge)
{
	if (Suite)
	{
		const unsigned int Op_AddWallFilter = ESFWallBorder;
		int32 IsSnap = Suite->FindSnapLocation(FORCE_TYPE(kPoint, Loc), FORCE_TYPE(kPoint, BestLoc), BestID, Ignore, ESnapFilter(Op_AddWallFilter), Torlerance, ToleranceEdge);
		if (IsSnap == 0)
			BestID = -1;
		return IsSnap;
	}
	return kESnapType::kENone;
}

int32 UBuildingSystem::Break(int32 EdgeID, FVector2D &Location)
{
	int32 BreakEdgeID = -1;
	if (Suite)
	{
		BreakEdgeID = Suite->Break(EdgeID, FORCE_TYPE(kPoint, Location));
		return BreakEdgeID;
	}
	return BreakEdgeID;
}

void UBuildingSystem::UpdateCorner()
{
	TArray<int32> ObjIDs = GetAllTypedObjects(EDR_ObjectType::EDR_Corner);
	for (int i = 0; i < ObjIDs.Num(); ++i)
		Move(ObjIDs[i], FVector2D(0, 0));
}