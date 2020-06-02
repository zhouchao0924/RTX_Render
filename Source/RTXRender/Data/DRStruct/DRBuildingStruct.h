#pragma once
#include "../Building/DRStruct.h"
#include "DRBuildingStruct.generated.h"




/*****************Enum*******************/

UENUM(BlueprintType)
enum class ESolidWallType : uint8
{
	EDROriginalWall,
	EDRNewWall,
};

UENUM(BlueprintType)
enum class ELightType : uint8
{
	EDRNull,
	EDRPointLight,
	EDRSpotLight,
	EDRSpotLight_ComplexIES,
	EDRSpotLight_NarrowComplexIES,
};


/*****************Struct*******************/

USTRUCT(Blueprintable)
struct FDRMaterial
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ModelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomClassID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CraftID;
};

USTRUCT(Blueprintable)
struct FDRUV
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D UVScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D UVOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float UVAngle;
};

USTRUCT(Blueprintable)
struct FDRWallPosition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector StartPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector EndPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector LeftStartPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector LeftEndPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RightStartPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RightEndPos;
};

USTRUCT(BlueprintType)
struct FPreAreaData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector2D> VectorList;
	UPROPERTY(BlueprintReadWrite)
		FString TagName;
	UPROPERTY(BlueprintReadWrite)
		float Height;
	UPROPERTY(BlueprintReadWrite)
		FVector2D UV;
	UPROPERTY(BlueprintReadWrite)
		FVector2D UVLocation;
	UPROPERTY(BlueprintReadWrite)
		float UVRotation;
	UPROPERTY(BlueprintReadWrite)
		int32 RoomId;
	UPROPERTY(BlueprintReadWrite)
		FString RoomName;
	UPROPERTY(BlueprintReadWrite)
		int32 RoomUsageId;
	UPROPERTY(BlueprintReadWrite)
		FString RoomUsageName;
	UPROPERTY(BlueprintReadWrite)
		float Area;
	UPROPERTY(BlueprintReadWrite)
		TArray<FDRMaterial> AreaMaterials;
	UPROPERTY(BlueprintReadWrite)
		int32 TypeId;
	UPROPERTY(BlueprintReadWrite)
		int32 RoomUseIndex = -1;
};

USTRUCT(BlueprintType)
struct FDRLightBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ELightType LightTyp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LightIntensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor LightColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttenuationRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SourceRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Temperature;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Highlight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ShadowResolutionScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsCastShadow;
};

USTRUCT(BlueprintType)
struct FDRPointLightBase : public FDRLightBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SourceLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsAutoPoint;
};

USTRUCT(BlueprintType)
struct FDRSpotLightBase : public FDRLightBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InnerConeAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float OuterConeAngle;
};

USTRUCT(BlueprintType)
struct FDRRectLightBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ELightType LightTyp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LightIntensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor LightColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttenuationRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SourceWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SourceHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BarnDoorAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BarnDoorLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Temperature;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Highlight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsCastShadow;

};

USTRUCT(BlueprintType)
struct FDNormalModel
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EModelType ModelType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ResId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ModelTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ModelId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CustomSkuId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomClassId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CraftId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool ApplyShadow = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDRUV UV;
	UPROPERTY(BlueprintReadWrite)
		TMap<int32, FDRMaterial>DependsMat;
};


USTRUCT(BlueprintType)
struct FDRSolidWall
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FString TagName;
	UPROPERTY(BlueprintReadWrite)
		FVector StartPos;
	UPROPERTY(BlueprintReadWrite)
		FVector EndPos;
	UPROPERTY(BlueprintReadWrite)
		float LeftThick;
	UPROPERTY(BlueprintReadWrite)
		float RightThick;
	UPROPERTY(BlueprintReadWrite)
		float Height;
	UPROPERTY(BlueprintReadWrite)
		bool RoomWall;
	UPROPERTY(BlueprintReadWrite)
		bool MainWall;
	UPROPERTY(BlueprintReadWrite)
		bool LeftRuler;
	UPROPERTY(BlueprintReadWrite)
		bool RightRuler;
	UPROPERTY(BlueprintReadWrite)
		ESolidWallType SolidWallType;
	UPROPERTY(BlueprintReadWrite)
		TArray<FDRUV> SolidWallUV;//0==Front;1==Back;2==Side;
	UPROPERTY(BlueprintReadWrite)
		TArray<FDRMaterial> SolidWallMaterials;
	UPROPERTY(BlueprintReadWrite)
		ENewWallType NewWallType;
	FDRSolidWall()
		:NewWallType(ENewWallType::EPlasterWall)
	{
		for (int i = 0; i < 3; ++i)
		{
			SolidWallUV.Add(FDRUV());
		}
	}
};

USTRUCT(BlueprintType)
struct FDRHole
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		FTransform Transform;
	UPROPERTY(BlueprintReadWrite)
		float Width;
	UPROPERTY(BlueprintReadWrite)
		float Height;
	UPROPERTY(BlueprintReadWrite)
		float ZPos;
	UPROPERTY(BlueprintReadWrite)
		bool HorizontalFlip;
	UPROPERTY(BlueprintReadWrite)
		bool VerticalFlip;
	UPROPERTY(BlueprintReadWrite)
		FString SelfTagName;
	UPROPERTY(BlueprintReadWrite)
		FString SlotActorTag;
	UPROPERTY(BlueprintReadWrite)
		EModelType HoleType;
	UPROPERTY(BlueprintReadWrite)
		TArray<FDRUV> HoleUVs;
	UPROPERTY(BlueprintReadWrite)
		TArray<FDRMaterial> HoleMaterials;
	UPROPERTY(BlueprintReadWrite)
		EHolestatusType HolestatusType;
};

USTRUCT(BlueprintType)
struct FDRVirtualWall
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector2D> VirtualWallPoint;
};

USTRUCT(BlueprintType)
struct FDRDoorAndWindow
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		EModelType ModelType;
	UPROPERTY(BlueprintReadWrite)
		FVector Loction;
	UPROPERTY(BlueprintReadWrite)
		TArray<int32> ModelIds;//0==Door;1==DoorFram;
	UPROPERTY(BlueprintReadWrite)
		TArray<int32> RoomClassIds;
	UPROPERTY(BlueprintReadWrite)
		TArray<int32> CraftIDs;
	UPROPERTY(BlueprintReadWrite)
		FString TagName;
	UPROPERTY(BlueprintReadWrite)
		FString Name;
	UPROPERTY(BlueprintReadWrite)
		TArray<FDRMaterial> DependsMats;
	FDRDoorAndWindow()
	{
		for (int i = 0; i < 2; ++i)
		{
			ModelIds.Add(0);
			RoomClassIds.Add(0);
			CraftIDs.Add(0);
			DependsMats.Add(FDRMaterial());
		}
	};
};

USTRUCT(BlueprintType)
struct FDRBasicArray
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		TArray<int32> IntArray;
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector2D> Vector2DArray;
};

USTRUCT(BlueprintType)
struct FDRComplexArray
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FDRMaterial> MatArray;
};

USTRUCT(BlueprintType)
struct FDRChangeHouseSubmitData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		int32 OldRoomId;
	UPROPERTY(BlueprintReadWrite)
		int32 NewRoomId;
	UPROPERTY(BlueprintReadWrite)
		int32 RoomUsageId;
	UPROPERTY(BlueprintReadWrite)
		FString RoomUsageName;
	UPROPERTY(BlueprintReadWrite)
		float Area;
	UPROPERTY(BlueprintReadWrite)
		int32 RoomType;
};