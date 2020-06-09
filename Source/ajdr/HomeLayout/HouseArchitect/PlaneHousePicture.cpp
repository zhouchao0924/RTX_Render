// Copyright? 2017 ihomefnt All Rights Reserved.


#include "PlaneHousePicture.h"
#include "EditorGameInStance.h"
#include "../CGALWrapper/PolygonAlg.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include "Data/DRProjData.h"
#include "Data/Adapter/DRSolidWallAdapter.h"
#include "Data/Adapter/DRHoleAdapter.h"

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point_2 = K::Point_2;
using Polygon_2 = CGAL::Polygon_2<K>;
using Segment_2 = K::Segment_2;

bool UPlaneHousePicture::GetAllWallsData2D(UObject* WorldContextObject, TArray<FWallData2D>& Wall2DArray)
{
	UWorld* MyWorld = WorldContextObject->GetWorld();
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	TArray<ALineWallActor*>  WallsInScene = GameInstance->WallBuildSystem->GetWallsInScene();
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GameInstance);
	if (projectDataManager == nullptr) {
		return false;
	}

	if (WallsInScene.Num() == 0)
	{
		return false;
	}
	for (ALineWallActor* WallActor : WallsInScene)
	{
		UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallActor->ObjectId));
		if (solidWallAdapter == nullptr) {
			continue;
		}

		FWallData2D TempWall2D;
		TempWall2D.PStartLeft = FVector2D(solidWallAdapter->GetWallPositionData().LeftStartPos.X, solidWallAdapter->GetWallPositionData().LeftStartPos.Y);
		TempWall2D.PEndLeft = FVector2D(solidWallAdapter->GetWallPositionData().LeftEndPos.X, solidWallAdapter->GetWallPositionData().LeftEndPos.Y);

		TempWall2D.PStartRight = FVector2D(solidWallAdapter->GetWallPositionData().RightStartPos.X, solidWallAdapter->GetWallPositionData().RightStartPos.Y);
		TempWall2D.PEndRight = FVector2D(solidWallAdapter->GetWallPositionData().RightEndPos.X, solidWallAdapter->GetWallPositionData().RightEndPos.Y);
		TempWall2D.PStartPos = FVector2D(solidWallAdapter->GetWallPositionData().StartPos.X, solidWallAdapter->GetWallPositionData().StartPos.Y);
		TempWall2D.PEndPos = FVector2D(solidWallAdapter->GetWallPositionData().EndPos.X, solidWallAdapter->GetWallPositionData().EndPos.Y);
		TempWall2D.BStartCorner = WallActor->DoStartCornerConnectWall();
		TempWall2D.BEndCorner = WallActor->DoEndCornerConnectWall();
		Wall2DArray.Add(TempWall2D);
	}

	return true;
}
bool  UPlaneHousePicture::GetAllRegions(UObject* WorldContextObject, TMap<int32, FRegion2D>& Regions)
{
	UWorld* MyWorld = WorldContextObject->GetWorld();
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	TArray<FRoomPath> AreaPathArray = GameInstance->WallBuildSystem->GetInnerRoomPathArray();
	for (int32 Index = 0; Index < AreaPathArray.Num(); Index++)
	{
		FRoomPath TempRegion = AreaPathArray[Index];
		TArray<FVector2D> TempPs = TempRegion.InnerRoomPath;
		FRegion2D Temp2D;
		Temp2D.InnerPoints = TempPs;
		Regions.Add(Index, Temp2D);
	}
	return true;
}


bool UPlaneHousePicture::GetAllRegionOuter(UObject* WorldContextObject, TArray<FBoundarySeg2D>& BoundaryLines)
{
	TMap<int32, FRegion2D> Regions;
	bool Success = GetAllRegions(WorldContextObject, Regions);
	if (!Success)
	{
		return false;
	}
	TArray <Polygon_2> PolygonS;
	for (int32 I = 0; I < Regions.Num(); I++)
	{
		Polygon_2 TempPolygon;
		TArray<FVector2D> TempRegion = Regions[I].InnerPoints;
		for (int32 J = 0; J < TempRegion.Num(); J++)
		{
			TempPolygon.push_back(Point_2(TempRegion[J].X, TempRegion[J].Y));
		}
		if (TempPolygon.area() < 0)
		{
			TempPolygon.reverse_orientation();
		}
		PolygonS.Add(TempPolygon);
	}

	for (int32 PolygonIndex = 0; PolygonIndex < PolygonS.Num(); PolygonIndex++)
	{
		TArray<FVector2D> PolygonToFVectors;
		for (auto Vbegin = PolygonS[PolygonIndex].vertices_begin(); Vbegin != PolygonS[PolygonIndex].vertices_end(); Vbegin++)
		{
			PolygonToFVectors.Add(FVector2D((*Vbegin).x(), (*Vbegin).y()));
		}
		for (int32 VIndex = 0; VIndex < PolygonToFVectors.Num(); VIndex++)
		{
			int32 VInRegionCount = 0;
			int32 NextIndex = 0;
			if (VIndex == PolygonToFVectors.Num() - 1)
			{
				NextIndex = 0;
			}
			else
			{
				NextIndex = VIndex + 1;
			}
			Segment_2 TempSeg(Point_2(PolygonToFVectors[VIndex].X, PolygonToFVectors[VIndex].Y),
				Point_2(PolygonToFVectors[NextIndex].X, PolygonToFVectors[NextIndex].Y));

			FBoundarySeg2D TempBoundaryLine;
			TArray<FVector2D> TempCollect;
			TempBoundaryLine.SegStart = FVector2D(PolygonToFVectors[VIndex].X, PolygonToFVectors[VIndex].Y);
			TempBoundaryLine.SegEnd = FVector2D(PolygonToFVectors[NextIndex].X, PolygonToFVectors[NextIndex].Y);

			float P1X = PolygonToFVectors[VIndex].X;
			float P1Y = PolygonToFVectors[VIndex].Y;
			float P2X = PolygonToFVectors[NextIndex].X;
			float P2Y = PolygonToFVectors[NextIndex].Y;
			Point_2  MiddlePoint(0.5*(P1X + P2X), 0.5*(P1Y + P2Y));

			FVector2D TempVec = FVector2D((TempSeg.source() - MiddlePoint).x(), (TempSeg.source() - MiddlePoint).y());
			TempVec.Normalize();

			FVector2D RotatedVec(TempVec.GetRotated(90));
			Point_2 P1 = Point_2(MiddlePoint.x() + 80 * TempVec.X, MiddlePoint.y() + 80 * TempVec.Y);
			Point_2 P2 = Point_2(MiddlePoint.x() - 80 * TempVec.X, MiddlePoint.y() - 80 * TempVec.Y);
			////判断
			for (int32 AreaIndex = 0; AreaIndex < PolygonS.Num(); AreaIndex++)
			{
				if (AreaIndex != PolygonIndex)
				{
					if (PolygonS[AreaIndex].has_on_bounded_side(P1))
					{
						VInRegionCount++;
					}
					if (PolygonS[AreaIndex].has_on_bounded_side(P2))
					{
						VInRegionCount++;
					}
				}
			}
			if (VInRegionCount == 1)
			{
				BoundaryLines.Add(TempBoundaryLine);
			}
		}
	}
	if (BoundaryLines.Num() == 0)
	{
		return false;
	}
	return true;
}


bool UPlaneHousePicture::GetAllInnerWalls(UObject* WorldContextObject, TArray<FBoundarySeg2D>& InnerWallLines)
{
	UWorld* MyWorld = WorldContextObject->GetWorld();
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	TArray<ALineWallActor*>  WallsInScene = GameInstance->WallBuildSystem->GetWallsInScene();
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GameInstance);
	if (projectDataManager == nullptr) {
		return false;
	}


	//////获取所有的区域
	TArray<Polygon_2> RegionPolygons;
	TArray<FRoomPath> AreaPathArray = GameInstance->WallBuildSystem->GetInnerRoomPathArray();
	for (int32 Index = 0; Index < AreaPathArray.Num(); Index++)
	{
		FRoomPath TempRegion = AreaPathArray[Index];
		TArray<FVector2D> TempPs = TempRegion.InnerRoomPath;
		Polygon_2 TempPolygon;
		for (int32 I = 0; I < TempPs.Num(); I++)
		{
			TempPolygon.push_back(Point_2(TempPs[I].X, TempPs[I].Y));
		}
		if (TempPolygon.area() < 0)
		{
			TempPolygon.reverse_orientation();
		}
		RegionPolygons.Add(TempPolygon);
	}


	/////获取所有的墙内线
	for (ALineWallActor* WallActor : WallsInScene)
	{
		UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallActor->ObjectId));
		if (solidWallAdapter == nullptr) {
			continue;
		}

		FVector2D PStartLeft = FVector2D(solidWallAdapter->GetWallPositionData().LeftStartPos.X, solidWallAdapter->GetWallPositionData().LeftStartPos.Y);
		FVector2D PEndLeft = FVector2D(solidWallAdapter->GetWallPositionData().LeftEndPos.X, solidWallAdapter->GetWallPositionData().LeftEndPos.Y);
		FVector2D PStartRight = FVector2D(solidWallAdapter->GetWallPositionData().RightStartPos.X, solidWallAdapter->GetWallPositionData().RightStartPos.Y);
		FVector2D PEndRight = FVector2D(solidWallAdapter->GetWallPositionData().RightEndPos.X, solidWallAdapter->GetWallPositionData().RightEndPos.Y);
		FVector2D PStartPos = FVector2D(solidWallAdapter->GetWallPositionData().StartPos.X, solidWallAdapter->GetWallPositionData().StartPos.Y);
		FVector2D PEndPos = FVector2D(solidWallAdapter->GetWallPositionData().EndPos.X, solidWallAdapter->GetWallPositionData().EndPos.Y);

		FVector2D CenterP = 0.5*(PStartPos + PEndPos);
		FVector2D StartToEndVec = PStartPos - PEndPos;
		StartToEndVec.Normalize();
		FVector2D PStartLeftToCenter = PStartLeft - CenterP;
		PStartLeftToCenter.Normalize();

		FVector2D RotatedVec = StartToEndVec.GetRotated(90);
		float DotValueCriter = FVector2D::DotProduct(RotatedVec, PStartLeftToCenter);

		FVector2D Direction1, Direction2;
		if (DotValueCriter > 0)
		{
			Direction1 = RotatedVec;
			Direction2 = -RotatedVec;
		}
		else
		{
			Direction1 = -RotatedVec;
			Direction2 = RotatedVec;
		}


		FVector2D ExtendP1 = CenterP + 65 * Direction1;
		FVector2D ExtendP2 = CenterP + 65 * Direction2;


		////判断
		int32 VInRegionCount = 0;
		bool FindExtend1 = false;
		bool FindExtend2 = false;
		for (int32 AreaIndex = 0; AreaIndex < RegionPolygons.Num(); AreaIndex++)
		{
			if (RegionPolygons[AreaIndex].has_on_bounded_side(Point_2(ExtendP1.X, ExtendP1.Y)))
			{
				VInRegionCount++;
				FindExtend1 = true;
			}

			if (RegionPolygons[AreaIndex].has_on_bounded_side(Point_2(ExtendP2.X, ExtendP2.Y)))
			{
				VInRegionCount++;
				FindExtend2 = true;
			}
		}
		if (VInRegionCount == 1)
		{
			if (FindExtend1)
			{
				FBoundarySeg2D TempSeg;
				TempSeg.SegStart = PStartLeft;
				TempSeg.SegEnd = PEndLeft;
				TempSeg.VerticalDir = Direction1;
				InnerWallLines.Add(TempSeg);
			}
			else if (FindExtend2)
			{
				FBoundarySeg2D TempSeg;
				TempSeg.SegStart = PStartRight;
				TempSeg.SegEnd = PEndRight;
				TempSeg.VerticalDir = Direction2;
				InnerWallLines.Add(TempSeg);
			}
		}
	}

	return true;
}


bool UPlaneHousePicture::DivideInnerWalls(UObject* WorldContextObject, const TArray<FBoundarySeg2D> InnerWallLines,
	TArray<FBoundarySegsArray>&InnerWallGroups)
{
	if (InnerWallLines.Num() == 0)
	{
		return false;
	}

	FVector2D Vec1 = FVector2D(0, 1);
	FVector2D Vec2 = FVector2D(0, -1);
	FVector2D Vec3 = FVector2D(1, 0);
	FVector2D Vec4 = FVector2D(-1, 0);
	TArray<FBoundarySeg2D> TempArray1, TempArray2, TempArray3, TempArray4;
	for (int32 I = 0; I < InnerWallLines.Num(); I++)
	{
		float TempWallLen = FVector2D::Distance(InnerWallLines[I].SegStart, InnerWallLines[I].SegEnd);
		if (TempWallLen >= 50)
		{
			float DotValue1 = FVector2D::DotProduct(Vec1, InnerWallLines[I].VerticalDir);
			float DotValue2 = FVector2D::DotProduct(Vec2, InnerWallLines[I].VerticalDir);
			float DotValue3 = FVector2D::DotProduct(Vec3, InnerWallLines[I].VerticalDir);
			float DotValue4 = FVector2D::DotProduct(Vec4, InnerWallLines[I].VerticalDir);
			if (abs(DotValue1 - 1) <= 0.1)
			{
				TempArray1.Add(InnerWallLines[I]);
			}
			else if (abs(DotValue2 - 1) <= 0.1)
			{
				TempArray2.Add(InnerWallLines[I]);
			}
			else if (abs(DotValue3 - 1) <= 0.1)
			{
				TempArray3.Add(InnerWallLines[I]);
			}
			else if (abs(DotValue4 - 1) <= 0.1)
			{
				TempArray4.Add(InnerWallLines[I]);
			}
		}
	}




	FBoundarySegsArray TempSegsArray1, TempSegsArray2, TempSegsArray3, TempSegsArray4;
	TempSegsArray1.BoundarySegs = TempArray1;
	TempSegsArray2.BoundarySegs = TempArray2;
	TempSegsArray3.BoundarySegs = TempArray3;
	TempSegsArray4.BoundarySegs = TempArray4;
	InnerWallGroups.Add(TempSegsArray1);
	InnerWallGroups.Add(TempSegsArray2);
	InnerWallGroups.Add(TempSegsArray3);
	InnerWallGroups.Add(TempSegsArray4);

	return true;
}


bool UPlaneHousePicture::CalculateCameraCenter(UObject* WorldContextObject, const TArray<FBoundarySeg2D> InnerWallLines, FVector2D& RegionCenter, float& Radius)
{

	TArray<float> XCoors, YCoors;
	int32 NumCount = InnerWallLines.Num();
	for (int32 Index = 0; Index < NumCount; Index++)
	{
		XCoors.Add(InnerWallLines[Index].SegStart.X);
		XCoors.Add(InnerWallLines[Index].SegEnd.X);
		YCoors.Add(InnerWallLines[Index].SegStart.Y);
		YCoors.Add(InnerWallLines[Index].SegEnd.Y);
	}
	float MinX = XCoors[0], MaxX = XCoors[0], MinY = YCoors[0], MaxY = YCoors[0];

	for (int32 I = 1; I < 2 * NumCount; I++)
	{
		if (MinX >= XCoors[I])
		{
			MinX = XCoors[I];
		}

		if (MaxX <= XCoors[I])
		{
			MaxX = XCoors[I];
		}
		if (MinY >= YCoors[I])
		{
			MinY = YCoors[I];
		}

		if (MaxY <= YCoors[I])
		{
			MaxY = YCoors[I];
		}
	}

	FVector2D NearlyCorner1(MinX, MinY);
	FVector2D NearlyCorner2(MinX, MaxY);
	FVector2D NearlyCorner3(MaxX, MinY);
	FVector2D NearlyCorner4(MaxX, MaxY);
	TArray<FVector2D> Nodes;
	Nodes.Add(NearlyCorner1);
	Nodes.Add(NearlyCorner2);
	Nodes.Add(NearlyCorner3);
	Nodes.Add(NearlyCorner4);

	RegionCenter = 0.25*(NearlyCorner1 + NearlyCorner2 + NearlyCorner3 + NearlyCorner4);

	float MaxDis = FVector2D::Distance(Nodes[0], RegionCenter);
	for (int32 NodeIndex = 1; NodeIndex < 4; NodeIndex++)
	{
		float DisToNode = FVector2D::Distance(Nodes[NodeIndex], RegionCenter);
		if (DisToNode >= MaxDis)
		{
			MaxDis = DisToNode;
		}
	}
	/////optimize the radius
	FVector2D Middle1 = 0.5*(NearlyCorner1 + NearlyCorner2);
	FVector2D Middle2 = 0.5*(NearlyCorner1 + NearlyCorner3);
	FVector2D Middle3 = 0.5*(NearlyCorner2 + NearlyCorner4);
	FVector2D Middle4 = 0.5*(NearlyCorner3 + NearlyCorner4);

	float Dis1 = FVector2D::Distance(Middle1, RegionCenter);
	float Dis2 = FVector2D::Distance(Middle2, RegionCenter);
	float Dis3 = FVector2D::Distance(Middle3, RegionCenter);
	float Dis4 = FVector2D::Distance(Middle4, RegionCenter);

	float MinDisToMiddle = Dis1;
	if (Dis2 < MinDisToMiddle)
	{
		MinDisToMiddle = Dis2;
	}
	if (Dis3 < MinDisToMiddle)
	{
		MinDisToMiddle = Dis3;
	}
	if (Dis4 < MinDisToMiddle)
	{
		MinDisToMiddle = Dis4;
	}
	float ExtendRDis = 135 * MaxDis / MinDisToMiddle;

	Radius = MaxDis + ExtendRDis;

	return true;
}


bool UPlaneHousePicture::CalculateRegionCenters(UObject* WorldContextObject, UVaRestJsonObject* InJson, TArray<FVector2D>& RegionCenters, 
	TArray<FString>& RoomUsageNameSet)
{
	if (InJson == nullptr)
	{
		return false;
	}

	TArray<UVaRestJsonObject*> WallJsonObj = InJson->GetObjectArrayField("walls");
	TArray<Polygon_2> WallPolygon;
	TArray<FVector2D> WallCenters;
	for (int32 Index = 0; Index < WallJsonObj.Num(); Index++)
	{
		TArray<FVector2D> WallRegionPoints;
		TArray<UVaRestJsonObject*> EachWallRegion = WallJsonObj[Index]->GetObjectArrayField("wallPoints");

		for (int32 WallIndex = 0; WallIndex < EachWallRegion.Num(); WallIndex++)
		{
			float TempX = EachWallRegion[WallIndex]->GetNumberField("x");
			float TempY = EachWallRegion[WallIndex]->GetNumberField("y");
			WallRegionPoints.Add(FVector2D(TempX, TempY));
		}
		Polygon_2 TempWallPolygon;
		float WallCenterX = 0.0, WallCenterY = 0.0;
		for (int32 PIndex = 0; PIndex < 4; PIndex++)
		{
			TempWallPolygon.push_back(Point_2(WallRegionPoints[PIndex].X, WallRegionPoints[PIndex].Y));
			WallCenterX = WallCenterX + WallRegionPoints[PIndex].X;
			WallCenterY = WallCenterY + WallRegionPoints[PIndex].Y;
		}
		WallCenterX = WallCenterX / 4;
		WallCenterY = WallCenterY / 4;
		WallCenters.Add(FVector2D(WallCenterX, WallCenterY));
		WallPolygon.Add(TempWallPolygon);
		TempWallPolygon.clear();
	}



	TArray<FVector2D> CameraLocations;
	TArray<FVector2D> SmallRegionCenters;
	TArray<FString> RoomNameSet;
	TArray<int> RoomIDs;
	TArray<float> UsageIDs;
	TArray<float> IDs;
	TArray<UVaRestJsonObject*> RoomJsonObj = InJson->GetObjectArrayField("roomList");



	TArray<int32> CleanRoomIndexs;
	TArray<Polygon_2> CleanRoomPolygons;
	TMap<int32, TArray<FVector2D>> IndexMapRoomPs;
	for (int32 Index = 0; Index < RoomJsonObj.Num(); Index++)
	{
		TArray<FVector2D> AreaPoints;
		FVector2D SuitableCenter;
		TArray<UVaRestJsonObject*> AreaCoorJson = RoomJsonObj[Index]->GetObjectArrayField("areas");

		FString TempRoomNameString = RoomJsonObj[Index]->GetStringField("roomName");
		RoomNameSet.Add(TempRoomNameString);

		int TempRoomID = RoomJsonObj[Index]->GetIntegerField("roomId");
		RoomIDs.Add(TempRoomID);

		FString TempRoomUseString = RoomJsonObj[Index]->GetStringField("roomUsageName");
		RoomUsageNameSet.Add(TempRoomUseString);

		float TempUsageID = RoomJsonObj[Index]->GetNumberField("usageId");
		UsageIDs.Add(TempUsageID);

		IDs.Add(Index + 1);

		if (abs(TempUsageID - 19) <= 1.0e-3)
		{
			int32 CleanRoomIndex = Index;
			CleanRoomIndexs.Add(CleanRoomIndex);
			TArray<FVector2D> TempCleanPs;
			Polygon_2 TempCleanPolygon;
			for (int32 AreaIndex = 0; AreaIndex < AreaCoorJson.Num(); AreaIndex++)
			{
				float TempX = AreaCoorJson[AreaIndex]->GetNumberField("x");
				float TempY = AreaCoorJson[AreaIndex]->GetNumberField("y");
				TempCleanPs.Add(FVector2D(TempX, TempY));
				TempCleanPolygon.push_back(Point_2(TempX, TempY));
				CleanRoomPolygons.Add(TempCleanPolygon);
				IndexMapRoomPs.Add(CleanRoomIndex, TempCleanPs);
			}
		}

		for (int32 AreaIndex = 0; AreaIndex < AreaCoorJson.Num(); AreaIndex++)
		{
			float TempX = AreaCoorJson[AreaIndex]->GetNumberField("x");
			float TempY = AreaCoorJson[AreaIndex]->GetNumberField("y");
			AreaPoints.Add(FVector2D(TempX, TempY));
		}


		float VecX1 = AreaPoints[1].X - AreaPoints[0].X;
		float VecY1 = AreaPoints[1].Y - AreaPoints[0].Y;

		float VecX2 = AreaPoints[AreaPoints.Num() - 1].X;
		float VecY2 = AreaPoints[AreaPoints.Num() - 1].Y;
		float CrossResZ = VecX1 * VecY2 - VecY1 * VecX2;
		if (CrossResZ < 0)
		{
			for (int32 Jindex = 0; Jindex < AreaPoints.Num() / 2; Jindex++)
			{
				FVector2D TempVec = AreaPoints[Jindex];
				AreaPoints[Jindex] = AreaPoints[AreaPoints.Num() - 1 - Jindex];
				AreaPoints[AreaPoints.Num() - 1 - Jindex] = TempVec;
			}
		}


		FString LocalRoomName = RoomJsonObj[Index]->GetStringField("roomName");


		if (LocalRoomName == TEXT("客厅"))
		{
			bool IsSuccess = FPolygonAlg::LocateSuitableCenters(AreaPoints, SuitableCenter, SmallRegionCenters);
			if (!IsSuccess)
			{
				return false;
			}
			CameraLocations.Add(SuitableCenter);
		}
		else
		{

			bool IsCreated = FPolygonAlg::LocateSuitableCenter(AreaPoints, SuitableCenter);
			if (!IsCreated)
			{
				return false;
			}
			CameraLocations.Add(SuitableCenter);
		}

	}


	for (int32 I = 0; I < CleanRoomIndexs.Num(); I++)
	{
		bool ChangeCleanRoomLocation = false;
		int32 CleanRoomIndex = CleanRoomIndexs[I];
		Polygon_2 CleanRoomPolygon = CleanRoomPolygons[I];
		TArray<FVector2D> CleanRoomPoints = IndexMapRoomPs[CleanRoomIndex];
		int32 TargetWallIndex = 0;
		Point_2 CleanRoomP(CameraLocations[CleanRoomIndex].X, CameraLocations[CleanRoomIndex].Y);
		for (int32 WallIndex = 0; WallIndex < WallPolygon.Num(); WallIndex++)
		{
			if ((WallPolygon[WallIndex].has_on_bounded_side(CleanRoomP)) || (WallPolygon[WallIndex].has_on_boundary(CleanRoomP)))
			{
				ChangeCleanRoomLocation = true;
				TargetWallIndex = WallIndex;
				break;
			}
			Point_2 WallInPoint(WallCenters[WallIndex].X, WallCenters[WallIndex].Y);
			int32 TargetMaxPIndex = 0;
			if (CleanRoomPolygons[I].has_on_bounded_side(WallInPoint))
			{
				Point_2 CleanRoomPointCenter(CameraLocations[CleanRoomIndex].X, CameraLocations[CleanRoomIndex].Y);
				float Dis = sqrt(CGAL::squared_distance(WallInPoint, CleanRoomPointCenter));
				float Xvec = (CleanRoomP.x() - WallInPoint.x()) / Dis;
				float Yvec = (CleanRoomP.y() - WallInPoint.y()) / Dis;
				FVector2D Direct(Xvec, Yvec);
				float MaxDisToWallPs = 0.0;

				for (int32 RoomIndex = 0; RoomIndex < CleanRoomPoints.Num(); RoomIndex++)
				{
					float TempDis = sqrt(CGAL::squared_distance(CleanRoomPointCenter, WallInPoint));
					if (TempDis > MaxDisToWallPs)
					{
						MaxDisToWallPs = TempDis;
						TargetMaxPIndex = RoomIndex;
					}
				}
				FVector2D VecToBoundary(CleanRoomPoints[TargetMaxPIndex].X - CleanRoomPointCenter.x(), CleanRoomPoints[TargetMaxPIndex].Y - CleanRoomPointCenter.y());

				float InnerCross = VecToBoundary.X*Direct.X + VecToBoundary.Y*Direct.Y;
				if (Dis / abs(InnerCross) < 0.2)
				{
					if (InnerCross >= 0)
					{
						float SuitX = CleanRoomPointCenter.x() + 0.5*InnerCross*Xvec;
						float SuitY = CleanRoomPointCenter.y() + 0.5*InnerCross*Yvec;
						CameraLocations[CleanRoomIndex] = FVector2D(SuitX, SuitY);
					}
					else if (InnerCross < 0)
					{
						float SuitX = CleanRoomPointCenter.x() - 0.5*InnerCross*Xvec;
						float SuitY = CleanRoomPointCenter.y() - 0.5*InnerCross*Yvec;
						CameraLocations[CleanRoomIndex] = FVector2D(SuitX, SuitY);
					}
				}
			}
		}

		if (ChangeCleanRoomLocation == true)
		{

			FVector2D TempWallCenter = WallCenters[TargetWallIndex];
			Polygon_2 TempWallPolygon = WallPolygon[TargetWallIndex];
			TArray<FVector2D> WallPolygonPs;
			for (auto VBegin = TempWallPolygon.vertices_begin(), VEnd = TempWallPolygon.vertices_end(); VBegin != VEnd; ++VBegin)
			{
				WallPolygonPs.Add(FVector2D((*VBegin).x(), (*VBegin).y()));
			}
			int32 NextI = 0;
			float MinL = 0.0;
			TMap<float, FVector2D> LengthMapDir;
			for (int32 index = 0; index < WallPolygonPs.Num(); index++)
			{
				if (index == WallPolygonPs.Num() - 1)
				{
					NextI = 0;
				}
				else
				{
					NextI = index + 1;
				}
				FVector2D TempTryDir = WallPolygonPs[NextI] - WallPolygonPs[index];
				TempTryDir.Normalize();
				float TempDis = FVector2D::Distance(WallPolygonPs[index], WallPolygonPs[NextI]);
				LengthMapDir.Add(TempDis, TempTryDir);
				if ((MinL == 0.0) || (MinL >= TempDis))
				{
					MinL = TempDis;
				}
			}
			CameraLocations[CleanRoomIndex] = WallCenters[TargetWallIndex] + 300 * LengthMapDir[MinL];

		}

		if (!CleanRoomPolygon.has_on_bounded_side(Point_2(CameraLocations[CleanRoomIndex].X, CameraLocations[CleanRoomIndex].Y)))
		{
			float MinX = 0.0, MinY = 0.0, MaxX = 0.0, MaxY = 0.0;
			for (int32 j = 0; j < CleanRoomPoints.Num(); j++)
			{
				if ((MinX == 0) || (MinX > CleanRoomPoints[j].X))
				{
					MinX = CleanRoomPoints[j].X;
				}
				if ((MaxX == 0.0) || (MaxX < CleanRoomPoints[j].X))
				{
					MaxX = CleanRoomPoints[j].X;
				}
				if ((MinY == 0.0) || (MinY > CleanRoomPoints[j].Y))
				{
					MinY = CleanRoomPoints[j].Y;
				}
				if ((MaxY == 0.0) || (MaxY < CleanRoomPoints[j].Y))
				{
					MaxY = CleanRoomPoints[j].Y;
				}
			}
			FVector2D Corner1 = FVector2D(MinX, MaxY);
			FVector2D Corner2 = FVector2D(MinX, MinY);
			FVector2D Corner3 = FVector2D(MaxX, MinY);
			FVector2D Corner4 = FVector2D(MaxX, MaxY);
			CameraLocations[CleanRoomIndex] = 0.25*(Corner1 + Corner2 + Corner3 + Corner4);
		}
	}
	RegionCenters = CameraLocations;
	return true;
}


bool UPlaneHousePicture::ProjectWallLines(UObject* WorldContextObject, const TArray<FBoundarySegsArray> InnerWallGroups,
	TArray<FBoundarySegsArray>&ProjectionWallLines, TArray<FBoundarySeg2D>& GlobalSegs)
{

	if (InnerWallGroups.Num() != 4)
	{
		return false;
	}

	////Post Process All Projection WallLines
	TArray<FBoundarySeg2D> TempArray1, TempArray2, TempArray3, TempArray4;

	////First Array :reverse direction of (0,1)
	float FirstGroupMinY = 0.0;
	int32 FirstGroupTarget = 0;
	for (int32 I = 0; I < InnerWallGroups[0].BoundarySegs.Num(); I++)
	{
		if ((FirstGroupMinY == 0.0) || (FirstGroupMinY >= InnerWallGroups[0].BoundarySegs[I].SegStart.Y))
		{
			FirstGroupMinY = InnerWallGroups[0].BoundarySegs[I].SegStart.Y;
			FirstGroupTarget = I;
		}
	}


	for (int32 Index = 0; Index < InnerWallGroups[0].BoundarySegs.Num(); Index++)
	{
		FBoundarySeg2D TempSeg = InnerWallGroups[0].BoundarySegs[Index];
		if (Index != FirstGroupTarget)
		{
			FVector BaseLineStart(InnerWallGroups[0].BoundarySegs[FirstGroupTarget].SegStart, 0);
			FVector BaseLineEnd(InnerWallGroups[0].BoundarySegs[FirstGroupTarget].SegEnd, 0);
			FVector TempSegStart = FMath::ClosestPointOnInfiniteLine(BaseLineStart, BaseLineEnd, FVector(TempSeg.SegStart, 0));
			FVector TempSegEnd = FMath::ClosestPointOnInfiniteLine(BaseLineStart, BaseLineEnd, FVector(TempSeg.SegEnd, 0));
			FBoundarySeg2D TempProjectionSeg;
			TempProjectionSeg.SegStart = FVector2D(TempSegStart.X, TempSegStart.Y);
			TempProjectionSeg.SegEnd = FVector2D(TempSegEnd.X, TempSegEnd.Y);
			TempProjectionSeg.VerticalDir = FVector2D(0, 1);
			TempArray1.Add(TempProjectionSeg);
		}
		else
		{
			FBoundarySeg2D TempProjectionSeg;
			TempProjectionSeg.SegStart = InnerWallGroups[0].BoundarySegs[FirstGroupTarget].SegStart;
			TempProjectionSeg.SegEnd = InnerWallGroups[0].BoundarySegs[FirstGroupTarget].SegEnd;
			TempProjectionSeg.VerticalDir = FVector2D(0, 1);
			TempArray1.Add(TempProjectionSeg);
		}
	}


	//////Second Array :reverse direction of (0,-1)
	float SecondGroupMaxY = 0.0;
	int32 SecondGroupTarget = 0;
	for (int32 I = 0; I < InnerWallGroups[1].BoundarySegs.Num(); I++)
	{
		if ((SecondGroupMaxY == 0.0) || (SecondGroupMaxY <= InnerWallGroups[1].BoundarySegs[I].SegStart.Y))
		{
			SecondGroupMaxY = InnerWallGroups[1].BoundarySegs[I].SegStart.Y;
			SecondGroupTarget = I;
		}
	}

	for (int32 SecondGroupIndex = 0; SecondGroupIndex < InnerWallGroups[1].BoundarySegs.Num(); SecondGroupIndex++)
	{
		FBoundarySeg2D TempSeg = InnerWallGroups[1].BoundarySegs[SecondGroupIndex];
		if (SecondGroupIndex != SecondGroupTarget)
		{
			FVector BaseLineStart(InnerWallGroups[1].BoundarySegs[SecondGroupTarget].SegStart, 0);
			FVector BaseLineEnd(InnerWallGroups[1].BoundarySegs[SecondGroupTarget].SegEnd, 0);
			FVector TempSegStart = FMath::ClosestPointOnInfiniteLine(BaseLineStart, BaseLineEnd, FVector(TempSeg.SegStart, 0));
			FVector TempSegEnd = FMath::ClosestPointOnInfiniteLine(BaseLineStart, BaseLineEnd, FVector(TempSeg.SegEnd, 0));
			FBoundarySeg2D TempProjectionSeg;
			TempProjectionSeg.SegStart = FVector2D(TempSegStart.X, TempSegStart.Y);
			TempProjectionSeg.SegEnd = FVector2D(TempSegEnd.X, TempSegEnd.Y);
			TempProjectionSeg.VerticalDir = FVector2D(0, -1);
			TempArray2.Add(TempProjectionSeg);
		}
		else
		{
			FBoundarySeg2D TempProjectionSeg;
			TempProjectionSeg.SegStart = InnerWallGroups[1].BoundarySegs[SecondGroupIndex].SegStart;
			TempProjectionSeg.SegEnd = InnerWallGroups[1].BoundarySegs[SecondGroupIndex].SegEnd;
			TempProjectionSeg.VerticalDir = FVector2D(0, -1);
			TempArray2.Add(TempProjectionSeg);
		}
	}


	//////Third Array :reverse direction of (1,0)
	float ThirdGroupMinX = 0.0;
	int32 ThirdGroupTarget = 0;
	for (int32 I = 0; I < InnerWallGroups[2].BoundarySegs.Num(); I++)
	{
		if ((ThirdGroupMinX == 0.0) || (ThirdGroupMinX >= InnerWallGroups[2].BoundarySegs[I].SegStart.X))
		{
			ThirdGroupMinX = InnerWallGroups[2].BoundarySegs[I].SegStart.X;
			ThirdGroupTarget = I;
		}
	}

	for (int32 ThirdGroupIndex = 0; ThirdGroupIndex < InnerWallGroups[2].BoundarySegs.Num(); ThirdGroupIndex++)
	{
		FBoundarySeg2D TempSeg = InnerWallGroups[2].BoundarySegs[ThirdGroupIndex];
		if (ThirdGroupIndex != ThirdGroupTarget)
		{
			FVector BaseLineStart(InnerWallGroups[2].BoundarySegs[ThirdGroupTarget].SegStart, 0);
			FVector BaseLineEnd(InnerWallGroups[2].BoundarySegs[ThirdGroupTarget].SegEnd, 0);
			FVector TempSegStart = FMath::ClosestPointOnInfiniteLine(BaseLineStart, BaseLineEnd, FVector(TempSeg.SegStart, 0));
			FVector TempSegEnd = FMath::ClosestPointOnInfiniteLine(BaseLineStart, BaseLineEnd, FVector(TempSeg.SegEnd, 0));
			FBoundarySeg2D TempProjectionSeg;
			TempProjectionSeg.SegStart = FVector2D(TempSegStart.X, TempSegStart.Y);
			TempProjectionSeg.SegEnd = FVector2D(TempSegEnd.X, TempSegEnd.Y);
			TempProjectionSeg.VerticalDir = FVector2D(1, 0);
			TempArray3.Add(TempProjectionSeg);
		}
		else
		{
			FBoundarySeg2D TempProjectionSeg;
			TempProjectionSeg.SegStart = InnerWallGroups[2].BoundarySegs[ThirdGroupTarget].SegStart;
			TempProjectionSeg.SegEnd = InnerWallGroups[2].BoundarySegs[ThirdGroupTarget].SegEnd;
			TempProjectionSeg.VerticalDir = FVector2D(1, 0);
			TempArray3.Add(TempProjectionSeg);
		}
	}

	///////Fourth Array: reverse directio of (-1,0)
	float FourthGroupMaxX = 0.0;
	int32 FourthGroupTarget = 0;
	for (int32 I = 0; I < InnerWallGroups[3].BoundarySegs.Num(); I++)
	{
		if ((FourthGroupMaxX == 0.0) || (FourthGroupMaxX <= InnerWallGroups[3].BoundarySegs[I].SegStart.X))
		{
			FourthGroupMaxX = InnerWallGroups[3].BoundarySegs[I].SegStart.X;
			FourthGroupTarget = I;
		}
	}

	for (int32 FourthGroupIndex = 0; FourthGroupIndex < InnerWallGroups[3].BoundarySegs.Num(); FourthGroupIndex++)
	{
		FBoundarySeg2D TempSeg = InnerWallGroups[3].BoundarySegs[FourthGroupIndex];
		if (FourthGroupIndex != FourthGroupTarget)
		{
			FVector BaseLineStart(InnerWallGroups[3].BoundarySegs[FourthGroupTarget].SegStart, 0);
			FVector BaseLineEnd(InnerWallGroups[3].BoundarySegs[FourthGroupTarget].SegEnd, 0);
			FVector TempSegStart = FMath::ClosestPointOnInfiniteLine(BaseLineStart, BaseLineEnd, FVector(TempSeg.SegStart, 0));
			FVector TempSegEnd = FMath::ClosestPointOnInfiniteLine(BaseLineStart, BaseLineEnd, FVector(TempSeg.SegEnd, 0));
			FBoundarySeg2D TempProjectionSeg;
			TempProjectionSeg.SegStart = FVector2D(TempSegStart.X, TempSegStart.Y);
			TempProjectionSeg.SegEnd = FVector2D(TempSegEnd.X, TempSegEnd.Y);
			TempProjectionSeg.VerticalDir = FVector2D(-1, 0);
			TempArray4.Add(TempProjectionSeg);
		}
		else
		{
			FBoundarySeg2D TempProjectionSeg;
			TempProjectionSeg.SegStart = InnerWallGroups[3].BoundarySegs[FourthGroupTarget].SegStart;
			TempProjectionSeg.SegEnd = InnerWallGroups[3].BoundarySegs[FourthGroupTarget].SegEnd;
			TempProjectionSeg.VerticalDir = FVector2D(-1, 0);
			TempArray4.Add(TempProjectionSeg);
		}
	}

	FBoundarySegsArray  TempSegsArray1, TempSegsArray2, TempSegsArray3, TempSegsArray4;

	TArray<FBoundarySeg2D> NewArray1, NewArray2, NewArray3, NewArray4;
	bool FilterArray1 = FilterInnerWallLines(TempArray1, NewArray1);
	bool FilterArray2 = FilterInnerWallLines(TempArray2, NewArray2);
	bool FilterArray3 = FilterInnerWallLines(TempArray3, NewArray3);
	bool FilterArray4 = FilterInnerWallLines(TempArray4, NewArray4);
	if ((!FilterArray1) || (!FilterArray2) || (!FilterArray3) || (!FilterArray4))
	{
		return false;
	}

	/*TempSegsArray1.BoundarySegs = TempArray1;
	TempSegsArray2.BoundarySegs = TempArray2;
	TempSegsArray3.BoundarySegs = TempArray3;
	TempSegsArray4.BoundarySegs = TempArray4;*/

	TempSegsArray1.BoundarySegs = NewArray1;
	TempSegsArray2.BoundarySegs = NewArray2;
	TempSegsArray3.BoundarySegs = NewArray3;
	TempSegsArray4.BoundarySegs = NewArray4;
	ProjectionWallLines.Add(TempSegsArray1);
	ProjectionWallLines.Add(TempSegsArray2);
	ProjectionWallLines.Add(TempSegsArray3);
	ProjectionWallLines.Add(TempSegsArray4);


	//////Get Global Segs
	for (int32 ProjectSegIndex = 0; ProjectSegIndex < 4; ProjectSegIndex++)
	{
		if (ProjectSegIndex == 0)
		{
			float MinX = 0.0, MaxX = 0.0;
			float SegMinX = 0.0, SegMaxX = 0.0;
			int32 TargetMinXIndex = 0, TargetMaxXIndex = 0;

			TArray<float> MinXSets, MaxXSets;
			for (int32 I = 0; I < ProjectionWallLines[ProjectSegIndex].BoundarySegs.Num(); I++)
			{

				if (ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.X < ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.X)
				{
					SegMinX = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.X;
					SegMaxX = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.X;
					MinXSets.Add(SegMinX);
					MaxXSets.Add(SegMaxX);
				}
				else
				{
					SegMinX = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.X;
					SegMaxX = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.X;
					MinXSets.Add(SegMinX);
					MaxXSets.Add(SegMaxX);
				}
			}

			for (int32 J = 0; J < MinXSets.Num(); J++)
			{
				if ((MinX == 0.0) || (MinX >= MinXSets[J]))
				{
					MinX = MinXSets[J];
					TargetMinXIndex = J;
				}
			}

			for (int32 K = 0; K < MaxXSets.Num(); K++)
			{
				if ((MaxX == 0.0) || (MaxX <= MaxXSets[K]))
				{
					MaxX = MaxXSets[K];
					TargetMaxXIndex = K;
				}
			}

			FBoundarySeg2D TempGlobleSeg0;
			TempGlobleSeg0.SegStart = FVector2D(MinXSets[TargetMinXIndex], ProjectionWallLines[0].BoundarySegs[TargetMinXIndex].SegEnd.Y);
			TempGlobleSeg0.SegEnd = FVector2D(MaxXSets[TargetMaxXIndex], ProjectionWallLines[0].BoundarySegs[TargetMaxXIndex].SegEnd.Y);
			TempGlobleSeg0.VerticalDir = FVector2D(0, 1);
			GlobalSegs.Add(TempGlobleSeg0);
		}
		else if (ProjectSegIndex == 1)
		{
			float MinX = 0.0, MaxX = 0.0;
			float SegMinX = 0.0, SegMaxX = 0.0;
			int32 TargetMinXIndex = 0, TargetMaxXIndex = 0;

			TArray<float> MinXSets, MaxXSets;
			for (int32 I = 0; I < ProjectionWallLines[ProjectSegIndex].BoundarySegs.Num(); I++)
			{

				if (ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.X < ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.X)
				{
					SegMinX = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.X;
					SegMaxX = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.X;
					MinXSets.Add(SegMinX);
					MaxXSets.Add(SegMaxX);
				}
				else
				{
					SegMinX = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.X;
					SegMaxX = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.X;
					MinXSets.Add(SegMinX);
					MaxXSets.Add(SegMaxX);
				}
			}

			for (int32 J = 0; J < MinXSets.Num(); J++)
			{
				if ((MinX == 0.0) || (MinX >= MinXSets[J]))
				{
					MinX = MinXSets[J];
					TargetMinXIndex = J;
				}
			}

			for (int32 K = 0; K < MaxXSets.Num(); K++)
			{
				if ((MaxX == 0.0) || (MaxX <= MaxXSets[K]))
				{
					MaxX = MaxXSets[K];
					TargetMaxXIndex = K;
				}
			}

			FBoundarySeg2D TempGlobleSeg1;
			TempGlobleSeg1.SegStart = FVector2D(MinXSets[TargetMinXIndex], ProjectionWallLines[1].BoundarySegs[TargetMinXIndex].SegEnd.Y);
			TempGlobleSeg1.SegEnd = FVector2D(MaxXSets[TargetMaxXIndex], ProjectionWallLines[1].BoundarySegs[TargetMaxXIndex].SegEnd.Y);
			TempGlobleSeg1.VerticalDir = FVector2D(0, -1);
			GlobalSegs.Add(TempGlobleSeg1);
		}
		else if (ProjectSegIndex == 2)
		{
			float MinY = 0.0, MaxY = 0.0;
			float SegMinY = 0.0, SegMaxY = 0.0;
			int32 TargetMinYIndex = 0, TargetMaxYIndex = 0;

			TArray<float> MinYSets, MaxYSets;
			for (int32 I = 0; I < ProjectionWallLines[ProjectSegIndex].BoundarySegs.Num(); I++)
			{

				if (ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.Y < ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.Y)
				{
					SegMinY = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.Y;
					SegMaxY = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.Y;
					MinYSets.Add(SegMinY);
					MaxYSets.Add(SegMaxY);
				}
				else
				{
					SegMinY = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.Y;
					SegMaxY = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.Y;
					MinYSets.Add(SegMinY);
					MaxYSets.Add(SegMaxY);
				}
			}

			for (int32 J = 0; J < MinYSets.Num(); J++)
			{
				if ((MinY == 0.0) || (MinY >= MinYSets[J]))
				{
					MinY = MinYSets[J];
					TargetMinYIndex = J;
				}
			}

			for (int32 K = 0; K < MaxYSets.Num(); K++)
			{
				if ((MaxY == 0.0) || (MaxY <= MaxYSets[K]))
				{
					MaxY = MaxYSets[K];
					TargetMaxYIndex = K;
				}
			}

			FBoundarySeg2D TempGlobleSeg2;
			TempGlobleSeg2.SegStart = FVector2D(ProjectionWallLines[2].BoundarySegs[TargetMinYIndex].SegStart.X, MinYSets[TargetMinYIndex]);
			TempGlobleSeg2.SegEnd = FVector2D(ProjectionWallLines[2].BoundarySegs[TargetMaxYIndex].SegEnd.X, MaxYSets[TargetMaxYIndex]);
			TempGlobleSeg2.VerticalDir = FVector2D(1, 0);
			GlobalSegs.Add(TempGlobleSeg2);
		}
		else if (ProjectSegIndex == 3)
		{

			float MinY = 0.0, MaxY = 0.0;
			float SegMinY = 0.0, SegMaxY = 0.0;
			int32 TargetMinYIndex = 0, TargetMaxYIndex = 0;

			TArray<float> MinYSets, MaxYSets;
			for (int32 I = 0; I < ProjectionWallLines[ProjectSegIndex].BoundarySegs.Num(); I++)
			{

				if (ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.Y < ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.Y)
				{
					SegMinY = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.Y;
					SegMaxY = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.Y;
					MinYSets.Add(SegMinY);
					MaxYSets.Add(SegMaxY);
				}
				else
				{
					SegMinY = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegEnd.Y;
					SegMaxY = ProjectionWallLines[ProjectSegIndex].BoundarySegs[I].SegStart.Y;
					MinYSets.Add(SegMinY);
					MaxYSets.Add(SegMaxY);
				}
			}

			for (int32 J = 0; J < MinYSets.Num(); J++)
			{
				if ((MinY == 0.0) || (MinY >= MinYSets[J]))
				{
					MinY = MinYSets[J];
					TargetMinYIndex = J;
				}
			}

			for (int32 K = 0; K < MaxYSets.Num(); K++)
			{
				if ((MaxY == 0.0) || (MaxY <= MaxYSets[K]))
				{
					MaxY = MaxYSets[K];
					TargetMaxYIndex = K;
				}
			}

			FBoundarySeg2D TempGlobleSeg3;
			TempGlobleSeg3.SegStart = FVector2D(ProjectionWallLines[3].BoundarySegs[TargetMinYIndex].SegStart.X, MinYSets[TargetMinYIndex]);
			TempGlobleSeg3.SegEnd = FVector2D(ProjectionWallLines[3].BoundarySegs[TargetMaxYIndex].SegEnd.X, MaxYSets[TargetMaxYIndex]);
			TempGlobleSeg3.VerticalDir = FVector2D(-1, 0);
			GlobalSegs.Add(TempGlobleSeg3);
		}
	}
	return true;
}


bool UPlaneHousePicture::GetOpeningsOnWalls(UObject* WorldContextObject, TArray<FOpeningsOnWall>& OpeningsOnWalls, bool IsChaigai)
{
	UWorld* MyWorld = WorldContextObject->GetWorld();
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	TArray<ALineWallActor*>  WallsInScene = GameInstance->WallBuildSystem->GetWallsInScene();
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GameInstance);
	if (projectDataManager == nullptr) {
		return false;
	}

	if (WallsInScene.Num() == 0)
	{
		return false;
	}

	TArray<AWall_Boolean_Base*> DoorOrGroundWindowOnWall;
	for (ALineWallActor* WallActor : WallsInScene)
	{
		UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallActor->ObjectId));
		if (solidWallAdapter == nullptr) {
			continue;
		}

		TArray<AWall_Boolean_Base*> TempBoolean = WallActor->GetOpeningsOnWall();
		FVector2D PStartPos = FVector2D(solidWallAdapter->GetWallPositionData().LeftStartPos.X, solidWallAdapter->GetWallPositionData().LeftStartPos.Y);
		FVector2D PEndPos = FVector2D(solidWallAdapter->GetWallPositionData().LeftEndPos.X, solidWallAdapter->GetWallPositionData().LeftEndPos.Y);
		FVector2D WallVec = PEndPos - PStartPos;
		WallVec.Normalize();

		FVector2D wallLeftStart(solidWallAdapter->GetWallPositionData().LeftStartPos.X, solidWallAdapter->GetWallPositionData().LeftStartPos.Y);
		FVector2D wallRightStart(solidWallAdapter->GetWallPositionData().RightStartPos.X, solidWallAdapter->GetWallPositionData().RightStartPos.Y);
		FVector2D wallLeftEnd(solidWallAdapter->GetWallPositionData().LeftEndPos.X, solidWallAdapter->GetWallPositionData().LeftEndPos.Y);
		FVector2D wallRightEnd(solidWallAdapter->GetWallPositionData().RightEndPos.X, solidWallAdapter->GetWallPositionData().RightEndPos.Y);

		FVector2D WallStandCenter = 0.25*(wallLeftStart + wallRightStart + wallLeftEnd + wallRightEnd);

		FVector2D OutVec = WallVec.GetRotated(90);
		for (int32 I = 0; I < TempBoolean.Num(); I++)
		{
			FOpeningsOnWall TempOpen;
			FOpeningSimpleAttr FOpenAttr = TempBoolean[I]->GetOpeningSimpleAttr();
			FVector2D TempLocation = FOpenAttr.Location;
			float TempLeftWidth = FOpenAttr.LeftWidth;
			float TempRightWidth = FOpenAttr.RightWidth;
			float TempWallLeftThick = 0.f;
			float TempWallRighThick = 0.f;
			if (WallActor->WallComp3d)
			{
				TempWallLeftThick = WallActor->WallComp3d->Data->GetFloat(TEXT("ThickLeft"));
				TempWallRighThick = WallActor->WallComp3d->Data->GetFloat(TEXT("ThickRight"));
			}
			//float TempWallLeftThick = TempBoolean[I]->Wall_Thick_Left;
			//float TempWallRighThick = TempBoolean[I]->Wall_Thick_Right;

			FVector2D PStartLeft = TempLocation - TempLeftWidth * WallVec + OutVec * TempWallLeftThick;
			FVector2D PEndLeft = TempLocation + TempRightWidth * WallVec + OutVec * TempWallLeftThick;
			FVector2D PStartRight = TempLocation - TempLeftWidth * WallVec - OutVec * TempWallRighThick;
			FVector2D PEndRight = TempLocation + TempRightWidth * WallVec - OutVec * TempWallRighThick;

			FVector2D TempCenter = 0.25*(PStartLeft + PStartRight + PEndLeft + PEndRight);
			FVector2D DoorCenterToWallStartPos = WallStandCenter - TempCenter;
			float VecCross = FVector2D::DotProduct(DoorCenterToWallStartPos, OutVec);
			PStartLeft = PStartLeft + VecCross * OutVec;
			PEndLeft = PEndLeft + VecCross * OutVec;
			PStartRight = PStartRight + VecCross * OutVec;
			PEndRight = PEndRight + VecCross * OutVec;

			TempOpen.PStartLeft = PStartLeft;
			TempOpen.PEndLeft = PEndLeft;
			TempOpen.PStartRight = PStartRight;
			TempOpen.PEndRight = PEndRight;
			UDRProjData *ProjectManager = UDRProjData::GetProjectDataManager(WorldContextObject);
			if (ProjectManager != nullptr)
			{
				UDRHoleAdapter *HoleAdapter = Cast<UDRHoleAdapter>(ProjectManager->GetAdapter(TempBoolean[I]->ObjectID));
				if (HoleAdapter != nullptr)
				{
					TempOpen.HorizontalFlip = HoleAdapter->GetHorizontalFlip();
					TempOpen.VerticalFlip = HoleAdapter->GetVerticalFlip();
					TempOpen.HoleStatusType = HoleAdapter->GetHoleStatusType();
					if (IsChaigai&&HoleAdapter->GetHoleStatusType() == EHolestatusType::EDRNewAddHole)
					{
						TArray<FVector2D>InPoints;
						InPoints.Add(PStartLeft);
						InPoints.Add(PEndLeft);
						InPoints.Add(PEndRight);
						InPoints.Add(PStartRight);
						GameInstance->WallBuildSystem->DrawSlash(false, InPoints, TempLeftWidth+ TempRightWidth, TempOpen.Lines,false,25);
					}
				}
			}
			TempOpen.Type = FOpenAttr.Type;
			OpeningsOnWalls.Add(TempOpen);
		}
	}
	if (IsChaigai)
	{
		TArray<FDeleteHoleInfo> TempHoleInfo = GameInstance->DRDemolishHole;
		for (int i = 0; i < TempHoleInfo.Num(); ++i)
		{
			FVector2D dir = FVector2D(0, 1);
			FVector2D OutVec = dir.GetRotated(TempHoleInfo[i].Angle + 90);
			dir = dir.GetRotated(TempHoleInfo[i].Angle);
			FOpeningsOnWall TempOpen;
			TempOpen.PStartLeft = FVector2D(TempHoleInfo[i].Loc) - TempHoleInfo[i].Width*0.5*dir + TempHoleInfo[i].Thickness*0.5*OutVec;
			TempOpen.PEndLeft = FVector2D(TempHoleInfo[i].Loc) + TempHoleInfo[i].Width*0.5*dir + TempHoleInfo[i].Thickness*0.5*OutVec;
			TempOpen.PStartRight = FVector2D(TempHoleInfo[i].Loc) - TempHoleInfo[i].Width*0.5*dir - TempHoleInfo[i].Thickness*0.5*OutVec;
			TempOpen.PEndRight = FVector2D(TempHoleInfo[i].Loc) + TempHoleInfo[i].Width*0.5*dir - TempHoleInfo[i].Thickness*0.5*OutVec;
			switch (TempHoleInfo[i].HoleType)
			{
			case EModelType::EDRDoor:
				TempOpen.Type = EOpeningType::SingleDoor;
				break;
			case EModelType::EDRDoubleDoor:
				TempOpen.Type = EOpeningType::DoubleDoor;
				break;
			case EModelType::EDRSliderDoor:
				TempOpen.Type = EOpeningType::SlidingDoor;
				break;
			case EModelType::EDRDoorWay:
				TempOpen.Type = EOpeningType::DoorWay;
				break;
			case EModelType::EDRWindow:
				TempOpen.Type = EOpeningType::Window;
				break;
			case EModelType::EDRGroundWindow:
				TempOpen.Type = EOpeningType::FrenchWindow;
				break;
			case EModelType::EDRBayWindow:
				TempOpen.Type = EOpeningType::BayWindow;
				break;
			default:
				break;
			}
			TempOpen.HorizontalFlip = TempHoleInfo[i].Horizontalflip;
			TempOpen.VerticalFlip = TempHoleInfo[i].Verticalflip;
			TempOpen.HoleStatusType = EHolestatusType::EDRDeleteHole;
			TArray<FVector2D>InPoints;
			InPoints.Add(TempOpen.PStartLeft);
			InPoints.Add(TempOpen.PEndLeft);
			//InPoints.Add(TempOpen.PStartRight);
			InPoints.Add(TempOpen.PEndRight);
			InPoints.Add(TempOpen.PStartRight);
			GameInstance->WallBuildSystem->DrawSlash(true, InPoints, TempHoleInfo[i].Width, TempOpen.Lines, false,25);
			OpeningsOnWalls.Add(TempOpen);
		}
	}
	return true;
}


bool UPlaneHousePicture::GetBayYeWindowRegion(UWorld* MyWorld,AWall_Boolean_Base* OpeningsOnWall,TArray<FVector2D>&RegionPs,TArray<FVector2D>BoundaryPs)
{
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	TArray<ALineWallActor*>  WallsInScene = GameInstance->WallBuildSystem->GetWallsInScene();
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GameInstance);
	if (projectDataManager == nullptr) {
		return false;
	}
	if (WallsInScene.Num() == 0)
	{
		return false;
	}

	bool FindWall = false;
	for (ALineWallActor* WallActor : WallsInScene)
	{
		UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallActor->ObjectId));
		if (solidWallAdapter == nullptr) {
			continue;
		}

		TArray<AWall_Boolean_Base*> TempBoolean = WallActor->GetOpeningsOnWall();
		for (int NumBoolean = 0; NumBoolean < TempBoolean.Num(); ++NumBoolean)
		{
			if (TempBoolean.Contains(OpeningsOnWall))
			{
				FVector2D PStartPos(solidWallAdapter->GetWallPositionData().StartPos.X, solidWallAdapter->GetWallPositionData().StartPos.Y);
				FVector2D PEndPos(solidWallAdapter->GetWallPositionData().EndPos.X, solidWallAdapter->GetWallPositionData().EndPos.Y);
				FVector2D WallVec = PEndPos - PStartPos;
				FVector2D TempWallVec = WallVec.GetSafeNormal();
				FVector2D OutVec = TempWallVec.GetRotated(90);
				FVector2D WallLoc = 0.5*(PStartPos + PEndPos);
				FVector2D TryP1 = WallLoc + 80 * OutVec;
				FVector2D TryP2 = WallLoc - 80 * OutVec;
				FOpeningSimpleAttr FOpenAttr = OpeningsOnWall->GetOpeningSimpleAttr();
				FVector2D TempLocation = FOpenAttr.Location;
				float TempLeftWidth = FOpenAttr.LeftWidth;
				float TempRightWidth = FOpenAttr.RightWidth;
				if (FPolygonAlg::JudgePointOnOrInPolygon(BoundaryPs, TryP1))
				{
					FVector2D Corner1 = TempLocation - TempLeftWidth * TempWallVec;
					FVector2D Corner2 = TempLocation + TempRightWidth * TempWallVec;
					FVector2D Corner3 = Corner2 - OutVec * 70;
					FVector2D Corner4 = Corner1 - OutVec * 70;
					RegionPs.Add(Corner1);
					RegionPs.Add(Corner2);
					RegionPs.Add(Corner3);
					RegionPs.Add(Corner4);
				}
				else if (FPolygonAlg::JudgePointOnOrInPolygon(BoundaryPs, TryP2))
				{
					FVector2D Corner1 = TempLocation - TempLeftWidth * TempWallVec;
					FVector2D Corner2 = TempLocation + TempRightWidth * TempWallVec;
					FVector2D Corner3 = Corner2 + OutVec * 70;
					FVector2D Corner4 = Corner1 + OutVec * 70;
					RegionPs.Add(Corner1);
					RegionPs.Add(Corner2);
					RegionPs.Add(Corner3);
					RegionPs.Add(Corner4);
				}
				FindWall = true;
				break;
			}
		}
		if (FindWall)
		{
			break;
		}
	}
	if (RegionPs.Num() != 4)
	{
		return false;
	}
	return true;
}

bool UPlaneHousePicture::GetBayYeWindowsOnWalls(UWorld* MyWorld,TArray<AWall_Boolean_Base*>& BayYeWindows)
{
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	TArray<ALineWallActor*>  WallsInScene = GameInstance->WallBuildSystem->GetWallsInScene();
	if (WallsInScene.Num() == 0)
	{
		return false;
	}

	for (ALineWallActor* WallActor : WallsInScene)
	{
		TArray<AWall_Boolean_Base*> TempBoolean = WallActor->GetOpeningsOnWall();
		for (int32 I = 0; I < TempBoolean.Num(); I++)
		{
			FOpeningsOnWall TempOpen;
			FOpeningSimpleAttr FOpenAttr = TempBoolean[I]->GetOpeningSimpleAttr();
			TempOpen.Type = FOpenAttr.Type;


			if (TempOpen.Type == EOpeningType::BayWindow)
			{
				BayYeWindows.Add(TempBoolean[I]);
			}
		}
	}
	return true;
}

bool UPlaneHousePicture::GetAllWindowsOnWalls(UWorld* MyWorld, TArray<AWall_Boolean_Base*>& Windows)
{
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	TArray<ALineWallActor*>  WallsInScene = GameInstance->WallBuildSystem->GetWallsInScene();
	if (WallsInScene.Num() == 0)
	{
		return false;
	}

	for (ALineWallActor* WallActor : WallsInScene)
	{
		TArray<AWall_Boolean_Base*> TempBoolean = WallActor->GetOpeningsOnWall();
		for (int32 I = 0; I < TempBoolean.Num(); I++)
		{
			FOpeningsOnWall TempOpen;
			FOpeningSimpleAttr FOpenAttr = TempBoolean[I]->GetOpeningSimpleAttr();
			TempOpen.Type = FOpenAttr.Type;

			if (TempOpen.Type == EOpeningType::BayWindow || TempOpen.Type == EOpeningType::Window)
			{
				Windows.Add(TempBoolean[I]);
			}
		}
	}
	return true;
}

bool UPlaneHousePicture::FilterInnerWallLines(const TArray<FBoundarySeg2D> InWallLines, TArray<FBoundarySeg2D>& FilteredWallLines)
{
	if (InWallLines.Num() < 0)
	{
		return false;
	}

	
	TArray<FBoundarySeg2D> TempWallLines = InWallLines;
	
	TArray<int32> ToRemoveIndexs;
	for (int32 I = 0; I < InWallLines.Num(); I++)
	{
		for (int32 J = 0; J < InWallLines.Num(); J++)
		{
			if ((J != I) && (!ToRemoveIndexs.Contains(J)))
			{
				FVector2D Vec1 = InWallLines[I].SegStart - InWallLines[J].SegStart;
				FVector2D Vec2 = InWallLines[I].SegEnd - InWallLines[J].SegStart;

				FVector2D Vec3 = InWallLines[I].SegStart - InWallLines[J].SegEnd;
				FVector2D Vec4 = InWallLines[I].SegEnd - InWallLines[J].SegEnd;

				float CrossValue1 = Vec1.X*Vec2.X + Vec1.Y*Vec2.Y;////J的Start在I的内部
				float CrossValue2 = Vec3.X*Vec4.X + Vec3.Y*Vec4.Y;////J的End在I的内部

				FVector2D Vec5 = InWallLines[J].SegStart - InWallLines[I].SegStart;
				FVector2D Vec6 = InWallLines[J].SegEnd - InWallLines[I].SegStart;

				FVector2D Vec7 = InWallLines[J].SegStart - InWallLines[I].SegEnd;
				FVector2D Vec8 = InWallLines[J].SegEnd - InWallLines[I].SegEnd;

				float CrossValue3 = Vec5.X*Vec6.X + Vec5.Y*Vec6.Y;////I的Start在J的内部
				float CrossValue4 = Vec7.X*Vec8.X + Vec7.Y*Vec8.Y;////I的End在J的内部


				if ((CrossValue1 < 0) && (CrossValue2 < 0))
				{

					float Dis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
					float Dis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
					if (Dis1 < Dis2)
					{
						ToRemoveIndexs.AddUnique(I);
					}
					else
					{
						ToRemoveIndexs.AddUnique(J);
					}

				}
				if ((CrossValue3 < 0) && (CrossValue4 < 0))
				{
					float Dis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
					float Dis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
					if (Dis1 < Dis2)
					{
						ToRemoveIndexs.AddUnique(I);
					}
					else
					{
						ToRemoveIndexs.AddUnique(J);
					}


				}

		

				if (InWallLines[I].SegStart.Equals(InWallLines[J].SegStart))
				{
					FVector2D TempVec1 = InWallLines[I].SegEnd - InWallLines[I].SegStart;
					FVector2D TempVec2 = InWallLines[J].SegEnd - InWallLines[J].SegStart;
					TempVec1.Normalize();
					TempVec2.Normalize();
					float VecCrossValue = FVector2D::DotProduct(TempVec1, TempVec2);
					if (VecCrossValue > 0)
					{
						float TempDis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
						float TempDis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
						if (TempDis1 < TempDis2)
						{
							ToRemoveIndexs.AddUnique(I);
						}
						else if (TempDis1 > TempDis2)
						{
							ToRemoveIndexs.AddUnique(J);
						}
					}
				}
				else if (InWallLines[I].SegStart.Equals(InWallLines[J].SegEnd))
				{
					FVector2D TempVec1 = InWallLines[I].SegEnd - InWallLines[I].SegStart;
					FVector2D TempVec2 = InWallLines[J].SegStart - InWallLines[J].SegEnd;
					TempVec1.Normalize();
					TempVec2.Normalize();
					float VecCrossValue = FVector2D::DotProduct(TempVec1, TempVec2);
					if (VecCrossValue > 0)
					{
						float TempDis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
						float TempDis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
						if (TempDis1 < TempDis2)
						{
							ToRemoveIndexs.AddUnique(I);
						}
						else if (TempDis1 > TempDis2)
						{
							ToRemoveIndexs.AddUnique(J);
						}
					}
				}
				else if (InWallLines[I].SegEnd.Equals(InWallLines[J].SegEnd))
				{
					FVector2D TempVec1 = InWallLines[I].SegEnd - InWallLines[I].SegStart;
					FVector2D TempVec2 = InWallLines[J].SegEnd - InWallLines[J].SegStart;
					TempVec1.Normalize();
					TempVec2.Normalize();
					float VecCrossValue = FVector2D::DotProduct(TempVec1, TempVec2);
					if (VecCrossValue > 0)
					{
						float TempDis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
						float TempDis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
						if (TempDis1 < TempDis2)
						{
							ToRemoveIndexs.AddUnique(I);
						}
						else if (TempDis1 > TempDis2)
						{
							ToRemoveIndexs.AddUnique(J);
						}
					}
				}
				else if (InWallLines[I].SegEnd.Equals(InWallLines[J].SegStart))
				{
					FVector2D TempVec1 = InWallLines[I].SegEnd - InWallLines[I].SegStart;
					FVector2D TempVec2 = InWallLines[J].SegStart - InWallLines[J].SegEnd;
					TempVec1.Normalize();
					TempVec2.Normalize();
					float VecCrossValue = FVector2D::DotProduct(TempVec1, TempVec2);
					if (VecCrossValue > 0)
					{
						float TempDis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
						float TempDis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
						if (TempDis1 < TempDis2)
						{
							ToRemoveIndexs.AddUnique(I);
						}
						else if (TempDis1 > TempDis2)
						{
							ToRemoveIndexs.AddUnique(J);
						}
					}
				}
				else if ((CrossValue1 < 0) && (CrossValue4 < 0))
				{
					float TempDis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
					float TempDis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
					if (TempDis1 < TempDis2)
					{
						ToRemoveIndexs.AddUnique(I);
					}
					else
					{
						ToRemoveIndexs.AddUnique(J);
					}
				}
				else if ((CrossValue2 < 0) && (CrossValue4 < 0))
				{
					float TempDis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
					float TempDis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
					if (TempDis1 < TempDis2)
					{
						ToRemoveIndexs.AddUnique(I);
					}
					else
					{
						ToRemoveIndexs.AddUnique(J);
					}
				}
				else if ((CrossValue1 < 0) && (CrossValue3 < 0))
				{
					float TempDis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
					float TempDis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
					if (TempDis1 < TempDis2)
					{
						ToRemoveIndexs.AddUnique(I);
					}
					else
					{
						ToRemoveIndexs.AddUnique(J);
					}
				}
				else if ((CrossValue2 < 0) && (CrossValue3 < 0))
				{
					float TempDis1 = FVector2D::Distance(InWallLines[I].SegStart, InWallLines[I].SegEnd);
					float TempDis2 = FVector2D::Distance(InWallLines[J].SegStart, InWallLines[J].SegEnd);
					if (TempDis1 < TempDis2)
					{
						ToRemoveIndexs.AddUnique(I);
					}
					else
					{
						ToRemoveIndexs.AddUnique(J);
					}
				}

			}
		}
	}


	//TArray<FBoundarySeg2D> TempFilteredWallLines;
	for (int32 Index = 0; Index < TempWallLines.Num(); Index++)
	{
		if (!ToRemoveIndexs.Contains(Index))
		{
			FilteredWallLines.Add(TempWallLines[Index]);
		}
	}

	//FilteredWallLines = TempFilteredWallLines;
	//TArray<FBoundarySeg2D> ProcessedInWallLines;
	//TArray<int>ProcessedIndexs;

	//for (int32 WallLineIndex = 0; WallLineIndex < TempFilteredWallLines.Num(); WallLineIndex++)
	//{
	//	for (int32 Index = 0; Index < TempFilteredWallLines.Num(); Index++)
	//	{
	//		if (Index != WallLineIndex)
	//		{
	//			if (TempFilteredWallLines[WallLineIndex].SegEnd.Equals(TempFilteredWallLines[Index].SegStart))
	//			{
	//				FVector2D Vec1 = TempFilteredWallLines[WallLineIndex].SegStart - TempFilteredWallLines[WallLineIndex].SegEnd;
	//				FVector2D Vec2 = TempFilteredWallLines[Index].SegEnd - TempFilteredWallLines[WallLineIndex].SegEnd;
	//				float CrossValue = Vec1.X*Vec2.X + Vec1.Y*Vec2.Y;
	//				if (CrossValue < 0)
	//				{
	//					FBoundarySeg2D TempSeg;
	//					TempSeg.SegStart = TempFilteredWallLines[WallLineIndex].SegStart;
	//					TempSeg.SegEnd = TempFilteredWallLines[Index].SegEnd;
	//					TempSeg.VerticalDir = TempFilteredWallLines[WallLineIndex].VerticalDir;
	//					ProcessedInWallLines.Add(TempSeg);
	//					ProcessedIndexs.AddUnique(WallLineIndex);
	//					ProcessedIndexs.AddUnique(Index);
	//				}
	//			}
	//			else if (TempFilteredWallLines[WallLineIndex].SegStart.Equals(TempFilteredWallLines[Index].SegStart))
	//			{
	//				FVector2D Vec1 = TempFilteredWallLines[WallLineIndex].SegEnd - TempFilteredWallLines[WallLineIndex].SegStart;
	//				FVector2D Vec2 = TempFilteredWallLines[Index].SegEnd - TempFilteredWallLines[WallLineIndex].SegStart;
	//				float CrossValue = Vec1.X*Vec2.X + Vec1.Y*Vec2.Y;
	//				if (CrossValue < 0)
	//				{
	//					FBoundarySeg2D TempSeg;
	//					TempSeg.SegStart = TempFilteredWallLines[WallLineIndex].SegEnd;
	//					TempSeg.SegEnd = TempFilteredWallLines[Index].SegEnd;
	//					TempSeg.VerticalDir = TempFilteredWallLines[WallLineIndex].VerticalDir;
	//					ProcessedInWallLines.Add(TempSeg);
	//					ProcessedIndexs.AddUnique(WallLineIndex);
	//					ProcessedIndexs.AddUnique(Index);
	//				}
	//			}
	//			else if (TempFilteredWallLines[WallLineIndex].SegEnd.Equals(TempFilteredWallLines[Index].SegEnd))
	//			{
	//				FVector2D Vec1 = TempFilteredWallLines[WallLineIndex].SegStart - TempFilteredWallLines[WallLineIndex].SegEnd;
	//				FVector2D Vec2 = TempFilteredWallLines[Index].SegStart - TempFilteredWallLines[WallLineIndex].SegEnd;
	//				float CrossValue = Vec1.X*Vec2.X + Vec1.Y*Vec2.Y;
	//				if (CrossValue < 0)
	//				{
	//					FBoundarySeg2D TempSeg;
	//					TempSeg.SegStart = TempFilteredWallLines[WallLineIndex].SegStart;
	//					TempSeg.SegEnd = TempFilteredWallLines[Index].SegStart;
	//					TempSeg.VerticalDir = TempFilteredWallLines[WallLineIndex].VerticalDir;
	//					ProcessedInWallLines.Add(TempSeg);
	//					ProcessedIndexs.AddUnique(WallLineIndex);
	//					ProcessedIndexs.AddUnique(Index);
	//				}
	//			}
	//			else if (TempFilteredWallLines[WallLineIndex].SegStart.Equals(TempFilteredWallLines[Index].SegEnd))
	//			{
	//				FVector2D Vec1 = TempFilteredWallLines[WallLineIndex].SegEnd - TempFilteredWallLines[WallLineIndex].SegStart;
	//				FVector2D Vec2 = TempFilteredWallLines[Index].SegStart - TempFilteredWallLines[WallLineIndex].SegStart;
	//				float CrossValue = Vec1.X*Vec2.X + Vec1.Y*Vec2.Y;
	//				if (CrossValue < 0)
	//				{
	//					FBoundarySeg2D TempSeg;
	//					TempSeg.SegStart = TempFilteredWallLines[WallLineIndex].SegEnd;
	//					TempSeg.SegEnd = TempFilteredWallLines[Index].SegStart;
	//					TempSeg.VerticalDir = TempFilteredWallLines[WallLineIndex].VerticalDir;
	//					ProcessedInWallLines.Add(TempSeg);
	//					ProcessedIndexs.AddUnique(WallLineIndex);
	//					ProcessedIndexs.AddUnique(Index);
	//				}
	//			}
	//		}
	//	}
	//}

	//for (int32 Index = 0; Index < TempFilteredWallLines.Num(); ++Index)
	//{
	//	if (!ProcessedIndexs.Contains(Index))
	//	{
	//		FilteredWallLines.Add(TempFilteredWallLines[Index]);
	//	}
	//}

	//for (int32 _CurLine = 0; _CurLine < ProcessedInWallLines.Num(); ++_CurLine)
	//{
	//	FilteredWallLines.Add(ProcessedInWallLines[_CurLine]);
	//}
	return true;
}

bool UPlaneHousePicture::GetAreaOrderData(const TArray<FVector2D> InnerWallPoints, const TArray<ALineWallActor*> BWalls, TArray<FAreaInnerWall>& outWallLines, TArray<FCustomizationBoolean>&outBooleanData)
{
	FVector2D Start;
	FVector2D End;
	FVector2D nomalv;
	FVector2D checkpoint;
	FVector2D checkinnerpoint;
	int32 count = InnerWallPoints.Num();
	float tempdx;
	TMap<int32, float>Mapdx;
	TMap<int32, int32>MapIndex;
	TMap<int32, ALineWallActor*>wallMap;

	if (count < 2) {
		return false;
	}
	for (int32 m = 0; m < BWalls.Num(); m++)
	{
		wallMap.Add(BWalls[m]->GetWallID(), BWalls[m]);
	}
	for (int32 i = 0; i < count; ++i)
	{
		Start = FVector2D(InnerWallPoints[i]);
		End = FVector2D(InnerWallPoints[(i + 1) % count]);
		nomalv = Start - End;
		nomalv = nomalv.GetRotated(-90);
		nomalv.Normalize();
		checkpoint = Start + 0.5f*(End - Start);
		checkinnerpoint = checkpoint + 2 * nomalv;
		bool isinner = FPolygonAlg::JudgePointInPolygon(InnerWallPoints, checkinnerpoint);
		FAreaInnerWall tempInwall;
		tempInwall.SegStart = Start;
		tempInwall.SegEnd = End;
		tempInwall.VerticalDir = isinner ? nomalv : -nomalv;
		tempInwall.WallID = IsPointNearstWallID(BWalls, checkpoint, tempdx);
		if (Mapdx.Contains(tempInwall.WallID)) {
			int32 index = MapIndex[tempInwall.WallID];
			float dx = Mapdx[tempInwall.WallID];
			if (dx > tempdx) {
				outWallLines[index].WallID = -1;
			}
			else
			{
				tempInwall.WallID = -1;
			}
		}
		Mapdx.Add(tempInwall.WallID, tempdx);
		MapIndex.Add(tempInwall.WallID, outWallLines.Num());
		outWallLines.Add(tempInwall);

	}
	int32 walllinenub = outWallLines.Num();
	int32 firstj = walllinenub - 1;
	for (int32 j = firstj; j >= 0; j--)
	{
		int32 pre = (j == 0) ? (walllinenub - 1) : (j - 1);
		int32 next = (j + 1) % walllinenub;
		if (outWallLines[j].WallID == -1) 
		{
		float length = FVector2D::Distance(outWallLines[j].SegStart, outWallLines[j].SegEnd);
	/*	int32 nextwallId = outWallLines[next].WallID;
		int32 prewallId= outWallLines[pre].WallID;
		float leftthick;
		float rightthick;
		float thick1 = 0;
		float thick2 = 0;
		if (nextwallId != -1) {
			wallMap[nextwallId]->GetWallThick(leftthick, rightthick);
			thick1 = leftthick + rightthick;
		}
		if (prewallId != -1) {
			wallMap[prewallId]->GetWallThick(leftthick, rightthick);
			thick2 = leftthick + rightthick;
		}
		if (length <= thick1 || length <= thick2) {
			outWallLines[next].SegStart = outWallLines[pre].SegEnd;
			outWallLines.RemoveAt(j);
			walllinenub -= 1;
		}*/
		if (length <= 5)
		{
			outWallLines[next].SegStart = outWallLines[pre].SegEnd;
			outWallLines.RemoveAt(j);
			walllinenub -= 1;
		}


		}

	}
	GetCustomizationBooleanByWalls(BWalls, outBooleanData);
	return true;
}

int32 UPlaneHousePicture::IsPointNearstWallID(const TArray<ALineWallActor*> BWalls, const FVector2D point, float &mindx)
{
	int32 wallid = 0;
	float tempdx;
	for (int i = 0; i < BWalls.Num(); i++)
	{
		int32 WallID = BWalls[i]->GetWallID();
		UBuildingSystem *BuildingSystem = BWalls[i]->WallComp3d->Data->GetBuildingSystem();
		UBuildingData* WallData = BuildingSystem->GetData(WallID);
		int StartCornerID = WallData->GetInt("P0");
		int EndCornerID = WallData->GetInt("P1");
		FVector2D PStartPos = BuildingSystem->GetData(StartCornerID)->GetVector2D("Location");
		FVector2D PEndPos = BuildingSystem->GetData(EndCornerID)->GetVector2D("Location");

		TArray<FVector2D>TempInArr;
		FVector2D OB = 0.5f*(PEndPos + PStartPos);

		float dx = FVector2D::Distance(OB, point);
		if (i == 0) {
			tempdx = dx;
			wallid = BWalls[i]->GetWallID();
		}
		if (i != 0 && dx < tempdx) {
			wallid = BWalls[i]->GetWallID();
			tempdx = dx;
		}
	}
	mindx = tempdx;
	return wallid;



}

bool UPlaneHousePicture::GetCustomizationBooleanByWalls(const TArray<ALineWallActor*> InBWalls, TArray<FCustomizationBoolean>& outBooleanData)
{
	int32 count = InBWalls.Num();
	if (count < 1) {
		return false;
	}

	for (ALineWallActor* WallActor : InBWalls)
	{
		TArray<AWall_Boolean_Base*> TempBoolean = WallActor->GetOpeningsOnWall();
		int32 WallID = WallActor->GetWallID();
		UBuildingSystem *BuildingSystem = WallActor->WallComp3d->Data->GetBuildingSystem();
		UBuildingData* WallData = BuildingSystem->GetData(WallID);
		int StartCornerID = WallData->GetInt("P0");
		int EndCornerID = WallData->GetInt("P1");

		FVector2D PStartPos = BuildingSystem->GetData(StartCornerID)->GetVector2D("Location");
		FVector2D PEndPos = BuildingSystem->GetData(EndCornerID)->GetVector2D("Location");
		FVector2D WallVec = PEndPos - PStartPos;
		WallVec.Normalize();
		FVector2D OutVec = WallVec.GetRotated(90);
		for (int32 i = 0; i < TempBoolean.Num(); i++)
		{
			FCustomizationBoolean tempBooleandata;
			FOpeningSimpleAttr FOpenAttr = TempBoolean[i]->GetOpeningSimpleAttr();
			FVector2D TempLocation = FOpenAttr.Location;
			float TempLeftWidth = FOpenAttr.LeftWidth;
			float TempRightWidth = FOpenAttr.RightWidth;
			float TempWallLeftThick = 0.f;
			float TempWallRighThick = 0.f;
			if (WallActor->WallComp3d)
			{
				TempWallLeftThick = WallActor->WallComp3d->Data->GetFloat(TEXT("ThickLeft"));
				TempWallRighThick = WallActor->WallComp3d->Data->GetFloat(TEXT("ThickRight"));
			}
			FVector2D PStartLeft = TempLocation - TempLeftWidth * WallVec + OutVec * TempWallLeftThick;
			FVector2D PEndLeft = TempLocation + TempRightWidth * WallVec + OutVec * TempWallLeftThick;
			tempBooleandata.PStartLeft = PStartLeft;
			tempBooleandata.PEndLeft = PEndLeft;
			tempBooleandata.Type = FOpenAttr.Type;
			tempBooleandata.HoleID = TempBoolean[i]->ObjectID;
			outBooleanData.Add(tempBooleandata);
		}

	}



	return true;
}

//@liff add
bool UPlaneHousePicture::GetHousePluginData(UObject* WorldContextObject, TArray<FHousePluginInfo>&outPlugData)
{
	outPlugData.Empty();
	TArray<AActor*> OutActor;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AComponentManagerActor::StaticClass(), OutActor);
	for (int i = 0; i < OutActor.Num(); ++i)
	{
		AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(OutActor[i]);
		if (ComponentManager)
		{
			TArray<FVector2D> Nodes;
			FComponenetInfo CompInfo = ComponentManager->GetComponenetInfo();
			FHousePluginInfo PluginInfo;
			if (ComponentManager->GetAllHouseComponent(Nodes))
			{
				PluginInfo.PLeftUp = Nodes[0];
				PluginInfo.PLeftDown = Nodes[1];
				PluginInfo.PRightDown = Nodes[2];
				PluginInfo.PRightUp = Nodes[3];
				PluginInfo.PluginID = CompInfo.DRActorID;
				PluginInfo.Type = CompInfo.ComponenetType;
				outPlugData.Add(PluginInfo);
			}

			if (CompInfo.ComponenetType== EComponenetType::_Sewer)
			{
				FVector2D CenterLoc = CompInfo.Loc;
				float RadVal = CompInfo.Width;
				FVector2D LenVector = FVector2D(0.0, RadVal / 2.0);
				FVector2D WidVector = FVector2D(RadVal / 2.0, 0.0);
				PluginInfo.PLeftUp = CenterLoc + LenVector - WidVector;
				PluginInfo.PLeftDown = CenterLoc - LenVector - WidVector;
				PluginInfo.PRightDown = CenterLoc - LenVector + WidVector;
				PluginInfo.PRightUp = CenterLoc + LenVector + WidVector;
				PluginInfo.PluginID = CompInfo.DRActorID;
				PluginInfo.Type = CompInfo.ComponenetType;
				outPlugData.Add(PluginInfo);
			}
		}
	}


	return true;
}
//@liff add end