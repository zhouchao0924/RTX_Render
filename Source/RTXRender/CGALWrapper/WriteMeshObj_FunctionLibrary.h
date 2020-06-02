// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PolygonAlg.h"
#include "ObjFileAnsyTask.h"
#include "HomeLayout/HouseArchitect/WallBuildSystem.h"
#include "FileHelper.h"
#include "../Building/DRStruct.h"
#include "WriteMeshObj_FunctionLibrary.generated.h"

USTRUCT(Blueprintable)
struct FloorGroup
{
	GENERATED_BODY()

		FloorGroup()
	{

	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> RoomIDs;

};

USTRUCT(Blueprintable)
struct FOldSceneLights
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* LightActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Trans;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Intensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Col;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Radius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InnerConeAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float OuterConeAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool  IsPointLight;
};

USTRUCT(Blueprintable)
struct FRegionLights
{
	GENERATED_BODY()

	FRegionLights()
	{

	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FIntelligentLight> LightInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RegionRoomId;
};


USTRUCT(Blueprintable)
struct FParentRoom
{
	GENERATED_BODY()

		FParentRoom()
	{

	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ParentRoomID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> RoomIDs;

};

USTRUCT(Blueprintable)
struct FRoomGroup
{
	GENERATED_BODY()

	FRoomGroup()
	{

	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> RoomIDs;

};

USTRUCT(Blueprintable)
struct FGroupRoomIDs
{
	GENERATED_BODY()

	FGroupRoomIDs()
	{

	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int GroupIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> RoomIDs;

};


USTRUCT(Blueprintable)
struct FPointPair
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RoutinePoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector WatchPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ActionMarker;
};

USTRUCT(Blueprintable)
struct FPathLine
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FPointPair> RoutinePoints;
};



USTRUCT(Blueprintable)
struct FRegionPsOfMovieProject
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> RegionPs;

};

USTRUCT(Blueprintable)
struct FRegionLightsOfRTX
{
	GENERATED_BODY()

	FRegionLightsOfRTX()
	{

	}
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FIntelligentLight> LightInfo;
};
UCLASS()


class AJDR_API UWriteMeshObj_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		
		UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool FromJsonToWriteObjFile(UObject* WorldContextObject, UVaRestJsonObject* InJson, const FString FilePath);


	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool  PolygonTriangulate(const TArray<FVector2D> InPnts, TArray<int32>&TriIndexs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetHouseRoomIDs(UObject* WorldContextObject, UVaRestJsonObject* InJson, TArray<int32>& AllRoomIDs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetWallsOfRoom(UObject* WorldContextObject, const int32 RoomID, TArray<ALineWallActor *>&WallsOfRegion);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetDoorsOfRegion(UObject* WorldContextObject, const int32 RoomID, TArray<FVector>&DoorPoints);


	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CreateDNALocationJsonWithCenters(UObject* WorldContextObject, UVaRestJsonObject* InJson, TArray<int32> AreaIndexsToRemove,
			UVaRestJsonObject* &LocationJson, TArray<FVector2D>& CameraLocations2D, int32& NumOfTotalPickPs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CreateAdvancedLocationJsonWithFloorsAndMulitiLivings(UObject* WorldContextObject, UVaRestJsonObject* InJson, UVaRestJsonObject* &LocationJson,
			TArray<FVector2D>& CameraLocations, int& NumOfTotalPickPs, TMap<int, FloorGroup>&GroupRoomIDs, TArray<FGroupRoomIDs>&GroupMapLivingOrDiningRoomIDs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CreateAdvancedLocationJsonWithFloors(UObject* WorldContextObject, UVaRestJsonObject* InJson, UVaRestJsonObject* &LocationJson,
			TArray<FVector2D>& CameraLocations, int32& NumOfTotalPickPs, TMap<int, FloorGroup>&GroupRoomIDs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CreateAdvancedPanorama(UObject* WorldContextObject, UVaRestJsonObject* InJson, UVaRestJsonObject* &OutPanoramaJson);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CreateAdvancedPanoramaOfDNA(UObject* WorldContextObject, UVaRestJsonObject* InJson, UVaRestJsonObject* &OutPanoramaJson);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CreatePanoramaWithInformation(UObject* WorldContextObject, UVaRestJsonObject* InJson, UVaRestJsonObject* &OutPanoramaJson,
			TMap<int32, FVector>& WatchPoints, TMap<int32, FVector>& LookDirs, const float WatchPointgHeight = 130);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool WriteLocationPointsToOFF(UObject* WorldContextObject, UVaRestJsonObject* InJson, const FString FilePath);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool WritePanoraWatchPointsToOFF(UObject* WorldContextObject, UVaRestJsonObject* InJson, const FString FilePath);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CreateMapWatchPoints(UObject* WorldContextObject, UVaRestJsonObject* InJson, int SolutionID, int RoomID,
			FVector& WatchPoint, FVector& LookDir);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool FromJsonToWriteFullObjFile(UObject* WorldContextObject, UVaRestJsonObject* InJson, const FString FileName,
			const FString FilePath);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool  CreateSegmentsJson(UObject* WorldContextObject, TArray<FVector2D>LivingRoomPs, int32 LivingRoomID,
			TArray<FVector>HoleDatas, UVaRestJsonObject* &SegmentsJson);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool SnapAreaPoints(UObject* WorldContextObject, const int32 RoomID, const TArray<FVector2D> OriginAreaPs, TArray<FVector2D>& OutAreaPs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool SaveFileToUTF8(UObject* WorldContextObject, const FString InStr, FString FileName, FString SaveDir, bool IsNoBom);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetWallPointsOfRoom(UObject* WorldContextObject, const int32 RoomID, TArray<FVector2D>&AllWallPoints);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool DivideAreas(UObject* WorldContextObject, UVaRestJsonObject* InJson, TMap<int, FloorGroup>&GroupRoomIDs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CalculateParentRelation(UObject* WorldContextObject, UVaRestJsonObject* InJson, TArray<int>&PossibleParentRoomIDs, TArray<FParentRoom>&ParentRooms);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool ReorderModelCenterPs(TArray<FVector2D> RegionPs, TArray<FVector> ModelCenters, TArray<FVector>& OrderedModelCenters, const float DisOfClip = 100);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool AutoMaticCreateLights(UObject* WorldContextObject, UVaRestJsonObject* InJson, TArray<FRegionLights>& AllRegionLights, const float ReduceHeight = 25);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool WriteLightsLocToFile(UObject* WorldContextObject, TArray<FVector> TotalLights, const FString FilePath, const FString FileName);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetAllLightsInWorld(UWorld* MyWorld, TArray<AActor*>&PointLights, TArray<AActor*>&SpotLights, TArray<AActor*>&UniqueSpotLight, TArray<FOldSceneLights>&OldLights);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static void FindModelsOfRoom(UObject* WorldContextObject, const TArray<FVector2D> RegionPs, TArray<AFurnitureModelActor*>&ModelsOfRoom);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static void FindCustomModelsOfRoom(UObject* WorldContextObject, const TArray<FVector2D> RegionPs, TArray<AHouseCustomActor*>&CustomModelsOfRoom);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool JudgeLightInExtendedBox(UObject* WorldContextObject, const TArray<FVector2D> RegionPs, const FVector InPoint, AFurnitureModelActor* FurModel, const float MaxLengthExtend = 40);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool  LocationOfDownLights(const TArray<FVector2D> RegionPs, const float LightEqualSpace, const int NumOfLights, TArray<FVector2D>&LightsLocation);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool SeperateRoomsIntoGroups(UObject* WorldContextObject, UVaRestJsonObject* InJson, TArray<FRoomGroup>&RoomGroups);

	//////////////////////////////////////////RTX小视频自动路径相关///////////////////////////////////////////
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool AutoMaticCreatePath(UObject* WorldContextObject, UVaRestJsonObject* InJson, const TArray<int>InPutRoomIDs, TArray<FPathLine>& AllPathLines, const float FlyingHeight = 130.0f);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool IsPointInOrOnRegion(const TArray<FVector2D> Region, const FVector2D JudgePoint);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateEntranceRoomID(UObject* WorldContextObject, UVaRestJsonObject* InJson, TArray<FRoomGroup>& RoomGroups, int &EntranceRoomID, FVector2D& EntranceDoorCenter, FVector2D& EntranceDir);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool FilterRoomPathPoints(UObject* WorldContextObject, const FPathLine WalkingLine, FPathLine& ResultLine, const TArray<FVector2D> RoomPs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool FilterLivingOrDiningPathPoints(UObject* WorldContextObject, const FPathLine WalkingLine, FPathLine& ResultLine, const TArray<FVector2D> RoomPs);


	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetWindowsOfRegion(UObject* WorldContextObject, const int32 RoomID, TArray<FVector>&WindowPoints);


	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateCriticalPointsOfLivingOrDining(UObject* WorldContextObject,
			const TArray<FVector2D> RoomPs,
			const int IDOfRoom,
			TArray<FVector2D> OppoSiteRoomPs,
			const TArray<int> HouseRoomIds,
			const TArray<FRegionPsOfMovieProject> RoomIdMapRegionPs,
			TArray<FVector2D>& CriticalPoints,
			TMap<int, FVector2D>& ChildRoomIDMapDoorCenter,
			TArray<int>& ChildRoomIDs,
			FVector2D& CenterPoint,
			bool IsLiving = false);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateCriticalPointsOfSpecialLivingOrDining(UObject* WorldContextObject,
			const TArray<FVector2D> RoomPs,
			const int IDOfRoom,
			TArray<FVector2D>& PathPoints,
			FVector2D& WatchPoint,
			bool IsLiving = false);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateCriticalPointsOfBalcony(UObject* WorldContextObject,
			const TArray<FVector2D> RoomPs,
			const int IDOfRoom,
			const TArray<FVector> DoorPoints,
			const FVector2D BasePoint,
			TArray<FVector2D>& CriticalPoints,
			FVector2D& RegionWatchPoint,
			int& ActionMarker);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateCriticalPointsOfNormalBalcony(UObject* WorldContextObject,
			const TArray<FVector2D> RoomPs,
			const int IDOfRoom,
			TArray<FVector2D>& CriticalPoints,
			FVector2D& RegionWatchPoint,
			int& ActionMarker);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateTwoCriticalPointsOfBalcony(UObject* WorldContextObject,
			const TArray<FVector2D> RoomPs,
			const int IDOfRoom,
			TArray<FVector2D>& CriticalPoints,
			FVector2D& RegionWatchPoint);


	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateCriticalPointsOfKitchen(UObject* WorldContextObject,
			const TArray<FVector2D> RoomPs,
			const int IDOfRoom,
			TArray<FVector2D>& CriticalPoints,
			FVector2D& RegionWatchPoint);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateCriticalPointsOfRoom(UObject* WorldContextObject,
			const TArray<FVector2D> LivingRoomPs,
			const TArray<FVector2D> DiningRoomPs,
			const TArray<FVector2D> RoomPoints,
			const int IDOfRoom,
			TArray<FVector2D>& CriticalPoints,
			FVector2D& RoomCenter);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateCriticalPointsOfToilet(UObject* WorldContextObject,
			const TArray<FVector2D> ToiletRoomPs,
			const TArray<FVector2D> WallCenters,
			const int IDOfRoom,
			TArray<FVector2D>& CriticalPoints,
			FVector2D& ToiletCenter);
	//////////////////////////////////////////////////////////RTX效果补灯相关///////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateRTXLightsOfDiningOrLivingRegion(UObject* WorldContextObject, const TArray<FVector2D> InPnts, const int RoomID, TArray<FVector>&LightsLoc, const float WallHeight = 280.0f,
			const float ReduceHeight = 25.0f);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateRTXLightsOfRoom(UObject* WorldContextObject, const TArray<FVector2D> InPnts, const int RoomID, TArray<FVector>&LightsLoc, const float WallHeight = 280.0f,
			const float ReduceHeight = 25.0f);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateRTXLightsOfNormalRoom(UObject* WorldContextObject, const TArray<FVector2D> InPnts, const int RoomID, TArray<FVector>&LightsLoc, const float WallHeight = 280.0f,
			const float ReduceHeight = 25.0f);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool AutoMaticCreateLightsOfRTX(UObject* WorldContextObject, UVaRestJsonObject* InJson, FRegionLightsOfRTX& LightsOfVestibule, FRegionLightsOfRTX& LightsOfOthers, const float ReduceHeight = 25.0f);

	//////////////////////////////////////////////////////////面板灯以及暖风机计算位置///////////////////////////////////////////
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool LocateCentersOfMainDefense(UObject* WorldContextObject, const TArray<FVector2D>RegionPoints, const TArray<FVector2D>ShowerAreaPoints, TArray<FVector2D>&Centers);



};
