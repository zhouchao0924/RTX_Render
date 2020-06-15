
#include "EditorGameInstance.h"
#include "HAL/FileManagerGeneric.h"
#include "PeExtendedToolKitBPLibrary.h"
#include "ResourceMgr.h"
#include "HomeLayout/HouseArchitect/SkirtingSystem.h"
#include "HomeLayout/Paper/ProjectMgr.h"
#include "ISuite.h"
#include "public/DCFBlueprintFunctionLibrary.h"
#include "../Data/FunctionLibrary/DROperationHouseFunctionLibrary.h"

UCEditorGameInstance::UCEditorGameInstance(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)	
	, SkirtingSystem(nullptr)
	, DrawHouse_Type(EDrawHouseType::NormalDrawType)
	, IsCanAutoSaveByDemolition(true)
	, bIsFirstOpenFile(true)
{
}

void UCEditorGameInstance::Init()
{
	Super::Init();
	UDROperationHouseFunctionLibrary::SetGameInstance(this);
	SkirtingSystem = nullptr;
}

void UCEditorGameInstance::Shutdown()
{
	Super::Shutdown();
}

ASkirtingSystem* UCEditorGameInstance::GetSkirtingSystem()
{
	if (SkirtingSystem)
	{
		return SkirtingSystem;
	}

	UWorld *MyWorld = GetWorld();
	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SkirtingSystem = MyWorld->SpawnActor<ASkirtingSystem>(SpawmParams);

	return SkirtingSystem;
}

UBuildingSystem * UCEditorGameInstance::GetBuildingSystemInstance()
{
	if (!BuildingSystem) {
		BuildingSystem = UBuildingSystem::CreateNewSuite(this, TEXT("SuiteBuildingSystem"));
	}

	return BuildingSystem;
}

void UCEditorGameInstance::GetAnchorAllObjID(const int32 & AnchorID, TArray<int> & Obj_IDs)
{
	ISuite *Suite = GetBuildingSystemInstance()->GetSuite();
	if (Suite)
	{
		IValue &Value = Suite->GetProperty(AnchorID, "LinkObjects");
		kArray<int> _Ver = Value.IntArrayValue();
		for (int i = 0; i < _Ver.size(); ++i)
		{
			Obj_IDs.Add(_Ver[i]);
		}
	}
}
bool UCEditorGameInstance::IsAnchorWindowType(UBuildingData * BuildData, int32 Anchor)
{
	if (BuildData)
	{
		IObject * _Obj = GetBuildingSystemInstance()->GetObject(Anchor);
		if (_Obj)
		{
			if (_Obj->GetType() == EObjectType::EWindow)
			{
				return true;
			}
		}
	}
	return false;
}

bool UCEditorGameInstance::IsAnchorDoorType(UBuildingData * BuildData, int32 Anchor)
{
	if (BuildData)
	{
		IObject * _Obj = GetBuildingSystemInstance()->GetObject(Anchor);
		if (_Obj)
		{
			if (_Obj->GetType() == EObjectType::EDoorHole)
			{
				return true;
			}
		}
	}
	return false;
}
bool UCEditorGameInstance::IsAnchorModelType(UBuildingData * BuildData, int32 Anchor)
{
	if (BuildData)
	{
		IObject * _Obj = GetBuildingSystemInstance()->GetObject(Anchor);
		if (_Obj)
		{
			if (_Obj->GetType() == EObjectType::EModelInstance)
			{
				return true;
			}
		}
	}
	return false;
}

ADRComponentModel * UCEditorGameInstance::GetComponentActor(int32 HoleID)
{
	ADRComponentModel * _TCM = nullptr;
		TArray<AActor*> _As;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADRComponentModel::StaticClass(), _As);
		if (_As.Num() > 0)
		{
			for (int i = _As.Num() - 1; i >= 0; --i)
			{
				_TCM = Cast<ADRComponentModel>(_As[i]);
				if (_TCM)
				{
					if (_TCM->HoleStruct.HoleID == HoleID /*&& _TCM->ComModelType == EDRModelType::EDRDoorFrame*/)
					{
						return _TCM;
					}
				}
			}
		}
	
	return nullptr;
}

int32 UCEditorGameInstance::AddModelToObject(int32 BaseObjID, FDRModelInstance Data)
{
	int32 ID = INVALID_OBJID;
	UBuildingSystem * _BS = GetBuildingSystemInstance();
	UDRModelFactory * _MF = GetCreateFactory();
	if (_BS && _MF)
	{
		ID = _BS->AddModelToObject(BaseObjID, Data.ResID, Data.Location);
		_MF->ModelData.Add(ID, Data);
		return ID;
	}
	return ID;
}


UDRModelFactory * UCEditorGameInstance::GetCreateFactory()
{
	UBuildingSystem * _BS = GetBuildingSystemInstance();
	if (_BS)
	{
		UDRModelFactory *  _MF = Cast<UDRModelFactory>(_BS->BuilldActorFactory);
		if (_MF)
			return _MF;
	}
	return nullptr;
}

FString UCEditorGameInstance::StringFromBinaryArray(TArray<uint8> BinaryArray)
{
	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
}

int UCEditorGameInstance::GetCurrentSolutionID_Implementation() const
{
	return 0;
}