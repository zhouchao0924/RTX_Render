// Copyright? 2017 ihomefnt All Rights Reserved.


#include "ModelAutomationView.h"
#include "DRGameMode.h"
#include "ModelFileComponent.h"
#include "ConvexHullPhysicBody.h"
#include "PhysicsEngine/BodySetup.h"
#include "EditorGameInstance.h"
#include "IrayRender/IrayScene.h"
#include "IrayRender/IrayInterface.h"
#include "EditorUtils.h"
#include "EditorContext.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "FileManager.h"
#include "ModelFile.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"


UModelAutomationView::UModelAutomationView(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
	, PreviewActor(NULL)
	, PreviewResource(NULL)
	, ModelFileComponent(NULL)
{

}


void UModelAutomationView::SetPreviewResource(const FString &resid)
{
	if (PendingResource)
	{
		PendingResource->StateChanged.RemoveDynamic(this, &UModelAutomationView::OnResourceStateChanged);
		PendingResource = NULL;
	}

	UResource *FoundResource = UResourceMgr::AsyncLoadRes(resid, this);
	if (FoundResource)
	{
		PendingResource = FoundResource;

		if (FoundResource->IsLoaded())
		{
			OnResourceStateChanged(FoundResource, EResState::EResLoaded, FoundResource->DRInfo);
		}
		else
		{
			FoundResource->StateChanged.AddDynamic(this, &UModelAutomationView::OnResourceStateChanged);
		}
	}
}

void UModelAutomationView::OnResourceStateChanged(UResource * Resource, EResState ResState, UVaRestJsonObject * DRInfo)
{
	check(Resource == PendingResource);

	if (ResState == EResState::EResLoaded)
	{
		if (PendingResource)
		{
			PendingResource->StateChanged.RemoveDynamic(this, &UModelAutomationView::OnResourceStateChanged);
			PendingResource = NULL;
		}
		OnPreviewResource(Resource);
	}
}

void UModelAutomationView::QuarantineMaterialByIndex(int32 PartIndex, int32 SlotIndex, bool IsVisible)
{
	UProceduralMeshComponent* part = Cast<UProceduralMeshComponent>(ModelFileComponent->GetPart(PartIndex));
	part->SetMeshSectionVisible(SlotIndex, IsVisible);
	
}

void UModelAutomationView::OnPreviewResource(UResource * Resource)
{
	PreviewResource = Resource;
	UWorld *MyWorld = GetWorld();
	UCEditorGameInstance *MyGame = MyWorld ? Cast<UCEditorGameInstance>(MyWorld->GetGameInstance()) : NULL;
	UModelFile *ModelFile = Cast<UModelFile>(Resource);
	if (!PreviewActor)
	{
		for (FActorIterator It(MyWorld); It; ++It)
		{
			AActor *SrcActor = *It;
			if (SrcActor->ActorHasTag(FName("ModelActor")))
			{
				PreviewActor = SrcActor;
			}
		}

		ModelFileComponent = NewObject<UModelFileComponent>(PreviewActor, TEXT("Model"));
	}

	if (ModelFile)
	{

		ModelFileComponent->StandardMaterials = MyGame->Context.MaterialCollection;
		ModelFileComponent->RegisterComponentWithWorld(MyWorld);
		ModelFileComponent->SetWorldTransform(PreviewActor->GetTransform());
		ModelFileComponent->AttachTo(PreviewActor->GetRootComponent());

		ModelFileComponent->UpdateModel(ModelFile);
		DrawComponent = Cast<UPDIDrawComponent>(PreviewActor->GetComponentByClass(UPDIDrawComponent::StaticClass()));
		if (DrawComponent)
		{
			DrawComponent->ModelFileComponent = ModelFileComponent;
			DrawComponent->RegisterComponentWithWorld(MyWorld);

		}
	}

	OnEditorResource(PreviewResource);
}
