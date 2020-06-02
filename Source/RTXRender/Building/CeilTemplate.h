// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CDynaMesh.h"
#include "CeilTemplate.generated.h"

UCLASS()
class UCeilTemplate : public USceneComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UCeilTemplate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void UseDefaultTemplate(float & fTopLayerSize, float unity = 50.f);
	bool AddTemplateData(int id, TArray<FVector> const& vPolygon, TArray<UMaterialInterface*> const& vMateerial);
	bool UpdateTemplateData(int id, TArray<FVector> const& vPolygon);
	bool UpdateTemplateData(int id, float XSize, float YSize);
	void ClearTemplateData();
	bool CreateMesh(bool b3DLoft = true, float loftSize = 100.f);
	bool CreateLine(float fSize = 10.f, bool bClosed = true);
	TArray<bool> ClearMesh(int uType = -1); //@uType: 0 3DLoft, 1 surface, 2 line
	void ShowElement(bool bShow, int id = -1);
	void ShowType(bool b3DLoft, bool b2DSurface, bool bLine);
	UActorComponent* GetElementActor(int id, int uType = 0); //@uType: 0 3DLoft, 1 surface, 2 line
	bool GetElementSize(int id, FVector2D & Size) const;
	const FVector& LampPosition() const;

public:
	CeilElement* GetElement(int id);
	void RegisterTemp(UWorld* pWorld);
	void TransformElement(FVector const& vPos, FVector const& vDir);
	void Clip(FVector const& vPoint, FVector const& vNormal);

private:
	void InitPrimData(float unity = 1.f);
	void InitElement(float x, float y, float width = 100.f, float height = 100.f);

	void UpdateElement(float x, float y, float width = 100.f, float height = 100.f);
	void Update();
	void PreProce();

	CeilElement* EnableElement(int id, bool bEnable = true);

	UDynaMesh* CreatElem(UWorld* pWorld);//(FName const& strName);
	

private:
	TArray<CeilPrimData>	mvPrimData;
	TArray<CeilElement>		mvElement;
	bool					mbUpdate;
	static const int		ElementMax = 12;
	TArray<CeilElement>		mvEleCopy;
	//bk
	float mfLoftSize;
	float mfLineSize;
	bool  mbLineClosed;
	FVector mvLampPos;
	TArray<bool> mvFlag;
	//
	UMaterialInterface*				mpDefMate;
	TArray<UMaterialInterface*>		mvMateColor;
	TArray<CeilPrimData>	mvTestData;
	float mfDelta;
};
