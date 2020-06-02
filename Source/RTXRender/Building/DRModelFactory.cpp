

#include "DRModelFactory.h"
#include "IClass.h"
#include "DRModelActor.h"
#include "DRFunLibrary.h"
#include "DRGameMode.h"
#include "DRComponentModel.h"
#include "Building/BuildingSystem.h"
#include "Model/ModelFileComponent.h"

ADRActor * UDRModelFactory::CreateActorByData_Implementation(UBuildingData * BuildingData)
{
	switch (BuildingData->GetObjectType())
	{
	case EObjectType::EBuildingObject:
		return nullptr;
	case EObjectType::EBuildingConfig:
		return nullptr;
	case EObjectType::EModelInstance:
		return CreateModel(BuildingData);
	case EObjectType::EWindow:
		return nullptr;
	case EObjectType::EDoorHole:
		return nullptr;
	};
	return nullptr;
}

ADRActor * UDRModelFactory::CreateComponentModel(UBuildingData *& Data,int32 & AnchorID)
{
	if (World)
	{
		ADRComponentModel * _Model = World->SpawnActor<ADRComponentModel>();

		_Model->BuildData = Data;
		if (isLoading)//
		{
			if (Data)
			{
				UBuildingSystem *_BS = Data->GetBuildingSystem();
				if (_BS)
				{
					int32 _DoorID = Data->GetInt(TEXT("Door"));
					int32 _DoorFrame = Data->GetInt(TEXT("DoorFrame"));
					int32 _DoorSone = Data->GetInt(TEXT("DoorSone"));
				}
			}
			return Cast<ADRActor>(_Model);
		}
		else
		{
			int32 _Type = Data->GetObjectType();
			//FTempComModelData * _CM = ComModelData.Find(AnchorID);
			//if (_CM)
			//{
			//	_Model->isLoadingData = true;
			//	_Model->ComModelType = _CM->ModelType;
			//
			//	_Model->ModelStruct.ModelType = _CM->ModelType;
			//	_Model->ModelStruct.ModelID = _CM->ModelID;
			//	_Model->ModelStruct.BindID = _CM->BindID;
			//	_Model->ModelStruct.isShadow = _CM->isShadow;
			//	_Model->ModelStruct.Location = _CM->ModelLocation;
			//	_Model->ModelStruct.Rotation = _CM->ModelRotation;
			//	_Model->ModelStruct.Scale = _CM->ModelScale;
			//	_Model->ModelStruct.ResID = _CM->ResID;
			//
			//	_Model->HoleStruct.HoleID = AnchorID;
			//	_Model->HoleStruct.ModelType = _CM->ModelType;
			//	_Model->HoleStruct.Location = _CM->Location;
			//	_Model->HoleStruct.Width = _CM->Width;
			//	_Model->HoleStruct.Hight = _CM->Hight;
			//	_Model->HoleStruct.GroundHeight = _CM->GroundHeight;
			//}
			//if (!_CM)//Default
			//{
			//	if (Data->GetObjectType() == EObjectType::EWindow)
			//	{
			//		_Model->ModelStruct.ModelID = -1;//WindowID:Window_A0022
			//		_Model->ModelStruct.ResID = TEXT("DF61B94D45A60B006D43B68FC0E03552");
			//		_Model->ComModelType = EDRModelType::EDRWindow;
			//		_Model->HoleStruct.ModelType = EDRModelType::EDRWindowHole;
			//	}
			//	if (Data->GetObjectType() == EObjectType::EDoorHole)
			//	{
			//		_Model->ModelStruct.ModelID = 22589;
			//		_Model->ModelStruct.ResID = TEXT("AB004CCF43A38A3A406E0080267F1EC0");
			//		//_Model->ComModelType = EDRModelType::EDRDoorFrame;
			//		_Model->HoleStruct.ModelType = EDRModelType::EDRDoorHole;
			//	}
			//	_Model->ModelStruct.BindID = Data->GetID();
			//	//_Model->ModelStruct.isShadow = _CM->isShadow;
			//	//_Model->ModelStruct.Location = _CM->ModelLocation;
			//	//_Model->ModelStruct.Rotation = _CM->ModelRotation;
			//	//_Model->ModelStruct.Scale = _CM->ModelScale;
			//	_Model->HoleStruct.HoleID = AnchorID;
			//	//_Model->HoleStruct.Location = Data->GetVector;
			//	_Model->HoleStruct.Width = Data->GetFloat(TEXT("Width"));
			//	_Model->HoleStruct.Hight = Data->GetFloat(TEXT("Height"));
			//	_Model->HoleStruct.GroundHeight = Data->GetFloat(TEXT("ZPos"));
			//}
			return Cast<ADRActor>(_Model);
		}
	}
	return nullptr;
}

ADRActor * UDRModelFactory::CreateModel(UBuildingData *& Data)
{
	if (World)
	{
		int32 _ID = Data->GetID();
		int32 _AnchorID = Data->GetInt(TEXT("AnchorID"));
		if (_AnchorID != -1)
		{
			IObject * _Obj = Data->GetBuildingSystem()->GetObject(_AnchorID);
			if (_Obj)
			{
				if (_Obj->GetType() == EObjectType::EWindow)
				{
					return  CreateComponentModel(Data, _AnchorID);
				}
				if (_Obj->GetType() == EObjectType::EDoorHole)
				{
					return  CreateComponentModel(Data, _AnchorID);
				}
			}
		}
		ADRModelActor * _Model = World->SpawnActor<ADRModelActor>();
		_Model->BuildData = Data;
		if (isLoading)
		{
			_Model->ModelStruct.ModelType = EDRModelType(Data->GetInt(TEXT("Type")));
			_Model->ModelStruct.ModelID = Data->GetInt(TEXT("ModelID"));
			_Model->ModelStruct.BindID = -1;
			_Model->ModelStruct.Location = Data->GetVector(TEXT("Location"));
			_Model->ModelStruct.Rotation = FRotator(0.f,0.f,0.f);
			//_Model->ModelStruct.Scale = Data->GetVector(TEXT("Scale"));;
			_Model->ModelStruct.ResID = Data->GetString(TEXT("ResID"));
			_Model->ModelStruct.isShadow = false; 
		}
		else
		{
			FDRModelInstance * _TModel = ModelData.Find(Data->GetID());
			if (_TModel)
			{
				_Model->ModelStruct.ModelType = _TModel->ModelType;
				_Model->ModelStruct.ModelID = _TModel->ModelID;
				_Model->ModelStruct.BindID = _TModel->BindID;
				_Model->ModelStruct.Location = _TModel->Location;
				_Model->ModelStruct.Rotation = _TModel->Rotation;
				//_Model->ModelStruct.Scale = _TModel->Scale;
				_Model->ModelStruct.ResID = _TModel->ResID;
				_Model->ModelStruct.isShadow = _TModel->isShadow;
			}
			ModelActor.Add(Data->GetID(), Cast<ADRActor>(_Model));
		}
		return Cast<ADRActor>(_Model);
	}
	return nullptr;
}

void UDRModelFactory::SetWorld(UWorld * _World)
{
	World = _World;
}
