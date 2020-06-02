// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomDataStructure.h "
#include "HouseFunction/CustomDataStructure.h"
#include "FunctionManage.generated.h"



UCLASS()
class RTXRENDER_API AFunctionManage : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFunctionManage();




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
