// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once
#include "Layout/CustomCabinetsObject.h"
#include "Math/LayoutMath.h"
#include "CoreMinimal.h"

/**
 * 
 */
struct  CabinetCustom
{
	TArray<CabinetsDate>CabinetList;
	bool IsLeft,IsRight;
	float Length;
};

class CUSTOMLAYOUT_API CabinetCustomization : public CustomCabinetsObject
{
public:
	CabinetCustomization();
	~CabinetCustomization();

public:
	CabinetCustom LayoutFloorCabinet(const FSelectWalldata& data);


public:

	virtual void AtuoLayout() override;

	void Execute(FSelectWalldata data);
	//ÊÇ·ñÓÐÂ¯Ôî£¬Ë®²Û¹ñ
	bool bIsRangeCabinet, bIsWaterCabinet;

	TArray<CabinetCustom> WallCabinetList;


};
