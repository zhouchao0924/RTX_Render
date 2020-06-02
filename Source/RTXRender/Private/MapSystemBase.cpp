// Copyright© 2017 ihomefnt All Rights Reserved.

#include "MapSystemBase.h"


// Sets default values
AMapSystemBase::AMapSystemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMapSystemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMapSystemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

