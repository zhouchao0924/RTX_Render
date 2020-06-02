// Fill out your copyright notice in the Description page of Project Settings.

#include "CDynaMesh.h"
#include "ProceduralMeshComponent.h"
#include "../CGALWrapper/PolygonAlg.h"
//#include "IBuildingSDK.h"
////#include "Class/Property.h"
//#include "Math/kArray.h"
#include "../EditorGameInstance.h"
#include "BuildingSystem.h"
#include "GeometryFunctionLibrary.h"
#pragma optimize( "", off )

// Sets default values for this component's properties
UDynaMesh::UDynaMesh()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	muIndex = 0;
	mfUVSize[0] = 0.25f;
	mfUVSize[1] = 1.f - 2 * mfUVSize[0];

	static uint32 num = 0;
	char sz[32];
	sprintf(sz, "ProceMesh_%d", num++);
	FName name(sz);
	mpMesh = CreateDefaultSubobject<UProceduralMeshComponent>(name);// (TEXT("mesh"));
	//mpMesh = NewObject<UProceduralMeshComponent>(this, name, RF_NoFlags, nullptr, false, nullptr);
	mpMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	
	mpMate = LoadObject<UMaterial>(NULL, TEXT("/Game/MateTest"), NULL, LOAD_None, NULL);
	mpMateWire = LoadObject<UMaterial>(NULL, TEXT("/Game/MateWire"), NULL, LOAD_None, NULL);
	mpMateBlend = LoadObject<UMaterial>(NULL, TEXT("/Game/MateBlend"), NULL, LOAD_None, NULL);
	mpMateDouble = LoadObject<UMaterial>(NULL, TEXT("/Game/MateDouble"), NULL, LOAD_None, NULL);

	//MeshDrawRect(FVector2D(0, 0), FVector2D(40, 100), FLinearColor(1, 0, 0));
	//MeshDrawCircle(FVector2D(-50, 0), 10, 50, FLinearColor(0, 1, 0));
}


// Called when the game starts
void UDynaMesh::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
}


// Called every frame
void UDynaMesh::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UDynaMesh::BuildLofting(TArray<FVector> const& pointArray, UMaterialInterface* pMaterial, float fSize /* = 10.f */, bool bClosed /* = true */)
{
	TArray<UMaterialInterface*> vMate;
	if (nullptr != pMaterial)
		vMate.Add(pMaterial);
	return BuildMesh(pointArray, vMate, fSize, bClosed);
}

bool UDynaMesh::BuildLoftingWithSubMesh(TArray<FVector> const& pointArray, TArray<UMaterialInterface*> vMaterial, float fSize /* = 10.f */, bool bClosed /* = true */)
{
	return true;
}

bool UDynaMesh::CreateLine(TArray<FVector> const& pointArray, UMaterialInterface* pMaterial, float fSize /* = 1.f */, bool bClosed /* = true */)
{
	return BuildLine(pointArray, pMaterial, fSize, bClosed);
}

bool UDynaMesh::CreateTopLayer(TArray<FVector> const& pointArray, UMaterialInterface* pMaterial, float fSize /* = 10.f */, bool bClosed /* = true */)
{
	TArray<UMaterialInterface*> vMate;
	if(nullptr != pMaterial)
		vMate.Add(pMaterial);
	BuildMesh(pointArray, vMate, fSize, bClosed);
	//BuildSurface(pointArray, pMaterial); // top
	//BuildSurface(pointArray, pMaterial); // bot
	return true;
}

void UDynaMesh::RegisterElem(UWorld* pWorld)
{
	RegisterComponentWithWorld(pWorld);
	mpMesh->RegisterComponentWithWorld(pWorld);
	SetVisibility(true);
}



bool UDynaMesh::Test(bool b3DLoft /* = true */)
{
	TArray<UMaterialInterface*> vMate;
	vMate.Add(mpMate);
	vMate.Add(mpMateBlend);
	vMate.Add(mpMateWire);

	TArray<FVector> vPoint;
	int cnt = 14;
	const float one = 2 * 3.1415 / (float)cnt;
	for (int i = 0; i < 10; i++)
		vPoint.Add(FVector(-350 + 100 * cos(i*one), -350 + 100 * sin(i*one), 0));
	BuildLine(vPoint, mpMateWire, 10.f, true, FVector2D(10.f, 1.f));

	vPoint.Empty();
	vPoint.Add(FVector(0, 0, 0));
	vPoint.Add(FVector(100, 0, 0));
	vPoint.Add(FVector(100, 100, 0));
	vPoint.Add(FVector(0, 100, 0));
	//vPoint.Add(FVector(0, 0, 0));
	vPoint.Add(FVector(200, 0, 0));
	vPoint.Add(FVector(250, 0, 0));
	vPoint.Add(FVector(250, 50, 0));
	vPoint.Add(FVector(200, 50, 0));
	BuildLine(vPoint, mpMateWire, 20.f, false);

	vPoint.Empty();
	for (int i = 0; i < 10; i++)
		vPoint.Add(FVector(350 + 100 * cos(i*one), 350 + 100 * sin(i*one), 0));
	BuildMesh(vPoint, vMate, 100.f, true, true);
	//BuildMesh(vPoint,FVector(0,0,1));

	vPoint.Empty();
	vPoint.Add(FVector(500, 0, 0));
	vPoint.Add(FVector(800, 100, 0));
	vPoint.Add(FVector(900, 0, 0));
	vPoint.Add(FVector(1000, 40, 200));
	BuildLine(vPoint, mpMateWire, 20.f, false, FVector2D(10, 1.f));

	vPoint.Empty();
	vPoint.Add(FVector(-200, 0, 0));
	vPoint.Add(FVector(-100, 0, 0));
	vPoint.Add(FVector(-100, 100, 0));
	vPoint.Add(FVector(-200, 100, 0));
	BuildMesh(vPoint, vMate, 100.f);

	vPoint.Empty();
	vPoint.Add(FVector(-200, 0, 120));
	vPoint.Add(FVector(-100, 0, 120));
	vPoint.Add(FVector(-100, 100, 120));
	//vPoint.Add(FVector(-200, 100, 0));
	BuildMesh(vPoint, vMate, 100.f);

	return true;
}

bool UDynaMesh::BuildMesh(TArray<FVector> const& pointArray, TArray<UMaterialInterface*> vMaterial, float fSize /* = 10.f */, bool bClosed /* = true */, bool bSubMesh /* = false */, FVector const& vDir /* = FVector(0.f, 0.f, -1.f) */, FVector2D uvScale /* = FVector2D(1.f, 1.f) */)
{
	if (pointArray.Num() < 2 || vDir.IsNearlyZero() || fSize <= 0.f)
		return false;
	TArray<FVector> vPoint = pointArray;
	if (bClosed)
		vPoint.Add(pointArray[0]);
	TArray<float> vLen;
	const int length = FilterInvalidPoint(&vPoint, &vLen);
	if (length <= 0.f || vLen.Num() < vPoint.Num())
		return false;
	//
	FVector dir(vDir);
	dir.Normalize();
	//
	TArray<FVector> vertices, verticesTop, verticesBot;
	TArray<int32>   triangles, trianglesTop, trianglesBot;
	TArray<FVector> normals, normalsTop, normalsBot;
	TArray<FVector2D> uv0, uvTop, uvBot;
	TArray<FProcMeshTangent> tangents, tangentsTop, tangentsBot;
	TArray<FColor> vertexColors;
	//
	FVector2D vMinPos, vMaxPos;
	CalcuSurfaceSize(vMinPos, vMaxPos, vPoint);
	const float sizeX = vMaxPos.X - vMinPos.X;
	const float sizeY = vMaxPos.Y - vMinPos.Y;
	FVector2D curUVs[4] = { FVector2D(0.f, 0.f), FVector2D(0.f, 0.f), FVector2D(0.f, 1.f * uvScale.Y), FVector2D(0.f, 1.f * uvScale.Y) };
	//
	if (!bSubMesh)
	{
		uvScale.X = uvScale.Y = 1.f;
	}
	FVector v0, v1, v2, v3;
	for (int i = 0; i < vPoint.Num()-1; i++)
	{
		curUVs[0].X = vLen[i] / length * uvScale.X;
		curUVs[1].X = vLen[i+1] / length * uvScale.X;
		curUVs[0].Y = curUVs[1].Y = (bSubMesh? 0.f : mfUVSize[0]);
		curUVs[2].X = curUVs[0].X;
		curUVs[3].X = curUVs[1].X;
		curUVs[2].Y = curUVs[3].Y = (bSubMesh? (1.f * uvScale.Y) : (1.f - mfUVSize[0]));
		//
		v0 = vPoint[i];
		v1 = vPoint[i+1];
		v2 = v0 + dir * fSize;
		v3 = v1 + dir * fSize;
		vertices.Add(v0);
		vertices.Add(v1);
		vertices.Add(v2);
		vertices.Add(v3);

		triangles.Add(4*i);
		triangles.Add(4*i+1);
		triangles.Add(4*i+2);
		triangles.Add(4*i+2);
		triangles.Add(4*i+1);
		triangles.Add(4*i+3);

		FVector vNor = FVector::CrossProduct(v1 - v0, vDir);
		vNor.Normalize();
		normals.Add(vNor);
		normals.Add(vNor);
		normals.Add(vNor);
		normals.Add(vNor);

		uv0.Add(curUVs[0]);
		uv0.Add(curUVs[1]);
		uv0.Add(curUVs[2]);
		uv0.Add(curUVs[3]);

		//vertexColors.Add(FLinearColor(1.f, 0.f, 0.f));
		//vertexColors.Add(FLinearColor(1.f, 0.f, 0.f));
		//vertexColors.Add(FLinearColor(1.f, 0.f, 0.f));
		//vertexColors.Add(FLinearColor(1.f, 0.f, 0.f));

		if (i > 0)
		{
			verticesTop.Add(vertices[0]);
			verticesTop.Add(v1);
			verticesTop.Add(v0);
			verticesBot.Add(vertices[2]);
			verticesBot.Add(v2);
			verticesBot.Add(v3);

			normalsTop.Add(-vDir);
			normalsTop.Add(-vDir);
			normalsTop.Add(-vDir);
			normalsBot.Add(vDir);
			normalsBot.Add(vDir);
			normalsBot.Add(vDir);

			if (!bSubMesh)
			{
				uvScale.X = 1.f;
				uvScale.Y = 0.25f;
			}
			uvTop.Add(FVector2D((vertices[0].X - vMinPos.X) / sizeX * uvScale.X, (vertices[0].Y - vMinPos.Y) / sizeY * uvScale.Y));
			uvTop.Add(FVector2D((v1.X - vMinPos.X) / sizeX * uvScale.X, (v1.Y - vMinPos.Y) / sizeY * uvScale.Y));
			uvTop.Add(FVector2D((v0.X - vMinPos.X) / sizeX * uvScale.X, (v0.Y - vMinPos.Y) / sizeY * uvScale.Y));
			uvBot.Add(FVector2D((vertices[2].X - vMinPos.X) / sizeX * uvScale.X, (vertices[2].Y - vMinPos.Y) / sizeY * uvScale.Y) + (bSubMesh? 0.f : (mfUVSize[0] + mfUVSize[1])));
			uvBot.Add(FVector2D((v2.X - vMinPos.X) / sizeX * uvScale.X, (v2.Y - vMinPos.Y) / sizeY * uvScale.Y) + (bSubMesh ? 0.f : (mfUVSize[0] + mfUVSize[1])));
			uvBot.Add(FVector2D((v3.X - vMinPos.X) / sizeX * uvScale.X, (v3.Y - vMinPos.Y) / sizeY * uvScale.Y) + (bSubMesh ? 0.f : (mfUVSize[0] + mfUVSize[1])));
		}
	}
	//
	//Triangulate(verticesTop, trianglesTop);
	//Triangulate(verticesBot, trianglesBot);

	const int mateCnt = vMaterial.Num();
	if (bSubMesh)
	{
		const int triCnt = verticesTop.Num() / 3;
		for (int i = 0; i < triCnt; i++)
		{
			trianglesTop.Add(3 * i);
			trianglesTop.Add(3 * i + 1);
			trianglesTop.Add(3 * i + 2);
			trianglesBot.Add(3 * i);
			trianglesBot.Add(3 * i + 1);
			trianglesBot.Add(3 * i + 2);
		}
		//
		UMaterialInterface* pMateTop = nullptr;
		UMaterialInterface* pMateBot = nullptr;
		if (mateCnt > 2)
		{
			pMateTop = vMaterial[1];
			pMateBot = vMaterial[2];
		}
		else if (mateCnt > 0)
		{
			pMateTop = pMateBot = vMaterial[mateCnt - 1];
		}
		UProceduralMeshComponent* pMesh = Cast<UProceduralMeshComponent>(mpMesh);
		pMesh->CreateMeshSection(muIndex, verticesTop, trianglesTop, normalsTop, uvTop, vertexColors, tangents, true);
		mpMesh->SetMaterial(muIndex++, pMateTop);
		pMesh->CreateMeshSection(muIndex, verticesBot, trianglesBot, normalsBot, uvBot, vertexColors, tangents, true);
		mpMesh->SetMaterial(muIndex++, pMateBot);
	}
	else
	{
		const int curVerIdx = vertices.Num();
		const int triCnt = verticesTop.Num() / 3 + verticesBot.Num() / 3;
		vertices.Append(verticesTop);
		vertices.Append(verticesBot);
		normals.Append(normalsTop);
		normals.Append(normalsBot);
		uv0.Append(uvTop);
		uv0.Append(uvBot);
		for (int i = 0; i < triCnt; i++)
		{
			triangles.Add(curVerIdx + 3 * i);
			triangles.Add(curVerIdx + 3 * i + 1);
			triangles.Add(curVerIdx + 3 * i + 2);
		}
	}
	//
	UProceduralMeshComponent* pMesh = Cast<UProceduralMeshComponent>(mpMesh);
	pMesh->CreateMeshSection(muIndex, vertices, triangles, normals, uv0, vertexColors, tangents, true);
	mpMesh->SetMaterial(muIndex++, ((mateCnt > 0) ? vMaterial[0] : nullptr));

	return true;
}

bool UDynaMesh::BuildLine(TArray<FVector> const& pointArray, UMaterialInterface* pMaterial, float fSize /* = 1.f */, bool bClosed /* = true */, FVector2D uvScale /* = FVector2D(1.f, 1.f) */)
{
	if (pointArray.Num() < 2 || fSize <= 0.f)// || uSegment < 3)
		return false;
	TArray<FVector> vPoint = pointArray;
	TArray<float> vLen;
	const int length = FilterInvalidPoint(&vPoint, &vLen);
	if (length <= 0.f || vLen.Num() < vPoint.Num())
		return false;
	if (uvScale.X <= 0.f)
		uvScale.X = 1.f;
	if (uvScale.Y <= 0.f)
		uvScale.Y = 1.f;
	//
	bClosed = (bClosed && vPoint.Num() > 2);
	//TArray<FVector> prePos, curPos;
	//prePos.Reset(4);
	//curPos.Reset(4);
	FVector prePos[4], curPos[4], preNor[4], curNor[4];
	FVector2D preUVs[4] = { FVector2D(0.f, 0.f), FVector2D(0.f, 0.25f * uvScale.Y), FVector2D(0.f, 0.5f * uvScale.Y), FVector2D(0.f, 1.f * uvScale.Y) };
	FVector2D curUVs[4] = { FVector2D(0.f, 0.f), FVector2D(0.f, 0.25f * uvScale.Y), FVector2D(0.f, 0.5f * uvScale.Y), FVector2D(0.f, 1.f * uvScale.Y) };

	FVector v0, v1, v2;
	if (vPoint.Num() == 2)
	{
		FVector tmpA, tmpB;
		v0 = vPoint[0];
		v1 = vPoint[1];

		FPlane plane(v0, v1 - v0);
		FVector vDir = v0-v1;
		if (vDir.Y == 0.f && vDir.Z == 0.f)
			vDir.Z += 1.f;
		else
			vDir.X += 1.f;
		vDir.Normalize();
		v2 = FMath::RayPlaneIntersection(v1, vDir, plane);
		tmpA = v2 - v0;
		tmpA.Normalize();
		tmpA *= fSize * 0.5f;
		tmpB = FVector::CrossProduct(tmpA, v1 - v0);
		tmpA.Normalize();
		tmpB.Normalize();
		prePos[0] = vPoint[0] + tmpA * fSize * 0.5f;
		prePos[1] = vPoint[0] + tmpB * fSize * 0.5f;
		prePos[2] = vPoint[0] - tmpA * fSize * 0.5f;
		prePos[3] = vPoint[0] - tmpB * fSize * 0.5f;

		preNor[0] = tmpA;
		preNor[1] = tmpB;
		preNor[2] = -tmpA;
		preNor[3] = -tmpB;
	}
	//
	TArray<FVector> vertices, verticesTop;
	TArray<int32>   triangles, trianglesTop;
	TArray<FVector> normals, normalsTop;
	TArray<FVector2D> uv0;
	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;
	int curVtx = 0;
	//
	FPlane plane;
	FVector vDir;
	bool bLine = false;
	const int cnt = vPoint.Num();
	for (int i = 1; i < cnt; i++)
	{
		bLine = false;
		curUVs[0].X = curUVs[1].X = curUVs[2].X = curUVs[3].X = vLen[i] / length * uvScale.X;
		if (i + 1 < cnt)
		{
			v0 = vPoint[i - 1];
			v1 = vPoint[i];
			v2 = vPoint[i + 1];
			FVector v10 = v0 - v1, v12 = v2 - v1;
			v10.Normalize();
			v12.Normalize();
			FVector tmpA = v10 + v12;
			FVector tmpB = FVector::CrossProduct(v10, v12);
			bLine = !(tmpA.Normalize() && tmpB.Normalize());
			//tmpA.Normalize();
			//tmpB.Normalize();

			curPos[0] = vPoint[i] + tmpA * fSize * 0.5f;
			curPos[1] = vPoint[i] + tmpB * fSize * 0.5f;
			curPos[2] = vPoint[i] - tmpA * fSize * 0.5f;
			curPos[3] = vPoint[i] - tmpB * fSize * 0.5f;

			curNor[0] = tmpA;
			curNor[1] = tmpB;
			curNor[2] = -tmpA;
			curNor[3] = -tmpB;

		}
		else // last pos
		{
			if (bClosed)
			{
				v0 = vPoint[i - 1];
				v1 = vPoint[i];
				v2 = vPoint[0];
				FVector v10 = v0 - v1, v12 = v2 - v1;
				v10.Normalize();
				v12.Normalize();
				FVector tmpA = v10 + v12;
				FVector tmpB = FVector::CrossProduct(v10, v12);
				bLine = !(tmpA.Normalize() && tmpB.Normalize());
				//tmpA.Normalize();
				//tmpB.Normalize();

				curPos[0] = vPoint[i] + tmpA * fSize * 0.5f;
				curPos[1] = vPoint[i] + tmpB * fSize * 0.5f;
				curPos[2] = vPoint[i] - tmpA * fSize * 0.5f;
				curPos[3] = vPoint[i] - tmpB * fSize * 0.5f;

				curNor[0] = tmpA;
				curNor[1] = tmpB;
				curNor[2] = -tmpA;
				curNor[3] = -tmpB;
			}
			else
			{
				vDir = vPoint[i] - vPoint[i - 1];
				vDir.Normalize();
				plane = FPlane(vPoint[i], vDir);
				curPos[0] = FMath::RayPlaneIntersection(prePos[0], vDir, plane);
				curPos[1] = FMath::RayPlaneIntersection(prePos[1], vDir, plane);
				curPos[2] = FMath::RayPlaneIntersection(prePos[2], vDir, plane);
				curPos[3] = FMath::RayPlaneIntersection(prePos[3], vDir, plane);

				curNor[0] = curPos[0] - vPoint[i];
				curNor[1] = curPos[1] - vPoint[i];
				curNor[2] = curPos[2] - vPoint[i];
				curNor[3] = curPos[3] - vPoint[i];
			}
		}

		if (i == 1)
		{
			if (cnt > 2)
			{
				if (bClosed)
				{
					v0 = vPoint[cnt - 1];
					v1 = vPoint[0];
					v2 = vPoint[1];
					FVector v10 = v0 - v1, v12 = v2 - v1;
					v10.Normalize();
					v12.Normalize();
					FVector tmpA = v10 + v12;
					FVector tmpB = FVector::CrossProduct(v10, v12);
					bLine = !(tmpA.Normalize() && tmpB.Normalize());
					//tmpA.Normalize();
					//tmpB.Normalize();

					prePos[0] = vPoint[0] + tmpA * fSize * 0.5f;
					prePos[1] = vPoint[0] + tmpB * fSize * 0.5f;
					prePos[2] = vPoint[0] - tmpA * fSize * 0.5f;
					prePos[3] = vPoint[0] - tmpB * fSize * 0.5f;

					preNor[0] = tmpA;
					preNor[1] = tmpB;
					preNor[2] = -tmpA;
					preNor[3] = -tmpB;
				}
				else
				{
					vDir = vPoint[0] - vPoint[1];
					vDir.Normalize();
					plane = FPlane(vPoint[0], vDir);
					prePos[0] = FMath::RayPlaneIntersection(curPos[0], vDir, plane);
					prePos[1] = FMath::RayPlaneIntersection(curPos[1], vDir, plane);
					prePos[2] = FMath::RayPlaneIntersection(curPos[2], vDir, plane);
					prePos[3] = FMath::RayPlaneIntersection(curPos[3], vDir, plane);

					preNor[0] = prePos[0] - vPoint[0];
					preNor[1] = prePos[1] - vPoint[0];
					preNor[2] = prePos[2] - vPoint[0];
					preNor[3] = prePos[3] - vPoint[0];
				}
			}

			//
			vertices.Add(prePos[0]);
			vertices.Add(prePos[1]);
			vertices.Add(prePos[2]);
			vertices.Add(prePos[3]);

			normals.Add(preNor[0]);
			normals.Add(preNor[1]);
			normals.Add(preNor[2]);
			normals.Add(preNor[3]);

			uv0.Add(preUVs[0]);
			uv0.Add(preUVs[1]);
			uv0.Add(preUVs[2]);
			uv0.Add(preUVs[3]);

			//
			if (!bClosed)
			{

			}
		}
		
		//
		if (bLine)
			continue;
		//
		vertices.Add(curPos[0]);
		vertices.Add(curPos[1]);
		vertices.Add(curPos[2]);
		vertices.Add(curPos[3]);
		//
		triangles.Add(curVtx + 0);
		triangles.Add(curVtx + 5);
		triangles.Add(curVtx + 4);
		triangles.Add(curVtx + 0);
		triangles.Add(curVtx + 1);
		triangles.Add(curVtx + 5);

		triangles.Add(curVtx + 1);
		triangles.Add(curVtx + 6);
		triangles.Add(curVtx + 5);
		triangles.Add(curVtx + 1);
		triangles.Add(curVtx + 2);
		triangles.Add(curVtx + 6);

		triangles.Add(curVtx + 2);
		triangles.Add(curVtx + 7);
		triangles.Add(curVtx + 6);
		triangles.Add(curVtx + 2);
		triangles.Add(curVtx + 3);
		triangles.Add(curVtx + 7);

		triangles.Add(curVtx + 3);
		triangles.Add(curVtx + 0);
		triangles.Add(curVtx + 7);
		triangles.Add(curVtx + 0);
		triangles.Add(curVtx + 4);
		triangles.Add(curVtx + 7);
		//
		normals.Add(curNor[0]);
		normals.Add(curNor[1]);
		normals.Add(curNor[2]);
		normals.Add(curNor[3]);
		//
		uv0.Add(curUVs[0]);
		uv0.Add(curUVs[1]);
		uv0.Add(curUVs[2]);
		uv0.Add(curUVs[3]);

		//
		if (i == cnt - 1)
		{
			if (bClosed && cnt > 2)
			{
				curVtx += 4;
				const int curVtx0 = -4;
				//
				triangles.Add(curVtx + 0);
				triangles.Add(curVtx0 + 5);
				triangles.Add(curVtx0 + 4);
				triangles.Add(curVtx + 0);
				triangles.Add(curVtx + 1);
				triangles.Add(curVtx0 + 5);

				triangles.Add(curVtx + 1);
				triangles.Add(curVtx0 + 6);
				triangles.Add(curVtx0 + 5);
				triangles.Add(curVtx + 1);
				triangles.Add(curVtx + 2);
				triangles.Add(curVtx0 + 6);

				triangles.Add(curVtx + 2);
				triangles.Add(curVtx0 + 7);
				triangles.Add(curVtx0 + 6);
				triangles.Add(curVtx + 2);
				triangles.Add(curVtx + 3);
				triangles.Add(curVtx0 + 7);

				triangles.Add(curVtx + 3);
				triangles.Add(curVtx + 0);
				triangles.Add(curVtx0 + 7);
				triangles.Add(curVtx + 0);
				triangles.Add(curVtx0 + 4);
				triangles.Add(curVtx0 + 7);
			}
			else if(!bClosed)
			{

			}
		}
		//
		prePos[0] = curPos[0];
		prePos[1] = curPos[1];
		prePos[2] = curPos[2];
		prePos[3] = curPos[3];

		//
		curVtx += 4;
	}
	//
	UProceduralMeshComponent* pMesh = Cast<UProceduralMeshComponent>(mpMesh);
	pMesh->CreateMeshSection(muIndex, vertices, triangles, normals, uv0, vertexColors, tangents, true);
	mpMesh->SetMaterial(muIndex++, pMaterial);

	return true;
}

bool UDynaMesh::BuildSurface(TArray<FVector> const& pointArray, UMaterialInterface* pMate /* = nullptr */, FVector2D uvScale /* = FVector2D(1.f, 1.f) */, FVector const& vDir /* = FVector(0.f, 0.f, -1.f) */, float fOffset /* = 0.f */)
{
	if (pointArray.Num() < 3)
		return false;
	TArray<FVector> vPoint = pointArray;
	TArray<float> vLen;
	const int length = FilterInvalidPoint(&vPoint, &vLen);
	FVector2D vMinPos, vMaxPos;
	CalcuSurfaceSize(vMinPos, vMaxPos, vPoint);
	const float sizeX = vMaxPos.X - vMinPos.X;
	const float sizeY = vMaxPos.Y - vMinPos.Y;
	//
	TArray<FVector> vertices = vPoint;
	TArray<int32>   triangles;
	TArray<FVector> normals;
	TArray<FVector2D> uv0;
	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;
	//
	FVector v0, v1, v2;
	v0 = vPoint[0];
	v1 = vPoint[1];
	v2 = vPoint[2];
	FVector nor = FVector::CrossProduct(v0 - v1, v2 - v1);
	nor.Normalize();
	//
	TArray<FVector2D> cgalPts;
	for (int i = 0; i < vPoint.Num(); i++)
	{
		cgalPts.Add(FVector2D(vPoint[i].X, vPoint[i].Y));
		normals.Add(nor);
		uv0.Add(FVector2D((vertices[i].X - vMinPos.X) / sizeX * uvScale.X, (vertices[i].Y - vMinPos.Y) / sizeY * uvScale.Y));
		if (fOffset > 0.f && !vDir.IsNearlyZero())
		{
			vertices[i] += vDir * fOffset;
		}
	}
	//
	Triangulate(vertices, triangles);
	//
	//const int cgalTri = cgalIdx.Num() / 3;
	//for (int i = 0; i < cgalTri; i++)
	//{
	//	normals.Add(nor);
	//	if (i >= 2)
	//	{
	//		triangles.Add(0);
	//		triangles.Add(i - 1);
	//		triangles.Add(i);

	//		//triangles.Add(0);
	//		//triangles.Add(i);
	//		//triangles.Add(i-1);
	//	}
	//	uv0.Add(FVector2D((vertices[i].X - vMinPos.X) / sizeX * uvScale.X, (vertices[i].Y - vMinPos.Y) / sizeY * uvScale.Y));
	//}
	//
	if (nullptr == pMate)
		pMate = mpMateDouble;
	UProceduralMeshComponent* pMesh = Cast<UProceduralMeshComponent>(mpMesh);
	pMesh->CreateMeshSection(muIndex, vertices, triangles, normals, uv0, vertexColors, tangents, true);
	mpMesh->SetMaterial(muIndex++, pMate);

	return true;
}

void UDynaMesh::Triangulate(TArray<FVector> & vVertices, TArray<int32> & vTriangles, bool bInvert /* = false */)
{
	//TArray<int> cgalIdx;
	//FPolygonAlg::PolygonTriangulate(cgalPts, triangles);

	UGameInstance* pInst = (GetWorld() ? GetWorld()->GetGameInstance() : nullptr);
	if (nullptr != pInst)
	{
		UBuildingSystem* pBuild = ((UCEditorGameInstance*)pInst)->GetBuildingSystemInstance();
		if (nullptr != pBuild)
		{
			IBuildingSDK *SDK = pBuild->GetBuildingSDK();
			IGeometry *pGeoLib = (SDK ? SDK->GetGeometryLibrary() : nullptr);
			if (pGeoLib)
			{
				std::vector<kPoint>	Shape;
				for (int i = 0; i < vVertices.Num(); i++)
				{
					int idx = (bInvert ? (vVertices.Num() - 1 - i) : i);
					Shape.push_back(kPoint(vVertices[idx].X, vVertices[idx].Y));
				}
				kArray<kPoint> TriVertices;
				kArray<int>	   Indices;
				pGeoLib->SimpleTriangulate(&Shape[0], (int)Shape.size() - 1, TriVertices, Indices);
				for (int i = 0; i < TriVertices.size() && i < vVertices.Num(); i++)
				{
					vVertices[i].X = TriVertices[i].X;
					vVertices[i].Y = TriVertices[i].Y;
					//vVertices.Add(FVector(TriVertices[i].X, TriVertices[i].Y, 0.f));
				}
				vTriangles.Empty();
				for (int i = 0; i < Indices.size(); i++)
				{
					vTriangles.Add(Indices[i]);
				}
			}
		}
	}
	
}

bool UDynaMesh::BuildSurfaceBK(TArray<FVector> const& pointArray, UMaterialInterface* pMate /* = nullptr */, FVector2D uvScale /* = FVector2D(1.f, 1.f) */)
{
	if (pointArray.Num() < 3)
		return false;
	TArray<FVector> vPoint = pointArray;
	TArray<float> vLen;
	const int length = FilterInvalidPoint(&vPoint, &vLen);
	FVector2D vMinPos, vMaxPos;
	CalcuSurfaceSize(vMinPos, vMaxPos, vPoint);
	const float sizeX = vMaxPos.X - vMinPos.X;
	const float sizeY = vMaxPos.Y - vMinPos.Y;
	//
	TArray<FVector> vertices = vPoint;
	TArray<int32>   triangles;
	TArray<FVector> normals;
	TArray<FVector2D> uv0;
	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;
	//
	FVector v0, v1, v2;
	v0 = vPoint[0];
	v1 = vPoint[1];
	v2 = vPoint[2];
	FVector nor = FVector::CrossProduct(v0 - v1, v2 - v1);
	nor.Normalize();
	for (int i = 0; i < vPoint.Num(); i++)
	{
		normals.Add(nor);
		if (i >= 2)
		{
			triangles.Add(0);
			triangles.Add(i-1);
			triangles.Add(i);

			//triangles.Add(0);
			//triangles.Add(i);
			//triangles.Add(i-1);
		}
		uv0.Add(FVector2D((vertices[i].X - vMinPos.X) / sizeX * uvScale.X, (vertices[i].Y - vMinPos.Y) / sizeY * uvScale.Y));
	}
	//
	if (nullptr == pMate)
		pMate = mpMateDouble;
	UProceduralMeshComponent* pMesh = Cast<UProceduralMeshComponent>(mpMesh);
	pMesh->CreateMeshSection(muIndex, vertices, triangles, normals, uv0, vertexColors, tangents, true);
	mpMesh->SetMaterial(muIndex++, pMate);

	return true;
}

int UDynaMesh::FilterInvalidPoint(TArray<FVector>* vPoint, TArray<float> * vLen) const
{
	if (nullptr == vPoint || vPoint->Num() < 2 || nullptr == vLen)
		return 0;
	//
	vLen->Empty();
	vLen->Add(0.f);
	int len = 0;
	for (int i = 1; i < vPoint->Num(); i++)
	{
		FVector v0, v1, v2;
		v0 = (*vPoint)[i - 1];
		v1 = (*vPoint)[i];
		if (v1 == v0)
		{
			vPoint->RemoveAt(i--);
			continue;
		}
		if (i + 1 < vPoint->Num())
		{
			v2 = (*vPoint)[i + 1];
			v0 = v0 - v1;
			v2 = v2 - v1;
			v0.Normalize();
			v2.Normalize();
			const float dot = FVector::DotProduct(v0, v2);
			if (dot == -1.f)
			{
				vPoint->RemoveAt(i--);
				continue;
			}
			if (dot == 1.f)
			{
				vPoint->RemoveAt(--i);
				continue;
			}
		}
		len += FVector::Dist((*vPoint)[i], (*vPoint)[i-1]);
		vLen->Add(len);
	}
	return len;
}

bool UDynaMesh::CalcuSurfaceSize(FVector2D & minPos, FVector2D & maxPos, TArray<FVector> const& pointArray) const
{
	if (pointArray.Num() < 2)
		return false;
	minPos.X = maxPos.X = pointArray[0].X;
	minPos.Y = maxPos.Y = pointArray[0].Y;
	for (int i = 1; i < pointArray.Num(); i++)
	{
		if (pointArray[i].X < minPos.X)
			minPos.X = pointArray[i].X;
		if (pointArray[i].Y < minPos.Y)
			minPos.Y = pointArray[i].Y;
		if (pointArray[i].X > maxPos.X)
			maxPos.X = pointArray[i].X;
		if (pointArray[i].Y > maxPos.Y)
			maxPos.Y = pointArray[i].Y;
	}
	return true;
}

void UDynaMesh::Show(bool bShow, int uSection /* = -1 */)
{
	if (nullptr != mpMesh)
	{
		UProceduralMeshComponent* pMesh = Cast<UProceduralMeshComponent>(mpMesh);
		if(uSection >= 0)
			pMesh->SetMeshSectionVisible(uSection, bShow);
		else
		{
			int cnt = pMesh->GetNumSections();
			for (int i = 0; i < cnt; i++)
				pMesh->SetMeshSectionVisible(i, bShow);
		}
	}
}

void UDynaMesh::Clear()
{
	if (nullptr != mpMesh)
	{
		UProceduralMeshComponent* pMesh = Cast<UProceduralMeshComponent>(mpMesh);
		pMesh->ClearAllMeshSections();
	}
}
//bool UDynaMesh::BuildLine(TArray<FVector> const& vPoint, float fSize /* = 1.f */, bool bClosed /* = false */, int uSegment /* = 4 */)
//{
//	if (vPoint.Num() < 2 || fSize <= 0.f || uSegment < 3)
//		return false;
//	TArray<FVector> prePos, posA, posB;
//	FPlane plane(vPoint[0], vPoint[1] - vPoint[0]);
//	FVector v0(vPoint[0]), v1(vPoint[1]), v2;// , tmp(vPoint[1] - vPoint[0]);
//	if (vPoint.Num() > 2)
//		v2 = vPoint[2];
//	else
//	{
//		v2 = v1 + (v1 - v0);
//		if (v2.Y == 0.f && v2.Z == 0.f)
//			v2.Z += 1.f;
//		else
//			v2.X += 1.f;
//	}
//	FVector v10 = v0 - v1, v12 = v2 - v1;
//	v10.Normalize();
//	v12.Normalize();
//	FVector tmpA = v10 + v12;
//	tmpA.Normalize();
//	FVector tmpB = FVector::CrossProduct(v10,v12);
//	tmpB.Normalize();
//	prePos.Empty();
//	prePos.Add(tmpA * fSize * 0.5f);
//	//prePos.Add(tmpB * fSize * 0.5f);
//	prePos.Add(-tmpA * fSize * 0.5f);
//	//prePos.Add(-tmpB * fSize * 0.5f);
//	//
//	for (int i = 0; i < prePos.Num(); i++)
//	{
//		FVector v = FMath::RayPlaneIntersection(prePos[i], vPoint[0] - vPoint[1], plane);
//		if (i % 2 == 0)
//			posA.Add(v);
//		else
//			posB.Add(v);
//	}
//	
//	const int cnt = vPoint.Num();
//	if (cnt == 2)
//	{
//		for (int i = 0; i < prePos.Num(); i++)
//		{
//			if (i % 2 == 0)
//				posA.Add(prePos[i]);
//			else
//				posB.Add(prePos[i]);
//			//curPos.Add(prePos[i]);
//		}
//	}
//	else
//	{
//		for (int i = 1; i < cnt-1; i++)
//		{
//			v10 = vPoint[i - 1] - vPoint[i];
//			v12 = vPoint[i + 1] - vPoint[i];
//			v10.Normalize();
//			v12.Normalize();
//			tmpA = v10 + v12;
//			tmpA.Normalize();
//			//
//			posA.Add(tmpA * fSize * 0.5f);
//			posB.Add(-tmpA * fSize * 0.5f);
//		}
//	}
//	TArray<FVector> vPolygon;
//	if (posA.Num() > 0 && posA.Num() == posB.Num())
//	{
//		vPolygon = posA;
//		for (int i = posB.Num() - 1; i >= 0; i--)
//			vPolygon.Add(posB[i]);
//	}
//	BuildMesh(vPolygon);
//	return true;
//}

bool UDynaMesh::isConvexPolygon(TArray<FVector> const& vPoint) const
{
	if (vPoint.Num() < 3)
		return false;
	bool bConvex = false;
	FVector vPre, vNext;
	const int cnt = vPoint.Num();
	for (int i = 0; i < cnt; i++)
	{
		vPre  = (i >= 1) ? vPoint[i - 1] : vPoint[cnt - 1];
		vNext = vPoint[(i + 1) % cnt];//(i < cnt - 1) ? vPoint[i + 1] : vPoint[0];
	}
	return bConvex;
}

void UDynaMesh::MeshDrawRect(FVector2D Position, FVector2D Size, FLinearColor color)
{
	TArray<FVector> vertices;
	vertices.Add(FVector(0, Position.X, Position.Y));
	vertices.Add(FVector(0, Position.X + Size.X, Position.Y));
	vertices.Add(FVector(0, Position.X + Size.X, Position.Y + Size.Y));

	vertices.Add(FVector(0, Position.X, Position.Y));
	vertices.Add(FVector(0, Position.X + Size.X, Position.Y + Size.Y));
	vertices.Add(FVector(0, Position.X, Position.Y + Size.Y));

	TArray<int32> Triangles;
	for (int i = 0; i < vertices.Num(); i++)
	{
		Triangles.Add(i);
	}

	TArray<FVector> normals;
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;

	TArray<FLinearColor> vertexColors;
	for (int i = 0; i < vertices.Num(); i++)
	{
		vertexColors.Add(color);
	}

	UProceduralMeshComponent* pMesh = Cast<UProceduralMeshComponent>(mpMesh);
	//if (mbIsRotate)
	//{
	//	//mpMesh->CreateMeshSection_LinearColor(muIndex, MeshRotation(vertices, m_centerRotate, m_degreeRotate), Triangles, normals, UV0, vertexColors, tangents, true);
	//}
	//else
	{
		pMesh->CreateMeshSection_LinearColor(muIndex, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	}
	mpMesh->SetMaterial(muIndex++, mpMate);

}

void UDynaMesh::MeshDrawCircle(FVector2D center, int radius, int NumSegments, FLinearColor color)
{
	if (NumSegments == 0)
		NumSegments = 360.0;
	float SegmentLength = 360.0 / NumSegments;

	TArray<FVector> vertices;
	for (float i = 0; i < 360.0; i += SegmentLength)
	{
		FVector2D PositionA(cos(i * PI / 180.0) * radius + center.X, sin(i * PI / 180.0) * radius + center.Y);
		FVector2D PositionB(cos((i + SegmentLength) * PI / 180.0) * radius + center.X, sin((i + SegmentLength) * PI / 180.0) * radius + center.Y);
		vertices.Add(FVector(0, center.X, center.Y));
		vertices.Add(FVector(0, PositionA.X, PositionA.Y));
		vertices.Add(FVector(0, PositionB.X, PositionB.Y));
	}

	TArray<int32> Triangles;
	for (int i = 0; i < vertices.Num(); i++)
	{
		Triangles.Add(i);
	}

	TArray<FVector> normals;
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;

	TArray<FLinearColor> vertexColors;
	for (int i = 0; i < vertices.Num(); i++)
	{
		vertexColors.Add(color);
	}

	UProceduralMeshComponent* pMesh = Cast<UProceduralMeshComponent>(mpMesh);
	pMesh->CreateMeshSection_LinearColor(muIndex, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	mpMesh->SetMaterial(muIndex++, mpMate);

}

UMeshComponent* UDynaMesh::MeshComponent()
{
	return mpMesh;
}

#pragma optimize( "", on )