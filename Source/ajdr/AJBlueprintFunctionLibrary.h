// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VaRestJsonObject.h"
#include "Struct/Struct.h"
#include "AJBlueprintFunctionLibrary.generated.h"


#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)
UCLASS()
class AJDR_API UAJBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "AJUtil")
	static void  openFolder(FString foldername);

	UFUNCTION(BlueprintCallable, Category = "AJUtil")
		static void  openUrl(FString url);

	UFUNCTION(BlueprintCallable, Category = "AJUtil")
	static bool InsidePolygon(TArray<FVector2D> points, FVector2D p);

	UFUNCTION(BlueprintCallable, Category = "AJUtil")
	static TArray<FVector2D> SpreadPolygon(TArray<FVector2D> points, int dist);

	UFUNCTION(BlueprintCallable, Category = "AJUtil")
		static TArray<FVector2D> IntersectPolygon(TArray<FVector2D> polygon1, TArray<FVector2D> polygon2, TArray<FVector2D> outpoints);

	UFUNCTION(BlueprintCallable, Category = "AJUtil")
		static bool IsPointInPolygonBoundary(TArray<FVector2D> points, FVector2D p);

	UFUNCTION(BlueprintCallable, Category = "AJUtil")
	static bool AssignMeshComponent(UActorComponent *Component, int32 FaceIndex, UMaterialInterface *NewMaterial);

	UFUNCTION(BlueprintCallable, Category = "AJUtil")
	static UActorComponent *GetComponentByName(AActor *InActor, TSubclassOf<UActorComponent> FoundClass, const FString &CompName);

	UFUNCTION(BlueprintCallable, Category = "AJUtil")
	static FString GetComponentName(UActorComponent *Comp);
	
	//获取向量在世界坐标的轴向
	UFUNCTION(BlueprintCallable, Category = "AJUtil")
	static FVector2D DirectionInWorld(FVector2D mvector);
	/*
	@复制信息到剪切板
	@Ctrl+C
	@author DCF
	*/
	UFUNCTION(BlueprintCallable, Category = "AJUtil")
		static void CopyMessageToClipboard(FString text);

	/*
	@从剪切板粘贴信息
	@Ctrl+V
	@author DCF
	*/
	UFUNCTION(BlueprintCallable, Category = "AJUtil")
	static FString PasteMessageFromClipboard();

	UFUNCTION(BlueprintCallable, Category = "Mesh")
	static FBox GetMeshExtent(UStaticMesh *InMesh);

	UFUNCTION(BlueprintCallable, Category = "String")
	static int32 GetStringLength(FString str, bool &isSuccess);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Construct Json Object From FString", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "Json")
		static bool GetJsonObjectFromFString(FString data, UVaRestJsonObject* VaRestJsonObject);

	//检测网络是否联通
	//UFUNCTION(BlueprintCallable, Category = "NetWork")
	//static bool CheckNetWorkLink();
	UFUNCTION(BlueprintCallable, Category = "Time")
	static FString DateTimeToString(FDateTime DateTime);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (Size)", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math|Size")
	static bool EqualEqual_SizeSize(FSize A, FSize B, float ErrorTolerance = 1.e-4f);

	UFUNCTION(BlueprintCallable, Category = "Area")
	static float GetAreaFromPoints(TArray<FVector2D> Points);

	UFUNCTION(BlueprintPure, Category = "Price")
	static bool CheckIsShowPrice(ESlateVisibility &VisibleState);

	UFUNCTION(BlueprintCallable, Category = "Price")
	static void SetIsShowPrice(bool IsShow);

	UFUNCTION(BlueprintCallable, Category = "Adsorption", meta = (WorldContext = "WorldContextObject"))
	static bool Adsorption(UObject* WorldContextObject, const FVector InActorLoction, const FVector InActorOrigin, const FVector InMoudelFilBounds, const FVector InActorBounds, const FRotator InOrientation, const FVector InDirection, const TArray<AActor*> ActorsToIgnore, const TArray<UBuildingComponent*> AreaComponent, FVector& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "GetOverlapActors", meta = (WorldContext = "WorldContextObject"))
	static void GetOverlapActors(UObject* WorldContextObject, const FVector InActorOrigin, const FVector InMoudelFilBounds, const FVector InActorBounds, const FRotator InOrientation, TArray<AActor*> &ActorsToIgnore, TArray<ARoomActor*> &AreaToIgnore,TArray<UBuildingComponent*> &AreaComponent);

	UFUNCTION(BlueprintCallable, Category = "Geometry")
	static bool IsPolyClockWise(const TArray<FVector2D>& InPolygons);

	UFUNCTION(BlueprintCallable, Category = "Geometry")
	static void ChangeClockwise(TArray<FVector2D>& InPolygons);

	//区域合并忽看会眼瞎
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Complex Combine Area"), Category = "Geometry")
	static bool ComplexCombineArea(const TArray<FVector2D>& InFristArea, const TArray<FVector2D>& InSecondArea, TArray<FVector2D>& OutCombineArea);

	UFUNCTION(BlueprintCallable, Category = "DisplayLog")
		static void DisplayLog(const FString& LogStr);

	/**
	*写入目录固定在：ProjectSavedDir()+ "Config/WindowsNoEditor/" 下
	*FileName: projectConfig.ini
	*SaveText: quality=1
	**/
	UFUNCTION(BlueprintCallable, Category = "Config")
		static bool AddConfig(FString FileName, FString Key, FString Value);
	
	/**
	*读取目录固定在：ProjectSavedDir()+ "Config/WindowsNoEditor/" 下
	*FileName: projectConfig.ini
	*Key: quality  @return:(FString)1
	**/
	UFUNCTION(BlueprintCallable, Category = "Config")
		static bool ReadConfig(FString FileName, FString Key, FString& OutValue);

	static bool GetBPInt(const UObject* Obj, FString PropertyName, int32& PropertyValue);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "StringToTransform", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|String")
		static bool Conv_StringToTransform(const FString &InStr, FTransform &outtransform);

	//如果Ceter的位置是原点，那么输出的是相对位置，如果是世界位置，那么输出的也是世界位置
	UFUNCTION(BlueprintPure, Category = "BoundPoints")
		static void GetBoundPointsByBoxExtent(const FVector& BoxExtent, const FTransform& Center, TArray<FVector>& BoundPoints);
	UFUNCTION(BlueprintPure, Category = "BoundPoints")
		static void GetBoundPointsByBoxBound(const FVector& Min, const FVector& Max, const FTransform& Center, TArray<FVector>& BoundPoints);

	UFUNCTION(BlueprintPure)
		static FString GetCurrentPCIPAddress();

private:
	static bool LaunchRay(UObject * WorldContextObject, FVector& HitPos, const FVector InActorOrigin, const FVector InDirection, const FRotator InOrientation, const FVector InHalfSize, const TArray<AActor*>& ActorsToIgnore);
	static bool MultiLaunchRay(UObject * WorldContextObject, TArray<FHitResult>& OutHits, const FVector InActorOrigin, const FVector InDirection, const FRotator InOrientation, const FVector InHalfSize, const TArray<AActor*>& ActorsToIgnore);
	static FVector CalculateActorPosition(const FVector InDirection, const FVector InHitPos, const FVector InActorOrigin, const FVector InActorBounds, const FVector InActorLocation);
	static FVector FVectorAbsValue(const FVector InFVector);
	static FVector GetHalfSizeByDirection(const FVector InDirection, const FVector InBounds);
	static bool IsPositiveDirection(const FVector InFVector);
};
