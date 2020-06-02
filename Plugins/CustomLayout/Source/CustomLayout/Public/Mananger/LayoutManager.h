// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once
#include "Mananger/AreaCabinetsManage.h"
#include "CoreMinimal.h"

/**
 * 
 */

class CUSTOMLAYOUT_API LayoutManager
{
public:
	LayoutManager();
	~LayoutManager();
public:
	static LayoutManager*GetInstance();

private:

	static LayoutManager* instance;
	TMap<FString, AreaCabinetsManage*>AllAreaManger;
	TMap<int, TArray<CabinetsDate>> TemplateCabinetsList;

public:
	TMap<int, TArray<CabinetsDate>> GetTemplateCabinets();



public:
	AreaCabinetsManage* FindAreaManger(FString areatag);


	void SetTemplateCabinets(TArray<CabinetsDate> Templatelist);
};
