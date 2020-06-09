// Fill out your copyright notice in the Description page of Project Settings.
// data model node for wall
#pragma once

#include "DataTypes.h"
#include "MaterialData.h"
#include "WallData.generated.h"


USTRUCT(Blueprintable)
struct FWallData : public FDataHandle
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int32	StartCornerIndex;
	UPROPERTY(BlueprintReadOnly)
	int32	EndCornerIndex;
	UPROPERTY(BlueprintReadOnly)
	float LeftThick;
	UPROPERTY(BlueprintReadOnly)
	float RightThick;
	UPROPERTY(BlueprintReadOnly)
	float Height;
	UPROPERTY(BlueprintReadOnly)
	bool bShowLeftRuler;
	UPROPERTY(BlueprintReadOnly)
	bool bShowRightRuler;
	UPROPERTY(BlueprintReadOnly)
	EWallUsage WallType;
	UPROPERTY(BlueprintReadOnly)
	int32 LeftRoomIndex;
	UPROPERTY(BlueprintReadOnly)
	int32 RightRoomIndex;

	FString StartCornerID;
	FString EndCornerID;
	FString LeftRoomID;
	FString RightRoomID;

	FWallData();

	void ComputeWallInfo();

	void ComputeWallLocalPos();

	FVector2D ConvertScene2LocalPnt(const FVector2D& ScenePnt) const;

	FVector2D ConvertLocal2ScenePnt(const FVector2D& LocalPnt) const;
};
