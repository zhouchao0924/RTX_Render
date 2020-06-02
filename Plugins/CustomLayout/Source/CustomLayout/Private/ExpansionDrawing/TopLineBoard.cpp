// Copyright? 2017 ihomefnt All Rights Reserved.

#include "ExpansionDrawing/TopLineBoard.h"
#include "Math/LayoutMath.h"
#include "AataStructure/ExpansionsDrawingStructure.h"

// Sets default values
ATopLineBoard::ATopLineBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	//mModel = CreateDefaultSubobject<UStretchPathComponent>(TEXT("CabinetSpline"));
	//RootComponent = mModel;
}

// Called when the game starts or when spawned
void ATopLineBoard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATopLineBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATopLineBoard::CreateTopMesh(TArray<FVector>& PointList,FString& Mx)
{
	//FStretchShapeSlot slot;
	//slot.TypeId = 9;
	//slot.MX = Mx;
	//TArray<FStretchShapeSlot> Slots;
	//Slots.Add(slot);

	//mModel->Update(Slots, PointList);
}

//void ATopLineBoard::SetTopLineBoardDate(const TArray<AActor*>& FurActor, const TArray <FExpansionLine>& WallList)
//{
//	TArray<ExpansionBasic>TempList;
//
//	for (int i = 0; i < FurActor.Num(); ++i)
//	{
//		bool bIs = true;
//		FVector Size, Ori;
//		FurActor[i]->GetActorBounds(bIs, Ori, Size);
//		FVector Extent = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), Size);
//		ExpansionBasic Temp(Ori, Size, -FurActor[i]->GetActorRightVector());
//		TempList.Add(Temp);
//	}
//
//	LayoutMath::GetInstance()->TopLineBoardCount(TempList, WallList);
//}