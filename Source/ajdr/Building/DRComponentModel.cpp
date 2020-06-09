

#include "DRComponentModel.h"
#include "Building/BuildingSystem.h"
#include "Building/DRModelFactory.h"
#include "ModelFileActor.h"
#include "ResourceMgr.h"
#include "DRFunLibrary.h"
#include "DRGameMode.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "EditorGameInstance.h"


ADRComponentModel::ADRComponentModel()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));
	ModelCom = CreateDefaultSubobject<UModelFileComponent>(TEXT("ModelCom"));
	ModelCom->SetupAttachment(RootComponent);
	isLoadingData = false;
}

void ADRComponentModel::UpdateBuildingData()
{
	if (BuildData)
	{
		UResourceMgr * ResMgr = UResourceMgr::GetResourceMgr();
		if (ResMgr)
		{
			UDRModelFactory * _MF = Cast<UDRModelFactory>(BuildData->GetBuildingSystem()->BuilldActorFactory);
			if (_MF)
			{
	/*			if(!isLoadingData || BuildData->GetObjectType() == EObjectType::EAnchor)
				{
					int32 _ID = BuildData->GetID();
					int32 _Type = BuildData->GetObjectType();
					int32 _AnchorID = BuildData->GetInt(TEXT("AnchorID"));
					FTempComModelData * _CMD = _MF->ComModelData.Find(_AnchorID);
					if (_CMD)
					{
						this->ComModelType = _CMD->ModelType;

						this->ModelStruct.ModelType = _CMD->ModelType;
						this->ModelStruct.ModelID = _CMD->ModelID;
						this->ModelStruct.BindID = _CMD->BindID;
						this->ModelStruct.isShadow = _CMD->isShadow;
						this->ModelStruct.Location = _CMD->ModelLocation;
						this->ModelStruct.Rotation = _CMD->ModelRotation;
						this->ModelStruct.Scale = _CMD->ModelScale;
						this->ModelStruct.ResID = _CMD->ResID;

						this->HoleStruct.HoleID = _AnchorID;
						this->HoleStruct.ModelType = _CMD->ModelType;
						this->HoleStruct.Location = _CMD->Location;
						this->HoleStruct.Width = _CMD->Width;
						this->HoleStruct.Hight = _CMD->Hight;
						this->HoleStruct.GroundHeight = _CMD->GroundHeight;
					}
				}*/
				UModelFile * _ModelFile = nullptr;
				_ModelFile = Cast<UModelFile>(ResMgr->FindRes(ModelStruct.ResID));
				if (_ModelFile)
				{
					int32  AlignType = BuildData->GetInt(TEXT("AlignType"));
					_ModelFile->ForceLoad();
					ModelCom->SetModelAlignType(ECenterAdjustType(AlignType));
					ModelCom->UpdateModel(_ModelFile);
					FProcMeshSection * _Mesh = ModelCom->Parts[0]->GetProcMeshSection(0);
					if (_Mesh)
					{
						TArray<FVector> MeshVer;
						for (int i = 0; i < _Mesh->ProcVertexBuffer.Num(); ++i)
						{
							MeshVer.Add(_Mesh->ProcVertexBuffer[i].Position);
						}
						FVector _Max = UDRFunLibrary::MaxVertex(MeshVer);
						FVector _Min = UDRFunLibrary::MinVertex(MeshVer);
						FVector _M = _Max - _Min;
						ModelCom->SetRelativeScale3D(FVector(HoleStruct.Width / _M.X, 1.f, HoleStruct.Hight / _M.Z));
					}		
				}
			}
		}
		int32 _AnchorID = BuildData->GetInt(TEXT("AnchorID"));
		if (_AnchorID)
		{
			IObject * _Obj = BuildData->GetBuildingSystem()->GetObject(_AnchorID);
			if (_Obj)
			{
				IValue &Location = BuildData->GetSuite()->GetProperty(_AnchorID, "Location");
				HoleStruct.Location = ToVector2D(Location.Vec2Value());
				IValue &Width = BuildData->GetSuite()->GetProperty(_AnchorID, "Width");
				HoleStruct.Width = Width.FloatValue();
				IValue &Height = BuildData->GetSuite()->GetProperty(_AnchorID, "Height");
				HoleStruct.Hight = Height.FloatValue();
				IValue &GroundHeight = BuildData->GetSuite()->GetProperty(_AnchorID, "GroundHeight");
				HoleStruct.GroundHeight = GroundHeight.FloatValue();

				ModelStruct.Location =  BuildData->GetVector(TEXT("Location"));
				ModelStruct.Size =  BuildData->GetVector(TEXT("Size"));
				ModelStruct.Rotation = BuildData->GetVector(TEXT("Forward")).Rotation();

				SetActorLocationAndRotation(ModelStruct.Location, ModelStruct.Rotation);
			}
		}
	}
}

void ADRComponentModel::SaveSelf()
{
}

void ADRComponentModel::LoadSelf()
{
}


void ADRComponentModel::UpdateHoleLocationInfo()
{
	int32 _AnchorID = BuildData->GetInt(TEXT("AnchorID"));
	if (_AnchorID)
	{
		IObject * _Obj = BuildData->GetBuildingSystem()->GetObject(_AnchorID);
		if (_Obj)
		{
			IValue &Location = BuildData->GetSuite()->GetProperty(_AnchorID, "Location");
			HoleStruct.Location = ToVector2D(Location.Vec2Value());
			IValue &Width = BuildData->GetSuite()->GetProperty(_AnchorID, "Width");
			HoleStruct.Width = Width.FloatValue();
			IValue &Height = BuildData->GetSuite()->GetProperty(_AnchorID, "Height");
			HoleStruct.Hight = Height.FloatValue();
			IValue &GroundHeight = BuildData->GetSuite()->GetProperty(_AnchorID, "GroundHeight");
			HoleStruct.GroundHeight = GroundHeight.FloatValue();

			ModelStruct.Location = BuildData->GetVector(TEXT("Location"));
			ModelStruct.Size = BuildData->GetVector(TEXT("Size"));
			ModelStruct.Rotation = BuildData->GetVector(TEXT("Forward")).Rotation();

			SetActorLocationAndRotation(ModelStruct.Location, ModelStruct.Rotation);
		}
	}
}

void ADRComponentModel::UpdateModelRes(const FDRModelInstance& Model)
{
	ModelStruct = Model;
	UModelFile * _ModelFile = nullptr;
	UResourceMgr * ResMgr = UResourceMgr::GetResourceMgr();
	if (ResMgr)
	{
		_ModelFile = Cast<UModelFile>(ResMgr->FindRes(ModelStruct.ResID));
		if (_ModelFile)
		{
			int32  AlignType = BuildData->GetInt(TEXT("AlignType"));
			_ModelFile->ForceLoad();
			ModelCom->SetModelAlignType(ECenterAdjustType(AlignType));
			ModelCom->UpdateModel(_ModelFile);
			FProcMeshSection * _Mesh = ModelCom->Parts[0]->GetProcMeshSection(0);
			if (_Mesh)
			{
				TArray<FVector> MeshVer;
				for (int i = 0; i < _Mesh->ProcVertexBuffer.Num(); ++i)
				{
					MeshVer.Add(_Mesh->ProcVertexBuffer[i].Position);
				}
				FVector _Max = UDRFunLibrary::MaxVertex(MeshVer);
				FVector _Min = UDRFunLibrary::MinVertex(MeshVer);
				FVector _M = _Max - _Min;
				ModelCom->SetRelativeScale3D(FVector(HoleStruct.Width / _M.X, 1.f, HoleStruct.Hight / _M.Z));
			}
		}
	}
}

void ADRComponentModel::MoveComponent(bool IsSelected)
{
	if (BooleanBaseHUDWidget)
	{
		bIsMove = IsSelected;
		BooleanBaseHUDWidget->bIsMove = bIsMove;
	}
}

void ADRComponentModel::Selected(bool IsSelected)
{
	if (BooleanBaseHUDWidget)
	{
		bIsSelect = IsSelected;
		BooleanBaseHUDWidget->bIsSelect;
	}
}

void ADRComponentModel::Init()
{
	UWorld *MyWorld = GetWorld();
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	if (GameInst)
	{
		UWallBuildSystem *WallSystem = GameInst->WallBuildSystem;
		if (WallSystem)
		{
			UClass *WidgetClass = WallSystem->BooleanUMG.Get();
			if (WidgetClass && WidgetClass->IsChildOf(UBooleanBase::StaticClass()))
			{
				BooleanBaseHUDWidget = Cast<UBooleanBase>(CreateWidget<UUserWidget>(GetWorld(), WallSystem->BooleanUMG));
				BooleanBaseHUDWidget->AddToViewport(-1);
				BooleanBaseHUDWidget->HoleID = HoleStruct.HoleID;
				if (WallSystem->BuildingSystem)
				{
					UBuildingData *HoleData = WallSystem->BuildingSystem->GetData(HoleStruct.HoleID);
					if (HoleData)
					{
						BooleanBaseHUDWidget->WallID = HoleData->GetInt("WallID");
					}
				}
			}
		}
	}
}

void ADRComponentModel::ReadyMove(bool IsReadyMove)
{
	if (BooleanBaseHUDWidget)
	{
		bIsReadyMove = IsReadyMove;
	}
}

void ADRComponentModel::UpDataWallID(int32 ID)
{
	if (BooleanBaseHUDWidget)
	{
		BooleanBaseHUDWidget->NewWallID = ID;
	}
}

void ADRComponentModel::GetMoveCurWallID(int32 &ID)
{
	ID = -1;
	if (BooleanBaseHUDWidget)
	{
		if (BooleanBaseHUDWidget->NewWallID != INDEX_NONE)
		{
			ID = BooleanBaseHUDWidget->NewWallID;
		}
		else
		{
			ID = BooleanBaseHUDWidget->WallID;
		}
	}
}

void ADRComponentModel::GetCurrentHoleWidth(float &Width)
{
	Width = 0;
	if (BooleanBaseHUDWidget)
	{
		Width = BooleanBaseHUDWidget->Width;
	}
}

int32 ADRComponentModel::GetHoleID()
{
	return HoleStruct.HoleID;
}

void ADRComponentModel::SetNewWallID(int32 ID)
{
	if (BooleanBaseHUDWidget)
	{
		if (ID != INDEX_NONE)
		{
			BooleanBaseHUDWidget->WallID = ID;
		}
	}
}