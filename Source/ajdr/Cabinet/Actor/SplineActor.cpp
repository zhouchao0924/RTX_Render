
#include "SplineActor.h"
#include "../base/common.h"
#include "BuildingGroupMeshComponent.h"

ASplineActor::ASplineActor()
{
	mModel = CreateDefaultSubobject<UStretchPathComponent>(TEXT("CabinetSpline"));
	RootComponent = mModel;
}

bool ASplineActor::Init(TArray<FVector2D>& Paths,bool bReversal, FString MX, float fHeight, eSplineType eType)
{
	CHECK_ERROR(Paths.Num() >= 2);
	if ( bReversal )
	{
		for (int i = Paths.Num() - 1; i >= 0; --i)
		{
			mPaths.Add(Paths[i]);
		}
	}
	else
	{
		mPaths = Paths;
	}
	mMx = MX;
	mHeight = fHeight;
	mType = eType;
	return UpdateSpline();
}
bool ASplineActor::SetMaterial(FString MX)
{
	CHECK_ERROR(!MX.IsEmpty());
	mMaterial = MX;
	mModel->SetSurface(9, mMaterial, 0);

	TArray<USceneComponent*> BuildingComponents;
	mModel->GetChildrenComponents(true, BuildingComponents);
	for (auto p : BuildingComponents)
	{
		UProceduralMeshComponent *Comp1 = Cast<UProceduralMeshComponent>(p);
		if (Comp1)
			Comp1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	return true;
}
bool ASplineActor::SetMesh(FString MX)
{
	CHECK_ERROR(!MX.IsEmpty());
	mMx = MX;
	UpdateSpline();
	SetMaterial(mMaterial);
	return false;
}
bool ASplineActor::SetPosZ(float PosZ)
{
	mHeight = PosZ;
	SetActorLocation(FVector(0.0f, 0.0f, mHeight*0.1f));
	return true;
}
bool ASplineActor::UpdateSpline()
{
	CHECK_ERROR(mPaths.Num() >= 2);

	FStretchShapeSlot slot;
	slot.TypeId = 9;
	slot.MX = mMx;
	TArray<FStretchShapeSlot> Slots;
	Slots.Add(slot);

	TArray<FVector> Path3d;
	for (auto p : mPaths)
	{
		FVector v(p.X*0.1f, p.Y*0.1f, 0.0f);
		Path3d.Add(v);
	}
	mModel->Update(Slots, Path3d);
	SetActorLocation(FVector(0.0f, 0.0f, mHeight*0.1f));
	return true;
}