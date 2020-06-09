

#include "DRCurtainData.h"

UDRCurtainData::UDRCurtainData(const FObjectInitializer &ObjectIntializer)
	:Super(ObjectIntializer)
{
}

void UDRCurtainData::SerializeDataByVersion(ISerialize &Ar, int32 Ver)
{
	Ar << WorldTransform;
	Ar << Romebar1;
	Ar << Cloth1;
	Ar << Romebar2;
	Ar << Cloth2;
	Ar << Length;
	Ar << Height;
	Ar << RomerbarHeadLength;
	Ar << RomerbarBodyLength;
	Ar << DistanceBetweenBar;
	Ar << IsBlind;
	Ar << BlindLength;
	Ar << ClothPosition;
}




