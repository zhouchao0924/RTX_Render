// Copyright? 2017 ihomefnt All Rights Reserved.


#include "Mananger/AreaCabinetsManage.h"
#include "Mananger/LayoutManager.h"

AreaCabinetsManage::AreaCabinetsManage(FString areatag)
{
	AreaTag = areatag;
}

AreaCabinetsManage::~AreaCabinetsManage()
{
}

Areadata AreaCabinetsManage::GetAreaData()
{
	return SelectAreaData;
}

Selectdata AreaCabinetsManage::GetwallData()
{
	return SelectwallData;
}

void AreaCabinetsManage::SetAreaData(Areadata data)
{
	SelectAreaData = data;
}

void AreaCabinetsManage::SetSelectData(Selectdata data)
{
	SelectwallData = data;
}
