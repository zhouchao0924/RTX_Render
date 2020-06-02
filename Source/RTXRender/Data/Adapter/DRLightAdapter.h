#pragma once

#include "DataAdapter.h"
#include "Data/DRStruct/DRBuildingStruct.h"
#include "DRLightAdapter.generated.h"

UCLASS(BlueprintType)
class UDRLightAdapter :public UDataAdapter
{
	GENERATED_UCLASS_BODY()
public:
	/********************************Get Function*************************************/
	EDR_ObjectType GetObjectType() override { return EDR_ObjectType::EDR_Light; }

	UFUNCTION(BlueprintPure)
	ELightType GetLightType();

	UFUNCTION(BlueprintPure)
	FLinearColor GetLightColor();

	UFUNCTION(BlueprintPure)
	bool GetIsCastShadow();

	UFUNCTION(BlueprintPure)
	bool GetIsVisible();
	
	/********************************Set Function*************************************/
	UFUNCTION(BlueprintCallable)
	void SetLightType(const ELightType InLightType);

	UFUNCTION(BlueprintCallable)
	void SetLightColor(const FLinearColor &Color);

	UFUNCTION(BlueprintCallable)
	void SetIsCastShadow(const bool Value);

	UFUNCTION(BlueprintCallable)
	void SetIsVisible(const bool Value);

};



