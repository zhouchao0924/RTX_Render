

#include "DRPostProcessAdapter.h"

UDRPostProcessAdapter::UDRPostProcessAdapter(const FObjectInitializer &ObjectIntializer)
	:Super(ObjectIntializer)
{
}

float UDRPostProcessAdapter::GetSaturation()
{
	return Obj->GetPropertyFloat("Saturation");
}

float UDRPostProcessAdapter::GetConstrast()
{
	return Obj->GetPropertyFloat("Constrast");
}

float UDRPostProcessAdapter::GetBloomIntensity()
{
	return Obj->GetPropertyFloat("BloomIntensity");
}

float UDRPostProcessAdapter::GetAmbientOcclusion()
{
	return Obj->GetPropertyFloat("AmbientOcclusion");
}

float UDRPostProcessAdapter::GetAmbientOcclusionRadius()
{
	return Obj->GetPropertyFloat("AmbientOcclusionRadius");
}

void UDRPostProcessAdapter::SetSaturation(float Value)
{
	Obj->SetPropertyFloat("Saturation", Value);
}

void UDRPostProcessAdapter::SetConstrast(float Value)
{
	Obj->SetPropertyFloat("Constrast", Value);
}

void UDRPostProcessAdapter::SetBloomIntensity(float Value)
{
	Obj->SetPropertyFloat("BloomIntensity", Value);
}

void UDRPostProcessAdapter::SetAmbientOcclusion(float Value)
{
	Obj->SetPropertyFloat("AmbientOcclusion", Value);
}

void UDRPostProcessAdapter::SetAmbientOcclusionRadius(float Value)
{
	Obj->SetPropertyFloat("AmbientOcclusionRadius", Value);
}




