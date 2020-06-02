// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cabinet/CabinetSave.h"
#include "CustomDataStructure.generated.h"


UENUM(BlueprintType)
enum class ECustomFurType : uint8
{
	None = 0		 UMETA(DisplayName = "无"),
	FloorCabinet	 UMETA(DisplayName = "地柜"),
	WallCupboard     UMETA(DisplayName = "吊柜"),
};

UENUM(BlueprintType)
enum class EArticlelofting: uint8
{
	None = 0		    UMETA(DisplayName = "无"),
	ForwardBreakwater	UMETA(DisplayName = "前挡水板"),
	LastBreakwater	    UMETA(DisplayName = "后挡水板"),
	Skirtingboard       UMETA(DisplayName = "踢脚板"),
	VertexAngle         UMETA(DisplayName = "顶角线"),
};


UENUM(BlueprintType)
enum class ECustomTwoFurType : uint8
{
	None = 0		 UMETA(DisplayName = "无"),
	CustomCabinet	 UMETA(DisplayName = "橱柜"),
};


UENUM(BlueprintType)
enum class ECustomState : uint8
{
	None = 0		 UMETA(DisplayName = "普通视口"),
	ProcessControl	 UMETA(DisplayName = "流程控制"),
	ThreeView		 UMETA(DisplayName = "三视口"),
};

USTRUCT(BlueprintType)
struct FIsLoft
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsSkirting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTopboard;
};


USTRUCT(BlueprintType)
struct FCustomData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECustomFurType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TeplateId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Categoryid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Pos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> SKU_ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> SKU_Pos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> SKU_Rota;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fRoteZ;

	//3两边显示 1 mesh_1 2 mesh_2 0都不显示
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Meshvisible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<bool> SKU_PosType;//true：中心 false：边缘
};


USTRUCT(BlueprintType)
struct FSkuData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 modelid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 skuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Categoryid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MD5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsFiltration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString UUID;//唯一识别id
};

USTRUCT(BlueprintType)
struct FMaterialColorData
{
	GENERATED_USTRUCT_BODY()
	FMaterialColorData()
	{
		propertyId = 1;
	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 propertyId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 propertyValueCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MD5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 propertyValueId;
};

USTRUCT(BlueprintType)
struct FCustomFurnitureWallData
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D end;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D Nor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 WallID;

};


USTRUCT(BlueprintType)
struct FCustomMaterialData
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 materialId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MD5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32  modelId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32  ChargeUnit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CategoryID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FMaterialColorData> ColorData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString customJson;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString guid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CategoryName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FVector> Scaleinfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 doorHandleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 doorhandleDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool MeshVisieble;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 InternalType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CabinetBodyType;
};

USTRUCT(BlueprintType)
struct FCustomActorData
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CategoryID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ParentCategoryID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ParentCategoryName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Gourpid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 GourpType;



};


USTRUCT(BlueprintType)
struct FCustomFurnitureData
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 TemplateID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ActorID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 TemplateCategoryID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCustomMaterialData> MaterialListData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Pos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECustomFurType FurnitureType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CounterclockwiseIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> Skuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray <FVector> Sku_Pos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTransform> SkuTrabsform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<int32, FSkuData>Skuinfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCustomActorData Actordata;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Roomid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> Modlelist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FString> CustomInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32  Versions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FIsLoft loft;
};



USTRUCT(BlueprintType)
struct FCustomDynamicTableboarddata
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector2D> ToPointList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector2D> Holes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Ids;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Hight;
};

USTRUCT(BlueprintType)
struct FCustomExpansionDrawingdata
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> ToPointList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> Ids;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Hight;
};


USTRUCT(BlueprintType)
struct FCustomLoftdata
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCustomDynamicTableboarddata>Tablelist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCustomExpansionDrawingdata>	ForwardBreakwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCustomExpansionDrawingdata>	LastBreakwater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCustomExpansionDrawingdata> Skirtboard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCustomExpansionDrawingdata> TopBoard;
};



USTRUCT(BlueprintType)
struct FCustomAllSaveData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCustomFurnitureWallData> WallData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCustomFurnitureData>FurnitureData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECustomTwoFurType FurnitureType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> floorWallid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> celiWallid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsVisible;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCustomLoftdata Loftdata;

};