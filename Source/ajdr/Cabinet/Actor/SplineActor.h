#pragma once
#include "Building/StretchMesh/StretchPathComponent.h"
#include "../base/common.h"
#include "SplineActor.generated.h"

UCLASS()
class ASplineActor : public AActor
{
	GENERATED_BODY()
public:
	ASplineActor();
	bool Init(TArray<FVector2D>& Paths, bool bReversal, FString MX, float fHeight, eSplineType eType);
	int32	mId;
	bool SetMaterial(FString MX);
	bool SetMesh(FString Mx);
	bool SetPosZ(float PosZ);
	eSplineType mType;
	FString GetMx() { return mMaterial; };
protected:
	UPROPERTY(Transient)
	UStretchPathComponent *mModel;
	TArray<FVector2D> mPaths;
	float  mHeight;
	FString mMx;
	FString	mMaterial;
protected:
	bool UpdateSpline();
};
