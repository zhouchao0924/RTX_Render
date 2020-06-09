// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Struct.h"
#include "MapSystemBase.generated.h"

UCLASS()
class AJDR_API AMapSystemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapSystemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Iray")
	TArray<FWindowAreaData> GetWindowInfo();
	
};
