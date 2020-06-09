// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../CGALWrapper/PolygonAlg.h"
#include "../HomeLayout/HouseArchitect/WallBuildSystem.h"
#include "Building/HouseComponent/ComponentManagerActor.h"
#include "Building/HouseComponent/ComponentBasePrimitive.h"
#include "Building/DRStruct.h"
#include "PlaneHousePicture.generated.h"

/**
 *
 */

USTRUCT(Blueprintable)
struct FRegion2D
{
	GENERATED_BODY()

	FRegion2D()
	{

	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> InnerPoints;

};


USTRUCT(Blueprintable)
struct FBoundarySeg2D
{
	GENERATED_BODY()

	 FBoundarySeg2D()
	{

	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D  SegStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D  SegEnd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D VerticalDir;
};



USTRUCT(Blueprintable)
struct FBoundarySegsArray
{
	GENERATED_BODY()

	FBoundarySegsArray()
	{

	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FBoundarySeg2D> BoundarySegs;
};



USTRUCT(Blueprintable)
struct FWallData2D
{
	GENERATED_BODY()

	FWallData2D()
	{

	}
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PStartLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PEndLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PStartRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PEndRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PStartPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PEndPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool BStartCorner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool BEndCorner;

};


USTRUCT(Blueprintable)
struct FOpeningsOnWall
{
	GENERATED_BODY()

	FOpeningsOnWall()
	{

	}
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PStartLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PEndLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PStartRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PEndRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool HorizontalFlip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool VerticalFlip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EOpeningType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FLinePos> Lines;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EHolestatusType HoleStatusType;
};

USTRUCT(Blueprintable)
struct FAreaInnerWall
{
	GENERATED_BODY()

		FAreaInnerWall()
	{

	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 WallID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D  SegStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D  SegEnd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D VerticalDir;
};

USTRUCT(Blueprintable)
struct FCustomizationBoolean
{
	GENERATED_BODY()

		FCustomizationBoolean()
	{

	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 HoleID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PStartLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PEndLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EOpeningType Type;
};

//@liff add housePluginInfo type
USTRUCT(Blueprintable)
struct FHousePluginInfo
{
	GENERATED_BODY()
		FHousePluginInfo()
	{

	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 PluginID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PLeftUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PLeftDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PRightDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D PRightUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EComponenetType Type;
};
//@liff add end


UCLASS()
class AJDR_API UPlaneHousePicture : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool ProjectWallLines(UObject* WorldContextObject, const TArray<FBoundarySegsArray> InnerWallGroups,
			TArray<FBoundarySegsArray>&ProjectionWallLines, TArray<FBoundarySeg2D>& GlobalSegs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool  GetAllRegions(UObject* WorldContextObject, TMap<int32, FRegion2D>& Regions);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool  GetAllRegionOuter(UObject* WorldContextObject, TArray<FBoundarySeg2D>& BoundaryLines);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetAllInnerWalls(UObject* WorldContextObject, TArray<FBoundarySeg2D>& InnerWallLines);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool DivideInnerWalls(UObject* WorldContextObject, const TArray<FBoundarySeg2D> InnerWallLines, TArray<FBoundarySegsArray>&InnerWallGroups);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CalculateCameraCenter(UObject* WorldContextObject, const TArray<FBoundarySeg2D> InnerWallLines, FVector2D& RegionCenter, float& Radius);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool CalculateRegionCenters(UObject* WorldContextObject, UVaRestJsonObject* InJson, TArray<FVector2D>& RegionCenters, TArray<FString>& RoomUsageNameSet);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetAllWallsData2D(UObject* WorldContextObject, TArray<FWallData2D>& WallData2D);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetOpeningsOnWalls(UObject* WorldContextObject, TArray<FOpeningsOnWall>& OpeningsOnWalls,bool IsChaigai=false);


	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetBayYeWindowsOnWalls(UWorld* MyWorld,TArray<AWall_Boolean_Base*>& BayYeWindows);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetAllWindowsOnWalls(UWorld* MyWorld, TArray<AWall_Boolean_Base*>& Windows);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool FilterInnerWallLines(const TArray<FBoundarySeg2D> InWallLines, TArray<FBoundarySeg2D>& FilteredWallLines);
	
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool GetBayYeWindowRegion(UWorld* MyWorld, AWall_Boolean_Base* OpeningsOnWall, TArray<FVector2D>&RegionPs, TArray<FVector2D>BoundaryPs);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetAreaOrderData(const TArray<FVector2D> InnerWallPoints, TArray<ALineWallActor*>BWalls, TArray<FAreaInnerWall>&outWallLines, TArray<FCustomizationBoolean>&outBooleanData);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "LineWallActor")
		static int32 IsPointNearstWallID(const TArray<ALineWallActor*> BWalls, const FVector2D point, float &mindx);
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetCustomizationBooleanByWalls(const TArray<ALineWallActor*>InBWalls, TArray<FCustomizationBoolean>&outBooleanData);
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetHousePluginData(UObject* WorldContextObject, TArray<FHousePluginInfo>&outPlugData);
};
