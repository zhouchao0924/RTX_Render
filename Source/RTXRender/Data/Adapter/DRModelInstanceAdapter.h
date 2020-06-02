#pragma once

#include "DataAdapter.h"
#include "DRStruct/DRBuildingStruct.h"
#include "DRModelInstanceAdapter.generated.h"

class UCEditorGameInstance;

UCLASS(BlueprintType)
class UDRModelInstanceAdapter :public UDataAdapter
{
	GENERATED_UCLASS_BODY()
public:
	/********************************Get Function*************************************/
	EDR_ObjectType GetObjectType() override { return EDR_ObjectType::EDR_ModelInstance; }

	UFUNCTION(BlueprintPure)
	EModelType GetType();

	UFUNCTION(BlueprintPure)
	FString GetResID();

	UFUNCTION(BlueprintPure)
	int32 GetAnchorID();

	UFUNCTION(BlueprintPure)
	int32 GetAnchorOwnerID();

	UFUNCTION(BlueprintPure)
	FVector GetLocation();

	UFUNCTION(BlueprintPure)
	FVector GetSize();

	UFUNCTION(BlueprintPure)
	FRotator GetRotation();

	UFUNCTION(BlueprintPure)
	int32 GetAlignType();

	UFUNCTION(BlueprintPure)
	FVector GetClipBase0();
	
	UFUNCTION(BlueprintPure)
	FVector GetClipNormal0();

	UFUNCTION(BlueprintPure)
	FVector GetClipBase1();

	UFUNCTION(BlueprintPure)
	FVector GetClipNormal1();

	UFUNCTION(BlueprintPure)
	int32 GetModelID();

	UFUNCTION(BlueprintPure)
	int32 GetCustomSkuId();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetRoomClassId"))
	int32 GetRoomClassIdByNormalModel();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetCraftId"))
	int32 GetCraftIdByNormalModel();

	UFUNCTION(BlueprintPure)
	FTransform GetTransform();

	UFUNCTION(BlueprintPure)
	FTransform GetTransformBySize();

	UFUNCTION(BlueprintPure)
	FDRMaterial GetDependsMat(const int32 SectionID);

	UFUNCTION(BlueprintPure)
	TMap<int32,FDRMaterial> GetDependsMaterialByValidSections();

	UFUNCTION(BlueprintPure)
	bool GetApplyShadow();

	UFUNCTION(BlueprintPure)
	FString GetModelTag();

	UFUNCTION(BlueprintPure)
	FDRUV GetDependsUV();

	/********************************Get Function*************************************/
	UFUNCTION(BlueprintCallable)
	void SetType(EModelType ModelType);

	UFUNCTION(BlueprintCallable)
	void SetModelID(int32 ModelID);

	UFUNCTION(BlueprintCallable)
	void SetCustomSkuId(const int32 InSkuId);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetRoomClassId"))
	void SetRoomClassIdByNormalModel(const int32 InRoomClassId);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetCraftId"))
	void SetCraftIdByNormalModel(const int32 InCraftId);

	UFUNCTION(BlueprintCallable)
	void SetResId(const FString& InResId);

	UFUNCTION(BlueprintCallable)
	void SetDependsMat(const FDRMaterial &DependsMat, const int32 SectionID);

	UFUNCTION(BlueprintCallable)
	void SetDependsMaterialByValidSections(const TMap<int32, FDRMaterial>& InDependsMaterial);

	UFUNCTION(BlueprintCallable)
	void SetApplyShadow(const bool &ApplyShadow);

	UFUNCTION(BlueprintCallable)
	void SetModelTag(const FString &ModelTag);

	UFUNCTION(BlueprintCallable)
	void SetLocation(const FVector &Location);

	UFUNCTION(BlueprintCallable)
	void SetSize(const FVector &Size);

	UFUNCTION(BlueprintCallable)
	void SetRotation(const FRotator &Rotation);

	UFUNCTION(BlueprintCallable)
	void SetTransformBySize(const FTransform &ModelTransform);

	UFUNCTION(BlueprintCallable)
	void SetDependsUV(const FDRUV& InUV);

private:
	UCEditorGameInstance* GetGameInstance();
};



