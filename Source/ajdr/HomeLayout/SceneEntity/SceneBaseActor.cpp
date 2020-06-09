// Copyright© 2017 ihomefnt All Rights Reserved.

#include "SceneBaseActor.h"


// Sets default values
ASceneBaseActor::ASceneBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASceneBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASceneBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

