// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DataAdapter.h"
#include "Data/DRStruct/DRBuildingStruct.h"
#include "DRAreaAdapter.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API UDRAreaAdapter : public UDataAdapter
{
	GENERATED_UCLASS_BODY()
public:
	/********************************Get Function*************************************/
	EDR_ObjectType GetObjectType() override { return EDR_ObjectType::EDR_Room; }
	
	UFUNCTION(BlueprintPure)
	TArray<FVector2D> GetPointList();

	UFUNCTION(BlueprintPure)
	FString GetTagName();

	UFUNCTION(BlueprintPure)
	float GetHeight();

	UFUNCTION(BlueprintPure)
	int32 GetRoomID();

	UFUNCTION(BlueprintPure)
	FString GetRoomName();

	UFUNCTION(BlueprintPure)
	int32 GetRoomUsageId();

	UFUNCTION(BlueprintPure)
	FString GetRoomUsageName();

	UFUNCTION(BlueprintPure)
	float GetArea();

	UFUNCTION(BlueprintPure)
	int32 GetTypeId();

	UFUNCTION(BlueprintPure)
	int32 GetRoomUseIndex();

	UFUNCTION(BlueprintPure)
	TArray<FDRMaterial> GetAreaMaterials();

	UFUNCTION(BlueprintPure)
	int32 GetCeilID();

	UFUNCTION(BlueprintPure)
	int32 GetFloorID();

	UFUNCTION(BlueprintPure)
	FVector2D GetFloorUVScale();

	UFUNCTION(BlueprintPure)
	FVector2D GetFloorUVOffset();

	UFUNCTION(BlueprintPure)
	float GetFloorUVRot();

	/********************************Set Function*************************************/
	UFUNCTION(BlueprintCallable)
	void SetTagName(const FString &TagName);

	UFUNCTION(BlueprintCallable)
	void SetHeight(const float &Height);

	UFUNCTION(BlueprintCallable)
	void SetRoomID(const int32 &RoomID);

	UFUNCTION(BlueprintCallable)
	void SetRoomName(const FString &RoomName);

	UFUNCTION(BlueprintCallable)
	void SetRoomUsageId(const int32 &RoomUsageId);

	UFUNCTION(BlueprintCallable)
	void SetRoomUsageName(const FString &RoomUsageName);

	UFUNCTION(BlueprintCallable)
	void SetArea(const float &Area);

	UFUNCTION(BlueprintCallable)
	void SetTypeId(const int32 &TypeId);

	UFUNCTION(BlueprintCallable)
	void SetRoomUseIndex(const int32 &RoomUseIndex);

	UFUNCTION(BlueprintCallable)
	void SetAreaMaterials(const TArray<FDRMaterial> &AreaMaterials);

	UFUNCTION(BlueprintCallable)
	void SetFloorUVScale(const FVector2D& InUVScale);

	UFUNCTION(BlueprintCallable)
	void SetFloorUVOffset(const FVector2D& InUVOffset);

	UFUNCTION(BlueprintCallable)
	void SetFloorUVRot(const float InUVRot);

};
