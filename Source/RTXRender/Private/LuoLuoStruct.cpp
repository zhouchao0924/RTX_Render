// Copyright© 2017 ihomefnt All Rights Reserved.

#include "LuoLuoStruct.h"
#include "WidgetLayoutLibrary.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "CGAL/Exact_predicates_exact_constructions_kernel.h"
#include "CGAL/intersections.h"
#include "Kismet/KismetMathLibrary.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include "CGALWrapper/PolygonAlg.h"
#include "CGAL/number_utils.h"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include "FileHelper.h"
#include "openssl/md5.h"

#define MIN_ORTHOZOOM				250.0					/* Limit of 2D viewport zoom in */
#define MAX_ORTHOZOOM				MAX_FLT					/* Limit of 2D viewport zoom out */
#define ALLOWABLE_ERROR				0.5

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef K::Segment_2 Segment_2;
typedef K::Intersect_2 Intersect_2;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef CGAL::Polygon_with_holes_2<K> Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2> Pwh_list_2;

FComboBoxInfo::FComboBoxInfo()
{

}

FComboBoxInfo::FComboBoxInfo(FString Inkey, float InId)
{
	Key = Inkey;
}

FComboBoxInfo::FComboBoxInfo(FString Inkey, TArray<UVaRestJsonObject*> InOptions)
{
	Key = Inkey;
	Options = InOptions;
}

void FComboBoxInfo::SetKey(FString Inkey)
{
	Key = Inkey;
}

void FComboBoxInfo::AddValue(UVaRestJsonObject* Option)
{
	Options.Add(Option);
}

void FComboBoxInfo::CleanInfo()
{
	Key.Empty();
	Options.Empty();
}

FBrandInfo::FBrandInfo()
{
	Name.Empty();
	ID = -1;
	Level = -1;
}

FBrandInfo::FBrandInfo(FString InName, int32 InID, int32 InLevel)
{
	Name = InName;
	ID = InID;
	Level = InLevel;
}

FCategoryInfo::FCategoryInfo()
{
	Name.Empty();
	ID = -1;
	Level = -1;
}

FCategoryInfo::FCategoryInfo(FString InName, int32 InID, int32 InLevel)
{
	Name = InName;
	ID = InID;
	Level = InLevel;
}

FRoomUsage::FRoomUsage(int32 InId, FString InName)
{
	Id = InId;
	Name = InName;
}

FRoomMark::FRoomMark(int32 InMark, FString InName, float InSize, TArray<FRoomUsage> InUsages)
{
	Mark = InMark;
	Name = InName;
	Size = InSize;
	Usages = InUsages;
}

void ULuoLuoBlueprintFunctionLibrary::SetKey(FComboBoxInfo& Target, FString InKey)
{
	Target.SetKey(InKey);
}

void ULuoLuoBlueprintFunctionLibrary::AddValue(FComboBoxInfo& Target, UVaRestJsonObject* Option)
{
	Target.AddValue(Option);
}

FString ULuoLuoBlueprintFunctionLibrary::GetKey(FComboBoxInfo& Target)
{
	return Target.Key;
}

UVaRestJsonObject* ULuoLuoBlueprintFunctionLibrary::GetOption(FComboBoxInfo& Target, int Index)
{
	return Target.Options[Index];
}

TArray<UVaRestJsonObject*> ULuoLuoBlueprintFunctionLibrary::GetOptions(FComboBoxInfo& Target)
{
	return Target.Options;
}

void ULuoLuoBlueprintFunctionLibrary::CleanInfo(FComboBoxInfo& Target)
{
	Target.CleanInfo();
}

void ULuoLuoBlueprintFunctionLibrary::Contains(TArray<FComboBoxInfo>& Target, FString Key, int& Index, FComboBoxInfo& Item, bool& bFound)
{
	Index = -1;
	bFound = false;
	for (int i = 0; i < Target.Num(); i++)
	{
		if (Target[i].Key == Key)
		{
			Index = i;
			Item = Target[i];
			bFound = true;
			return;
		}
	}
}

UVaRestJsonObject* ULuoLuoBlueprintFunctionLibrary::FindStringField(TArray<UVaRestJsonObject*>& Target, FString Key, FString Value, int& Index, bool& bFound)
{
	Index = -1;
	bFound = false;

	for (int i = 0; i < Target.Num(); i++)
	{
		if (Target[i]->GetStringField(Key) == Value)
		{
			Index = i;
			bFound = true;
			return Target[i];
		}
	}
	return nullptr;
}

UVaRestJsonObject* ULuoLuoBlueprintFunctionLibrary::FindNumberField(TArray<UVaRestJsonObject*>& Target, FString Key, float Value, int& Index, bool& bFound)
{
	Index = -1;
	bFound = false;

	for (int i = 0; i < Target.Num(); i++)
	{
		if (Target[i]->GetNumberField(Key) == Value)
		{
			Index = i;
			bFound = true;
			return Target[i];
		}
	}
	return nullptr;
}

UVaRestJsonObject* ULuoLuoBlueprintFunctionLibrary::FindBoolField(TArray<UVaRestJsonObject*>& Target, FString Key, bool Value, int& Index, bool& bFound)
{
	Index = -1;
	bFound = false;

	for (int i = 0; i < Target.Num(); i++)
	{
		if (Target[i]->GetBoolField(Key) == Value)
		{
			Index = i;
			bFound = true;
			return Target[i];
		}
	}
	return nullptr;
}

UVaRestJsonObject* ULuoLuoBlueprintFunctionLibrary::FindObjectField(TArray<UVaRestJsonObject*>& Target, FString Key, UVaRestJsonObject* Value, int& Index, bool& bFound)
{
	Index = -1;
	bFound = false;

	for (int i = 0; i < Target.Num(); i++)
	{
		if (Target[i]->GetObjectField(Key) == Value)
		{
			Index = i;
			bFound = true;
			return Target[i];
		}
	}
	return nullptr;
}

void ULuoLuoBlueprintFunctionLibrary::SetMember_BrandInfo(FBrandInfo& Target, const FString& InName, int32 InID, int32 InLevel)
{
	Target.Name = InName;
	Target.ID = InID;
	Target.Level = InLevel;
}

void ULuoLuoBlueprintFunctionLibrary::SetMember_CategoryInfo(FCategoryInfo& Target, const FString& InName, int32 InID, int32 InLevel)
{
	Target.Name = InName;
	Target.ID = InID;
	Target.Level = InLevel;
}

void ULuoLuoBlueprintFunctionLibrary::GetIDWithName_BrandInfo(TArray<FBrandInfo>& Target, const FString& Name, FBrandInfo& Item, bool& bFound)
{
	bFound = false;
	for (int i = 0; i < Target.Num(); i++)
	{
		if (Target[i].Name == Name)
		{
			Item = Target[i];
			bFound = true;
			return;
		}
	}
}

void ULuoLuoBlueprintFunctionLibrary::GetIDWithName_CategoryInfo(TArray<FCategoryInfo>& Target, const FString& Name, FCategoryInfo& Item, bool& bFound)
{
	bFound = false;
	for (int i = 0; i < Target.Num(); i++)
	{
		if (Target[i].Name == Name)
		{
			Item = Target[i];
			bFound = true;
			return;
		}
	}

}

float ULuoLuoBlueprintFunctionLibrary::GetFloatFromStringWithClean(FString InStr, FString StrToRemove)
{
	FString Str = InStr;

	bool bFinished = false;

	while (bFinished == false)
	{
		int32 FoundIndex = Str.Find(TEXT(","));
		if (FoundIndex != -1)
		{
			Str.RemoveAt(FoundIndex);
		}
		else
		{
			bFinished = true;
		}
	}

	if (!StrToRemove.IsEmpty())
	{
		bFinished = false;

		while (bFinished == false)
		{
			int32 FoundIndex = Str.Find(StrToRemove);
			if (FoundIndex != -1)
			{
				Str.RemoveAt(FoundIndex);
			}
			else
			{
				bFinished = true;
			}
		}
	}
	/*
	bFinished = false;

	while (bFinished == false)
	{
		int32 FoundIndex = Str.Find(TEXT("."));
		if (FoundIndex != -1)
		{
			Str.RemoveAt(FoundIndex);
		}
		else
		{
			bFinished = true;
		}
	}
	*/
	return FCString::Atof(*Str);
}

float ULuoLuoBlueprintFunctionLibrary::FloatStringOperation(TArray<FString> Values, int32 Precision, bool bAdd)
{
	int32 PrecisionVal = 1;

	for (int i = 0; i < Precision; i++)
	{
		PrecisionVal *= 10;
	}

	TArray<FString> TempValues = Values;
	TArray<int32>	TempInt;
	bool bFinished = false;

	for (int i = 0; i < TempValues.Num(); i++)
	{
		bFinished = false;
		while (!bFinished)
		{
			int32 FoundIndex = TempValues[i].Find(TEXT(","));
			if (FoundIndex != -1)
			{
				TempValues[i].RemoveAt(FoundIndex);
			}
			else
			{
				bFinished = true;
			}
		}

		bFinished = false;

		while (!bFinished)
		{
			int32 FoundIndex = TempValues[i].Find(TEXT("."));
			if (FoundIndex != -1)
			{
				TempValues[i].RemoveAt(FoundIndex);
			}
			else
			{
				bFinished = true;
			}
		}

		TempInt.Add(FCString::Atoi(*TempValues[i]));
	}

	float Result = 0.0;

	if (bAdd)
	{
		int32 TempResult = 0;

		for (int i = 0; i < TempInt.Num(); i++)
		{
			TempResult += TempInt[i];
		}

		Result = (float)TempResult / PrecisionVal;
	}
	else
	{
		if (TempInt.IsValidIndex(0))
		{
			int32 TempResult = TempInt[0];

			for (int i = 1; i < TempInt.Num(); i++)
			{
				TempResult -= TempInt[i];
			}

			Result = (float)TempResult / PrecisionVal;
		}
	}

	return Result;
}

void ULuoLuoBlueprintFunctionLibrary::GetIdWithName(TArray<FComboBoxInfo>& Target, FString InKey, float& Id, bool& bFound)
{
	bFound = false;
	Id = -1;

	for (int i = 0; i < Target.Num(); i++)
	{
		if (Target[i].Key == InKey)
		{
			bFound = true;
			Id = Target[i].Id;
			return;
		}
	}
}

void ULuoLuoBlueprintFunctionLibrary::GetNameWithId(TArray<FComboBoxInfo>& Target, float InId, FString& Name, bool& bFound)
{
	bFound = false;
	Name = "N/A";

	for (int i = 0; i < Target.Num(); i++)
	{
		if (Target[i].Id == InId)
		{
			bFound = true;
			Name = Target[i].Key;
			return;
		}
	}
}

int32 ULuoLuoBlueprintFunctionLibrary::PriceCeil_Int(int32 Price)
{
	if (Price > 10)
	{
		int32 TempPrice = Price / 10;
		int32 PriceFactor = Price - (TempPrice * 10);

		if (PriceFactor > 0)
		{
			return ((TempPrice * 10) + 10);
		}
		else
		{
			return (TempPrice * 10);
		}
	}
	else if (Price > 0)
	{
		return 10;
	}
	else
	{
		return 0;
	}
}

int32 ULuoLuoBlueprintFunctionLibrary::PriceCeil_Float(float Price)
{
	int32 PriceInt = ceil(Price);

	return PriceCeil_Int(PriceInt);
}

FString ULuoLuoBlueprintFunctionLibrary::DoubleStringOperation(TArray<FString>Values, int32 Precision, bool bAdd)
{
	int32 PrecisionVal = 1;

	for (int i = 0; i < Precision; i++)
	{
		PrecisionVal *= 10;
	}

	TArray<FString> TempValues = Values;

	TArray<int32>	TempInt;
	bool bFinished = false;

	for (int i = 0; i < TempValues.Num(); i++)
	{
		bFinished = false;
		while (!bFinished)
		{
			int32 FoundIndex = TempValues[i].Find(TEXT(","));
			if (FoundIndex != -1)
			{
				TempValues[i].RemoveAt(FoundIndex);
			}
			else
			{
				bFinished = true;
			}
		}

		double TempDouble = (FCString::Atod(*TempValues[i]) + 0.005) * PrecisionVal;

		TempInt.Add((int32)TempDouble);
	}

	double Result = 0.0;

	if (bAdd)
	{
		int32 TempResult = 0;

		for (int i = 0; i < TempInt.Num(); i++)
		{
			TempResult += TempInt[i];
		}
		Result = (double)TempResult / PrecisionVal;
	}
	else
	{
		if (TempInt.IsValidIndex(0))
		{
			int32 TempResult = TempInt[0];

			for (int i = 1; i < TempInt.Num(); i++)
			{
				TempResult -= TempInt[i];
			}
			Result = (double)TempResult / PrecisionVal;
		}
	}

	FString ResultStr = FString::SanitizeFloat(Result);

	int32 DotIndex = ResultStr.Find(".");
	if (DotIndex != -1)
	{
		if (((ResultStr.Len() - 1) - DotIndex) == 1)
		{
			ResultStr.Append(TEXT("0"));
		}
	}
	
	return ResultStr;
}

void ULuoLuoBlueprintFunctionLibrary::ClipboardCopy(FString Message)
{
	FWindowsPlatformMisc::ClipboardCopy(*Message);
}

FString ULuoLuoBlueprintFunctionLibrary::ClipboardPaste()
{
	FString Result;
	FWindowsPlatformMisc::ClipboardPaste(Result);
	return Result;
}

FString ULuoLuoBlueprintFunctionLibrary::GetFilePathWithExtension(FString FilePath, FString Extension)
{
	TArray<FString> Files;

	IFileManager& FileManager = IFileManager::Get();
	FileManager.FindFiles(Files, *FilePath, *Extension);

	for (int32 i = 0; i < Files.Num(); i++)
	{
		FString TempName = Files[i];
		Files[i] = FilePath + Files[i];
	}

	double MaxFileAge = FileManager.GetFileAgeSeconds(*(Files[0]));
	int32 FileIndex = 0;

	for (int32 i = 1; i < Files.Num(); i++)
	{
		if (MaxFileAge > FileManager.GetFileAgeSeconds(*(Files[i])))
		{
			MaxFileAge = FileManager.GetFileAgeSeconds(*(Files[i]));
			FileIndex = i;
		}
	}

	if (Files.IsValidIndex(FileIndex))
	{
		return Files[FileIndex];
	}
	else
	{
		return TEXT("");
	}
}

void ULuoLuoBlueprintFunctionLibrary::EmulateInputForAnsel()
{
	if (GEngine)
	{
		GEngine->GetWorld()->GetFirstPlayerController()->InputKey(EKeys::LeftAlt, EInputEvent::IE_Pressed, 1.0f, false);
		GEngine->GetWorld()->GetFirstPlayerController()->InputKey(EKeys::F2, EInputEvent::IE_Pressed, 0.5f, false);
	}
}

TArray<FString> ULuoLuoBlueprintFunctionLibrary::ScanDirectory(FString FilePath, FString Extension)
{
	FString SearchPath = FilePath + Extension;
	TArray<FString> FoundFiles;

	IFileManager::Get().FindFiles(FoundFiles, *SearchPath, true, false);

	return FoundFiles;
}

FCSVFile ULuoLuoBlueprintFunctionLibrary::CreateCSVHandle(int32 Cols)
{
	return FCSVFile(Cols);
}

void ULuoLuoBlueprintFunctionLibrary::SetCSVFileHeader(UPARAM(ref)FCSVFile& Handle, FCSVAtom Header)
{
	Handle.SetHeader(Header);
}

void ULuoLuoBlueprintFunctionLibrary::AddCSVFileContent(UPARAM(ref)FCSVFile& Handle, FCSVAtom Content)
{
	Handle.AddContent(Content);
}

FCSVAtom ULuoLuoBlueprintFunctionLibrary::GetHeaderWithHandle(UPARAM(ref)FCSVFile& Handle)
{
	return Handle.Header;
}

void ULuoLuoBlueprintFunctionLibrary::SaveCSVFile(UPARAM(ref)FCSVFile& Handle, FString Path)
{
	if (Path.IsEmpty())
	{
		return;
	}

	// Write header to file
	FString TempHeader;

	for (int i = 0; i < Handle.Cols; i++)
	{
		TempHeader += Handle.Header.Content[i];
		TempHeader += ",";
	}
	TempHeader.RemoveFromEnd(",");
	TempHeader += "\n";

	FFileHelper::SaveStringToFile(TempHeader, *Path, FFileHelper::EEncodingOptions::ForceUTF8);

	// Write content to file
	FString TempRow;

	for (int i = 0; i < Handle.Content.Num(); i++)
	{
		TempRow.Empty();

		for (int j = 0; j < Handle.Cols; j++)
		{
			if (!Handle.Content[i].Content[j].IsEmpty())
			{
				TempRow += Handle.Content[i].Content[j];
				TempRow += ",";
			}
		}
		TempRow.RemoveFromEnd(",");
		TempRow += "\n";

		FFileHelper::SaveStringToFile(TempRow, *Path, FFileHelper::EEncodingOptions::ForceUTF8, &(IFileManager::Get()), FILEWRITE_Append);
	}

}

void ULuoLuoBlueprintFunctionLibrary::SnapActorToTarget(AActor* Target, bool bSnapToFloor)
{
	if (!Target)
	{
		return;
	}

	FVector StartLocation = Target->GetRootComponent()->Bounds.Origin;
	FVector Extent = Target->GetRootComponent()->Bounds.BoxExtent;
	FVector LocationOffset = FVector::ZeroVector;

	FVector Direction = FVector::ZeroVector;
	if (bSnapToFloor)
	{
		LocationOffset = StartLocation - Target->GetActorLocation();
		Direction = FVector(0.f, 0.f, -1.f);
	}
	else
	{
		LocationOffset = StartLocation - (Target->GetActorLocation() + FVector::FVector(0, 0, Target->GetRootComponent()->Bounds.BoxExtent.Z));
		Direction = FVector(0.f, 0.f, 1.f);
	}

	FHitResult Hit(1.0f);
	FCollisionQueryParams Params(FName(TEXT("MoveActorToTrace")), false);

	Params.AddIgnoredActor(Target);

	if (Target->GetWorld()->SweepSingleByChannel(Hit, StartLocation, StartLocation + Direction * WORLD_MAX, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeBox(Extent), Params))
	{
		FVector NewLocation = Hit.Location - LocationOffset;
		NewLocation.Z += KINDA_SMALL_NUMBER;

		Target->SetActorLocation(NewLocation);
	}
}

UTexture2D* ULuoLuoBlueprintFunctionLibrary::GetViewportTextureData()
{
	return nullptr;
}

FComputerInfo ULuoLuoBlueprintFunctionLibrary::GetComputerInformation()
{
	FComputerInfo ResultInfo;

	ResultInfo.CPU = FPlatformMisc::GetCPUBrand();

	ResultInfo.GPU = FPlatformMisc::GetPrimaryGPUBrand();

	FString OsSubVersion;

	FPlatformMisc::GetOSVersions(ResultInfo.OS, OsSubVersion);

	FPlatformMemoryStats MemoryState = FPlatformMemory::GetStats();

	ResultInfo.RAM = FString::FromInt(MemoryState.TotalPhysicalGB);
	ResultInfo.RAM.Append(TEXT(" GB"));

	return ResultInfo;
}

FString ULuoLuoBlueprintFunctionLibrary::GetComputerMacAddress()
{
	return FPlatformMisc::GetMacAddressString();
}

EHttpCodes ULuoLuoBlueprintFunctionLibrary::ConvertToEnum(int32 ErrorCode)
{
	EHttpCodes Result = EHttpCodes::Ok;

	switch (ErrorCode)
	{
	case 100:
		Result = EHttpCodes::Continue;
		break;

	case 101:
		Result = EHttpCodes::SwitchProtocol;
		break;

	case 200:
		Result = EHttpCodes::Ok;
		break;

	case 201:
		Result = EHttpCodes::Created;
		break;

	case 202:
		Result = EHttpCodes::Accepted;
		break;

	case 203:
		Result = EHttpCodes::Partial;
		break;

	case 204:
		Result = EHttpCodes::NoContent;
		break;

	case 205:
		Result = EHttpCodes::ResetContent;
		break;

	case 206:
		Result = EHttpCodes::PartialContent;
		break;

	case 300:
		Result = EHttpCodes::Ambiguous;
		break;

	case 301:
		Result = EHttpCodes::Moved;
		break;

	case 302:
		Result = EHttpCodes::Redirect;
		break;

	case 303:
		Result = EHttpCodes::RedirectMethod;
		break;

	case 304:
		Result = EHttpCodes::NotModified;
		break;

	case 305:
		Result = EHttpCodes::UseProxy;
		break;

	case 307:
		Result = EHttpCodes::RedirectKeepVerb;
		break;

	case 400:
		Result = EHttpCodes::BadRequest;
		break;

	case 401:
		Result = EHttpCodes::Denied;
		break;

	case 402:
		Result = EHttpCodes::PaymentReq;
		break;

	case 403:
		Result = EHttpCodes::Forbidden;
		break;

	case 404:
		Result = EHttpCodes::NotFound;
		break;

	case 405:
		Result = EHttpCodes::BadMethod;
		break;

	case 406:
		Result = EHttpCodes::NoneAcceptable;
		break;

	case 407:
		Result = EHttpCodes::ProxyAuthReq;
		break;

	case 408:
		Result = EHttpCodes::RequestTimeout;
		break;

	case 409:
		Result = EHttpCodes::Conflict;
		break;

	case 410:
		Result = EHttpCodes::Gone;
		break;

	case 411:
		Result = EHttpCodes::LengthRequired;
		break;

	case 412:
		Result = EHttpCodes::PrecondFailed;
		break;

	case 413:
		Result = EHttpCodes::RequestTooLarge;
		break;

	case 414:
		Result = EHttpCodes::UriTooLong;
		break;

	case 415:
		Result = EHttpCodes::UnsupportedMedia;
		break;

	case 429:
		Result = EHttpCodes::TooManyRequests;
		break;

	case 449:
		Result = EHttpCodes::RetryWith;
		break;

	case 500:
		Result = EHttpCodes::ServerError;
		break;

	case 501:
		Result = EHttpCodes::NotSupported;
		break;

	case 502:
		Result = EHttpCodes::BadGateway;
		break;

	case 503:
		Result = EHttpCodes::ServiceUnavail;
		break;

	case 504:
		Result = EHttpCodes::GatewayTimeout;
		break;

	case 505:
		Result = EHttpCodes::VersionNotSup;
		break;

	default:
		Result = EHttpCodes::Unknown;
		break;
	}

	return Result;
}

FWallDependsArea ULuoLuoBlueprintFunctionLibrary::SetRoomIDWithStruct(UPARAM(ref)FWallDependsArea& Target, int32 NewRoomID)
{
	FWallDependsArea Result = Target;

	Result.RoomID = NewRoomID;

	return Result;
}

FWallDependsArea ULuoLuoBlueprintFunctionLibrary::SetChannelIDWithStruct(UPARAM(ref)FWallDependsArea& Target, int32 NewChannelID)
{
	FWallDependsArea Result = Target;

	Result.ChannelID = NewChannelID;

	return Result;
}

FWallDependsArea ULuoLuoBlueprintFunctionLibrary::SetGUIDWithStruct(UPARAM(ref)FWallDependsArea& Target, FString NewGUID)
{
	FWallDependsArea Result = Target;

	Result.WallGUID = NewGUID;

	return Result;
}

FWallDependsArea ULuoLuoBlueprintFunctionLibrary::SetAcreageWithStruct(UPARAM(ref)FWallDependsArea& Target, FString NewAcreage)
{
	FWallDependsArea Result = Target;

	Result.Acreage = NewAcreage;

	return Result;
}

FWallDependsArea ULuoLuoBlueprintFunctionLibrary::SetDesignsWithStruct(UPARAM(ref)FWallDependsArea& Target, TArray<FHardDesignNode> NewDesigns)
{
	FWallDependsArea Result = Target;

	Result.Designs = NewDesigns;

	return Result;
}

bool ULuoLuoBlueprintFunctionLibrary::SetDesignWithStruct(UPARAM(ref)FWallDependsArea& Target, FWallDependsArea& Result, FHardDesignNode NewDesign)
{
	FWallDependsArea TempResult = Target;

	for (int i = 0; i < TempResult.Designs.Num(); i++)
	{
		if (TempResult.Designs[i].UsageID == NewDesign.UsageID)
		{
			TempResult.Designs[i] = NewDesign;

			Result = TempResult;

			return true;
		}
	}

	Result = TempResult;

	return false;
}

bool ULuoLuoBlueprintFunctionLibrary::GetDesignWithUsageID(UPARAM(ref)FWallDependsArea& Target, int32 UsageId, FHardDesignNode& Result)
{
	for (int i = 0; i < Target.Designs.Num(); i++)
	{
		if (Target.Designs[i].UsageID == UsageId)
		{
			Result = Target.Designs[i];

			return true;
		}
	}

	return false;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::ConstructBigDecimalWithBigDecimal(const FBigDecimal& InVal)
{
	FBigDecimal Result(InVal);

	return Result;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::ConstructBigDecimalWithString(const FString& InVal)
{
	FString Temp = InVal;
	if (InVal.IsEmpty())
	{
		Temp = "0";
	}

	FBigDecimal Result(Temp);
	return Result;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::ConstructBigDecimalWithInt(const int32 InVal)
{
	FBigDecimal Result(InVal);

	return Result;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::ConstructBigDecimalWithFloat(const float InVal)
{
	FBigDecimal Result(InVal);

	return Result;
}

FBigDecimal& ULuoLuoBlueprintFunctionLibrary::AssignmentWithBigDecimal(UPARAM(ref)FBigDecimal& Target, const FBigDecimal& InVal)
{
	Target = InVal;

	return Target;
}

FBigDecimal& ULuoLuoBlueprintFunctionLibrary::AssignmentWithString(UPARAM(ref)FBigDecimal& Target, const FString& InVal)
{
	Target = InVal;

	return Target;
}

FBigDecimal& ULuoLuoBlueprintFunctionLibrary::AssignmentWithInt(UPARAM(ref)FBigDecimal& Target, const int32 InVal)
{
	Target = InVal;

	return Target;
}

FBigDecimal& ULuoLuoBlueprintFunctionLibrary::AssignmentWithFloat(UPARAM(ref)FBigDecimal& Target, const float InVal)
{
	Target = InVal;

	return Target;
}

FString ULuoLuoBlueprintFunctionLibrary::ConvertToString(FBigDecimal InVal)
{
	return ConverToStringWithPrecisionControl(InVal, 2, false);
}

FString ULuoLuoBlueprintFunctionLibrary::ConverToStringWithPrecisionControl(FBigDecimal InVal, int32 Precision, bool bRound)
{

	FString ResultStr = InVal.ToString();

	FString LeftStr;

	FString RightStr;

	ResultStr.Split(TEXT("."), &LeftStr, &RightStr);

	ResultStr.Empty();

	ResultStr.Append(LeftStr);

	if (Precision > 0)
	{
		ResultStr.Append(TEXT("."));
		if (Precision >= RightStr.Len())
		{
			ResultStr.Append(RightStr);

			for (int i = RightStr.Len(); i < Precision; i++)
			{
				ResultStr.Append(TEXT("0"));
			}
		}
		else
		{
			FString RealRight;
			for (int i = 0; i < Precision; i++)
			{
				RealRight.AppendChar(RightStr[i]);
			}
			
			ResultStr.Append(RealRight);

			if (bRound)
			{

				ResultStr.RemoveAt(ResultStr.Find(TEXT(".")));

				FString DirtyStr;

				DirtyStr.AppendChar(RightStr[Precision]);

				FBigDecimal DirtyBD(DirtyStr);

				FBigDecimal ResultBD(ResultStr);

				if (DirtyBD > 4)
				{
					ResultBD += 1;
				}

				ResultStr = ResultBD.ToString();

				ResultStr.RemoveAt(ResultStr.Find(TEXT(".")));

				bool bIsPositive = ResultStr.Find(TEXT("-")) == -1;

				if (!bIsPositive)
				{
					ResultStr.RemoveAt(ResultStr.Find(TEXT("-")));
				}

				ResultStr.ReverseString();

				int32 LoopTimes = Precision - ResultStr.Len();

				for (int32 i = 0; i <= LoopTimes; i++)
				{
					ResultStr.Append(TEXT("0"));
				}

				ResultStr.InsertAt(Precision, TEXT("."));

				if (!bIsPositive)
				{
					ResultStr.Append(TEXT("-"));
				}

				ResultStr.ReverseString();
			}
		}
	}
	else
	{
		if (RightStr.Len() > 0)
		{
			FString DirtyStr;

			DirtyStr.AppendChar(RightStr[0]);

			FBigDecimal DirtyBD(DirtyStr);

			FBigDecimal ResultBD(ResultStr);

			if (DirtyBD > 4)
			{
				ResultBD += 1;
			}

			ResultStr = ResultBD.ToString();

			ResultStr.RemoveAt(ResultStr.Find(TEXT(".")));
		}
	}

	return ResultStr;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::Operation_Add(FBigDecimal First, FBigDecimal Second)
{
	return First + Second;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::Operation_Minus(FBigDecimal First, FBigDecimal Second)
{
	return First - Second;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::Operation_Multiply(FBigDecimal First, FBigDecimal Second)
{
	FString FirstNum = First.ToString().Append(TEXT("0"));

	FString SecondNum = Second.ToString().Append(TEXT("0"));

	int32 FirstDotIdx = FirstNum.Find(TEXT("."));

	int32 SecondDotIdx = SecondNum.Find(TEXT("."));

	if (FirstDotIdx != -1)
	{
		FirstNum.RemoveAt(FirstDotIdx);
	}

	if (SecondDotIdx != -1)
	{
		SecondNum.RemoveAt(SecondDotIdx);
	}

	int32 FinalDotIdx = (FirstNum.Len() - (FirstDotIdx == -1 ? 0 : FirstDotIdx)) + (SecondNum.Len() - (SecondDotIdx == -1 ? 0 : SecondDotIdx));

	int64 FinalNum = FCString::Atoi64(*FirstNum) * FCString::Atoi64(*SecondNum);

	FBigDecimal Result(FString::Printf(TEXT("%lld"), FinalNum));

	FBigDecimal Dirty(FMath::Pow(10, FinalDotIdx));

	return Result / Dirty;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::Operation_Divided(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second)
{
	return First / Second;
}

bool ULuoLuoBlueprintFunctionLibrary::Operation_Bigger(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second)
{
	return First > Second;
}

bool ULuoLuoBlueprintFunctionLibrary::Operation_Less(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second)
{
	return First < Second;
}

bool ULuoLuoBlueprintFunctionLibrary::Operation_Equal(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second)
{
	return First == Second;
}

bool ULuoLuoBlueprintFunctionLibrary::Operation_BiggerOrEqual(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second)
{
	return First >= Second;
}

bool ULuoLuoBlueprintFunctionLibrary::Operation_LessOrEqual(UPARAM(ref)FBigDecimal& First, UPARAM(ref)FBigDecimal& Second)
{
	return First <= Second;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::Operation_Abs(UPARAM(ref)FBigDecimal& Target)
{
	return Target.Abs();
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::Operation_Floor(UPARAM(ref)FBigDecimal& Target)
{
	return Target.Floor();
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::Operation_Ceil(UPARAM(ref)FBigDecimal& Target)
{
	return Target.Ceil();
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::Operation_Round(UPARAM(ref)FBigDecimal& Target)
{
	return Target.Round();
}

bool ULuoLuoBlueprintFunctionLibrary::Operation_IsPositiveNumber(UPARAM(ref)FBigDecimal& Target)
{
	return Target.IsPositiveNumber();
}

bool ULuoLuoBlueprintFunctionLibrary::Operation_IsNegativeNumber(UPARAM(ref)FBigDecimal& Target)
{
	return Target.IsNegativeNumber();
}

bool ULuoLuoBlueprintFunctionLibrary::Operation_IsZero(UPARAM(ref)FBigDecimal& Target)
{
	return Target.IsZero();
}

bool ULuoLuoBlueprintFunctionLibrary::CheckPrecision(const FString& InVal, int32 LimitPrecision)
{
	if (LimitPrecision < 0)
	{
		return false;
	}

	FString LeftStr;

	FString RightStr;

	InVal.Split(TEXT("."), &LeftStr, &RightStr);

	if (RightStr.Len() > LimitPrecision)
	{
		return false;
	}

	return true;
}

FBigDecimal ULuoLuoBlueprintFunctionLibrary::Operation_DECCeil(UPARAM(ref)FBigDecimal& Target)
{
	FString TempPrice = ConverToStringWithPrecisionControl(Target.Ceil(), 0, false);

	FBigDecimal ResultBD("0");

	int32 PriceInt = FCString::Atoi(*TempPrice);

	if (TempPrice.Len() == 1)
	{
		if (PriceInt == 0)
		{
			ResultBD = 0;
		}
		else
		{
			ResultBD = 10;
		}

		return ResultBD;
	}
	else
	{
		FString TempDirty;

		const TArray<TCHAR> TempArray = TempPrice.GetCharArray();

		TempDirty = FString::Chr(TempArray[TempArray.Num() - 2]);

		FString DirtyStr(FString::Chr(TempArray[TempArray.Num() - 2]));
		TempPrice.RemoveFromEnd(DirtyStr);

		TempPrice.Append("0");

		int32 PriceInt = FCString::Atoi(*TempPrice);

		int32 DirtyInt = FCString::Atoi(*TempDirty);

		if (DirtyInt > 0)
		{
			PriceInt += 10;
		}

		ResultBD = PriceInt;

		return ResultBD;
	}
}

void ULuoLuoBlueprintFunctionLibrary::SetSkyLightAngle(USkyLightComponent *Skylight, float angle)
{
	if (Skylight != NULL)
	{
		Skylight->SourceCubemapAngle = angle;
		Skylight->RecaptureSky();
	}
}

bool ULuoLuoBlueprintFunctionLibrary::MyRegularExpression(FString str, FString reg)
{
	if (str.IsEmpty() || reg.IsEmpty())
	{
		return false;
	}
	FRegexPattern myPattern(reg);
	FRegexMatcher myMatcher(myPattern, str);
	myMatcher.SetLimits(0, str.Len());
	return myMatcher.FindNext();
}

void ULuoLuoBlueprintFunctionLibrary::FlushPressedKeys()
{
	APlayerController* playController = UGameplayStatics::GetPlayerController(GWorld, 0);
	if (playController != nullptr)
	{
		playController->FlushPressedKeys();
	}
	
}

bool ULuoLuoBlueprintFunctionLibrary::StrOperation_Bigger(FString A, FString B)
{
	if (A.Len() > B.Len())
	{
		return true;
	}
	else if (A.Len() < B.Len())
	{
		return false;
	}
	else
	{

		for (int32 i = 0; i < A.Len(); i++)
		{
			if (A.GetCharArray()[i] > B.GetCharArray()[i])
			{
				return true;
			}
			else if (A.GetCharArray()[i] < B.GetCharArray()[i])
			{
				return false;
			}
		}

		return false;
	}
}

bool ULuoLuoBlueprintFunctionLibrary::StrOperation_Less(FString A, FString B)
{
	if (A.Len() < B.Len())
	{
		return true;
	}
	else if (A.Len() > B.Len())
	{
		return false;
	}
	else
	{
		for (int32 i = 0; i < A.Len(); i++)
		{
			if (A.GetCharArray()[i] > B.GetCharArray()[i])
			{
				return false;
			}
			else if (A.GetCharArray()[i] < B.GetCharArray()[i])
			{
				return true;
			}
		}

		return false;
	}
}

bool ULuoLuoBlueprintFunctionLibrary::StrOperation_BiggerOrEqual(FString A, FString B)
{
	return StrOperation_Bigger(A, B) || (A == B);
}

bool ULuoLuoBlueprintFunctionLibrary::StrOperation_LessOrEqual(FString A, FString B)
{
	return StrOperation_Less(A, B) || (A == B);
}

FString ULuoLuoBlueprintFunctionLibrary::GetUnixTimestamp(int32 TimeZone)
{
	FDateTime CurrentTime = FDateTime::UtcNow() + FTimespan(TimeZone, 0, 0);

	int64 TempTime = CurrentTime.ToUnixTimestamp();

	FString ResultStr = FString::Printf(TEXT("%llu"), TempTime);

	return ResultStr;
}

FString ULuoLuoBlueprintFunctionLibrary::GetDesktopPath()
{
	FString ResultStr(FPlatformProcess::UserDir());

	FString LeftStr;
	FString RightStr;

	if (ResultStr.Split(TEXT("Documents/"), &LeftStr, &RightStr, ESearchCase::Type::IgnoreCase, ESearchDir::Type::FromEnd))
	{
		ResultStr = LeftStr;
	}

	return FPaths::Combine(ResultStr, TEXT("Desktop"));
}

FString ULuoLuoBlueprintFunctionLibrary::GetFileSize(FString FilePath)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		return TEXT("Error(Cloud not find file)");
	}

	const int64 TempSize = FPlatformFileManager::Get().GetPlatformFile().FileSize(*FilePath);

	FString FileSizeStr = FString::SanitizeFloat(TempSize / 1024.0f / 1024.0);

	return FileSizeStr;
}


ECameraAngleType ULuoLuoBlueprintFunctionLibrary::ConvertToCameraAngleType(int32 Source)
{
	switch (Source)
	{
	case 1:
		return ECameraAngleType::CAT_FlatView;

	case 2:
		return ECameraAngleType::CAT_UpwardView;

	case 3:
		return ECameraAngleType::CAT_VerticalView;

	default:
		return ECameraAngleType::CAT_FlatView;
	}
}

void ULuoLuoBlueprintFunctionLibrary::ExploreFolder(FString FilePath)
{
	FPlatformProcess::ExploreFolder(*FilePath);
}

bool ULuoLuoBlueprintFunctionLibrary::GetFileMD5(FString FilePath, FString & MD5)
{
	TArray<uint8> FileData;
	unsigned char MD5Result[16];
	if (FilePath.Len() > 0)
	{
		if (FFileHelper::LoadFileToArray(FileData, *FilePath))
		{
			MD5_CTX md5_ctx;
			MD5_Init(&md5_ctx);
			if (FileData.Num() > 0)
			{
				MD5_Update(&md5_ctx, FileData.GetData(), FileData.Num());
				MD5_Final(MD5Result, &md5_ctx);
				MD5.Empty();
				for (auto c_md5 : MD5Result)
				{
					MD5 += FString::Printf(TEXT("%02X"), (int)c_md5);
				}
				return true;
			}
		}
	}
	return false;
}

bool ULuoLuoBlueprintFunctionLibrary::CheckFileMD5(FString FilePath, FString InMD5, FString& MD5)
{
	if (GetFileMD5(FilePath, MD5))
	{
		if (MD5.Equals(InMD5, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}
	return false;
}

void ULuoLuoBlueprintFunctionLibrary::FocusCameraToSelection(class APlayerController* Player, class AActor* Target, float Factor)
{
	FVector Origin;
	FVector BoxExtent;

	Target->GetActorBounds(true, Origin, BoxExtent);

	FBox BoundingBox(Origin - (BoxExtent * Factor), Origin + (BoxExtent * Factor));

	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(Target);

	FVector Position = BoundingBox.GetCenter();

	float AspectToUse = ViewportSize.X / ViewportSize.Y;

	float Radius = FMath::Max(BoundingBox.GetExtent().Size(), 10.0f);

	if (!Player->PlayerCameraManager->IsOrthographic())
	{
		if (AspectToUse > 1.0f)
		{
			Radius *= AspectToUse;
		}

		const float HalfFOVRadians = FMath::DegreesToRadians(Player->PlayerCameraManager->GetFOVAngle() / 2.0f);

		const float DistanceFromSphere = Radius / FMath::Tan(HalfFOVRadians);

		APawn* OwnerPawn = Player->GetPawn();

		FVector PositionOffset = Player->GetControlRotation().Vector() * -DistanceFromSphere;

		if (OwnerPawn)
		{
			OwnerPawn->SetActorLocation(Position + PositionOffset);
		}
	}
	else
	{
		APawn* OwnerPawn = Player->GetPawn();

		if (OwnerPawn)
		{
			OwnerPawn->SetActorLocation(Position);
		}

		float NewOrthoZoom;

		uint32 MinAxisSize = (AspectToUse > 1.0f) ? ViewportSize.Y : ViewportSize.X;
		float Zoom = Radius / (MinAxisSize / 2.0f);

		NewOrthoZoom = Zoom * (ViewportSize.X * 15.0f);
		NewOrthoZoom = FMath::Clamp<float>(NewOrthoZoom, MIN_ORTHOZOOM, MAX_ORTHOZOOM);
		Player->PlayerCameraManager->SetOrthoWidth(NewOrthoZoom);
	}
}

void ULuoLuoBlueprintFunctionLibrary::FocusSceneCaptureToSelection(class USceneCaptureComponent2D* Camera, class AActor* Target, float Factor /*= 1.0f*/)
{
	FVector Origin;
	FVector BoxExtent;

	Target->GetActorBounds(false, Origin, BoxExtent);

	FBox BoundingBox(Origin - (BoxExtent * Factor), Origin + (BoxExtent * Factor));

	FVector2D ViewportSize(Camera->TextureTarget->SizeX, Camera->TextureTarget->SizeY);

	FVector Position = BoundingBox.GetCenter();

	float AspectToUse = ViewportSize.X / ViewportSize.Y;

	float Radius = FMath::Max(BoundingBox.GetExtent().Size(), 10.0f);

	if (Camera->ProjectionType != ECameraProjectionMode::Orthographic)
	{
		if (AspectToUse > 1.0f)
		{
			Radius *= AspectToUse;
		}

		const float HalfFOVRadians = FMath::DegreesToRadians(Camera->FOVAngle / 2.0f);

		const float DistanceFromSphere = Radius / FMath::Tan(HalfFOVRadians);

		FVector PositionOffset = Camera->GetComponentRotation().Vector() * -DistanceFromSphere;

		Camera->SetWorldLocation(Position + PositionOffset);

	}
	else
	{
		float NewOrthoZoom;

		uint32 MinAxisSize = (AspectToUse > 1.0f) ? ViewportSize.Y : ViewportSize.X;
		float Zoom = Radius / (MinAxisSize / 2.0f);

		NewOrthoZoom = Zoom * ViewportSize.X;
		NewOrthoZoom = FMath::Clamp<float>(NewOrthoZoom, MIN_ORTHOZOOM, MAX_ORTHOZOOM);
		
		Camera->OrthoWidth = NewOrthoZoom;
	}
}

float ULuoLuoBlueprintFunctionLibrary::ConvertScreenshotTypeToFloat(EScreenshotType Type)
{
	switch (Type)
	{
	case EScreenshotType::SST_Front:
		return 1.0f;
		
	case EScreenshotType::SST_FrontSecond:
		return 2.0f;

	case EScreenshotType::SST_FrontThird:
		return 3.0f;

	case EScreenshotType::SST_Left:
		return 4.0f;

	case EScreenshotType::SST_Right:
		return 5.0f;

	case EScreenshotType::SST_Back:
		return 6.0f;

	case EScreenshotType::SST_Top:
		return 7.0f;

	default:
		return 0.0f;
	}
}

void ULuoLuoBlueprintFunctionLibrary::ForceGC()
{
	GEngine->ForceGarbageCollection(true);
}

bool ULuoLuoBlueprintFunctionLibrary::IsCuttingLegal(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints, CuttingErrorType& Errortype)
{
	//判断起始点是否在区域边上
	Errortype = IsPointOnAreaLine(AreaPoints, CuttingPoints);
	if (Errortype != CuttingErrorType::Correct)
		return false;

	//判断切割线是否与区域边线相交、重叠
	Errortype = IsSegmentIntersectionLegal(AreaPoints, CuttingPoints);
	if (Errortype != CuttingErrorType::Correct)
		return false;

	//判断切割线是否与切割线相交
	Errortype = IsCuttingLineLegal(AreaPoints, CuttingPoints);
	if (Errortype == CuttingErrorType::Correct)
		return true;
	else
		return false;
}

bool ULuoLuoBlueprintFunctionLibrary::AreaSegment(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints, TArray<int32> PreviousCuttingLineIndex,TArray<FVector2D> &OutArea_1, TArray<FVector2D> &OutArea_2, TArray<int32>& Area1_CuttingLine, TArray<int32>& Area2_CuttingLine, CuttingErrorType& Errortype)
{
	TArray<FVector2D> TempCuttingPoints;
	for (FVector2D CurrentCuttingPoint : CuttingPoints)
	{
		bool FindNearPoint = false;
		FVector2D NearPoint;
		for (FVector2D CurrentAreaPoint : AreaPoints)
		{
			FVector2D TempSeg = CurrentCuttingPoint - CurrentAreaPoint;
			if (UKismetMathLibrary::Abs(TempSeg.Size()) < ALLOWABLE_ERROR)
			{
				FindNearPoint = true;
				NearPoint = CurrentAreaPoint;
				break;
			}
		}
		if (FindNearPoint)
		{
			TempCuttingPoints.Add(NearPoint);
		}
		else
		{
			TempCuttingPoints.Add(CurrentCuttingPoint);
		}
	}
	CuttingPoints = TempCuttingPoints;

	if (IsCuttingLegal(AreaPoints, CuttingPoints, Errortype))
	{
		FVector StartPoint, EndPoint;
		TArray<FVector2D> PointList = AreaPoints;
		TArray<FVector2D> TempCuttingPoints = CuttingPoints;
		int32 StartPointIndex = -1;
		int32 EndPointIndex = -1;
		TArray<FLineSegment> PreviousCuttingSeg;

		if (PreviousCuttingLineIndex.Num() > 0)
		{
			for (int32 i = 0; i < PreviousCuttingLineIndex.Num(); i++)
			{
				int32 index = PreviousCuttingLineIndex[i];
				FVector PreviousStartPoint, PreviousEndPoint;
				FLineSegment CuttingSeg;
				if ((index >= 0) && (index < AreaPoints.Num()))
				{
					PreviousStartPoint = FVector(AreaPoints[index].X, AreaPoints[index].Y, 0);
					if ((index + 1) < AreaPoints.Num())
						PreviousEndPoint = FVector(AreaPoints[index+1].X, AreaPoints[index+1].Y, 0);
					else
						PreviousEndPoint = FVector(AreaPoints[0].X, AreaPoints[0].Y, 0);
					CuttingSeg = FLineSegment(PreviousStartPoint, PreviousEndPoint);
					PreviousCuttingSeg.AddUnique(CuttingSeg);
				}
			}
		}

		//将切割起始点插入区域点位
		for (int32 i = 0; i < PointList.Num(); i++)
		{
			StartPoint = FVector(PointList[i].X, PointList[i].Y, 0);
			if ((i + 1) == PointList.Num())
				EndPoint = FVector(PointList[0].X, PointList[0].Y, 0);
			else
				EndPoint = FVector(PointList[i + 1].X, PointList[i + 1].Y, 0);
			if(UKismetMathLibrary::GetPointDistanceToSegment(FVector(CuttingPoints[0].X, CuttingPoints[0].Y, 0), StartPoint, EndPoint) <= ALLOWABLE_ERROR)
			{
				FVector PointOnSegment= UKismetMathLibrary::FindClosestPointOnSegment(FVector(CuttingPoints[0].X, CuttingPoints[0].Y, 0), StartPoint, EndPoint);
				TempCuttingPoints[0].X = PointOnSegment.X;
				TempCuttingPoints[0].Y = PointOnSegment.Y;
				StartPointIndex = i + 1;
				PointList.Insert(FVector2D(PointOnSegment.X, PointOnSegment.Y), StartPointIndex);
				if ((EndPointIndex != -1) && (StartPointIndex <= EndPointIndex))
				{
					EndPointIndex++;
				}
				break;
			}
		}

		//将切割终点插入区域点位
		for (int32 i = 0; i < PointList.Num(); i++)
		{
			StartPoint = FVector(PointList[i].X, PointList[i].Y, 0);
			if ((i + 1) == PointList.Num())
				EndPoint = FVector(PointList[0].X, PointList[0].Y, 0);
			else
				EndPoint = FVector(PointList[i + 1].X, PointList[i + 1].Y, 0);
			//AreaSegment = Segment_2(StartPoint, EndPoint);
			if (UKismetMathLibrary::GetPointDistanceToSegment(FVector(CuttingPoints[CuttingPoints.Num() - 1].X, CuttingPoints[CuttingPoints.Num() - 1].Y, 0), StartPoint, EndPoint) <= ALLOWABLE_ERROR)
			{
				FVector PointOnSegment = UKismetMathLibrary::FindClosestPointOnSegment(FVector(CuttingPoints[CuttingPoints.Num() - 1].X, CuttingPoints[CuttingPoints.Num() - 1].Y, 0), StartPoint, EndPoint);
				TempCuttingPoints[CuttingPoints.Num() - 1].X = PointOnSegment.X;
				TempCuttingPoints[CuttingPoints.Num() - 1].Y = PointOnSegment.Y;
				EndPointIndex = i + 1;
				PointList.Insert(FVector2D(PointOnSegment.X, PointOnSegment.Y), EndPointIndex);
				if ((StartPointIndex != -1) && (EndPointIndex <= StartPointIndex))
				{
					StartPointIndex++;
				}
				break;
			}
		}

		if (StartPointIndex == -1 || EndPointIndex == -1)
		{
			return false;
		}

		//分割后的第一个区域
		for (int32 i = 0; i < PointList.Num(); i++)
		{
			if ((i != StartPointIndex) & (i != EndPointIndex))
			{
				OutArea_1.Add(PointList[i]);
			}
			else
			{
				if (i == StartPointIndex)
				{
					for (int32 j = 0; j < TempCuttingPoints.Num(); j++)
					{
						OutArea_1.Add(TempCuttingPoints[j]);
					}
					i = EndPointIndex;
				}
				else if (i == EndPointIndex)
				{
					for (int32 j = TempCuttingPoints.Num() - 1; j >= 0; j--)
					{
						OutArea_1.Add(TempCuttingPoints[j]);
					}
					i = StartPointIndex;
				}
			}
		}
		//分割后的第二个区域
		if (StartPointIndex < EndPointIndex)
		{
			for (int32 i = TempCuttingPoints.Num() - 1; i >= 0; i--)
			{
				OutArea_2.Add(TempCuttingPoints[i]);
			}
			for (int32 i = StartPointIndex + 1; i < EndPointIndex; i++)
			{
				OutArea_2.Add(PointList[i]);
			}
		}
		else
		{
			for (int32 i = 0; i < TempCuttingPoints.Num(); i++)
			{
				 OutArea_2.Add(TempCuttingPoints[i]);
			}
			for (int32 i = EndPointIndex + 1; i < StartPointIndex; i++)
			{
				OutArea_2.Add(PointList[i]);
			}
		}
		DeleteCoincidentPoint(OutArea_1);
		DeleteCoincidentPoint(OutArea_2);
		Area1_CuttingLine = GetCuttingLineIndex(OutArea_1, TempCuttingPoints, PreviousCuttingSeg);
		Area2_CuttingLine = GetCuttingLineIndex(OutArea_2, TempCuttingPoints, PreviousCuttingSeg, false);
		return true;
	}
	return false;
}

bool ULuoLuoBlueprintFunctionLibrary::IsPointOnAreaSegment(TArray<FVector2D> AreaPoints, FVector2D Point)
{
	FVector StartPoint, EndPoint;
	for (int32 i = 0; i < AreaPoints.Num(); i++)
	{
		StartPoint = FVector(AreaPoints[i].X, AreaPoints[i].Y, 0);

		if ((i + 1) == AreaPoints.Num())
			EndPoint = FVector(AreaPoints[0].X, AreaPoints[0].Y, 0);
		else
			EndPoint = FVector(AreaPoints[i + 1].X, AreaPoints[i + 1].Y, 0);
		
		if (UKismetMathLibrary::GetPointDistanceToSegment(FVector(Point.X, Point.Y, 0), StartPoint, EndPoint) <= ALLOWABLE_ERROR)
		{
			return true;
		}
	}
	return false;
}

TArray<FVector2D> ULuoLuoBlueprintFunctionLibrary::GetCuttingPoints(TArray<FVector2D> InArea_1, TArray<FVector2D> InArea_2)
{
	TArray<FVector2D> CuttingPoints;
	for (TArray<FVector2D>::TIterator Area1_it = InArea_1.CreateIterator(); Area1_it; Area1_it++)
	{
		for (TArray<FVector2D>::TIterator Area2_it = InArea_2.CreateIterator(); Area2_it; Area2_it++)
		{
			if (Area1_it->Equals(*Area2_it))
			{
				CuttingPoints.Add(*Area1_it);
			}
		}
	}
	return CuttingPoints;
}

void ULuoLuoBlueprintFunctionLibrary::DeletePointOnSeg(TArray<FVector2D> InPointList, TArray<FVector2D> &OutPointList)
{
	if (!(InPointList.Num() > 0))
	{
		return ;
	}

	DeleteCoincidentPoint(InPointList);

	if (InPointList[0].Equals(InPointList.Last(), ALLOWABLE_ERROR))
	{
		InPointList.RemoveAt(InPointList.Num() - 1);
	}
		
	if (InPointList.Num() > 2)
	{
		for (int32 i = 0; i < (InPointList.Num() - 2); i++)
		{
			OutPointList.Add(InPointList[i]);
			for (int32 j = i + 2; j < InPointList.Num(); j++)
			{
				float PointDistance = UKismetMathLibrary::GetPointDistanceToSegment(FVector(InPointList[j - 1].X, InPointList[j - 1].Y, 0.0f), FVector(InPointList[i].X, InPointList[i].Y, 0.0f), FVector(InPointList[j].X, InPointList[j].Y, 0.0f));
				if (PointDistance > ALLOWABLE_ERROR)
				{
					if(j == (InPointList.Num() - 1))
					{
						OutPointList.Add(InPointList[(InPointList.Num() - 2)]);
					}
					i = j - 2;
					break;
				}
				if (j == (InPointList.Num() - 1))
				{
					i = j - 2;
				}
			}
		}
		OutPointList.Add(InPointList[InPointList.Num() - 1]);

		bool DeleteStart = false;
		bool DeleteEnd = false;

		if (OutPointList.Num() >= 4)
		{
			float PointDistance = UKismetMathLibrary::GetPointDistanceToSegment(FVector(OutPointList[OutPointList.Num() - 1].X, OutPointList[OutPointList.Num() - 1].Y, 0.0f), FVector(OutPointList[0].X, OutPointList[0].Y, 0.0f), FVector(OutPointList[OutPointList.Num() - 2].X, OutPointList[OutPointList.Num() - 2].Y, 0.0f));
			if (PointDistance < ALLOWABLE_ERROR)
			{
				DeleteEnd = true;
			}
			PointDistance = UKismetMathLibrary::GetPointDistanceToSegment(FVector(OutPointList[0].X, OutPointList[0].Y, 0.0f), FVector(OutPointList[OutPointList.Num() - 1].X, OutPointList[OutPointList.Num() - 1].Y, 0.0f), FVector(OutPointList[1].X, OutPointList[1].Y, 0.0f));
			if (PointDistance < ALLOWABLE_ERROR)
			{
				DeleteStart = true;
			}
		}
		if (DeleteStart)
		{
			OutPointList.RemoveAt(0);
		}
		if (DeleteEnd)
		{
			OutPointList.RemoveAt(OutPointList.Num() - 1);
		}
	}
	else
		OutPointList = InPointList;

}

void ULuoLuoBlueprintFunctionLibrary::CalculatePolygonIntersectant(TArray<FVector2D> FirstArea, TArray<FVector2D> SecondArea,float & IntersectantArea)
{
	TArray<FVector2D> UpdateFirstArea, UpdateSecondArea;
	for (FVector2D TempFirstP : FirstArea)
	{
		UpdateFirstArea.AddUnique(TempFirstP);
	}

	for (FVector2D TempSecondP : SecondArea)
	{
		UpdateSecondArea.AddUnique(TempSecondP);
	}

	Polygon_2 FirstPolygon, SecondPolygon;
	for (FVector2D Point : UpdateFirstArea)
	{
		FirstPolygon.push_back(Point_2(Point.X, Point.Y));
	}
	for (FVector2D Point : UpdateSecondArea)
	{
		SecondPolygon.push_back(Point_2(Point.X, Point.Y));
	}

	Pwh_list_2	InterPolygon;
	Pwh_list_2::const_iterator it;
	IntersectantArea = 0;
	CGAL::intersection(FirstPolygon, SecondPolygon, std::back_inserter(InterPolygon));
	for (it = InterPolygon.begin(); it != InterPolygon.end(); ++it)
	{
		IntersectantArea += CGAL::to_double(it->outer_boundary().area());
	}
}

bool ULuoLuoBlueprintFunctionLibrary::AreaCombine(TArray<FVector2D> FirstArea, TArray<FVector2D> SecondArea, TArray<FVector2D>& CombineArea)
{
	if (FPolygonAlg::CGPolygonCombine(FirstArea, SecondArea, CombineArea))
		return true;
	else
		return false;
}

bool ULuoLuoBlueprintFunctionLibrary::CheckCuttingLine(const TArray<FVector2D>& InCurrentPoints, const TArray<FVector2D>& InComparisonPoints, const TArray<FVector2D>& InOriginalPoints, TArray<int32> &OutCuttingLine)
{
	if (InCurrentPoints.Num() == 0 || InComparisonPoints.Num() == 0)
	{
		return false;
	}

	TArray<FVector2D> Local_TempArray;

	for (FVector2D CurrentPoint : InCurrentPoints)
	{
		if (CheckPointAtLine(FVector(CurrentPoint, 0.0f), InComparisonPoints))
		{
			Local_TempArray.AddUnique(CurrentPoint);
		}
	}

	if (Local_TempArray.Num() >= 2)
	{
		bool Local_CheckCollineation(false);
		for (int index = 0; index < Local_TempArray.Num(); index++)
		{
			int32 Local_Index(0);
			FVector Local_CurrentPoint(Local_TempArray[index], 0.0f);
			FVector Local_NextPoint(Local_TempArray[0], 0.0f);
			if (index + 1 < Local_TempArray.Num())
			{
				Local_NextPoint = FVector(Local_TempArray[index + 1], 0.0f);
			}

			if (!CheckTwoPointAtCommonLine(Local_CurrentPoint, Local_NextPoint, InOriginalPoints))
			{
				bool Local_Contains(false);
				if (InCurrentPoints.Find(Local_TempArray[index], Local_Index))
				{
					if (Local_Index + 1 < InCurrentPoints.Num())
					{
						Local_Contains = Local_TempArray.Contains(InCurrentPoints[index + 1]);
					}
					else
					{
						Local_Contains = Local_TempArray.Contains(InCurrentPoints[0]);
					}

					if (Local_Contains)
					{
						OutCuttingLine.Add(Local_Index);
					}
				}
			}
		}
	}

	return OutCuttingLine.Num() != 0;
}

bool ULuoLuoBlueprintFunctionLibrary::CheckPolygonCanCombine(TArray<FVector2D> FirstPolygon, TArray<FVector2D> SecondPolygon)
{
	TArray<FLineSegment> FirstSegmentList;
	TArray<FLineSegment> SecondSegmentList;
	for (int PointIndex = 0; PointIndex < FirstPolygon.Num(); PointIndex++)
	{
		FVector2D FirstStart, FirstEnd;
		FirstStart = FirstPolygon[PointIndex];
		if (PointIndex != (FirstPolygon.Num() - 1))
		{
			FirstEnd = FirstPolygon[PointIndex + 1];
		}
		else
		{
			FirstEnd = FirstPolygon[0];
		}
		FirstSegmentList.Add(FLineSegment(FirstStart, FirstEnd));
	}

	for (int PointIndex = 0; PointIndex < SecondPolygon.Num(); PointIndex++)
	{
		FVector2D SecondStart, SecondEnd;
		SecondStart = SecondPolygon[PointIndex];
		if (PointIndex != (SecondPolygon.Num() - 1))
		{
			SecondEnd = SecondPolygon[PointIndex + 1];
		}
		else
		{
			SecondEnd = SecondPolygon[0];
		}
		SecondSegmentList.Add(FLineSegment(SecondStart, SecondEnd));
	}

	for (FLineSegment CurrentFirstSegment : FirstSegmentList)
	{
		for (FLineSegment CurrentSecondSegment : SecondSegmentList)
		{
			bool bCoincide = LineCoincide(CurrentFirstSegment.StartPos, CurrentFirstSegment.EndPos, CurrentSecondSegment.StartPos, CurrentSecondSegment.EndPos);
			if (bCoincide)
			{
				return true;
			}
		}
	}
	return false;
}

CuttingErrorType ULuoLuoBlueprintFunctionLibrary::IsPointOnAreaLine(TArray<FVector2D> AreaPoints, TArray<FVector2D>& CuttingPoints)
{ 
	FVector StartPoint, EndPoint;
	bool FindStartPoint = false;
	bool FindEndPoint = false;

	if(CuttingPoints.Num() < 2)
	{
		return CuttingErrorType::PointDoincident;
	}
	if (CuttingPoints[0] == CuttingPoints[CuttingPoints.Num() - 1])
	{
		//ERROR:分割线起点终点重合
		return CuttingErrorType::PointDoincident;
	}

	for (int32 i = 0; i < AreaPoints.Num(); i++)
	{
		StartPoint = FVector(AreaPoints[i].X, AreaPoints[i].Y, 0);

		if ((i + 1) == AreaPoints.Num())
			EndPoint = FVector(AreaPoints[0].X, AreaPoints[0].Y, 0);
		else
			EndPoint = FVector(AreaPoints[i + 1].X, AreaPoints[i + 1].Y, 0);

		if (!FindStartPoint)
		{
			if (UKismetMathLibrary::GetPointDistanceToSegment(FVector(CuttingPoints[0].X, CuttingPoints[0].Y, 0), StartPoint, EndPoint) <= ALLOWABLE_ERROR)
			{
				FVector StartPointOnSeg = UKismetMathLibrary::FindClosestPointOnSegment(FVector(CuttingPoints[0].X, CuttingPoints[0].Y, 0), StartPoint, EndPoint);
				CuttingPoints[0] = FVector2D(StartPointOnSeg.X, StartPointOnSeg.Y);
				FindStartPoint = true;
			}
		}

		if (!FindEndPoint)
		{
			if (UKismetMathLibrary::GetPointDistanceToSegment(FVector(CuttingPoints[CuttingPoints.Num() - 1].X, CuttingPoints[CuttingPoints.Num() - 1].Y, 0), StartPoint, EndPoint) <= ALLOWABLE_ERROR)
			{
				FVector EndPointOnSeg = UKismetMathLibrary::FindClosestPointOnSegment(FVector(CuttingPoints[CuttingPoints.Num() - 1].X, CuttingPoints[CuttingPoints.Num() - 1].Y, 0), StartPoint, EndPoint);
				CuttingPoints[CuttingPoints.Num() - 1] = FVector2D(EndPointOnSeg.X, EndPointOnSeg.Y);
				FindEndPoint = true;
			}
		}

		if (FindEndPoint && FindEndPoint)
		{
			return CuttingErrorType::Correct;
		}
	}
	////ERROR:分割线起点或终点不在区域边线上
	return CuttingErrorType::PointNotOnSegment;
}

CuttingErrorType ULuoLuoBlueprintFunctionLibrary::IsSegmentIntersectionLegal(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints)
{
	for (int32 i = 0; i < AreaPoints.Num(); i++)
	{
		Segment_2 AreaSeg;
		Segment_2 CuttingSeg;
		Point_2 StartPoint;
		Point_2 EndPoint;
		FVector2D StartVector;
		FVector2D EndVector;

		StartPoint = Point_2(AreaPoints[i].X, AreaPoints[i].Y);
		if ((i + 1) == AreaPoints.Num())
			EndPoint = Point_2(AreaPoints[0].X, AreaPoints[0].Y);
		else
			EndPoint = Point_2(AreaPoints[i + 1].X, AreaPoints[i + 1].Y);

		AreaSeg = Segment_2(StartPoint, EndPoint);
		for (int32 j = 0; j < CuttingPoints.Num() - 1; j++)
		{
			StartVector = FVector2D(CuttingPoints[j].X, CuttingPoints[j].Y);
			EndVector = FVector2D(CuttingPoints[j + 1].X, CuttingPoints[j + 1].Y);
			ShortenSegment(StartVector, EndVector, 0.5);

			StartPoint = Point_2(StartVector.X, StartVector.Y);
			EndPoint = Point_2(EndVector.X, EndVector.Y);
			CuttingSeg = Segment_2(StartPoint, EndPoint);
			if (CGAL::do_intersect(AreaSeg, CuttingSeg))
			{
				CGAL::cpp11::result_of<Intersect_2(Segment_2, Segment_2)>::type result = CGAL::intersection(AreaSeg, CuttingSeg);
				if (result)
				{
					if (const Point_2* p = boost::get<Point_2>(&*result))
					{
						//如果有交点，且交点不是分割线的起点和终点，则不符合规则
						if (!((j == 0) && (p->x() == CuttingPoints[0].X && p->y() == CuttingPoints[0].Y)))
						{
							if (!((j == (CuttingPoints.Num() - 1)) && (p->x() == CuttingPoints[CuttingPoints.Num() - 1].X && p->y() == CuttingPoints[CuttingPoints.Num() - 1].Y)))
							//ERROR:分割线与区域边线相交
							return CuttingErrorType::SegmentCross;
						}
					}
					if (const Segment_2* p = boost::get<Segment_2>(&*result))
					{
						//ERROR:分割线与区域边线重合
						return CuttingErrorType::SegmentOverlap;
					}
				}
			}
		}
	}
	return CuttingErrorType::Correct;
}

CuttingErrorType ULuoLuoBlueprintFunctionLibrary::IsCuttingLineLegal(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints)
{
	if (CuttingPoints.Num() >= 2)
	{
		for (int32 i = 0; i < CuttingPoints.Num() - 2; i++)
		{
			Point_2 StartPoint;
			Point_2 EndPoint;
			Segment_2 CuttingSeg_1;
			Segment_2 CuttingSeg_2;
			FVector2D StartVectot, EndVector;

			StartPoint = Point_2(CuttingPoints[i].X, CuttingPoints[i].Y);
			EndPoint = Point_2(CuttingPoints[i + 1].X, CuttingPoints[i + 1].Y);
			CuttingSeg_1 = Segment_2(StartPoint, EndPoint);
			for (int32 j = i + 1; j < CuttingPoints.Num() - 1; j++)
			{
				StartVectot = CuttingPoints[j];
				EndVector = CuttingPoints[j+1];
				ShortenSegment(StartVectot, EndVector, 0.5);

				StartPoint = Point_2(StartVectot.X, StartVectot.Y);
				EndPoint = Point_2(EndVector.X, EndVector.Y);

				CuttingSeg_2 = Segment_2(StartPoint, EndPoint);
				
				if (CGAL::do_intersect(CuttingSeg_1, CuttingSeg_2))
				{
					CGAL::cpp11::result_of<Intersect_2(Segment_2, Segment_2)>::type result = CGAL::intersection(CuttingSeg_1, CuttingSeg_2);
					if (result)
					{
						if (const Point_2* p = boost::get<Point_2>(&*result))
						{
							if (!((i == 0) && ((p->x() == CuttingPoints[0].X) && (p->y() == CuttingPoints[0].Y))))
							//ERROR:分割线相交
							return CuttingErrorType::SegmentCross;
						}
						if (const Segment_2* p = boost::get<Segment_2>(&*result))
						{
							//ERROR:分割线重合
							return CuttingErrorType::SegmentOverlap;
						}
					}
				}
			}
		}
		return CuttingErrorType::Correct;
	}
	else
		//ERROR:分割线少于2个点
		return CuttingErrorType::ShortOfPoint;
}

void ULuoLuoBlueprintFunctionLibrary::DeleteCoincidentPoint(TArray<FVector2D> & PointList)
{
	TArray<FVector2D> Local_PointList;
	for (int32 i = 0; i < PointList.Num(); i++)
	{
		if (i > 0)
		{
			bool FindEqualsPoint = false;
			for (int32 j = 0; j < Local_PointList.Num(); j++)
			{
				FindEqualsPoint = PointList[i].Equals(Local_PointList[j], ALLOWABLE_ERROR);
				if (FindEqualsPoint)
					break;
			}
			if (!FindEqualsPoint)
			{
				Local_PointList.AddUnique(PointList[i]);
			}
		}
		else
			Local_PointList.AddUnique(PointList[i]);
	}
	PointList = Local_PointList;
}

TArray<int32> ULuoLuoBlueprintFunctionLibrary::GetCuttingLineIndex(TArray<FVector2D> AreaPoints, TArray<FVector2D> CuttingPoints, TArray<FLineSegment> PreviousCuttingLines, bool CheckCuttingPoints)
{
	TArray<int32> CuttingLineIndex;
	for (int32 i = 0; i < AreaPoints.Num(); i++)
	{
		bool FindSegmentStart = false;
		bool FindSegmentEnd = false;
		int32 NextPoint = i + 1;
		FLineSegment CurrentAreaSegment;
		
		if (i == (AreaPoints.Num() - 1))
		{
			NextPoint = 0;
		}

		CurrentAreaSegment = FLineSegment(FVector(AreaPoints[i].X, AreaPoints[i].Y, 0), FVector(AreaPoints[NextPoint].X, AreaPoints[NextPoint].Y, 0));

		if (CheckCuttingPoints)
		{
			for (TArray<FVector2D>::TIterator it = CuttingPoints.CreateIterator(); it; it++)
			{
				if (!FindSegmentStart)
				{
					FindSegmentStart = AreaPoints[i].Equals(*it, 0.01);
				}
				if (!FindSegmentEnd)
				{
					FindSegmentEnd = AreaPoints[NextPoint].Equals(*it, 0.01);
				}

				if (FindSegmentStart && FindSegmentEnd)
				{
					CuttingLineIndex.Add(i);
					break;
				}
			}
		}

		if (!CuttingLineIndex.Contains(i))
		{
			for (TArray<FLineSegment>::TIterator it_Line = PreviousCuttingLines.CreateIterator(); it_Line; it_Line++)
			{
				float StartDistance = UKismetMathLibrary::GetPointDistanceToSegment(CurrentAreaSegment.StartPos, it_Line->StartPos, it_Line->EndPos);
				float EndDistance  = UKismetMathLibrary::GetPointDistanceToSegment(CurrentAreaSegment.EndPos, it_Line->StartPos, it_Line->EndPos);
				if ((StartDistance <= ALLOWABLE_ERROR) && (EndDistance <= ALLOWABLE_ERROR))
				{
					CuttingLineIndex.Add(i);
					break;
				}
			}
		}
	}
	return CuttingLineIndex;
}

void ULuoLuoBlueprintFunctionLibrary::ShortenSegment(FVector2D & StartPoint, FVector2D & EndPoint, float ShortenLen)
{
	FVector2D DirectionVector = (EndPoint - StartPoint).GetSafeNormal();
	StartPoint += DirectionVector * ShortenLen;
	EndPoint -= DirectionVector * ShortenLen;
}

bool ULuoLuoBlueprintFunctionLibrary::CheckPointAtLine(const FVector& InCurrentPoint, const TArray<FVector2D>& InOriginalPoints)
{
	for (int index = 0; index < InOriginalPoints.Num(); index++)
	{
		FVector Local_StartPoint(InOriginalPoints[index], 0.0f);
		FVector Local_EndPoint(InOriginalPoints[0], 0.0f);

		if (index + 1 < InOriginalPoints.Num())
		{
			Local_EndPoint = FVector(InOriginalPoints[index + 1], 0.0f);
		}

		float Local_PointDistance = UKismetMathLibrary::GetPointDistanceToSegment(InCurrentPoint, Local_StartPoint, Local_EndPoint);

		if (Local_PointDistance < ALLOWABLE_ERROR)
		{
			return true;
		}
	}
	return false;
}

bool ULuoLuoBlueprintFunctionLibrary::CheckTwoPointAtCommonLine(const FVector & InCurrentPoint, const FVector & InNextPoints, const TArray<FVector2D>& InOriginalPoints)
{

	for (int index = 0; index < InOriginalPoints.Num(); index++)
	{
		FVector Local_StartPoint(InOriginalPoints[index], 0.0f);
		FVector Local_EndPoint(InOriginalPoints[0], 0.0f);

		if (index + 1 < InOriginalPoints.Num())
		{
			Local_EndPoint = FVector(InOriginalPoints[index + 1], 0.0f);
		}

		float Local_PointDistance = UKismetMathLibrary::GetPointDistanceToSegment(InCurrentPoint, Local_StartPoint, Local_EndPoint);
		float Local_NextPointDistance = UKismetMathLibrary::GetPointDistanceToSegment(InNextPoints, Local_StartPoint, Local_EndPoint);

		if (Local_PointDistance < ALLOWABLE_ERROR && Local_NextPointDistance < ALLOWABLE_ERROR)
		{
			return true;
		}
	}
	return false;
}

bool ULuoLuoBlueprintFunctionLibrary::LineCoincide(FVector FirstStart, FVector FirstEnd, FVector SecondStart, FVector SecondEnd)
{
	bool OnLineFirstStart, OnLineFirstEnd, OnLineSecondStart, OnLineSecondEnd;
	OnLineFirstStart = UKismetMathLibrary::GetPointDistanceToSegment(FirstStart, SecondStart, SecondEnd) < ALLOWABLE_ERROR ? true : false;
	OnLineFirstEnd = UKismetMathLibrary::GetPointDistanceToSegment(FirstEnd, SecondStart, SecondEnd) < ALLOWABLE_ERROR ? true : false;
	OnLineSecondStart = UKismetMathLibrary::GetPointDistanceToSegment(SecondStart, FirstStart, FirstEnd) < ALLOWABLE_ERROR ? true : false;
	OnLineSecondEnd = UKismetMathLibrary::GetPointDistanceToSegment(SecondEnd, FirstStart, FirstEnd) < ALLOWABLE_ERROR ? true : false;

	bool FirstCoincide = (OnLineFirstStart && OnLineFirstEnd) || (OnLineSecondStart && OnLineSecondEnd);
	bool SecondCoincide = (OnLineFirstStart && OnLineSecondStart && (FirstStart != SecondStart))
		|| (OnLineFirstStart && OnLineSecondEnd && (FirstStart != SecondEnd))
		|| (OnLineFirstEnd && OnLineSecondStart && (FirstEnd != SecondStart))
		|| (OnLineFirstEnd && OnLineSecondEnd && (FirstEnd != SecondEnd));
	return (FirstCoincide || SecondCoincide);
}

TArray<FVector> ULuoLuoBlueprintFunctionLibrary::SplitLineWithSpace(const FVector & StartPos, const FVector & EndPos, const TArray<FVector2D>& AreaPoints, float Space, float InvalidSpace, int32 LimitNum, bool IsFlatLight)
{
	TArray<FVector> Result;

	if (IsFlatLight)
	{
		FVector2D SuitableCenter;
		FPolygonAlg::LocateSuitableCenter(AreaPoints, SuitableCenter);
		Result.Add(FVector(SuitableCenter.X, SuitableCenter.Y, StartPos.Z));
	}
	else
	{
		FLineSegment LineSegment(StartPos, EndPos);

		if (LineSegment.Length() < Space || LineSegment.Length() - (InvalidSpace * 2) < Space)
		{
			Result.Add(LineSegment.Mid());
		}
		else
		{
			int32 SplitPointNum = (LineSegment.Length() - (InvalidSpace * 2)) / Space + 1;

			if (SplitPointNum > 1 && LimitNum > 0)
			{
				SplitPointNum = LimitNum > SplitPointNum ? SplitPointNum : LimitNum;
			}

			FVector FirstPoint;

			if (SplitPointNum % 2 == 0)
			{
				int32 HalfNum = SplitPointNum / 2;

				FirstPoint = LineSegment.Mid() + LineSegment.GetDirection() * (((HalfNum * Space) - (Space / 2)) * -1);
			}
			else
			{
				int32 HalfNum = (SplitPointNum - 1) / 2;
				FirstPoint = LineSegment.Mid() + LineSegment.GetDirection() * ((HalfNum * Space) * -1);
			}

			for (int32 i = 0; i < SplitPointNum; i++)
			{
				Result.Add(FirstPoint + (LineSegment.GetDirection() * (i * Space)));
			}
		}
	}
	return Result;
}

int32 ULuoLuoBlueprintFunctionLibrary::GetLightLengthFromSpace(const FVector & StartPos, const FVector & EndPos, float Space, float InvalidSpace, int32 LimitNum)
{
	TArray<FVector> Result;

	FLineSegment LineSegment(StartPos, EndPos);

	int32 SplitPointNum;

	if (LineSegment.Length() < Space || LineSegment.Length() - (InvalidSpace * 2) < Space)
	{
		Result.Add(LineSegment.Mid());
		SplitPointNum = Result.Num();
	}
	else
	{
		SplitPointNum = (LineSegment.Length() - (InvalidSpace * 2)) / Space + 1;

		if (SplitPointNum > 1 && LimitNum > 0)
		{
			SplitPointNum = LimitNum > SplitPointNum ? SplitPointNum : LimitNum;
		}
	}

	return SplitPointNum;
}

void ULuoLuoBlueprintFunctionLibrary::CaculateMaxiumLine(const TArray<FVector>& AreaPoints, FVector & StartPos, FVector & EndPos)
{
	if (AreaPoints.Num() < 3)
	{
		StartPos = FVector::ZeroVector;
		EndPos = FVector::ZeroVector;
	}
	else
	{
		float MaxiumLength = 0.0f;

		FVector ResultStart, ResultEnd;

		for (int32 i = 0; i < AreaPoints.Num(); i++)
		{
			float TempLength = 0.0f;

			if (i + 1 == AreaPoints.Num())
			{
				TempLength = (AreaPoints[i] - AreaPoints[0]).Size();

				if (MaxiumLength < TempLength)
				{
					MaxiumLength = TempLength;
					ResultStart = AreaPoints[i];
					ResultEnd = AreaPoints[0];
				}
			}
			else
			{
				TempLength = (AreaPoints[i] - AreaPoints[i + 1]).Size();

				if (MaxiumLength < TempLength)
				{
					MaxiumLength = TempLength;
					ResultStart = AreaPoints[i];
					ResultEnd = AreaPoints[i + 1];
				}
			}
		}
		StartPos = ResultStart;
		EndPos = ResultEnd;
	}
}

void ULuoLuoBlueprintFunctionLibrary::CaculateMinimumLine(const TArray<FVector>& AreaPoints, FVector & StartPos, FVector & EndPos)
{

	if (AreaPoints.Num() < 3)
	{
		StartPos = FVector::ZeroVector;
		EndPos = FVector::ZeroVector;
	}
	else
	{
		float MinimumLength = MAX_FLT;

		FVector ResultStart, ResultEnd;

		for (int32 i = 0; i < AreaPoints.Num(); i++)
		{
			float TempLength = 0.0f;

			if (i + 1 == AreaPoints.Num())
			{
				TempLength = (AreaPoints[i] - AreaPoints[0]).Size();

				if (MinimumLength > TempLength)
				{
					MinimumLength = TempLength;
					ResultStart = AreaPoints[i];
					ResultEnd = AreaPoints[0];
				}
			}
			else
			{
				TempLength = (AreaPoints[i] - AreaPoints[i + 1]).Size();

				if (MinimumLength > TempLength)
				{
					MinimumLength = TempLength;
					ResultStart = AreaPoints[i];
					ResultEnd = AreaPoints[i + 1];
				}
			}
		}
		StartPos = ResultStart;
		EndPos = ResultEnd;
	}
}

void ULuoLuoBlueprintFunctionLibrary::CaculateMaxiumLineSegment(const TArray<FLineSegment>& AreaLineSegments, FLineSegment & Outer)
{
	FLineSegment ResultLineSegment;

	for (int32 i = 0; i < AreaLineSegments.Num(); i++)
	{
		if (ResultLineSegment < AreaLineSegments[i])
		{
			ResultLineSegment = AreaLineSegments[i];
		}
	}

	Outer = ResultLineSegment;
}

void ULuoLuoBlueprintFunctionLibrary::CaculateMinimumLineSegment(const TArray<FLineSegment>& AreaLineSegments, FLineSegment & Outer)
{
	FLineSegment ResultLineSegment;

	if (AreaLineSegments.IsValidIndex(0))
	{
		ResultLineSegment = AreaLineSegments[0];
	}

	for (int32 i = 0; i < AreaLineSegments.Num(); i++)
	{
		if (ResultLineSegment > AreaLineSegments[i])
		{
			ResultLineSegment = AreaLineSegments[i];
		}
	}

	Outer = ResultLineSegment;
}

TArray<FLineSegment> ULuoLuoBlueprintFunctionLibrary::MakeSegmentsFromPoints(const TArray<FVector>& AreaPoints)
{
	TArray<FLineSegment> Result;

	if (AreaPoints.Num() > 1)
	{
		for (int32 i = 0; i < AreaPoints.Num(); i++)
		{
			FLineSegment TempSegment;

			if (i + 1 == AreaPoints.Num())
			{
				TempSegment.Init(AreaPoints[i], AreaPoints[0]);
				Result.Add(TempSegment);
			}
			else
			{
				TempSegment.Init(AreaPoints[i], AreaPoints[i + 1]);
				Result.Add(TempSegment);
			}
		}
	}

	return Result;
}

TArray<FVector2D> ULuoLuoBlueprintFunctionLibrary::ConvertVectorArrayToVector2D(const TArray<FVector>& Points)
{
	TArray<FVector2D> Result;

	for (int32 i = 0; i < Points.Num(); i++)
	{
		Result.Add(FVector2D(Points[i].X, Points[i].Y));
	}

	return Result;
}

TArray<FVector> ULuoLuoBlueprintFunctionLibrary::Get3DPoints(const TArray<FLineSegment>& Segments)
{
	TArray<FVector> Result;

	for (int32 i = 0; i < Segments.Num(); i++)
	{
		Result.Add(Segments[i].StartPos);
	}

	return Result;
}

TArray<FVector2D> ULuoLuoBlueprintFunctionLibrary::Get2DPoints(const TArray<FLineSegment>& Segments)
{
	TArray<FVector> SourcePoints;

	SourcePoints = Get3DPoints(Segments);

	TArray<FVector2D> Result;

	for (int32 i = 0; i < SourcePoints.Num(); i++)
	{
		Result.Add(FVector2D(SourcePoints[i].X, SourcePoints[i].Y));
	}

	return Result;
}

bool ULuoLuoBlueprintFunctionLibrary::CheckSegmentAngleTolerance(const FLineSegment & Origin, const FLineSegment & Target, float Tolerance)
{
	return Origin.CheckAngleTolerance(Target, Tolerance);
}

TArray<FLineSegment> ULuoLuoBlueprintFunctionLibrary::CleanLineSegmentsByRule(const TArray<FLineSegment>& AreaLineSegments, float Tolerance)
{
	FLineSegment OriginLineSegment;

	TArray<FLineSegment> TempLineSegments = AreaLineSegments;

	if (TempLineSegments.IsValidIndex(0))
	{
		CaculateMaxiumLineSegment(TempLineSegments, OriginLineSegment);

		TempLineSegments.RemoveAt(TempLineSegments.Find(OriginLineSegment));

		TArray<FLineSegment>::TConstIterator It = TempLineSegments.CreateConstIterator();

		while (It)
		{
			if (OriginLineSegment.CheckAngleTolerance(*It, Tolerance))
			{
				TempLineSegments.RemoveAt(It.GetIndex());
				It--;
			}
			else
			{
				It++;
			}
		}
	}

	return TempLineSegments;
}

float ULuoLuoBlueprintFunctionLibrary::GetLineSegmentLength(const FLineSegment & Target)
{
	return Target.Length();
}

FVector ULuoLuoBlueprintFunctionLibrary::GetLineSegmentDirection(const FLineSegment & Target)
{
	return Target.GetDirection();
}

FVector ULuoLuoBlueprintFunctionLibrary::GetLineSegmentMidLocation(const FLineSegment & Target)
{
	return Target.Mid();
}
