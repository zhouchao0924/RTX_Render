
#include "Building.h"

FVector2D ToVector2D(const kPoint &pt) 
{ 
	return FVector2D(pt.X, pt.Y); 
}

FVector2D ToUE4Vector2D(const kPoint &pt)
{
	return FVector2D(pt.X, pt.Y);
}

FVector ToUE4Vector(const kVector3D &V)
{
	return FVector(V.x, V.y, V.z);
}

FVector ToUE4Position(const kVector3D &V)
{
	return FVector(V.x, V.y, V.z);
}

FRotator ToUE4Rotation(const kRotation &V)
{
	return FRotator(V.Pitch, V.Yaw, V.Roll);
}

FBox ToUE4Bounds(const kBox3D &V)
{
	return FBox(ToUE4Position(V.MinEdge), ToUE4Position(V.MaxEdge));
}

FPlane ToUE4Plane(const kPlane3D &Plane)
{
	return FPlane(ToUE4Position(Plane.Normal), Plane.D);
}

FLinearColor ToUE4LinearColor(const kColor &Color)
{
	return FLinearColor((Color.R / ColorFloatMax), (Color.G / ColorFloatMax), (Color.B / ColorFloatMax), (Color.A / ColorFloatMax));
}

FVector4 ToUE4Vector4(const kVector4D& V)
{
	return FVector4(V.x, V.y, V.z, V.w);
}

kVector3D ToBuildingPosition(const FVector &V)
{
	return kVector3D(V.X, V.Y, V.Z);
}

kPoint	ToBuildingPosition(const FVector2D &V)
{
	return kPoint(V.X, V.Y);
}

kVector3D ToBuildingVector(const FVector &V)
{
	return kVector3D(V.X, V.Y, V.Z);
}

kPoint	ToBuildingVector(const FVector2D &V)
{
	return kPoint(V.X, V.Y);
}

kRotation ToBuildingRotation(const FRotator &Rotation)
{
	//kRotation Pitch = Yaw = Roll;
	//ue4 roll,x,, pitch,y ,yaw,z
	return kRotation(Rotation.Yaw, Rotation.Pitch, Rotation.Roll);
}

kBox3D ToBuildingBox(const FBox &V)
{
	return kBox3D(ToBuildingPosition(V.Min), ToBuildingPosition(V.Max));
}

float ToBuildingLen(float UE4Len)
{
	return UE4Len / 100.0f;
}

kColor ToBuildingColor(const FLinearColor& Color)
{
	 return kColor((unsigned char)(Color.R * ColorFloatMax), (unsigned char)(Color.G * ColorFloatMax), (unsigned char)(Color.B * ColorFloatMax), (unsigned char)(Color.A * ColorFloatMax));
}

kVector4D ToBuildingVector4D(const FVector4& V)
{
	return kVector4D(V.X, V.Y, V.Z, V.W);
}

const char* FStringToConstChar(FString V)
{
	 return TCHAR_TO_ANSI(*V);
}

float ToUE4Len(float SuiteLen)
{
	return SuiteLen * 100.0f;
}





