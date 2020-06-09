

#include "AHousePluginActor.h"
#include "BuildingComponent.h"
#include "EditorGameInstance.h"
#include "BuildingGroupMeshComponent.h"
class UBuildingSystem;
AHousePluginActor::AHousePluginActor(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, bTopCeil(0)
	,HousePluginID(-1)
	,modelID(22314)
	,RoomClassID(0)
	, CraftID(0)
	,Scale(100,100,0)
	,Offset(0,0,0)
	,Angle(0.0f)
	,IsPillarPlugin(false)
	,NewWallMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/GameEditor/EditorPrograms/MapWallSystem/Material/DefaultWallMaterial.DefaultWallMaterial'")))
	,WallMaterial(LoadObject<UMaterialInstance>(nullptr, TEXT("MaterialInstanceConstant'/Game/GameEditor/EditorPrograms/MapWallSystem/Material/BearingWallMaterial_Inst.BearingWallMaterial_Inst'")))
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	HouseRootComponent = RootComponent;
	ModelFileComponent= CreateDefaultSubobject<UModelFileComponent>(TEXT("ModelFile"));
	ModelFileComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	GroupComponent = CreateDefaultSubobject<UBuildingGroupMeshComponent>(TEXT("Group"));
	GroupComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);	
}

void AHousePluginActor::Update(UBuildingData *Data)
{
	UBuildingComponent * Comp = FindComponentByClass<UBuildingComponent>();
	if (Comp)
	{
		Comp->SetData(Data);
	}
}

void AHousePluginActor::InitSurface()
{
	int32 MeshID = GroupComponent->GetID();
	if (ComponentManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("The Component object is exited.\n"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No componentactor.\n"));
	}
}

void AHousePluginActor::ChangePillarSurf()
{
	if (GroupComponent)
	{
		UMaterialInstanceDynamic *MaterialInst = GroupComponent->CreateDynamicMaterialInstance(0, WallMaterial);
		if (MaterialInst && IsPillarPlugin)
		{
			GroupComponent->SetMaterial(0, MaterialInst);
		}
		else
		{
			UMaterialInstanceDynamic* MatInst = GroupComponent->CreateDynamicMaterialInstance(0, NewWallMaterial);
			GroupComponent->SetMaterial(0, MatInst);
		}
	}
}

void AHousePluginActor::SetPluginTopMaterial(bool b)
{
	if (GroupComponent)
	{
		if (b)
		{
			UMaterialInstanceDynamic *MaterialInst = GroupComponent->CreateDynamicMaterialInstance(0, WallMaterial);
			if (MaterialInst && IsPillarPlugin)
			{
				GroupComponent->SetMaterial(0, MaterialInst);
			}
		}
		else
		{
			UMaterialInstanceDynamic* MatInst = GroupComponent->CreateDynamicMaterialInstance(0, NewWallMaterial);
			if (MatInst&&IsPillarPlugin)
			{
				GroupComponent->SetMaterial(0, MatInst);
			}
		}

	}
}