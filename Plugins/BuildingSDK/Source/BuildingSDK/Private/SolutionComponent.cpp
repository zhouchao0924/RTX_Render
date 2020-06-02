// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SolutionComponent.h"
#include "ResourceActor.h"

USolutionComponent::USolutionComponent(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, Suite(nullptr)
{
}

void USolutionComponent::OnUnregister()
{
	DestroySuite();
	Super::OnUnregister();
}

void USolutionComponent::CreateNewSuite()
{
	IBuildingSDK *SDK = GetBuildingSDK();
	if (SDK)
	{
		if (Suite)
		{
			SDK->DestroySuite(Suite);
			Suite = nullptr;
		}
		
		Suite = SDK->CreateSuite("");
	}
}

void USolutionComponent::Delete(int32 InstanceID)
{
	if (Suite)
	{
		Suite->DeleteObject(InstanceID);
	}
}

void USolutionComponent::DestroySuite()
{
	IBuildingSDK *SDK = GetBuildingSDK();
	if (SDK && Suite)
	{
		SDK->DestroySuite(Suite);
		Suite = nullptr;
	}
}

int32 USolutionComponent::AddModelByGUID(const FString &ResID, FTransform Transform, int32 BaseID /*= INVALID_OBJID*/)
{
	if (Suite)
	{
		std::string AnsiResID = TCHAR_TO_ANSI(*ResID);
		FVector Location = Transform.GetLocation();
		FVector Scale = Transform.GetScale3D();
		FRotator Rotation = Transform.Rotator();
		return Suite->AddModel(BaseID, AnsiResID.c_str(), FORCE_TYPE(kVector3D, Location), FORCE_TYPE(kRotation, Rotation), FORCE_TYPE(kVector3D, Scale));
	}
	return INVALID_OBJID;
}

int32 USolutionComponent::AddModelByObjectID(int32 ResourceObjID, FTransform Transform, int32 BaseID /*= INVALID_OBJID*/)
{
	IBuildingResourceMgr *ResMgr = GetSDKResourceMgr();
	if (Suite && ResMgr)
	{
		IObject *pObj = ResMgr->GetResource(ResourceObjID);
		IResource *pResource = pObj ? pObj->GetResource() : nullptr;
		if (pResource)
		{
			FVector Location = Transform.GetLocation();
			FVector Scale = Transform.GetScale3D();
			FRotator Rotation = Transform.Rotator();
			return Suite->AddModel(BaseID, pResource->GetResID(), FORCE_TYPE(kVector3D, Location), FORCE_TYPE(kRotation, Rotation), FORCE_TYPE(kVector3D, Scale));
		}
	}
	return INVALID_OBJID;
}

AResourceActor *USolutionComponent::SpawnResourceActor(int32 ResourceID, FTransform Transform)
{
	AResourceActor *pActor = nullptr;

	if (ResourceID != INVALID_OBJID)
	{
		pActor = GetWorld()->SpawnActor<AResourceActor>(AResourceActor::StaticClass(), Transform);
		if (pActor)
		{
			pActor->SetResource(this, ResourceID);
		}
	}

	return pActor;
}

IObject *USolutionComponent::GetSDKObject(int32 ObjID)
{
	if (Suite)
	{
		return Suite->GetObject(ObjID);
	}
	return nullptr;
}

