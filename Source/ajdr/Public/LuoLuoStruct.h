// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once
#include "Struct.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Components/SkyLightComponent.h"
#include "Internationalization/Regex.h"
#include "Modules/ModuleManager.h"
#include "LuoLuoStruct.generated.h"

UENUM(BlueprintType)
enum class CuttingErrorType : uint8
{
	Correct,
	PointNotOnSegment,
	PointDoincident,
	SegmentCross,
	SegmentOverlap,
	ShortOfPoint,
};

UCLASS()
class AJDR_API ULuoLuoBlueprintFunctionLibrary :public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//--------------------------------FComboBoxInfo Operation---------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static void SetKey(UPARAM(ref)FComboBoxInfo& Target, FString InKey);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static void AddValue(UPARAM(ref)FComboBoxInfo& Target, UVaRestJsonObject* Option);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static FString GetKey(UPARAM(ref)FComboBoxInfo& Target);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static UVaRestJsonObject* GetOption(UPARAM(ref)FComboBoxInfo& Target, int Index);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static TArray<UVaRestJsonObject*> GetOptions(UPARAM(Ref)FComboBoxInfo& Target);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static void CleanInfo(UPARAM(ref)FComboBoxInfo& Target);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static void Contains(UPARAM(ref)TArray<FComboBoxInfo>& Target, FString Key, int& Index, FComboBoxInfo& Item, bool& bFound);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static UVaRestJsonObject* FindStringField(UPARAM(ref)TArray<UVaRestJsonObject*>& Target, FString Key, FString Value, int& Index, bool& bFound);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static UVaRestJsonObject* FindNumberField(UPARAM(ref)TArray<UVaRestJsonObject*>& Target, FString Key, float Value, int& Index, bool& bFound);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static UVaRestJsonObject* FindBoolField(UPARAM(ref)TArray<UVaRestJsonObject*>& Target, FString Key, bool Value, int& Index, bool& bFound);

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static UVaRestJsonObject* FindObjectField(UPARAM(ref)TArray<UVaRestJsonObject*>& Target, FString Key, UVaRestJsonObject* Value, int& Index, bool& bFound);

	UFUNCTION(BlueprintPure, Category = LuoLuoLibrary)
		static bool IsEmpty(FString Target) { return Target.IsEmpty(); };

	UFUNCTION(BlueprintCallable, Category = ComboBoxInfo)
		static void SetId(UPARAM(ref)FComboBoxInfo& Target, float InId) { Target.Id = InId; };

	UFUNCTION(BlueprintPure, Category = ComboBoxInfo)
		static void GetIdWithName(UPARAM(ref)TArray<FComboBoxInfo>& Target, FString InKey, float& Id, bool& bFound);

	UFUNCTION(BlueprintPure, Category = ComboBoxInfo)
		static void GetNameWithId(UPARAM(ref)TArray<FComboBoxInfo>& Target, float InId, FString& Name, bool& bFound);

	//--------------------------------FBrandInfo Operation---------------------------------------------------------

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetID"), Category = BrandInfo)
		static int32 GetID_BrandInfo(UPARAM(ref)FBrandInfo& Target) { return Target.ID; };

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetName"), Category = BrandInfo)
		static FString GetName_BrandInfo(UPARAM(ref)FBrandInfo& Target) { return Target.Name; };

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetLevel"), Category = BrandInfo)
		static int32 GetLevel_BrandInfo(UPARAM(ref)FBrandInfo& Target) { return Target.Level; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetName"), Category = BrandInfo)
		static void SetName_BrandInfo(UPARAM(ref)FBrandInfo& Target, const FString& InName) { Target.Name = InName; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetID"), Category = BrandInfo)
		static void SetID_BrandInfo(UPARAM(ref)FBrandInfo& Target, int32 InID) { Target.ID = InID; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetLevel"), Category = BrandInfo)
		static void SetLevel_BrandInfo(UPARAM(ref)FBrandInfo& Target, int32 InLevel) { Target.Level = InLevel; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetMember"), Category = BrandInfo)
		static void SetMember_BrandInfo(UPARAM(ref)FBrandInfo& Target, const FString& InName, int32 InID, int32 InLevel);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Search"), Category = BrandInfo)
		static void GetIDWithName_BrandInfo(UPARAM(ref)TArray<FBrandInfo>& Target, const FString& Name, FBrandInfo& Item, bool& bFound);

	//--------------------------------FCategoryInfo Operation---------------------------------------------------------

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetID"), Category = CategoryInfo)
		static int32 GetID_CategoryInfo(UPARAM(ref)FCategoryInfo& Target) { return Target.ID; };

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetName"), Category = CategoryInfo)
		static FString GetName_CategoryInfo(UPARAM(ref)FCategoryInfo& Target) { return Target.Name; };

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetLevel"), Category = CategoryInfo)
		static int32 GetLevel_CategoryInfo(UPARAM(ref)FCategoryInfo& Target) { return Target.Level; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetName"), Category = CategoryInfo)
		static void SetName_CategoryInfo(UPARAM(ref)FCategoryInfo& Target, const FString& InName) { Target.Name = InName; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetID"), Category = CategoryInfo)
		static void SetID_CategoryInfo(UPARAM(ref)FCategoryInfo& Target, int32 InID) { Target.ID = InID; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetLevel"), Category = CategoryInfo)
		static void SetLevel_CategoryInfo(UPARAM(ref)FCategoryInfo& Target, int32 InLevel) { Target.Level = InLevel; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetMember"), Category = CategoryInfo)
		static void SetMember_CategoryInfo(UPARAM(ref)FCategoryInfo& Target, const FString& InName, int32 InID, int32 InLevel);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Search"), Category = CategoryInfo)
		static void GetIDWithName_CategoryInfo(UPARAM(ref)TArray<FCategoryInfo>& Target, const FString& Name, FCategoryInfo& Item, bool& bFound);

	//--------------------------------FRoomUsage Operation---------------------------------------------------------

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetID"), Category = RoomUsage)
		static int32 GetID_RoomUsage(UPARAM(ref)FRoomUsage& Target) { return Target.Id; };

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetName"), Category = RoomUsage)
		static FString GetName_RoomUsage(UPARAM(ref)FRoomUsage& Target) { return Target.Name; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetID"), Category = RoomUsage)
		static void SetID_RoomUsage(UPARAM(ref)FRoomUsage& Target, int32 InId) { Target.Id = InId; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetName"), Category = RoomUsage)
		static void SetName_RoomUsage(UPARAM(ref)FRoomUsage& Target, FString InName) { Target.Name = InName; };


	//--------------------------------FRoomMark Operation---------------------------------------------------------

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetID"), Category = RoomMark)
		static int32 GetID_RoomMark(UPARAM(ref)FRoomMark& Target) { return Target.Mark; };

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetSize"), Category = RoomMark)
		static float GetSize_RoomMark(UPARAM(ref)FRoomMark& Target) { return Target.Size; };

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetName"), Category = RoomMark)
		static FString GetName_RoomMark(UPARAM(ref)FRoomMark& Target) { return Target.Name; };

	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetUsages"), Category = RoomMark)
		static TArray<FRoomUsage> GetUsages_RoomMark(UPARAM(ref)FRoomMark& Target) { return Target.Usages; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetID"), Category = RoomMark)
		static void SetID_RoomMark(UPARAM(ref)FRoomMark& Target, int32 InId) { Target.Mark = InId; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetSize"), Category = RoomMark)
		static void SetSize_RoomMark(UPARAM(ref)FRoomMark&Target, float InSize) { Target.Size = InSize; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetName"), Category = RoomMark)
		static void SetName_RoomMark(UPARAM(ref)FRoomMark& Target, FString InName) { Target.Name = InName; };

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetUseages"), Category = RoomMark)
		static void SetUsages_RoomMark(UPARAM(ref)FRoomMark& Target, TArray<FRoomUsage> InUsages) { Target.Usages = InUsages; };

	//--------------------------------Float Precision----------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Float To String"), Category = FloatPrecision)
		static FORCEINLINE FString GetFloatAsStringWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
	{
		float Rounded = roundf(TheFloat);
		if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10, -1 * Precision))
		{
			TheFloat = Rounded;
		}
		FNumberFormattingOptions NumberFormat;
		NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
		NumberFormat.MaximumIntegralDigits = 10000;
		NumberFormat.MinimumFractionalDigits = Precision;
		NumberFormat.MaximumFractionalDigits = Precision;
		return FText::AsNumber(TheFloat, &NumberFormat).ToString();
	};

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Float To Text"), Category = FloatPrecision)
		static FORCEINLINE FText GetFloatAsTextWithPrecision(float TheFloat, int32 Precision, bool IncludeLeadingZero = true)
	{
		float Rounded = roundf(TheFloat);
		if (FMath::Abs(TheFloat - Rounded) < FMath::Pow(10, -1 * Precision))
		{
			TheFloat = Rounded;
		}
		FNumberFormattingOptions NumberFormat;
		NumberFormat.MinimumIntegralDigits = (IncludeLeadingZero) ? 1 : 0;
		NumberFormat.MaximumIntegralDigits = 10000;
		NumberFormat.MinimumFractionalDigits = Precision;
		NumberFormat.MaximumFractionalDigits = Precision;
		return FText::AsNumber(TheFloat, &NumberFormat);
	};

	UFUNCTION(BlueprintPure, meta = (DisplayName = "String To Float"), Category = FloatPrecision)
		static float GetFloatFromStringWithClean(FString InStr, FString StrToRemove);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Float String Operation"), Category = FloatPrecision)
		static float FloatStringOperation(TArray<FString> Values, int32 Precision, bool bAdd);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Price Ceil"), Category = Price)
		static int32 PriceCeil_Int(int32 Price);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Price Ceil"), Category = Price)
		static int32 PriceCeil_Float(float Price);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Double String Operation"), Category = DoublePrecision)
		static FString DoubleStringOperation(TArray<FString>Values, int32 Precision, bool bAdd);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Clipboard Copy"), Category = Clipboard)
		static void ClipboardCopy(FString Message);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Clipboard Paste"), Category = Clipboard)
		static FString ClipboardPaste();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetFilePath"), Category = FilePath)
		static FString	GetFilePathWithExtension(FString FilePath, FString Extension);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Emulate Input For Ansel"), Category = Ansel)
		static void EmulateInputForAnsel();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Scan Directory"), Category = Directory)
		static TArray<FString> ScanDirectory(FString FilePath, FString Extension);

	//--------------------------------CSV File Operation---------------------------------------

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create CSV"), Category = CSV)
		static FCSVFile CreateCSVHandle(int32 Cols);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Header"), Category = CSV)
		static void SetCSVFileHeader(UPARAM(ref)FCSVFile& Handle, FCSVAtom Header);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Content"), Category = CSV)
		static void AddCSVFileContent(UPARAM(ref)FCSVFile& Handle, FCSVAtom Content);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Header"), Category = CSV)
		static FCSVAtom GetHeaderWithHandle(UPARAM(ref)FCSVFile& Handle);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Save CSV File"), Category = CSV)
		static void SaveCSVFile(UPARAM(ref)FCSVFile& Handle, FString Path);

	//-----------------------------------Snap Actor To Target------------------------------------
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Adsorption"), Category = Adsorption)
		static void SnapActorToTarget(AActor* Target, bool bSnapToFloor);

	//-------------------------------------Get Texture From Viewport-----------------------------
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Viewport Texture"), Category = Viewport)
		static UTexture2D* GetViewportTextureData();

	//-------------------------------------Get Computer Information------------------------------
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Computer Information"), Category = ComputerInfo)
		static FComputerInfo GetComputerInformation();

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Mac Address"), Category = ComputerInfo)
		static FString GetComputerMacAddress();

	//-------------------------------------Conver http error code to enum------------------------
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Convert Http Code"), Category = Http)
		static EHttpCodes ConvertToEnum(int32 ErrorCode);

	//-------------------------------------FWallDependsArea Interface----------------------------

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Set RoomID"), Category = "Utilities|WallDepends")
		static FWallDependsArea SetRoomIDWithStruct(UPARAM(ref)FWallDependsArea& Target, int32 NewRoomID);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Set ChannelID"), Category = "Utilities|WallDepends")
		static FWallDependsArea SetChannelIDWithStruct(UPARAM(ref)FWallDependsArea& Target, int32 NewChannelID);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Set GUID"), Category = "Utilities|WallDepends")
		static FWallDependsArea SetGUIDWithStruct(UPARAM(ref)FWallDependsArea& Target, FString NewGUID);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Set Acreage"), Category = "Utilities|WallDepends")
		static FWallDependsArea SetAcreageWithStruct(UPARAM(ref)FWallDependsArea& Target, FString NewAcreage);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Set Designs"), Category = "Utilities|WallDepends")
		static FWallDependsArea SetDesignsWithStruct(UPARAM(ref)FWallDependsArea& Target, TArray<FHardDesignNode> NewDesigns);

	// If the specified usage id is exsits, will set the element of array, if not will add the element to the array.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Set Design With UsageID"), Category = "Utilities|WallDepends")
		static bool SetDesignWithStruct(UPARAM(ref)FWallDependsArea& Target, FWallDependsArea& Result, FHardDesignNode NewDesign);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Design（UsageID）"), Category = "Utilities|WallDepends")
		static bool GetDesignWithUsageID(UPARAM(ref)FWallDependsArea& Target, int32 UsageId, FHardDesignNode& Result);

	//-----------------------------------------FBigDecimal Interface----------------------------------------------------------------

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct BigDecimal（BigDecimal）"), Category = "Utilities|BigDecimal")
		static FBigDecimal ConstructBigDecimalWithBigDecimal(const FBigDecimal& InVal);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct BigDecimal（String）"), Category = "Utilities|BigDecimal")
		static FBigDecimal ConstructBigDecimalWithString(const FString& InVal);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct BigDecimal（Int）"), Category = "Utilities|BigDecimal")
		static FBigDecimal ConstructBigDecimalWithInt(const int32 InVal);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Construct BigDecimal（Float）"), Category = "Utilities|BigDecimal")
		static FBigDecimal ConstructBigDecimalWithFloat(const float InVal);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Assignment（BigDecimal）", CompactNodeTitle = " = "), Category = "Utilities|BigDecimal")
		static FBigDecimal& AssignmentWithBigDecimal(UPARAM(ref)FBigDecimal& Target, const FBigDecimal& InVal);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Assignment（String）", CompactNodeTitle = " = "), Category = "Utilities|BigDecimal")
		static FBigDecimal& AssignmentWithString(UPARAM(ref)FBigDecimal& Target, const FString& InVal);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Assignment（Int）", CompactNodeTitle = " = "), Category = "Utilities|BigDecimal")
		static FBigDecimal& AssignmentWithInt(UPARAM(ref)FBigDecimal& Target, const int32 InVal);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Assignment（Float）", CompactNodeTitle = " = "), Category = "Utilities|BigDecimal")
		static FBigDecimal& AssignmentWithFloat(UPARAM(ref)FBigDecimal& Target, const float InVal);

	// Default precision is two bits.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|BigDecimal")
		static FString ConvertToString(FBigDecimal InVal);

	// Convert to string with the given settings of precision.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString（Precision Control）", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|BigDecimal")
		static FString ConverToStringWithPrecisionControl(FBigDecimal InVal, int32 Precision, bool bRound);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " + ", CompactNodeTitle = " + ", Keywords = "+ Add", CommutativeAssociativeBinaryOperator = "true"), Category = "Utilities|BigDecimal")
		static FBigDecimal Operation_Add(FBigDecimal First, FBigDecimal Second);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " - ", CompactNodeTitle = " - ", Keywords = "- Minus", CommutativeAssociativeBinaryOperator = "true"), Category = "Utilities|BigDecimal")
		static FBigDecimal Operation_Minus(FBigDecimal First, FBigDecimal Second);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " * ", CompactNodeTitle = " * ", Keywords = "* Multiply", CommutativeAssociativeBinaryOperator = "true"), Category = "Utilities|BigDecimal")
		static FBigDecimal Operation_Multiply(FBigDecimal First, FBigDecimal Second);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " / ", CompactNodeTitle = " / ", Keywords = "/ Divided"), Category = "Utilities|BigDecimal")
		static FBigDecimal Operation_Divided(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " > ", CompactNodeTitle = " > ", Keywords = "> Bigger"), Category = "Utilities|BigDecimal")
		static bool Operation_Bigger(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " < ", CompactNodeTitle = " < ", Keywords = "< Less"), Category = "Utilities|BigDecimal")
		static bool Operation_Less(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " == ", CompactNodeTitle = " == ", Keywords = "== Equal"), Category = "Utilities|BigDecimal")
		static bool Operation_Equal(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " >= ", CompactNodeTitle = " >= ", Keywords = ">= Bigger or Equal"), Category = "Utilities|BigDecimal")
		static bool Operation_BiggerOrEqual(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " <= ", CompactNodeTitle = " <= ", Keywords = "<= Less or Equal"), Category = "Utilities|BigDecimal")
		static bool Operation_LessOrEqual(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " Abs ", CompactNodeTitle = " Abs ", Keywords = "Abs"), Category = "Utilities|BigDecimal")
		static FBigDecimal Operation_Abs(UPARAM(ref)FBigDecimal& Target);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " Floor ", CompactNodeTitle = " Floor ", Keywords = "Floor"), Category = "Utilities|BigDecimal")
		static FBigDecimal Operation_Floor(UPARAM(ref)FBigDecimal& Target);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " Ceil ", CompactNodeTitle = " Ceil ", Keywords = "Ceil"), Category = "Utilities|BigDecimal")
		static FBigDecimal Operation_Ceil(UPARAM(ref)FBigDecimal& Target);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " Round ", CompactNodeTitle = " Round ", Keywords = "Round"), Category = "Utilities|BigDecimal")
		static FBigDecimal Operation_Round(UPARAM(ref)FBigDecimal& Target);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " Is Positive Number ", CompactNodeTitle = " Is Positive Number ", Keywords = "Is Positive Number"), Category = "Utilities|BigDecimal")
		static bool Operation_IsPositiveNumber(UPARAM(ref)FBigDecimal& Target);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " Is Negative Number ", CompactNodeTitle = " Is Negative Number ", Keywords = "Is Negative Number"), Category = "Utilities|BigDecimal")
		static bool Operation_IsNegativeNumber(UPARAM(ref)FBigDecimal& Target);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " Is Zero ", CompactNodeTitle = " Is Zero ", Keywords = "Is Zero"), Category = "Utilities|BigDecimal")
		static bool Operation_IsZero(UPARAM(ref)FBigDecimal& Target);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " DEC Ceil", CompactNodeTitle = " DEC Ceil", Keywords = "Ceil"), Category = "Utilities|BigDecimal")
		static FBigDecimal Operation_DECCeil(UPARAM(ref)FBigDecimal& Target);

	//--------------------------------Normal functions---------------------------------------------------------

	UFUNCTION(BlueprintPure, Category = LuoLuobrary)
		static bool CheckPrecision(const FString& InVal, int32 LimitPrecision = 0);

	UFUNCTION(BlueprintCallable, Category = LuoLuobrary)
		static void SetSkyLightAngle(USkyLightComponent *Skylight, float angle);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " RegularExpression"), Category = "LuoLuobrary")
		static bool MyRegularExpression(FString str, FString reg);

	UFUNCTION(BlueprintCallable, Category = "LuoLuobrary")
		static void FlushPressedKeys();

	UFUNCTION(BlueprintPure, meta = (DisplayName = " > ", CompactNodeTitle = " > ", Keywords = "> Bigger"), Category = "Utilities|String")
		static bool StrOperation_Bigger(FString A, FString B);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " < ", CompactNodeTitle = " < ", Keywords = "< Less"), Category = "Utilities|String")
		static bool StrOperation_Less(FString A, FString B);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " >= ", CompactNodeTitle = " >= ", Keywords = ">= Bigger or Equal"), Category = "Utilities|String")
		static bool StrOperation_BiggerOrEqual(FString A, FString B);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " <= ", CompactNodeTitle = " <= ", Keywords = "<= Less or Equal"), Category = "Utilities|String")
		static bool StrOperation_LessOrEqual(FString A, FString B);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "UnixTimestamp"), Category = "Utilities|TimeStamp")
		static FString GetUnixTimestamp(int32 TimeZone = 0);

	UFUNCTION(BlueprintPure, Category = "Utilities|Path")
		static FString GetDesktopPath();

	UFUNCTION(BlueprintPure, Category = "Utils|File")
		static FString GetFileSize(FString FilePath);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " · ", CompactNodeTitle = " · ", Keywords = "Convert to ECameraAngleType"), Category = "Utils|AutoScreenshot")
		static ECameraAngleType ConvertToCameraAngleType(int32 Source);

	UFUNCTION(BlueprintCallable, Category = "Utils|Platform")
		static void ExploreFolder(FString FilePath);

	UFUNCTION(BlueprintCallable, Category = "Utile|File")
		static bool GetFileMD5(FString FilePath, FString& MD5);

	UFUNCTION(BlueprintCallable, Category = "Utile|File")
		static bool CheckFileMD5(FString FilePath, FString InMD5, FString& MD5);

	//--------------------------------------------Viewport functions---------------------------------------------------------

	// 这个函数暂时有点问题，逻辑可以借鉴但是不要直接用，正交部分有点问题。
	UFUNCTION(BlueprintCallable, Category = "Utils|Viewport")
		static void FocusCameraToSelection(class APlayerController* Player, class AActor* Target, float Factor = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Utils|Viewport")
		static void FocusSceneCaptureToSelection(class USceneCaptureComponent2D* Camera, class AActor* Target, float Factor = 1.0f);

	UFUNCTION(BlueprintPure, meta = (DisplayName = " · ", CompactNodeTitle = " · ", Keywords = "To Float"), Category = "Utilities|Viewport")
		static float ConvertScreenshotTypeToFloat(EScreenshotType Type);

	UFUNCTION(BlueprintCallable, Category = "GC")
		static void ForceGC();

	UFUNCTION(BlueprintCallable, Category = "Geometry")
		static bool IsCuttingLegal(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints, CuttingErrorType& Errortype);

	UFUNCTION(BlueprintCallable, Category = "Geometry")
		static bool AreaSegment(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints, TArray<int32> PreviousCuttingLineIndex, TArray<FVector2D> &OutArea_1, TArray<FVector2D> &OutArea_2, TArray<int32>& Area1_CuttingLine, TArray<int32>& Area2_CuttingLine,CuttingErrorType& Errortype);

	UFUNCTION(BlueprintCallable, Category = "Geometry")
		static bool IsPointOnAreaSegment(TArray<FVector2D> AreaPoints, FVector2D Point);

	UFUNCTION(BlueprintCallable, Category = "Geomerty")
		static TArray<FVector2D> GetCuttingPoints(TArray<FVector2D> InArea_1, TArray<FVector2D> InArea_2);

	UFUNCTION(BlueprintPure, Category = "Geomerty")
		static void DeletePointOnSeg(TArray<FVector2D> InPointList, TArray<FVector2D> &OutPointList);

	UFUNCTION(BlueprintCallable, Category = "Geometry")
		static void CalculatePolygonIntersectant(TArray<FVector2D> FirstArea, TArray<FVector2D>SecondArea, float &IntersectantArea);
	
	UFUNCTION(BlueprintCallable, Category = "Geometry")
		static bool AreaCombine(TArray<FVector2D> FirstArea, TArray<FVector2D>SecondArea, TArray<FVector2D> &CombineArea);

	UFUNCTION(BlueprintCallable, Category = "Geometry")
		static bool CheckCuttingLine(const TArray<FVector2D>& InCurrentPoints, const TArray<FVector2D>& InComparisonPoints, const TArray<FVector2D>& InOriginalPoints, TArray<int32> &OutCuttingLine);

	UFUNCTION(BlueprintCallable, Category = "Geometry")
		static bool CheckPolygonCanCombine(TArray<FVector2D> FirstPolygon, TArray<FVector2D> SecondPolygon);

	//--------------------------------------------Caculation for custom ceil model.---------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Math")
		static TArray<FVector> SplitLineWithSpace(const FVector& StartPos, const FVector& EndPos, const TArray<FVector2D>& AreaPoints, float Space, float InvalidSpace, int32 LimitNum = 0, bool IsFlatLight = false);

	UFUNCTION(BlueprintCallable, Category = "Math")
		static int32 GetLightLengthFromSpace(const FVector& StartPos, const FVector& EndPos, float Space, float InvalidSpace, int32 LimitNum = 0);

	UFUNCTION(BlueprintPure, Category = "Math")
		static void CaculateMaxiumLine(const TArray<FVector>& AreaPoints, FVector& StartPos, FVector& EndPos);

	UFUNCTION(BlueprintPure, Category = "Math")
		static void CaculateMinimumLine(const TArray<FVector>& AreaPoints, FVector& StartPos, FVector& EndPos);

	UFUNCTION(BlueprintPure, Category = "Math")
		static void CaculateMaxiumLineSegment(const TArray<FLineSegment>& AreaLineSegments, FLineSegment& Outer);

	UFUNCTION(BlueprintPure, Category = "Math")
		static void CaculateMinimumLineSegment(const TArray<FLineSegment>& AreaLineSegments, FLineSegment& Outer);

	UFUNCTION(BlueprintPure, Category = "Math")
		static TArray<FLineSegment> MakeSegmentsFromPoints(const TArray<FVector>& AreaPoints);

	UFUNCTION(BlueprintPure, Category = "Math")
		static TArray<FVector2D> ConvertVectorArrayToVector2D(const TArray<FVector>& Points);

	UFUNCTION(BlueprintPure, Category = "Math")
		static TArray<FVector> Get3DPoints(const TArray<FLineSegment>& Segments);

	UFUNCTION(BlueprintPure, Category = "Math")
		static TArray<FVector2D> Get2DPoints(const TArray<FLineSegment>& Segments);

	UFUNCTION(BlueprintPure, Category = "Math")
		static bool CheckSegmentAngleTolerance(const FLineSegment& Origin, const FLineSegment& Target, float Tolerance);

	UFUNCTION(BlueprintPure, Category = "Math")
		static TArray<FLineSegment> CleanLineSegmentsByRule(const TArray<FLineSegment>& AreaLineSegments, float Tolerance);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Length"),Category = "LineSegment")
		static float GetLineSegmentLength(const FLineSegment& Target);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Direction"), Category = "LineSegment")
		static FVector GetLineSegmentDirection(const FLineSegment& Target);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Mid"), Category = "LineSegment")
		static FVector GetLineSegmentMidLocation(const FLineSegment& Target);

private:
	static CuttingErrorType IsPointOnAreaLine(TArray<FVector2D> AreaPoints, TArray<FVector2D>& CuttingPoints);

	static CuttingErrorType IsSegmentIntersectionLegal(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints);

	static CuttingErrorType IsCuttingLineLegal(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints);

	static void DeleteCoincidentPoint(TArray<FVector2D> & PointList);

	static TArray<int32> GetCuttingLineIndex(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints, TArray<FLineSegment> PreviousCuttingLines, bool CheckCuttingPoints = true);

	static void ShortenSegment(FVector2D &StartPoint, FVector2D &EndPoint, float ShortenLen);

	static bool CheckPointAtLine(const FVector& InCurrentPoint, const TArray<FVector2D>& InOriginalPoints);

	static bool CheckTwoPointAtCommonLine(const FVector& InCurrentPoint, const FVector& InNextPoints, const TArray<FVector2D>& InOriginalPoints);

	static bool LineCoincide(FVector FirstStart, FVector FirstEnd, FVector SecondStart, FVector SecondEnd);
};