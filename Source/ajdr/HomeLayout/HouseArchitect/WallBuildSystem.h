// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "../DataNodes/SavedDataNode.h"
#include "../DataNodes/SkirtingNode.h"
#include "../SceneEntity/CornerActor.h"
#include "../SceneEntity/LineWallActor.h"
#include "../SceneEntity/Wall_Boolean_Base.h"
#include "../SceneEntity/RoomActor.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Linkage/WallLinkage.h"
#include <vector>
#include "IBuildingSDK.h"
#include "../../Building/WallPlane.h"
#include "Struct/Struct.h"
#include "OBBQuadrilateral.h"
#include "WallBuildSystem.generated.h"
class UBuildingSystem;
class ADRGameMode;
class AComponentManagerActor;
UENUM(BlueprintType)
enum class ESelectObjectType : uint8
{
	None_Object,
	Wall_Object,
    Corner_Object,
	Room_Object,
	Door_Object,
	Window_Object,
};
UENUM(BlueprintType)
enum class EDRAllStatusWallType : uint8
{
	EDROriginalWall,
	EDRNewWall,
	EDRDeleteWall,
};
USTRUCT(BlueprintType)
struct FWallPints
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FVector2D StartLoc;
	UPROPERTY(BlueprintReadWrite)
		FVector2D EndLoc;
	UPROPERTY(BlueprintReadWrite)
		float RightThick;
	UPROPERTY(BlueprintReadWrite)
		bool IsRoomWall;
	FWallPints()
	{
		IsRoomWall = true;
	}
};

USTRUCT(BlueprintType)
struct FRoomInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> AreaPos;
	UPROPERTY(BlueprintReadWrite)
		FString roomName;
	UPROPERTY(BlueprintReadWrite)
		FString usageId;
};
USTRUCT(BlueprintType)
struct FCutAreaPos
{
	GENERATED_BODY()

public:
	int32 ObjectId;
	FVector2D Loc;
	FVector2D OriginalLoc;
};

struct FCornerInfoBySnap
{
	FVector2D Loc;
	int32 ID;
	double Distance;
	FCornerInfoBySnap(FVector2D InitLoc, int32 IntID, double InitDis)
		:Loc(InitLoc)
		, ID(IntID)
		, Distance(InitDis)
	{
	}
};

USTRUCT(BlueprintType)
struct FVirtualWallInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	int32 WallID;
	UPROPERTY(BlueprintReadOnly)
	FVector2D SartPos;
	UPROPERTY(BlueprintReadOnly)
	FVector2D EndtPos;
	UPROPERTY(BlueprintReadOnly)
	int32 AssociationID;
	UPROPERTY(BlueprintReadOnly)
	FVector2D OriginalSartPos;
	UPROPERTY(BlueprintReadOnly)
	FVector2D OriginalEndtPos;
	UPROPERTY(BlueprintReadOnly)
		int32 SnapObj0;
	UPROPERTY(BlueprintReadOnly)
		int32 SnapObj1;
	FVirtualWallInfo()
	{
		AssociationID = -1;
		SnapObj0 = -1;
		SnapObj1 = -1;
	}
};


UCLASS(BlueprintType, Blueprintable)
class AJDR_API UWallBuildSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallBuildSystem();
	UFUNCTION(BlueprintCallable, Category = "Build")
	const TArray<ALineWallActor*>& GetWallsInScene();

	TArray<ACornerActor*>& GetCornersInScene();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (Category = "Build"))
	void ShowToast();

	UFUNCTION(BlueprintCallable, Category = "CalculateWallCache")
	void BuildWallGeometry();

	UFUNCTION(BlueprintCallable, Category = "CalculateWallCache")
	void ComputeGeomFromData();

	UFUNCTION(BlueprintCallable, Category = "Build")
	void UpdateWallActors();
	
	UFUNCTION(BlueprintCallable, Category = "Build")
	void SpawnWallByNode(const FSavedDataNode &WallSavedData);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void UpdateWallRulers(UPARAM(ref) FPaintContext &Context);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void DragMoveWallCorner(const FString &CornerName, const FVector2D &NewCornerPos);

	FVector2D GetDrawWallPosByMousePnt(ALineWallActor *&OutSnapWall, ACornerActor* &OutSnapX, ACornerActor* &OutSnapY, 
		const FVector2D &InMousePnt, const TArray<ACornerActor*> &InIgnoredCorners,
		const TArray<ALineWallActor*> &InIgnoredWalls,bool IsShowToast=false);

	UFUNCTION(BlueprintCallable, Category = "Build")
	FString AddNewCorner(const FVector2D &CornerPos,const int32 ObjectId=-1);
	
	UFUNCTION(BlueprintCallable, Category = "Build")
	void MergeCorner(const FString &Corner2Merge, const FString &Corner2Del, bool bUpdate = true);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void RemoveWall(ALineWallActor *Wall2Remove);

	UFUNCTION(BlueprintCallable, Category = "Build")
	ALineWallActor* CuttingWallByPosition(ALineWallActor *Wall2Cut, const FVector2D &PosOnWall);

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Build"))
	void SpawnDefaultAreaByPath(const TArray<FVector2D> &AreaPath);

	UFUNCTION(BlueprintPure)
	TArray<FRoomPath> GetInnerRoomPathArray() const;

	UFUNCTION(BlueprintCallable, Category = "Build")
	TArray<ALineWallActor*> GetRelatedCornerWall(const FString &Corner);

	UFUNCTION(BlueprintCallable, Category = "Build")
	TArray<ALineWallActor*> GetAdjWallsByCorner(const FString &CornerID);

	UFUNCTION(BlueprintCallable, Category = "Build")
	TArray<FString> GetAdjCorners(const FString &Corner);

	UFUNCTION(BlueprintCallable, Category = "Wall Cut")
	void CheckAlignPoint(ALineWallActor *Wall, float ErrorTolerance, float& XPos, float& YPos, float& XLen, float& YLen);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void MakeWallCorners();

	UFUNCTION(BlueprintImplementableEvent, Category = "Build")
	void UpdateWallHudCPP();

	

	void UpdateWallCornerByPos(ALineWallActor *Wall2Update);

	ALineWallActor* GetWallActorByID(const FString &WallID);

	ACornerActor* GetCornerActorByID(const FString &CornerID);

	int32 GetDegreeOfCorner(const FString &InCornerID);

	ALineWallActor* SearchWallByEndPntMethod2(const FVector2D &StartPnt, const FVector2D &EndPnt);

	void SearchWallByEndPnt(TArray<ALineWallActor*> &OutWalls, const FVector2D &StartPnt, const FVector2D &EndPnt);

	void SearchWallsByEndPnt(TArray<ALineWallActor*> &OutWalls, const FVector2D &StartPnt, const FVector2D &EndPnt);

	void SearchSideWallsByEndPnt(TArray<ALineWallActor*> &OutWalls, const FVector2D &StartPnt, const FVector2D &EndPnt);

	UFUNCTION(BlueprintCallable, meta = (Category = "Judge"))
	ARoomActor* GetSegmentOwnerRoom(const FVector2D &Start, const FVector2D &End);

	UFUNCTION(BlueprintCallable, Category = "Build")
	TArray<FVector2D> FindLocationOfCorners(TArray<FString> CornerIDs);

	const FSavedDataNode& GetDefaultWallSetting() const;

	UFUNCTION(BlueprintCallable, Category = "Test")
	void CreateTestWallActor();

	ALineWallActor* GetWallByCorner(const FString &StartCorner, const FString &EndCorner);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void CalcuateWallGeometry(bool bCreateCollison = true);

	void UpdateWallsByCorners(const TArray<FString>& CornersModified, bool bCreateCollison = true);

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Build")
		void GetBPAllWall(TArray<ALineWallActor*>& bwall);
	UFUNCTION(BlueprintCallable, Category = "Build")
	int32 IsSelectWallAndShowCorner(const FVector2D &Location);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ClearSlectCornner();
	UFUNCTION(BlueprintCallable, Category = "Build")
	int32 IsOverCornner(const FVector2D &Loc);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool MoveCornner(const int32 objectID, const FVector2D &Loc);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool CornerMovable(const int32 objectID, const FVector2D &Loc);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ClearAllCornnerUMG();
	UFUNCTION(BlueprintCallable, Category = "Build")
	FVector2D GetCurrentCornerViewPortPos(const int32 &objectID,FVector &WorldPos);
	UFUNCTION(BlueprintCallable, Category = "Build")
	ESelectObjectType GetSlectObjectType(const FVector2D &Location,int32 &objectID);
	UFUNCTION(BlueprintCallable, Category = "Build")
	bool IsCorner(const int32 &objectID);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ShowSlectCorner(int32 objectID);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ShowSlectWall(int32 objectID);
	UFUNCTION(BlueprintCallable, Category = "Build")
	FVector2D ShowSlectRoom(int32 objectID, TArray<FVector>& SlotPos);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void DeleteSomeActors();
	UFUNCTION(BlueprintCallable, Category = "Build")
	void AddVirtualWallCorner(const FVector2D &Loc);
	UFUNCTION(BlueprintCallable, Category = "Build")
	TArray<int32> AddVirtualWall(int32 StartCorner, int32 EndCornner);
	UFUNCTION(BlueprintCallable, Category = "Build")
	int32 AddWindow(int32 WallID, const FVector2D &Location, float zPos, float Width, float Height, const FString &WindowResID, const FString &SillsResID);
	UFUNCTION(BlueprintCallable, Category = "Build")
	TArray<int32> AddEdge(const FVector2D &P0, const FVector2D &P1, int32 SnapObj0 = -1, int32 SnapObj1 = -1, bool bBreak = false);
	UFUNCTION(BlueprintCallable, Category = "Build")
	int32 AddDoor(int32 WallID, const FVector2D &Location, float zPos, float Width, float Height);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void  GetWallVectorToView(int32 WallID, FVector2D &P0, FVector2D &P1, FVector2D &Right);
	UFUNCTION(BlueprintCallable, Category = "Build")
	bool IsLastCornerFreeAndClear();
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ShowAllCornerUMG();
	UFUNCTION(BlueprintCallable, Category = "Build")
	void DeleteSlectActor(int32 objectID,bool IsDeleteWallPlane=true,bool bIsBillingBoolean=false);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ComputePolygonS(int32 objectID, float & OutS);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ReadyClearAllActor();
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ClearAllActor();
	UFUNCTION(BlueprintCallable, Category = "Build")
	bool GetWallBorderPos(int32 objectID, TArray<FVector>&WallNodes,float &ZPos);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void GetWallHole(UBooleanBase* booleanActor, const FVector2D& Deltavector, const bool IsFirst, int32 objectID, int32 WallID, const float CurWidth ,FVector2D &Loc, float& Width, FVector2D &StartLoc, FVector2D &EndLoc, float &Angle,FVector2D& Size,bool& Isaway, float &Thickness);
	UFUNCTION(BlueprintCallable, Category = "Build")
	FVector2D GetRotated2D(const FVector2D & Vec, const float &Angle);
	UFUNCTION(BlueprintCallable, Category = "Build")
	FVector2D GetBestHolePostion(int32 WallID, float Width, UBooleanBase* booleanActor);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void MoveBooleanActor(int32 HoleID, FVector2D Loc);
	void GetAllObject(TArray<int32>& TobjectIDArray, EObjectType InClass,bool bIncludeDeriveType = true);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void GetAllHoleObject(TArray<int32>& TobjectIDArray);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void UpdataCornerUMGData();
	UFUNCTION(BlueprintCallable, Category = "Build")
	float GetWallRotate(FVector2D StartPos, FVector2D EndPos);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void SearchWallsBySegPoints(TArray<int32>& OutWalls, FVector2D StartPos, FVector2D EndPos);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void CutAreaFailed();
	UFUNCTION(BlueprintCallable, Category = "Build")
	void CutArea(const int32 &WallID, const FVector2D &Loc);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void CancelCutArea();
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ViewFilter(bool IsShow);
	void AddVirtualWallUMG(const int32 &WallID);
	void UpdateVirtualWallUMG(const int32 &WallID);
	void DeleteVirtualWallUMG(const int32 &WallID);
	FVector2D LocateMouseSnapState(const FVector2D &Loc);
	FVector2D LocateMouseSnapState(const int CornerID,const FVector2D &Loc);
	void ClearAllCornerData();
	float GetHitWallThickness(int32 Obj);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void DeleteHole(const int32& HoleID);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ShowWallUI(bool IsShow);
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ShowAllWallUI();
	UFUNCTION(BlueprintCallable, Category = "Build")
	void  TestCornerLoc(TArray<FVector> &Corners, TArray<int32> &CornerID);
	UFUNCTION(BlueprintImplementableEvent, Category = "Build")
	bool IsDeleteWalls();
	UFUNCTION(BlueprintImplementableEvent, Category = "Build")
	bool IsDeleteHoles();
	UFUNCTION(BlueprintCallable, Category = "Build")
	void ReturnSnapCornerID(const bool IsCalculate, const FVector2D &StartPos,const FVector2D &EndPos, TMap<FVector2D, FVector2D>TailMapDirs,FVector2D& OutStartPos, FVector2D&OutEndPos);
	UFUNCTION(BlueprintCallable, Category = "Build")
		const float GetWallThickness(const int32& WallID);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		ARoomActor* FindAreaByRoomID(const int32& AreaID);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		ARoomActor* FindRoomByRoomID(const int32& RoomID);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		bool IsDiningRoom(const int32& AreaID);
	UFUNCTION(BlueprintCallable, Category = "Build")
		FVector2D FindBestLoc(const FVector2D & Loc,const int32& ObjID,const float&Width,float& OutWidth,bool& IsSuccess);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool CreateVirtualWall(const TArray<FVector2D> & PolygonFirst,const TArray<FVector2D> & PolygonSecond, TArray<FVector2D>& VirtualWallPs, TMap<FVector2D, FVector2D>&TailPsMapDir);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void WorldPosToView(const TArray<FVector2D>& WorldPos, TArray<FVector2D>& ViewPos);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void TwoLineIntersection(const FVector2D&StartPos, const FVector2D& EndPos,const FVector2D& Temp, FVector2D& _OutPos);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		bool ShowToastDrawWall();
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool JudgePointInPolygon(const TArray<FVector> InPnts, const FVector2D ToJudgePoint);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void VirWallWorldPosToView(const FVector2D &StartPos, const FVector2D &EndPos, FVector2D &_OutStartPos, FVector2D &_Out_EndPos);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool IsTwoLineSegmentsIntersect(const FVector2D& a1, const FVector2D& a2, const FVector2D& b1, const FVector2D& b2);
	UFUNCTION(BlueprintCallable, Category = "Build")
		FVector2D GetwoSegmentsIntersect(const FVector2D& a1, const FVector2D& a2, const FVector2D& b1, const FVector2D& b2);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void LoadAllVirtualWall();
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool OnlyGetAreaSlots(const int32& AreaID, TArray<FVector2D>& SlotPos);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void SetWallIsClosed(const TArray<int32> WallIDS, bool IsClosed);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void BuildRoom();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void CollectRoomInfoAfterDelete();
	UFUNCTION(BlueprintPure, Category = "Build")
		FSavedDataNode GetDefaultData();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void GetDefaultRoomClassAndCraft(const int32& ModelID, int32& RoomClassID, int32& Craft);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void BuildAreaWithVirWalls();
	UFUNCTION(BlueprintCallable, Category = "Build")
		void OnlyDelateVirWall(const TArray<int32>& VirWallIDS);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void ClearFreeCorner();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void ChangeWallID(UBooleanBase* booleanActor);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void CutAreaTick(const FVector2D &Loc);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool IsLoadVirWall();
	UFUNCTION(BlueprintCallable, Category = "Build")
		void SaveVirWall();
	UFUNCTION(BlueprintImplementableEvent, Category = "Build")
		void GetAllAreaPoints(TArray<FVector2DArrayStruct>& OutAreaPoints);//仅限于打开方案计算虚拟墙使用
	UFUNCTION(BlueprintCallable, Category = "Build")
		void FilterAreaSlot(const TArray<FVector>& InPnts, TArray<int32>&OutIndex);
	UFUNCTION(BlueprintCallable, Category = "Build")
		ESelectObjectType IsWallType(const int32 &objectID, int32 &OutID);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void CutWallByMousePos(const FVector2D &Loc);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void ShowToastCutWall();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void RecordHoleData(int32 WallID);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void AddRecordedHoleData(int32 WallID1, int32 WallID2);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void UpdataWallPlaneMaterial(bool IsOpenNewWallType=false);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void FindWallAndPlaneByID(const int32& WallID, bool IsOpenNewWallType = false);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void MergeWall(const int32 &objectID);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		bool IsCanMergeWallBP(const int32 &WallID1, const int32 &WallID2);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		bool SetIsDeleteTempArea(bool IsOpen=true);
	UFUNCTION(BlueprintCallable, Category = "Build")
		FVector2D OrthogonalDraw(const FVector2D &Loc, const int32 & LastID, bool IsOpen = false);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void ShowUniteToast(const FString &Title, const int32 &Type, const float &time);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void MergeAllPolygon(TArray<FPolygonInfo>&Out_Polygons);
	bool IsPolyClockWise(const TArray<FVector2D> & Polygons);
	void ChangeClockwise(TArray<FVector2D> & Polygons);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void MergeOnePolygon(const TArray<FString>& AreaTag,TArray<FVector2D>&Out_Points);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void CalculateWallIntersection(const TArray<FWallPints>& In_Points, TArray<FWallPints>&Out_Points);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void IsNeedSegmentAreaByAIData(const TArray<FWallPints>& In_Points, const TArray<FRoomInfo>& Area, TArray<FWallPints>& Out_Points);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		AWall_Boolean_Base* FindBooleanActorByHoleID(const int32& HoleID);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void UpdateEdgeCornerPos(FVector2D& StartPos, FVector2D& EndPos, int32 SnapObj0 = -1, int32 SnapObj1 = -1);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool CheckEdgeCornerPos(FVector2D& StartPos, FVector2D& EndPos, int32 SnapObj = -1);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void FilterEdgeInfo();
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool FilterShortEdge(int32 EdgeID);
	UFUNCTION(BlueprintCallable, Category = "Build")
		int32 AddEdgeCorner(FVector2D &Location, int32 WallID);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void CalculateBestEdgeCornerPos(FVector2D& StartPos, FVector2D& EndPos, int32& SnapObj_0, int32& SnapObj_1);
	UFUNCTION(BlueprintCallable, Category = "Build")
		FString GetvalidString(const FString Tag);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool IsCanMoveWall(const int32& ConnerID);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool CheckCollisionBP(TArray<FVector> obb1, FVector2D max, FVector2D min);
		bool CheckCollision(TArray<FVector2D> obb1, FVector2D max, FVector2D min);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		bool CheckDeleteWallCollision(FVector2D max, FVector2D min);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void LoadAreaByDemolition();
	UFUNCTION(BlueprintCallable, Category = "Build")
		void GetCornerWindow(const int32 WallID);
	void RefreshAlignmentPoint(TArray<float>&TempX, TArray<float>&TempY, float& Snap);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool GetHousePluginOnSurfaceWall(const bool& IsP0, const int32& WallID, const TArray<AActor*>&HousePlugins, TArray<AComponentManagerActor*>&OutHousePluginArr);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void UpdateMoveHousePlugin();
	UFUNCTION(BlueprintCallable, Category = "Build")
		void OnlyUpdateMoveHousePluginLoc();
			UFUNCTION(BlueprintCallable, Category = "Build")
		void ClearMoveHousePluginArray();
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool IsSelectHousePlugin();
	void FillWallDefaultProperties(int32 WallId);
	UFUNCTION(BlueprintCallable)
		int32 UpdateTheWallLength(float CurrentLength);
	UFUNCTION(BlueprintCallable)
		void DragTheWallMove(FVector2D CurrentMousePos, int32 WallID);
	UFUNCTION(BlueprintCallable)
		bool DragTheVirtualWallMove(FVector2D CurrentMousePos, int32 WallID);
	UFUNCTION(BlueprintCallable)
		int32 GetDragWallID(FVector2D CurrentMousePos, int32 HitObjID);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HoleOpertaion")
		void SetHoleWidgetMoveStates(bool bMove);
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetWallHeightConfigForDR(float Height);
	UFUNCTION(BlueprintCallable)
		void GetHolesPosition(int32 WallID);
	UFUNCTION(BlueprintCallable)
		void UpdateHolesObjectPosition(int32 WallID);
	UFUNCTION(BlueprintCallable)
		void GetAllHolesObject();
	UFUNCTION(BlueprintCallable, Category = "Build")
		FDeleteHoleInfo CalculateRemoveHoleInfo(const int32 HoleID);
	UFUNCTION(BlueprintCallable)
	FVector2D GetWallCenterLoc(int32 objectID, int32 WallID, float Width);
	UFUNCTION(BlueprintCallable)
		bool IsSeleteDeleteHole(UBooleanBase*& Boolean);
	UFUNCTION(BlueprintCallable)
		bool GetWallTypeByHole(const UBooleanBase* DeleteBoolean,const AWall_Boolean_Base*Boolean, EDRAllStatusWallType& WallType);
	UFUNCTION(BlueprintCallable)
		void GetAllHoleTypeByWall(const int32 WallID, const int32 StructIndex, TArray<AWall_Boolean_Base*>& OutBoolean, TArray<UBooleanBase*>& OutDeleteBoolean);
	UFUNCTION(BlueprintCallable)
		bool IsSetThisWall(const int32 WallID, const FVector2D Center);
	UFUNCTION(BlueprintCallable)
		void DrawSlash(const bool IsDelete, const TArray<FVector2D>&InPoints, const float SizeX, TArray<FLinePos>&OutPoints,bool IsScreen=true ,float error = 20);
	UFUNCTION(BlueprintCallable)
		void BoxSelectDeleteHole(const FVector2D Main, const FVector2D Max);
	UFUNCTION(BlueprintCallable)
		TArray<UBooleanBase*> GetAllDeleteHoles();
	UFUNCTION(BlueprintCallable)
		bool IsinterlinkNewWall(const int32 WallID);
	bool IsinterlinkNewWallByCornner(const int32 CornerID, const FVector2D Dir,const int32 WallID);
	UFUNCTION(BlueprintCallable)
		bool IsErrorPoints(const TArray<FVector2D>InPoints);
	UFUNCTION(BlueprintCallable)
		bool IsNeedDeleteWindowSill(const FDeleteHoleInfo& CurrentHoleInfo,const FVector Center);
	UFUNCTION(BlueprintCallable, Category = "Build")
		bool CheckIsWallCorner(int32 CornerID);
	UFUNCTION(BlueprintCallable, Category = "Build")
		void CheckEdgeCorner(int32 ObjID);
	UFUNCTION(BlueprintCallable)
		int32 GetClickedVirtualWall(FVector2D CurrentMousePos);
	UFUNCTION(BlueprintCallable)
		bool GetReleatedCorner(int32 BorderCornerId, int32& CornerID);
	UFUNCTION(BlueprintCallable)
		void IsShowVirtualWallCorner(int32 WallID, bool bShow);
	UFUNCTION(BlueprintCallable)
		bool IsMoveBorderWallCorner(int32 CornerID, FVector2D& tWallCornerPos_1, FVector2D& tWallCornerPos_2);
	UFUNCTION(BlueprintCallable)
		void CorrectCornerPos(int32 CornerID);
	UFUNCTION(BlueprintCallable)
		void ShowLastWallLengthText(int32 SelectedWallID);
	UFUNCTION(BlueprintCallable)
		void HideAllWallLengthText();
	UFUNCTION(BlueprintCallable)
		void CancelDrawWallOpertaion();
	UFUNCTION(BlueprintCallable, Category = "Build")
		void RebuildWall(int32 WallID, FVector2D CornerPos, FVector2D TargetPos);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void NewAddRecordedHoleData(const TArray<int32>& WallIDArray);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Build")
		void GetMaxMinUMGSize(FVector2D& Min, FVector2D& Max);
protected:
	UPROPERTY(EditAnywhere, meta = (Category = "Property"))
	FVector2D Wall1Start;
	UPROPERTY(EditAnywhere, meta = (Category = "Property"))
	FVector2D Wall1End;
	UPROPERTY(EditAnywhere, meta = (Category = "Property"))
	FVector2D Wall2Start;
	UPROPERTY(EditAnywhere, meta = (Category = "Property"))
	FVector2D Wall2End;
	UPROPERTY(EditAnywhere, meta = (Category = "Property"))
	FString TestWallMatResID;

public:
	void IsInRangeWallComponent(int32 WallID, int32 CornerID, FVector2D Loc);
	bool IsInRangeWallComponent(int32 CornerID, FVector2D Loc);
	void DeleteComponentInRange(int32 WallID, int32 CornerID, FVector2D Loc) const;
	void GetWallIDFromPosition(const FVector2D &Loc, int32 Ignore, FVector2D &BestLoc, int32 &BestID, float Torlerance, float ToleranceEdge);

	FVector2D KeepHolesPosition(int32 WallID, float Length);
	bool IsPointOnLine(FVector2D PointVec, FVector2D LineStartPoint, FVector2D LineEndPoint);
protected:
	ACornerActor* GetNearestCornerByPnt(const FVector2D &Pnt) const;

	void UpdateWallPositionInfo();

protected:
	void DefaultWallSetting();


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Category="Property"))
	TArray<ALineWallActor*>	WallsInScene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	TArray<ACornerActor*> WallCorners;

	UPROPERTY(BlueprintReadWrite, meta = (Category = "Property"))
	TArray<ARoomActor*> RoomsInScene;

	friend class FWallArrObserver;
	friend class UWallLinkageComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	UWallLinkageComponent *WallLinkage;

public:
// Default Setting
	UPROPERTY(BlueprintReadWrite, meta = (Category = "Default Const"))
	FSavedDataNode DefaultWallProp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	bool bObjectSnap; // align
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	bool bShowConnectedPnts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	float ObjectSnapTol;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	float WallThickness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	float PntGridTol;
	UPROPERTY(EditAnywhere, meta = (Category = "DrawWall"))
	TSubclassOf<UUserWidget> CornerHudWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	bool IsStopDrawWall;
	UPROPERTY(BlueprintReadOnly, meta = (Category = "Property"))
	UBuildingSystem  *BuildingSystem;
	UPROPERTY(BlueprintReadOnly, meta = (Category = "Property"))
	ADRGameMode  *DRGameMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACornerActor*> ShowCorners;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D PreMouseLoction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDragCorner;
	//std::vector<IObject*> _OutAllCornerObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Testbool;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DeleteMinMousePos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DeleteMaxMousePos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LastCorner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NewCorner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCanCutArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> AllWallArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> AllCornerArray;
	UPROPERTY(EditAnywhere, meta = (Category = "DrawWall"))
	TSubclassOf<UUserWidget>RoomWidgetClass;
	UPROPERTY(EditAnywhere, meta = (Category = "Drawboolean"))
	TSubclassOf<UUserWidget> BooleanUMG;
	UPROPERTY(EditAnywhere, meta = (Category = "WallUMG"))
	TSubclassOf<AActor> WallPlaneActor;
	UPROPERTY(EditAnywhere, meta = (Category = "DrawWall"))
	TSubclassOf<UUserWidget> RulerUMG;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	TMap<int32, AWallPlaneBase*>	WallPlaneMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	TArray<AWallPlaneBase*>	DeletedWallPlaneArry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	TMap<int32, AActor*>	Area_Tag_Map;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "DrawWall"))
	TArray<FCutAreaPos>	CutAreaPos_Tarray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCutAreaDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FirstWallID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LastWallID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVirtualWallInfo> AllVirtualLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> TempVirtualLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 LastpointID;
	UPROPERTY(BlueprintReadOnly)
		int32 TenmpID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> CurrentTempVirtualLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> TempHoleIDs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNormalDraw =true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FRoomInfo> RoomInfoList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AComponentManagerActor*> MoveHousePluginArr;
	UPROPERTY(BlueprintReadOnly)
		class UCEditorGameInstance *GameInst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UDrawLineWidget* DrawLineWidget;
public:
	UPROPERTY(BlueprintReadWrite)
		bool bWallMidInside = true; //墙中线画墙和墙内线画墙

	UPROPERTY(BlueprintReadWrite)
		bool bMoving = false;

	UPROPERTY(BlueprintReadWrite)
		int32 LastDragWallID;

	UPROPERTY(BlueprintReadWrite)
		TMap<int32, float> HolesPosition; //用于记录墙上组件的ID和距P0的长度

	UPROPERTY(BlueprintReadWrite)
		TArray<ACornerActor*> MovingWallCorner;
	UPROPERTY(BlueprintReadWrite)
		TArray<int32> MovingVirtualWallCorner;

	UPROPERTY(BlueprintReadWrite)
		TMap<int32, UObject*> AllHolesObject;

	UPROPERTY(BlueprintReadWrite)
		TArray<UBooleanBase*> BooleanBase;

	UPROPERTY(BlueprintReadWrite)
		TArray<int32> VirtualWallIDArray;
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector2D> SelectedVirtualWallPos;
	UPROPERTY(BlueprintReadWrite)
		FVector2D WallCornerPos_1;         //the Corner ID near the BorderCorner 
	UPROPERTY(BlueprintReadWrite)
		FVector2D WallCornerPos_2;        //the Corner ID near the BorderCorner 
	UPROPERTY(BlueprintReadWrite)
		bool bMoveBorderWallCorner = false;

	UPROPERTY()
		class UDrawWallCmd *DrawWallCmd;

	FVector2D LastMousePos;

	int32 UseCount;
	int32 SelectWallText = -1;
};