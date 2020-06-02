// Copyright© 2017 ihomefnt All Rights Reserved.


#include "FurnitureModelActor.h"
#include "Building/BooleanBase.h"
#include "../../Building/DRFunLibrary.h"
#include "../HouseArchitect/ModelSystem.h"
#include "EditorGameInstance.h"
#include "ResourceMgr.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRDoorHoleAdapter.h"
#include "Data/Adapter/DRWindowHoleAdapter.h"


AFurnitureModelActor::AFurnitureModelActor()
	:isInit(false)
	, BuildingData(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFurnitureModelActor::UpdateHoleData()
{
	if (BuildingData
		&& ( BuildingData->GetObjectType() == EObjectType::EDoorHole 
		  || BuildingData->GetObjectType() == EObjectType::EWindow))
	{
		BuildingData->Update();
	}
}


void AFurnitureModelActor::UpdateModelData(UModelFileComponent* MeshCom)
{
	if (MeshCom && MeshCom->BuildData
		&&  MeshCom->BuildData->GetObjectType() == EObjectType::EModelInstance)
	{
		MeshCom->BuildData->Update();
	}
}

void AFurnitureModelActor::UpdateBuildResModel(int32 ModelID, FString ResID, UModelFileComponent* MeshCom)
{
	if (MeshCom && MeshCom->BuildData
		&&  MeshCom->BuildData->GetObjectType() == EObjectType::EModelInstance)
	{
		MeshCom->BuildData->SetInt(TEXT("ModelID"),ModelID);
		MeshCom->BuildData->SetString(TEXT("ResID"),ResID);
		MeshCom->BuildData->Update();
	}
}

void AFurnitureModelActor::UpdateBuildTranfrom(FTransform Location, UModelFileComponent* MeshCom)
{
	if (MeshCom && MeshCom->BuildData
		&&  MeshCom->BuildData->GetObjectType() == EObjectType::EModelInstance)
	{
		FBox _Box = MeshCom->Bounds.GetBox();
		FVector Size = _Box.Max = _Box.Min;
		MeshCom->BuildData->SetVector(TEXT("Location"), Location.GetLocation());
		MeshCom->BuildData->SetVector(TEXT("Forward"),Location.GetRotation().Vector());
		MeshCom->BuildData->SetVector(TEXT("Size"), FVector(Size*Location.GetScale3D()));
		MeshCom->BuildData->Update();
	}
}

void AFurnitureModelActor::UpdateBuildingData(UBuildingData * Data)
{
	if (Data)
	{
		if (Data->GetObjectType() == EObjectType::EModelInstance)
		{
			BPUpdateBuildingData(Data);
		}
	}
}


FVector AFurnitureModelActor::SetModelFileComponetScale(UModelFileComponent* MeshCom, FVector Scale)
{
	FVector OutScale = FVector::OneVector;
	float Loc_Width, Loc_Thickness, Loc_Height;
	if (MeshCom)
	{
		UDRProjData *ProjectManager = UDRProjData::GetProjectDataManager(this);
		if (ProjectManager != nullptr)
		{
			if (Cast<UDRDoorHoleAdapter>(ProjectManager->GetAdapter(ObjectId)) != nullptr)
			{
				Loc_Width = Cast<UDRDoorHoleAdapter>(ProjectManager->GetAdapter(ObjectId))->GetWidth();
				Loc_Thickness = Cast<UDRDoorHoleAdapter>(ProjectManager->GetAdapter(ObjectId))->GetThickness();
				Loc_Height = Cast<UDRDoorHoleAdapter>(ProjectManager->GetAdapter(ObjectId))->GetHeight();
			}
			else if (Cast<UDRWindowHoleAdapter>(ProjectManager->GetAdapter(ObjectId)) != nullptr)
			{
				Loc_Width = Cast<UDRWindowHoleAdapter>(ProjectManager->GetAdapter(ObjectId))->GetWidth();
				Loc_Thickness = Cast<UDRWindowHoleAdapter>(ProjectManager->GetAdapter(ObjectId))->GetThickness();
				Loc_Height = Cast<UDRWindowHoleAdapter>(ProjectManager->GetAdapter(ObjectId))->GetHeight();
			}

			if (MeshCom->GetModel())
			{
				//FProcMeshSection * screen = MeshCom->GetPart(0)->GetProcMeshSection(0);
				FVector _Size = MeshCom->GetModel()->GetLocalBounds().Max - MeshCom->GetModel()->GetLocalBounds().Min;

				if (Scale.X)
					OutScale.X = Loc_Width / _Size.X;
				else
					OutScale.X = 1.f;

				if (Scale.Y)
					OutScale.Y = Loc_Thickness / _Size.Y;
				else
					OutScale.Y = 1.f;

				if (Scale.Z)
					OutScale.Z = Loc_Height / _Size.Z;
				else
					OutScale.Z = 1.f;
			}
		}
	}
	return OutScale;
}

void AFurnitureModelActor::BeginPlay()
{
	Super::BeginPlay();

	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	if (GameInst)
	{
		if (!GameInst->ModelSystem)
		{
			GameInst->ModelSystem = GetWorld()->SpawnActor<AModelSystem>();
		}
		GameInst->ModelSystem->OnModelActorSpawned(this);
	}
}

void AFurnitureModelActor::Destroyed()
{
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	if (GameInst)
	{
		GameInst->ModelSystem->OnModelActorDestroyed(this);
	}
	Super::Destroyed();
}

TArray<FMatOfMX> AFurnitureModelActor::GetDependMats_Implementation()
{
	TArray<FMatOfMX> Mats;
	return Mats;
}

TArray<FMatOfMX> AFurnitureModelActor::GetMeshMats_Implementation(UModelFileComponent* MeshCom)
{
	TArray<FMatOfMX> Mats;
	return Mats;
}

FString AFurnitureModelActor::NewUUID()
{
	int32 Seconds;
	float PartialSeconds;
	UGameplayStatics::GetAccurateRealTime(this, Seconds, PartialSeconds);
	FString uuid = FString::Printf(TEXT("FurnitureModelActor_%f"), Seconds + PartialSeconds);
	return uuid;
}