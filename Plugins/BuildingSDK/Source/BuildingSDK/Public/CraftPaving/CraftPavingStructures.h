// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CraftPavingStructures.generated.h"

UENUM(BlueprintType)
enum class EShapeTypes : uint8
{
	ST_Triangle = 0,
	ST_Diamond = 1,
	ST_Rectangle = 2,
	ST_Parallelogram = 3,
	ST_Hexagon = 4,
	ST_UnKown
};

USTRUCT(BlueprintType)
struct FShapeOptions
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = ShapeOptions)
		EShapeTypes Type;
	UPROPERTY(BlueprintReadWrite, Category = ShapeOptions)
		float Length;
	UPROPERTY(BlueprintReadWrite, Category = ShapeOptions)
		float Width;
	UPROPERTY(BlueprintReadWrite, Category = ShapeOptions)
		float Radius;
	UPROPERTY(BlueprintReadWrite, Category = ShapeOptions)
		float Angle;
	UPROPERTY(BlueprintReadWrite, Category = ShapeOptions)
		float CornerRadius;
	UPROPERTY(BlueprintReadWrite, Category = ShapeOptions)
		float Segments;
	UPROPERTY(BlueprintReadWrite, Category = ShapeOptions)
		FTransform Transform;
};

UENUM(BlueprintType)
enum class EAttributeTypes : uint8
{
	AT_Length = 0,
	AT_Width = 1,
	AT_Radius = 2,
	AT_Angle = 3,
	AT_CornerRadius = 4,
	AT_Segments = 5
};