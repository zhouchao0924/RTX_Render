// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "AataStructure/ExpansionsDrawingStructure.h"
#include "CoreMinimal.h"

/**
 * 
 */
class AreaCabinetsManage;

class CUSTOMLAYOUT_API CustomCabinetsObject
{
public:
	CustomCabinetsObject();
	CustomCabinetsObject(AreaCabinetsManage* area, CustomCabinetsType type);
	~CustomCabinetsObject();

protected:
	AreaCabinetsManage* AreaManager;
	CustomCabinetsType CabinetType;
	TArray<FSelectWalldata> PolishingSelectwall;
public:
	void PolishingWalldata();

	virtual void AtuoLayout();

public:
	
	TMap<int, TArray<CabinetsDate>> GetTemplist();
	Areadata GetSelectAreaData();
	Selectdata GetSelectWallData();
	TArray<FSelectWalldata> GetPolishingSelectwall();
	

};
