// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UseWidget/ComponentBaseWidget.h"
#include "IClass.h"
#include "HomeLayout/UserInterface/ComponentPositionWidget.h"
#include "ComponentManagerActor.generated.h"

USTRUCT(Blueprintable)
struct FComponentData
{
	GENERATED_BODY()

		FComponentData()
	{

	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EComponenetType  componenttype;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> fourpoint;
};

USTRUCT(BlueprintType)
struct FRoomVecData
{
	GENERATED_USTRUCT_BODY()
public:
	FRoomVecData()
		:AngleVal(0)
		, IsWidth(false)
		, wallId(INDEX_NONE)
	{
	}
	UPROPERTY(BlueprintReadOnly)
		FVector2D startPos;
	UPROPERTY(BlueprintReadOnly)
		FVector2D endPos;
	UPROPERTY(BlueprintReadOnly)
		float AngleVal;
	UPROPERTY(BlueprintReadOnly)
		bool IsWidth;
	UPROPERTY(BlueprintReadOnly)
		int32 wallId;
	UPROPERTY(BlueprintReadOnly)
	FVector2D  rightDir;
	void InitData(class AComponentManagerActor* Manager, const FVector2D& P0, const FVector2D& P1);
	float operator-(const FVector2D& V)
	{
		return FMath::PointDistToSegment(FVector(V, 0), FVector(startPos, 0), FVector(endPos, 0));
	}
	bool operator==(const FRoomVecData& vec)
	{
		if (vec.startPos == this->startPos&&vec.endPos == this->endPos)
			return true;
		return false;
	}
};
UCLASS()
class RTXRENDER_API AComponentManagerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AComponentManagerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed();
	void InitBuildSystem();
	FVector2D GetMouse2D(const FVector2D& screenPos);
	EObjectType FindWalls(const FVector2D& Loc, int32& WallID);
	void NewComponent(UWorld* MyWorld, const FVector2D& screenPos);//在添加状态下，更�?2D图元的位�?


	bool DetectHitPrimitive2D(const FPrimitive2D& primintivePos, const FVector2D& mousePos);//检测是否发生碰�?
	FPrimitive2D GetPrimitive2DWithAngle(const FPrimitive2D&  inPrimitive, float Angle = 0.0f);//获得旋转后的四个点的坐标
	FPrimitive2D GetWorldPosBySrceen(FPrimitive2D PosNodes);
	FVector2D WorldLocationToScreen(UWorld* MyWorld, const FVector2D& PrimitiveNodes);
	void Update();

	bool getDataForLayout(FComponentData &data);
public:
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void Create2dPrimitiveWidget(EComponenetType type);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		bool IsAllowPlacement();
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		bool bAllowPlaceComp();    //only judge the is allowed and not addcomponent
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void FirstUpdate();
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void SnapComponentLoc(const int32& WallNum);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		bool AddHouseComponent(const FVector2D &Location, float Width, float Length, bool IsFirst=true);
	void SetHousePluginDefMaterial(int32 ObjID, const FString &MaterialUri = "");
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void UpdateComponent(UWorld* MyWorld, int32 ObjectID, const FVector2D&Loc, float Width = -1.0f, float Length = -1.0f);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void ChangeName(const FString& Name);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void ChangeSize(float Val);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void ChangleWidth(float Val);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void ChangleLength(float Val);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Component Base Actor")
		void SelectComponentEvent(EComponenetType type);
	UFUNCTION(BlueprintImplementableEvent, Category = "Component Base Actor")
		void CanCelComponentEvent(EComponenetType type);
	UFUNCTION(BlueprintImplementableEvent, Category = "Component Base Actor")
		void DeleteComponentEvent(EComponenetType type);
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Component Base Actor")
		bool IsHousePluginType();
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void SetPluginTopMaterial(bool boolean);
	UFUNCTION(BlueprintImplementableEvent, Category = "Component Base Actor")
		void InitMaterial(const int32& ModelID);

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void HiddenPrimitiveWidget(bool boolean);

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void RevertComponenet(const FComponenetInfo& Info);

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		bool GetHouseComponent(int32 RoomID, TArray<FVector2D>& Nodes);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		bool GetAllHouseComponent(TArray<FVector2D>& Nodes);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		bool GetAllPillars(TArray<FVector2D>& Nodes);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void LeftMouseDown();
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void LeftMouseUp();
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void ClearSelect();
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		FComponenetInfo GetHouseComponentInfo() const;
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void DeleteHouseComponent();
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		FComponenetInfo GetComponenetInfo() const;
	void ManualDestroyHouseComponent();
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void SetHousePluginMaterial(int32 Index, UMaterialInterface* InMaterial);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		FString GetHousePluginArea(float acreage);

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		FString GetHousePluginAreaWithOutWall();

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void GetHousePluginAreaWithWall(const TArray<int32>&Wallindex, FString& AddArea);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Component Base Actor")
		float GetHousePluginOneSurfaceArea(const FVector WallStart, const FVector WallEnd);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Component Base Actor")
		FString FloatToStringBy3Point(float num);

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void SetHousePluginActorMaterialInfo(const int32 modelID, const int32& RoomClassID, const int32& CraftID);

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void SetHousePluginActorMaterialUVInfo(FVector Scale, FVector Offset, float Angle);

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void SetHousePluginActorDefaultScale();

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void SetHouseComponentInfoHeight(float Height);

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void ResetHousePluginMaterialUV(FVector Scale, FVector Offset, float Angle);

	void GetDirByWall(FVector& RayStart, FVector& RayDir);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void SetMaterialInfo(int32 MaterialChannel, const FString& ResID, int32 modelID, const int32& RoomClassID, const int32& CraftID);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void SetMaterialUVInfo(FVector Scale, FVector Offset, float Angle);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		EComponenetState GetState();
	TMap<int32, FString> GetMaterialInfo() const;
	void DeleteSelectRangeComponent(const FVector2D& Min, const FVector2D& Max);
	bool IsInBoxRange(const FVector2D& Min, const FVector2D& Max, const FPrimitive2D& FramePrimitive);
	//bool RefreshWall(const FPrimitive2D&  inPrimitive);
	int HitPolygon(int nvert, float *vertx, float *verty, float testx, float testy);
	/**
	*  除去重复�?
	*/
	int GetPolygonNoCommonPoint(int32 ObjecID, TArray<FVector2D>& TPoylgon);
	inline int FindRoomVecData(const FRoomVecData& data);
	void UpdatePlygon(int32 id);

	inline void GetNewPos(UComponentBasePrimitive* ComponentBase,const FRoomVecData& data, FVector2D& P0, FVector2D& P1);
	void UpdateObject(/*int32 id, */int32 wallID);
	//@liff add test plugin delete event
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
	void DeleteObject(int32 id);

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void RefreshPolygonData(/*int32 polyId*/);
	//add end
	//void FindWallByCreate(const FVector2D& Loc,const float& Length,const float& Width);
	bool CalculateSegmentAngle(const FRoomVecData& Predata, const FRoomVecData& Curdata,float& Angle);
	float GetPluginAreaAtWall(int32 id);
	void MovePlugin(UWorld* MyWorld, const FVector2D&Loc, float Width, float Length);
	void SetHousePlugin(class AHousePluginActor * HousePl);
	void UpdateHousePlugin(int32 id);
	void CalcuIsWidth(bool boolean, const FRoomVecData& firstData, const FRoomVecData& secData);
	void UpdateArea(int32 id);
	void UpdateComponentUUID(UComponentBasePrimitive *base);
	void SetWallIdsInfo(const TArray<int32>&WallIDs);
	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
	FString GetComponentUUID();

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		void GetComponentPrimitive2DPoints(TArray<FVector2D> &Points) const;

	UFUNCTION(BlueprintCallable, Category = "Component Base Actor")
		FBox2D GetComponentPrimitive2DBox() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Component Widget")
		void ShowComponentPositionWidget();
	UFUNCTION(BlueprintImplementableEvent, Category = "Component Widget")
		void AddWidgetToViewport();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component Base Actor")
		float CameraAngleVal;
	UPROPERTY()
		FPrimitive2D FramePrimitiveWithAngle;//外框,带角�?
	UPROPERTY()
		UComponentBaseWidget* ComponentBaseWidget;
	UPROPERTY(BlueprintReadOnly, meta = (Category = "Property"))
		class UBuildingSystem  *BuildingSystem;
	UPROPERTY(BlueprintReadOnly, meta = (Category = "Property"))
		class AHousePluginActor * HousePluginActor;
	UPROPERTY()
	bool bLeftMouseActive;
	UPROPERTY()
	TArray<FRoomVecData> roomVecDatas;
	UPROPERTY()
	FRoomVecData curSideData;
	UPROPERTY()
	FRoomVecData preSideData;
	UPROPERTY()
	FRoomVecData nextSideData;
	UPROPERTY()
	FVector2D preMousePos;
	const float AdsorptionRange = 5;//吸附容差3cm
	UPROPERTY()
	bool b;
	UPROPERTY()
	int32 PreID;
	UPROPERTY()
	bool bFindWall;
	UPROPERTY()
	FVector2D offset;
	UPROPERTY()
	bool bSelect;
	UPROPERTY()
	FVector2D MoveWallPos;
	UPROPERTY()
	int32 WallIndex;
	UPROPERTY()
	float WallCornnerLength;
	float AlongWallLength;

	UPROPERTY(BlueprintReadWrite, Category = "CompPositionWidget")
		UComponentPositionWidget* CompPositionWidget;
	UPROPERTY(EditDefaultsOnly, Category = "CompPositionWidget")
	TSubclassOf<UComponentPositionWidget> CompPostitionWidgetClass;
};