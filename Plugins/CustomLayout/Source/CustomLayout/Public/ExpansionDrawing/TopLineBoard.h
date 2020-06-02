// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "Building/StretchMesh/StretchPathComponent.h"

#include "TopLineBoard.generated.h"

UCLASS()
class CUSTOMLAYOUT_API ATopLineBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATopLineBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//UFUNCTION(BlueprintCallable)
	//	void SetTopLineBoardDate(const TArray<AActor*>& FurActor, const TArray <FExpansionLine>& WallList);

	
	//UStretchPathComponent *mModel;

	void CreateTopMesh(TArray<FVector>&PointList, FString& Mx);
};
