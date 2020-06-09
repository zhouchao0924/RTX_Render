// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DataAdapter.h"
#include "Data/DRStruct/DRBuildingStruct.h"
#include "Building/DRStruct.h"
#include "DRHoleAdapter.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API UDRHoleAdapter : public UDataAdapter
{
	GENERATED_UCLASS_BODY()
public:
	/********************************Get Function*************************************/
	EDR_ObjectType GetObjectType()override { return EDR_ObjectType::EDR_WallHole; }

	UFUNCTION(BlueprintPure)
	EModelType GetHoleType();

	UFUNCTION(BlueprintPure)
	FVector GetLocation();

	UFUNCTION(BlueprintPure)
	FVector GetSize();

	UFUNCTION(BlueprintPure)
	FRotator GetRotation();

	UFUNCTION(BlueprintPure)
	FTransform GetTransform();

	UFUNCTION(BlueprintPure)
	int32 GetDependWallId();

	UFUNCTION(BlueprintPure)
	float GetWallThickLeft();
	
	UFUNCTION(BlueprintPure)
	float GetWallThickRight();

	UFUNCTION(BlueprintPure)
	float GetWidth();

	UFUNCTION(BlueprintPure)
	float GetHeight();

	UFUNCTION(BlueprintPure)
	float GetThickness();

	UFUNCTION(BlueprintPure)
	float GetGroundHeight();

	UFUNCTION(BlueprintPure)
	bool GetHorizontalFlip();

	UFUNCTION(BlueprintPure)
	bool GetVerticalFlip();

	UFUNCTION(BlueprintPure)
	FString GetSelfTagName();

	UFUNCTION(BlueprintPure)
	FString GetSlotActorTag();

	UFUNCTION(BlueprintPure)
	TArray<FVector2D> GetPoints();

	UFUNCTION(BlueprintPure)
	TArray<FDRMaterial> GetHoleMaterials();

	UFUNCTION(BlueprintPure)
	TArray<FDRUV> GetHoleUVs();

	UFUNCTION(BlueprintPure)
	EHolestatusType GetHoleStatusType();
	/********************************Set Function*************************************/
	UFUNCTION(BlueprintCallable)
	void SetHoleType(const EModelType InHoleType);

	UFUNCTION(BlueprintCallable)
	void SetGroundHeight(const float InGroundHeight);

	UFUNCTION(BlueprintCallable)
	void SetHeight(const float InHeight);

	UFUNCTION(BlueprintCallable)
	void SetWidth(const float InWidth);

	UFUNCTION(BlueprintCallable)
	void SetHorizontalFlip(const bool bInFlip);

	UFUNCTION(BlueprintCallable)
	void SetVerticalFlip(const bool bInFlip);

	UFUNCTION(BlueprintCallable)
	void SetSelfTagName(const FString& InSelfTagName);

	UFUNCTION(BlueprintCallable)
	void SetSlotActorTag(const FString& InSlotActorTag);

	UFUNCTION(BlueprintCallable)
	void SetHoleMaterials(const TArray<FDRMaterial>& InHoleMaterials);

	UFUNCTION(BlueprintCallable)
	void SetHoleUVs(const TArray<FDRUV>& InHoleUVs);

	UFUNCTION(BlueprintCallable)
		void SetHoleStatusType(const EHolestatusType HoleStatusType);
protected:
	UDRMaterialAdapter *GetMaterial(int32 ObjID, int32 SectionIndex) override;
};
