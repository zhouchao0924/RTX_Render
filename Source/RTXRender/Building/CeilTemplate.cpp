// Fill out your copyright notice in the Description page of Project Settings.

#include "CeilTemplate.h"
#include "../EditorGameInstance.h"
//#include "KismetProceduralMeshLibrary.h"
#include "BuildingSystem.h"
#include "GeometryFunctionLibrary.h"
#pragma optimize( "", off )

// Sets default values
UCeilTemplate::UCeilTemplate()
{
 	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//
	mbUpdate = false;
	mfLoftSize = 1.f;
	mfLineSize = 1.f;
	mbLineClosed = true;
	mfDelta = 0.f;
	mvFlag.Empty();
	for (int i = 0; i < CeilElement::ActorMax; i++)
		mvFlag.Add(false);

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
void UCeilTemplate::BeginPlay()
{
	Super::BeginPlay();

	//if(nullptr == RootComponent)
	//	RootComponent = NewObject<USceneComponent>(this, "CeilTempRoot", RF_NoFlags, nullptr, false, nullptr);
	//RootComponent->Mobility = EComponentMobility::Movable;	
}

//// Called every frame
//void UCeilTemplate::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//	//
//	mfDelta += DeltaTime;
//	Update();
//}

// Called every frame
void UCeilTemplate::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//
	mfDelta += DeltaTime;
	Update();
}

UDynaMesh* UCeilTemplate::CreatElem(UWorld* pWorld)//(FName const& name)
{
	static uint32 num = 0;
	char sz[256];
	sprintf(sz, "CeilElem_%d", num++);
	FName tmp(sz);
	UDynaMesh* pEleComp = NewObject<UDynaMesh>(this, tmp, RF_NoFlags, nullptr, false, nullptr);
	if (nullptr != pEleComp)
	{
		pEleComp->Mobility = EComponentMobility::Movable;
		pEleComp->SetVisibility(true);
		pEleComp->RegisterElem(pWorld);
	}
	return pEleComp;
}

void UCeilTemplate::UseDefaultTemplate(float & fTopLayerSize, float unity /* = 50.f */)
{
	fTopLayerSize = 0.f;
	//
	InitPrimData(unity);
	TArray<UMaterialInterface*> vMate;
	vMate.Add(mpDefMate);
	for (int i = 0; i < mvTestData.Num(); i++)
	{
		//if (i % 3)continue;
		//vMate.Empty();
		//vMate.Add(mvMateColor[i % 8]);
		AddTemplateData(i, mvTestData[i].polygon, vMate);
		//
		if (0 == mvTestData[i].id)
		{
			FVector vMin, vMax;
			if (CeilPrimData::CalcuBox(vMin, vMax, mvTestData[i].polygon))
			{
				const FVector vTmp = vMax - vMin;
				fTopLayerSize = vTmp.Y;
			}
		}
	}
	//
}

bool UCeilTemplate::AddTemplateData(int id, TArray<FVector> const& vPolygon, TArray<UMaterialInterface*> const& vMateerial)
{
	if (id < 0 || vPolygon.Num() <= 0)
		return false;
	//
	mvPrimData.Add(CeilPrimData(id, vPolygon));
	CeilElement ele;
	ele.id = id;
	ele.primID = id;
	ele.show = true;
	ele.mateArray = vMateerial;
	mvElement.Add(ele);
	//
	mbUpdate = true;
	return true;
}

bool UCeilTemplate::UpdateTemplateData(int id, float XSize, float YSize)
{
	if (!(id >= 0 && id < CeilPrimData::ElementMax && XSize > 0.f && YSize > 0.f))
		return false;
	CeilElement* ele = mvElement.FindByKey<int>(id);
	if (nullptr == ele)
		return false;
	CeilPrimData* pri = mvPrimData.FindByKey<int>(ele->primID);
	if (nullptr == pri)
		return false;
	XSize *= 0.5f;
	YSize *= 0.5f;
	FVector v0, v1;
	CeilPrimData::CalcuBox(v0, v1, pri->polygon);
	float x, y;
	const FVector tmp = (v0 + v1) * 0.5f;
	TArray<FVector> vPolygon;
	for (int i = 0; i < pri->polygon.Num(); i++)
	{
		x = tmp.X + (pri->polygon[i].X - tmp.X) / pri->hx * XSize;
		y = tmp.Y + (pri->polygon[i].Y - tmp.Y) / pri->hy * YSize;
		vPolygon.Add(FVector(x, y, 0.f));
	}
	pri->init(pri->id, vPolygon);
	mbUpdate = true;
	return true;


	
}

bool UCeilTemplate::UpdateTemplateData(int id, TArray<FVector> const& vPolygon)
{
	if (id >= 0 && id < mvPrimData.Num() && vPolygon.Num() > 0)
	{
		CeilPrimData* pData = mvPrimData.FindByKey<int>(id);
		if (nullptr != pData)
		{
			pData->init(id, vPolygon);
			mbUpdate = true;
			return true;
		}
	}
	return false;
}

void UCeilTemplate::ClearTemplateData()
{
	ClearMesh();
	mvPrimData.Empty();
	mvElement.Empty();
}

bool UCeilTemplate::CreateMesh(bool b3DLoft /* = true */, float loftSize /* = 100.f */)
{
	UWorld* pWorld = GetWorld();
	if (nullptr == pWorld || mvElement.Num() <= 0)
		return false;
	if (loftSize <= 0.f)
		loftSize = 1.f;
	ClearMesh(b3DLoft ? 0 : 1);
	mvFlag[b3DLoft ? 0 : 1] = true;
	//
	if(b3DLoft)
		mfLoftSize = loftSize;
	UDynaMesh* pActor = nullptr;
	UMaterialInterface* pMate = nullptr;
	const int eleCnt = mvElement.Num();
	for (int i = 0; i < eleCnt; i++)
	{
		if (!mvElement[i].show)
			continue;
		pActor = CreatElem(pWorld);
		pMate = ((mvElement[i].mateArray.Num() > 0) ? mvElement[i].mateArray[0] : mpDefMate);
		const int id = mvElement[i].primID;
		if (nullptr != pActor && id >= 0 && id < mvPrimData.Num())
		{
			if (mvPrimData[id].convexArray.Num() <= 0)
			{
				if(b3DLoft)
					pActor->BuildLofting(mvPrimData[id].polygon, pMate, mfLoftSize);
				else
					pActor->BuildSurface(mvPrimData[id].polygon, pMate);
			}
			else
			{
				for (int convIdx = 0; convIdx < mvPrimData[id].convexArray.Num(); convIdx++)
				{
					if(b3DLoft)
						pActor->BuildLofting(mvPrimData[id].convexArray[convIdx], pMate, mfLoftSize);
					else
						pActor->BuildSurface(mvPrimData[id].convexArray[convIdx], pMate);
				}
			}
			pActor->SetRelativeScale3D(mvElement[i].scale);
			pActor->SetRelativeRotation(mvElement[i].quat);
			pActor->SetRelativeLocation(mvElement[i].pos);
			//pActor->SetWorldScale3D(mvElement[i].scale);
			//pActor->SetWorldRotation(mvElement[i].quat);
			//pActor->SetWorldLocation(mvElement[i].pos);
			pActor->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
			if (CeilElement::ActorMax > 1)
				mvElement[i].actor[(b3DLoft ? 0 : 1)] = pActor;
			
		}
	}
	//
	return true;
}

bool UCeilTemplate::CreateLine(float fSize /* = 10.f */, bool bClosed /* = true */)
{
	UWorld* pWorld = GetWorld();
	if (nullptr == pWorld || mvElement.Num() <= 0)
		return false;
	if (fSize <= 0.f)
		fSize = 1.f;
	ClearMesh(2);
	mvFlag[2] = true;
	//
	mfLineSize = fSize;
	mbLineClosed = bClosed;
	UDynaMesh* pActor = nullptr;
	UMaterialInterface* pMate = nullptr;
	const int eleCnt = mvElement.Num();
	for (int i = 0; i < eleCnt; i++)
	{
		if (!mvElement[i].show)
			continue;
		pActor = CreatElem(pWorld);
		pMate = ((mvElement[i].mateArray.Num() > 0) ? mvElement[i].mateArray[0] : mpDefMate);
		const int id = mvElement[i].primID;
		if (nullptr != pActor && id >= 0 && id < mvPrimData.Num())
		{
			if (mvPrimData[id].convexArray.Num() <= 0)
			{
				pActor->CreateLine(mvPrimData[id].polygon, pMate, mfLineSize, mbLineClosed);
			}
			else
			{
				for (int convIdx = 0; convIdx < mvPrimData[id].convexArray.Num(); convIdx++)
				{
					pActor->CreateLine(mvPrimData[id].polygon, pMate, mfLineSize, mbLineClosed);
				}
			}
			pActor->SetRelativeScale3D(mvElement[i].scale);
			pActor->SetRelativeRotation(mvElement[i].quat);
			pActor->SetRelativeLocation(mvElement[i].pos);
			pActor->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
			if(CeilElement::ActorMax > 2)
				mvElement[i].actor[2] = pActor;

			

		}
	}
	//
	return true;
}

TArray<bool> UCeilTemplate::ClearMesh(int uType /* = -1 */)
{
	TArray<bool> vFlag;
	for (int i = 0; i < CeilElement::ActorMax; i++)
		vFlag.Add(false);

	UDynaMesh* pActor = nullptr;
	for (int i = 0; i < mvElement.Num(); i++)
	{
		if (uType >= 0 && uType < CeilElement::ActorMax)
		{
			if (nullptr != mvElement[i].actor[uType])
			{
				pActor = Cast<UDynaMesh>(mvElement[i].actor[uType]);
				pActor->Clear();
				pActor->DestroyComponent(true);
				mvElement[i].actor[uType] = nullptr;
				vFlag[uType] = true;
			}
		}
		else if (uType < 0)
		{
			for (int j = 0; j < CeilElement::ActorMax; j++)
			{
				if (nullptr != mvElement[i].actor[j])
				{
					pActor = Cast<UDynaMesh>(mvElement[i].actor[j]);
					pActor->Clear();
					pActor->DestroyComponent(true);
					mvElement[i].actor[j] = nullptr;
					vFlag[j] = true;
				}
			}
		}
		
	}
	//
	return vFlag;
}

void UCeilTemplate::ShowElement(bool bShow, int id /* = -1 */)
{
	if (id < 0)
	{
		for (int i = 0; i < mvElement.Num(); i++)
		{
			mvElement[i].show = bShow;
			mbUpdate = true;
		}
	}
	else if (id >= 0 && id < mvElement.Num() && mvElement[id].show != bShow)
	{
		mvElement[id].show = bShow;
		mbUpdate = true;
	}
}

void UCeilTemplate::ShowType(bool b3DLoft, bool b2DSurface, bool bLine)
{
	if (b3DLoft == mvFlag[0] && b2DSurface == mvFlag[1] && bLine == mvFlag[2])
		return;
	mvFlag[0] = b3DLoft;
	mvFlag[1] = b2DSurface;
	mvFlag[2] = bLine;
	mbUpdate = true;
}

UActorComponent* UCeilTemplate::GetElementActor(int id, int uType /* = 0 */)
{
	UActorComponent* pActor = nullptr;
	if (id >= 0 && id < mvElement.Num() && uType >= 0 && uType < CeilElement::ActorMax)
	{
		pActor = mvElement[id].actor[uType];
	}
	return pActor;
}

void UCeilTemplate::Update()
{
	if (!mbUpdate || mvElement.Num() <= 0)
		return;
	//
	TArray<bool> flag = ClearMesh(-1);
	UpdateElement(0.f, 0.f);
	if (mvFlag[0])
		CreateMesh(true, mfLoftSize);
	if (mvFlag[1])
		CreateMesh(false, mfLoftSize);
	if (mvFlag[2])
		CreateLine(mfLineSize, mbLineClosed);
	//
	mbUpdate = false;
}

void UCeilTemplate::InitPrimData(float unity /* = 1.f */)
{
	mvTestData.Empty();

	const float s = ((unity <= 0.f) ? 1.f : unity);
	CeilPrimData tmp;

	tmp.Clear();
	tmp.id = 0;
	tmp.Rect(20 * s, 2 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 1;
	tmp.Rect(15 * s, 3 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 2;
	tmp.Triangle(3 * s, 3 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 3;
	tmp.Rect(10 * s, 15 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 4;
	tmp.Rect(1 * s, 8 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 5;
	tmp.Triangle(4 * s, 4 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 6;
	tmp.Rect(1 * s, 6 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 7;
	tmp.Triangle(3 * s, 3 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 8;
	tmp.Rect(3 * s, 2 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 9;
	tmp.reverseL(12 * s, 5 * s, 2 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 10;
	tmp.Circle(1 * s);
	mvTestData.Add(tmp);

	tmp.Clear();
	tmp.id = 11;
	tmp.Triangle(5 * s, 5 * s);
	mvTestData.Add(tmp);

}

void UCeilTemplate::InitElement(float x, float y, float width /* = 100.f */, float height /* = 100.f */)
{
	CeilElement tmp;
	mvElement.Empty();

	float tx = 0;
	float ty = 0;

	tx = mvPrimData[0].hx;
	ty = 0.f;
	tmp.id = 0;
	tmp.primID = 0;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[1].hx;
	ty = -mvPrimData[0].hy - mvPrimData[1].hy;
	tmp.id = 1;
	tmp.primID = 1;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[1].hx * 2.f + mvPrimData[2].hx;
	//ty = -mvPrimData[0].hy - mvPrimData[1].hy;
	tmp.id = 2;
	tmp.primID = 2;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[3].hx;
	ty = -mvPrimData[0].hy - mvPrimData[1].hy * 2.f - mvPrimData[3].hy;
	tmp.id = 3;
	tmp.primID = 3;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[3].hx * 2.f + mvPrimData[4].hx;
	ty = - mvPrimData[0].hy - mvPrimData[1].hy * 2.f - mvPrimData[4].hy;
	tmp.id = 4;
	tmp.primID = 4;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[3].hx * 2.f + mvPrimData[4].hx * 2.f + mvPrimData[5].hx;
	ty = - mvPrimData[0].hy - mvPrimData[1].hy * 2.f - mvPrimData[5].hy;
	tmp.id = 5;
	tmp.primID = 5;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[6].hx;
	ty = -mvPrimData[0].hy - mvPrimData[1].hy * 2.f - mvPrimData[3].hy * 2.f - mvPrimData[6].hy;
	tmp.id = 6;
	tmp.primID = 6;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[6].hx * 2.f + mvPrimData[7].hx;
	ty = -mvPrimData[0].hy - mvPrimData[1].hy * 2.f - mvPrimData[3].hy * 2.f - mvPrimData[7].hy;
	tmp.id = 7;
	tmp.primID = 7;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[3].hx * 2.f + mvPrimData[8].hx;
	ty = -mvPrimData[0].hy - mvPrimData[1].hy * 2.f - mvPrimData[3].hy * 2.f - mvPrimData[8].hy;
	tmp.id = 8;
	tmp.primID = 8;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[3].hx * 2.f + mvPrimData[9].hx;
	ty = -mvPrimData[0].hy - mvPrimData[1].hy * 2.f - mvPrimData[3].hy * 2.f + mvPrimData[9].hy;
	tmp.id = 9;
	tmp.primID = 9;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[3].hx * 2.f + mvPrimData[10].hx * 2.f;
	ty = -mvPrimData[0].hy - mvPrimData[1].hy * 2.f - mvPrimData[3].hy * 2.f + mvPrimData[9].hz * 2.f + mvPrimData[10].hz * 1.2f;
	tmp.id = 10;
	tmp.primID = 10;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, y + ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);

	tx = mvPrimData[3].hx * 2.f + mvPrimData[9].hx * 2.f + mvPrimData[11].hx;
	ty = mvElement[9].pos.Z;
	//ty = -mvPrimData[0].hy - mvPrimData[1].hy * 2.f - mvPrimData[3].hy * 2.f + mvPrimData[9].hz;// *2.f + mvPrimData[10].hz * 1.2f;
	tmp.id = 11;
	tmp.primID = 11;
	tmp.show = true;
	tmp.pos = FVector(x + tx, 0, ty);
	tmp.scale = FVector(1.f, 1.f, 1.f);
	tmp.mateArray = mvMateColor;
	mvElement.Add(tmp);




}

void UCeilTemplate::UpdateElement(float x, float y, float width /* = 100.f */, float height /* = 100.f */)
{
	PreProce();
	CeilPrimData* tmp = nullptr;
	CeilElement* ele = nullptr;
	float tx = 0.f;
	float ty = 0.f;
	int id = 0;

	if (mvElement.Num() < 1)
		return;
	//ele = (mvElement.Num() > 0)? &mvElement[0] : nullptr;
	ele = &mvElement[0];
	if (ele->show)
	{
		//id = ele->primID;
		//tx = mvPrimData[id].hx;
		//ty = -mvPrimData[id].hy;
		tmp = mvPrimData.FindByKey(ele->primID);
		if (nullptr != tmp)
		{
			tx = tmp->hx;
			ty = -tmp->hy;
			ele->pos = FVector(x + tx, 0, y + ty);
		}
	}

	if (mvElement.Num() < 2)
		return;
	ele = &mvElement[1];
	if (ele->show)
	{
		//id = ele->primID;
		//tx = mvPrimData[id].hx;
		//ty = -mvPrimData[id].hy;
		tmp = mvPrimData.FindByKey(ele->primID);
		if (nullptr != tmp)
		{
			tx = tmp->hx;
			ty = -tmp->hy;
		}
		if (mvElement[0].show)
		{
			tmp = mvPrimData.FindByKey(mvElement[0].primID);
			if (tmp)
			{
				ty += -tmp->hy * 2.f;
			}
		}
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 3)
		return;
	ele = &mvElement[2];
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			tx += mvPrimData[1].hx * 2.f;
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 4)
		return;
	ele = &mvElement[3];
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			ty += -mvPrimData[1].hy * 2.f;
		else if(mvElement[2].show)
			ty += -mvPrimData[2].hy * 2.f;
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 5)
		return;
	ele = &mvElement[4];
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			ty += -mvPrimData[1].hy * 2.f;
		else if (mvElement[2].show)
			ty += -mvPrimData[2].hy * 2.f;
		if (mvElement[3].show)
			tx += mvPrimData[3].hx * 2.f;
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 6)
		return;
	ele = &mvElement[5];
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			ty += -mvPrimData[1].hy * 2.f;
		else if (mvElement[2].show)
			ty += -mvPrimData[2].hy * 2.f;
		if (mvElement[3].show)
			tx += mvPrimData[3].hx * 2.f;
		if (mvElement[4].show)
			tx += mvPrimData[4].hx * 2.f;
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 7)
		return;
	ele = &mvElement[6];
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			ty += -mvPrimData[1].hy * 2.f;
		else if (mvElement[2].show)
			ty += -mvPrimData[2].hy * 2.f;
		if (mvElement[3].show)
			ty += -mvPrimData[3].hy * 2.f;
		else if(mvElement[4].show)
			ty += -mvPrimData[4].hy * 2.f;
		else if (mvElement[5].show)
			ty += -mvPrimData[5].hy * 2.f;
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 8)
		return;
	ele = &mvElement[7];
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			ty += -mvPrimData[1].hy * 2.f;
		else if (mvElement[2].show)
			ty += -mvPrimData[2].hy * 2.f;
		if (mvElement[3].show)
			ty += -mvPrimData[3].hy * 2.f;
		else if (mvElement[4].show)
			ty += -mvPrimData[4].hy * 2.f;
		else if (mvElement[5].show)
			ty += -mvPrimData[5].hy * 2.f;
		if (mvElement[6].show)
			tx += mvPrimData[6].hx * 2.f;
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 9)
		return;
	const bool bHide345 = (!mvElement[3].show && !mvElement[4].show && !mvElement[5].show);
	ele = &mvElement[8];
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			ty += -mvPrimData[1].hy * 2.f;
		else if (mvElement[2].show)
			ty += -mvPrimData[2].hy * 2.f;
		if (mvElement[3].show)
		{
			ty += -mvPrimData[3].hy * 2.f;
			tx += mvPrimData[3].hx * 2.f;
		}
		else
		{
			int idx = 4;
			if (bHide345)
				idx = 6;
			if (mvElement[idx].show)
			{
				ty += -mvPrimData[idx].hy * 2.f;
				tx += mvPrimData[idx].hx * 2.f;
			}
			else if (mvElement[idx + 1].show)
			{
				ty += -mvPrimData[idx + 1].hy * 2.f;
			}
			if (mvElement[idx + 1].show)
			{
				tx += mvPrimData[idx + 1].hx * 2.f;
			}
		}
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 10)
		return;
	ele = &mvElement[9];
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			ty += -mvPrimData[1].hy * 2.f;
		else if (mvElement[2].show)
			ty += -mvPrimData[2].hy * 2.f;
		if (mvElement[3].show)
		{
			ty += -mvPrimData[3].hy * 2.f;
			tx += mvPrimData[3].hx * 2.f;
		}
		else
		{
			int idx = 4;
			if (bHide345)
				idx = 6;
			if (mvElement[idx].show)
			{
				ty += -mvPrimData[idx].hy * 2.f;
				tx += mvPrimData[idx].hx * 2.f;
			}
			else if (mvElement[idx + 1].show)
			{
				ty += -mvPrimData[idx + 1].hy * 2.f;
			}
			if (mvElement[idx + 1].show)
			{
				tx += mvPrimData[idx + 1].hx * 2.f;
			}
		}
		ty += mvPrimData[id].hy * 2.f;
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 11)
		return;
	ele = &mvElement[10];
	ele->show &= mvElement[9].show;
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			ty += -mvPrimData[1].hy * 2.f;
		else if (mvElement[2].show)
			ty += -mvPrimData[2].hy * 2.f;
		if (mvElement[3].show)
		{
			ty += -mvPrimData[3].hy * 2.f;
			tx += mvPrimData[3].hx * 2.f;
		}
		else
		{
			int idx = 4;
			if (bHide345)
				idx = 6;
			if (mvElement[idx].show)
			{
				ty += -mvPrimData[idx].hy * 2.f;
				tx += mvPrimData[idx].hx * 2.f;
			}
			else if (mvElement[idx + 1].show)
			{
				ty += -mvPrimData[idx + 1].hy * 2.f;
			}
			if (mvElement[idx + 1].show)
			{
				tx += mvPrimData[idx + 1].hx * 2.f;
			}
		}
		ty += mvPrimData[id].hy * 2.f;
		ty += mvPrimData[9].hy * 1.f;// mvPrimData[9].hz * 2.f;
		tx += mvPrimData[id].hx * 0.5f;
		ty += mvPrimData[id].hy * 0.5f;
		ele->pos = FVector(x + tx, 0, y + ty);
	}

	if (mvElement.Num() < 12)
		return;
	ele = &mvElement[11];
	ele->show &= mvElement[9].show;
	if (ele->show)
	{
		id = ele->primID;
		tx = mvPrimData[id].hx;
		ty = -mvPrimData[id].hy;
		if (mvElement[0].show)
			ty += -mvPrimData[0].hy * 2.f;
		if (mvElement[1].show)
			ty += -mvPrimData[1].hy * 2.f;
		else if (mvElement[2].show)
			ty += -mvPrimData[2].hy * 2.f;
		if (mvElement[3].show)
		{
			ty += -mvPrimData[3].hy * 2.f;
			tx += mvPrimData[3].hx * 2.f;
		}
		else
		{
			int idx = 4;
			if (bHide345)
				idx = 6;
			if (mvElement[idx].show)
			{
				ty += -mvPrimData[idx].hy * 2.f;
				tx += mvPrimData[idx].hx * 2.f;
			}
			else if (mvElement[idx + 1].show)
			{
				ty += -mvPrimData[idx + 1].hy * 2.f;
			}
			if (mvElement[idx + 1].show)
			{
				tx += mvPrimData[idx + 1].hx * 2.f;
			}
		}
		ty += mvPrimData[id].hy * 2.f;
		tx += mvPrimData[9].hx * 2.f;
		ele->pos = FVector(x + tx, 0, y + ty);
	}




}

CeilElement* UCeilTemplate::GetElement(int id)
{
	if (id >= 0 && id < mvElement.Num())// && mvElement[id].show)
		return &mvElement[id];
	//
	return nullptr;
}

CeilElement* UCeilTemplate::EnableElement(int id, bool bEnable /* = true */)
{
	if (id >= 0 && id < mvElement.Num())
		mvElement[id].show = bEnable;
	//
	return GetElement(id);
}

void UCeilTemplate::PreProce()
{
	mvPrimData.Sort();
	mvElement.Sort();
	const int eleCnt = mvElement.Num();
	if (eleCnt >= ElementMax)
		return;

	mvEleCopy = mvElement;
	mvElement.Reset(ElementMax);
	for (int i = 0; i < ElementMax; i++)
	{
		mvElement.Add(CeilElement());
		CeilElement* pData = mvEleCopy.FindByKey<int>(i);
		if (nullptr != pData)
		{
			mvElement[i] = *pData;
		}
		else
		{
			mvElement[i].show = false;
		}
	}

}

void UCeilTemplate::TransformElement(FVector const& vPos, FVector const& vDir)
{
	FVector dir(vDir);
	dir.Z = 0.f;
	if (dir.Normalize())
	{
		const FVector vOri(0.f, 1.f, 0.f); //(1.f, 0.f, 0.f);
		const FVector vNor = FVector(0.f, 0.f, 1.f);
		FQuat quat = FQuat::FindBetweenNormals(vOri, dir);
		quat *= FQuat(vNor, 3.1415926f);
		//SetActorRotation(quat);
		//SetActorLocation(vPos);
		SetRelativeRotation(quat);
		SetRelativeLocation(vPos);
	}

	//
	USceneComponent* pComp = nullptr;
	for (int i = 0; i < mvElement.Num(); i++)
	{
		if (0 == mvElement[i].id && mvElement[i].show)
		{
			pComp = Cast<USceneComponent>(mvElement[0].actor[0]);
			if (nullptr != pComp)
			{
				pComp->SetVisibility(false);
			}
		}
		if (10 == mvElement[i].id && mvElement[i].show)
		{
			pComp = Cast<USceneComponent>(mvElement[i].actor[0]);
			if (nullptr != pComp)
			{
				mvLampPos = pComp->GetComponentLocation();
				pComp->SetVisibility(false);
			}
		}
	}
	
}

bool UCeilTemplate::GetElementSize(int id, FVector2D & Size) const
{
	if (id >= 0 && id < CeilPrimData::ElementMax)
	{
		const CeilElement* ele = mvElement.FindByKey(id);
		if (nullptr == ele)
			return false;
		const CeilPrimData* prim = mvPrimData.FindByKey(ele->primID);
		if (nullptr != prim)
		{
			Size.X = prim->hx * 2.f;
			Size.Y = prim->hy * 2.f;
			return true;
		}
	}
	return false;
}

const FVector& UCeilTemplate::LampPosition() const
{
	return mvLampPos;
}

void UCeilTemplate::RegisterTemp(UWorld* pWorld)
{
	RegisterComponentWithWorld(pWorld);
	SetVisibility(true);
}

void UCeilTemplate::Clip(FVector const& vPoint, FVector const& vNormal)
{
	////static void SliceProceduralMesh(
	//UProceduralMeshComponent* InProcMesh = nullptr; FVector PlanePosition; FVector PlaneNormal; bool bCreateOtherHalf = false;
	//UProceduralMeshComponent* OutOtherHalfProcMesh = nullptr; EProcMeshSliceCapOption CapOption = {}; UMaterialInterface* CapMaterial = nullptr;
	//UMeshComponent* p = nullptr;
	//UKismetProceduralMeshLibrary::SliceProceduralMesh(InProcMesh, vPoint, vNormal, false, OutOtherHalfProcMesh, CapOption, CapMaterial);
}


#pragma optimize( "", on )