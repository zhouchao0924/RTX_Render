#pragma once

#include "DataAdapter.h"
#include "DRPostProcessAdapter.generated.h"

UCLASS(BlueprintType)
class UDRPostProcessAdapter :public UDataAdapter
{
	GENERATED_UCLASS_BODY()
public:
	EDR_ObjectType GetObjectType() override { return EDR_ObjectType::EDR_PostProcess; }

	UFUNCTION(BlueprintCallable)
	float GetSaturation();

	UFUNCTION(BlueprintCallable)
	float GetConstrast();

	UFUNCTION(BlueprintCallable)
	float GetBloomIntensity();
	
	UFUNCTION(BlueprintCallable)
	float GetAmbientOcclusion();

	UFUNCTION(BlueprintCallable)
	float GetAmbientOcclusionRadius();

	UFUNCTION(BlueprintCallable)
	void SetSaturation(float Value);

	UFUNCTION(BlueprintCallable)
	void SetConstrast(float Value);

	UFUNCTION(BlueprintCallable)
	void SetBloomIntensity(float Value);

	UFUNCTION(BlueprintCallable)
	void SetAmbientOcclusion(float Value);

	UFUNCTION(BlueprintCallable)
	void SetAmbientOcclusionRadius(float Value);
};



