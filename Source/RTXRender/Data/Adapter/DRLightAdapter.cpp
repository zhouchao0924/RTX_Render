

#include "DRLightAdapter.h"

UDRLightAdapter::UDRLightAdapter(const FObjectInitializer &ObjectIntializer)
	:Super(ObjectIntializer)
{
}

/********************************Get Function*************************************/
ELightType UDRLightAdapter::GetLightType()
{
	return (ELightType)Obj->GetInt("LightType");
}

FLinearColor UDRLightAdapter::GetLightColor()
{
	return ToLinearColor(Obj->GetPropertyColor("LightColor"));
}

bool UDRLightAdapter::GetIsCastShadow()
{
	return Obj->GetPropertyBool("IsCastShadow");
}

bool UDRLightAdapter::GetIsVisible()
{
	return Obj->GetPropertyBool("IsVisible");
}

/********************************Set Function*************************************/

void UDRLightAdapter::SetLightType(const ELightType InLightType)
{
	Obj->SetInt("LightType", (int32)InLightType);
}

void UDRLightAdapter::SetLightColor(const FLinearColor &Color)
{
	Obj->SetPropertyColor("LightColor", ToKColor(Color));
}

void UDRLightAdapter::SetIsCastShadow(const bool Value)
{
	Obj->SetPropertyBool("IsCastShadow", Value);
}

void UDRLightAdapter::SetIsVisible(const bool Value)
{
	Obj->SetPropertyBool("IsVisible", Value);
}


