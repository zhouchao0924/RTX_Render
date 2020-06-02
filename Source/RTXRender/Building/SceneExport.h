// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneExport.generated.h"

UCLASS()
class ASceneExport : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASceneExport();

	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	bool Export(int SolutionID, const FString& strPath = "");

	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	bool Upload();

	void Open(int SolutionID);
	bool UploadFiles(const FString& InUrl, const TArray<FString>& InFiles);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
