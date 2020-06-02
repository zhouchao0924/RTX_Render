
#pragma once
#include "DRStruct.generated.h"

UENUM(BlueprintType)
enum class EDRModelType : uint8
{
	EDRNull,
	EDRModel,
	EDRRoomCeill,
	EDRRoomFloor,
	EDRWindowHole,
	EDRWindow,
	EDRBayWindow,
	EDRFloorWindow,
	EDRDoor,
	EDRDoubleDoor,
	EDRSlidingDoor,
	EDRDoorHole,
	EDRWindowSill,
	EDRDoorSone,
	EDRSkirtingCeil,
	EDRSkirtingFloor,
	EDRCustomCurtain,
	EDRLight,

};

UENUM(BlueprintType)
enum class ENewWallType :uint8
{
	EPlasterWall,//石膏
	EBrickWall//砖墙
};

UENUM(BlueprintType)
enum class EHolestatusType :uint8
{
	EDRNormalHole,
	EDRNewAddHole,
	EDRDeleteHole
};


UENUM(BlueprintType)
enum class EDRComplex : uint8
{
	ENull,
	EDoor,
	EDoorL,
	EDoorR,
	EDoorFrame,
	EDoorSone,
	EWindowSill
};
UENUM(BlueprintType)
enum class EDRWallType : uint8
{
	ENull,
	ESolidWall,
	EVirtualWall
};

UENUM(BlueprintType)
enum class EDRLightType : uint8
{
	EDRPointLight,
	EDRSpotLight
};

UENUM(BlueprintType)
enum class EModelType : uint8
{
	EDRNull,
	EDRNormalModel,
	EDRDoorFram,
	EDRDoor,
	EDRDoubleDoor,
	EDRSliderDoor,
	EDRDoorWay,
	EDRWindow,
	EDRGroundWindow,
	EDRBayWindow,
};

UENUM(BlueprintType)
enum class ETextureSlot : uint8
{
	EDiffuse,
	ESpecular,
	EEmissive,
	ERoughness,
	EMetallic,
	EOpacity,
	ENormal
};

USTRUCT(BlueprintType)
struct FLinePos
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FVector2D Start;
	UPROPERTY(BlueprintReadWrite)
		FVector2D End;
};

USTRUCT(BlueprintType)
struct FDRLight
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int32 ID;
	UPROPERTY(BlueprintReadWrite)
		EDRModelType ObjType;
	UPROPERTY(BlueprintReadWrite)
		EDRLightType Type;
	UPROPERTY(BlueprintReadWrite)
		FLinearColor LightColor;
	UPROPERTY(BlueprintReadWrite)
		bool isCastShadows;
	UPROPERTY(BlueprintReadWrite)
		bool isVisible;
	UPROPERTY(BlueprintReadWrite)
		FVector  Location;
	UPROPERTY(BlueprintReadWrite)
		bool hasBeenDeleted;
	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
};

USTRUCT(BlueprintType)
struct FDRPointLight : public FDRLight
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		float SoftSourceRadius;
	UPROPERTY(BlueprintReadWrite)
		float SourceRadius;
	UPROPERTY(BlueprintReadWrite)
		float SourceLength;
	UPROPERTY(BlueprintReadWrite)
		float Intensity;
	UPROPERTY(BlueprintReadWrite)
		float ShadowResolutionScale;
	UPROPERTY(BlueprintReadWrite)
		float AttenuationRadius;
	UPROPERTY(BlueprintReadWrite)
		bool IsAutoPoint;
};

USTRUCT(BlueprintType)
struct FDRSpotLight : public FDRLight
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FRotator Rotation;
	UPROPERTY(BlueprintReadWrite)
		float SoftSourceRadius;
	UPROPERTY(BlueprintReadWrite)
		float SourceRadius;
	UPROPERTY(BlueprintReadWrite)
		float SourceLength;
	UPROPERTY(BlueprintReadWrite)
		float Intensity;
	UPROPERTY(BlueprintReadWrite)
		float ShadowResolutionScale;
	UPROPERTY(BlueprintReadWrite)
		float AttenuationRadius;
	UPROPERTY(BlueprintReadWrite)
		float InnerConeAngle;
	UPROPERTY(BlueprintReadWrite)
		float OuterConeAngle;
};

USTRUCT(BlueprintType)
struct FDRModelInstance
{
	GENERATED_USTRUCT_BODY()
public:

	FDRModelInstance();

	UPROPERTY(BlueprintReadWrite)
		int32 ModelID;
	UPROPERTY(BlueprintReadWrite)
		int32 BindID;
	UPROPERTY(BlueprintReadWrite)
		bool isShadow;
	UPROPERTY(BlueprintReadWrite)
		EDRModelType ModelType;
	UPROPERTY(BlueprintReadWrite)
		FVector Location;
	UPROPERTY(BlueprintReadWrite)
		FRotator Rotation;
	UPROPERTY(BlueprintReadWrite)
		FVector OriginalSize;
	UPROPERTY(BlueprintReadWrite)
		FVector Size;
	UPROPERTY(BlueprintReadWrite)
		FVector Scale;
	UPROPERTY(BlueprintReadWrite)
		FText Remarks;
	UPROPERTY(BlueprintReadWrite)
		FString ResID;
	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
	UPROPERTY(BlueprintReadWrite)
		bool IsFirstAdd;
};

USTRUCT(BlueprintType)
struct FDRComplexModel :public FDRModelInstance
{
	GENERATED_USTRUCT_BODY()
public:

	FDRComplexModel();

	UPROPERTY(BlueprintReadWrite)
		EDRComplex ComplexType;
};

USTRUCT(BlueprintType)
struct FDRHoleStruct
{
	GENERATED_USTRUCT_BODY()
public:

	FDRHoleStruct();

	UPROPERTY(BlueprintReadWrite)
		int32 HoleID;
	UPROPERTY(BlueprintReadWrite)
		FVector2D Location;
	UPROPERTY(BlueprintReadWrite)
		FRotator Rotation;
	UPROPERTY(BlueprintReadWrite)
		EDRModelType ModelType;
	UPROPERTY(BlueprintReadWrite)
		float Width;
	UPROPERTY(BlueprintReadWrite)
		float Hight;
	UPROPERTY(BlueprintReadWrite)
		float Thickness;
	UPROPERTY(BlueprintReadWrite)
		float GroundHeight;
	UPROPERTY(BlueprintReadWrite)
		bool bFlip;
	UPROPERTY(BlueprintReadWrite)
		bool bOpeningDoor;

	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
	UPROPERTY(BlueprintReadWrite)
		bool IsFirstAdd;
};

USTRUCT(BlueprintType)
struct FDRWallStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		EDRWallType Type;
	UPROPERTY(BlueprintReadWrite)
		int32 StartID;
	UPROPERTY(BlueprintReadWrite)
		int32 EndID;
	UPROPERTY(BlueprintReadWrite)
		float LeftThick;
	UPROPERTY(BlueprintReadWrite)
		float RightThick;
	UPROPERTY(BlueprintReadWrite)
		float Height;
	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
	UPROPERTY(BlueprintReadWrite)
		bool bRoomWall;
	UPROPERTY(BlueprintReadWrite)
		bool IsSave;
	FDRWallStruct();
};


USTRUCT(BlueprintType)
struct FDRVirtualWallStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsDelete;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index;
	FDRVirtualWallStruct();
};

USTRUCT(BlueprintType)
struct FDRHouseImage
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		bool CopyMode;
// 	UPROPERTY(BlueprintReadWrite)
// 		bool DeleteCDraw;
	UPROPERTY(BlueprintReadWrite)
		float SpinValue;
	UPROPERTY(BlueprintReadWrite)
		float SceneWorldDistance;
// 	UPROPERTY(BlueprintReadWrite)
// 		float PlaneRotation;
	UPROPERTY(BlueprintReadWrite)
		FString Path;
	UPROPERTY(BlueprintReadWrite)
		FVector Scene0Location;
	UPROPERTY(BlueprintReadWrite)
		FVector SLocation;
	UPROPERTY(BlueprintReadWrite)
		FVector PlaneLocation;
	UPROPERTY(BlueprintReadWrite)
		FVector PlaneRotation;
	UPROPERTY(BlueprintReadWrite)
		FVector PlaneScale;
// 	UPROPERTY(BlueprintReadWrite)
// 		FVector SelfLocation;
// 	UPROPERTY(BlueprintReadWrite)
// 		FVector SelfRotation;
// 	UPROPERTY(BlueprintReadWrite)
// 		FVector SelfScale;

	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
};

USTRUCT(BlueprintType)
struct FSaveMaterial
{
	GENERATED_USTRUCT_BODY()
public:
	//材质相关参数
	UPROPERTY(BlueprintReadWrite)
		int32 ModelID;
	UPROPERTY(BlueprintReadWrite)
		FString ResID;
	UPROPERTY(BlueprintReadWrite)
		FString Url;
	UPROPERTY(BlueprintReadWrite)
		FVector2D UV;
	UPROPERTY(BlueprintReadWrite)
		FVector2D UVOffset;
	UPROPERTY(BlueprintReadWrite)
		float Angle;
};

USTRUCT(BlueprintType)
struct FRoom
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		EDRModelType Type;
	UPROPERTY(BlueprintReadWrite)
		int32 RoomID;
	UPROPERTY(BlueprintReadWrite)
		FString RoomName;
	UPROPERTY(BlueprintReadWrite)
		FString UseName;
	UPROPERTY(BlueprintReadWrite)
		FVector Mark;
	UPROPERTY(BlueprintReadWrite)
		float GroundHeight;
	UPROPERTY(BlueprintReadWrite)
		float Thickness;
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> InnerPolygons;
	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
	//材质保存数据，包括历史记录，默认使用是第4个（从0开始）
	UPROPERTY(BlueprintReadWrite)
		TArray<FSaveMaterial> Materials;
	
	//UPROPERTY(BlueprintReadWrite)
	//	TArray<FVector> OuterPolygons;
};

USTRUCT(BlueprintType)
struct FSkirting
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		EDRModelType Type;
	UPROPERTY(BlueprintReadWrite)
		int32 SkirtingID;
	UPROPERTY(BlueprintReadWrite)
		int32 RoomID;
	UPROPERTY(BlueprintReadWrite)
		int32 ModelID;
	UPROPERTY(BlueprintReadWrite)
		FVector Extent;
	UPROPERTY(BlueprintReadWrite)
		FString ResID;
	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
};

USTRUCT(BlueprintType)
struct FCustomCurtain
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		EDRModelType Type;
	UPROPERTY(BlueprintReadWrite)
		int32 TemplateID;
	UPROPERTY(BlueprintReadWrite)
		int32 GroupID;
	UPROPERTY(BlueprintReadWrite)
		float Length;
	UPROPERTY(BlueprintReadWrite)
		float Height;
	UPROPERTY(BlueprintReadWrite)
		float DistanceBetweenBar;
	UPROPERTY(BlueprintReadWrite)
		FVector Location;
	UPROPERTY(BlueprintReadWrite)
		FRotator Rotation;
	UPROPERTY(BlueprintReadWrite)
		FString ModelTag;
	UPROPERTY(BlueprintReadWrite)
		int32  RomebarNumber;
	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
};

USTRUCT(BlueprintType)
struct FGlobalLight
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int32 ID;
	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
};

USTRUCT(BlueprintType)
struct FDirectionLight : public FGlobalLight
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FRotator Rotation;
	UPROPERTY(BlueprintReadWrite)
		FLinearColor Color;
	UPROPERTY(BlueprintReadWrite)
		float Intensity;
};

USTRUCT(BlueprintType)
struct FDRSkyLight : public FGlobalLight
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		float Intensity;
	UPROPERTY(BlueprintReadWrite)
		FLinearColor Color;
	UPROPERTY(BlueprintReadWrite)
		float Angle;
};


USTRUCT(BlueprintType)
struct FPostProcess
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int32 ID;
	UPROPERTY(BlueprintReadWrite)
		float Saturation;
	UPROPERTY(BlueprintReadWrite)
		FVector Constrast;
	UPROPERTY(BlueprintReadWrite)
		float BloomIntensity;
	UPROPERTY(BlueprintReadWrite)
		float AmbientOcclusion;
	UPROPERTY(BlueprintReadWrite)
		float AmbientOcclusionRadius;
	UPROPERTY(BlueprintReadWrite)
		float CrushHighLights;
	UPROPERTY(BlueprintReadWrite)
		float HaloSize;
	UPROPERTY(BlueprintReadWrite)
		int32 OriginalDataID;
};


class ARoomActor;
class ALineWallActor;

//户型2.0
UENUM(BlueprintType)
enum class EWallSufaceInfoType : uint8
{
	EBottom,
	ETop,
	EBack,
	EFront,
	ERight,
	ELeft,
	ENull
};


USTRUCT(BlueprintType)
struct FWallSufaceInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		ARoomActor * OwnerRoom;
	UPROPERTY(BlueprintReadOnly)
		ALineWallActor * OwnerWall;
	UPROPERTY(BlueprintReadOnly)
		float Acreage;
	UPROPERTY(BlueprintReadOnly)
		int32 SurFaceIndex;
	UPROPERTY(BlueprintReadOnly)
		EWallSufaceInfoType Type;

public:
	FWallSufaceInfo();
	void UpdateSurfaceData();
private:
	void SetSurFaceType(int32 index);
};

USTRUCT(BlueprintType)
struct FWallBindInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		ALineWallActor * Wall;
	UPROPERTY(BlueprintReadOnly)
		TArray<FWallSufaceInfo> SurFaces;

	FWallBindInfo();
};


USTRUCT(BlueprintType)
struct FRoomBindInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		ARoomActor * Room;
	UPROPERTY(BlueprintReadOnly)
		float Acreage;
	UPROPERTY(BlueprintReadOnly)
		TArray<FWallBindInfo> Walls;
public:
	void UpdateSurfaceData();
};

USTRUCT(BlueprintType)
struct FDeleteWall
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		bool isDelete;
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> Point;
	UPROPERTY(BlueprintReadWrite)
		float WallArea;
	UPROPERTY(BlueprintReadWrite)
		float Thickness;
	UPROPERTY(BlueprintReadWrite)
		float Height;
	UPROPERTY(BlueprintReadWrite)
		FVector StartPos;
	UPROPERTY(BlueprintReadWrite)
		FVector EndPos;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ThicknessLeft;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ThicknessRight;
};

USTRUCT(BlueprintType)
struct FTempHoleAndModel
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int32 HoleIndex;
	UPROPERTY(BlueprintReadWrite)
		int32 ModelIndex;
	FTempHoleAndModel();
};

USTRUCT(BlueprintType)
struct FIntelligentLight
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FVector Loc;
	UPROPERTY(BlueprintReadWrite)
		FVector Dir = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
		FLinearColor LightColor = FLinearColor::White;
	UPROPERTY(BlueprintReadWrite)
		float Intensity = 6777.0;
	UPROPERTY(BlueprintReadWrite)
		float OuterConeAngle = 80.f;
};

USTRUCT(BlueprintType)
struct FHouseInfoValue
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FString HouseName;
	UPROPERTY(BlueprintReadWrite)
		FString CommunityName;
	UPROPERTY(BlueprintReadWrite)
		FString RegionName;
	UPROPERTY(BlueprintReadWrite)
		bool ExtdeesigerType;
	UPROPERTY(BlueprintReadWrite)
		bool IsShared;
	UPROPERTY(BlueprintReadWrite)
		bool IsCheck;
	UPROPERTY(BlueprintReadWrite)
		int32 CommunityID;
	UPROPERTY(BlueprintReadWrite)
		FString RegionNameLevel1;
	UPROPERTY(BlueprintReadWrite)
		FString RegionNameLevel2;
	UPROPERTY(BlueprintReadWrite)
		FString RegionNameLevel3;
	UPROPERTY(BlueprintReadWrite)
		int32 RegionLevel1ID;
	UPROPERTY(BlueprintReadWrite)
		int32 RegionLevel2ID;
	UPROPERTY(BlueprintReadWrite)
		int32 RegionLevel3ID;
	UPROPERTY(BlueprintReadWrite)
		bool OperationRole;
	UPROPERTY(BlueprintReadWrite)
		bool ExternalProgram;
	UPROPERTY(BlueprintReadWrite)
		bool IsMyHouse;
	UPROPERTY(BlueprintReadWrite)
		FString CopyImageURL;
	FHouseInfoValue();
};

UENUM(BlueprintType)
enum class EComponenetType :uint8
{
	_NONE,
	_FLUE,
	_BagRiser,
	_Pillar,
	_Sewer
};

USTRUCT(BlueprintType)
struct FComponenetInfo
{
	GENERATED_BODY()
public:
	FComponenetInfo()
		:AngleVal(0.0f)
		, IsWidth(false)
		, Loc(0, 0)
		, Width(-1)
		, Length(-1)
		, Height(280)
		, ComponenetType(EComponenetType::_NONE)
		, RoomID(-1)
		, DRActorID(-1)
		, modelID(22314)
		, RoomClassID(16)
		, CraftID(301)
		, Offset(0, 0, 0)
		, Scale(100, 100, 0)
		, Angle(0.0f)
	{
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> WallID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AngleVal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D Loc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EComponenetType ComponenetType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 DRActorID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D> TPoylgon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<int32, FString> MaterialInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 modelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomClassID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CraftID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString UUID;//唯一识别uuid
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Scale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Angle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString SewerName;
};

USTRUCT(BlueprintType)
struct FRoomInfoByAI
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FString RoomName;
	UPROPERTY(BlueprintReadWrite)
		FString RoomUserName;
	UPROPERTY(BlueprintReadWrite)
		int32 RoomID;
	UPROPERTY(BlueprintReadWrite)
		int32 RoomUserID;
	UPROPERTY(BlueprintReadWrite)
		int32 TypeID;
	UPROPERTY(BlueprintReadWrite)
		FVector2D Point;
};

USTRUCT(BlueprintType)
struct FDeleteHoleInfo
{
	GENERATED_USTRUCT_BODY()
public:
	FDeleteHoleInfo()
		:EFillWallType(ENewWallType::EPlasterWall)
	{
	}
	UPROPERTY(BlueprintReadWrite)
		EModelType HoleType;
	UPROPERTY(BlueprintReadWrite)
		FVector Loc;
	UPROPERTY(BlueprintReadWrite)
		float Angle;
	UPROPERTY(BlueprintReadWrite)
		float Thickness;
	UPROPERTY(BlueprintReadWrite)
		float Width;
	UPROPERTY(BlueprintReadWrite)
		float Height;
	UPROPERTY(BlueprintReadWrite)
		float Length;
	UPROPERTY(BlueprintReadWrite)
		float GroundHeight;
	UPROPERTY(BlueprintReadWrite)
		bool Horizontalflip;
	UPROPERTY(BlueprintReadWrite)
		bool Verticalflip;
	UPROPERTY(BlueprintReadWrite)
		ENewWallType EFillWallType;
};

UENUM(BlueprintType)
enum class EDConfigType : uint8
{
	EDRWallConfig,
	EDRAreaGroundConfig,
	EDRAreaRoofConfig,
	EDRTopSkirtingConfig,
	EDRSkirtingConfig,
	EDRDoorConfig,
	EDRDoorFrameConfig,
	EDRDoubleDoorConfig,
	EDRDoubleDoorFrameConfig,
	EDRSliderDoorConfig,
	EDRSliderDoorFrameConfig,
	EDRDoorWayFrameConfig,
	EDRWindowConfig,
	EDRGroundWindowConfig,
	EDRBayWindowConfig,
};

USTRUCT(BlueprintType)
struct FDHouseConfigModelID
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> AllDHouseConfigModelIDs;
	FDHouseConfigModelID()
	{
		InitConfig();
	}

	int32 GetConfigModelIDByType(const EDConfigType Type)
	{
		if (AllDHouseConfigModelIDs.Num() == 0)
		{
			InitConfig();
		}

		if (AllDHouseConfigModelIDs.IsValidIndex((int32)Type))
			return AllDHouseConfigModelIDs[(int32)Type];
		else
		{
			InitConfig();
			return AllDHouseConfigModelIDs[(int32)Type];
		}
	}

	void InitConfig()
	{
		AllDHouseConfigModelIDs.SetNum(15);
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRWallConfig] = 22314;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRAreaGroundConfig] = 22299;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRAreaRoofConfig] = 22314;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRTopSkirtingConfig] = 22507;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRSkirtingConfig] = 22506;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRDoorConfig] = 22496;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRDoorFrameConfig] = 22589;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRDoubleDoorConfig] = 22496;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRDoubleDoorFrameConfig] = 24616;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRSliderDoorConfig] = 22547;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRSliderDoorFrameConfig] = 22502;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRDoorWayFrameConfig] = 22502;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRWindowConfig] = 22773;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRGroundWindowConfig] = 31831;
		AllDHouseConfigModelIDs[(int32)EDConfigType::EDRBayWindowConfig] = 16514;
	}
};