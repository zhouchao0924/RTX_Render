// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "../CommandTypes.h"
#include "../../../Building/WallPlane.h"
#include "DrawHouseBaseCmd.h"
#include <vector>
#include "IBuildingSDK.h"
#include "DrawWallCmd.generated.h"

class ADRGameMode;
class UBuildingSystem;
USTRUCT(BlueprintType)
struct FSnapLoc
{
	GENERATED_BODY()
	FSnapLoc() { Reset(); }
	bool		bActive;
	int32		BestID;
	FVector2D	Loc;
	void Reset() { bActive = false; BestID = INDEX_NONE; }
};

USTRUCT(BlueprintType)
struct FDrawWallContext
{
	GENERATED_BODY()

public:
	FDrawWallContext()
	{
		LastDrawWallState = DrawWallState = EDrawWallState::NotStart;

		LastSnapCorner = SnapCorner = nullptr;
		LastSnapWall = SnapWall = nullptr;
	}

	void ChangeDrawWallState(EDrawWallState InNewDrawWallState)
	{
		// back up current state
		LastMouseSnap = MouseSnapPos;
		LastSnapCorner = SnapCorner;
		LastSnapWall = SnapWall;

		LastDrawWallState = DrawWallState;

		DrawWallState = InNewDrawWallState;
	}

public:
	FVector2D LastMouseSnap;

	class ACornerActor *LastSnapCorner;
	class ALineWallActor *LastSnapWall;

	FVector2D MouseSnapPos;
	class ACornerActor *SnapCorner;
	class ALineWallActor *SnapWall;

	EDrawWallState LastDrawWallState;
	EDrawWallState DrawWallState;

	UPROPERTY(BlueprintReadWrite)
	FDrawWallSetting DrawWallSetting;
};


/**
 * 
 */
UCLASS()
class AJDR_API UDrawWallCmd : public UDrawHouseBaseCmd
{
	GENERATED_BODY()

public:
	virtual void CommandWillStart();

	virtual void CommandWillEnd();

	virtual void CommandTick(float DeltaTime);

	virtual void OnLeftMouseButtonDown();

	virtual void OnLeftMouseButtonUp();

	virtual void OnRightMouseButtonDown();

	virtual void OnRightMouseButtonUp();

	virtual void OnEscapeDown();

	virtual void CancelCommand();

	UFUNCTION(BlueprintCallable)
	void OnShowConnectedPntsChanged();

	UFUNCTION(BlueprintCallable)
	void OnWallHeightValueChanged();

	UFUNCTION(BlueprintCallable)
	void OnWallThicknessValueChanged();

	UFUNCTION(BlueprintCallable)
	void OnWallColorChanged();

protected:
	void CancelLastOperation(bool bAlsoCancelThisCmd = false);

	FVector2D MousePosSnap2WorldPos();

	FVector2D GetMouseWorldPos();

	void HideCornerSnapState();

	void ShowSnapStateOfCorner(class ACornerActor *InShowedCorner);

	FVector2D LocateMouseSnapState(class ALineWallActor *&OutSnapWall, class ACornerActor *&OutSnapCorner,bool IsShowToast=false);

	void CutWallByPnt(TArray<FString> &OutCorners2Update, class ACornerActor *&CutCorner,
		class ALineWallActor *InWall2Break, const FVector2D &CutPnt);

	void SplitWallIntoSegments(TArray<FString> &CornersNeeds2Update);

	void SplitWallByIntersection(TArray<FString> &CornersNeeds2Update, class ALineWallActor *NewWall2Split);

	void UpdataCorner();

	void FillWallDefaultProperties(int32 WallId);
	
	void AddWallInsideCorner(FVector2D MouseSnapPos, int32& NewPointIndex);//墙内线画法，添加Corner方法

public:
	void CheckAndAddCornerWall(FVector2D CurrentMousePos, int32& NewIndex);

protected:
	UPROPERTY(BlueprintReadWrite)
	FDrawWallContext DrawWallContext;

	UPROPERTY()
	class UWallBuildSystem *WallSystem;

	UPROPERTY()
	TArray<class ALineWallActor*> WallsOnPath;

	UPROPERTY()
	class UMouseSnapWidget* MouseSnapWidget;

public:
	AWallPlaneBase* WallPlaneTemp;
	FSnapLoc	_SnapPoint;
	bool bRightButtonDown;

protected:
	FVector2D LastMouseScreenPos;
	bool bMovedInRMBD;
	int32	LastPoint;
	UBuildingSystem  *BuildingSystem;
	ADRGameMode  *DRGameMode;
	FVector2D LastLeftMousePos;

};
