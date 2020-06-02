// light node
#pragma once

#include "ModelConfigLight.generated.h"

// model config light
USTRUCT(Blueprintable)
struct FModelConfigLightCPP
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LightIntensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor LightColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InnerConeAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float OuterConeAngle;
};