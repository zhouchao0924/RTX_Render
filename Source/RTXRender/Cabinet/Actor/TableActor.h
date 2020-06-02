#pragma once
#include "../base/common.h"
#include "TriangulatedMeshComponent.h"
#include "TableActor.generated.h"

UCLASS()
class ATableActor : public AActor
{
	GENERATED_BODY()
public:
	ATableActor();
	~ATableActor();
	bool Init(TArray<FVector2D>& PathFrame,TArray<TPath2d>& holes, float Thickness,FString Mx,float fHeight);
	bool SetMaterial(FString mx);
	FString GetMx() { return mMx; };
protected:
	UPROPERTY(Transient)
		class UTriangulatedMeshComponent *mModel;
protected:
	bool Refresh();
	TArray<FVector2D> mPathFrameRaw;
	TArray<TPath2d> mholes;
	float mThickness;
	FString mMx;
	float mHeight;
};