// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DRWallAdapter.h"
#include "Data/DRStruct/DRBuildingStruct.h"
#include "DRSolidWallAdapter.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UDRSolidWallAdapter : public UDRWallAdapter
{
	GENERATED_UCLASS_BODY()
public:
	EDR_ObjectType GetObjectType() override { return EDR_ObjectType::EDR_SolidWall; }

	/********************************Get Function*************************************/
	UFUNCTION(BlueprintPure)
	bool GetRoomWall();

	UFUNCTION(BlueprintPure)
	ESolidWallType GetSolidWallType();

	UFUNCTION(BlueprintPure)
	bool GetMainWall();

	UFUNCTION(BlueprintPure)
	FDRWallPosition GetWallPositionData();

	UFUNCTION(BlueprintPure)
	float GetLeftThick();

	UFUNCTION(BlueprintPure)
	float GetRightThick();

	UFUNCTION(BlueprintPure)
	float GetHeight();

	//GetHeightByProperty此方法仅限ZTB使用
	UFUNCTION(BlueprintPure)
	float GetHeightByProperty();

	UFUNCTION(BlueprintPure)
	float GetGroundHeight();

	UFUNCTION(BlueprintPure)
	FVector2D GetFrontUVScale();

	UFUNCTION(BlueprintPure)
	FVector2D GetSideUVScale();

	UFUNCTION(BlueprintPure)
	FVector2D GetBackUVScale();

	UFUNCTION(BlueprintPure)
	bool GetLeftRuler();

	UFUNCTION(BlueprintPure)
	bool GetRightRuler();

	UFUNCTION(BlueprintPure)
	float GetFrontUVAngle();

	UFUNCTION(BlueprintPure)
	float GetSideUVAngle();

	UFUNCTION(BlueprintPure)
	float GetBackUVAngle();

	UFUNCTION(BlueprintPure)
	FVector2D GetFrontUVPos();

	UFUNCTION(BlueprintPure)
	FVector2D GetSideUVPos();

	UFUNCTION(BlueprintPure)
	FVector2D GetBackUVPos();

	UFUNCTION(BlueprintPure)
	TArray<FDRMaterial> GetWallMaterials();

	UFUNCTION(BlueprintPure)
	TArray<int32> GetHoles();

	UFUNCTION(BlueprintPure)
	FString GetTagName();

	UFUNCTION(BlueprintPure)
	ENewWallType GetNewWallType();
	/********************************Set Function*************************************/
	UFUNCTION(BlueprintCallable)
	void SetRoomWall(const bool bRoomWall);

	UFUNCTION(BlueprintCallable)
	void SetSolidWallType(const ESolidWallType InSolidWallType);

	UFUNCTION(BlueprintCallable)
	void SetMainWall(const bool bMainWall);

	UFUNCTION(BlueprintCallable)
	void SetLeftThick(const float InLeftThick);

	UFUNCTION(BlueprintCallable)
	void SetRightThick(const float InRightThick);

	UFUNCTION(BlueprintCallable)
	void SetHeight(const float InHeight);

	UFUNCTION(BlueprintCallable)
	void SetGroundHeight(const float GroundHeight);

	UFUNCTION(BlueprintCallable)
	void SetFrontUVScale(const FVector2D& InFrontUVScale);

	UFUNCTION(BlueprintCallable)
	void SetSideUVScale(const FVector2D& InSideUVScale);

	UFUNCTION(BlueprintCallable)
	void SetBackUVScale(const FVector2D& InBackUVScale);

	UFUNCTION(BlueprintCallable)
	void SetLeftRuler(const bool bLeftRuler);

	UFUNCTION(BlueprintCallable)
	void SetRightRuler(const bool bRightRuler);

	UFUNCTION(BlueprintCallable)
	void SetFrontUVAngle(const float InFrontUVAngle);

	UFUNCTION(BlueprintCallable)
	void SetSideUVAngle(const float InSideUVAngle);

	UFUNCTION(BlueprintCallable)
	void SetBackUVAngle(const float InBackUVAngle);

	UFUNCTION(BlueprintCallable)
	void SetFrontUVPos(const FVector2D& InFrontUVPos);

	UFUNCTION(BlueprintCallable)
	void SetSideUVPos(const FVector2D& InSideUVPos);

	UFUNCTION(BlueprintCallable)
	void SetBackUVPos(const FVector2D& InBackUVPos);

	UFUNCTION(BlueprintCallable)
	void SetWallMaterials(const TArray<FDRMaterial>& InWallMaterials);
	
	UFUNCTION(BlueprintCallable)
	void SetWallTagName(const FString& TagName);

	UFUNCTION(BlueprintCallable)
	void SetNewWallType(const ENewWallType NewWallType);
};
