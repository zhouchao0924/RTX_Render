
#pragma once
#include "DRComponentModel.h"
#include "DRStruct.h"
#include "DRModelFactory.h"
#include "Building/BuildingSystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HomeLayout/SceneEntity/FurnitureModelActor.h"
#include "BuildingData.h"
#include "DRActor.h"
#include "Building/BuildingSystem.h"
#include "HomeLayout/BusinessInterface/AreaCollection.h"
#include "HomeLayout/DataNodes/MaterialNode.h"
#include "HomeLayout/SceneEntity/RoomActor.h"
#include "DRFunLibrary.generated.h"

class ALineWallActor;
class ACornerActor;
class ARoomActor;
struct FDRMaterial;

UCLASS()
class UDRFunLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Suite")
	static FVector MaxVertex(const TArray<FVector> & Vertex);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	static FVector MinVertex(const TArray<FVector> & Vertex);
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
		static bool IsBuildDataModelType(UBuildingData * BuildData);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
		static bool IsBuildDataWindowHoleType(UBuildingData * BuildData);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
		static bool IsBuildDataDoorHoleType(UBuildingData * BuildData);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
		static bool IsBuildDataPointLightType(UBuildingData * BuildData);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
		static bool IsBuildDataSpotLightType(UBuildingData * BuildData);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
		static bool IsBuildDataSolidWallType(UBuildingData * BuildData);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
		static void IsBuildDataRoomType(UBuildingData * BuildData, bool & isFloorPlane, bool & isCeilPlane);
	//@liff add judge the BuildingData is housePlugin type or not
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
		static bool IsBuildDataHousePluginType(UBuildingData *BuildData);

	/*UFUNCTION(BlueprintCallable, Category = "Suite")
		static float GetHousePluginAreaAndWallArea(const int32 &PluginID, const TArray<int32> &WallIDs, UBuildingSystem* BS);*/
	//@liff add end
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void UpdateModelFileCom_Update(UModelFileComponent * ModelFile, UBuildingData * BuildData, FString ResID);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void UpdateModelFileChildCom(UModelFileComponent * ModelFile, UBuildingData * BuildData, FString ResID);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void GetWallAllHoles(int32 WallID, TArray<int32> & HoleIDs);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void SetBuildingSystemWorld(ADRGameMode * GameModel, UBuildingSystem * _Sys);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static bool IsPointInRoom(const  TArray<FVector> & Vertex, const FVector & Point);
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static void SetHoleHiddent(const UObject* WorldContextObject,bool isShow = false);
	/////////////////区域与墙的关系///////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Suite",meta = (WorldContext = "WorldContextObject"))
		static TArray<FWallBindInfo> GetWallBingInfo(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static FWallBindInfo GetWallBingInfoByWall(const ALineWallActor* Wall);
	UFUNCTION(BlueprintCallable, Category = "Suite",meta = (WorldContext = "WorldContextObject"))
		static TArray<FRoomBindInfo> GetRoomBindInfo(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static FRoomBindInfo GetRoomBindInfoByRomm(const ARoomActor* Room);
	//计算区域的所有内墙面积
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static float GetWallAcreageByRoom(const ARoomActor* Room);
	UFUNCTION(BlueprintCallable, Category = "Suite",meta = (WorldContext = "WorldContextObject"))
		static FRoomBindInfo GetDesignationRoomBindInfo(const UObject* WorldContextObject,ARoomActor * Room);
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static FWallBindInfo GetDesignationWallBindInfo(const UObject* WorldContextObject, ALineWallActor * Wall);
	/* Wall Surface Acreage*/
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static FWallBindInfo GetDesignationWallSurfaceAcreage(const UObject* WorldContextObject, ALineWallActor * Wall);
	//基于点击墙面返回所有在此区域内的墙面信息（用于给墙更换涂料）
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	static FRoomBindInfo GetRoomByHitWallIndex(ALineWallActor* Wall, const int32& HitWallIndex);

	//用于提交方案中收集墙面信息
	UFUNCTION(BlueprintPure, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	static FWallCollectionHandle GetWallSectionCollection(const FWallSufaceInfo& WallSurfaceInfo, const FDRMaterial& SurfaceMatNode);

	UFUNCTION(BlueprintPure, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	static EWallSurfaceType WallType2SurfaceType(EWallSufaceInfoType SurfaceType);
	/////////////////区域与墙的关系///////////////////////////////////

	///////////////////////	户型1.0数据中墙面索引	/////////////////////////
	////	墙终点		 顶面是4
	////	 3
	////	|↑|
	////0	|↑| 1
	///		|↑|
	///		 2
	///	   墙起点		底面是5
	//////////////////////	户型1.0数据中墙面索引	/////////////////////////

	///////////////////////	户型2.0数据中墙面索引	/////////////////////////
	////	墙终点		 顶面是1
	////	 3
	////	|↑|
	////5	|↑| 4
	///		|↑|
	///		 2
	///	   墙起点		底面是0
	//////////////////////	户型2.0数据中墙面索引	/////////////////////////
	//2.0墙面索引转1.0墙面索引。
	UFUNCTION(BlueprintPure, Category = "Suite")
		static int32 GetCompatibleWallIndex(EWallSufaceInfoType NewIndexType);
	//2.0墙面索引转1.0墙面索引。
	UFUNCTION(BlueprintPure, Category = "Suite")
		static int32 NewWallIndexToOldWallIndex(const int32& NewWallIndex);
	// 1.0墙面索引转2.0墙面索引
	UFUNCTION(BlueprintPure, Category = "Suite")
	static int32 OldWallIndexToNewWallIndex(const int32& OldIndex);
	UFUNCTION(BlueprintPure, Category = "Suite")
	static EWallSufaceInfoType GetWallSurfaceTypeByIndex(const int32& Index);
	UFUNCTION(BlueprintPure, Category = "Suite")
	static UMaterialInterface*	GetMaterialInterfaceByResID(FString ResID);

	UFUNCTION(BlueprintCallable, Category = "Suite")
		static float GetWallSurfaceActeage(ALineWallActor * Wall , int32 SurfaceIndex);

	//获取UProceduralMeshComponent的2d平面上的点
	UFUNCTION(BlueprintPure, Category = "Suite")
		static TArray<FVector2D> GetVerticesBySectionIndex(UPARAM(ref) UProceduralMeshComponent* PMesh, UPARAM(ref) int32 SectionIndex);

	//判断墙的那个面与区域相邻，如果不相邻，返回-1
	UFUNCTION(BlueprintPure, Category = "Suite")
		static int32 GetNearSection(UPARAM(ref) UProceduralMeshComponent* AreaMesh, UPARAM(ref) UProceduralMeshComponent* WallMesh, UPARAM(ref) int32 SectionIndex);
	//判断墙与面所有相邻的区域
	UFUNCTION(BlueprintPure, Category = "Suite")
		static TArray<int32> GetNearSectionList(UPARAM(ref) UProceduralMeshComponent* AreaMesh, UPARAM(ref) UProceduralMeshComponent* WallMesh);

	/////////////////////////////Set BuildingData/////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void SetObjIntValue(UPARAM(ref) UBuildingSystem* BS, const int32& ObjID, UPARAM(ref) FString& ValueName, const int32& IntValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void SetObjFloatValue(UPARAM(ref) UBuildingSystem* BS, const int32& ObjID, UPARAM(ref) FString& ValueName, const int32& FloatValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void SetObjFStringValue(UPARAM(ref) UBuildingSystem* BS, const int32& ObjID, UPARAM(ref) FString& ValueName, UPARAM(ref) FString& FStringValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void SetObjFVector(UPARAM(ref) UBuildingSystem* BS, const int32& ObjID, UPARAM(ref) FString& ValueName, UPARAM(ref)FVector& FVectorValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void SetObjFVector2D(UPARAM(ref) UBuildingSystem* BS, const int32& ObjID, UPARAM(ref) FString& ValueName, UPARAM(ref)FVector2D& FVectorValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static void SetObjFVector4D(UPARAM(ref) UBuildingSystem* BS, const int32& ObjID, UPARAM(ref) FString& ValueName, UPARAM(ref)FVector4& FVectorValue);
	/////////////////////////////Set BuildingData/////////////////////////////////////////////
	//通过ResID获取材质
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static UMaterialInterface* GetUE4MatByResID(const FString& ResID, const int32& MaterialIndex = 0);


	//画2D户型上的墙上的标尺, LinePos是3D坐标点
	UFUNCTION(BlueprintCallable, Category = "Wall | Line")
	static void DrawSubline(UPARAM(ref)FPaintContext& Context, const APlayerController* PC, UPARAM(ref)TArray<FVector> LinePos, FLinearColor Tint, bool bAntiAlias);

	//@liff add
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Wall | Line")
		static FString FloatToStringBy3Point(float num);

	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static TArray<AActor*> GetAllHousePlugins(const UObject* WorldContextObject);
	//@liff end

public:
	UFUNCTION(BlueprintCallable, Category = "Math")
		static bool IsPolyClockWise(const TArray<FVector2D> & Polygons);
	UFUNCTION(BlueprintCallable, Category = "Math")
		static void ChangeClockwise(const TArray<FVector2D> & InPolygons, TArray<FVector2D> & OutPolygons);
public:
	UFUNCTION(BlueprintCallable, Category = "Building Data")
	static void SetModelBuildingData(AFurnitureModelActor* FurnitureModelActor, UBuildingData* BuildingData);
	UFUNCTION(BlueprintCallable, Category = "Building Data")
	static void SetPointLightSturct(AFurnitureModelActor* FurnitureModelActor, const FDRPointLight& PointLightStruct);
	static void SetSpotLightSturct(AFurnitureModelActor* FurnitureModelActor, const FDRSpotLight& SpotLightStruct);
public:
	static void CalculateBooleanMaxMinLoction(const TArray<FVector>&WallNodes, FVector& _OutStartPos, FVector& _OutEndPos);

	static TArray<ALineWallActor*> GetWallActorArray(UObject* WorldContext);
	static TArray<ACornerActor*> GetCornerActorArray(UObject* WorldContext);
	static TArray<ARoomActor*> GetRoomActorArray(UObject* WorldContext);
public:
	UFUNCTION(BlueprintCallable)
		static void AddRoomIndex(const int32 & Index, const FString & Key, const TMap<FString, FRoomList>& Map, ARoomActor * R);
	UFUNCTION(BlueprintCallable)
		static void AddRoom(const FString & Key, const TMap<FString, FRoomList> & Map, ARoomActor * R);
	UFUNCTION(BlueprintCallable)
		static void DelRoom(const FString & Key, const TMap<FString, FRoomList> & Map, ARoomActor * R);
	UFUNCTION(BlueprintCallable)
		static void ChaRoom(const FString & SrcKey, const FString & Key, const TMap<FString, FRoomList> & Map, ARoomActor * R);
	UFUNCTION(BlueprintCallable)
		static float GetWallRotate(FVector2D StartPos, FVector2D EndPos);
};