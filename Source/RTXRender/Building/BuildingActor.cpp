
#include "BuildingActor.h"
#include "BuildingComponent.h"

ABuildingActor::ABuildingActor(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, bTopCeil(0)
{
}

void ABuildingActor::Update(UBuildingData *Data)
{
	UBuildingComponent *Comp = FindComponentByClass<UBuildingComponent>();
	if (Comp)
	{
		Comp->SetData(Data);
	}
}

