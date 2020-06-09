
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
	/////////////////������ǽ�Ĺ�ϵ///////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Suite",meta = (WorldContext = "WorldContextObject"))
		static TArray<FWallBindInfo> GetWallBingInfo(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static FWallBindInfo GetWallBingInfoByWall(const ALineWallActor* Wall);
	UFUNCTION(BlueprintCallable, Category = "Suite",meta = (WorldContext = "WorldContextObject"))
		static TArray<FRoomBindInfo> GetRoomBindInfo(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static FRoomBindInfo GetRoomBindInfoByRomm(const ARoomActor* Room);
	//���������������ǽ���
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static float GetWallAcreageByRoom(const ARoomActor* Room);
	UFUNCTION(BlueprintCallable, Category = "Suite",meta = (WorldContext = "WorldContextObject"))
		static FRoomBindInfo GetDesignationRoomBindInfo(const UObject* WorldContextObject,ARoomActor * Room);
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static FWallBindInfo GetDesignationWallBindInfo(const UObject* WorldContextObject, ALineWallActor * Wall);
	/* Wall Surface Acreage*/
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
		static FWallBindInfo GetDesignationWallSurfaceAcreage(const UObject* WorldContextObject, ALineWallActor * Wall);
	//���ڵ��ǽ�淵�������ڴ������ڵ�ǽ����Ϣ�����ڸ�ǽ����Ϳ�ϣ�
	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	static FRoomBindInfo GetRoomByHitWallIndex(ALineWallActor* Wall, const int32& HitWallIndex);

	//�����ύ�������ռ�ǽ����Ϣ
	UFUNCTION(BlueprintPure, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	static FWallCollectionHandle GetWallSectionCollection(const FWallSufaceInfo& WallSurfaceInfo, const FDRMaterial& SurfaceMatNode);

	UFUNCTION(BlueprintPure, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	static EWallSurfaceType WallType2SurfaceType(EWallSufaceInfoType SurfaceType);
	/////////////////������ǽ�Ĺ�ϵ///////////////////////////////////

	///////////////////////	����1.0������ǽ������	/////////////////////////
	////	ǽ�յ�		 ������4
	////	 3
	////	|��|
	////0	|��| 1
	///		|��|
	///		 2
	///	   ǽ���		������5
	//////////////////////	����1.0������ǽ������	/////////////////////////

	///////////////////////	����2.0������ǽ������	/////////////////////////
	////	ǽ�յ�		 ������1
	////	 3
	////	|��|
	////5	|��| 4
	///		|��|
	///		 2
	///	   ǽ���		������0
	//////////////////////	����2.0������ǽ������	/////////////////////////
	//2.0ǽ������ת1.0ǽ��������
	UFUNCTION(BlueprintPure, Category = "Suite")
		static int32 GetCompatibleWallIndex(EWallSufaceInfoType NewIndexType);
	//2.0ǽ������ת1.0ǽ��������
	UFUNCTION(BlueprintPure, Category = "Suite")
		static int32 NewWallIndexToOldWallIndex(const int32& NewWallIndex);
	// 1.0ǽ������ת2.0ǽ������
	UFUNCTION(BlueprintPure, Category = "Suite")
	static int32 OldWallIndexToNewWallIndex(const int32& OldIndex);
	UFUNCTION(BlueprintPure, Category = "Suite")
	static EWallSufaceInfoType GetWallSurfaceTypeByIndex(const int32& Index);
	UFUNCTION(BlueprintPure, Category = "Suite")
	static UMaterialInterface*	GetMaterialInterfaceByResID(FString ResID);

	UFUNCTION(BlueprintCallable, Category = "Suite")
		static float GetWallSurfaceActeage(ALineWallActor * Wall , int32 SurfaceIndex);

	//��ȡUProceduralMeshComponent��2dƽ���ϵĵ�
	UFUNCTION(BlueprintPure, Category = "Suite")
		static TArray<FVector2D> GetVerticesBySectionIndex(UPARAM(ref) UProceduralMeshComponent* PMesh, UPARAM(ref) int32 SectionIndex);

	//�ж�ǽ���Ǹ������������ڣ���������ڣ�����-1
	UFUNCTION(BlueprintPure, Category = "Suite")
		static int32 GetNearSection(UPARAM(ref) UProceduralMeshComponent* AreaMesh, UPARAM(ref) UProceduralMeshComponent* WallMesh, UPARAM(ref) int32 SectionIndex);
	//�ж�ǽ�����������ڵ�����
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
	//ͨ��ResID��ȡ����
	UFUNCTION(BlueprintCallable, Category = "Suite")
		static UMaterialInterface* GetUE4MatByResID(const FString& ResID, const int32& MaterialIndex = 0);


	//��2D�����ϵ�ǽ�ϵı��, LinePos��3D�����
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