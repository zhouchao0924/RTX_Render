

#include "ModelFileActor.h"
#include "BuildingData.h"
#include "ResourceMgr.h"

AModelFileActor::AModelFileActor(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	ModelComponentFile = ObjectInitializer.CreateDefaultSubobject<UModelFileComponent>(this, TEXT("ModelFileComponent"));
}

void AModelFileActor::Update(UBuildingData *Data)
{
	UpdateCompoennt(ModelComponentFile, Data);
}

void AModelFileActor::UpdateCompoennt(UModelFileComponent *InModelFileComponent, UBuildingData *Data)
{
	if (InModelFileComponent && Data)
	{
		FString ResID = Data->GetString(TEXT("ResID"));
		int32   ModelID = Data->GetInt(TEXT("ModelID"));

		UResourceMgr *ResMgr = UResourceMgr::GetResourceMgr();
		if (ResMgr)
		{
			UModelFile *ModelFile = Cast<UModelFile>(ResMgr->FindRes(ResID));
			if (ModelFile)
			{
				FVector Location = Data->GetVector(TEXT("Location"));
				FVector Forward = Data->GetVector(TEXT("Forward"));
				int32  AlignType = Data->GetInt(TEXT("AlignType"));

				AActor *pActor = Cast<AActor>(InModelFileComponent->GetOwner());
				if (pActor)
				{
					pActor->SetActorLocationAndRotation(Location, Forward.ToOrientationRotator(), false);
				}
				else
				{
					InModelFileComponent->SetWorldLocationAndRotation(Location, Forward.ToOrientationRotator());
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
					InModelFileComponent->AddClipPlane(FPlane(ClipBase1, ClipNormal1));
				}

				ModelFile->ForceLoad();
				InModelFileComponent->SetModelAlignType(ECenterAdjustType(AlignType));
				InModelFileComponent->UpdateModel(ModelFile);
			}
		}
	}
}
