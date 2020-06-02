// Copyright? 2017 ihomefnt All Rights Reserved.

#include "Layout/CabinetCustomization.h"

CabinetCustomization::CabinetCustomization()
{
}

CabinetCustomization::~CabinetCustomization()
{
}

CabinetCustom CabinetCustomization::LayoutFloorCabinet(const FSelectWalldata & data)
{
	FLine line1, line2, line3;
	line1.Start = data.Wall.Start.Point + data.Dir * 10.f;
	line1.End = data.Wall.End.Point + data.Dir * 10.f;

	auto Templist = data.Wall.Start.RelevancePoint;


	return CabinetCustom();
}

void CabinetCustomization::AtuoLayout()
{
	auto TemplateCabinets = GetTemplist();
	bIsWaterCabinet = bIsRangeCabinet = false;
	for (int i = 0; i < PolishingSelectwall.Num(); ++i)
	{
		if (PolishingSelectwall[i].Wall.Start.RelevancePoint.Num() > 1)
		{
			
		}

    }



}

void CabinetCustomization::Execute(FSelectWalldata data)
{
}
