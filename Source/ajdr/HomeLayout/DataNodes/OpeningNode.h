// Fill out your copyright notice in the Description page of Project Settings.
// opening data structure for opening model

#pragma once

#include "../SceneEntity/OpeningActor.h"
#include "MaterialNode.h"
#include "OpeningNode.generated.h"

// wall boolean saved config
USTRUCT(Blueprintable)
struct FOpeningNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString WallActorTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LeftWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RightWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GroundHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHFlip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bVFlip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsDelect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AOpeningActor> Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SelfTagname;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SlotActorTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FMaterialNode Mat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Position;
};

