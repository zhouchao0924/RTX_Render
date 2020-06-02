// Fill out your copyright notice in the Description page of Project Settings.
// base data model for actor in house layout

#pragma once


#include "BaseNode.generated.h"

// for linkage and redo undo function
USTRUCT(Blueprintable)
struct FBaseNode
{
	GENERATED_BODY()
public:
	FString Name;
};

