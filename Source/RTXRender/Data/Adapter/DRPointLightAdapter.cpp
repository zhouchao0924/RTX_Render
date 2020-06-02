// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRPointLightAdapter.h"

UDRPointLightAdapter::UDRPointLightAdapter(const FObjectInitializer& ObjectIntializer)
	:Super(ObjectIntializer)
{
}

/********************************Get Function*************************************/
FVector UDRPointLightAdapter::GetLocation()
{
	return ToVector(Obj->GetPropertyVector3D("Location"));
}

float UDRPointLightAdapter::GetSoftSourceRadius()
{
	return Obj->GetPropertyFloat("SoftSourceRadius");
}

float UDRPointLightAdapter::GetSourceRadius()
{
	return Obj->GetPropertyFloat("SourceRadius");
}

float UDRPointLightAdapter::GetSourceLength()
{
	return Obj->GetPropertyFloat("SourceLength");
}

float UDRPointLightAdapter::GetIntensity()
{
	return Obj->GetPropertyFloat("Intensity");
}

float UDRPointLightAdapter::GetShadowResolutionScale()
{
	return Obj->GetPropertyFloat("ShadowResolutionScale");
}

float UDRPointLightAdapter::GetAttenuationRadius()
{
	return Obj->GetPropertyFloat("AttenuationRadius");
}

float UDRPointLightAdapter::GetColorTemperature()
{
	return Obj->GetPropertyFloat("ColorTemperature");
}

float UDRPointLightAdapter::GetHighlight()
{
	return Obj->GetPropertyFloat("Highlight");
}

bool UDRPointLightAdapter::GetIsAutoPoint()
{
	return Obj->GetPropertyBool("IsAutoPoint");
}

/********************************Set Function*************************************/

void UDRPointLightAdapter::SetLocation(const FVector& InLocation)
{
	Obj->SetPropertyVector3D("Location", ToKVector3D(InLocation));
}

void UDRPointLightAdapter::SetSoftSourceRadius(const float InSoftSourceRadius)
{
	Obj->SetPropertyFloat("SoftSourceRadius", InSoftSourceRadius);
}

void UDRPointLightAdapter::SetSourceRadius(const float InSourceRadius)
{
	Obj->SetPropertyFloat("SourceRadius", InSourceRadius);
}

void UDRPointLightAdapter::SetSourceLength(const float InSourceLength)
{
	Obj->SetPropertyFloat("SourceLength", InSourceLength);
}

void UDRPointLightAdapter::SetIntensity(const float InIntensity)
{
	Obj->SetPropertyFloat("Intensity", InIntensity);
}

void UDRPointLightAdapter::SetShadowResolutionScale(const float InShadowResolutionScale)
{
	Obj->SetPropertyFloat("ShadowResolutionScale", InShadowResolutionScale);
}

void UDRPointLightAdapter::SetAttenuationRadius(const float InAttenuationRadius)
{
	Obj->SetPropertyFloat("AttenuationRadius", InAttenuationRadius);
}

void UDRPointLightAdapter::SetColorTemperature(const float InColorTemperature)
{
	Obj->SetPropertyFloat("ColorTemperature", InColorTemperature);
}

void UDRPointLightAdapter::SetHighlight(const float InHighlight)
{
	Obj->SetPropertyFloat("Highlight", InHighlight);
}

void UDRPointLightAdapter::SetIsAutoPoint(const bool bIsAutoPoint)
{
	Obj->SetPropertyBool("IsAutoPoint",bIsAutoPoint);
}