#pragma once
#include "CoreMinimal.h"
#include "HomeLayout/HouseArchitect/PlaneHousePicture.h"
#include "CabinetSave.generated.h"
//////////////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ESplineTypeBlue : uint8
{
	SplineBlueTableBack,//后档
	SplineBlueFloorBottom,//脚线
	SplineBlueHangTop,//顶线
	SplineBlueFront,//前挡
	SplineBlueTypeNum,
};

#define TCabibetSaveVersion 100

UENUM(BlueprintType)
enum class EFCabinetVersion : uint8
{
	Unkown = 0,
	ETCabibetSaveVersion = TCabibetSaveVersion,
};

USTRUCT(Blueprintable)
struct FCabinetSave
{
	GENERATED_BODY()
		FCabinetSave() 
	{
		mbSpline.Add(false);
		mbSpline.Add(false);
		mbSpline.Add(false);
		mbSpline.Add(false);
		mMeshScale=FVector(1, 1, 1);
		//mMeshScale = "";
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32	Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D mSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D mScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D mDir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D mPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32	  mTemplateId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<bool>	  mbSpline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32	  mPosZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32	  mHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString>		  mHardSkuMd5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32>		mCategoryId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString	  		    mBoyMd5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector				mMeshScale;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	FString				mMeshScale;
};

USTRUCT(Blueprintable)
struct FLine2dSave
{
	GENERATED_BODY()
		FLine2dSave() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32	Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int	mState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D mStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D mEnd;
};

USTRUCT(Blueprintable)
struct FLine2dSelect
{
	GENERATED_BODY()
		FLine2dSelect() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D mStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D mEnd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32	Id;
};

USTRUCT(Blueprintable)
struct FCabinetSetGroupBlue
{
	GENERATED_BODY()
		FCabinetSetGroupBlue() { mSetType = -1; }
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32	mSetType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> mSplinesMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> mSplineMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString		mTableMeterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetSave> mCabinets;
};

USTRUCT(Blueprintable)
struct FSkuDataSave
{
	GENERATED_BODY()
		FSkuDataSave()
	{
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32	mSkuId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32	mCategoryId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mMxFileMD5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector	mSize;
};

USTRUCT(Blueprintable)
struct FSkuInstanceSave
{
	GENERATED_BODY()
		FSkuInstanceSave()
	{
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSkuDataSave	mSku;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mDir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32		mLeftCabinetId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32		mRightCabinetId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32		mHeight;
};

USTRUCT(Blueprintable)
struct FCabinetAreaSave
{
	GENERATED_BODY()
		FCabinetAreaSave() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FLine2dSave> mWalls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FAreaInnerWall> mOutWallLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetSetGroupBlue> mCabinetGroups;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSkuInstanceSave mSkuSink;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSkuInstanceSave mSkuChimery;
};


USTRUCT(Blueprintable)
struct FCabinetAreaMgrSave
{
	GENERATED_BODY()
		FCabinetAreaMgrSave() 
	{
		mVersion = 0;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 mVersion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 mCabinetIdxMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> mAuxMxs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetAreaSave> mAreas;
};