// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once
#include "AataStructure/ExpansionsDrawingStructure.h"
#include "Layout/CustomCabinetsObject.h"
#include "CoreMinimal.h"

/**
 * 
 */
class LayoutManager;
class CUSTOMLAYOUT_API AreaCabinetsManage
{
public:
	AreaCabinetsManage(FString areatag);
	~AreaCabinetsManage();
private:

	FString AreaTag;
	TMap<CustomCabinetsType, CustomCabinetsObject>AllCabinets;

	Areadata SelectAreaData;
	Selectdata SelectwallData;

public:
	Areadata GetAreaData();
	Selectdata GetwallData();
public:
	
	void SetAreaData(Areadata data);
	void SetSelectData(Selectdata data);


};
