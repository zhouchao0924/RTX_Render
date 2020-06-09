// Fill out your copyright notice in the Description page of Project Settings.
// room data structure for room actor
#pragma once

#include "BaseNode.h"
#include "RoomNode.generated.h"

//SkuStruct
USTRUCT(Blueprintable)
struct FSkuStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SkuId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SkuName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Portrait;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Category;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasSku;
};

// room struct
USTRUCT(Blueprintable)
struct FRoomNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString RoomName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSkuStruct> SkuList;
};

//// RoomUsage
//USTRUCT(Blueprintable)
//struct FRoomUsage
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Id;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//};