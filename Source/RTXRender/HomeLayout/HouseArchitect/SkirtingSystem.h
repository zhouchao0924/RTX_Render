// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once


#include "Components/ActorComponent.h"
#include "../SceneEntity/MontageMeshComponent.h"
#include "SkirtingSystem.generated.h"

class ARoomActor;

struct FVector2DArea
{
	FVector2D Pos;
	int32 tArea;
};

USTRUCT(Blueprintable)
struct FBoundaryPsOfRegion
{
	GENERATED_BODY()
		FBoundaryPsOfRegion()
	{

	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> BoundaryPs;
};

UCLASS(BlueprintType, Blueprintable)
class RTXRENDER_API ASkirtingSystem : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ASkirtingSystem();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Build")
	USkirtingMeshComponent* GetSkirtingObjectBySnapPnt(const FVector2D &SnapPoint, UPARAM(ref) ESkirtingSnapType &SnapType, ESkirtingType InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Build")
	USkirtingMeshComponent* GetSkirtingMeshByPnt(const FVector2D &SnapPnt);

	UFUNCTION(BlueprintCallable, Category = "Build")
	USkirtingSegComponent* CreateSkirtingSegByNode(const FSkirtingSegNode &InSkirtingSegNode, UStaticMesh *SkirtingMesh);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void DelSkirtingComponentsByRect(const FVector2D &LeftTopPnt, const FVector2D &RightBottomPnt, ESkirtingType DelType);


	UFUNCTION(BlueprintCallable, Category = "Build")
	void DeleteSkirtingSegs(const TArray<FVector2D> RegionPoints, ESkirtingType DelType);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void SyncAreaOutlinePathWithType(ESkirtingType InSkirtingType);


	UFUNCTION(BlueprintCallable, Category = "Build")
	void GetDecorateLinesAroundPillars(TArray<USkirtingMeshComponent*> &CollectLines, ESkirtingType InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void GetAllPointsAroundComponents(TArray<FVector2D>&CollectComponentNodes);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void GetComponentPointsOfLocalRegion(const TArray<FVector2D> LocalRegion, TArray<FVector2D>&CollectComponentNodes);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void UpdateDrawingLinesOfPillars(ESkirtingType InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void LocalRegionCreateDecorateLine(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassID, int32 CraftID,
	int32 MaterialModelID, FString MaterialResID, ESkirtingType InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void CreateLocalSingleRegionNode(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassID, int32 CraftID, int32 SkuID,
	int32 MaterialModelID, FString MaterialResID, FSkirtingNode& InSkirtingNode, ESkirtingType InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void CreateLocalSingleRegionNodeWithCom(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassID, int32 CraftID, int32 SkuID,
			int32 MaterialModelID, FString MaterialResID, FSkirtingNode& InSkirtingNode, FSkirtingNode& InSkirtingComNode, ESkirtingType InSkirtingType);


	UFUNCTION(BlueprintCallable, meta = (Category = "Property"))
	bool UpdateCurrentDecorateLines(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassId, int32 CraftId, 
	int32 SkuID, int32 MaterialModelID, FString MaterialResID, ESkirtingType LineType);

	UFUNCTION(BlueprintCallable, meta = (Category = "Property"))
	bool UpdateCurrentDecorateLinesWithComponents(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassId, int32 CraftId,
			int32 SkuID, int32 MaterialModelID, FString MaterialResID, ESkirtingType LineType);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void RemoveRegionNodes(int32 RoomID, ESkirtingType InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Build")
	TArray<USkirtingMeshComponent*> FindDecorateLinesOfRoom(int32 RoomID, ESkirtingType InSkirtingType);

	TArray<USkirtingMeshComponent*> GetSkirtingByRoomId(int32 RoomID);//根据roomid获得踢脚线

	UFUNCTION(BlueprintCallable, Category = "Build")
	USkirtingMeshComponent* LocalCreateLinesByModelFile(const FSkirtingNode &InSkirtingNode, FString ModelResID,
	FString MaterialResID, UStaticMesh *InSkirtingMesh = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void SyncTargetSkirtingPath(ESkirtingType TargetType);

	UFUNCTION(BlueprintCallable, Category = "Build")
	USkirtingMeshComponent* CreateSkirtingActorByDefault(ESkirtingType InSkirtingType,
		ECgHeadingDirection HeadingDir);

	UFUNCTION(BlueprintCallable, Category = "Build")
	USkirtingMeshComponent* CreateSkirtingComponentByNode(FSkirtingNode InSkirtingNode, UModelFile *InSkirtingModel = nullptr, UStaticMesh *InSkirtingMesh = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void RebuildSkirtingByType(ESkirtingType InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Default")
	UModelFile* GetDefSkirtingModelByType(ESkirtingType InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Default")
	UStaticMesh* GetDefSkirtingMeshByType(ESkirtingType InSkirtingType);

	UFUNCTION(BlueprintCallable, Category = "Test")
	void CreateStaticMesh();

	UFUNCTION(BlueprintCallable, Category = "Test")
	void CreateProceduralMesh();

	UFUNCTION(BlueprintCallable, Category = "Test")
	void CreateAStaticMesh();

	UFUNCTION(BlueprintCallable, Category = "Test")
	void CreateAProceduralMesh();

	///////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Build")
	USkirtingMeshComponent* CreateSkirtingComponentByNodes(const FSkirtingNode &InSkirtingNode, bool IsComponent = true, UModelFile *InSkirtingModel = nullptr, UStaticMesh *InSkirtingMesh = nullptr);



	/////////////////////////////////////////////////////////////////////////
	float GetSkirtingTopLineDefHeight() const;

	void CollectAllSkirtingLines(TArray<USkirtingMeshComponent*> &OutSkirtingLines);

	 TArray<FVector2D> FVector2DAreaToFVector2D(TArray<FVector2DArea> & Vec2DArea);

	template <typename T>
		TMap<FString, T*> SphereTraceMultiForObjects(const FVector Begin, const FVector End, float Radius);


	bool IsPolyClockWise(const TArray<FVector2D> & Polygons);
	void FilterVertex(TArray<FVector2D> & Polygons);
	TArray<FVector2D> PolygonScale(TArray<FVector2D> & Polygons, int multiple);
	void ChangeClockwise(TArray<FVector2D> & Polygons);

	UFUNCTION(BlueprintCallable, Category = "Build")
		void RemoveRegionDecorateLines(TArray<FVector2D> RegionPoints);

	UFUNCTION(BlueprintCallable, Category = "Build")
		void RemoveDecolateLinesOfSomeRegions(TArray<FBoundaryPsOfRegion> RegionsToRemove);
public:
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "GetSkiringmxFile"))
	void GetSkiringmxFileImplByBlueprint(const FSkirtingNode & InSkirtingNode);

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Property"))
	TArray<FVector2D> RoomIDMapRegionPs(const int32 InPutRoomID);

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Property"))
	void GetLivingRoomRegion(int32 &RoomID, TArray<FVector2D>&LivingBoundary);

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Property"))
	void GetDiningRoomRegion(int32 &RoomID, TArray<FVector2D>&DiningBoundary);

	UFUNCTION(BlueprintImplementableEvent, meta = (Category = "Property"))
		void GetActiveRoomRegion(int32 &RoomID, TArray<FVector2D>&DiningBoundary);
	//UFUNCTION(BlueprintCallable, Category = "Build")
	//void SetSkiringDefaultData(FSkirtingNode &InSkirtingNode);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void SetSkiringDefaultData(FSkirtingNode &InSkirtingNode, bool HasComponent = false);

	void CalculationErrorData(TArray<FVector2D> &PolygonA, TArray<FVector2D> &PolygonB);

	//踢角线MeshComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	TArray<USkirtingMeshComponent*> SkirtingMeshComponents;
	
	//顶角线MeshComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	TArray<USkirtingMeshComponent*> TopSkirtingMeshComponents;
	
	//踢角线 区域 MeshComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	TArray<USkirtingMeshComponent*> SkirtingAreaMeshComps;

	//顶角线 区域 MeshComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	TArray<USkirtingMeshComponent*> TopSkirtingAreaMeshComps;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	FSkirtingLinedefaultNode SkirtingLinedefaultNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	FSkirtingLinedefaultNode SkirtingTopLinedefaultNode;



protected:
	UModelFile* ForceLoadMXByID(const FString &ResID);

private:
	UPROPERTY(EditAnywhere, meta = (Category = "Default"))
	UStaticMesh *SkirtingLineMesh;
	UPROPERTY(EditAnywhere, meta = (Category = "Default"))
	UStaticMesh *SkirtingTopLineMesh;

	UPROPERTY(EditAnywhere, meta = (Category = "Default"))
	UModelFile *SkirtingLineRes;
	UPROPERTY(EditAnywhere, meta = (Category = "Default"))
	UModelFile *SkirtingTopLineRes;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	FString SkirtingLineResID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	FString SkirtingTopLineResID;
	UPROPERTY(EditAnywhere, meta = (Category = "Property"))
	float SkirtingTopLineDefHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	int32 SkirtingLineModelID;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Property"))
	FString TestResID;
};

UCLASS(BlueprintType, Blueprintable)
class RTXRENDER_API ASkirtingActor : public AActor
{
	GENERATED_BODY()

public:
	ASkirtingActor();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "SkirtingActor")
		FString GetSkirtingNodeUuid();
};

template <typename T>
TMap<FString, T*>
ASkirtingSystem::SphereTraceMultiForObjects(const FVector Begin, const FVector End, float Radius)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreActor;
	TArray<FHitResult> OutHits;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TMap<FString, ARoomActor*> CollisioinPos;
	UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Begin, End, Radius, ObjectTypes,false, IgnoreActor, EDrawDebugTrace::Type::ForOneFrame, OutHits, true);
	for (int j = 0; j < OutHits.Num(); ++j)
	{
		T * ARA = Cast<T>(OutHits[j].GetActor());
		if (ARA)
		{
			CollisioinPos.Add(ARA->GetName(), ARA);
		}
	}
	return CollisioinPos;
}


