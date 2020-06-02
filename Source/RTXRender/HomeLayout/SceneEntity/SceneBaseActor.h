// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "SceneBaseActor.generated.h"

// base actor in room scene that may be furniture, door, window, light or something else.
// THomeSrc name:
// Map Base
UCLASS()
class RTXRENDER_API ASceneBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASceneBaseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
