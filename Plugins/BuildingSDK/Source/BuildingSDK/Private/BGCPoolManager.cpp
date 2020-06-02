// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "BGCPoolManager.h"
#include "BuildingSDKSystem.h"

UBGCPoolManager::UBGCPoolManager(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

UBGCPoolManager *UBGCPoolManager::Instance()
{
	ABuildingSDKSystem *SDK = ABuildingSDKSystem::GetSDKActor();
	if (SDK)
	{
		return SDK->BGPoolManager;
	}
	return nullptr;
}

UBuildingGroupMeshComponent *UBGCPoolManager::AllocComponent(AActor *Owner)
{
	UBuildingGroupMeshComponent *Component = NULL;
	if (Components.Num()>0)
	{
		Component = Components.Pop();
	}
	else
	{
		Component = NewObject<UBuildingGroupMeshComponent>();
	}

	if (Component)
	{
		Component->HoldActor = Owner;
		Owner->AddInstanceComponent(Component);
	}

	return Component;
}

void UBGCPoolManager::FreeComponent(UBuildingGroupMeshComponent *Component)
{
	if (Component)
	{
		UBGCPoolManager *Pool = Instance();
		if (Pool)
		{
			Pool->Free(Component);
		}
		else
		{
			Component->DestroyComponent();
		}
	}
}

void UBGCPoolManager::Free(UBuildingGroupMeshComponent *Component)
{
	if (Component)
	{
		AActor *InstanceActor = Component->HoldActor;
		if (InstanceActor)
		{
			Component->HoldActor = nullptr;
			InstanceActor->RemoveInstanceComponent(Component);
		}
		Component->UnregisterComponent();
		Components.Push(Component);
	}
}

void UBGCPoolManager::ReleaseAll()
{
	for (int32 i = 0; i < Components.Num(); ++i)
	{
		UBuildingGroupMeshComponent *Component = Components[i];
		if (Component)
		{
			Component->ConditionalBeginDestroy();
		}
	}
	Components.Empty();
}


