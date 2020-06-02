
#include "WallData.h"

FWallData::FWallData()
{
	Height = 300.0f;
}

void FWallData::ComputeWallInfo()
{
	
}

void FWallData::ComputeWallLocalPos()
{
	
}

FVector2D FWallData::ConvertScene2LocalPnt(const FVector2D& ScenePnt) const
{
	return FVector2D(0, 0);
}

FVector2D FWallData::ConvertLocal2ScenePnt(const FVector2D& LocalPnt) const
{
	return FVector2D(0, 0);
}
