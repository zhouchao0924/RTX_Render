// Copyright© 2017 ihomefnt All Rights Reserved.


#include "ModelSystem.h"
#include "EditorGameInstance.h"


// Sets default values for this component's properties
AModelSystem::AModelSystem()
{
	
}

void AModelSystem::BeginPlay()
{
	Super::BeginPlay();
}

void AModelSystem::OnModelActorSpawned(AFurnitureModelActor* Actor)
{
	ModelsInWorld.Add(Actor);
}

void AModelSystem::OnModelActorDestroyed(AFurnitureModelActor* Actor)
{
	ModelsInWorld.Remove(Actor);
}

const TArray<AFurnitureModelActor*>& AModelSystem::GetModelsInWorld() const
{
	return ModelsInWorld;
}
