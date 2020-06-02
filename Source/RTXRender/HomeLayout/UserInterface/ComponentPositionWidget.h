// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "ComponentPositionWidget.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UComponentPositionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Update")
		void UpdateCompInfo();

	UFUNCTION(BlueprintCallable, Category = "Update")
		void UpDateTopLength(float Length);
	UFUNCTION(BlueprintCallable, Category = "Update")
		void UpDateBottomLength(float Length);
	UFUNCTION(BlueprintCallable, Category = "Update")
		void UpDateLeftLength(float Length);
	UFUNCTION(BlueprintCallable, Category = "Update")
		void UpDateRightLength(float Length);
	UFUNCTION(BlueprintImplementableEvent, Category = "Update")
		void SetWidgetToViewport(FVector2D OffsetPos);
	UFUNCTION(BlueprintImplementableEvent, Category = "Update")
		void SetWidgetAngle(float Angle);
	UFUNCTION(BlueprintCallable, Category = "Filterq		1")
		FString FilterCharacter(FString InputStr);


private:
	void UpdateComponentPos(FVector2D CurrentPos);
	void NewUpdateArea(int32 id);
	float CheckCompAngel(FVector2D LateTargetPos);
	void ResetCompPos(float RotateAngle);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		int32 ObjectID = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		FVector2D TargetPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		float mWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		float mLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		float mAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		TArray<FVector2D> TPoylgon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		UEditableTextBox* TopTextBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		UEditableTextBox* BottomTextBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		UEditableTextBox* LeftTextBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		UEditableTextBox* RightTextBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		UCanvasPanel* MainCanvasPanel;

	//previous Date save
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		float LastTopDist = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		float LastBottomDist = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		float LastLeftDist = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		float LastRightDist = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
		float LastInputLength = -1;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
//		FVector2D CurrentScreenPos;

public:
	UPROPERTY(BlueprintReadWrite, meta = (Category = "Property"))
	class AComponentManagerActor* CompManager;

private:
	class UBuildingSystem* BuildingSystem;

	TArray<FVector2D> TopVec;
	TArray<FVector2D> BottomVec;
	TArray<FVector2D> LeftVec;
	TArray<FVector2D> RightVec;

	bool bResetPos = false;
//	bool bMoved = false;
};
