#pragma once
#include "CoreMinimal.h"
#include "HomeLayout/HouseArchitect/PlaneHousePicture.h"
#include "CabinetStruct.generated.h"

USTRUCT(Blueprintable)
struct FCabinetPillar
{
	GENERATED_BODY()
		FCabinetPillar() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32		mType;	//0.烟道，1.柱子 2.大于550的烟道柱子
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mSize;//尺寸的一半
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mDir;
};

USTRUCT(Blueprintable)
struct FCabinetAreaRaw
{
	GENERATED_BODY()
		FCabinetAreaRaw() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FAreaInnerWall> mWallLines;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCustomizationBoolean> mHoles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetPillar> mPillars;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mName;
};

USTRUCT(Blueprintable)
struct FCabinetWallLine
{
	GENERATED_BODY()
		FCabinetWallLine() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mStart;//色块范围起点
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mEnd;  //色块范围终点
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32		Id;//web自定义的Id（可不填）
};

USTRUCT(Blueprintable)
struct FCabinetAutoSet
{
	GENERATED_BODY()
		FCabinetAutoSet() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32						mCategoryId;//定制类目id
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetWallLine>	mSelectWalls;//选择的区域
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32						mActionType; //0：布局 1：偏好布局
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32						mDepth;	//深度(只有榻榻米定制才有)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32						mFloor;	//地柜布局 1,2 吊柜布局
};

USTRUCT(Blueprintable)
struct FCabinetModle
{
	GENERATED_BODY()
		FCabinetModle() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 actorId;		//DR对应的物品唯一ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 templateId;	//模板ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 categoryId;	//类目ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float turnBSize;	//转角柜短边长度
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString categoryName;// " : "地柜"，//类目
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector	modelSize;		 // "" : {“dx”:500, "dy" : 100, "dz" : 1000},//模型原始尺寸
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector point;//	"point" : {"x":0, "y" : 0, "z" : 0},//位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector	rotate;// "rotate" : {"yAxis":0, "xAxis" : 0, "zAxis" : 0},//旋转
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector	scale;// "scale" : {"x":1, "y" : 1, "z" : 1},//缩放
};

USTRUCT(Blueprintable)
struct FCabinetSkuModel
{
	GENERATED_BODY()
		FCabinetSkuModel() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 skuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 type;//0.炉灶 1.水槽）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	point;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rotate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCabinetWallLine	wallline;
};


USTRUCT(Blueprintable)
struct FCabinetTatamiExtern
{
	GENERATED_BODY()
		FCabinetTatamiExtern() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int type;// "type":0 / 1 (0左1右）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int selected;//	"selected":0 / 1(0.未选中，1选中)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D point;//	"point" : {"x":0, "y" : 0},//位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rotate;//	"rotate" : 0//旋转
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D size;//	"size" : {“dx”:500, "dy" : 100},//尺寸
};

USTRUCT(Blueprintable)
struct FCabinetWalldata
{
	GENERATED_BODY()
		FCabinetWalldata() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 success;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString msg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCabinetWallLine	wallline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetModle>	mCabinets;
};
USTRUCT(Blueprintable)
struct FCabinetLayoutResult
{
	GENERATED_BODY()
		FCabinetLayoutResult() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 success;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString msg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 comtype;// ":0/1/2(0."initLayout" 1.“replaceLayoutTemplate”2.“selectedwardrobelayout” )
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetModle>	mCabinets;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< FCabinetSkuModel> mCookExtern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< FCabinetTatamiExtern> mTatamiExtern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetWalldata> mFailDatas;
};




USTRUCT(Blueprintable)
struct FCabinetOverlayResult
{
	GENERATED_BODY()
		FCabinetOverlayResult() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetWalldata> mFailDatas;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString msg;
};