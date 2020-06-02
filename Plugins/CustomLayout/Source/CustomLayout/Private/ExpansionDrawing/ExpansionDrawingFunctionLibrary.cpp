// Copyright? 2017 ihomefnt All Rights Reserved.

#include "ExpansionDrawingFunctionLibrary.h"
#include "Math/LayoutMath.h"
#include "TopLineBoard.h"

void UExpansionDrawingFunctionLibrary::CreateExpansionDrawingMesh(UObject * WorldContextObject,const TArray<AActor*>& FurActor, const TArray <FExpansionLine>& WallList, FString mx)
{
	/*TArray<ExpansionBasic>TempList;

	for (int i = 0; i < FurActor.Num(); ++i)
	{
		bool bIs = true;
		FVector Size, Ori;
		FurActor[i]->GetActorBounds(bIs, Ori, Size);
		FVector Extent = LayoutMath::GetInstance()->GetPosToRotator(FurActor[i]->GetActorRotation(), Size);
		ExpansionBasic Temp(Ori, Size, -FurActor[i]->GetActorRightVector());
		TempList.Add(Temp);
	}

	auto list = LayoutMath::GetInstance()->TopLineBoardCount(TempList, WallList);
	for(auto i : list)
	{
		auto Actor = WorldContextObject->GetWorld()->SpawnActor<ATopLineBoard>();
		Actor->CreateTopMesh(i, mx);
	}*/

}
