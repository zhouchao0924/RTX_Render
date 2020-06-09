// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CgTypes.h"


class   FPolygonAlg
{

public:
	static void PolyTriagulation(FPoint2DArray& OutPnts,
		const FPoint2DArray& InputPoly, const TArray<FPoint2DArray>& InputHoles,
		bool bDoubleSide = true);

	static bool LocateBetterCenter(const FPoint2DArray InPnts, FVector2D& BetterCenter);

	static bool LocateSuitableCenter(const FPoint2DArray InPnts, FVector2D& SuitableCenter);

	static bool LocateSuitableCenters(const FPoint2DArray InPnts, FVector2D& MaxRegionCenter, TArray<FVector2D>& SmallRegionCenters);

	static bool LocateCentersOfDiningOrLivingRegion(const FPoint2DArray InPnts, FVector2D& MaxRegionCenter,
		TArray<FVector2D>& UpdateSmallRegionCenters, TMap<FVector2D, TArray<FVector2D>>&CenterMapLocalRegions);

	static bool LocateCentersOfRoom(const FPoint2DArray InPnts, FVector2D& MaxRegionCenter,
		TArray<FVector2D>& UpdateSmallRegionCenters, TMap<FVector2D, TArray<FVector2D>>&CenterMapLocalRegions);

	static bool LocateCentersOfBalcony(const TArray<FVector2D> InPnts, FVector2D& MaxRegionCenter,
		TArray<FVector2D>& UpdateSmallRegionCenters, TMap<FVector2D, TArray<FVector2D>>&CenterMapLocalRegions);


	static bool LocateCenterOfKitchen(const TArray<FVector2D> InPnts, FVector2D& MaxRegionCenter);

	static bool LocateCenterOfOneToilet(const TArray<FVector2D>InPnts, FVector2D& Center);

	static bool LocateCentersOfSecondToilet(const TArray<FVector2D>InPnts, const TArray<FVector2D> WallPointsOfRegion, TArray<FVector2D>& Centers);


	static bool CalculateShortestL(const FPoint2DArray InPnts, float& ShortestL);

	static bool CalculateInterSection(const FPoint2DArray InPnts, FVector2D PSource, FVector2D PTarget, FVector2D& InterSectP);

	static bool MergeShortEdges(const FPoint2DArray InPnts, FPoint2DArray& OutPnts);

	static bool MergeCollinearPoints(const FPoint2DArray InPnts, FPoint2DArray& OutPnts);

	static bool StricMergeCollinear(const FPoint2DArray InPnts, FPoint2DArray& OutPnts);

	static bool MergePoints(const FPoint2DArray InPnts, const float DisBound, TArray<FVector2D>& RegionCenters);

	static bool MergeShrotEdgeCmSize(const FPoint2DArray InPnts, FPoint2DArray& OutPnts);

	static void PolygonTriangulate(const FPoint2DArray InPnts, TArray<int32>&TriIndexs);

	static bool MesherPolygonToGetCenter(const FPoint2DArray InPnts, FVector2D& Center);

	static bool GetAllInterSectPs(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>&UpdatePolygonA,
		TArray<FVector2D>& UpdatePolygonB, TArray<FVector2D>& InterSectPs);

	static bool CreateVirtualWallPoints(const TArray<FVector2D> InterSectPs, TArray<FVector2D>& VirtualWallPs);

	static bool CreateActualVirtualWallPoints(const TArray<FVector2D> FirstPolygon,const TArray<FVector2D> SecondPolygon,const TArray<FVector2D> InterSectPs, TArray<FVector2D>& VirtualWallPs);

	static bool CreatePolygonUnion(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>&NewPolygonFirst,
		TArray<FVector2D>&NewPolygonSecond, TArray<FVector2D>&PolygonUnion);

	static bool CreateVirtualWall(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>& VirtualWallPs);

	static bool JudgePointInPolygon(const TArray<FVector2D> InPnts, FVector2D ToJudgePoint);

	static bool JudgePointOnOrInPolygon(const TArray<FVector2D> InPnts, FVector2D ToJudgePoint);

	static bool CreateVirtualWallWithDir(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>& VirtualWallPs,
		TMap<FVector2D, FVector2D>&TailPsMapDir);

	static bool CalculateTwoPolygonInterSect(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>&UpdatePolygonFirst,
		TArray<FVector2D>& UpdatePolygonSecond, TArray<FVector2D>&InterSectPs, TArray<FVector2D>& CombinedPolygon);

	static bool CGPolygonCombine(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>& CombinedPolygon);

	static bool JudgePolygonInterSect(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond);

	static bool FindParallelEdge(const TArray<FVector2D>InPolygon, FVector2D PStart, FVector2D PEnd, FVector2D& ParallelPStart,
		FVector2D& ParallelPEnd, int32& ParallelEdgeIndex, const float& DisValue);

	static bool JudgeHasClosePoint(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, const float DisBound = 5.0);

	static bool MergeShortEdgesPreProcess(const TArray<FVector2D> InPnts, TArray<FVector2D>& OutPnts, const float DisBound = 0.1);

	static bool GetMaxAreaEndPoints(const TArray<FVector2D> InPutPolygon, TArray<FVector2D>&EndPoints, TArray<FVector2D>&MapPoints, TArray<FVector2D>& MaxRegion);

	static bool GetWayInterSectPs(const TArray<FVector2D> InPutPolygon, const TArray<FVector2D>EndPoints, const TArray<FVector2D>MapPoints,
		TArray<FVector2D>&InterSectPs);

	static bool GetWayInterSectPsWithSegInfo(const TArray<FVector2D> InPutPolygon, const TArray<FVector2D>EndPoints, const TArray<FVector2D>MapPoints,
		TArray<FVector2D>&InterSectPs, bool &IfSegClose);

	static bool LocateLightsOfPassageAndVestibule(const TArray<FVector2D> InPutPolygon, TArray<FVector>&LocationOfLights, const float WallHeight = 280, const float ReduceHeight = 25);

	static bool DivideDoorPointsToGroups(const TArray<FVector> DoorPoints, TArray<FVector2D> EndPoints, TArray<FVector2D> MapEndPoints,
		TMap<int32, int32>&DoorIndexMapSegIndex);

	static bool GetUsefulRegionOfMinArea(const TArray<FVector2D> InPutPolygon, TArray<FVector2D>& MinAreaRegion, FVector2D& SuitPoint);

	static float DistanceOfPointToSeg(const FVector2D InPoint, const FVector2D StartOfSeg, const FVector2D EndOfSeg);

	static bool GetUsefulRegionOfMaxArea(const TArray<FVector2D> InPutPolygon, TArray<FVector2D>& MinAreaRegion, FVector2D& SuitPoint);

	static bool FindParallelEdges(const TArray<FVector2D>InPolygon, FVector2D PStart, FVector2D PEnd, TMap<int32, TArray<FVector2D>>&EdgeMapPoints,
		TArray<int32>& EdgeIndexs, const float& DisValue);

	static bool GetMaxRegionOfSpecialRoom(const TArray<FVector2D> InPutPolygon, TArray<FVector2D>& MaxAreaRegion,
		FVector2D& SuitPoint, TArray<FVector>DoorPoints);

	static bool GetRegionOfFourCornerSpecialRoom(const TArray<FVector2D> InPolygon, FVector2D& SuitPoint, TArray<FVector2D> WallPs,
		TArray<FVector> DoorPoints, const float DisValue);

	static bool GetMaxLocalRegionCenter(const FPoint2DArray InPnts, FVector2D& SuitableCenter);

	static bool JudgeDoorOnLine(const FVector2D StartP, const FVector2D EndP, TArray<FVector> DoorPoints, const float DisValue = 0.01);

	static bool JudgeDoorOnLineAndGetDoor(const FVector2D StartP, const FVector2D EndP, TArray<FVector> DoorPoints,
		FVector2D& DoorCenter, const float DisValue = 0.01);

	static bool GetDisOfPointToPolygon(const FVector2D TestPoint, TArray<FVector2D> RegionPs);

	static bool JudgePointInOrOnDingingOrLivingRegion(const FVector2D ModelCenter, const TArray<FVector2D>LivingRegionPs, const TArray<FVector2D>DiningRegionPs, int&Result);

	static bool ReOrderModelCenters(TArray<FVector2D> RegionPs, TArray<FVector> ModelCenters, TArray<FVector>& OrderedModelCenters, const float DisOfClip = 100);

	static bool CheckRegionUseness(TArray<FVector2D> RegionPs);

	static bool StrictPolygonUnion(TArray<FVector2D> PolygonOne, TArray<FVector2D> PolygonTwo, TArray<FVector2D>& PolygonUnion);

	static bool LocateLightsOfDiningOrLivingRegion(const TArray<FVector2D> InPnts, TArray<FVector>&LightsLoc, const float WallHeight = 280, const float ReduceHeight = 25);

	static bool LocateLightsOfRoom(const TArray<FVector2D> InPnts, TArray<FVector>&LightsLoc, const float WallHeight = 280, const float ReduceHeight = 25);

	static bool LocateLightsOfSmallRegion(const TArray<FVector2D> InPnts, TArray<FVector>&LightsLoc, const float WallHeight = 280, const float ReduceHeight = 25);

	static bool LocateLightOfKitchen(const TArray<FVector2D> InPnts, FVector& LightLoc, const float WallHeight = 280, const float ReduceHeight = 25);

	static bool LocateLightOfOneToilet(const TArray<FVector2D>InPnts, FVector&LightLoc, const float WallHeight = 280, const float ReduceHeight = 25);

	static bool LocateTwoLightsOfOneToilet(const TArray<FVector2D> InPnts, TArray<FVector>&LightLocs, const float WallHeight = 280, const float ReduceHeight = 25);

	static bool LocateLightsOfSecondToliet(const TArray<FVector2D> InPnts, const TArray<FVector2D> WallPoints, TArray<FVector>& LightsLoc, const float WallHeight = 280, const float ReduceHeight = 25);

	static float GetAreaOfRegion(const TArray<FVector2D> InPnts);

	static bool CalculateRayInterSectPoly(const TArray<FVector2D> InPnts, const FVector2D StartPoint, const FVector2D PointVec, FVector2D& InterSectP, const float DisBound);

	static bool JudgePolygonsSnap(TArray<FVector2D> PolygonOne, TArray<FVector2D>PolygonTwo, TArray<FVector2D>&RegionPs, TArray<FVector2D>&ComponentPs);

	static bool RegionSnapMoreComps(TArray<FVector2D> RegionPs, TArray<FVector2D>Comps, TArray<FVector2D>&UpdateRegion, TArray<FVector2D>&NoSnapComps);

	static bool GetMinDisOfPointToPolygon(const FVector2D InPnt, const TArray<FVector2D>InPolygon, float& MinDis);

	static bool FilterLights(const TArray<FVector>LightLocs, const TArray<FVector2D> InPolygon, TArray<FVector>&UpdateLights, const float DisBound = 20);

	static void CalculateRayToInterSectP(const TArray<FVector2D> InPolygon, const FVector2D StartPoint, FVector2D PointVec, FVector2D& InterSectP);

	static bool GetRayToInterSectP(const TArray<FVector2D> InPolygon, const FVector2D StartPoint, FVector2D PointVec, FVector2D& InterSectP);

	static bool AdjustWatchAndPickPoints(const FVector2D FirstPickP, const FVector2D SecondPickP, const TArray<FVector2D> FirstPoly,
		const TArray<FVector2D> SecondPoly, FVector2D& UpdateFirstPoint, FVector2D& UpdateSecondPoint, FVector2D& UpdateWatchPoint);

	static bool JudgeModelBoxUseness(const FVector FMin, const FVector FMax, const TArray<FVector2D> RegionPs);

	static bool DownLightsLocation(const TArray<FVector2D> RegionPs, const float LightEqualSpace, const int NumOfLights, TArray<FVector2D>&LightsLocation);

	static float GetShortestdistanceBySegments(const FVector2D Seg1Start, const FVector2D Seg1End, const FVector2D Seg2Start, const FVector2D Seg2End);

	static bool IntersectionByRayAndSegment(const FVector2D StartRay, const FVector2D Dir, const FVector2D SegStart, const FVector2D SegEnd, FVector2D& OutPos);

	static bool  LocateMaxLocalRegionOfLivingOrDiningRoom(const TArray<FVector2D> RoomPs,
		const int RoomID,
		const TArray<FVector2D> OppoSiteRoomPs,
		TArray<FVector2D>& MaxLocalRegion);

	static int SplitRegionAndLocateMaxModel(const TArray<FVector2D> RegionPs, const FVector MaxModelCenter, TArray<FVector2D>& LeftSidePs, TArray<FVector2D>& RightSidePs);

	static bool LocateMaxLocalRegionOfRoom(const TArray<FVector2D> RoomPs, TArray<FVector2D>&MaxLocalRegion);

	static bool CalculateRayInterSectWithPoly(const FVector2D StartPos, const FVector2D RayVec, const TArray<FVector2D> Poly, TArray<FVector2D>& InterSectPs);

	static bool SplitPolygonByRay(const TArray<FVector2D> Polygon, const FVector2D SegStart, const FVector2D SegVec, TArray<FVector2D>& Part1Polygon, TArray<FVector2D>& Part2Polygon);
};
