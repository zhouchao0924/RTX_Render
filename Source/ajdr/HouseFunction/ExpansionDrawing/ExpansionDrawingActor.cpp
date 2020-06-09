// Copyright? 2017 ihomefnt All Rights Reserved.


#include "ExpansionDrawingActor.h"
#include "BuildingGroupMeshComponent.h"

// Sets default values
AExpansionDrawingActor::AExpansionDrawingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	mModel = CreateDefaultSubobject<UStretchPathComponent>(TEXT("CabinetSpline"));
	RootComponent = mModel;
	bIsFinish = false;
}

// Called when the game starts or when spawned
void AExpansionDrawingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExpansionDrawingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExpansionDrawingActor::CreateTopMesh(TArray<FVector>&PointList, FString& Mx, float z, TArray<int32>& ids, EArticlelofting ty, int32 actortypeid)
{
	type = ty;
	Ids = ids;
	FStretchShapeSlot slot;
	slot.TypeId = 9;
	slot.MX = Mx;
	TArray<FStretchShapeSlot> Slots;
	Slots.Add(slot);
	TopZ = z;
	ActorCategoryId = actortypeid;
	MeshMx = Mx;
	ToPointList = PointList;
	mModel->Update(Slots, ToPointList);
	SetColor(MeshMx);
	SetActorLocation(FVector(0, 0, TopZ));
}

void AExpansionDrawingActor::SetColor(FString MX)
{
	if (MX.IsEmpty())
		return;

	ColorMx = MX;
	mModel->SetSurface(9, ColorMx, 0);

	TArray<USceneComponent*> BuildingComponents;
	mModel->GetChildrenComponents(true, BuildingComponents);
	for (auto p : BuildingComponents)
	{
		UProceduralMeshComponent *Comp1 = Cast<UProceduralMeshComponent>(p);
		if (Comp1)
			Comp1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AExpansionDrawingActor::SetMesh(FString Mx)
{
	if (Mx.IsEmpty())
		return;
	MeshMx = Mx;
	
	FStretchShapeSlot slot;
	slot.TypeId = 9;
	slot.MX = MeshMx;
	TArray<FStretchShapeSlot> Slots;
	Slots.Add(slot);
	
	mModel->Update(Slots, ToPointList);

	SetColor(MeshMx);
}

bool AExpansionDrawingActor::IsFind(int32 id)
{
	return Ids.Find(id) >= 0;
}

TArray<int32>AExpansionDrawingActor::GetSelfActorids()
{
	return Ids;
}

EArticlelofting AExpansionDrawingActor::GetType()
{
	return type;
}

void  AExpansionDrawingActor::FinishArticlelofting()
{
	RecolorMx = ColorMx;
	RemeshMx = MeshMx;
}

void AExpansionDrawingActor::RestoreArticlelofting()
{
	if (type == EArticlelofting::Skirtingboard)
	{
		SetColor(RecolorMx);
	}
	else
	{
		SetMesh(MeshMx);
		SetColor(ColorMx);
	}
}

ADynamicTableboard * AExpansionDrawingActor::GetTable()
{
	return table;
}

void AExpansionDrawingActor::SetTable(ADynamicTableboard* data)
{
	table = data;
}

void AExpansionDrawingActor::SetFunctionManage(AFunctionManage* data)
{
	manage = data;
}

FCustomExpansionDrawingdata AExpansionDrawingActor::GetSaveData()
{
	FCustomExpansionDrawingdata data;
	data.ToPointList = ToPointList;
	data.Ids = Ids;
	if (type == EArticlelofting::VertexAngle)
		data.Hight = GetActorLocation().Z;
	else
		data.Hight = TopZ;

	data.mx = MeshMx;
	return data;
}