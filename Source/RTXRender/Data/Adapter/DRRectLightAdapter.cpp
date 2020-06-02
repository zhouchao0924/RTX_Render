// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DRRectLightAdapter.h"

/********************************Get Function*************************************/
FVector UDRRectLightAdapter::GetLocation()
{
	return ToVector(Obj->GetPropertyVector3D("Location"));
}
FRotator UDRRectLightAdapter::GetRotation()
{
	return ToRotator(Obj->GetPropertyRotator("Rotation"));
}
float UDRRectLightAdapter::GetIntensity()
{
	return Obj->GetPropertyFloat("Intensity");
}
float UDRRectLightAdapter::GetAttenuationRadius()
{
	return Obj->GetPropertyFloat("AttenuationRadius");
}
float UDRRectLightAdapter::GetSourceWidth()
{
	return Obj->GetPropertyFloat("SourceWidth");
}
float UDRRectLightAdapter::GetSourceHeight()
{
	return Obj->GetPropertyFloat("SourceHeight");
}
float UDRRectLightAdapter::GetBarnDoorAngle()
{
	return Obj->GetPropertyFloat("BarnDoorAngle");
}
float UDRRectLightAdapter::GetBarnDoorLength()
{
	return Obj->GetPropertyFloat("BarnDoorLength");
}
float UDRRectLightAdapter::GetColorTemperature()
{
	return Obj->GetPropertyFloat("ColorTemperature");
}

float UDRRectLightAdapter::GetHighlight()
{
	return Obj->GetPropertyFloat("Highlight");
}
/********************************Set Function*************************************/

void UDRRectLightAdapter::SetLocation(const FVector& InLocation)
{
	Obj->SetPropertyVector3D("Location", ToKVector3D(InLocation));
}

void UDRRectLightAdapter::SetRotation(const FRotator& InRotation)
{
	Obj->SetPropertyRotator("Rotation", ToKRotation(InRotation));
}

void UDRRectLightAdapter::SetIntensity(const float InIntensity)
{
	Obj->SetPropertyFloat("Intensity", InIntensity);
}
void UDRRectLightAdapter::SetAttenuationRadius(const float InAttenuationRadius)
{
	Obj->SetPropertyFloat("AttenuationRadius", InAttenuationRadius);
}
void UDRRectLightAdapter::SetSourceWidth(const float InSourceWidth)
{
	Obj->SetPropertyFloat("SourceWidth", InSourceWidth);
}

void UDRRectLightAdapter::SetSourceHeight(const float InSourceHeight)
{
	Obj->SetPropertyFloat("SourceHeight", InSourceHeight);
}

void UDRRectLightAdapter::SetBarnDoorAngle(const float InBarnDoorAngle)
{
	Obj->SetPropertyFloat("BarnDoorAngle", InBarnDoorAngle);
}

void UDRRectLightAdapter::SetBarnDoorLength(const float InBarnDoorLength)
{
	Obj->SetPropertyFloat("BarnDoorLength", InBarnDoorLength);
}

void UDRRectLightAdapter::SetColorTemperature(const float InColorTemperature)
{
	Obj->SetPropertyFloat("ColorTemperature", InColorTemperature);
}

void UDRRectLightAdapter::SetHighlight(const float InHighlight)
{
	Obj->SetPropertyFloat("Highlight", InHighlight);
}