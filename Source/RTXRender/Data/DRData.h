
#pragma once

#include "DRChunkID.h"
#include "ISerialize.h"
#include "WLink.h"
#include "DRSerializer.h"
#include "DRData.generated.h"

UCLASS(BlueprintType)
class UDRData :public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual int32 GetVersion() { return 0; }
	virtual int32 GetChunkID() { return 0; }
	void SerializeData(ISerialize &Ar);
public:
	static 	FLinearColor ToLinearColor(kColor Col);
	static 	kColor ToKColor(const FLinearColor &Col);
	static  FVector ToVector(const kVector3D &V);
	static  FRotator ToRotator(const kRotation &V);
	static  FVector2D ToVector2D(const kPoint &V);
	static  kPoint ToKVector2D(const FVector2D &V);
	static  kVector3D ToKVector3D(const FVector &V);
	static  kRotation ToKRotation(const FRotator &R);
	static  FTransform ToTransform(const kXform &T);
protected:
	friend class UDRProjData;
	virtual void SerializeDataByVersion(ISerialize &Ar, int32 Ver) { }
protected:
	UPROPERTY(Transient)
	class UDRProjData *ProjData;
};


