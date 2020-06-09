#pragma once

#include "NodeTypes.h"

#include "MaterialNode.h"
#include "FurnitureOtherInfoNode.h"
#include "OrderFurnitureStruct.h"

#include "../SceneEntity/OpeningActor.h"
#include "../SceneEntity/SceneInteriorActor.h"

#include "UnknownNode.generated.h"

// actor struct
USTRUCT(Blueprintable)
struct FActorStructCPP
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> ActorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UMaterialInterface*> Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ActorName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> Axios;
};
//
//// boolean ico
//USTRUCT(Blueprintable)
//struct FBooleanICO
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture2D *Texture;
//};
//
//
//// build saved
//USTRUCT(Blueprintable)
//struct FBuildSaved
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TSubclassOf<AOpeningActor> BuildType;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FTransform Transform;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector> VerList;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bIsDelect;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FMaterialNode> MatList;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Tag;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector2D> MatScale;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector2D> MatPosition;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<float> MatRotation;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bIsWallPillar;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bWallObliqueLine;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bIsWallRoof;
//};
//
//// company struct
//USTRUCT(Blueprintable)
//struct FCompanyStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 CompanyCode;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString CompanyName;
//};
//
//
//// config
//USTRUCT(Blueprintable)
//struct FConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ConfigPath;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ConfigString;
//};
//
//
//// config struct
//USTRUCT(Blueprintable)
//struct FConfigStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Code;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Value;
//};
//
//
//// debug line
//USTRUCT(Blueprintable)
//struct FDebugLine
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector StartLocation;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector EndLocation;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor Color;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bIsValid;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector Direction;
//};
//
//// draw line vector
//USTRUCT(Blueprintable)
//struct FDrawLineVector
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector2D VectorA;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector2D VectorB;
//};
//
//// draw string
//USTRUCT(Blueprintable)
//struct FDrawString
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector Location;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString String;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor Color;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Size;
//};
//
//// editor config
//USTRUCT(Blueprintable)
//struct FEditorConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString LocationAlignmentValue;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString RotationAlignmentValue;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ScaleAlignmentValue;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Display;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float SpeedCamera;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FOV;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bAuxiliaryLine;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bHideEditorElement;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bSpawnPointPlay;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bSurfaceCapture;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bWorldMode;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bShowFPS;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float MouseSpeed;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bFastRotation;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FLinearColor> ColorList;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor TextColor;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor AntiTextColor;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor ShadowColor;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float BlurValue;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bOpenBlur;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString StartProjectName;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Language;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float ToolScale;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor WallBackgroundColor;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor WallLineColor;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor WallLineSelectColor;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bWallIndependentAttributes;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor WallWindowDoorLineColor;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bWallAntiAliasing;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bOpenCtrlOperating;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 WallGrid;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bWallSplitLineShow;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bWallRulerShow;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bWallModeShow;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bWallBSPShow;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bWallCapture;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float WallCaptureValue;
//};
//
//// editor material saved config
////TODO
//USTRUCT(Blueprintable)
//struct FEditorMaterialSavedConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//};
//
//// game editor post processing
//USTRUCT(Blueprintable)
//struct FGameEditorPostProcessing
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector Contrast;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector Gamma;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector Offset;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor Tint;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Saturation;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float CrushHighLights;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FringeIntensity;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float VignetteIntensity;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float BloomIntensity;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FPostDepthOfField DOF;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Exposure;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float AmbientOcclusion;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float HaloSize;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float AmbientOcclusionRadius;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float WhiteTemp;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float WhiteTint;
//};
//
//// Goodses Struct
//USTRUCT(Blueprintable)
//struct FGoodsesStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//};
//
//// ICO
//USTRUCT(Blueprintable)
//struct FICO
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture2D *Texture;
//};
//
////IdearStruct
//USTRUCT(Blueprintable)
//struct FIdearStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString IdearName;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ChangeTime;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 ID;
//};
//
//// Image Buffer
//USTRUCT(Blueprintable)
//struct FImageBuffer
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture *Texture;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Width;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Hight;
//};
//
//// InterfaceColors
//USTRUCT(Blueprintable)
//struct FInterfaceColors
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture *Texture;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Width;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Hight;
//};
//
//// intersection polygon
//USTRUCT(Blueprintable)
//struct FIntersectionPolygon
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector2D> Polygon;
//};
//
//// Layer
//USTRUCT(Blueprintable)
//struct FLayer
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FString> ActorList;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bVisible;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bIsDelect;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor Color;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Location;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Height;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString DrawingPath;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector2D DrawingSize;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString DrawingTag;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FTransform Trans;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float DrawingTransparency;
//};
//
//// LineIndex
//USTRUCT(Blueprintable)
//struct FLineIndex
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 A;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 B;
//};
//
//// line segment
//USTRUCT(Blueprintable)
//struct FLineSegment
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 A;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 B;
//};
//
//// Map Light
//USTRUCT(Blueprintable)
//struct FMapLight
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FRotator ParallelLightRotation;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor ParallelLightColor;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float ParallelLightIntensity;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float SkyLightIntensity;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor SkyLightColor;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float ParallelLightIntensigy2;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bRoomLightIconVisible;
//};

// Mat
USTRUCT(Blueprintable)
struct FMatChannelInfo
{
public:
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString DiffuseName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString NormalName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString LightName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString LusterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MetalName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DiffuseSaturation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float NormalSaturation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LightSaturation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LusterSaturation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MetalSaturation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Selfxluminous;
};


//
//// Mat Config
//USTRUCT(Blueprintable)
//struct FMatConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UMaterialInterface *Material;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture2D *Texuture2D;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bIsHeatLoad;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString HeatLoadPath;
//};
//
//// model config
//USTRUCT(Blueprintable)
//struct FModelConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Folder;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<UMaterialInterface*> MaterialList;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bOpenPhysics;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	UTexture2D *ICO;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Size;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Detail;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Classification;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FFurnitureOtherInfoNode Other;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Path;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bCanConvertedToGeom;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString DoorMaterialDatas;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString DoorMaterialPath;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString DoorMaterialPakPath;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ModelDatas;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FOrderFurnitureStructCPP> OrderFurniture;
//};
//
//// model config other
//USTRUCT(Blueprintable)
//struct FModelConfigOther
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool BoolSlot1;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool BoolSlot2;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool BoolSlot3;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool BoolSlot4;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FloatSlot1;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FloatSlot2;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FloatSlot3;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FloatSlot4;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString StringSlot1;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString StringSlot2;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString StringSlot3;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString StringSlot4;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FTransform TransformSlot1;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FTransform TransformSlot2;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FTransform TransformSlot3;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FTransform TransformSlot4;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor ColorSlot1;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor ColorSlot2;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor ColorSlot3;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FLinearColor ColorSlot4;
//};
//
//// para light struct
//USTRUCT(Blueprintable)
//struct FParaLightStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Strength;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Angle;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Time;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float ParaLightColorR;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float ParaLightColorG;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float ParaLightColorB;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float ParaLightColorA;
//};
//
//// PictureManageItemStruct
//USTRUCT(Blueprintable)
//struct FPictureManageItemStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 FileId;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString FileName;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString FileUrl;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector2D ViewPortSize;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector2D MousePosition;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector2D ScreenShotSize;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector2D PngSize;
//};
//
////PictureManageStruct
//USTRUCT(Blueprintable)
//struct FPictureManageStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 HousingId;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 RoomId;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString RoomName;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 SolutionId;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FPictureManageItemStruct> FileList;
//};
//
//// PostDepthOfField
//USTRUCT(Blueprintable)
//struct FPostDepthOfField
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FocalDistance;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FocalRegion;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float NearTransitionRegion;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FarTransitionRegion;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Scale;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float MaxBlurSize;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float FarBlurSize;
//};
//
//// PreContent Config
//USTRUCT(Blueprintable)
//struct FPreContentConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bIsMat;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TSubclassOf<ASceneInteriorActor> ObjectBP;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FMatConfig MatConfig;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString FirstTierCategory;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString SecondaryClassification;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bThermalLoadType;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UStaticMesh *StaticMesh;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Size;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Detailed;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Path;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture *Texture;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bSliceable;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bNew;
//};
//
//// product struct
//USTRUCT(Blueprintable)
//struct FProductStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TSubclassOf<AParentProductActor> ProductClass;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture2D *ButtonTexture;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Url;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UMaterial *Material; //TODO
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		EProductType MyProductType;
//};
//
//// project configs
//USTRUCT(Blueprintable)
//struct FProjectConfigs
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Maps;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Date;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ID;
//};
//
////ProjectOtherConfig
//USTRUCT(Blueprintable)
//struct FProjectOtherConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FTransform CarmeraTransform;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bGravity;
//};
//
////QualitySetting
//USTRUCT(Blueprintable)
//struct FQualitySetting
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ViewDistanceQuality;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ResolutionQuality;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString AntiAliasingQuality;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString PostProcessQuality;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ShadowQuality;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString TextureQuality;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString EffectsQuality;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString FoliageQuality;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Accuracy;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Antialiased;
//};
//
//// S_ChildMenuConfig
//USTRUCT(Blueprintable)
//struct FChildMenuConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bSplitBar;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bPrompt;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FText Text;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FText AttchText;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bDisplayImage;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture *Image;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FName ToolTip;
//};
//
//// S_Function
//USTRUCT(Blueprintable)
//struct FFunctionStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ReturnValueType;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Parameter;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Code;
//};
//
//// S_MenuConfig
//USTRUCT(Blueprintable)
//struct FMenuConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bSplitBar;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bPrompt;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FText Text;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FText AttachText;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bDisplayImage;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture *Image;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FChildMenuConfig ChildMenu;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FName ToolTip;
//};
//
//// SaveModeDatas
//USTRUCT(Blueprintable)
//struct FBooleanICO
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString PakPath;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ModelDatas;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString ModePath;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString MaterialPath;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString DoorMaterialDatas;
//};
//
//// ScreenExpStruct
//USTRUCT(Blueprintable)
//struct FScreenExpStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float ExpoValue;
//};
//
//// SeasonTimeStruct
//USTRUCT(Blueprintable)
//struct FSeasonTimeStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 SeasonValue;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 TimeValue;
//};
//
//// SeriesStruct
//USTRUCT(Blueprintable)
//struct FSeriesStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 ID;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//};
//
////SG_Line
//USTRUCT(Blueprintable)
//struct FSGLine
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector Point;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector Direction;
//};
//
////SG_Plane
//USTRUCT(Blueprintable)
//struct FSGPlane
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector Point;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector Normal;
//};
//
//// SG_Triangle
//USTRUCT(Blueprintable)
//struct FSGTriangle
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector A;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector B;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector C;
//};
//
//// SGXStruct
//USTRUCT(Blueprintable)
//struct FSGXStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Style;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UWallLine *Mesh;
//};
//
//// SkyLightStruct
//USTRUCT(Blueprintable)
//struct FSkyLightStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Strength;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float SkyLightColorR;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float SkyLightColorG;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float SkyLightColorB;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float SkyLightColorA;
//};
//
//// StyleStruct
//USTRUCT(Blueprintable)
//struct FStyleStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 ID;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//};
//
//// T3DFile
//USTRUCT(Blueprintable)
//struct FT3DFile
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector> Vertex;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<int32> Triangles;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector> Normals;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector2D> UV;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FProcMeshTangent> Tangents;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString MaterialPath;
//};
//
//// TaskProgressItem
//USTRUCT(Blueprintable)
//struct FTaskProgressItem
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Url;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString FileName;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Guid;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString DestinationPath;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float Progress;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Status;
//};
//
//// TemplateConfig
//USTRUCT(Blueprintable)
//struct FTemplateConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UTexture *Texture;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString MapName;
//};
//
//// TJXStruct
//USTRUCT(Blueprintable)
//struct FTJXStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Style;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UWallLine *Mesh;
//};
//
//
//// Triangle2D
//USTRUCT(Blueprintable)
//struct FTriangle2D
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 A;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 B;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 C;
//};
//
//// V2D_Index
//USTRUCT(Blueprintable)
//struct FBooleanICO
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector2D Vector2D;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 Index;
//};
//
//// WallandPointStruct
//USTRUCT(Blueprintable)
//struct FWallAndPointStruct
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Wall;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector2D> Points;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FVector2D> BoolPoints;
//};
//
//// Window Config
//USTRUCT(Blueprintable)
//struct FWindowConfig
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool bIsOpen;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector2D Position;
//};
//
//// Window List
//USTRUCT(Blueprintable)
//struct FWindowList
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FWindowConfig MemberVar0;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FWindowConfig MemberVar1;
//};
