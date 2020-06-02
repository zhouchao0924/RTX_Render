// Fill out your copyright notice in the Description page of Project Settings.

#include "CeilWrapper.h"
#include "../EditorGameInstance.h"
#include "../Model/ResourceMgr.h"
#include "BuildingSystem.h"
#include "GeometryFunctionLibrary.h"
#pragma optimize( "", off )

// Sets default values
ACeilWrapper::ACeilWrapper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//
	mpTopLayer = nullptr;
	mfTopLayerSize = 0.f;
	mfTopLayerSizeBK = 0.f;
	mfDelta = 0.f;
	mbDirty = false;
	bool bUseDefaultTemplate = false;
	float fDefaultTemplateScale = 1.f;
	mpDefMate = UMaterial::GetDefaultMaterial(MD_Surface);
	//mpDefMate->BaseColor.Constant = FColor(240, 4, 4);

	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateTest"), NULL, LOAD_None, NULL);
	//mvMate.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateWire"), NULL, LOAD_None, NULL);
	//mvMate.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateBlend"), NULL, LOAD_None, NULL);
	//mvMate.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateDouble"), NULL, LOAD_None, NULL);
	//mvMate.Add(pMate);
	////
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateRed"), NULL, LOAD_None, NULL);
	//mvMateColor.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateGreen"), NULL, LOAD_None, NULL);
	//mvMateColor.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateBlue"), NULL, LOAD_None, NULL);
	//mvMateColor.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateYellow"), NULL, LOAD_None, NULL);
	//mvMateColor.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MatePink"), NULL, LOAD_None, NULL);
	//mvMateColor.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateGray"), NULL, LOAD_None, NULL);
	//mvMateColor.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateCyan"), NULL, LOAD_None, NULL);
	//mvMateColor.Add(pMate);
	//pMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateGreenDeep"), NULL, LOAD_None, NULL);
	//mvMateColor.Add(pMate);

}

// Called when the game starts or when spawned
void ACeilWrapper::BeginPlay()
{
	Super::BeginPlay();

	if(nullptr == RootComponent)
		RootComponent = NewObject<USceneComponent>(this, "CeilWrapperRoot", RF_NoFlags, nullptr, false, nullptr);
	RootComponent->Mobility = EComponentMobility::Movable;

	//ChildActorComponent = NewObject<UChildActorComponent>(this, "name", RF_NoFlags, nullptr, false, nullptr);
	//AddInstanceComponent(RootComponent);
	//AddInstanceComponent(ChildActorComponent);
	
}

// Called every frame
void ACeilWrapper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if (mbDirty)
	//{
	//	Clear(true, false);
	//	CreateMeshEx();
	//	mbDirty = false;
	//}
}

TArray<FVector2D> ACeilWrapper::GetPathFromRoomId(int32 uRoomID) const
{
	TArray<FVector2D> vPolygon;
	UGameInstance* pInst = GetGameInstance();
	if (nullptr != pInst)
	{
		UBuildingSystem* pBuild = ((UCEditorGameInstance*)pInst)->GetBuildingSystemInstance();
		if (nullptr != pBuild)
		{
			int32 num = pBuild->GetPolygon(uRoomID, vPolygon, true);
		}
	}
	return vPolygon;
}

bool ACeilWrapper::GetBoundary(FString const& strMXFileName, TArray<FVector> & vBoundary) const
{
	vBoundary.Empty();
	TArray<FVector2D> tmp;
	bool bOk = UGeometryFunctionLibrary::GetBoundary(strMXFileName, tmp);
	if (bOk)
	{
		for (int i = 0; i < tmp.Num(); i++)
		{
			vBoundary.Add(FVector(-tmp[i].X, -tmp[i].Y, 0.f));
		}
	}
	return bOk;
}

bool ACeilWrapper::SetElementRes(int uID, FString const& strResName, UMaterialInterface* pMaterial /* = nullptr */, bool bClearAllBeforeSet /* = false */)
{
	if (bClearAllBeforeSet)
		mvEleRes.Empty();
	//
	if (uID >= 0 && uID < CeilPrimData::ElementMax && !strResName.IsEmpty())
	{
		UModelFile* pModelFile = Cast<UModelFile>(UResourceMgr::GetResourceMgr()->FindRes(strResName));
		if (nullptr == pModelFile)
			return false;
		//
		ElementRes eleRes;
		eleRes.id = uID;
		if (nullptr != pMaterial)
			eleRes.vMaterial.Add(pMaterial);
		else
		{
			pModelFile->ForceLoad();
			UMaterialInstanceDynamic *pMateInst = pModelFile->GetUE4Material(0);
			if (nullptr != pMateInst)
			{
				eleRes.vMaterial.Add(pMateInst);
			}
		}
		if (GetBoundary(pModelFile->Filename, eleRes.vBoundary))
			mvEleRes.Add(eleRes);
		else
			return false;
		//
		if (0 == eleRes.id)
			mpTopLayerMate = pMaterial;
	}
	return true;
}

bool ACeilWrapper::SetPath(TArray<FVector2D> const& vPath, bool bClose /* = true */, bool bUseDefaultTemplate /* = false */, float fDefaultTemplateScale /* = 1.f */)
{
	UWorld* pWorld = GetWorld();
	if (nullptr == pWorld)
		return false;
	Clear(false, false);
	if (vPath.Num() > 1)
		mvPath = vPath;
	else
		return false;
	//
	if (bClose && mvPath.Num() > 2)
	{
		FVector2D startPoint(mvPath[0]);
		mvPath.Add(startPoint);
	}
	//
	mbUseDefaultTemplate = bUseDefaultTemplate;
	mfDefaultTemplateScale = fDefaultTemplateScale;
	//
	for (int i = 0; i < mvPath.Num() - 1; i++)
	{
		UCeilTemplate* pActor = CreatTemp(pWorld);
		if (nullptr != pActor)
		{
			if (bUseDefaultTemplate)
				pActor->UseDefaultTemplate(mfTopLayerSize, fDefaultTemplateScale);
			else
				InitTemplateData(pActor, ((0 == i) ? &mfTopLayerSize : nullptr));
			mvCeil.Add(pActor);
		}
		else
			return false;
	}
	//
	CreateMeshEx();
	//
	return true;
}

bool ACeilWrapper::InitTemplateData(UCeilTemplate* pActor, float* fTopLayerSize /* = nullptr */)
{
	if (nullptr == pActor || mvEleRes.Num() <= 0)
		return false;
	UCeilTemplate* pTemp = (UCeilTemplate*)pActor;
	pTemp->ClearTemplateData();
	for (int i = 0; i < mvEleRes.Num(); i++)
	{
		pTemp->AddTemplateData(mvEleRes[i].id, mvEleRes[i].vBoundary, mvEleRes[i].vMaterial);
		//
		if (0 == mvEleRes[i].id && nullptr != fTopLayerSize)
		{
			FVector v0, v1;
			if (CeilPrimData::CalcuBox(v0, v1, mvEleRes[i].vBoundary))
			{
				const FVector vTmp = v1 - v0;
				*fTopLayerSize = vTmp.Y;
				mfTopLayerSizeBK = *fTopLayerSize;
			}
		}
	}
	return true;
}

void ACeilWrapper::Clear(bool bOnlyRedraw /* = true */, bool bClearEleRes /* = false */)
{
	for (int i = 0; i < mvCeil.Num(); i++)
	{
		if (nullptr != mvCeil[i])
		{
			mvCeil[i]->ClearMesh();
			mvCeil[i]->DestroyComponent(true);
			mvCeil[i] = nullptr;
		}
	}
	if (!bOnlyRedraw)
	{
		mvCeil.Empty();
		mvPath.Empty();
	}
	//
	if (bClearEleRes)
	{
		mvEleRes.Empty();
		mpTopLayerMate = nullptr;
		mfTopLayerSize = 0.f;
	}
	//
	if (nullptr != mpTopLayer)
	{
		mpTopLayer->SetVisibility(false, true);
		mpTopLayer->DestroyComponent(true);
		mpTopLayer = nullptr;
		mfTopLayerSize = 0.f;
	}
	
}

//bool ACeilWrapper::CreateMeshBK()
//{
//	UWorld* pWorld = GetWorld();
//	if (nullptr == pWorld || mvCeil.Num() <= 0)
//		return false;
//	//
//	//for (int i = 0; i < mvCeil.Num() && i + 1 < mvPath.Num(); i++)
//	//{
//	//	const FVector v0(mvPath[i].X, mvPath[i].Y, 0.f);
//	//	const FVector v1(mvPath[i + 1].X, mvPath[i + 1].Y, 0.f);
//	//	FVector dir(v1 - v0);
//	//	if (!dir.Normalize())
//	//		continue;
//	//	const FVector vPos(v0.X, v0.Y, fHeight);
//	//	const float len = (b3DLoft ? FVector::Dist(v0, v1) : 0.f);
//	//	mvCeil[i]->CreateMesh(b3DLoft, len);
//	//	mvCeil[i]->TransformElement(vPos + dir * len, dir);
//	//	mvCeil[i]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
//	//}
//
//	for (int i = 0; i < mvEleRes.Num(); i++)
//	{
//		UGameInstance* pInst = GetGameInstance();
//		if (nullptr != pInst)
//		{
//			CeilElement* pCeil = mvCeil[0]->GetElement(i);
//			UBuildingSystem* pBuild = ((UCEditorGameInstance*)pInst)->GetBuildingSystemInstance();
//			if (nullptr != pBuild)
//			{
//				//const FVector v0(mvPath[0].X, mvPath[0].Y, 0.f) + pCeil->pos;
//				//FVector vLoc = FVector(mvPath[0].X, mvPath[0].Y, 0.f) + pCeil->pos;
//				FVector vLoc(0.f, pCeil->pos.X, pCeil->pos.Y);
//				FRotator rota;
//				FVector size(1.f, 1.f, 1.f);
//				int32 kid = pBuild->AddStretchMesh(mvEleRes[i].strFilename, mvPathEx, vLoc, rota, size);
//			}
//		}
//		//int32 AddStretchMesh(const FString &Filename, const TArray<FVector> &Path, const FVector &Location, const FRotator &Rotation, const FVector &Size);
//		//int32 kid = AddStretchMesh(const FString &Filename, const TArray<FVector> &Path, const FVector &Location, const FRotator &Rotation, const FVector &Size);
//
//	}
//	return true;
//}

bool ACeilWrapper::CreateMesh(bool b3DLoft /* = true */, float fHeight /* = 0.f */)
{
	return true;
}

bool ACeilWrapper::CreateMeshEx(bool b3DLoft /* = true */, float fHeight /* = 0.f */)
{
	UWorld* pWorld = GetWorld();
	if (nullptr == pWorld || mvCeil.Num() <= 0)
		return false;
	//
	for (int i = 0; i < mvCeil.Num() && i + 1 < mvPath.Num(); i++)
	{
		const FVector v0(mvPath[i].X, mvPath[i].Y, 0.f);
		const FVector v1(mvPath[i + 1].X, mvPath[i + 1].Y, 0.f);
		FVector dir(v1 - v0);
		if (!dir.Normalize())
			continue;
		const FVector vPos(v0.X, v0.Y, fHeight);
		const float len = (b3DLoft? FVector::Dist(v0, v1) : 0.f);
		mvCeil[i]->CreateMesh(b3DLoft, len);
		mvCeil[i]->TransformElement(vPos + dir * len, dir);
		mvCeil[i]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		//
		if (0 == i)// && mfTopLayerSize > 0.f)
		{
			if (mfTopLayerSize <= 0.f)
				mfTopLayerSize = mfTopLayerSizeBK;
			CreateTopLayer(pWorld, mpTopLayerMate, mfTopLayerSize);
		}
	}
	mbDirty = false;
	return true;
}

//bool ACeilWrapper::CreateSurface()
//{
//	CreateMesh(false);
//}

bool ACeilWrapper::ShowElement(bool bShow, int uEdgeID /* = -1 */, int uID /* = -1 */)
{
	bool bOk = false;
	if (uEdgeID >= 0 && uEdgeID < mvCeil.Num())
	{
		if (nullptr != mvCeil[uEdgeID])
		{
			mvCeil[uEdgeID]->ShowElement(bShow, uID);
			bOk = true;
		}
	}
	else if (uEdgeID < 0)
	{
		for (int i = 0; i < mvCeil.Num(); i++)
		{
			if (nullptr != mvCeil[i])
			{
				mvCeil[i]->ShowElement(bShow, uID);
				bOk = true;
			}
		}
	}
	if ((0 == uEdgeID || uEdgeID < 0) && nullptr != mpTopLayer && bShow != mpTopLayer->bVisible)
	{
		mpTopLayer->SetVisibility(bShow, true);
		//
		//if (bShow)
		//	mfTopLayerSize = mfTopLayerSizeBK;
		//else //if (!bShow)//(nullptr != mpTopLayer)
		//{
		//	mpTopLayer->DestroyComponent(true);
		//	mpTopLayer = nullptr;
		//	mfTopLayerSize = 0.f;
		//}
	}
	mbDirty = true;
	return bOk;
}

bool ACeilWrapper::UpdateElement(int uEdgeID, int id, float XSize, float YSize)
{
	bool bOk = false;
	UCeilTemplate* ptr = Cast<UCeilTemplate>(GetCeilActor(uEdgeID));
	if (nullptr != ptr)
	{
		bOk = ptr->UpdateTemplateData(id, XSize, YSize);
	}
	mbDirty = true;
	return bOk;
}

UActorComponent* ACeilWrapper::GetCeilActor(int uEdgeID) const
{
	if (uEdgeID >= 0 && uEdgeID < mvCeil.Num())
	{
		return mvCeil[uEdgeID];
	}
	//if (muTopLayerID == uEdgeID)
	//	return mpTopLayer;
	return nullptr;
}

int ACeilWrapper::GetCeilActorID(UActorComponent* pActor) const
{
	if (nullptr == pActor || mvCeil.Num() <= 0)
		return -1;
	if (pActor == mpTopLayer)
		return muTopLayerID;
	int uEdgeID = -1;
	for (int i = 0; i < mvCeil.Num(); i++)
	{
		if (pActor == mvCeil[i])
		{
			uEdgeID = i;
			break;
		}
	}
	return uEdgeID;
}

UActorComponent* ACeilWrapper::GetElementActor(int uEdgeID, int uID, int uType /* = 0 */) const
{
	if (uEdgeID >= 0 && uEdgeID < mvCeil.Num())
	{
		return mvCeil[uEdgeID]->GetElementActor(uID, uType);
	}
	return nullptr;
}

bool ACeilWrapper::GetElementSize(int uEdgeID, int uID, FVector2D & Size) const
{
	if (uEdgeID >= 0 && uEdgeID < mvCeil.Num())
	{
		return mvCeil[uEdgeID]->GetElementSize(uID, Size);
	}
	return false;
}

FVector ACeilWrapper::GetLampPosition(int uEdgeID) const
{
	FVector vPos;
	UCeilTemplate* ptr = Cast<UCeilTemplate>(GetCeilActor(uEdgeID));
	if (nullptr != ptr)
		vPos = ptr->LampPosition();
	return vPos;
}

UCeilTemplate* ACeilWrapper::CreatTemp(UWorld* pWorld)//(FName const& name)
{
	static uint32 num = 0;
	char sz[256];
	sprintf(sz, "CeilTemp_%d", num++);
	FName tmp(sz);
	UCeilTemplate* pEleComp = NewObject<UCeilTemplate>(this, tmp, RF_NoFlags, nullptr, false, nullptr);
	if (nullptr != pEleComp)
	{
		pEleComp->Mobility = EComponentMobility::Movable;
		pEleComp->SetVisibility(true);
		pEleComp->RegisterTemp(pWorld);
	}
	return pEleComp;
}

void ACeilWrapper::CreateTopLayer(UWorld* pWorld, UMaterialInterface* pMaterial, float fSize)
{
	if (nullptr == pWorld)
		return;
	if (nullptr != mpTopLayer)
	{
		mpTopLayer->SetVisibility(false, true);
		mpTopLayer->DestroyComponent(true);
		mpTopLayer = nullptr;
	}
	static uint32 num = 0;
	char sz[256];
	sprintf(sz, "CeilTopLayer_%d", num++);
	FName tmp(sz);
	UDynaMesh* pEleComp = NewObject<UDynaMesh>(this, tmp, RF_NoFlags, nullptr, false, nullptr);
	if (nullptr != pEleComp)
	{
		TArray<FVector> vPath;
		for (int i = 0; i < mvPath.Num(); i++)
		{
			if(i < mvPath.Num() - 1)
				vPath.Add(FVector(mvPath[i].X, mvPath[i].Y, 0.f));
			else if(mvPath[i] != mvPath[0])
				vPath.Add(FVector(mvPath[i].X, mvPath[i].Y, 0.f));
		}
		pEleComp->Mobility = EComponentMobility::Movable;
		pEleComp->SetVisibility(true);
		pEleComp->RegisterElem(pWorld);
		pEleComp->CreateTopLayer(vPath, pMaterial, fSize, true);
		//
		CeilElement* ele = mvCeil[0]->GetElement(0);
		//pEleComp->SetRelativeScale3D(ele->scale);
		//pEleComp->SetRelativeRotation(ele->quat);
		pEleComp->SetRelativeLocation(ele->pos);
		//
		////void UCeilTemplate::TransformElement(FVector const& vPos, FVector const& vDir)
		//{
		//	const FVector v0(mvPath[0].X, mvPath[0].Y, 0.f);
		//	const FVector v1(mvPath[1].X, mvPath[1].Y, 0.f);
		//	FVector dir(v1 - v0);
		//	if (dir.Normalize())
		//	{
		//		const FVector vPos(v0.X, v0.Y, 0.f);// fHeight);
		//		//FVector vPos(mvPath[0].X, mvPath[0].Y, 0.f);
		//		//FVector dir(vDir);
		//		//dir.Z = 0.f;
		//		//if (dir.Normalize())
		//		{
		//			const FVector vOri(0.f, 1.f, 0.f); //(1.f, 0.f, 0.f);
		//			const FVector vNor = FVector(1.f, 0.f, 0.f);// FVector(0.f, 0.f, 1.f);
		//			FQuat quat = FQuat::FindBetweenNormals(vOri, dir);
		//			quat *= FQuat(vNor, 1.5707f);// 3.1415926f);
		//			quat *= FQuat(vNor, 1.5707f);
		//			quat *= FQuat(FVector(0.f, 0.f, 1.f), 1.5707f);
		//			//pEleComp->SetRelativeRotation(quat);
		//			//pEleComp->SetRelativeLocation(vPos);
		//		}
		//	}
		//}
		//
		pEleComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	mpTopLayer = pEleComp;
}




#pragma optimize( "", on )