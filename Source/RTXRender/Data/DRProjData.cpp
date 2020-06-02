

#include "DRProjData.h"
#include "Adapter/DRLightAdapter.h"
#include "Adapter/DRAreaAdapter.h"
#include "Unidentity/DRCurtainData.h"
#include "Adapter/DRPostProcessAdapter.h"
#include "Adapter/DRModelInstanceAdapter.h"
#include "Adapter/DRDoorHoleAdapter.h"
#include "Adapter/DRWindowHoleAdapter.h"
#include "Adapter/DRSolidWallAdapter.h"
#include "Adapter/DRVirtualWallAdapter.h"
#include "Adapter/DRMaterialAdapter.h"
#include "Adapter/DRPointLightAdapter.h"
#include "Adapter/DRSpotLightAdapter.h"
#include "Adapter/DRDoorAdapter.h"
#include "Adapter/DRWindowAdapter.h"
#include "Adapter/DRCornerAdapter.h"
#include "Adapter/DRBorderCornerAdapter.h"
#include "EditorGameInstance.h"
#include "GameFramework/Actor.h"


#define REG_ADAPTER(adapter_type){																					\
	adapter_type *adapter = ObjectIntializer.CreateDefaultSubobject<adapter_type>(this, TEXT(#adapter_type));		\
	adapter->ProjData = this;																						\
	Adapters.Add((int32)adapter->GetObjectType(), adapter);															\
}

UDRProjData* UDRProjData::_projectDataManager = nullptr;

UDRProjData* UDRProjData::GetProjectDataManager(const UObject* WorldContextObject)
{
	if (_projectDataManager == nullptr) {

		do {

			if (WorldContextObject == nullptr) {
				break;
			}

			UWorld* currentWorld = GEngine->GetWorldFromContextObject(WorldContextObject);

			if (currentWorld == nullptr) {
				break;
			}

			_projectDataManager = NewObject<UDRProjData>(currentWorld);
			_projectDataManager->AddToRoot();

			UCEditorGameInstance* _gameInstance = Cast<UCEditorGameInstance>(currentWorld->GetGameInstance());
			if (_gameInstance == nullptr) {
				break;
			}

			UBuildingSystem* _buildingSystem = _gameInstance->GetBuildingSystemInstance();
			if (_buildingSystem == nullptr) {
				break;
			}

			_projectDataManager->Suite = _buildingSystem->Suite;
		} while (0);
	}

	return _projectDataManager;
}

void UDRProjData::ClearProjectDataManager()
{
	if (_projectDataManager == nullptr) {
		return;
	}

	_projectDataManager->RemoveFromRoot();
	_projectDataManager = nullptr;
}

UDRProjData::UDRProjData(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
	, Suite(nullptr)
{
	REG_ADAPTER(UDRLightAdapter)
	REG_ADAPTER(UDRAreaAdapter)
	REG_ADAPTER(UDRPostProcessAdapter)
	REG_ADAPTER(UDRModelInstanceAdapter)
	REG_ADAPTER(UDRDoorHoleAdapter)
	REG_ADAPTER(UDRWindowHoleAdapter)
	REG_ADAPTER(UDRSolidWallAdapter)
	REG_ADAPTER(UDRVirtualWallAdapter)
	REG_ADAPTER(UDRPointLightAdapter)
	REG_ADAPTER(UDRSpotLightAdapter)
	REG_ADAPTER(UDRCornerAdapter)
	REG_ADAPTER(UDRBorderCornerAdapter)
	//REG_ADAPTER(UDRDoorAdapter)
	//REG_ADAPTER(UDRWindowAdapter)
	DoorAdapter = ObjectIntializer.CreateDefaultSubobject<UDRDoorAdapter>(this, TEXT("UDRDoorAdapter"));
	DoorAdapter->ProjData = this;
	WindowAdapter = ObjectIntializer.CreateDefaultSubobject<UDRWindowAdapter>(this, TEXT("UDRWindowAdapter"));
	WindowAdapter->ProjData = this;
	MaterialAdapter = ObjectIntializer.CreateDefaultSubobject<UDRMaterialAdapter>(this, TEXT("MaterialAdapter"));
}

UDataAdapter *UDRProjData::GetAdapter(int32 ObjID)
{
	if (Suite)
	{
		IObject *pObj = Suite->GetObject(ObjID);
		if (pObj)
		{
			EObjectType ObjectType = pObj->GetType();
			UDataAdapter **ppAdapter = Adapters.Find((int32)ObjectType);
			if (ObjectType == EObjectType::EPolygonArea)
			{
				ppAdapter = Adapters.Find((int32)EObjectType::ERoom);
			}
			if (ppAdapter)
			{
				if (ObjectType == EObjectType::EModelInstance)
				{
					UDRModelInstanceAdapter *ModelAdapter = Cast<UDRModelInstanceAdapter>(*ppAdapter);
					if (ModelAdapter != nullptr)
					{
						ModelAdapter->Obj = pObj;
						EModelType ModelType = ModelAdapter->GetType();
						if (ModelType == EModelType::EDRWindow || ModelType == EModelType::EDRGroundWindow || ModelType == EModelType::EDRBayWindow)
						{
							WindowAdapter->Obj = pObj;
							return WindowAdapter;
						}
						else if (ModelType == EModelType::EDRDoor || ModelType == EModelType::EDRDoubleDoor || ModelType == EModelType::EDRSliderDoor \
							|| ModelType == EModelType::EDRDoorWay || ModelType == EModelType::EDRDoorFram)
						{
							DoorAdapter->Obj = pObj;
							return DoorAdapter;
						}
						else
						{
							(*ppAdapter)->Obj = pObj;
							return *ppAdapter;
						}
					}
				}
				else
				{
					(*ppAdapter)->Obj = pObj;
					return *ppAdapter;
				}
			}
		}
	}
	return nullptr;
}

void UDRProjData::AddProjData(UDRData *Data)
{
	if (Data)
	{
		if (UDRCurtainData *CurtainItem = Cast<UDRCurtainData>(Data))
		{
			CurtainData.Add(CurtainItem);
		}
	}
}

void UDRProjData::RemoveProjData(UDRData* Data)
{
	if (Data == nullptr) {
		return;
	}

	if (UDRCurtainData* CurtainItem = Cast<UDRCurtainData>(Data)) {
		CurtainData.Remove(CurtainItem);
	}
}

void UDRProjData::AddObjectContext(int32 ObjectId, UObject * UEObject, EDR_ObjectType ObjectType)
{
	if (ObjectId == INVALID_OBJID) {
		return;
	}

	switch (ObjectType) {
	case EDR_ObjectType::EDR_Room:
	case EDR_ObjectType::EDR_PolygonArea:
		AreaMap.Add(ObjectId, UEObject); 
		break;

	case EDR_ObjectType::EDR_SolidWall:
		SolidWallMap.Add(ObjectId, UEObject);
		break;

	case EDR_ObjectType::EDR_DoorHole:
		DoorHoleMap.Add(ObjectId, UEObject);
		break;

	case EDR_ObjectType::EDR_Window:
		WindowHoleMap.Add(ObjectId, UEObject);
		break;

	case EDR_ObjectType::EDR_ModelInstance:
	{
		UDRModelInstanceAdapter* ModelAdapter = Cast<UDRModelInstanceAdapter>(GetAdapter(ObjectId));
		if (ModelAdapter == nullptr) {
			break;
		}
		EModelType ModelType = ModelAdapter->GetType();

		switch (ModelType)
		{
		case EModelType::EDRNull:
			break;
		case EModelType::EDRNormalModel:
			NormalModelMap.Add(ObjectId, UEObject);
			break;
		default:
			break;
		}
		break;
	}

	case EDR_ObjectType::EDR_PointLight:
		PointLightMap.Add(ObjectId, UEObject);
		break;

	case EDR_ObjectType::EDR_SpotLight:
		SpotLightMap.Add(ObjectId, UEObject);
		break;

	default:
		break;
	}
}

void UDRProjData::AddDoorAndWindowContext(int32 ObjectId, UObject* UEObject, bool bIsDoor)
{
	if (ObjectId == INVALID_OBJID) {
		return;
	}
	if (bIsDoor)
	{
		DoorMap.Add(ObjectId, UEObject);
	}
	else
	{
		WindowMap.Add(ObjectId, UEObject);
	}
}

void UDRProjData::RemoveObjectContext(int32 ObjectId, EDR_ObjectType ObjectType)
{
	if (ObjectId == INVALID_OBJID) {
		return;
	}

	UObject** objectForDestroy(nullptr);

	switch (ObjectType) {
	case EDR_ObjectType::EDR_Room:
	case EDR_ObjectType::EDR_PolygonArea:
		objectForDestroy = AreaMap.Find(ObjectId);
		AreaMap.Remove(ObjectId);
		break;
		
	case EDR_ObjectType::EDR_SolidWall:
		objectForDestroy = SolidWallMap.Find(ObjectId);
		SolidWallMap.Remove(ObjectId);
		break;

	case EDR_ObjectType::EDR_ModelInstance:
	{
		UDRModelInstanceAdapter* ModelAdapter = Cast<UDRModelInstanceAdapter>(GetAdapter(ObjectId));
		if (ModelAdapter == nullptr) {
			break;
		}
		EModelType ModelType = ModelAdapter->GetType();

		switch (ModelType)
		{
		case EModelType::EDRNull:
			break;
		case EModelType::EDRNormalModel:
			objectForDestroy = NormalModelMap.Find(ObjectId);
			NormalModelMap.Remove(ObjectId);
			break;
		default:
			break;
		}
		break;
	}

	case EDR_ObjectType::EDR_PointLight:
		objectForDestroy = PointLightMap.Find(ObjectId);
		PointLightMap.Remove(ObjectId);
		break;

	case EDR_ObjectType::EDR_SpotLight:
		objectForDestroy = SpotLightMap.Find(ObjectId);
		SpotLightMap.Remove(ObjectId);
		break;

	case EDR_ObjectType::EDR_Window:
		objectForDestroy = WindowHoleMap.Find(ObjectId);
		WindowHoleMap.Remove(ObjectId);
		break;

	case EDR_ObjectType::EDR_DoorHole:
		objectForDestroy = DoorHoleMap.Find(ObjectId);
		DoorHoleMap.Find(ObjectId);
		break;

	default:
		break;
	}

	if (objectForDestroy == nullptr) {
		return;
	}

	AActor* actorForDestroy = Cast<AActor>(*objectForDestroy);
	if (actorForDestroy == nullptr) {
		return;
	}

	actorForDestroy->Destroy();
}

void UDRProjData::RemoveDoorAndWindowContext(int32 ObjectId)
{
	if (ObjectId == INVALID_OBJID) {
		return;
	}

	UObject** objectForDestroy(nullptr);
	AActor* actorForDestroy(nullptr);

	objectForDestroy = DoorMap.Find(ObjectId);
	if (objectForDestroy) {
		actorForDestroy = Cast<AActor>(*objectForDestroy);
		actorForDestroy->Destroy();
	}

	objectForDestroy = WindowMap.Find(ObjectId);
	if (objectForDestroy) {
		actorForDestroy = Cast<AActor>(*objectForDestroy);
		actorForDestroy->Destroy();
	}

	DoorMap.Remove(ObjectId);
	WindowMap.Remove(ObjectId);
}

template<typename DataType>
void SerializeObjArray(int32 ChunkID, ISerialize &Ar, TArray<DataType *> &Datas)
{
	if (Ar.IsSaving())
	{
		Ar.WriteChunk(ChunkID);

		int Num = Datas.Num();
		Ar << Num;
		for (int i = 0; i < Num; ++i)
		{
			Datas[i]->SerializeData(Ar);
		}

		Ar.EndChunk(ChunkID);
	}
	else if (Ar.IsLoading())
	{
		int ID = Ar.ReadChunk();
		if (ID == ChunkID)
		{
			int Num = 0;
			Ar << Num;
			Datas.SetNum(Num);
			for (int i = 0; i < Num; ++i)
			{
				Datas[i] = NewObject<DataType>();
				Datas[i]->SerializeData(Ar);
			}
		}
	}
}

void UDRProjData::SerializeDataByVersion(ISerialize &Ar, int32 Ver)
{
	SerializeMap<int32, FString>(Ar, Model2ResIDMapping, CHUNK_DR_MODEL2RESMAPPING); //modelid->resid 映射表

	// 窗帘对象列表
	SerializeObjArray<UDRCurtainData>(CHUNK_DR_CurtainData, Ar, CurtainData);
}

UDRMaterialAdapter *UDRProjData::GetMaterialAdapter(int32 ObjID, int32 SectionIndex)
{
	if (Suite)
	{
		IObject *pObj = Suite->GetObject(ObjID);
		return GetMaterial(pObj, SectionIndex);
	}
	return nullptr;
}

UDRMaterialAdapter *UDRProjData::GetMaterial(IObject *Obj, int32 SectionIndex)
{
	if (Suite && MaterialAdapter && Obj)
	{
		MaterialAdapter->Obj = Obj;
		MaterialAdapter->SectionIndex = SectionIndex;
		return MaterialAdapter;
	}
	return nullptr;
}
