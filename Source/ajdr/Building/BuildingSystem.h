// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISuite.h"
#include "ModelFileComponent.h"
#include "BuildingData.h"
#include "BuildingConfig.h"
#include "Math/kColor.h"
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "IBuildingSDK.h"
#include "DRStruct.h"
#include "DataType.h"
#include "HomeLayout/DataNodes/WallNode.h"
#include "DRStruct/DRBuildingStruct.h"
#include "BuildingSystem.generated.h"

class EditorGameInstance;
class UWallBuildSystem;
DECLARE_DYNAMIC_DELEGATE_OneParam(FBuildingObjectChangedDelegate, class UBuildingData *, Data);

USTRUCT(BlueprintType)
struct FObjectInfo
{
	GENERATED_BODY()
	
	FObjectInfo()
		:Data(NULL)
	{
	}
	
	FObjectInfo(UBuildingData *InData)
		:Data(InData)
	{
	}

	UPROPERTY()
	UBuildingData *Data;

	UPROPERTY()
	FBuildingObjectChangedDelegate Delegate;
	
	UPROPERTY()
	TArray<class ADRActor *>  Actorts;
};

class IBuildingSDK;

struct FSlateContext
{
	TSharedPtr<FSlateBrush>			WallBrush;
	FSlateResourceHandle			WallRenderingResourceHandle;
	TSharedPtr<FSlateBrush>			AreaBrush;
	FSlateResourceHandle			AreaRenderingResourceHandle;
};

class IBuildingVisitor
{
public:
	virtual void OnCheckObjectVisible(FObjectInfo &ObjInfo) = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBuildingPropertyChangedHander, UBuildingData *, Data, const FString &, PropName);

class ADRActor;
class ABuildingActor;

UCLASS(BlueprintType)
class UBuildingSystem :public UObject, public ISuiteListener, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Suite")
	bool LoadFile(const FString &InFilename);
	
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SaveFile(const FString &InFilename);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void ExportScene(const FString &InFilename);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void ClearSuite();

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void ForceUpdateSuit();

	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	void AddToWorld(UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Suite", meta = (WorldContext = "WorldContextObject"))
	void RemoveFromWorld(UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetRoomCeilVisible (bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	bool IsRoomCeilVisible();

	UFUNCTION(BlueprintCallable, Category = "Suite")
	UBuildingData *GetData(int32 ID);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddCorner(const FVector2D &Location);

	//通过两个墙角点ID获取墙ID
	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 GetWallObjectIDByTwoCorner(const int32& CornerStart, const int32& CornerEnd);

	//从1.0数据中读取材质，UV数据到BuildingData
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetWallInitData(FWallNode WallNode, const int32& WallObjID);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddModelToObject(int32 BaseObjID, const FString &ResID, const FVector &Location);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddNewModel(const FString& ResId, const FTransform& Transform, EModelType Type);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddModelToAnchor(int32 AnchorID, const FString &ResID, const FVector &Location);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	TArray<int32> AddWall(int32 StartCorner, int32 EndCorner, float ThickLeft = 20.0f, float ThickRight = 20.0f, float Height = 280.0f);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddWindow(int32 WallID, const FVector2D &Location, float zPos, float Width, float Height, const FString &WindowResID, const FString &SillsResID);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddDoor(int32 WallID, const FVector2D &Location, float Width, float Height, float zPos = 0);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddPointLight(const FVector &Location, float SourceRadius, float SoftSourceRadius, float SourceLength, float Intensity, FLinearColor LightColor, bool bCastShadow);
	
	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddSpotLight(const FVector &Location, const FRotator &Rotationn, float AttenuationRadius, float SourceRadius, float SoftSourceRadius, float SourceLength, float InnerConeAngle, float OuterConeAngle, float Intensity, FLinearColor LightColor, bool bCastShadow, float ColorTemperature = 6500.0f, float Highlight = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	TArray<int32> AddEdge(const FVector2D &P0, const FVector2D &P1, int32 SnapObj0 = -1, int32 SnapObj1 = -1, bool bBreak = false);

	//@liff add houseComponent operation function start
	UFUNCTION(BlueprintCallable, Category = "Suite")
		int32 AddFlue(const FVector2D &Location, float Width, float Length, float Height = 280.0);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		int32 AddTube(const FVector2D &Location, float Width, float Length, float Height = 280.0);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		int32 AddPillar(const FVector2D &Location, float Width, float Length, float Height = 280.0);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		int32 AddSewer(const FVector2D &Location, float Diameter);

	/*update houseComponent attribute value and movement*/
	UFUNCTION(BlueprintCallable, Category = "Suite")
		bool UpdateFlueProperty(int32 ObjID, const FVector2D &Location, float Angle, float Width = -1.0, float Length = -1.0);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		bool UpdateTubeProperty(int32 ObjID, const FVector2D &Location, float Angle, float Width = -1.0, float Length = -1.0);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		bool UpdatePillarProperty(int32 ObjID, const FVector2D &Location, float Angle, float Width = -1.0, float Length = -1.0);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		bool UpdateSewerProperty(int32 ObjID, const FVector2D &Location, float Diameter = -1.0);

	/*update houseComponent when the anchor wall is moved*/
	UFUNCTION(BlueprintCallable, Category = "Suite")
		bool UpdateFlue(int32 ObjID, const FVector2D &LocationVal, float RotateAng);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		bool UpdateTube(int32 ObjID, const FVector2D &LocationVal, float RotateAng);
	UFUNCTION(BlueprintCallable, Category = "Suite")
		bool UpdatePillar(int32 ObjID, const FVector2D &LocationVal, float RotateAng);

	/*set houseComponent material data*/

	UFUNCTION(BlueprintCallable, Category = "Suite")
		void UpdateCorner();

	UFUNCTION(BlueprintCallable, Category = "Suite")
		bool SetHousePluginSurface(int32 ObjID, int32 MaterialChannel, const FString &MaterialUri, int32 MaterialType);

	bool SetFlueSurface(int32 ObjID, int32 SectionIndex, const FString &MaterailUri, int32 MaterialType);
	bool SetTubeSurface(int32 ObjID, int32 SectionIndex, const FString &MaterailUri, int32 MaterialType);
	bool SetPillarSurface(int32 ObjID, int32 SectionIndex, const FString &MaterailUri, int32 MaterialType);

	/*set houseComponent height data*/
	UFUNCTION(BlueprintCallable, Category = "Suite")
		bool SetHousePluginHeight(int32 ObjID, float height);

	/*get wall height data*/
	UFUNCTION(BlueprintCallable, Category = "Suite")
		float GetWallHeight();

	/*delete houseComponent*/
	void DeleteFlueObj(int32 ObjID);
	void DeleteTubeObj(int32 ObjID);
	void DeletePillarObj(int32 ObjID);
	void DeleteSewerObj(int32 ObjID);

	/*return the houseComponent 3d model value to 2d primitive*/
	int GetPluginTrans(int32 ObjID, float* ValArr);
	void GetHousePluginBorder(int32 ObjID, kVector3D* BorderArray);

	/*judge the distance to wall of houseComponent is equal to width or not*/
	bool IsWallWidth(int32 WallID, const FVector2D &LocVal, float ThickVal, float Width, float Length);

	/*load data from mx file*/
	void LoadMxModel(UModelFileComponent *InModelFileComponent, UBuildingData *Data);
	//@liff houseComponent operation end

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetSkyLight(const FDRSkyLight & Sky,const FDirectionLight & Direction);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetPostProcess(const FPostProcess & Post);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32  FindCloseWall(const FVector2D &Location, float Width, FVector2D &BestLoc,float Tolerance = -1.0f);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void Build();

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetChannelVisbile(EVisibleChannel Channel, bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetHouseImage(FDRHouseImage _HI);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void GetHouseImage(FDRHouseImage & HouseImage);

	UFUNCTION(BlueprintPure, Category = "Suite")
	TArray<int32> GetAllTypedObjects(EDR_ObjectType InClass = EDR_ObjectType::EDR_UnkownObject, bool bIncludeDeriveType = true);

	int32 GetAllObjects(IObject** &ppOutObject, EObjectType InClass = EUnkownObject,bool bIncludeDeriveType=true);

	void LoadingConfig(FBuildingConfig * Config);
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void GetAllCornerActorLoction(TArray<FVector2D> &OutAllCornerActorLoction, int32 objectID);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void GetWallBorderLineLocation(TArray<FVector2D> &OutAllCornerActorLoction);

	ObjectID IsHitCornner(const FVector2D &Location);

	ObjectID HitTest(const FVector2D &Location);

	bool Move(ObjectID objID, const FVector2D &DeltaMove);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 GetPolygon(int32 objID, TArray<FVector2D>& TPolygons, bool binner);

	int	HitTestMinMax(const FVector2D &Min, const FVector2D &Max, int32 *&pResults);

	IValue* GetProperty(ObjectID ID, const char *PropertyName);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 GetRoomByLocation(const FVector2D &Loc);

	UFUNCTION(BlueprintCallable, Category = "Suite", meta=(WorldContext="WorldContextObject"))
	static UPrimitiveComponent *HitTestPrimitiveByCursor(UObject *WorldContextObject);

	bool GetWallVector(int32 WallID, FVector2D &P0, FVector2D &P1, FVector2D &Right);
	
	UFUNCTION(BlueprintCallable, Category = "Suite")
	TArray<int32>  AddVirtualWall(int32 StartCorner, int32 EndCorner);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetConfigTolerance(const float Num);

	UFUNCTION(BlueprintCallable, Category = "Suite")
		void SetWallHightConfig(float Height);


	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetConfigWallHeight(float WallHeight);

	int32 GetConnectWalls(int32 objID, int32 *&pConnectedWalls);

	bool GetWallBorderLines(int32 WallID, TArray<FVector>&WallNodes,float & ZPos);
	
	void OnUpdateSurfaceValue(IObject *RawObj, int SectionIndex, ObjectID SurfaceID);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	UWallBuildSystem* GetWallBuildSystem();
	IObject *GetObject(int32 ObjID);

	UFUNCTION(BlueprintPure, Category = "Suite")
	EDR_ObjectType GetObjectType(int32 ObjectId);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void DeleteObject(int32 ObjID);
	ISuite *GetSuite() { return Suite; }
	bool IsFree(int32 ObjID);
	void Tick(float DeltaTime);
	bool IsTickable() const { return true;}
	int32 Snap(const FVector2D &Loc, int32 Ignore, FVector2D &BestLoc, int32 &BestID, float Torlerance, float ToleranceEdge);
	int32 CutAreaSnap(const FVector2D &Loc, int32 Ignore, FVector2D &BestLoc, int32 &BestID, float Torlerance = 0.1f, float ToleranceEdge = -1.0f);
	TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(UBuildingSystem, STATGROUP_Tickables); }
	void OnPropertyChanged(UBuildingData *Data, const FString &PropName, const IValue &NewValue);
	void SetSelected(ObjectID ObjID, bool bSelected);
	static UBuildingSystem *LoadNewSuite(UObject *Outer, const FString &InFilename);
	static UBuildingSystem *CreateNewSuite(UObject *Outer, const FString &InFilename = TEXT(""));
	static IBuildingSDK  *GetBuildingSDK();
	static IValueFactory *GetValueFactory();
	UWorld *GetWorld() const override;
	void SetVisitor(IBuildingVisitor *Visitor);
	void UpdateChannelVisible();
	FVector2D CalculateCrossoverpoint(FVector2D Line1Point1, FVector2D Line1Point2, FVector2D Line2Point1, FVector2D Line2Point2);
	int32 Break(int32 EdgeID, FVector2D &Location);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetObjIntValue(const int32& ObjID, UPARAM(ref) FString& ValueName, const int32& IntValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetObjFloatValue(const int32& ObjID, UPARAM(ref) FString& ValueName, const int32& FloatValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetObjFStringValue(const int32& ObjID, UPARAM(ref) FString& ValueName, UPARAM(ref) FString& FStringValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetObjFVector(const int32& ObjID, UPARAM(ref) FString& ValueName, UPARAM(ref)FVector& FVectorValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetObjFVector2D(const int32& ObjID, UPARAM(ref) FString& ValueName, UPARAM(ref)FVector2D& FVectorValue);
	UFUNCTION(BlueprintCallable, Category = "Suite")
	void SetObjFVector4D(const int32& ObjID, UPARAM(ref) FString& ValueName, UPARAM(ref)FVector4& FVectorValue);

	UFUNCTION(BlueprintPure, Category = "Suite")
	UObject* FindAreaByChildId(int32 ObjectId, bool bIsFloor);

protected:
	void LoadObjInfo();
	void OnAddObject(IObject *RawObj);
	void OnDeleteObject(IObject *RawObj);
	void OnUpdateObject(IObject *RawObj, unsigned int ChannelMask);
	int  GetExternalChunkID() override { return 0x8fffffff; }
	void OnSwitchLayer(ISuite *InSuite, int LayerIndex) override { }
	void Serialize(ISerialize &Ar) override { }
	void ClearObjInfo(FObjectInfo *ObjInfo);
	FObjectInfo *NewSuiteData(IObject *RawObj);
	ADRActor *SpawnActorByObject(UWorld *World, FObjectInfo &ObjInfo);
	int32 FindHostWorld(UWorld *World);
	ADRActor *SpawnPrimitiveComponent(UWorld *MyWorld, FObjectInfo &ObjInfo, int ObjectType);
	ADRActor *SpawnModelComponent(UWorld *MyWorld, FObjectInfo &ObjInfo);
	ADRActor *SpawnPluginComponent(UWorld *MyWorld, FObjectInfo &ObjInfo, int ObjectType);
public:
	UPROPERTY(BlueprintReadOnly, Category = "Suite")
	FString						Filename;
	
	UPROPERTY(BlueprintReadOnly, Category = "Suite")
	TMap<int32, FObjectInfo>	ObjMap;

	UPROPERTY(Transient, BlueprintAssignable, Category = "3D")
	FBuildingPropertyChangedHander EventPropertyChangedHandler;
	UPROPERTY(Transient, BlueprintReadWrite)
	UObject						*BuilldActorFactory;

	IBuildingVisitor			*Visitor;
	TArray<FWeakObjectPtr>		HostWorlds;
	ISuite						*Suite;
	FSlateContext				SlateContext;
	static	IBuildingSDK		*BuildingSDK;
};

