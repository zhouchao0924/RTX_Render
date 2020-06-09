// Copyright© 2017 ihomefnt All Rights Reserved.

#include "GroupActorBase.h"


// Sets default values
AGroupActorBase::AGroupActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsLocked = true;

	GroupActors.Empty();

	GroupParentReference.Empty();
}

// Called when the game starts or when spawned
void AGroupActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGroupActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGroupActorBase::Lock()
{
	bIsLocked = true;
}

void AGroupActorBase::Unlock()
{
	bIsLocked = false;
}

bool AGroupActorBase::IsLocked()
{
	return bIsLocked;
}

bool AGroupActorBase::ShouldHandleSelect(AActor* InActor)
{
	bool ActorInGroup = CheckIsInGroup(InActor);

	return ActorInGroup && bIsLocked;
}

bool AGroupActorBase::CheckIsInGroup(AActor* InActor)
{
	return GroupActors.Find(InActor) != INDEX_NONE;
}

bool AGroupActorBase::CheckIsRoot()
{
	return GroupParentReference.Num() == 0;
}

int32 AGroupActorBase::CalculateActorCount()
{
	int32 ResultCount = 0;

	for (auto & Item : GroupActors)
	{
		AGroupActorBase* GroupItem = Cast<AGroupActorBase>(Item);

		if (GroupItem != nullptr)
		{
			ResultCount += GroupItem->CalculateActorCount();
		}
		else
		{
			ResultCount += 1;
		}
	}

	return ResultCount;
}