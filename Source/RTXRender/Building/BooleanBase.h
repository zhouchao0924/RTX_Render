// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DRStruct.h"
#include "Data/DRStruct/DRBuildingStruct.h"
#include "BooleanBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class AJDR_API UBooleanBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "SetColor"))
	void SetColor(bool bchangecolor=false);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSelect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFlip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Test;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HoleID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WallID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NewWallID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EModelType BooleanType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Verticalflip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool horizontalflip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFirst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Deltavector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bISet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistLength = 0; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsRemoveType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Thickness_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Thickness_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WallID_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float angle_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float angle_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Loc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Loc_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Loc_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EHolestatusType HolestatusType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDeleteHoleInfo DeleteHoleInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> PointList;
};
