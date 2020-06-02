// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class RTXRENDER_API OBBQuadrilateral
{
public:

	~OBBQuadrilateral();
	OBBQuadrilateral(TArray<FVector2D> points);
	OBBQuadrilateral(FVector2D min, FVector2D max);
	
	FVector2D centerPoint;
	FVector2D extents;
	TArray<FVector2D> axes;
	float getProjectionRadius(FVector2D axes);
};
