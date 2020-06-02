// Fill out your copyright notice in the Description page of Project Settings.
// wall point data structure
#pragma once

#include "BaseNode.h"
#include "NodeTypes.h"
#include "PointNode.generated.h"

// point config
USTRUCT(Blueprintable)
struct FPointNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ActorTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ActorTag2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ELinkRelationshipCPP RelationShip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString NewWallTag;
};

