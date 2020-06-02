// Copyright? 2017 ihomefnt All Rights Reserved.

//#include "LayoutManager.h"
#include "Mananger/LayoutManager.h"



LayoutManager* LayoutManager::instance = nullptr;
LayoutManager::LayoutManager()
{
}

LayoutManager::~LayoutManager()
{
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}

LayoutManager * LayoutManager::GetInstance()
{
	if (!instance)
		instance = new LayoutManager();
	
	return instance;
}

TMap<int, TArray<CabinetsDate>> LayoutManager::GetTemplateCabinets()
{
	return TemplateCabinetsList;
}

AreaCabinetsManage * LayoutManager::FindAreaManger(FString areatag)
{
	auto Temp = AllAreaManger.Find(areatag);
	if(Temp)
	return (*Temp);

	AreaCabinetsManage* Temp1 = new AreaCabinetsManage(areatag);
	return Temp1;
}

void LayoutManager::SetTemplateCabinets(TArray<CabinetsDate> Templatelist)
{
	for (auto i : Templatelist)
	{
		auto temp = TemplateCabinetsList.Find(i.Categoryid);
		if (temp)
		{
			(*temp).Add(i);
			TemplateCabinetsList.Add(i.Categoryid, (*temp));
		}
		else
		{
			TArray<CabinetsDate> TempList;
			TempList.Add(i);
			TemplateCabinetsList.Add(i.Categoryid, TempList);
		}
	}
	

}
