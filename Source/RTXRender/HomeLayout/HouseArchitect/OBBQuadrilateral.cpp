// Copyright? 2017 ihomefnt All Rights Reserved.


#include "OBBQuadrilateral.h"


OBBQuadrilateral::~OBBQuadrilateral()
{
}

OBBQuadrilateral::OBBQuadrilateral(TArray<FVector2D> points)
{
	if (points.Num() == 6)
	{
		points.RemoveAt(3);
		points.RemoveAt(0);
	}
	if (points.Num() != 4)
	{
		return;
	}
	centerPoint = FVector2D(0,0);
	for (size_t i = 0; i < points.Num(); i++)
	{
		centerPoint += points[i];
	}
	centerPoint /= points.Num();
	extents = FVector2D(abs((points[0] - points[1]).Size()) / 2, abs((points[1] - points[2]).Size()) / 2);
	axes.Init(FVector2D(0,0), 2);
	axes[0] = (points[0] - points[1]).GetSafeNormal();
	axes[1] = (points[1] - points[2]).GetSafeNormal();
}

OBBQuadrilateral::OBBQuadrilateral(FVector2D min, FVector2D max)
{
	centerPoint = (max + min) / 2;
	extents = FVector2D(abs(max.X - min.X)/2, abs(max.Y - min.Y)/2);
	axes.Init(FVector2D(0,0), 2);
	axes[0] = FVector2D(1, 0);
	axes[1] = FVector2D(0, 1);
}

float OBBQuadrilateral::getProjectionRadius(FVector2D axes)
{
	return extents.X * abs(FVector2D::DotProduct(this->axes[0], axes)) + extents.Y * abs(FVector2D::DotProduct(this->axes[1], axes));
}