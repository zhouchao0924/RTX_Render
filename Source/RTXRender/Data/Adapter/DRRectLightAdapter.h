// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Adapter/DRLightAdapter.h"
#include "DRRectLightAdapter.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UDRRectLightAdapter : public UDRLightAdapter
{
	GENERATED_BODY()
public:
	/********************************Get Function*************************************/
	EDR_ObjectType GetObjectType()override { return EDR_ObjectType::EDR_RectLight; }

	UFUNCTION(BlueprintPure)
		FVector GetLocation();

	UFUNCTION(Blueprintpure)
		FRotator GetRotation();

	UFUNCTION(BlueprintPure)
		float GetIntensity();

	UFUNCTION(BlueprintPure)
		float GetAttenuationRadius();

	UFUNCTION(BlueprintPure)
		float GetSourceWidth();

	UFUNCTION(BlueprintPure)
		float GetSourceHeight();

	UFUNCTION(BlueprintPure)
		float GetBarnDoorAngle();

	UFUNCTION(BlueprintPure)
		float GetBarnDoorLength();

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
		void SetIntensity(const float InIntensity);

	UFUNCTION(BlueprintCallable)
		void SetAttenuationRadius(const float InAttenuationRadius);

	UFUNCTION(BlueprintCallable)
		void SetSourceWidth(const float InSourceWidth);

	UFUNCTION(BlueprintCallable)
		void SetSourceHeight(const float InSourceHeight);

	UFUNCTION(BlueprintCallable)
		void SetBarnDoorAngle(const float InBarnDoorAngle);

	UFUNCTION(BlueprintCallable)
		void SetBarnDoorLength(const float InBarnDoorLength);

	UFUNCTION(BlueprintCallable)
		void SetColorTemperature(const float InColorTemperature);

	UFUNCTION(BlueprintCallable)
		void SetHighlight(const float InHighlight);
};
