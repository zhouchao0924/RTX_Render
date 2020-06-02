

#include "DRModelActor.h"
#include "Building/BuildingSystem.h"
#include "Building/DRModelFactory.h"

ADRModelActor::ADRModelActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponet"));
	ModelCom = CreateDefaultSubobject<UModelFileComponent>(TEXT("ModelCom"));
	ModelCom->SetupAttachment(RootComponent);

}

void ADRModelActor::UpdateBuildingData()
{
	if (BuildData)
	{
		UDRModelFactory * _MF = Cast<UDRModelFactory>(BuildData->GetBuildingSystem()->BuilldActorFactory);
		if (_MF)
		{
			//FDRModelInstance * _CMI = _MF->ComModelData.Find(BuildData->GetID());
			FDRModelInstance * _MI = _MF->ModelData.Find(BuildData->GetID());
			if (_MI)
			{
				if (_MI->ModelType == EDRModelType::EDRModel)
				{
					ModelStruct = *_MI;
					this->SetActorRotation(_MI->Rotation);
					//this->SetActorScale3D(_MI->Size);
				}
			}
		}
	}
}

void ADRModelActor::SaveSelf()
{
}

void ADRModelActor::LoadSelf()
{
}
