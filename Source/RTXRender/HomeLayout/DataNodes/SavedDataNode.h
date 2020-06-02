#pragma once

#include "NodeTypes.h"
#include "PointNode.h"
#include "WallNode.h"
#include "Building/DRStruct.h"
#include "SavedDataNode.generated.h"

UENUM(BlueprintType)
enum class EWallType : uint8
{
	OriginalWall,
	NewWall,
};

// SavedConfig
USTRUCT(Blueprintable)
struct FSavedDataNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsDelete;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWallTypeCPP Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString TagName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FWallNode WallNode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FPointNode PointNode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString WallOrPointAreaTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bRoomWall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bisSaved;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWallType Wall_Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsLoadBearingWall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ENewWallType NewWallType;
public:
	FSavedDataNode()
		: bRoomWall(true)
		, bisSaved(false)
		, Wall_Type(EWallType::OriginalWall)
		, bIsLoadBearingWall(false)
		, NewWallType(ENewWallType::EPlasterWall)
	{}
};