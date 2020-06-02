// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DynamicTableboard.h"


// Sets default values
ADynamicTableboard::ADynamicTableboard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	mModel = CreateDefaultSubobject<UTriangulatedMeshComponent>(TEXT("CabinetTable"));
	RootComponent = mModel;
	bIsFinish = false;
}

// Called when the game starts or when spawned
void ADynamicTableboard::BeginPlay()
{
	Super::BeginPlay();
	
}



void ADynamicTableboard::SetMaterial(FString mx)
{
	mMx = mx;
	mModel->SetSurface(mMx);
}

void ADynamicTableboard::CreateMesh(TArray<FVector2D>& PathFrame, TArray<TArray<FVector2D>>& holes, float Thickness, FString Mx, float fHeight, TArray<int32>& ids, int32 actortypeid)
{
	Ids = ids;
	PointList = PathFrame;
	Hight = fHeight;

	Holes = holes;
	mModel->CreatePolygon(PointList, Thickness);
	for (auto i : holes)
		mModel->AddHole(i);

	ActorCategoryId = actortypeid;
	SetActorLocation(FVector(0,0, Hight));
	SetMaterial(Mx);
}

bool ADynamicTableboard::IsFind(int32 id)
{
	return Ids.Find(id) >= 0;
}

TArray<int32> ADynamicTableboard::GetSelfActorids()
{
	return Ids;
}


void ADynamicTableboard::FinishArticlelofting()
{
	RecolorMx = mMx;
}

void ADynamicTableboard::RestoreArticlelofting()
{

	SetMaterial(RecolorMx);
}

TArray<AExpansionDrawingActor*> ADynamicTableboard::Getbreakwaters()
{
	return breakwaters;
}

void ADynamicTableboard::Addbreakwaters(AExpansionDrawingActor* data)
{
	breakwaters.Add(data);
}

void ADynamicTableboard::SetFunctionManage(AFunctionManage * data)
{
	manage = data;
}

FCustomDynamicTableboarddata ADynamicTableboard::GetSaveData()
{
	FCustomDynamicTableboarddata data;
	data.ToPointList = PointList;
	data.Ids = Ids;
	data.Hight = Hight;
	data.mx = mMx;

	if(Holes.Num() > 0)
	data.Holes = Holes[0];
	return  data;
}