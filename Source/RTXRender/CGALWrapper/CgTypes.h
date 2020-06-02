#pragma once

#pragma warning(disable : 4668)
#include "CoreMinimal.h"

typedef TArray<FVector2D> FPoint2DArray;

UENUM(BlueprintType)
enum class ECgPolyBoundedSide : uint8
{
	EON_UNBOUNDED = 0,
	EON_BOUNDARY,
	EON_BOUNDED
};

UENUM(BlueprintType)
enum class ECgPolyOrientation : uint8
{
	Unkown = 0,
	EUNDEFINED = 1,
	ECLOCKWISE,
	ECOUNTERCLOCKWISE,
};

UENUM(BlueprintType)
enum class ECgHeadingDirection : uint8
{
	Unkown = 0,
	EUndefined = 1,
	ELeftTurn = 2,
	ERightTurn,
};