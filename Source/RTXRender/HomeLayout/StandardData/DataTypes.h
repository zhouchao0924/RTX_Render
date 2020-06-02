// all data enum types should be here
#pragma once

#include "DataTypes.generated.h"

UENUM(BlueprintType)
enum class EWallUsage : uint8
{
	None = 0,
	OrdinaryWall, // 普通墙
	MainWall,	// 承重墙
	LightWall,	// 轻质墙
	PartitionWall, // 隔断
};

