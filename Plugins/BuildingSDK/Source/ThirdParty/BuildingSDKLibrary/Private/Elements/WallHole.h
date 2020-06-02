
#pragma once


#include "kLine.h"
#include "Anchor.h"
#include "Math/kVector2D.h"

class WallHole :public  Anchor
{
	DEFIN_CLASS()
public:
	WallHole();
	bool IsDeletable() override { return true; }
	void OnDestroy() override;
	EObjectType GetType() { return EWallHole; }
	void GetPolygon(kPoint *&pPolygon, int &Num);
public:
	ObjectID WallID;
	float	 ZPos;
	float	 Width;
	float	 Height;
	float	 Thickness;
	bool	 bFlip;
	std::vector<kPoint> polygons;
};


