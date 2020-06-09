// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DRLightAdapter.h"
#include "DRSpotLightAdapter.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API UDRSpotLightAdapter : public UDRLightAdapter
{
	GENERATED_UCLASS_BODY()
public:
	/********************************Get Function*************************************/
	EDR_ObjectType GetObjectType() override { return EDR_ObjectType::EDR_SpotLight; }
	
	UFUNCTION(BlueprintPure)
	FVector GetLocation();
	
	UFUNCTION(Blueprintpure)
	FRotator GetRotation();
	
	UFUNCTION(BlueprintPure)
	float GetSoftSourceRadius();

	UFUNCTION(BlueprintPure)
	float GetSourceRadius();
	
	UFUNCTION(BlueprintPure)
	float GetSourceLength();
	
	UFUNCTION(BlueprintPure)
	float GetIntensity();
	
	UFUNCTION(BlueprintPure)
	float GetShadowResolutionScale();
	
	UFUNCTION(BlueprintPure)
	float GetAttenuationRadius();
	
	UFUNCTION(BlueprintPure)
	float GetInnerConeAngle();
	
	UFUNCTION(BlueprintPure)
	float GetOuterConeAngle();
	
	UFUNCTION(BlueprintPure)
	float GetColorTemperature();
	
	UFUNCTION(BlueprintPure)
	float GetHighlight();
	
	/********************************Set Function*************************************/
	UFUNCTION(BlueprintCallable)
	void SetLocation(const FVector& InLocation);
	
	UFUNCTION(BlueprintCallable)
	void SetRotation(const FRotator& InRotation);
	
	UFUNCTION(BlueprintCallable)
	void SetSoftSourceRadius(const float InSoftSourceRadius);
	
	UFUNCTION(BlueprintCallable)
	void SetSourceRadius(const float InSourceRadius);
	
	UFUNCTION(BlueprintCallable)
	void SetSourceLength(const float InSourceLength);
	
	UFUNCTION(BlueprintCallable)
	void SetIntensity(const float InIntensity);
	
	UFUNCTION(BlueprintCallable)
	void SetShadowResolutionScale(const float InShadowResolutionScale);
	
	UFUNCTION(BlueprintCallable)
	void SetAttenuationRadius(const float AttenuationRadius);
		
	UFUNCTION(BlueprintCallable)
	void SetInnerConeAngle(const float InInnerConeAngle);
	
	UFUNCTION(BlueprintCallable)
	void SetOuterConeAngle(const float InOuterConeAngle);
	
	UFUNCTION(BlueprintCallable)
	void SetColorTemperature(const float InColorTemperature);
	
	UFUNCTION(BlueprintCallable)
	void SetHighlight(const float InHighlight);
	
};
