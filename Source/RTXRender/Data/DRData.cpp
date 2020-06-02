

#include "DRData.h"

UDRData::UDRData(const FObjectInitializer &ObjectIntializer)
	:Super(ObjectIntializer)
{
}

void UDRData::SerializeData(ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		int32 Ver = GetVersion();
		Ar << Ver;
		SerializeDataByVersion(Ar, Ver);
	}
	else if(Ar.IsLoading())
	{
		int32 Ver = 0;
		Ar << Ver;
		SerializeDataByVersion(Ar, Ver);
	}
}

FLinearColor UDRData::ToLinearColor(kColor Col)
{
	return FLinearColor(Col.R / 255.0f, Col.G / 255.0f, Col.B / 255.0f, Col.A / 255.0f);
}

FVector UDRData::ToVector(const kVector3D &V)
{
	return FORCE_TYPE(FVector, V);
}

FRotator UDRData::ToRotator(const kRotation &V)
{
	return FORCE_TYPE(FRotator, V);
}

kColor UDRData::ToKColor(const FLinearColor &Col)
{
	int A = FMath::FloorToInt(Col.A * 255.999f);
	int R = FMath::FloorToInt(Col.R * 255.999f);
	int G = FMath::FloorToInt(Col.G * 255.999f);
	int B = FMath::FloorToInt(Col.B * 255.999f);
	return kColor(R, G, B, A);
}

FVector2D UDRData::ToVector2D(const kPoint &V)
{
	return FORCE_TYPE(FVector2D, V);
}

kPoint UDRData::ToKVector2D(const FVector2D &V)
{
	return FORCE_TYPE(kTexUV, V);
}

kVector3D UDRData::ToKVector3D(const FVector &V)
{
	return FORCE_TYPE(kVector3D, V);
}

kRotation UDRData::ToKRotation(const FRotator &R)
{
	return FORCE_TYPE(kRotation, R);
}

FTransform UDRData::ToTransform(const kXform &T)
{
	FVector location = ToVector(T.Location);
	FVector scale = ToVector(T.Scale);
	FRotator rot = ToRotator(T.Rotation);
	return FTransform(rot.Quaternion(), location, scale);
}
