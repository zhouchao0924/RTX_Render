
#pragma once

#include "Math/Box.h"
#include "Math/kBox.h"
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "Math/kColor.h"

#define ColorIntMax 255
#define ColorFloatMax 255.0

FVector2D ToVector2D(const kPoint &pt);
FVector2D ToUE4Vector2D(const kPoint &pt);
FVector   ToUE4Vector(const kVector3D &Loc);
FVector   ToUE4Position(const kVector3D &Loc);
FRotator  ToUE4Rotation(const kRotation &Rotation);
FBox	  ToUE4Bounds(const kBox3D &Bounds);
float	  ToUE4Len(float SuiteLen);
FPlane	  ToUE4Plane(const kPlane3D &Plane);
FLinearColor	ToUE4LinearColor(const kColor &Color);
FVector4		ToUE4Vector4(const kVector4D& V);


kVector3D ToBuildingPosition(const FVector &V);
kPoint	  ToBuildingPosition(const FVector2D &V);
kVector3D ToBuildingVector(const FVector &V);
kPoint	  ToBuildingVector(const FVector2D &V);
kRotation ToBuildingRotation(const FRotator &Rotation);
kBox3D	  ToBuildingBox(const FBox &V);
float	  ToBuildingLen(float UE4Len);
kColor	  ToBuildingColor(const FLinearColor& Color);
kVector4D ToBuildingVector4D(const FVector4& V);
const char* FStringToConstChar(FString V);



