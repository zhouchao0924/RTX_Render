// Copyright? 2017 ihomefnt All Rights Reserved.


#include "MapBase_VirtualObject.h"


// Sets default values
AMapBase_VirtualObject::AMapBase_VirtualObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMapBase_VirtualObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMapBase_VirtualObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

