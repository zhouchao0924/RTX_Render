// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma warning(disable: 4146)

#include "VaRestJsonObject.h"
#include "HomeLayout/DataNodes/WallNode.h"
#include "gmp.h"
#include "HomeLayout/DataNodes/MaterialNode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Struct.generated.h"

USTRUCT(BlueprintType)
//MX模型依赖的材质结构体
struct FMatOfMX
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString					ResID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int					    ModelId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString					URL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int						MtrlChannelId;
};

// 模型UV记录结构体
USTRUCT(BlueprintType)
struct FUVOffset
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = Geometry)
		float	U;

	UPROPERTY(BlueprintReadWrite, Category = Geometry)
		float	V;
};

USTRUCT(BlueprintType)
struct FUVScale
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = Geometry)
		float	U;

	UPROPERTY(BlueprintReadWrite, Category = Geometry)
		float	V;
};

USTRUCT(BlueprintType)
//MX模型保存的结构体
struct FMXSaved
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor>     ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString					ResID;  

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int					    ModelId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform				Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool					HasDeleted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMatOfMX>		DependsMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUVOffset	UVOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUVScale	UVScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		UVRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ApplyShadow = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RoomClassID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CraftID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32					SkuId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString	ModelTag;
};

//TODO--用于墙体，地面，踢脚线等硬装所使用材质的SX文件存储结构体（不存储模型的材质，模型材质由FMXSaved结构体中SX的Resid关联）


// ZCBlueprintFunctionLibrary移过来的，不知道是干什么用的
USTRUCT(BlueprintType)
struct FMXFileLocation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "ZHUtil")
		FString      ResID;

	UPROPERTY(BlueprintReadOnly, Category = "ZHUtil")
		TArray<int>  ResIndexInResIDList;
};

// 用于存储CSV数据
USTRUCT(BlueprintType)
struct FCSVAtom
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = CSV)
		TArray<FString> Content;

public:

	FCSVAtom()
	{
		Content.Init(TEXT(""), 0);
	};

	FCSVAtom(int32 Number)
	{
		Content.Init(TEXT(""), Number);
	};

	void Init(int32 Number)
	{
		Content.Init(TEXT(""), Number);
	}
};

USTRUCT(BlueprintType)
struct FCSVFile
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = CSV)
		int32 Cols;

	UPROPERTY(BlueprintReadOnly, Category = CSV)
		FCSVAtom Header;

	UPROPERTY(BlueprintReadOnly, Category = CSV)
		TArray<FCSVAtom> Content;

public:

	FCSVFile()
	{
		Cols = 0;

		Header.Init(0);

		Content.Init(0, 0);
	};

	FCSVFile(int32 InCols)
	{
		Cols = InCols;

		Header.Init(Cols);

		Content.Init(Cols, 0);
	};

	void SetHeader(FCSVAtom InHeader)
	{
		if (InHeader.Content.Num() >= Cols)
		{
			for (int i = 0; i < Cols; i++)
			{
				Header.Content[i] = InHeader.Content[i];
			}
		}
		else
		{
			for (int i = 0; i < Cols; i++)
			{
				if (i < InHeader.Content.Num())
				{
					Header.Content[i] = InHeader.Content[i];
				}
				else
				{
					Header.Content[i] = TEXT("");
				}
			}
		}
	};

	void AddContent(FCSVAtom InContent)
	{
		FCSVAtom TempAtom(Cols);

		if (InContent.Content.Num() >= Cols)
		{
			for (int i = 0; i < Cols; i++)
			{
				TempAtom.Content[i] = InContent.Content[i];
			}
		}
		else
		{

			for (int i = 0; i < InContent.Content.Num(); i++)
			{
				TempAtom.Content[i] = InContent.Content[i];
			}
		}

		Content.Add(TempAtom);
	};
};

// 方案迁移页面

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Unkown		UMETA(DisplayName = "未知"),
	IT_Normal		UMETA(DisplayName = "成品"),
	IT_Customized	UMETA(Displayname = "定制"),
	IT_Gift			UMETA(DisplayName = "赠品"),
};

USTRUCT(BlueprintType)
struct FComboBoxInfo
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComboBoxInfo)
		FString	Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComboBoxInfo)
		float	Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComboBoxInfo)
		TArray<UVaRestJsonObject*>	Options;

	FComboBoxInfo();

	FComboBoxInfo(FString Inkey, float InId);

	FComboBoxInfo(FString Inkey, TArray<UVaRestJsonObject*> InOptions);

	void SetKey(FString Inkey);

	void AddValue(UVaRestJsonObject* Option);

	void CleanInfo();
};

USTRUCT(BlueprintType)
struct FBrandInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BrandInfo)
		FString	Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BrandInfo)
		int32	ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BrandInfo)
		int32	Level;

	FBrandInfo();

	FBrandInfo(FString InName, int32 InID, int32 InLevel);
};

USTRUCT(BlueprintType)
struct FCategoryInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CategoryInfo)
		FString	Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CategoryInfo)
		int32	ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CategoryInfo)
		int32	Level;

	FCategoryInfo();

	FCategoryInfo(FString InName, int32 InID, int32 InLevel);

};

USTRUCT(BlueprintType)
struct FRoomUsage
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoomUsage)
		int32	Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoomUsage)
		FString Name;

	FRoomUsage() {};

	FRoomUsage(int32 InId, FString InName);

};

USTRUCT(BlueprintType)
struct FRoomMark
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoomMark)
		int32	Mark;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoomMark)
		FString	Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoomMark)
		float	Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoomMark)
		TArray<FRoomUsage>	Usages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoomMark)
		int32	Typeid;
	FRoomMark() :Typeid(0){};

	FRoomMark(int32 InMark, FString InName, float InSize, TArray<FRoomUsage> InUsages);
};

//选配包子空间结构体
USTRUCT(BlueprintType)
struct FMXChildDesignSaved
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		int32 ParentId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		int32 RoomId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		int32 UsageId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		int32 DesignId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		int32 DesignIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		FString RoomName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		TArray<FMXSaved> MxActorList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		FString AreaTag;
};

//选配包主空间结构体
USTRUCT(BlueprintType)
struct FMXMainDesignSaved
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		int32 RoomId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		int32 UsageId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		int32 DesignId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		int32 DesignIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		FString RoomName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		TArray<FMXSaved> MxActorList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		TArray<FMXChildDesignSaved> MxDesignList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MXDesignSaved)
		FString AreaTag;
};

//全景图点位信息

USTRUCT(BlueprintType)
struct FPanoramaPoint
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Panorama)
		FString PointName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Panorama)
		TArray<FString> ConnectPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Panorama)
		FVector PointLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Panorama)
		FRotator PointRotation;
};

//全景图Room信息

USTRUCT(BlueprintType)
struct FPanormaRoom
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Panorama)
		int RoomID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Panorama)
		TArray<FPanoramaPoint> PointList;
};

//全景图方案信息

USTRUCT(BlueprintType)
struct FPanormaSolution
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Panorama)
		int SolutionID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Panorama)
		FString url;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Panorama)
		TArray<FPanormaRoom> RoomList;
};


// 权限系统 - 界面 子结构体

USTRUCT(BlueprintType)
struct FAuthorityCell
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Authority)
		FString		AuthorityID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Authority)
		bool		bVisibility;
};


// 权限系统 - 界面 分类主结构体

USTRUCT(BlueprintType)
struct FAuthority
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Authority)
		FString		CategoryName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Authority)
		bool		bVisibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Authority)
		TArray<FAuthorityCell> Child;
};


USTRUCT(BlueprintType)
struct FComputerInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		FString		CPU;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		FString		GPU;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		FString		OS;

	// 内存容量信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		FString		RAM;
};

USTRUCT(BlueprintType)
struct FModelStatus
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		int Type;//状态 1.移动 2.创建 3.删除
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		int Id;//模型Id
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		bool NoAreaModel;//是否是无区域模型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		AActor *Actor;//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		FMXSaved MxSaved;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ComputerInfo)
		FRotator rotationXYZ;
};


// Http error codes enum
UENUM(BlueprintType)
enum class EHttpCodes : uint8
{
	Unknown,
	Continue,
	SwitchProtocol,
	Ok,
	Created,
	Accepted,
	Partial,
	NoContent,
	ResetContent,
	PartialContent,
	Ambiguous,
	Moved,
	Redirect,
	RedirectMethod,
	NotModified,
	UseProxy,
	RedirectKeepVerb,
	BadRequest,
	Denied,
	PaymentReq,
	Forbidden,
	NotFound,
	BadMethod,
	NoneAcceptable,
	ProxyAuthReq,
	RequestTimeout,
	Conflict,
	Gone,
	LengthRequired,
	PrecondFailed,
	RequestTooLarge,
	UriTooLong,
	UnsupportedMedia,
	TooManyRequests,
	RetryWith,
	ServerError,
	NotSupported,
	BadGateway,
	ServiceUnavail,
	GatewayTimeout,
	VersionNotSup,
};


USTRUCT(BlueprintType)
struct FHardDesignNode
{
	GENERATED_USTRUCT_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	ModelID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	SkuID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	RoomClassID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	CraftID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	DesignID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	UsageID = -1;
};

USTRUCT(BlueprintType)
struct FModelDesignNode
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	ModelID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	SkuID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	RoomClassID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	CraftID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	DesignID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	UsageID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	MaterialModelID= -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DesignNode")
		int32	MaterialSKUID = -1;
};

// 某一面墙依赖关系
USTRUCT(BlueprintType)
struct FWallDependsArea
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		int32	RoomID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		int32	ChannelID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		FString WallGUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		FString	Acreage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		TArray<FHardDesignNode>	Designs;
};

// 某一区域依赖关系

USTRUCT(BlueprintType)
struct FAreaDepends
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		int32	RoomID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString	GroundAcreage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString	RoofAcreage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString	GroundPerimeter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString	RoofPerimeter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		TArray<FHardDesignNode>	GroundDesigns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		TArray<FHardDesignNode>	RoofDesigns;

};

//窗的依赖关系
USTRUCT(BlueprintType)
struct FWindowDependsArea
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		int32	RoomID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		FString WindowGUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString	WindowArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		TArray<FModelDesignNode> WindowDesigns;
};

//门的依赖关系
USTRUCT(BlueprintType)
struct FDoorDependsArea
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		int32	RoomID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		FString DoorGUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString	DoorArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		TArray<FModelDesignNode> DoorDesigns;
};

//门套的依赖关系
USTRUCT(BlueprintType)
struct FDoorPocketDependsArea
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		int32	RoomID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		FString DoorPocketGUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString DoorPocketLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		TArray<FModelDesignNode> DoorPocketDesigns;
};

//过门石依赖关系
USTRUCT(BlueprintType)
struct FThresholdDependsArea
{
	GENERATED_USTRUCT_BODY() 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		int32 RoomID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		FString ThresholdGUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString ThresholdLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		TArray<FHardDesignNode> ThresholdDesigns;
};

//踢脚线依赖关系
USTRUCT(BlueprintType)
struct FSkirtingDependsArea
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		int32 RoomID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallDependency")
		FString SkirtingGUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString SkirtingLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		TArray<FModelDesignNode> SkirtingDesigns;
};

//硬装模型依赖关系
USTRUCT(BlueprintType)
struct FHardRiftModelDependsArea
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		int32 RoomID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		FString Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaDependency")
		TArray<FModelDesignNode> HardRiftModelDesigns;
};


// Bigdecimal struct for multi precisions

USTRUCT(BlueprintType)
struct FBigDecimal
{
	GENERATED_USTRUCT_BODY();

public:

	mpf_t	Internal;

private:
	/*
	int32 CompareBigDecimal(const FBigDecimal& Target)
	{
		mp_exp_t SelfExp = 0;

		mp_exp_t TargetExp = 0;

		FString SelfStr = ToString();

		FString TargetStr = Target.ToString();

		bool bSelfIsPositive = SelfStr.Find(TEXT("-")) == -1 ? false : true;
		bool bTargetIsPositive = TargetStr.Find(TEXT("-")) == -1 ? false : true;

		if (bSelfIsPositive && !bTargetIsPositive)
		{
			return 1;
		}
		else if (!bSelfIsPositive && bTargetIsPositive)
		{
			return -1;
		}
		else
		{
			if (SelfExp > TargetExp)
			{
				return bSelfIsPositive ? 1 : -1;
			}
			else if (SelfExp < TargetExp)
			{
				return bSelfIsPositive ? -1 : 1;
			}		
			else
			{
				int32 CompareInt = CompareValue(SelfStr.LeftChop(SelfExp), TargetStr.LeftChop(TargetExp), bSelfIsPositive, true);

				if (CompareInt == 0)
				{
					return CompareValue(SelfStr.RightChop(SelfExp), TargetStr.RightChop(TargetExp), bSelfIsPositive, false);
				}
				else
				{
					return CompareInt;
				}
			}
		}
	}

	int32 CompareValue(const FString& FirstVal, const FString& SecondVal, bool bIsPositive, bool bIsInteger)
	{
		if (bIsInteger)
		{
			for (int32 i = 0; i < FirstVal.Len(); i++)
			{
				FString TempFirst(FString::Chr(FirstVal[i]));
				FString TempSecond(FString::Chr(SecondVal[i]));

				int32 FirstInt = FCString::Atoi(*TempFirst);
				int32 SecondInt = FCString::Atoi(*TempSecond);

				if (FirstInt > SecondInt)
				{
					return bIsPositive ? 1 : -1;
				}
				else if (FirstInt < SecondInt)
				{
					return bIsPositive ? -1 : 1;
				}
			}

			return 0;
		}
		else
		{
			bool bUseFirstLen = FirstVal.Len() < SecondVal.Len();

			int32 UsedLen = bUseFirstLen ? FirstVal.Len() : SecondVal.Len();

			for (int32 i = 0; i < UsedLen; i++)
			{
				FString TempFirst(FString::Chr(FirstVal[i]));
				FString TempSecond(FString::Chr(SecondVal[i]));

				int32 FirstFloat = FCString::Atoi(*TempFirst);
				int32 SecondFloat = FCString::Atoi(*TempSecond);
				if (FirstFloat > SecondFloat)
				{
					return bIsPositive ? 1 : -1;
				}
				else if (FirstFloat < SecondFloat)
				{
					return bIsPositive ? -1 : 1;
				}
			}

			if (FirstVal.Len() > SecondVal.Len())
			{
				return bIsPositive ? 1 : -1;
			}
			else if (FirstVal.Len() < SecondVal.Len())
			{
				return bIsPositive ? -1 : 1;
			}
			else
			{
				return 0;
			}
			
		}
	}
	*/
public:

	// Construction Function

	FBigDecimal()
	{
		mpf_init2(Internal, 1024);
	};

	~FBigDecimal()
	{
		mpf_clear(Internal);
	};

	FBigDecimal(const mpf_t& InSource)
	{
		mpf_init2(Internal, 1024);

		mpf_set(Internal, InSource);
	};

	FBigDecimal(const FBigDecimal& InSource)
	{
		mpf_init2(Internal, 1024);

		mpf_set(Internal, InSource.Internal);
	};

	FBigDecimal(const FString& InSource)
	{
		mpf_init2(Internal, 1024);

		mpf_set_str(Internal, TCHAR_TO_ANSI(*InSource), 10);
	};

	FBigDecimal(int32 InSource)
	{
		mpf_init2(Internal, 1024);

		mpf_set_si(Internal, InSource);
	};


	FBigDecimal(float InSource)
	{
		mpf_init2(Internal, 1024);

		mpf_set_d(Internal, InSource);
	};

	FString ToString() const
	{
		mp_exp_t Exp;

		FString Result(ANSI_TO_TCHAR(mpf_get_str(nullptr, &Exp, 10, 0, Internal)));

		bool bIsPositive = Result.Find(TEXT("-")) == -1;

		if (Exp == 0)
		{
			if (!bIsPositive)
			{
				Result.RemoveAt(Result.Find(TEXT("-")));
				Result.InsertAt(Exp, "-0.");
			}
			else
			{
				Result.InsertAt(Exp, TEXT("0."));
			}
		}
		else if (Exp < 0)
		{
			Exp *= -1;

			if (!bIsPositive)
			{
				Result.RemoveAt(0);
			}

			FString TempResult = bIsPositive ? "0." : "-0.";

			for (int i = 0; i < Exp; i++)
			{
				TempResult += TEXT("0");
			}

			TempResult += Result;

			Result = TempResult;
		}
		else
		{
			if (Exp > Result.Len())
			{
				int32 LoopTimes = Exp - Result.Len();

				for (int i = 0; i < LoopTimes; i++)
				{
					Result.Append(TEXT("0"));
				}
			}

			if (bIsPositive)
			{
				Result.InsertAt(Exp, TEXT("."));
			}
			else
			{
				Result.InsertAt(Exp + 1, TEXT("."));
			}
		}

		return Result;
	};

public:

	// Comparison Function

	bool operator>(const FBigDecimal& Target) const
	{
		int Status = mpf_cmp(Internal, Target.Internal);
		return Status > 0;
	};

	bool operator<(const FBigDecimal& Target) const
	{
		int Status = mpf_cmp(Internal, Target.Internal);
		return Status < 0;
	};

	bool operator==(const FBigDecimal& Target) const
	{
		int Status = mpf_cmp(Internal, Target.Internal);
		return Status == 0;
	};

	bool operator>=(const FBigDecimal& Target) const
	{
		int32 Result = mpf_cmp(Internal, Target.Internal);

		return ((Result > 0) || (Result == 0));
	};

	bool operator<=(const FBigDecimal& Target) const
	{
		int32 Result = mpf_cmp(Internal, Target.Internal);

		return ((Result < 0) || (Result == 0));
	};

public:

	// Assignment Function

	void operator=(const FBigDecimal& Target)
	{
		mpf_set(Internal, Target.Internal);
	};

	void operator=(const FString& Target)
	{
		mpf_set_str(Internal, TCHAR_TO_ANSI(*Target), 10);
	};

	void operator=(const int32 Target)
	{
		mpf_set_si(Internal, Target);
	};

	void operator=(const float Target)
	{
		mpf_set_d(Internal, Target);
	};

public:

	// Arithmetic Function

	FBigDecimal operator+(const FBigDecimal& Target)
	{
		mpf_t ResultMpf;
		mpf_init2(ResultMpf, 1024);

		mpf_add(ResultMpf, Internal, Target.Internal);

		FBigDecimal Result(ResultMpf);
		return Result;
	};

	FBigDecimal operator-(const FBigDecimal& Target)
	{
		mpf_t ResultMpf;
		mpf_init2(ResultMpf, 1024);

		mpf_sub(ResultMpf, Internal, Target.Internal);

		FBigDecimal Result(ResultMpf);

		return Result;
	};

	FBigDecimal operator*(const FBigDecimal& Target)
	{
		mpf_t ResultMpf;
		mpf_init2(ResultMpf, 1024);

		mpf_mul(ResultMpf, Internal, Target.Internal);

		FBigDecimal Result(ResultMpf);
		return Result;
	};

	FBigDecimal operator/(const FBigDecimal& Target)
	{
		FString DirtyStr;
		DirtyStr = TEXT("0");
		FBigDecimal DirtyBD(DirtyStr);

		if (Target == DirtyBD)
		{
			return DirtyBD;
		}
		else
		{
			mpf_t ResultMpf;
			mpf_init2(ResultMpf, 1024);

			mpf_div(ResultMpf, Internal, Target.Internal);

			FBigDecimal Result(ResultMpf);

			return Result;
		}
	};

	FBigDecimal& operator+=(const FBigDecimal& Target)
	{
		mpf_add(Internal, Internal, Target.Internal);
		return *this;
	};

	FBigDecimal& operator-=(const FBigDecimal& Target)
	{
		mpf_sub(Internal, Internal, Target.Internal);

		return *this;
	};

	FBigDecimal& operator*=(const FBigDecimal& Target)
	{
		mpf_mul(Internal, Internal, Target.Internal);

		return *this;
	};

	FBigDecimal& operator/=(const FBigDecimal& Target)
	{
		mpf_div(Internal, Internal, Target.Internal);

		return *this;
	};

	FBigDecimal Abs()
	{
		mpf_t ResultMpf;

		mpf_init2(ResultMpf, 1024);

		mpf_abs(ResultMpf, Internal);

		FBigDecimal Result(ResultMpf);

		return Result;
	};

	FBigDecimal Floor()
	{
		mpf_t ResultMpf;

		mpf_init2(ResultMpf, 1024);

		mpf_floor(ResultMpf, Internal);

		FBigDecimal Result(ResultMpf);

		return Result;
	};

	FBigDecimal Ceil()
	{
		mpf_t ResultMpf;

		mpf_init2(ResultMpf, 1024);

		mpf_ceil(ResultMpf, Internal);

		FBigDecimal Result(ResultMpf);

		return Result;
	};

	FBigDecimal Round()
	{
		FBigDecimal Temp(Internal);

		if (Temp > 0)
		{
			Temp += FBigDecimal::FBigDecimal((float)0.5);

			Temp.Floor();
			Floor();
		}
		else
		{
			Temp -= FBigDecimal::FBigDecimal((float)0.5);

			Temp.Ceil();
		}

		return Temp;
	};

	bool IsPositiveNumber()
	{
		int32 Result = mpf_sgn(Internal);

		return Result == 1;
	};

	bool IsNegativeNumber()
	{
		int32 Result = mpf_sgn(Internal);

		return Result == -1;
	};

	bool IsZero()
	{
		int32 Result = mpf_sgn(Internal);

		return Result == 0;
	};
};


USTRUCT(BlueprintType)
struct FMXDWSaved
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor>     ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool					HasDeleted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool					ApplyShadow = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32>			ModelIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32>			SkulIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32>			RoomClassId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32>			CraftId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString					Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString					TagName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform				Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMaterialNode>	DependsMat;
};

USTRUCT(BlueprintType)
struct FHardCraftStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HardCraft)
		int32					CraftId;//项目Id

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HardCraft)
		FString					CraftName;//项目名称

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HardCraft)
		FString					Image;	//工艺图片

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HardCraft)
		int32					baseFlag;//是不是默认工艺
};

USTRUCT(BlueprintType)
struct FHardClassStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HardCraft)
		int32					ClassId;//项目Id

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HardCraft)
		FString					ClassName;//项目名称

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HardCraft)
		TArray<FHardCraftStruct>	CraftList;
};


USTRUCT(BlueprintType)
struct FHardState
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HardCraft)
		int32					roomUsageId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HardCraft)
		TArray<FHardClassStruct>	ClassList;

};



//Area材质多空间使用
USTRUCT(BlueprintType)
struct FMaterialArrayOfDesign
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMaterialNode> MaterialNodeArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 DesignID;

};


//门窗多空间使用
USTRUCT(BlueprintType)
struct FDoorAndWindowOfDesign
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FMXDWSaved> DWSavedArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> IDs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 DesignID;
	
};

//物体隐藏属性
USTRUCT(BlueprintType)
struct FHiddenItemsInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int			modelId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int			index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString		uuid;
};

//窗户信息
USTRUCT(BlueprintType)
struct FWindowAreaData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector			WindowPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector			WindowDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D		WindowSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float			WinodwIntensity;
};

USTRUCT(BlueprintType)
struct FAutoSaveStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool			IsEnableAutoSave = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32			TimeInterval = 600;

};

USTRUCT(BlueprintType)
struct FGroupActorInfo
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString				GroupName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform			GroupActorTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString				GroupSelfTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString				CollectionId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString>		GroupChildTag;
};

USTRUCT(BlueprintType)
struct FDnaRoomSaved
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 RoomId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UVaRestJsonObject*>	ModelJsonList;
};


//toast类型：success,fail,warning,doing
UENUM(BlueprintType)
enum class	ToastType: uint8
{
	Success				UMETA(DisplayName = "sucess"),
	Fail				UMETA(DisplayName = "fail"),
	Warning				UMETA(DisplayName = "warning"),
	Doing				UMETA(DisplayName = "doing"),
	Fixed				UMETA(DisplayName = "Fixed"),//仅用于新建方案未保存时
	FixedWarning		UMETA(DisplayName = "FixedWarning"),//常驻型的警告类型
	FixedError			UMETA(DisplayName = "FixedError"),//常驻型的错误类型
};

USTRUCT(BlueprintType)
struct FCustomCurtainSaved
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 TemplateID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 GroupID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DistanceBetweenBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<int32, int32> ModelRelation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ModelTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RomebarNumber;
};

//模型下载加载类型
UENUM(BlueprintType)
enum class	CategoryType : uint8
{
	NormalModel			UMETA(DisplayName = "NormalModel"), //普通模型
	CustomCurtain		UMETA(DisplayName = "CustomCurtain"),//定制窗帘
	CustomCeiling		UMETA(DisplayName = "CustomCeiling"),//定制吊顶
};

// The view type for auto screenshot.
UENUM(BlueprintType)
enum class EScreenshotType : uint8
{
	SST_Top			UMETA(DisplayName = "Top"),
	SST_Bottom		UMETA(DisplayName = "Bottom"),
	//Forward degrees
	SST_Front		UMETA(DisplayName = "Front"),
	//Forward around z axis -45°
	SST_FrontSecond	UMETA(DisplayName = "Front_45"),
	//Forward around z axis 45°
	SST_FrontThird	UMETA(DisplayName = "Front_-45"),
	SST_Back		UMETA(DisplayName = "Back"),
	SST_Left		UMETA(DisplayName = "Left"),
	SST_Right		UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class ECameraAngleType :uint8
{
	CAT_FlatView		UMETA(DisplayName = "FlatView"),
	CAT_UpwardView		UMETA(DisplayName = "UpwardView"),
	CAT_VerticalView	UMETA(DisplayName = "VerticalView")
};


USTRUCT(BlueprintType)
struct FLampsSaved
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 IESProfileID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Distance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor PointLightColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 PointLightIntensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 PointLightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor SpotLightColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SpotLightIntensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SpotLightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SpotLightOuterConeAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SpotLightTemperature;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsOpenLight = true;
};

USTRUCT(BlueprintType)
struct FSize
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float		Length;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float		Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float		Height;

public:
	bool Equals(const FSize& S, float Tolerance = KINDA_SMALL_NUMBER) const;
};

FORCEINLINE bool FSize::Equals(const FSize& S, float Tolerance) const
{
	return FMath::Abs(Length - S.Length) <= Tolerance && FMath::Abs(Width - S.Width) <= Tolerance && FMath::Abs(Height - S.Height) <= Tolerance;
}

USTRUCT(BlueprintType)
struct FBOMInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32		ModelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32		index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSize		Size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool		Lofting;
};

USTRUCT(BlueprintType)
struct FLineInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32				LineIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FBOMInfo>	BOMList;
};

UENUM(BlueprintType)
enum class CustomCeilingType : uint8
{
	DrywallCeiling,
	PlanCeiling,
	InvertedCeiling,
	ModelingCeiling
};

USTRUCT(BlueprintType)
struct FPolygonInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
		TArray<FVector2D> RegionalPoints;
	UPROPERTY(BlueprintReadOnly)
		TArray<FString> AreaTags;
};

USTRUCT(BlueprintType)
struct FCeilingInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					TemplateID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					GroupID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D>		Points;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FBOMInfo>		BOMList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FLineInfo>		Lines;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32>			CuttingLineIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D				PlanUVOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float					LiftHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString					DependRoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString					CeilingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		CustomCeilingType	   CeilingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString>			 AreaTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					CategoryId;
};

USTRUCT(BlueprintType)
struct FMouldingCeilingInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					TemplateID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					GroupID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					ModelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform				Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString					DependRoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32>			CuttingLineIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector2D>		Points;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString>			AreaTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString					CeilingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32					CategoryId;
};

USTRUCT(BlueprintType)
struct FCeilingLightGroup
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ModelID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLampsSaved LightSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTransform> LightTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CeilingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Lineindex;
};


// Types of template
UENUM(BlueprintType)
enum class ETemplateType : uint8
{
	CurtainTemplate			UMETA(DisplayName = "CurtainTemplate"),
	CeilingTemplate			UMETA(DisplayName = "CeilingTemplate"),
	WholeHouseTemplate		UMETA(DisplayName = "WholeHouseTemplate"),
};

// Types of template
UENUM(BlueprintType)
enum class ViewType : uint8
{
	ThreeView,
	TopView,
	bottomView,
};

USTRUCT(BlueprintType)
struct FIntArrayStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32>	IntArray;
	
};

USTRUCT(BlueprintType)
struct FVectorArrayStruct
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector>	VectorArray;

};

USTRUCT(BlueprintType)
struct FVector2DArrayStruct
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector2D>	Vector2DArray;

};

USTRUCT(BlueprintType)
struct FUserWidgetArrayStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UUserWidget*>	WidgetArray;

};

USTRUCT(BlueprintType)
struct FLineSegment
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector StartPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector EndPos;

public:

	FLineSegment() 
	{
		StartPos = FVector::ZeroVector;
		EndPos = FVector::ZeroVector;
	}

	FLineSegment(const FVector& InStart, const FVector& InEnd)
		: StartPos(InStart), EndPos(InEnd)
	{

	}

	FLineSegment(const FVector2D& InStart, const FVector2D& InEnd)
	{
		StartPos = FVector(InStart.X, InStart.Y, 0);
		EndPos = FVector(InEnd.X, InEnd.Y, 0);
	}

	FLineSegment(const FLineSegment& Other)
	{
		StartPos = Other.StartPos;
		EndPos = Other.EndPos;
	}

	void Init(const FVector& InStart, const FVector& InEnd)
	{
		StartPos = InStart;
		EndPos = InEnd;
	}

	void Init(const FVector2D& InStart, const FVector2D& InEnd)
	{
		StartPos = FVector(InStart.X, InStart.Y, 0);
		EndPos = FVector(InEnd.X, InEnd.Y, 0);
	}

	void Init(const FLineSegment& Other)
	{
		StartPos = Other.StartPos;
		EndPos = Other.EndPos;
	}

public:

	float Length() const
	{
		return (EndPos - StartPos).Size();
	}

	FVector GetDirection() const
	{
		FVector Direction = EndPos - StartPos;

		Direction.Normalize();

		return Direction;
	}

	FVector Mid() const
	{
		return (StartPos + EndPos) / 2;
	}

	bool CheckAngleTolerance(const FLineSegment& Other, float Tolerance) const
	{
		FVector SelfDirection = GetDirection();
		FVector OtherDirection = Other.GetDirection();

		float RealTolerance = UKismetMathLibrary::DegAcos(FVector::DotProduct(SelfDirection, OtherDirection) / (SelfDirection.Size() * OtherDirection.Size()));

		return RealTolerance <= Tolerance || (180 - RealTolerance) <= Tolerance;
	}
	
public:

	bool operator==(const FLineSegment& Other) const
	{
		return StartPos == Other.StartPos && EndPos == Other.EndPos;
	}

	bool operator>(const FLineSegment& Other) const
	{
		return Length() > Other.Length();
	}

	bool operator<(const FLineSegment& Other) const
	{
		return Length() < Other.Length();
	}

	bool operator>=(const FLineSegment& Other) const
	{
		return Length() > Other.Length() || (StartPos == Other.StartPos && EndPos == Other.EndPos);
	}

	bool operator<=(const FLineSegment& Other) const
	{
		return Length() < Other.Length() || (StartPos == Other.StartPos && EndPos == Other.EndPos);
	}

	FLineSegment& operator=(const FLineSegment& Other)
	{
		StartPos = Other.StartPos;
		EndPos = Other.EndPos;

		return *this;
	}
};

UENUM(BlueprintType)
enum class ECategoryFilter : uint8
{
	CF_Normal	UMETA(DisplayName = "Normal"),
	CF_Global	UMETA(DisplayName = "Global"),
	CF_All		UMETA(DisplayName = "All")
};

UENUM(BlueprintType)
enum class EQualityType : uint8
{
	Auto = 0,
	Low,
	Medium,
	High,
	Extreme
};

UENUM(BlueprintType)
enum class ArrowDirection : uint8
{
	Auto = 0,
	Up,
	Down,
	Left,
	Right,
};