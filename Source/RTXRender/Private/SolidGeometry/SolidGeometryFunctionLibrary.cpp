 
#pragma once

#include "SolidGeometry/SolidGeometryFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "GeomTools.h"
#include "ProcMeshFunctionLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "HomeLayout/DataNodes/NodeTypes.h"
#include "Kismet/KismetSystemLibrary.h"


void USolidGeometryFunctionLibrary::TwoLineMinDistance(UObject* WorldContextObject,FVector Line1Point, FVector Line1Direction, FVector Line2Point, FVector Line2Direction, FVector &out_PointOfIntersection, FVector &out_1To2Direction, float &out_Distance,bool Debug)
{
	FVector F = Line1Point - Line2Point;
	float C1L = FVector::DotProduct(F, Line1Direction.GetSafeNormal());
	float C2L = FVector::DotProduct(F, Line2Direction.GetSafeNormal());
	float C21L = FVector::DotProduct(Line1Direction.GetSafeNormal(), Line2Direction.GetSafeNormal());
	float n = ((C2L - (C1L*C21L)) / (1 - FMath::Pow(C21L, 2)));
	float m = ((n*C21L) - C1L);
	FVector nV = Line2Point + Line2Direction*n;
	FVector mV = Line1Point + Line1Direction*m;
	out_PointOfIntersection = (( nV+ mV) / 2);
	out_1To2Direction = ((mV - nV).GetSafeNormal());
	out_Distance = ((mV - nV).Size());

	if (Debug)
	{
		UKismetSystemLibrary::DrawDebugPoint(WorldContextObject, mV, 20, FLinearColor(1, 0.1, 0, 1));//M
		UKismetSystemLibrary::DrawDebugPoint(WorldContextObject, nV, 20, FLinearColor(0, 0.2, 1, 1));//N
		UKismetSystemLibrary::DrawDebugPoint(WorldContextObject, out_PointOfIntersection, 20, FLinearColor(0, 1, 0, 1));//交点
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, mV, nV, FLinearColor(0.7, 0.5, 0, 1), 0, 0);//两交点线
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, Line1Point, (Line1Point + (Line1Direction * 100000)), FLinearColor(1, 0, 0, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, Line1Point, (Line1Point + (Line1Direction * -100000)), FLinearColor(1, 0, 0, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, Line2Point, (Line2Point + (Line2Direction * 100000)), FLinearColor(0, 0, 1, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, Line2Point, (Line2Point + (Line2Direction * -100000)), FLinearColor(0, 0, 1, 1), 0, 0);
	}
}

void USolidGeometryFunctionLibrary::TwoLineMinDistance_StraightLine(UObject* WorldContextObject, FVector Line1StartPoint, FVector Line1EndPoint, FVector Line2StartPoint, FVector Line2EndPoint, FVector &out_PointOfIntersection, FVector &out_1To2Direction, float &out_Distance, bool Debug)
{
	TwoLineMinDistance(WorldContextObject, (Line1StartPoint + Line1EndPoint) / 2, (Line1StartPoint - Line1EndPoint).GetSafeNormal(), (Line2StartPoint + Line2EndPoint) / 2, (Line2StartPoint - Line2EndPoint).GetSafeNormal(), out_PointOfIntersection, out_1To2Direction, out_Distance, Debug);
}


FVector USolidGeometryFunctionLibrary::GetFourPointCenter(UObject* WorldContextObject,FVector Point1, FVector Point2, FVector Point3, FVector Point4,bool Debug)
{

	FVector out_1To2Direction;
	float out_Distance;

	FVector X1;
	FVector X2;

	TwoLineMinDistance_StraightLine(WorldContextObject, (Point1 + Point2) / 2, Point3, (Point2 + Point3) / 2, Point1, X1, out_1To2Direction, out_Distance, false);
	TwoLineMinDistance_StraightLine(WorldContextObject, (Point1 + Point3) / 2, Point4, (Point3 + Point4) / 2, Point1, X2, out_1To2Direction, out_Distance, false);
    
	if (Debug)
	{
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, Point1, Point2, FLinearColor(1, 0, 0, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, Point2, Point3, FLinearColor(1, 0, 0, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, Point3, Point4, FLinearColor(1, 0, 0, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, Point4, Point1, FLinearColor(1, 0, 0, 1), 0, 0);

		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, Point1, Point3, FLinearColor(1, 0, 0, 1), 0, 0);

		UKismetSystemLibrary::DrawDebugPoint(WorldContextObject, (X1 + X2) / 2, 20, FLinearColor(1, 0.1, 0, 1));//M

		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, (Point1 + Point2) / 2, Point3, FLinearColor(0, 0, 1, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, (Point2 + Point3) / 2, Point1, FLinearColor(0, 0, 1, 1), 0, 0);

		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, (Point1 + Point3) / 2, Point4, FLinearColor(0, 0, 1, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, (Point3 + Point4) / 2, Point1, FLinearColor(0, 0, 1, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, (Point1 + Point3) / 2, Point2, FLinearColor(0, 0, 1, 1), 0, 0);
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, (Point1 + Point4) / 2, Point3, FLinearColor(0, 0, 1, 1), 0, 0);
	}
	return (X1 + X2) / 2;
 }


//计算法线
FVector USolidGeometryFunctionLibrary::SurfaceNormal(USceneComponent* Ver1, USceneComponent* Ver2, USceneComponent* Ver3)
{
	FVector r1;
	FVector r2;
	FRotator R;
	FVector ReturnValue;

	r1 = Ver1->GetComponentLocation() - Ver3->GetComponentLocation();
	r2 = Ver2->GetComponentLocation() - Ver3->GetComponentLocation();
	R = UKismetMathLibrary::Cross_VectorVector(r1, r2).Rotation();

	ReturnValue = UKismetMathLibrary::Conv_RotatorToVector(R);

	return ReturnValue;
}


//计算法线
FVector USolidGeometryFunctionLibrary::SurfaceNormal_Location(FVector Ver1, FVector Ver2, FVector Ver3)
{
	FVector r1;
	FVector r2;
	FRotator R;
	FVector ReturnValue;

	r1 = Ver1 - Ver3;

	r2 = Ver2 - Ver3;

	R = UKismetMathLibrary::Cross_VectorVector(r1, r2).Rotation();

	ReturnValue = UKismetMathLibrary::Conv_RotatorToVector(R);

	return ReturnValue;
}

void USolidGeometryFunctionLibrary::spawnbrush(UObject* WorldContextObject)
{
	FActorSpawnParameters SpawnInfo;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	ABrush* sactor = World->SpawnActor<ABrush>(FVector(0,0,0), FRotator(0,0,0), SpawnInfo);
	sactor->GetRootComponent()->SetMobility(EComponentMobility::Movable);
	sactor->BrushType = EBrushType::Brush_Default;
	sactor->GetRootComponent()->UpdateBounds();
 

}



TArray<FProcMeshVertex> FOnceGeometryToProcMeshVertex(TArray<FOnceGeometry> Config)
{
	TArray<FProcMeshVertex> returnvertex;

	FProcMeshVertex AddConfig;

	for (int i = 0; i < Config.Num(); i++)
	{

		AddConfig.Position = Config[i].Vertices;
		AddConfig.Normal = Config[i].Normals;
		AddConfig.Tangent = Config[i].Tangents;
		AddConfig.UV0 = Config[i].UV0;

		returnvertex.Add(AddConfig);
	}
	return returnvertex;
}


/** Given a polygon, decompose into triangles. */
bool USolidGeometryFunctionLibrary::TriangulatePoly(TArray<int32>& OutTris, const TArray<FOnceGeometry>& PolyVertList, int32 VertBase, const FVector& PolyNormal)
{

	TArray<FProcMeshVertex> PolyVerts;
	PolyVerts = FOnceGeometryToProcMeshVertex(PolyVertList);

	// Can't work if not enough verts for 1 triangle
	int32 NumVerts = PolyVerts.Num() - VertBase;
	if (NumVerts < 3)
	{
		OutTris.Add(0);
		OutTris.Add(2);
		OutTris.Add(1);

		// Return true because poly is already a tri
		return true;
	}

	// Remember initial size of OutTris, in case we need to give up and return to this size
	const int32 TriBase = OutTris.Num();

	// Init array of vert indices, in order. We'll modify this
	TArray<int32> VertIndices;
	VertIndices.AddUninitialized(NumVerts);
	for (int VertIndex = 0; VertIndex < NumVerts; VertIndex++)
	{
		VertIndices[VertIndex] = VertBase + VertIndex;
	}

	// Keep iterating while there are still vertices
	while (VertIndices.Num() >= 3)
	{
		// Look for an 'ear' triangle
		bool bFoundEar = false;
		for (int32 EarVertexIndex = 0; EarVertexIndex < VertIndices.Num(); EarVertexIndex++)
		{
			// Triangle is 'this' vert plus the one before and after it
			const int32 AIndex = (EarVertexIndex == 0) ? VertIndices.Num() - 1 : EarVertexIndex - 1;
			const int32 BIndex = EarVertexIndex;
			const int32 CIndex = (EarVertexIndex + 1) % VertIndices.Num();

			const FProcMeshVertex& AVert = PolyVerts[VertIndices[AIndex]];
			const FProcMeshVertex& BVert = PolyVerts[VertIndices[BIndex]];
			const FProcMeshVertex& CVert = PolyVerts[VertIndices[CIndex]];

			// Check that this vertex is convex (cross product must be positive)
			const FVector ABEdge = BVert.Position - AVert.Position;
			const FVector ACEdge = CVert.Position - AVert.Position;
			const float TriangleDeterminant = (ABEdge ^ ACEdge) | PolyNormal;
			if (TriangleDeterminant > 0.f)
			{
				continue;
			}

			bool bFoundVertInside = false;
			// Look through all verts before this in array to see if any are inside triangle
			for (int32 VertexIndex = 0; VertexIndex < VertIndices.Num(); VertexIndex++)
			{
				const FProcMeshVertex& TestVert = PolyVerts[VertIndices[VertexIndex]];

				if (VertexIndex != AIndex &&
					VertexIndex != BIndex &&
					VertexIndex != CIndex &&
					FGeomTools::PointInTriangle(AVert.Position, BVert.Position, CVert.Position, TestVert.Position))
				{
					bFoundVertInside = true;
					break;
				}
			}

			// Triangle with no verts inside - its an 'ear'! 
			if (!bFoundVertInside)
			{
				OutTris.Add(VertIndices[AIndex]);
				OutTris.Add(VertIndices[CIndex]);
				OutTris.Add(VertIndices[BIndex]);

				// And remove vertex from polygon
				VertIndices.RemoveAt(EarVertexIndex);

				bFoundEar = true;
				break;
			}
		}

		// If we couldn't find an 'ear' it indicates something is bad with this polygon - discard triangles and return.
		if (!bFoundEar)
		{
			OutTris.SetNum(TriBase, true);
			return false;
		}
	}

	return true;
}


/** Given a polygon, decompose into triangles. */
bool CTriangulatePoly(TArray<uint32>& OutTris, const TArray<FProcMeshVertex>& PolyVerts, int32 VertBase, const FVector& PolyNormal)
{
	// Can't work if not enough verts for 1 triangle
	int32 NumVerts = PolyVerts.Num() - VertBase;
	if (NumVerts < 3)
	{
		OutTris.Add(0);
		OutTris.Add(2);
		OutTris.Add(1);

		// Return true because poly is already a tri
		return true;
	}

	// Remember initial size of OutTris, in case we need to give up and return to this size
	const int32 TriBase = OutTris.Num();

	// Init array of vert indices, in order. We'll modify this
	TArray<int32> VertIndices;
	VertIndices.AddUninitialized(NumVerts);
	for (int VertIndex = 0; VertIndex < NumVerts; VertIndex++)
	{
		VertIndices[VertIndex] = VertBase + VertIndex;
	}

	// Keep iterating while there are still vertices
	while (VertIndices.Num() >= 3)
	{
		// Look for an 'ear' triangle
		bool bFoundEar = false;
		for (int32 EarVertexIndex = 0; EarVertexIndex < VertIndices.Num(); EarVertexIndex++)
		{
			// Triangle is 'this' vert plus the one before and after it
			const int32 AIndex = (EarVertexIndex == 0) ? VertIndices.Num() - 1 : EarVertexIndex - 1;
			const int32 BIndex = EarVertexIndex;
			const int32 CIndex = (EarVertexIndex + 1) % VertIndices.Num();

			const FProcMeshVertex& AVert = PolyVerts[VertIndices[AIndex]];
			const FProcMeshVertex& BVert = PolyVerts[VertIndices[BIndex]];
			const FProcMeshVertex& CVert = PolyVerts[VertIndices[CIndex]];

			// Check that this vertex is convex (cross product must be positive)
			const FVector ABEdge = BVert.Position - AVert.Position;
			const FVector ACEdge = CVert.Position - AVert.Position;
			const float TriangleDeterminant = (ABEdge ^ ACEdge) | PolyNormal;
			if (TriangleDeterminant > 0.f)
			{
				continue;
			}

			bool bFoundVertInside = false;
			// Look through all verts before this in array to see if any are inside triangle
			for (int32 VertexIndex = 0; VertexIndex < VertIndices.Num(); VertexIndex++)
			{
				const FProcMeshVertex& TestVert = PolyVerts[VertIndices[VertexIndex]];

				if (VertexIndex != AIndex &&
					VertexIndex != BIndex &&
					VertexIndex != CIndex &&
					FGeomTools::PointInTriangle(AVert.Position, BVert.Position, CVert.Position, TestVert.Position))
				{
					bFoundVertInside = true;
					break;
				}
			}

			// Triangle with no verts inside - its an 'ear'! 
			if (!bFoundVertInside)
			{
				OutTris.Add(VertIndices[AIndex]);
				OutTris.Add(VertIndices[CIndex]);
				OutTris.Add(VertIndices[BIndex]);

				// And remove vertex from polygon
				VertIndices.RemoveAt(EarVertexIndex);

				bFoundEar = true;
				break;
			}
		}

		// If we couldn't find an 'ear' it indicates something is bad with this polygon - discard triangles and return.
		if (!bFoundEar)
		{
			OutTris.SetNum(TriBase, true);
			return false;
		}
	}

	return true;
}

/** Transform triangle from 2D to 3D static-mesh triangle. */
void STransform2DPolygonTo3D(const FUtilPoly2D& InPoly, const FMatrix& InMatrix, TArray<FProcMeshVertex>& OutVerts, FBox& OutBox)
{
	FVector PolyNormal = -InMatrix.GetUnitAxis(EAxis::Z);
	FProcMeshTangent PolyTangent(InMatrix.GetUnitAxis(EAxis::X), false);

	for (int32 VertexIndex = 0; VertexIndex < InPoly.Verts.Num(); VertexIndex++)
	{
		const FUtilVertex2D& InVertex = InPoly.Verts[VertexIndex];

		FProcMeshVertex NewVert;

		NewVert.Position = InMatrix.TransformPosition(FVector(InVertex.Pos.X, InVertex.Pos.Y, 0.f));
		NewVert.Normal = PolyNormal;
		NewVert.Tangent = PolyTangent;
		NewVert.Color = InVertex.Color;
		NewVert.UV0 = InVertex.UV;

		OutVerts.Add(NewVert);

		// Update bounding box
		OutBox += NewVert.Position;
	}
}
int32 SBoxPlaneCompare(FBox InBox, const FPlane& InPlane)
{
	FVector BoxCenter, BoxExtents;
	InBox.GetCenterAndExtents(BoxCenter, BoxExtents);

	// Find distance of box center from plane
	float BoxCenterDist = InPlane.PlaneDot(BoxCenter);

	// See size of box in plane normal direction
	float BoxSize = FVector::BoxPushOut(InPlane, BoxExtents);

	if (BoxCenterDist > BoxSize)
	{
		return 1;
	}
	else if (BoxCenterDist < -BoxSize)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

/** Take two static mesh verts and interpolate all values between them */
FProcMeshVertex SInterpolateVert(const FProcMeshVertex& V0, const FProcMeshVertex& V1, float Alpha)
{
	FProcMeshVertex Result;

	// Handle dodgy alpha
	if (FMath::IsNaN(Alpha) || !FMath::IsFinite(Alpha))
	{
		Result = V1;
		return Result;
	}

	Result.Position = FMath::Lerp(V0.Position, V1.Position, Alpha);

	Result.Normal = FMath::Lerp(V0.Normal, V1.Normal, Alpha);

	Result.Tangent.TangentX = FMath::Lerp(V0.Tangent.TangentX, V1.Tangent.TangentX, Alpha);
	Result.Tangent.bFlipTangentY = V0.Tangent.bFlipTangentY; // Assume flipping doesn't change along edge...

	Result.UV0 = FMath::Lerp(V0.UV0, V1.UV0, Alpha);

	Result.Color.R = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.R), float(V1.Color.R), Alpha)), 0, 255);
	Result.Color.G = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.G), float(V1.Color.G), Alpha)), 0, 255);
	Result.Color.B = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.B), float(V1.Color.B), Alpha)), 0, 255);
	Result.Color.A = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.A), float(V1.Color.A), Alpha)), 0, 255);

	return Result;
}

FProcMeshConfig ProcMeshSectionToProcMeshConfig(FProcMeshSection screen)
{
	FProcMeshConfig tempproc;
	FProcMeshSection Section= screen;//定义Section

	FProcMeshVertex* ModifyVert = 0;//定义ModifyVert并初始化(顶点信息数组)
	uint32* Triang = 0;//定义Triang并初始化(三角面数组)

	int32 VerticeNum = Section.ProcVertexBuffer.Num(); //获得顶点数量
	int32 TrianglesNum = Section.ProcIndexBuffer.Num();//获得三角面数量

													   //初始化数组数量
	tempproc.Vertices.SetNum(VerticeNum, false);
	tempproc.Normals.SetNum(VerticeNum, false);
	tempproc.UV0.SetNum(VerticeNum, false);
	tempproc.Tangents.SetNum(VerticeNum, false);
	tempproc.Triangles.SetNum(TrianglesNum, false);

	//设置Vertices数组信息
	for (int LoopID = 0; LoopID < VerticeNum; LoopID = LoopID++)
	{
		ModifyVert = &Section.ProcVertexBuffer[LoopID]; //对ModifyVert赋指定ID的值
		//设置数组id的值
		tempproc.Vertices[LoopID] = ModifyVert->Position;
		tempproc.Normals[LoopID] = ModifyVert->Normal;
		tempproc.UV0[LoopID] = ModifyVert->UV0;
		tempproc.Tangents[LoopID] = ModifyVert->Tangent;
	}

	//设置Triangles数组信息
	for (int ID = 0; ID < TrianglesNum; ID = ID++)
	{
		Triang = &Section.ProcIndexBuffer[ID]; //对Triang赋指定ID的值
		tempproc.Triangles[ID] = *Triang;//设置数组id的值
	}

	return tempproc;
}

FBlueprintBox USolidGeometryFunctionLibrary::GetProcMeshBox(int32 Index ,UProceduralMeshComponent* ProceduralMesh)
{
	FBlueprintBox box;

	FProcMeshSection* screen = ProceduralMesh->GetProcMeshSection(Index);
	if (screen)
	{
		box.Min = screen->SectionLocalBox.Min;
		box.Max = screen->SectionLocalBox.Max;
		box.IsValid = screen->SectionLocalBox.IsValid;
	}
	
	return  box;
}

void USolidGeometryFunctionLibrary::TransformToPlane(const FTransform& ActorLocation,FVector Location, FRotator Rotation, FVector& OutLocation, FVector& OutNormal)
{

	FTransform ProcCompToWorld= ActorLocation;

	FVector LocalPlanePos = ProcCompToWorld.InverseTransformPosition(Location);
	FVector LocalPlaneNormal = ProcCompToWorld.InverseTransformVectorNoScale(Rotation.Vector());
	LocalPlaneNormal = LocalPlaneNormal.GetSafeNormal(); // Ensure normalized

	OutLocation = LocalPlanePos;
	OutNormal = LocalPlaneNormal;

}


void USolidGeometryFunctionLibrary::SliceMesh(UObject* WorldContextObject, FBlueprintBox Box, FProcMeshConfig InConfig, FVector PlanePosition, FVector PlaneNormal,
	bool bCreateOtherHalf, bool bCreateCap, FProcMeshConfig& OutBaseConfig, bool& BaseValid, FProcMeshConfig& OutOtherConfig, bool& OtherValid, FProcMeshConfig& OutCapConfig)
{
	//切片平面
	FPlane SlicePlane(PlanePosition, PlaneNormal);
	//另一半的参数
	TArray<FProcMeshSection> OtherSections;
	FProcMeshSection* BaseSection = new FProcMeshSection();  //基础参数  // = InProcMesh->GetProcMeshSection(SectionIndex);//基础
	// Set of new edges created by clipping polys by plane
	TArray<FUtilEdge3D> ClipEdges;


	//添加顶点信息到BaseSection
	for (int i = 0; i <= InConfig.Vertices.Num() - 1; i++)
	{
		FProcMeshVertex C;
		C.Position = InConfig.Vertices[i];
		C.Normal = InConfig.Normals[i];
		C.Tangent = InConfig.Tangents[i];
		C.Color = FColor(0, 0, 0, 0);
		C.UV0 = InConfig.UV0[i];
		BaseSection->ProcVertexBuffer.Add(C);
	}
	//添加面信息到BaseSection
	for (int i = 0; i <= InConfig.Triangles.Num() - 1; i++)
	{
		BaseSection->ProcIndexBuffer.Add(InConfig.Triangles[i]);
	}

	//设置边框信息
	BaseSection->SectionLocalBox.Min = Box.Min;
	BaseSection->SectionLocalBox.Max = Box.Max;
	BaseSection->SectionLocalBox.IsValid = Box.IsValid;


	// If we have a section, and it has some valid geom
	if (BaseSection != nullptr && BaseSection->ProcIndexBuffer.Num() > 0 && BaseSection->ProcVertexBuffer.Num() > 0)
	{

		//交错状态Compare bounding box of section with slicing plane
		int32 BoxCompare = SBoxPlaneCompare(BaseSection->SectionLocalBox, SlicePlane);

		//判断交错状态—被完全修剪
		if (BoxCompare == -1)
		{
			// 将整个部分添加到另一半
			if (bCreateOtherHalf)
			{
				OtherValid = true;
				OtherSections.Add(*BaseSection);
			}
			else
			{
				OtherValid = false;
			}
			BaseValid = false;
			///InProcMesh->ClearMeshSection(SectionIndex); //清除全部section
		}
		//判断交错状态—完全在切片平面的另一边
		else if (BoxCompare == 1)
		{
			//直接输出输入参数
			OutBaseConfig = InConfig;
			BaseValid = true;
			OtherValid = false;
		}
		//判断交错状态—相交
		else
		{
			// 新的Section
			FProcMeshSection NewSection;

			// 另一半Section ，New section for 'other half' geometry (if desired)
			FProcMeshSection* NewOtherSection = nullptr;

			if (bCreateOtherHalf)
			{
				int32 OtherSectionIndex = OtherSections.Add(FProcMeshSection());
				NewOtherSection = &OtherSections[OtherSectionIndex];
			}



			// Map of base vert index to sliced vert index
			TMap<int32, int32> BaseToSlicedVertIndex;
			TMap<int32, int32> BaseToOtherSlicedVertIndex;

			const int32 NumBaseVerts = BaseSection->ProcVertexBuffer.Num();

			// Distance of each base vert from slice plane
			TArray<float> VertDistance;
			VertDistance.AddUninitialized(NumBaseVerts);

			// Build vertex buffer 
			for (int32 BaseVertIndex = 0; BaseVertIndex < NumBaseVerts; BaseVertIndex++)
			{
				FProcMeshVertex& BaseVert = BaseSection->ProcVertexBuffer[BaseVertIndex];

				// Calc distance from plane
				VertDistance[BaseVertIndex] = SlicePlane.PlaneDot(BaseVert.Position);

				// See if vert is being kept in this section
				if (VertDistance[BaseVertIndex] > 0.f)
				{
					// Copy to sliced v buffer
					int32 SlicedVertIndex = NewSection.ProcVertexBuffer.Add(BaseVert);
					// Update section bounds
					NewSection.SectionLocalBox += BaseVert.Position;
					// Add to map
					BaseToSlicedVertIndex.Add(BaseVertIndex, SlicedVertIndex);
				}
				// Or add to other half if desired
				else if (NewOtherSection != nullptr)
				{
					int32 SlicedVertIndex = NewOtherSection->ProcVertexBuffer.Add(BaseVert);
					NewOtherSection->SectionLocalBox += BaseVert.Position;
					BaseToOtherSlicedVertIndex.Add(BaseVertIndex, SlicedVertIndex);
				}
			}


			// Iterate over base triangles (ie 3 indices at a time)
			for (int32 BaseIndex = 0; BaseIndex < BaseSection->ProcIndexBuffer.Num(); BaseIndex += 3)
			{
				int32 BaseV[3]; // Triangle vert indices in original mesh
				int32* SlicedV[3]; // Pointers to vert indices in new v buffer
				int32* SlicedOtherV[3]; // Pointers to vert indices in new 'other half' v buffer

										// For each vertex..
				for (int32 i = 0; i < 3; i++)
				{
					// Get triangle vert index
					BaseV[i] = BaseSection->ProcIndexBuffer[BaseIndex + i];
					// Look up in sliced v buffer
					SlicedV[i] = BaseToSlicedVertIndex.Find(BaseV[i]);
					// Look up in 'other half' v buffer (if desired)
					if (bCreateOtherHalf)
					{
						SlicedOtherV[i] = BaseToOtherSlicedVertIndex.Find(BaseV[i]);
						// Each base vert _must_ exist in either BaseToSlicedVertIndex or BaseToOtherSlicedVertIndex 
						check((SlicedV[i] != nullptr) != (SlicedOtherV[i] != nullptr));
					}
				}

				// If all verts survived plane cull, keep the triangle
				if (SlicedV[0] != nullptr && SlicedV[1] != nullptr && SlicedV[2] != nullptr)
				{
					NewSection.ProcIndexBuffer.Add(*SlicedV[0]);
					NewSection.ProcIndexBuffer.Add(*SlicedV[1]);
					NewSection.ProcIndexBuffer.Add(*SlicedV[2]);
				}
				// If all verts were removed by plane cull
				else if (SlicedV[0] == nullptr && SlicedV[1] == nullptr && SlicedV[2] == nullptr)
				{
					// If creating other half, add all verts to that
					if (NewOtherSection != nullptr)
					{
						NewOtherSection->ProcIndexBuffer.Add(*SlicedOtherV[0]);
						NewOtherSection->ProcIndexBuffer.Add(*SlicedOtherV[1]);
						NewOtherSection->ProcIndexBuffer.Add(*SlicedOtherV[2]);
					}
				}
				// If partially culled, clip to create 1 or 2 new triangles
				else
				{
					int32 FinalVerts[4];
					int32 NumFinalVerts = 0;

					int32 OtherFinalVerts[4];
					int32 NumOtherFinalVerts = 0;

					FUtilEdge3D NewClipEdge;
					int32 ClippedEdges = 0;

					float PlaneDist[3];
					PlaneDist[0] = VertDistance[BaseV[0]];
					PlaneDist[1] = VertDistance[BaseV[1]];
					PlaneDist[2] = VertDistance[BaseV[2]];

					for (int32 EdgeIdx = 0; EdgeIdx < 3; EdgeIdx++)
					{
						int32 ThisVert = EdgeIdx;

						// If start vert is inside, add it.
						if (SlicedV[ThisVert] != nullptr)
						{
							check(NumFinalVerts < 4);
							FinalVerts[NumFinalVerts++] = *SlicedV[ThisVert];
						}
						// If not, add to other side
						else if (bCreateOtherHalf)
						{
							check(NumOtherFinalVerts < 4);
							OtherFinalVerts[NumOtherFinalVerts++] = *SlicedOtherV[ThisVert];
						}

						// If start and next vert are on opposite sides, add intersection
						int32 NextVert = (EdgeIdx + 1) % 3;

						if ((SlicedV[EdgeIdx] == nullptr) != (SlicedV[NextVert] == nullptr))
						{
							// Find distance along edge that plane is
							float Alpha = -PlaneDist[ThisVert] / (PlaneDist[NextVert] - PlaneDist[ThisVert]);
							// Interpolate vertex params to that point
							FProcMeshVertex InterpVert = SInterpolateVert(BaseSection->ProcVertexBuffer[BaseV[ThisVert]], BaseSection->ProcVertexBuffer[BaseV[NextVert]], FMath::Clamp(Alpha, 0.0f, 1.0f));

							// Add to vertex buffer
							int32 InterpVertIndex = NewSection.ProcVertexBuffer.Add(InterpVert);
							// Update bounds
							NewSection.SectionLocalBox += InterpVert.Position;

							// Save vert index for this poly
							check(NumFinalVerts < 4);
							FinalVerts[NumFinalVerts++] = InterpVertIndex;

							// If desired, add to the poly for the other half as well
							if (NewOtherSection != nullptr)
							{
								int32 OtherInterpVertIndex = NewOtherSection->ProcVertexBuffer.Add(InterpVert);
								NewOtherSection->SectionLocalBox += InterpVert.Position;
								check(NumOtherFinalVerts < 4);
								OtherFinalVerts[NumOtherFinalVerts++] = OtherInterpVertIndex;
							}

							// When we make a new edge on the surface of the clip plane, save it off.
							check(ClippedEdges < 2);
							if (ClippedEdges == 0)
							{
								NewClipEdge.V0 = InterpVert.Position;
							}
							else
							{
								NewClipEdge.V1 = InterpVert.Position;
							}

							ClippedEdges++;
						}
					}

					// Triangulate the clipped polygon.
					for (int32 VertexIndex = 2; VertexIndex < NumFinalVerts; VertexIndex++)
					{
						NewSection.ProcIndexBuffer.Add(FinalVerts[0]);
						NewSection.ProcIndexBuffer.Add(FinalVerts[VertexIndex - 1]);
						NewSection.ProcIndexBuffer.Add(FinalVerts[VertexIndex]);
					}

					// If we are making the other half, triangulate that as well
					if (NewOtherSection != nullptr)
					{
						for (int32 VertexIndex = 2; VertexIndex < NumOtherFinalVerts; VertexIndex++)
						{
							NewOtherSection->ProcIndexBuffer.Add(OtherFinalVerts[0]);
							NewOtherSection->ProcIndexBuffer.Add(OtherFinalVerts[VertexIndex - 1]);
							NewOtherSection->ProcIndexBuffer.Add(OtherFinalVerts[VertexIndex]);
						}
					}

					check(ClippedEdges != 1); // Should never clip just one edge of the triangle

											  // If we created a new edge, save that off here as well
					if (ClippedEdges == 2)
					{
						ClipEdges.Add(NewClipEdge);
					}
				}
			}

			// Remove 'other' section from array if no valid geometry for it
			if (NewOtherSection != nullptr && (NewOtherSection->ProcIndexBuffer.Num() == 0 || NewOtherSection->ProcVertexBuffer.Num() == 0))
			{
				OtherSections.RemoveAt(OtherSections.Num() - 1);
			}

			// If we have some valid geometry, update section
			if (NewSection.ProcIndexBuffer.Num() > 0 && NewSection.ProcVertexBuffer.Num() > 0)
			{
				BaseValid = true;
				OutBaseConfig = ProcMeshSectionToProcMeshConfig(NewSection);
			}
			else // If we don't, remove this section
			{
				BaseValid = false;
			}
		}
	}





	// Create cap geometry (if some edges to create it from)
	if (bCreateCap && ClipEdges.Num() > 0)
	{
		FProcMeshSection CapSection;



		//CapSection = *BaseSection;


		  // Project 3D edges onto slice plane to form 2D edges
		TArray<FUtilEdge2D> Edges2D;
		FUtilPoly2DSet PolySet;
		FGeomTools::ProjectEdges(Edges2D, PolySet.PolyToWorld, ClipEdges, SlicePlane);

		// Find 2D closed polygons from this edge soup
		FGeomTools::Buid2DPolysFromEdges(PolySet.Polys, Edges2D, FColor(255, 255, 255, 255));

		// Remember start point for vert and index buffer before adding and cap geom
		int32 CapVertBase = CapSection.ProcVertexBuffer.Num();
		int32 CapIndexBase = CapSection.ProcIndexBuffer.Num();




		// Triangulate each poly
		for (int32 PolyIdx = 0; PolyIdx < PolySet.Polys.Num(); PolyIdx++)
		{
			// Generate UVs for the 2D polygon.
			FGeomTools::GeneratePlanarTilingPolyUVs(PolySet.Polys[PolyIdx], 64.f);

			// Remember start of vert buffer before adding triangles for this poly
			int32 PolyVertBase = CapSection.ProcVertexBuffer.Num();

			// Transform from 2D poly verts to 3D
			STransform2DPolygonTo3D(PolySet.Polys[PolyIdx], PolySet.PolyToWorld, CapSection.ProcVertexBuffer, CapSection.SectionLocalBox);

			// Triangulate this polygon
			CTriangulatePoly(CapSection.ProcIndexBuffer, CapSection.ProcVertexBuffer, PolyVertBase, PlaneNormal);
		}

		// Set geom for cap section
////		InProcMesh->SetProcMeshSection(0, CapSection);

		OutCapConfig = ProcMeshSectionToProcMeshConfig(CapSection);


	}



	/**

			// Array of sliced collision shapes
			TArray< TArray<FVector> > SlicedCollision;
			TArray< TArray<FVector> > OtherSlicedCollision;

			UBodySetup* ProcMeshBodySetup = InProcMesh->GetBodySetup();

			for (int32 ConvexIndex = 0; ConvexIndex < ProcMeshBodySetup->AggGeom.ConvexElems.Num(); ConvexIndex++)
			{
				FKConvexElem& BaseConvex = ProcMeshBodySetup->AggGeom.ConvexElems[ConvexIndex];

				int32 BoxCompare = SBoxPlaneCompare(BaseConvex.ElemBox, SlicePlane);

				// If box totally clipped, add to other half (if desired)
				if (BoxCompare == -1)
				{
					if (bCreateOtherHalf)
					{
						OtherSlicedCollision.Add(BaseConvex.VertexData);
					}
				}
				// If box totally valid, just keep mesh as is
				else if (BoxCompare == 1)
				{
					SlicedCollision.Add(BaseConvex.VertexData);
				}
				// Need to actually slice the convex shape
				else
				{
					TArray<FVector> SlicedConvexVerts;
					SliceConvexElem(BaseConvex, SlicePlane, SlicedConvexVerts);
					// If we got something valid, add it
					if (SlicedConvexVerts.Num() >= 4)
					{
						SlicedCollision.Add(SlicedConvexVerts);
					}

					// Slice again to get the other half of the collision, if desired
					if (bCreateOtherHalf)
					{
						TArray<FVector> OtherSlicedConvexVerts;
						SliceConvexElem(BaseConvex, SlicePlane.Flip(), OtherSlicedConvexVerts);
						if (OtherSlicedConvexVerts.Num() >= 4)
						{
							OtherSlicedCollision.Add(OtherSlicedConvexVerts);
						}
					}
				}
			}

			// Update collision of proc mesh
			InProcMesh->SetCollisionConvexMeshes(SlicedCollision);
			*/


	if (bCreateOtherHalf)
	{
		if (OtherSections.Num() > 0)
		{
			OtherValid = true;
			OutOtherConfig = ProcMeshSectionToProcMeshConfig(OtherSections[0]);
		}
		else
		{
			OtherValid = false;
		}
	}
	else
	{
		OtherValid = false;
	}

	if (BaseSection != nullptr)
	{
		delete BaseSection;
	}
}


UProceduralMeshComponent* USolidGeometryFunctionLibrary::AddProceduralMeshComponent(AActor* Actor)
{

	//	UProceduralMeshComponent* Mesh =ConstructObject(UProceduralMeshComponent::StaticClass(), Actor);

	UProceduralMeshComponent* Mesh = NewObject<UProceduralMeshComponent>(Actor);
	Mesh->SetupAttachment(Actor->GetRootComponent());
	Mesh->RegisterComponent();

	return  Mesh;
}


void USolidGeometryFunctionLibrary::TransformPnts(TArray<FVector> &OutTransformedPnts, const TArray<FVector> &InPnts, const FTransform &Transform)
{
	int32 NumVerts = InPnts.Num();
	OutTransformedPnts.SetNum(NumVerts);
	for (int32 i = 0; i < NumVerts; ++i)
	{
		OutTransformedPnts[i] = Transform.TransformPosition(InPnts[i]);
	}
}

void USolidGeometryFunctionLibrary::ClipPntsByPlaneAndDirection(TArray<FVector> &OutClippedPnts, const TArray<FVector> &InPnts,
	const FVector &PntOnPlane, const FVector &NormalOfPlane, 
	const FVector &Direction2Project)
{
	FVector NormalOfPlaneNormalize = NormalOfPlane.GetSafeNormal();
	FVector Dir2ProjNormalize = Direction2Project.GetSafeNormal();
	if (abs(NormalOfPlaneNormalize | Dir2ProjNormalize) < SmallPositiveNumber)
	{
		OutClippedPnts = InPnts;
		return;
	}
	FPlane Plane(PntOnPlane, NormalOfPlane);

	for (int i = 0; i < InPnts.Num(); ++i)
	{
		const FVector &Position = InPnts[i];
		float DistOfPntAndPlane = Plane.PlaneDot(Position);
		if (DistOfPntAndPlane > 0)
		{
			OutClippedPnts[i] = Position - Direction2Project * (DistOfPntAndPlane / (Direction2Project | NormalOfPlane));
		}
	}
}
