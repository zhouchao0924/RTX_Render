
#include "CabinetNode.h"
#include "Building/BuildingSystem.h"
#include "ResourceMgr.h"

ACabinetNode::ACabinetNode(const FObjectInitializer &ObjectInitializer):Super(ObjectInitializer)
{
	mModel = ObjectInitializer.CreateDefaultSubobject<UModelFileComponent>(this, TEXT("CabinetNodeRoot"));
}

bool ACabinetNode::Init(TRectBase CabinetSet)
{
	UResourceMgr* gResMgr = UResourceMgr::GetResourceMgr();
	UResource* pSource = gResMgr->FindRes("3983B9BA4647A1D11D58EFB8330D6FDE", true);
	CHECK_ERROR(pSource);
	pSource->ForceLoad();

	UModelFile* pModelFile = Cast<UModelFile>(pSource);
	CHECK_ERROR(pModelFile);
	mModel->UpdateModel(pModelFile);

	FVector Pos = FVector(CabinetSet.mPos.X, CabinetSet.mPos.Y, CabinetSet.mHeight)*0.1f;
	SetActorLocation(Pos);

	float fAngle = atan2(CabinetSet.mDir.Y, CabinetSet.mDir.X);
	FRotator r(0, fAngle * 180 / PI, 0);
	SetActorRotation(r);

	SetActorScale3D(FVector(CabinetSet.mScale.X, CabinetSet.mScale.Y, 1.0f));

	return true;
}