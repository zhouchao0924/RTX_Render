// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRSpotLightAdapter.h"

UDRSpotLightAdapter::UDRSpotLightAdapter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

/********************************Get Function*************************************/
FVector UDRSpotLightAdapter::GetLocation()
{
	return ToVector(Obj->GetPropertyVector3D("Location"));
}

FRotator UDRSpotLightAdapter::GetRotation()
{
	return ToRotator(Obj->GetPropertyRotator("Rotation"));
}

float UDRSpotLightAdapter::GetSoftSourceRadius()
{
	return Obj->GetPropertyFloat("SoftSourceRadius");
}

float UDRSpotLightAdapter::GetSourceRadius()
{
	return Obj->GetPropertyFloat("SourceRadius");
}

float UDRSpotLightAdapter::GetSourceLength()
{
	return Obj->GetPropertyFloat("SourceLength");
}

float UDRSpotLightAdapter::GetIntensity()
{
	return Obj->GetPropertyFloat("Intensity");
}

float UDRSpotLightAdapter::GetShadowResolutionScale()
{
	return Obj->GetPropertyFloat("ShadowResolutionScale");
}

float UDRSpotLightAdapter::GetAttenuationRadius()
{
	return Obj->GetPropertyFloat("AttenuationRadius");
}

float UDRSpotLightAdapter::GetInnerConeAngle()
{
	return Obj->GetPropertyFloat("InnerConeAngle");
}

float UDRSpotLightAdapter::GetOuterConeAngle()
{
	return Obj->GetPropertyFloat("OuterConeAngle");
}

float UDRSpotLightAdapter::GetColorTemperature()
{
	return Obj->GetPropertyFloat("ColorTemperature");
}

float UDRSpotLightAdapter::GetHighlight()
{
	return Obj->GetPropertyFloat("Highlight");
}

/********************************Set Function*************************************/

void UDRSpotLightAdapter::SetLocation(const FVector& InLocation)
{
	Obj->SetPropertyVector3D("Location", ToKVector3D(InLocation));
}

void UDRSpotLightAdapter::SetRotation(const FRotator& InRotation)
{
	Obj->SetPropertyRotator("Rotation",ToKRotation(InRotation));
}

void UDRSpotLightAdapter::SetSoftSourceRadius(const float InSoftSourceRadius)
{
	Obj->SetPropertyFloat("SoftSourceRadius", InSoftSourceRadius);
}

void UDRSpotLightAdapter::SetSourceRadius(const float InSourceRadius)
{
	Obj->SetPropertyFloat("SourceRadius", InSourceRadius);
}

void UDRSpotLightAdapter::SetSourceLength(const float InSourceLength)
{
	Obj->SetPropertyFloat("SourceLength", InSourceLength);
}

void UDRSpotLightAdapter::SetIntensity(const float InIntensity)
{
	Obj->SetPropertyFloat("Intensity", InIntensity);
}

void UDRSpotLightAdapter::SetShadowResolutionScale(const float InShadowResolutionScale)
{
	Obj->SetPropertyFloat("ShadowResolutionScale", InShadowResolutionScale);
}

void UDRSpotLightAdapter::SetAttenuationRadius(const float InAttenuationRadius)
{
	Obj->SetPropertyFloat("AttenuationRadius", InAttenuationRadius);
}

void UDRSpotLightAdapter::SetInnerConeAngle(const float InInnerConeAngle)
{
	Obj->SetPropertyFloat("InnerConeAngle", InInnerConeAngle);
}

void UDRSpotLightAdapter::SetOuterConeAngle(const float InOuterConeAngle)
{
	Obj->SetPropertyFloat("OuterConeAngle", InOuterConeAngle);
}

void UDRSpotLightAdapter::SetColorTemperature(const float InColorTemperature)
{
	Obj->SetPropertyFloat("ColorTemperature", InColorTemperature);
}

void UDRSpotLightAdapter::SetHighlight(const float InHighlight)
{
	Obj->SetPropertyFloat("Highlight", InHighlight);
}



