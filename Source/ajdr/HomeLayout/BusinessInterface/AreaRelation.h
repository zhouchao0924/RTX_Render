// Fill out your copyright notice in the Description page of Project Settings.
// material node
#pragma once

#include "AreaCollection.h"
#include "../SceneEntity/Wall_Boolean_Base.h"
#include "../SceneEntity/RoomActor.h"
#include "../SceneEntity/LineWallActor.h"
#include "../SceneEntity/MontageMeshComponent.h"
#include "AreaRelation.generated.h"

UCLASS(BlueprintType, Blueprintable)
class USkirtingSegObject : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
	FSkirtingCollectionHandle GetSkirtingCollection();

	UFUNCTION(BlueprintPure)
	FSkirtingSegNode GetRelatedSkirtingSegNode();

protected:
	double CalculateSkirtingLength() const;

	FString GetSkirtingModelID() const;

	FString GetSkirtingMaterialID() const;

protected:
	friend class AAreaSystem;
	UPROPERTY(BlueprintReadWrite)
	USkirtingSegComponent *RelateSkirtingSeg;

	void SetStart(float InSegStart);

	void SetEnd(float InSegEnd);

protected:
	float SegStart;
	float SegEnd;
};

UCLASS(BlueprintType, Blueprintable)
class UWallSegObject : public UObject
{
	GENERATED_BODY()
public:
	UWallSegObject();

	UFUNCTION(BlueprintCallable)
	FWallCollectionHandle GetWallCollection();

protected:
	friend class AAreaSystem;
	double BuildWallArea();

	void SetStart(double InSegStart);

	void SetEnd(double InSegEnd);

	void SetWallSegStart(const FVector2D &InWallSegStart);

	void SetWallSegEnd(const FVector2D &InWallSegEnd);

	void SetRegionStart(const FVector2D &InRegionStart);

	void SetRegionEnd(const FVector2D &InRegionEnd);

	int32 GetFaceType(EWallSurfaceType & Type);

	UPROPERTY(BlueprintReadWrite)
	ALineWallActor *WallLayOnSeg;

	UPROPERTY(BlueprintReadWrite)
	EWallSurfaceType FaceType;

	UPROPERTY(BlueprintReadWrite)
	TArray<AWall_Boolean_Base*> OpeningsOnThisWallSurface;

	virtual void BeginDestroy();

protected:
	bool bWallInRegion;

	FVector2D WallSegStart;
	FVector2D WallSegEnd;
	
	bool bSameDirWithRegion;

	// local variable
	double SegStart;
	double SegEnd;

	// no use
	FVector2D OwnerRegionSegStart;
	FVector2D OwnerRegionSegEnd;
};

// related to the surface of an edge of an area, or an surface of an wall that lies in this area
UCLASS(BlueprintType, Blueprintable)
class UWallSurfaceObject : public UObject
{
	GENERATED_BODY()
public:
	UWallSurfaceObject()
		:bWallSurfaceInRoom(false)
	{

	}

	UFUNCTION(BlueprintPure)
	const TArray<UWallSegObject*>& GetWallSegs() const
	{
		return WallsLayOnThisSeg;
	}

protected:
	friend class AAreaSystem;
	friend class URegionObject;
	UPROPERTY(BlueprintReadWrite)
	TArray<UWallSegObject*> WallsLayOnThisSeg;
	UPROPERTY(BlueprintReadWrite)
	FVector2D StartPosOfRegion;
	UPROPERTY(BlueprintReadWrite)
	FVector2D EndPosOfRegion;

private:
	//TODO: no use
	bool bWallSurfaceInRoom;
};


UCLASS(BlueprintType, Blueprintable)
class URegionObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FRegionCollection RegionCollection;

	UFUNCTION(BlueprintCallable)
	TArray<FSkirtingCollectionHandle> MergeSameSkirtingLines();
protected:
	const FRegionCollection& CalculateRegionCollection();

	friend class AAreaSystem;
	UPROPERTY(BlueprintReadWrite)
	ARoomActor *RegionActor;
	UPROPERTY(BlueprintReadWrite)
	TArray<UWallSurfaceObject*> WallsCcb;
	UPROPERTY(BlueprintReadWrite)
	TArray<UWallSurfaceObject*> WallsInThisRegion;
	UPROPERTY(BlueprintReadWrite)
	TArray<USkirtingSegObject*> SkirtingLines;
};
