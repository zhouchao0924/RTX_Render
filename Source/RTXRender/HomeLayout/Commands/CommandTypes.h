// all data enum types should be here
#pragma once


#include "CommandTypes.generated.h"


enum class EDrawWallState : uint8
{
	NotStart = 1,
	StartDraw,
	StartDrag,
	EndDraw,
};

UENUM(BlueprintType)
enum class EDrawWallMode : uint8
{
	Unkown = 0,
	DrawWallByMiddle = 1,
	DrawWallByLeft,
	DrawWallByRight
};

USTRUCT(BlueprintType)
struct FDrawWallSetting
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EDrawWallMode DrawWallMode;
	UPROPERTY(BlueprintReadWrite)
	bool bSnap;
	UPROPERTY(BlueprintReadWrite)
	float SnapTol;
	UPROPERTY(BlueprintReadWrite)
	bool bShowConnectedPnts;
	UPROPERTY(BlueprintReadWrite)
	float WallThick;
	UPROPERTY(BlueprintReadWrite)
	float WallHeight;
	UPROPERTY(BlueprintReadWrite)
	FLinearColor WallColor;
};