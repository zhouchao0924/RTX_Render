// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ResourceActor.h"
#include "SolutionComponent.h"
#include "CompundResourceMeshComponent.h"

AResourceActor::AResourceActor(const FObjectInitializer &ObjectIntializer)
	:Super(ObjectIntializer)
{
	RootComponent = ObjectIntializer.CreateDefaultSubobject<USceneComponent>(this, "RootComponent");
}

void AResourceActor::SetResource(USolutionComponent *SolutionComponent, int32 ID)
{
	IBuildingResourceMgr *ResMgr = GetSDKResourceMgr();
	if (ResMgr)
	{
		IObject *pObj = ResMgr->GetResource(ID);
		if (pObj)
		{
			if (pObj->GetType() == ECompoundFile)
			{
				ResourceMeshComp = NewObject<UCompundResourceMeshComponent>(this, TEXT("CompoundResourceMeshComponent"));
			}
			else if (pObj->GetType() == EMXFile)
			{
				ResourceMeshComp = NewObject<UResourceMeshComponent>(this, TEXT("ResourceMeshComponent"));
			}
		}
	}

	if (ResourceMeshComp)
	{
		ResourceMeshComp->SolutionComponent = SolutionComponent;
		ResourceMeshComp->RegisterComponentWithWorld(GetWorld());
		ResourceMeshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		ResourceMeshComp->SetResource(ID);
	}
}

void AResourceActor::LoadFile(const FString &Filename)
{
	IBuildingSDK *SDK = GetBuildingSDK();
	if (SDK)
	{
		const char *AnsiFilename = TCHAR_TO_ANSI(*Filename);
		IObject *pObj = SDK->LoadFile(AnsiFilename);
		if (pObj)
		{
			ResourceMeshComp->SetResource(pObj->GetID());
		}
	}
}


