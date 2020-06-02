// Copyright? 2017 ihomefnt All Rights Reserved.

#include "Layout/CustomCabinetsObject.h"
#include "Mananger/AreaCabinetsManage.h"
#include "Math/LayoutMath.h"
#include "Mananger/LayoutManager.h"

CustomCabinetsObject::CustomCabinetsObject()
{
}

CustomCabinetsObject::CustomCabinetsObject(AreaCabinetsManage * area, CustomCabinetsType type)
{
	AreaManager = area;
	CabinetType = type; 
}

CustomCabinetsObject::~CustomCabinetsObject()
{
}

void CustomCabinetsObject::PolishingWalldata()
{
	auto SelectArea = GetSelectAreaData();
	auto SelectWall = GetSelectWallData();
	for (auto j : SelectWall.wallList)
	{
		FLine line(j.Start,j.End);
		FSelectWalldata tempwall;
		for (auto i : SelectArea.wallList)
		{
			FLine line1(i.Start, i.End);
			if (LayoutMath::GetInstance()->IsPointToLine(j.Start, line1))
			{	
				tempwall.Wall.Set(i.Start, i.End);
				tempwall.Dir = j.Dir;
				tempwall.Id = i.Id;
				break;
			}
		}

		for (auto i : SelectArea.doorwindowList)
		{
			FLine line1(i.start, i.end);
			auto pos = line1.GetMiddlePoint();
			if (LayoutMath::GetInstance()->IsPointToLine(pos, line))
			{
				DoorWindow tempdoorwindow = i;		
				tempwall.DoorWindowlist.Add(tempdoorwindow);
			}
		}

		for (auto i : SelectArea.PillarsList)
		{
			if (LayoutMath::GetInstance()->IsPointToLine(i.Pos, line))
			{
				FExpansionCabnetPillars temppillars = i;
				tempwall.PillarsList.Add(temppillars);
			}
		}
		PolishingSelectwall.Add(tempwall);
	}

	for (int i = 0; i < PolishingSelectwall.Num(); ++i)
	{
		for (int j = 0; j < PolishingSelectwall.Num(); ++j)
		{
			if (i == j)
				continue;
			if (PolishingSelectwall[i].Wall.Start.Point.Equals(PolishingSelectwall[j].Wall.End.Point, 1.f))
				PolishingSelectwall[i].Wall.Start.RelevancePoint.Add(PolishingSelectwall[j].Wall.End);

			else if(PolishingSelectwall[i].Wall.Start.Point.Equals(PolishingSelectwall[j].Wall.Start.Point, 1.f))
				PolishingSelectwall[i].Wall.Start.RelevancePoint.Add(PolishingSelectwall[j].Wall.Start);

			else if (PolishingSelectwall[i].Wall.End.Point.Equals(PolishingSelectwall[j].Wall.End.Point, 1.f))
				PolishingSelectwall[i].Wall.End.RelevancePoint.Add(PolishingSelectwall[j].Wall.End);

			else if (PolishingSelectwall[i].Wall.End.Point.Equals(PolishingSelectwall[j].Wall.Start.Point, 1.f))
				PolishingSelectwall[i].Wall.End.RelevancePoint.Add(PolishingSelectwall[j].Wall.Start);

		}
	}



}

void CustomCabinetsObject::AtuoLayout()
{
}

TMap<int, TArray<CabinetsDate>> CustomCabinetsObject::GetTemplist()
{
	return LayoutManager::GetInstance()->GetTemplateCabinets();
}

Areadata CustomCabinetsObject::GetSelectAreaData()
{
	return AreaManager->GetAreaData();
}

Selectdata CustomCabinetsObject::GetSelectWallData()
{
	return AreaManager->GetwallData();
}

TArray<FSelectWalldata> CustomCabinetsObject::GetPolishingSelectwall()
{
	return PolishingSelectwall;
}



