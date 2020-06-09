
#pragma once

#include "Building/DRStruct.h"
#include "Building/DRActor.h"
#include "Building/BuildingData.h"
#include "Model/ModelFileComponent.h"
#include "BooleanBase.h"
#include "DRComponentModel.generated.h"


UCLASS(BlueprintType)
class ADRComponentModel : public ADRActor
{
	GENERATED_BODY()
private:
	
	UModelFileComponent* ModelCom;
public:
	UPROPERTY(BlueprintReadWrite)
		UBuildingData * BuildData;
	UPROPERTY(BlueprintReadWrite)
		bool isLoadingData;
	UPROPERTY(BlueprintReadWrite)
		EDRModelType ComModelType;
	UPROPERTY(BlueprintReadWrite)
		FDRModelInstance ModelStruct;
	UPROPERTY(BlueprintReadWrite)
		FDRHoleStruct HoleStruct;
public:
	ADRComponentModel();
public:
	UFUNCTION(BlueprintCallable)
	virtual void UpdateBuildingData() override;

	virtual void SaveSelf() override;
	virtual void LoadSelf() override;

public:
	UFUNCTION(BlueprintCallable)
		void UpdateHoleLocationInfo();
	UFUNCTION(BlueprintCallable)
		void UpdateModelRes(const FDRModelInstance& Model);
public:
	UFUNCTION(BlueprintCallable)
		int32 GetHoleID();
	UFUNCTION(BlueprintCallable)
		void SetNewWallID(int32 ID);
	UFUNCTION(BlueprintCallable)
		void MoveComponent(bool IsSelected = false);
	UFUNCTION(BlueprintCallable)
		void Init();
	UFUNCTION(BlueprintCallable)
		void ReadyMove(bool IsReadyMove = false);
	UFUNCTION(BlueprintCallable)
		void Selected(bool IsSelected = false);
	UFUNCTION(BlueprintCallable)
		void UpDataWallID(int32 ID);
	UFUNCTION(BlueprintCallable)
		void GetMoveCurWallID(int32 &ID);
	UFUNCTION(BlueprintCallable)
		void GetCurrentHoleWidth(float &Width);
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UBooleanBase *BooleanBaseHUDWidget;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsSelect;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsMove;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsReadyMove;
};