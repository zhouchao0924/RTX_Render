
#include "TableActor.h"
#include "BuildingGroupMeshComponent.h"
#include "ProceduralMeshComponent.h"

ATableActor::ATableActor()
{
	mModel = CreateDefaultSubobject<UTriangulatedMeshComponent>(TEXT("CabinetTable"));
	RootComponent = mModel;
}
ATableActor::~ATableActor()
{
}
bool ATableActor::Init(TArray<FVector2D>& PathFrameRaw, TArray<TPath2d>& holes, float Thickness, FString Mx, float fHeight)
{
	mPathFrameRaw.Empty();
	for (auto p : PathFrameRaw)
	{
		FVector2D pos = p / 10;
		mPathFrameRaw.Add(pos);
	}
	mholes.Empty();
	for (auto hole : holes)
	{
		TPath2d	holecm;
		for (auto p : hole.mOutLine)
		{
			FVector2D pos = p / 10;
			holecm.mOutLine.Add(pos);
		}
		mholes.Add(holecm);
	}
	mThickness = Thickness;
	mMx = Mx;
	mHeight = fHeight;
	Refresh();
	return true;
}

bool ATableActor::Refresh()
{
	mModel->CreatePolygon(mPathFrameRaw, mThickness*0.1f);
	for (auto hole : mholes)
	{
		mModel->AddHole(hole.mOutLine);
	}
	SetActorScale3D(FVector(1, 1, 1.0f));
	SetActorLocation(FVector(0.0f, 0.0f, mHeight*0.1f));
	SetMaterial(mMx);
	return true;
}
bool ATableActor::SetMaterial(FString mx)
{
	mMx = mx;
	mModel->SetSurface(mMx);
	//mModel->MeshComponents
	
	//TArray<UProceduralMeshComponent*> BuildingComponents;
	//mModel->GetChildrenComponents(true, BuildingComponents);

	//for (auto p : BuildingComponents)
	//{
	//		p->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}
	return true;
}
