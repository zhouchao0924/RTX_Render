// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseNode.h"
#include "WallNode.h"
#include "CGALWrapper/CgTypes.h"
#include "SkirtingNode.generated.h"


UENUM(BlueprintType)
enum class ESkirtingType : uint8 
{
	Unkown = 0,
	SkirtingLine = 1,
	SkirtingTopLine,
};

UENUM(BlueprintType)
enum class ESkirtingSrcType : uint8
{
	SrcFormDrawing,
	SrcFromSyncPath,
};


UENUM(BlueprintType)
enum class ESkirtingSnapType : uint8
{
	SnapNull,
	SnapHead = 1,
	SnapTail,
};

struct FSkirtingInterval
{
public:
	FSkirtingInterval(const FVector2D &InCacheStart, const FVector2D &InCacheEnd)
		:IntervalStartCache(InCacheStart), IntervalEndCache(InCacheEnd)
	{
	}

	const FVector2D& GetStart() const
	{
		return IntervalStartCache;
	}

	const FVector2D& GetEnd() const
	{
		return IntervalEndCache;
	}

protected:
	FVector2D IntervalStartCache;
	FVector2D IntervalEndCache;
};

// skiring segment node
USTRUCT(Blueprintable)
struct  FSkirtingSegNode
{
	GENERATED_BODY()
public:
	FSkirtingSegNode()
	{
		ID = FGuid::NewGuid().ToString().ToLower();

		SkirtingMeshModelID = MaterialModelID = -1;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCornerNode		SkirtingStartCorner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCornerNode		SkirtingEndCorner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString			ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RoomClassID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SKUID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CraftId;

// properties in skirting node, used for temp
	UPROPERTY(Transient, BlueprintReadWrite)
	float			StartSectionAngle;
	//UPROPERTY(Transient, BlueprintReadWrite)
	//FVector2D		StartSectionDir;
	UPROPERTY(Transient, BlueprintReadWrite)
	float			EndSectionAngle;
	//UPROPERTY(Transient, BlueprintReadWrite)
	//FVector2D		EndSectionDir;
	UPROPERTY(Transient, BlueprintReadWrite)
	FVector StartClippedPlaneDir;
	UPROPERTY(Transient, BlueprintReadWrite)
	FVector EndClippedPlaneDir;

	UPROPERTY(Transient, BlueprintReadWrite)
	FString SkirtingMeshID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkirtingMeshModelID;
	UPROPERTY(Transient, BlueprintReadWrite)
	FString MaterialID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialModelID;

	UPROPERTY(Transient, BlueprintReadWrite)
	ECgHeadingDirection HeadingDir;
	UPROPERTY(Transient, BlueprintReadWrite)
	ESkirtingType SkirtingType;
	UPROPERTY(Transient, BlueprintReadWrite)
	FString	OwnerWall; // may be null

	void AddInterval(const FVector2D &StartPnt, const FVector2D &EndPnt)
	{
		SkirtingIntervals.Add(FSkirtingInterval(StartPnt, EndPnt));
	}

	void ClearIntervalCache()
	{
		SkirtingIntervals.Empty();
	}

	const TArray<FSkirtingInterval>& GetCachedIntervals() const
	{
		return SkirtingIntervals;
	}

protected:
	TArray<FSkirtingInterval> SkirtingIntervals;
};


// skirting node
USTRUCT(Blueprintable)
struct FSkirtingNode
{
	GENERATED_BODY()
public:
	FSkirtingNode()
	{
		SkirtingMeshModelID = MaterialModelID = -1;
		SkirtingMeshModelID = 22506;
	}

	void CopyAttrToSegNode();

	bool IsClosedSkirting() const;

	void InitFromAreaPath(const TArray<FVector2D> &AreaPath, ESkirtingType InSkirtingType,bool isClosure = true);

	void AutoHeadingDirByClosedPath(const TArray<FVector2D> &ClosedPath);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkirtingMeshModelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SkirtingMeshID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialModelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MaterialID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECgHeadingDirection HeadingDir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkirtingType SkirtingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkirtingSrcType SrcType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RoomID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RoomClassID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SKUID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CraftId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bClosed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkirtingSegNode> SkirtingSegments;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Uuid;//Î¨Ò»Ê¶±ð
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasComponent;
};


// skirting path surrounding area
USTRUCT(Blueprintable)
struct FRoomPath
{
	GENERATED_BODY()

	FRoomPath()
	{
		bIsLivingOrDiningRoom = false;
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector2D> InnerRoomPath;
	UPROPERTY(BlueprintReadWrite)
	bool bIsLivingOrDiningRoom;
	UPROPERTY(BlueprintReadWrite)
	AActor * RoomActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AreaTag;
};


// simplified attributes for door or ground window
USTRUCT(Blueprintable)
struct FOpeningSimpleAttr
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float	LeftWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float	RightWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float	ZPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float	Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOpeningType Type;
};


USTRUCT(Blueprintable)
struct FSkirtingLinedefaultNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkirtingMeshModelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RoomClassID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SKUID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CraftId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SkirtingLineResID;
};
