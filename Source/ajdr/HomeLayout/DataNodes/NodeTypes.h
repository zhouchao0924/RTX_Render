// all data enum types should be here
#pragma once

#define _USE_MATH_DEFINES
#include "Math/UnrealMathUtility.h"

#include "NodeTypes.generated.h"

UENUM(BlueprintType)
enum class ELinkRelationshipCPP : uint8
{
	None,
	AA,
	BB,
	BAAB,
};


UENUM(BlueprintType)
enum class EWallTypeCPP : uint8 
{
	Wall,
	Point,
	Bool,
};


UENUM(BlueprintType)
enum class EProductType : uint8
{
	All, // 全部
	LivingRoom, // 客厅
	DiningRoom, // 餐厅
	Bedroom, // 卧室
	KitchenRoom, // 厨卫
	StudyRoom, // 书房
	Material, // 材质
	Other, // 其他
};

UENUM(BlueprintType)
enum class EWallSurfaceType : uint8
{
	Front = 0,
	Back,
	StartSide,
	EndSide,
	Top,
	Bottom
};

UENUM(BlueprintType)
enum class EOpeningType : uint8
{
	Unkown = 0,
	DoorWay = 1,
	SingleDoor,
	DoubleDoor,
	SlidingDoor,
	Window = 10,
	BayWindow,
	FrenchWindow,
};


const float PLANE_INCREASING_HEIGHT = 0.1f;
const FString DEF_TOP_WALL_MATERIAL(TEXT("Material'/Game/GameEditor/EditorPrograms/MapWallSystem/Material/Color_Inst.Color_Inst'"));
const FString DEFAULT_WALL_MATERIAL(TEXT("Material'/Game/GameEditor/EditorPrograms/MapWallSystem/Material/DefaultWallMaterial.DefaultWallMaterial'"));
const FString DEFAULT_AREA_MATERIAL(TEXT("Material'/Game/GameEditor/EditorPrograms/MapWallSystem/Material/DefaultAreaMat_Inst.DefaultAreaMat_Inst'"));
const FVector2D DEF_WALL_MAT_SIZE(100, 100);
const FVector DEF_WALL_CUBE_SIZE(100, 20, 280);
const float SEG_PARALLEL_TOLERANCE = 0.001f;
const float SAME_CORNER_TOLERANCE = 0.01f;
const float WALL_RULER_OFFSET = 10.0f;
const FLinearColor WALL_DEFAULT_COLOR(0.155, 0.155, 0.155);
const float LEGAL_WALL_LENGTH = 10.0f;
const float WALL_AREA_SNAP_TOL = 5.0f;
const float DEF_CEILING_HEIGHT = 280.f;
const float SmallPositiveNumber = 0.1f;
const float CornerSnap = 1.0f;

const float HALF_CIRCLE_ANGLE = 90;
const float HALF_CIRCLE_DEG = PI*0.5f;

const FLinearColor DRAWLINE_SNAPCOLOR(0.042f, 1.0f, 1.0f);