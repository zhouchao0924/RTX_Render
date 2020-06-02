// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CDynaMesh.generated.h"

struct CeilPrimData
{
	int id = 0;
	TArray<FVector> polygon;
	TArray<TArray<FVector>> convexArray;
	//
	float hx = 0.f; // half or radius
	float hy = 0.f;
	float hz = 0.f;
	//
	static const int ElementMax = 12;

	CeilPrimData()
		: id(0), hx(0.f), hy(0.f), hz(0.f)
	{
		polygon.Empty();
		convexArray.Empty();
	}

	CeilPrimData(CeilPrimData const& o)
		: id(0), hx(0.f), hy(0.f), hz(0.f)
	{
		id = o.id;
		polygon = o.polygon;
		convexArray = o.convexArray;
		//
		hx = o.hx;
		hy = o.hy;
		hz = o.hz;
	}

	CeilPrimData(int uID, TArray<FVector> const& vPolygon)
		: id(uID), hx(0.f), hy(0.f), hz(0.f)
	{
		init(uID, vPolygon);

		//FVector vBox;
		//if (UpdatePolygon(polygon, vPolygon, &vBox))
		//{
		//	hx = vBox.X * 0.5f;
		//	hy = vBox.Y * 0.5f;
		//	hz = vBox.Z * 0.5f;
		//}
		//convexArray.Empty();
	}

	CeilPrimData(int uID, TArray<FVector> const& vPolygon, TArray<TArray<FVector>> const& vConvexArray)
		: id(uID), hx(0.f), hy(0.f), hz(0.f)
	{
		init(uID, vPolygon, vConvexArray);

		//FVector vBox;
		//if (UpdatePolygon(polygon, vPolygon, &vBox))
		//{
		//	hx = vBox.X * 0.5f;
		//	hy = vBox.Y * 0.5f;
		//	hz = vBox.Z * 0.5f;
		//}

		//convexArray.Empty();
		//TArray<FVector> vDest;
		//for (int i = 0; i < vConvexArray.Num(); i++)
		//{
		//	if (UpdatePolygon(vDest, vConvexArray[i], nullptr))
		//		convexArray.Add(vDest);
		//}
	}

	CeilPrimData& operator = (CeilPrimData const& o)
	{
		id = o.id;
		polygon = o.polygon;
		convexArray = o.convexArray;
		//
		hx = o.hx;
		hy = o.hy;
		hz = o.hz;
		//
		return *this;
	}

	bool operator<(CeilPrimData const& o) const
	{
		return (id < o.id);
	}

	bool operator==(int key) const
	{
		return (id == key);
	}

	bool operator()(CeilPrimData const& A, CeilPrimData const& B) const
	{
		return (A < B);
	}

	~CeilPrimData()
	{
		Clear();
	}

	bool init(int uID, TArray<FVector> const& vPolygon)
	{
		if (id < 0 || vPolygon.Num() <= 0)
			return false;

		id = uID;
		hx = 0.f;
		hy = 0.f;
		hz = 0.f;

		FVector vBox;
		if (UpdatePolygon(polygon, vPolygon, &vBox))
		{
			hx = vBox.X * 0.5f;
			hy = vBox.Y * 0.5f;
			hz = vBox.Z * 0.5f;
		}

		convexArray.Empty();
		return true;
	}

	bool init(int uID, TArray<FVector> const& vPolygon, TArray<TArray<FVector>> const& vConvexArray)
	{
		if (!init(uID, vPolygon))
			return false;
		TArray<FVector> vDest;
		for (int i = 0; i < vConvexArray.Num(); i++)
		{
			if (UpdatePolygon(vDest, vConvexArray[i], nullptr))
				convexArray.Add(vDest);
		}
		return true;
	}

	static bool CalcuBox(FVector & vMinPos, FVector & vMaxPos, TArray<FVector> const& vPointArray)
	{
		if (vPointArray.Num() < 2)
			return false;
		vMinPos.X = vMaxPos.X = vPointArray[0].X;
		vMinPos.Y = vMaxPos.Y = vPointArray[0].Y;
		vMinPos.Z = vMaxPos.Z = vPointArray[0].Z;
		for (int i = 1; i < vPointArray.Num(); i++)
		{
			if (vPointArray[i].X < vMinPos.X)
				vMinPos.X = vPointArray[i].X;
			if (vPointArray[i].Y < vMinPos.Y)
				vMinPos.Y = vPointArray[i].Y;
			if (vPointArray[i].Z < vMinPos.Z)
				vMinPos.Z = vPointArray[i].Z;

			if (vPointArray[i].X > vMaxPos.X)
				vMaxPos.X = vPointArray[i].X;
			if (vPointArray[i].Y > vMaxPos.Y)
				vMaxPos.Y = vPointArray[i].Y;
			if (vPointArray[i].Z > vMaxPos.Z)
				vMaxPos.Z = vPointArray[i].Z;

		}
		return true;
	}

	bool UpdatePolygon(TArray<FVector> & vDest, TArray<FVector> const& vSrc, FVector* vBox = nullptr) const
	{
		vDest.Empty();
		FVector vMinPos, vMaxPos;
		if (!CalcuBox(vMinPos, vMaxPos, vSrc))
			return false;
		const FVector vCen = (vMinPos + vMaxPos) * 0.5f;
		for (int i = 0; i < vSrc.Num(); i++)
		{
			vDest.Add(vSrc[i] - vCen);
		}
		if (nullptr != vBox)
		{
			const FVector vTmp = vMaxPos - vMinPos;
			vBox->X = vTmp.X;
			vBox->Y = vTmp.Y;
			vBox->Z = vTmp.Z;
		}
		return true;
	}

	void Clear()
	{
		id = 0;
		polygon.Empty();
		convexArray.Empty();
		hx = hy = hz = 0.f;
	}

	void Rect(float cx, float cy)
	{
		if (cx <= 0.f || cy <= 0.5f)
			return;
		hx = cx * 0.5f;
		hy = cy * 0.5f;
		polygon.Empty();
		polygon.Add(FVector(hx, hy, 0.f));
		polygon.Add(FVector(-hx, hy, 0.f));
		polygon.Add(FVector(-hx, -hy, 0.f));
		polygon.Add(FVector(hx, -hy, 0.f));
		convexArray.Empty();
	}

	void Triangle(float cx, float cy)
	{
		if (cx <= 0.f || cy <= 0.5f)
			return;
		hx = cx * 0.5f;
		hy = cy * 0.5f;
		polygon.Empty();
		polygon.Add(FVector(hx, hy, 0.f));
		polygon.Add(FVector(-hx, hy, 0.f));
		polygon.Add(FVector(-hx, -hy, 0.f));
		convexArray.Empty();
	}

	void Circle(float radius, int segment = 24)
	{
		if (radius <= 0.f || segment < 3)
			return;
		hx = hy = hz = radius;
		polygon.Empty();
		const float one = 2.f * 3.1415f / (float)segment;
		for (int i = 0; i < segment; i++)
			polygon.Add(FVector(hx * cos(i*one), hy * sin(i*one), 0.f));
		convexArray.Empty();
	}

	void reverseL(float cx, float cy, float thick)
	{
		if (cx <= 0.f || cy <= 0.f || thick <= 0.f)
			return;
		hx = cx * 0.5f;
		hy = cy * 0.5f;
		hz = thick * 0.5f;
		polygon.Empty();
		polygon.Add(FVector(hx - hz * 2.f, -hy + hz * 2.f, 0.f));
		polygon.Add(FVector(-hx, -hy + hz * 2.f, 0.f));
		polygon.Add(FVector(-hx, -hy, 0.f));
		polygon.Add(FVector(hx, -hy, 0.f));
		polygon.Add(FVector(hx, hy, 0.f));
		polygon.Add(FVector(hx-hz*2.f, hy, 0.f));
		//
		convexArray.Empty();
		//TArray<FVector> conv;
		//conv = { polygon[0],polygon[1], polygon[2], polygon[3] };
		//convexArray.Add(conv);
		//conv = { polygon[0], polygon[3], polygon[4], polygon[5] };
		//convexArray.Add(conv);
	}

	
};


//USTRUCT()
struct CeilElement
{
	int id = 0;
	int primID = 0;
	bool show = false;
	FVector pos = FVector(0.f, 0.f, 0.f);
	FQuat quat = FQuat(FVector(1.f, 0.f, 0.f), 1.5707f);//FQuat::Identity;// 
	FVector scale = FVector(1.f, 1.f, 1.f);
	//
	TArray<UMaterialInterface*> mateArray;
	static const int ActorMax = 3;
	UActorComponent* actor[ActorMax] = { nullptr };

	CeilElement()
		:id(0),primID(0),show(false),pos(FVector(0.f, 0.f, 0.f)),quat(FQuat(FVector(1.f, 0.f, 0.f), 1.5707f)),scale(FVector(1.f, 1.f, 1.f))
	{
		for (int i = 0; i < ActorMax; i++)
			actor[i] = nullptr;
	}

	CeilElement(CeilElement const& o)
	{
		id = o.id;
		primID = o.primID;
		show = o.show;
		pos = o.pos;
		quat = o.quat;
		scale = o.scale;
		mateArray = o.mateArray;
		for (int i = 0; i < ActorMax; i++)
			actor[i] = o.actor[i];

	}

	CeilElement& operator = (CeilElement const& o)
	{
		id = o.id;
		primID = o.primID;
		show = o.show;
		pos = o.pos;
		quat = o.quat;
		scale = o.scale;
		mateArray = o.mateArray;
		for (int i = 0; i < ActorMax; i++)
			actor[i] = o.actor[i];
		//
		return *this;
	}

	bool operator<(CeilElement const& o) const
	{
		return (id < o.id);
	}

	bool operator==(int key) const
	{
		return (id == key);
	}
	
	bool operator()(CeilElement const& A, CeilElement const& B) const
	{
		return (A < B);
	}

	
};



UCLASS()
class UDynaMesh : public USceneComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UDynaMesh();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	bool BuildLofting(TArray<FVector> const& pointArray, UMaterialInterface* pMaterial, float fSize = 10.f, bool bClosed = true);
	bool BuildLoftingWithSubMesh(TArray<FVector> const& pointArray, TArray<UMaterialInterface*> vMaterial, float fSize = 10.f, bool bClosed = true);
	bool BuildSurface(TArray<FVector> const& pointArray, UMaterialInterface* pMate = nullptr, FVector2D uvScale = FVector2D(1.f, 1.f), FVector const& vDir = FVector(0.f, 0.f, -1.f), float fOffset = 0.f);
	void Show(bool bShow, int uSection = -1);
	void Clear();
	bool CreateLine(TArray<FVector> const& pointArray, UMaterialInterface* pMaterial, float fSize = 1.f, bool bClosed = true);
	bool CreateTopLayer(TArray<FVector> const& pointArray, UMaterialInterface* pMaterial, float fSize = 10.f, bool bClosed = true);
	void RegisterElem(UWorld* pWorld);
	//void Clip(FVector const& vPoint, FVector const& vNormal);
	UMeshComponent* MeshComponent();
	void SetInst(UGameInstance* pInst);

private:
	//
	bool BuildSurfaceBK(TArray<FVector> const& pointArray, UMaterialInterface* pMate = nullptr, FVector2D uvScale = FVector2D(1.f, 1.f));
	bool Test(bool b3DLoft = true);

	bool BuildMesh(TArray<FVector> const& pointArray, TArray<UMaterialInterface*> vMaterial, float fSize = 10.f, bool bClosed = true, bool bSubMesh = false, FVector const& vDir = FVector(0.f,0.f,-1.f), FVector2D uvScale = FVector2D(1.f, 1.f));
	bool BuildLine(TArray<FVector> const& pointArray, UMaterialInterface* pMaterial, float fSize = 1.f, bool bClosed = true, FVector2D uvScale = FVector2D(1.f,1.f));
	int FilterInvalidPoint(TArray<FVector> * vPoint, TArray<float> * vLen) const;
	bool CalcuSurfaceSize(FVector2D & minPos, FVector2D & maxPos, TArray<FVector> const& pointArray) const;

	void MeshDrawRect(FVector2D Position, FVector2D Size, FLinearColor color);
	void MeshDrawCircle(FVector2D center, int radius, int NumSegments, FLinearColor color);

	bool isConvexPolygon(TArray<FVector> const& vPoint) const;

	void Triangulate(TArray<FVector> & vVertices, TArray<int32> & vTriangles, bool bInvert = false);


private:
	UMeshComponent* mpMesh;
	UMaterialInterface*      mpMate;
	UMaterialInterface*      mpMateWire;
	UMaterialInterface*      mpMateBlend;
	UMaterialInterface*      mpMateDouble;

	int				muIndex;
	float           mfUVSize[2];
	
};
