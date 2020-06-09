// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "../BusinessInterface/AreaRelation.h"
#include "../SceneEntity/FurnitureModelActor.h"
#include "AreaSystem.generated.h"
class UBuildingSystem;
class UWallBuildSystem;

USTRUCT(Blueprintable)
struct FModelsInRegion
{
	GENERATED_BODY()
	FModelsInRegion()
	{

	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ARoomActor *Region;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AFurnitureModelActor*> ModelsInRegion;
};

UCLASS(BlueprintType, Blueprintable)
class AJDR_API AAreaSystem : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AAreaSystem();
	virtual void BeginPlay() override;

	// initialization
	UFUNCTION(BlueprintCallable)
	void ReArrangeRegion();

	// query
	UFUNCTION(BlueprintCallable)
	FRegionCollection CalculateRegionCollection(ARoomActor *RegionInW, bool bFindByCache = false);

	UFUNCTION(BlueprintCallable)
	TArray<FSkirtingCollectionHandle> GetSkirtingLineCollection(const FRegionCollection &InRegionC);

	UFUNCTION(BlueprintCallable)
	TArray<FSkirtingCollectionHandle> GetSkirtingTopLineCollection(const FRegionCollection &InRegionC);

	UFUNCTION(BlueprintCallable)
	TArray<AWall_Boolean_Base*> GetRelatedOpeningsOfRoom(ARoomActor *RegionInW, bool bFindByCache = false);



	UFUNCTION(BlueprintCallable)
	TArray<AFurnitureModelActor*> CollectRelatedModelsOfRoom(ARoomActor *RegionInW, bool bFindByCache = false);


	UFUNCTION(BlueprintCallable)
	bool SeperateModelsIntoRooms(TArray<ARoomActor*> RegionsInW, TArray<FModelsInRegion>&ModelsOfHouse,bool NoStored=true);
	
	UFUNCTION(BlueprintCallable)
	bool IsTheModelInTheRoom(AFurnitureModelActor *FuritureModel, ARoomActor *RegionInW, bool bFindByCache = false);

	UFUNCTION(BlueprintCallable)
	bool IsLocationInTheRoom(FVector2D Location, ARoomActor *RegionInW, bool bFindByCache = false);

	UFUNCTION(BlueprintPure)
	float GetWallCollectionArea(const FWallCollectionHandle &WallCollection) const;

	UFUNCTION(BlueprintPure)
	FString GetWallAreaByM2(const FWallCollectionHandle &WallCollection) const;

	UFUNCTION(BlueprintPure)
	float GetSkirtingCollectionLength(const FSkirtingCollectionHandle &SkirtingCollection) const;

	UFUNCTION(BlueprintPure)
	float GetAreaPerimeter(ARoomActor *Actor) const;

	UFUNCTION(BlueprintCallable)
	TArray<ARoomActor*> SearchRelatedAreasByWallSurface(ALineWallActor *InWallActor, EWallSurfaceType WallFace);

	UFUNCTION(BlueprintCallable)
	TArray<ALineWallActor*> CollectCcbWallsOfRoom(ARoomActor *RegionInW, bool bFindByCache = false);

	// ReArrangeRegion must be called before
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	TArray<ARoomActor*> SearchRelatedAreaByOpening(AWall_Boolean_Base *InOpeningActor, const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable)
	float GetRoomArea(ARoomActor *InRoomActor) const;

	UFUNCTION(BlueprintPure)
	const TArray<URegionObject*>& GetRegionsWithRCInWorld() const;

public:
	UFUNCTION(BlueprintPure)
	const TArray<ARoomActor*>& GetRegionsInWorld() const;

private:
	URegionObject* CacheRoomRelationObj(ARoomActor *RegionInW, bool bFindByCache);

	void ArrangeWalls2Region(URegionObject *RegionObj);

	void ArrangeInnerWalls2Room(URegionObject *RegionObj);

	void ArrangeSkirtingLines2Region(URegionObject *RegionObj);

	void RebuildWallSurRelationCache(UWallSurfaceObject *Region2Build);

	friend class ARoomActor;
	void OnRegionActorSpawned(ARoomActor* Actor);

	void OnRegionActorDestroyed(ARoomActor* Actor);

	bool IsTheModelInTheRoomImpl(AFurnitureModelActor *FuritureModel, ARoomActor *RegionInW, const TArray<AWall_Boolean_Base*> &OpeningsInTheRoom);

	bool IsLocationInTheRoomImpl(const FVector2D &ModelCenter, ARoomActor *RegionInW, const TArray<AWall_Boolean_Base*> &OpeningsInTheRoom);

private:
	UPROPERTY()
	TArray<ARoomActor*> RegionsInWorld;

	UPROPERTY()
	TArray<URegionObject*> RegionsWithRCInWorld; // regions with relation cache

protected:
	UPROPERTY()
	UWorld *WorldCache;
	UPROPERTY()
	UWallBuildSystem* WallSystem;
	UPROPERTY()
	TArray<FModelsInRegion> TotalModelsOfHouse;
};
