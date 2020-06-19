#pragma once

#include "PeExtendedToolKitBPLibrary.h"
#include "PeExtendedToolKitPrivatePCH.h" 
#include <string>
using namespace std;

#include "Windows/AllowWindowsPlatformTypes.h"
#include<windows.h>
#include "shellapi.h"
#include "CommDlg.h"
#include "Shlobj.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"

#include "PlatformFeatures.h"
#include "GameFramework/SaveGame.h"

#include "Misc/CoreMisc.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "Kismet/KismetTextLibrary.h"
#include "Internationalization/TextFormatter.h"

#include "Types/ReflectionMetadata.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"

//#include "DesktopPlatformModule.h"
#include "Engine/Console.h"

#include "Blueprint/WidgetLayoutLibrary.h"



#include <chrono>
#include <random>
#include "Windows/WindowsWindow.h"
#include "ProceduralMeshComponent.h"
#include "Sound/SoundWave.h"
#include "AudioDevice.h"
#include "Runtime/Engine/Public/VorbisAudioInfo.h"
#include "Developer/TargetPlatform/Public/Interfaces/IAudioFormat.h"

#include "Runtime/AssetRegistry/Public/IAssetRegistry.h"
#include "Runtime/AssetRegistry/Private/AssetRegistry.h"
#include "Misc/FileHelper.h"


#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) //按键是否按下
#define LOCTEXT_NAMESPACE "UMG"

#define MAX_FILETYPES_STR 4096
#define MAX_FILENAME_STR 65536

UPeExtendedToolKitBPLibrary::UPeExtendedToolKitBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


static const int UE4_SAVEGAME_FILE_TYPE_TAG = 0x53415647;		// "sAvG"

struct FSaveGameFileVersion
{
	enum Type
	{
		InitialVersion = 1,
		// serializing custom versions into the savegame data to handle that type of versioning
		AddedCustomVersions = 2,

		// -----<new versions can be added above this line>-------------------------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};
};


#define USE_WINDOWS 0

#if USE_WINDOWS
#include <windows.h>
#include <shellapi.h>
#endif


void UPeExtendedToolKitBPLibrary::openFolder(FString foldername)
{
#if USE_WINDOWS
	FString name = TEXT("Explorer ");

	name.Append(foldername);
	FString str("/");
	const TCHAR* c1 = *str;
	FString str2("\\");
	const TCHAR* c2 = *str2;
	FString name2 = name.Replace(c1, c2);
	system(TCHAR_TO_UTF8(*name2));
#endif
};

void UPeExtendedToolKitBPLibrary::openUrl(FString url)
{
#if USE_WINDOWS
	//ShellExecute(NULL, "open", "cmd.exe", "start chrome.exe", NULL, SW_SHOWDEFAULT);
	FString name = TEXT("start chrome.exe ");
	name.Append(url);
	system(TCHAR_TO_UTF8(*name));
#endif
};

//void UPeExtendedToolKitBPLibrary::RunFile(FString filePath, EDisplayMode windowMode)
//{
//#if USE_WINDOWS
//	//HINSTANCE hNewExe = ShellExecuteA(NULL, "open", "d:\\tese.log", NULL, NULL, SW_HIDE);
//	HINSTANCE hNewExe = ShellExecuteA(NULL, "open", TCHAR_TO_UTF8(*filePath), NULL, NULL, windowMode);
//#endif
//};

bool UPeExtendedToolKitBPLibrary::GetActorTagKey(AActor * InActor, FName InKey, FName & OutValue)
{
	if (InActor == NULL) return false;
	if (!InActor->ActorHasTag(InKey))  return false;
	if ((InActor->Tags.IndexOfByKey(InKey) + 1)>(InActor->Tags.Num() - 1)) return false;
	OutValue = InActor->Tags[InActor->Tags.IndexOfByKey(InKey) + 1];
	return true;
}

bool UPeExtendedToolKitBPLibrary::SetActorTagKey(AActor * InActor, FName InKey, FName InValue)
{
	if (InActor == NULL) return false;
	if (!InActor->ActorHasTag(InKey))  return false;
	if ((InActor->Tags.IndexOfByKey(InKey) + 1)>(InActor->Tags.Num() - 1)) return false;
	InActor->Tags[InActor->Tags.IndexOfByKey(InKey) + 1] = InValue;
	if (!InActor->ActorHasTag(InValue))  return false;
	return true;
}

bool UPeExtendedToolKitBPLibrary::AddActorTagKey(AActor * InActor, FName InKey, FName InValue)
{
	if (InActor == NULL) return false;
	if (InActor->ActorHasTag(InKey))  return false;
	if (InActor->Tags.Num() < 2 && InActor->Tags.Num() != 0) return false;
	if (InActor->Tags.Num() > 2 && InActor->Tags.Num() % 2 != 0) return false;
	InActor->Tags.Add(InKey);
	InActor->Tags.Add(InValue);
	if (!InActor->ActorHasTag(InKey))  return false;
	if ((InActor->Tags.IndexOfByKey(InKey) + 1)>(InActor->Tags.Num() - 1)) return false;
	if (InActor->Tags[InActor->Tags.IndexOfByKey(InKey) + 1] != InValue) return false;
	return true;
}

bool UPeExtendedToolKitBPLibrary::DeleteActorTagKey(AActor * InActor, FName InKey)
{
	if (InActor == NULL) return false;
	if (!InActor->ActorHasTag(InKey))  return true;
	if (InActor->Tags.Num() < 2 && InActor->Tags.Num() != 0) return false;
	if (InActor->Tags.Num() > 2 && InActor->Tags.Num() % 2 != 0) return false;
	if ((InActor->Tags.IndexOfByKey(InKey) + 1)>(InActor->Tags.Num() - 1)) return false;
	InActor->Tags.RemoveAt(InActor->Tags.IndexOfByKey(InKey), 2);
	if (!InActor->ActorHasTag(InKey))  return true;
	else return false;
	return true;
}

bool UPeExtendedToolKitBPLibrary::ClearActorTagKey(AActor * InActor)
{
	if (InActor == NULL) return false;
	InActor->Tags.RemoveAt(0, InActor->Tags.Num());
	return true;
}

bool UPeExtendedToolKitBPLibrary::GetClassTagKey(TSubclassOf<AActor> InClass, FName InKey, FName & OutValue)
{
	if (InClass.GetDefaultObject() == NULL) return false;
	if (!InClass.GetDefaultObject()->ActorHasTag(InKey))  return false;
	if ((InClass.GetDefaultObject()->Tags.IndexOfByKey(InKey) + 1)>(InClass.GetDefaultObject()->Tags.Num() - 1)) return false;
	OutValue = InClass.GetDefaultObject()->Tags[InClass.GetDefaultObject()->Tags.IndexOfByKey(InKey) + 1];
	return true;
}

bool UPeExtendedToolKitBPLibrary::SetClassTagKey(TSubclassOf<AActor> InClass, FName InKey, FName InValue)
{
	if (InClass.GetDefaultObject() == NULL) return false;
	if (!InClass.GetDefaultObject()->ActorHasTag(InKey))  return false;
	if ((InClass.GetDefaultObject()->Tags.IndexOfByKey(InKey) + 1)>(InClass.GetDefaultObject()->Tags.Num() - 1)) return false;
	InClass.GetDefaultObject()->Tags[InClass.GetDefaultObject()->Tags.IndexOfByKey(InKey) + 1] = InValue;
	if (!InClass.GetDefaultObject()->ActorHasTag(InValue))  return false;
	return true;
}

bool UPeExtendedToolKitBPLibrary::AddClassTagKey(TSubclassOf<AActor> InClass, FName InKey, FName InValue)
{
	if (InClass.GetDefaultObject() == NULL) return false;
	if (InClass.GetDefaultObject()->ActorHasTag(InKey))  return false;
	if (InClass.GetDefaultObject()->Tags.Num() < 2 && InClass.GetDefaultObject()->Tags.Num() != 0) return false;
	if (InClass.GetDefaultObject()->Tags.Num() > 2 && InClass.GetDefaultObject()->Tags.Num() % 2 != 0) return false;
	InClass.GetDefaultObject()->Tags.Add(InKey);
	InClass.GetDefaultObject()->Tags.Add(InValue);
	if (!InClass.GetDefaultObject()->ActorHasTag(InKey))  return false;
	if ((InClass.GetDefaultObject()->Tags.IndexOfByKey(InKey) + 1)>(InClass.GetDefaultObject()->Tags.Num() - 1)) return false;
	if (InClass.GetDefaultObject()->Tags[InClass.GetDefaultObject()->Tags.IndexOfByKey(InKey) + 1] != InValue) return false;
	return true;
}

bool UPeExtendedToolKitBPLibrary::DeleteClassTagKey(TSubclassOf<AActor> InClass, FName InKey)
{
	if (InClass.GetDefaultObject() == NULL) return false;
	if (!InClass.GetDefaultObject()->ActorHasTag(InKey))  return true;
	if (InClass.GetDefaultObject()->Tags.Num() < 2 && InClass.GetDefaultObject()->Tags.Num() != 0) return false;
	if (InClass.GetDefaultObject()->Tags.Num() > 2 && InClass.GetDefaultObject()->Tags.Num() % 2 != 0) return false;
	if ((InClass.GetDefaultObject()->Tags.IndexOfByKey(InKey) + 1)>(InClass.GetDefaultObject()->Tags.Num() - 1)) return false;
	InClass.GetDefaultObject()->Tags.RemoveAt(InClass.GetDefaultObject()->Tags.IndexOfByKey(InKey), 2);
	if (!InClass.GetDefaultObject()->ActorHasTag(InKey))  return true;
	else return false;
	return true;
}

bool UPeExtendedToolKitBPLibrary::ClearClassTagKey(TSubclassOf<AActor> InClass)
{
	if (InClass.GetDefaultObject() == NULL) return false;
	InClass.GetDefaultObject()->Tags.RemoveAt(0, InClass.GetDefaultObject()->Tags.Num());
	return true;
}


//获取Root路径
FString UPeExtendedToolKitBPLibrary::GetRootPath()
{
	return FPaths::RootDir();
}

//获取引擎路径
FString UPeExtendedToolKitBPLibrary::GetEnginePath(bool Full)
{
	if (Full) return FPaths::ConvertRelativePathToFull(FPaths::EngineDir());
	return FPaths::EngineDir();
}

//获取游戏路径
FString UPeExtendedToolKitBPLibrary::GetGamePath(bool Full)
{
	if (Full) return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	return FPaths::ProjectDir();
}

//获取游戏路径
FString UPeExtendedToolKitBPLibrary::GetGameDataPath(bool Full)
{
	if (Full) return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()+"Data/");
	return FPaths::ProjectDir()+"Data/";
}

//获取游戏存档路径
FString UPeExtendedToolKitBPLibrary::GetGameSavePath(bool Full)
{
	if (Full) return FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	return FPaths::ProjectSavedDir();
}

//获取游戏截图路径
FString UPeExtendedToolKitBPLibrary::GetGameScreenShotPath(bool Full)
{
	if (Full) return FPaths::ConvertRelativePathToFull(FPaths::ScreenShotDir());
	return FPaths::ScreenShotDir();
}

//归递查找文件
TArray<FString> UPeExtendedToolKitBPLibrary::File_Find(FString InPath, FString InFilter, bool InFiles, bool InDirectory)
{
	TArray<FString> Result;
	Result.Empty();
	FFileManagerGeneric::Get().FindFilesRecursive(Result, *InPath, *InFilter, InFiles, InDirectory);
	return Result;
}
//查找文件
TArray<FString> UPeExtendedToolKitBPLibrary::SearchFile(FString InPath, FString InFilter)
{
	TArray<FString> Result;
	Result.Empty();
	FFileManagerGeneric::Get().FindFiles(Result, *InPath, *InFilter);
	return Result;
}

//删除文件
bool UPeExtendedToolKitBPLibrary::File_Delete(FString InPath)
{
	return FFileManagerGeneric::Get().Delete(*InPath);
}

//删除目录
bool UPeExtendedToolKitBPLibrary::Dir_Delete(FString InPath)
{
	return FFileManagerGeneric::Get().DeleteDirectory(*InPath);
}

//创建目录
bool UPeExtendedToolKitBPLibrary::Dir_Create(FString InPath)
{
	return FFileManagerGeneric::Get().MakeDirectory(*InPath);
}

//判断文件是否已存在
bool UPeExtendedToolKitBPLibrary::File_IsExists(FString InPath)
{
	return FFileManagerGeneric::Get().FileExists(*InPath);
}

//判断目录是否存在
bool UPeExtendedToolKitBPLibrary::Dir_IsExists(FString InPath)
{
	return FFileManagerGeneric::Get().DirectoryExists(*InPath);
}


//复制
int32 UPeExtendedToolKitBPLibrary::File_Copy(FString InFrom, FString InTo)
{
	return FFileManagerGeneric::Get().Copy(*InFrom, *InTo);
}

//移动文件
int32 UPeExtendedToolKitBPLibrary::File_Move(FString InFrom, FString InTo)
{
	return IFileManager::Get().Move(*InTo, *InFrom);
}




//GetImageWrapperByExtention
static TSharedPtr<IImageWrapper> GetImageWrapperByExtention(const FString InImagePath)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (InImagePath.EndsWith(".png"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if (InImagePath.EndsWith(".jpg") || InImagePath.EndsWith(".jpeg"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else if (InImagePath.EndsWith(".bmp"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	}
	else if (InImagePath.EndsWith(".ico"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO);
	}
	else if (InImagePath.EndsWith(".exr"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	}
	else if (InImagePath.EndsWith(".icns"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS);
	}

	return nullptr;
}

//载入图像
UTexture2D* UPeExtendedToolKitBPLibrary::LoadTexture2D(const FString& ImagePath, bool& IsValid, int32& OutWidth, int32& OutHeight)
{
	UTexture2D* Texture = nullptr;
	IsValid = false;

	// To avoid log spam, make sure it exists before doing anything else.
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ImagePath))
	{
		return nullptr;
	}

	TArray<uint8> CompressedData;
	if (!FFileHelper::LoadFileToArray(CompressedData, *ImagePath))
	{
		return nullptr;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtention(ImagePath);

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(CompressedData.GetData(), CompressedData.Num()))
	{
		TArray<uint8> UncompressedRGBA;

		if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);

			if (Texture != nullptr)
			{
				IsValid = true;

				OutWidth = ImageWrapper->GetWidth();
				OutHeight = ImageWrapper->GetHeight();

				void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
				Texture->PlatformData->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}

	return Texture;
}



UTexture2D* UPeExtendedToolKitBPLibrary::ColorToTexture2D(TArray<FLinearColor> ColorArray, int32 Width, int32 Height, bool Transparent, bool Invert)
{
	TArray<uint8>UncompressedBGRA; //图像信息

	for (int32 i = 0; i < ColorArray.Num(); i++)
	{
		if (Invert)
		{

			UncompressedBGRA.Add((uint8)FMath::TruncToInt(ColorArray[i].B * 255));
			UncompressedBGRA.Add((uint8)FMath::TruncToInt(ColorArray[i].G * 255));
			UncompressedBGRA.Add((uint8)FMath::TruncToInt(ColorArray[i].R * 255));
		}
		else
		{
			UncompressedBGRA.Add(255 - (uint8)FMath::TruncToInt(ColorArray[i].B * 255));
			UncompressedBGRA.Add(255 - (uint8)FMath::TruncToInt(ColorArray[i].G * 255));
			UncompressedBGRA.Add(255 - (uint8)FMath::TruncToInt(ColorArray[i].R * 255));
		}
		if (Transparent)
			UncompressedBGRA.Add((uint8)FMath::TruncToInt(ColorArray[i].A * 255));
		else

			UncompressedBGRA.Add(255);

	}



	UTexture2D* LoadedT2D = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);   //创建一个Texture2D

																					   //Texture2D是否有效
	if (!LoadedT2D)
	{
		return NULL; //Texture2D是否有效
	}
	void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num()); //设置数据到texure
	LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();//解锁
	LoadedT2D->UpdateResource();//更新texture

	return LoadedT2D;

}



UTexture2D* UPeExtendedToolKitBPLibrary::ByteToTexture2D(TArray<uint8> ByteArray, int32 Width, int32 Height)
{
	TArray<uint8>UncompressedBGRA = ByteArray; //图像信息

	UTexture2D* LoadedT2D = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);   //创建一个Texture2D

																					   //Texture2D是否有效
	if (!LoadedT2D)
	{
		return NULL; //Texture2D是否有效
	}
	void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num()); //设置数据到texure
	LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();//解锁
	LoadedT2D->UpdateResource();//更新texture

	return LoadedT2D;

}

FString ByteToString(uint8 InByte)
{
	return FString::Printf(TEXT("%d"), InByte);
}
FString StrStr(const FString& A, const FString& B)
{
	// faster, preallocating method
	FString StringResult;
	StringResult.Empty(A.Len() + B.Len() + 1); // adding one for the string terminator
	StringResult += A;
	StringResult += B;

	return StringResult;
}
FString UPeExtendedToolKitBPLibrary::ByteArrayToString(TArray<uint8>Byte)
{
	FString Text;

	for (int32 v = 0; v < Byte.Num(); v++)
	{
		//Text=StrStr(Text, ByteToString(Byte[v]));
		Text = Text + ByteToString(Byte[v]);
	}
	return Text;

}





TArray<uint8>UPeExtendedToolKitBPLibrary::LoadTexture2DToByte(const FString& FullFilePath, int32& Width, int32& Height)
{
	TArray<uint8> N;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FullFilePath))
	{
		return N;
	}

	TArray<uint8> CompressedData;
	if (!FFileHelper::LoadFileToArray(CompressedData, *FullFilePath))
	{
		return N;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtention(FullFilePath);

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(CompressedData.GetData(), CompressedData.Num()))
	{
		TArray<uint8> UncompressedRGBA;

		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedRGBA))
		{
			Width = ImageWrapper->GetWidth();//设置输出宽度
			Height = ImageWrapper->GetHeight();//设置输出高度
			return UncompressedRGBA;
		}
	}
	return N;
}

//this is how you can make cpp only internal functions!
static EImageFormat GetJoyImageFormat(EPEImageFormats Format)
{
	switch (Format)
	{
	case EPEImageFormats::JPG: return EImageFormat::JPEG;
	case EPEImageFormats::PNG: return EImageFormat::PNG;
	case EPEImageFormats::BMP: return EImageFormat::BMP;
	case EPEImageFormats::ICO: return EImageFormat::ICO;
	case EPEImageFormats::EXR: return EImageFormat::EXR;
	case EPEImageFormats::ICNS: return EImageFormat::ICNS;
	}
	return EImageFormat::JPEG;
}

static bool FolderExists(const FString& Dir)
{
	return FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*Dir);
}


static bool VCreateDirectory(FString FolderToMake) //not const so split can be used, and does not damage input
{
	if (FolderExists(FolderToMake))
	{
		return true;
	}

	// Normalize all / and \ to TEXT("/") and remove any trailing TEXT("/") if the character before that is not a TEXT("/") or a colon
	FPaths::NormalizeDirectoryName(FolderToMake);

	//Normalize removes the last "/", but is needed by algorithm
	//  Guarantees While loop will end in a timely fashion.
	FolderToMake += "/";

	FString Base;
	FString Left;
	FString Remaining;

	//Split off the Root
	FolderToMake.Split(TEXT("/"), &Base, &Remaining);
	Base += "/"; //add root text and Split Text to Base

	while (Remaining != "")
	{
		Remaining.Split(TEXT("/"), &Left, &Remaining);

		//Add to the Base
		Base += Left + FString("/"); //add left and split text to Base

									 //Create Incremental Directory Structure!
		if (!FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*Base) &&
			!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*Base))
		{
			return false;
			//~~~~~
		}
	}

	return true;
}


bool UPeExtendedToolKitBPLibrary::SavePixels(const FString& FullFilePath, int32 Width, int32 Height, const TArray<FLinearColor>& ImagePixels, FString& ErrorString)
{
	if (FullFilePath.Len() < 1)
	{
		ErrorString = "No file path";
		return false;
	}
	//~~~~~~~~~~~~~~~~~

	//Ensure target directory exists, 
	//		_or can be created!_ <3 Rama
	FString NewAbsoluteFolderPath = FPaths::GetPath(FullFilePath);
	FPaths::NormalizeDirectoryName(NewAbsoluteFolderPath);
	if (!VCreateDirectory(NewAbsoluteFolderPath))
	{
		ErrorString = "Folder could not be created, check read/write permissions~ " + NewAbsoluteFolderPath;
		return false;
	}

	//Create FColor version
	TArray<FColor> ColorArray;
	for (const FLinearColor& Each : ImagePixels)
	{
		ColorArray.Add(Each.ToFColor(true));
	}

	if (ColorArray.Num() != Width * Height)
	{
		ErrorString = "Error ~ height x width is not equal to the total pixel array length!";
		return false;
	}

	//Remove any supplied file extension and/or add accurate one
	FString FinalFilename = FPaths::GetBaseFilename(FullFilePath, false) + ".png";  //false = dont remove path

																					//~~~

	TArray<uint8> CompressedPNG;
	FImageUtils::CompressImageArray(
		Width,
		Height,
		ColorArray,
		CompressedPNG
		);

	ErrorString = "Success! or if returning false, the saving of file to disk did not succeed for File IO reasons";
	return FFileHelper::SaveArrayToFile(CompressedPNG, *FinalFilename);

}

bool UPeExtendedToolKitBPLibrary::GetPixelFromT2D(UTexture2D* T2D, int32 X, int32 Y, FLinearColor& PixelColor)
{
	if (!T2D)
	{
		return false;
	}

	if (X <= -1 || Y <= -1)
	{
		return false;
	}

	T2D->SRGB = false;
	T2D->CompressionSettings = TC_VectorDisplacementmap;

	//Update settings
	T2D->UpdateResource();

	FTexture2DMipMap& MipsMap = T2D->PlatformData->Mips[0];
	int32 TextureWidth = MipsMap.SizeX;
	int32 TextureHeight = MipsMap.SizeY;

	FByteBulkData* RawImageData = &MipsMap.BulkData;

	if (!RawImageData)
	{
		return false;
	}

	FColor* RawColorArray = reinterpret_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));

	//Safety check!
	if (X >= TextureWidth || Y >= TextureHeight)
	{
		return false;
	}

	//Get!, converting FColor to FLinearColor 
	PixelColor = RawColorArray[Y * TextureWidth + X];

	RawImageData->Unlock();
	return true;
}

bool UPeExtendedToolKitBPLibrary::GetPixelsArrayFromT2D(UTexture2D* T2D, int32& TextureWidth, int32& TextureHeight, TArray<FLinearColor>& PixelArray)
{
	if (!T2D)
	{
		return false;
	}

	//To prevent overflow in BP if used in a loop
	PixelArray.Empty();

	T2D->SRGB = false;
	T2D->CompressionSettings = TC_VectorDisplacementmap;

	//Update settings
	T2D->UpdateResource();

	FTexture2DMipMap& MyMipMap = T2D->PlatformData->Mips[0];
	TextureWidth = MyMipMap.SizeX;
	TextureHeight = MyMipMap.SizeY;

	FByteBulkData* RawImageData = &MyMipMap.BulkData;

	if (!RawImageData)
	{
		return false;
	}

	FColor* RawColorArray = reinterpret_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));

	for (int32 x = 0; x < TextureWidth; x++)
	{
		for (int32 y = 0; y < TextureHeight; y++)
		{
			PixelArray.Add(RawColorArray[x * TextureWidth + y]);
		}
	}

	RawImageData->Unlock();
	return true;
}


UTexture2D* UPeExtendedToolKitBPLibrary::LoadTexture2D_From_File(const FString& FullFilePath, EPEImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height)
{


	IsValid = false;
	UTexture2D* LoadedT2D = NULL;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(GetJoyImageFormat(ImageFormat));

	//Load From File
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath)) return NULL;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//Create T2D!
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			//Valid?
			if (!LoadedT2D) return NULL;
			//~~~~~~~~~~~~~~

			//Out!
			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			//Copy!
			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			//Update!
			LoadedT2D->UpdateResource();
		}
	}

	// Success!
	IsValid = true;
	return LoadedT2D;
}
UTexture2D* UPeExtendedToolKitBPLibrary::LoadTexture2D_From_File_Pixels(const FString& FullFilePath, EPEImageFormats ImageFormat, bool& IsValid, int32& Width, int32& Height, TArray<FLinearColor>& OutPixels)
{
	//Clear any previous data
	OutPixels.Empty();

	IsValid = false;
	UTexture2D* LoadedT2D = NULL;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(GetJoyImageFormat(ImageFormat));

	//Load From File
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath)) return NULL;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//Create T2D!
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray<uint8> UncompressedRGBA;
		if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);

			//Valid?
			if (!LoadedT2D) return NULL;
			//~~~~~~~~~~~~~~

			//Out!
			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			const TArray<uint8>& ByteArray = UncompressedRGBA;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			for (int32 v = 0; v < ByteArray.Num(); v += 4)
			{
				if (!ByteArray.IsValidIndex(v + 3))
				{
					break;
				}

				OutPixels.Add(
					FLinearColor(
						ByteArray[v],		//R
						ByteArray[v + 1],		//G
						ByteArray[v + 2],		//B
						ByteArray[v + 3] 		//A
						)
					);
			}
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			//Copy!
			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			//Update!
			LoadedT2D->UpdateResource();
		}
	}

	// Success!
	IsValid = true;
	return LoadedT2D;

}







//保存文本文件
bool UPeExtendedToolKitBPLibrary::SaveStringTextToFile(FString SaveDirectory,FString FileName,FString SaveText,bool AllowOverWriting) {
	//目录存在？
	if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDirectory))
	{
		//如果不存在，创建目录
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*SaveDirectory);

		//仍然不能创建目录？
		if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDirectory))
		{
			//无法使用指定的目录
			return false;
			//返回假
		}
	}

	//获得完整的文件路径
	SaveDirectory += "\\";
	SaveDirectory += FileName;

	//是否掩盖保存？
	if (!AllowOverWriting)
	{
		//检查文件是否已经存在
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*SaveDirectory))
		{
			
			return false;
		}
	}

	return FFileHelper::SaveStringToFile(SaveText, *SaveDirectory);
}

//以数组的形式保存文本文件
bool UPeExtendedToolKitBPLibrary::SaveStringArrayToFile(FString SaveDirectory, FString FileName, TArray<FString> SaveText, bool AllowOverWriting)
{
	//目录存在？
	if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDirectory))
	{
		//如果不存在，创建目录
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*SaveDirectory);

		//仍然不能创建目录？
		if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*SaveDirectory))
		{
			//无法使用指定的目录
			return false;
			//返回假
		}
	}

	//获得完整的文件路径
	SaveDirectory += "\\";
	SaveDirectory += FileName;

	//是否掩盖保存？
	if (!AllowOverWriting)
	{
		//检查文件是否已经存在
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*SaveDirectory))
		{
			//没有覆盖
			return false;
		}
	}

	FString FinalStr = "";
	for (FString& Each : SaveText)
	{
		FinalStr += Each;
		FinalStr += LINE_TERMINATOR;
	}



	return FFileHelper::SaveStringToFile(FinalStr, *SaveDirectory);

}

//载入文本文件
bool UPeExtendedToolKitBPLibrary::LoadStringFromFile(FString& Contents, FString FullFilePath)
{

	FString File;

	if (FullFilePath == "" || FullFilePath == " ") return false;

	if (!FFileHelper::LoadFileToString(File, *FullFilePath, FFileHelper::EHashOptions::None))
	{
		return false;
	}

	Contents = File;
	return true;

}



//载入文本文件
bool UPeExtendedToolKitBPLibrary::LoadStringArrayFromFile(TArray<FString>& StringArray, int32& ArraySize, FString FullFilePath, bool ExcludeEmptyLines)
{
	ArraySize = 0;

	if (FullFilePath == "" || FullFilePath == " ") return false;

	//Empty any previous contents!
	StringArray.Empty();

	TArray<FString> FileArray;



	if (!FFileHelper::LoadANSITextFileToStrings(*FullFilePath, NULL, FileArray))
	{
		return false;
	}

	if (ExcludeEmptyLines)
	{
		for (const FString& Each : FileArray)
		{
			if (Each == "") continue;


			//检查是否有非空白
			bool FoundNonWhiteSpace = false;
			for (int32 v = 0; v < Each.Len(); v++)
			{
				if (Each[v] != ' ' && Each[v] != '\n')
				{
					FoundNonWhiteSpace = true;
					break;
				}
				//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			}

			if (FoundNonWhiteSpace)
			{
				StringArray.Add(Each);
			}
		}
	}
	else
	{
		StringArray.Append(FileArray);
	}

	ArraySize = StringArray.Num();
	return true;
}


//截图
FString UPeExtendedToolKitBPLibrary::TakeScreenShoot(FString picName, bool bUnique, bool ShowHUD)
{
	FString inStr;
	FString Filename;

	FScreenshotRequest::RequestScreenshot(picName + ".png", ShowHUD, bUnique);

	FString ResultStr = FScreenshotRequest::GetFilename();
	ResultStr = FPaths::GetBaseFilename(ResultStr, true);
	return ResultStr + ".png";
}


bool UPeExtendedToolKitBPLibrary::LoadAsset(FString Path, UObject* &Object)
{
	UObject* lo = LoadObject<UObject>(NULL, *Path);

	if (IsValid(lo))
	{
		Object = lo;
		return true;
	}
	else
	{
		return false;
	}
}

FString UPeExtendedToolKitBPLibrary::GetObjectFullName(UObject* Object)
{
	return Object->GetFullName();//GetArchetype()->GetFullName();
}

TArray<FString> UPeExtendedToolKitBPLibrary::GetAssetList(FString Path)
{
	UObjectLibrary* ObjectLibrary = NewObject<UObjectLibrary>(UObjectLibrary::StaticClass());
	//ObjectLibrary->ObjectBaseClass = BaseClass;
	ObjectLibrary->UseWeakReferences(GIsEditor);
	ObjectLibrary->AddToRoot();

	ObjectLibrary->LoadAssetDataFromPath(Path);

	//		ObjectLibrary->LoadAssetsFromAssetData();

	TArray<FAssetData> AssetDatas;
	ObjectLibrary->GetAssetDataList(AssetDatas);

	TArray<FString> list;

	for (int32 i = 0; i < AssetDatas.Num(); i++)
	{
		list.Add(AssetDatas[i].GetExportTextName());
	}
	return list;
}

/*
bool UPeExtendedToolKitBPLibrary::AddAssetPath(FName Path)
{
	return FAssetRegistry::AddPath(Path);
}
*/


UObject* UPeExtendedToolKitBPLibrary::LoadObjectFromAssetPath(TSubclassOf<UObject> ObjectClass, TSubclassOf<UObject> InOuter, FName Path , bool AllowObjectReconciliation, bool& IsValid)
{
	/**
	IsValid = false;

	if (Path == "")
	{
		return NULL;
	}

	FStreamableManager StreamableManager;


	FStringAssetReference reference = Path.ToString();

	UObject* LoadedObj = StreamableManager.SynchronousLoad(reference);     // StaticLoadObject(ObjectClass, InOuter, *Path.ToString(), *Name.ToString(), LOAD_Async, NULL, AllowObjectReconciliation);
	IsValid = LoadedObj != nullptr;

	return LoadedObj;
	*/

	/**
	IsValid = false;
	FStringAssetReference asset = Path.ToString();
	 UObject* itemObj = asset.ResolveObject();
	 IsValid = itemObj != nullptr;
	 return itemObj;
 */

 /*
	IsValid = false;

	UObject* Obj = LoadObject<UObject>(NULL, *Path.ToString());

	*/
	//LoadObject<UObject>(NULL, *Path.ToString());


	IsValid = false;

	if (Path == "")
	{
		return NULL;
	} 

	UObject* LoadedObj = StaticLoadObject(ObjectClass, InOuter, *Path.ToString(), NULL, LOAD_None, NULL, AllowObjectReconciliation);

	IsValid = LoadedObj != nullptr;

	return LoadedObj;


}

UClass* UPeExtendedToolKitBPLibrary::LoadObjectFromAssetClassPath(TSubclassOf<UObject> BaseClass, FName Path, bool& IsValid)
{
	IsValid = false;

	if (Path == NAME_None) return NULL;
	//~~~~~~~~~~~~~~~~~~~~~

	UClass* LoadedObj = StaticLoadClass(BaseClass, NULL, *Path.ToString(), NULL, LOAD_None, NULL);

	IsValid = LoadedObj != nullptr;




	return LoadedObj;


}





#if !PLATFORM_PS4
//填写声波信息
int32 UPeExtendedToolKitBPLibrary::fillSoundWaveInfo(class USoundWave* sw, TArray<uint8>* rawFile)
{
	//FSoundQualityInfo info;



	//FVorbisAudioInfo vorbis_obj;
	//if (!vorbis_obj.ReadCompressedInfo(rawFile->GetData(), rawFile->Num(), &info))
	//{
	//	//Debug("无法加载");
	//	return 1;

	//}

	////if (!sw) return 1;

	//sw->DecompressionType = EDecompressionType::DTYPE_RealTime;

	//sw->SoundGroup = ESoundGroup::SOUNDGROUP_Default;
	//sw->NumChannels = info.NumChannels;
	//sw->Duration = info.Duration;
	//sw->RawPCMDataSize = info.SampleDataSize;

	//sw->SetSampleRate(info.SampleRate);




	return 0;
}

#endif



//载入ogg声音文件
class USoundWave* UPeExtendedToolKitBPLibrary::LoadSoundWaveFromFile(const FString& FilePath)
{
#if PLATFORM_PS4
	UE_LOG(LogTemp, Error, TEXT("UVictoryBPFunctionLibrary::GetSoundWaveFromFile ~ vorbis-method not supported on PS4. See UVictoryBPFunctionLibrary::fillSoundWaveInfo"));
	return nullptr;
#endif

	USoundWave* sw = NewObject<USoundWave>(USoundWave::StaticClass());

	if (!sw)
		return NULL;

	//* 如果为true歌曲已成功加载
	bool loaded = false;

	//*加载歌曲文件（二进制编码）
	TArray < uint8 > rawFile;

	loaded = FFileHelper::LoadFileToArray(rawFile, FilePath.GetCharArray().GetData());

	if (loaded)
	{

		loaded = fillSoundWaveInfo(sw, &rawFile) == 0 ? true : false;


		FByteBulkData* bulkData = &sw->CompressedFormatData.GetFormat(TEXT("OGG"));
		bulkData->Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(bulkData->Realloc(rawFile.Num()), rawFile.GetData(), rawFile.Num());
		bulkData->Unlock();




	}

	if (!loaded)
		return NULL;



	//	GetPCMDataFromFile(sw);
	return sw;
}

/*

int UPeExtendedToolKitBPLibrary::findSource(class USoundWave* sw, class FSoundSource* out_audioSource)
{
	FAudioDevice* device = GEngine ? GEngine->GetMainAudioDevice() : NULL; //gently ask for the audio device

	FActiveSound* activeSound;
	FSoundSource* audioSource;
	FWaveInstance* sw_instance;
	if (!device)
	{
		activeSound = NULL;
		audioSource = NULL;
		out_audioSource = audioSource;
		return -1;
	}

	TArray<FActiveSound*> tmpActualSounds = device->GetActiveSounds();
	if (tmpActualSounds.Num())
	{
		for (auto activeSoundIt(tmpActualSounds.CreateIterator()); activeSoundIt; ++activeSoundIt)
		{
			activeSound = *activeSoundIt;
			for (auto WaveInstanceIt(activeSound->WaveInstances.CreateIterator()); WaveInstanceIt; ++WaveInstanceIt)
			{
				sw_instance = WaveInstanceIt.Value();
				if (sw_instance->WaveData->CompressedDataGuid == sw->CompressedDataGuid)
				{
					audioSource = device->FAudioDevice::WaveInstanceSourceMap.FindRef(sw_instance);  // device->WaveInstanceSourceMap.FindRef(sw_instance);
					out_audioSource = audioSource;
					return 0;
				}
			}
		}
	}

	audioSource = NULL;
	activeSound = NULL;
	out_audioSource = audioSource;
	return -2;
}

*/


//执行exe
bool UPeExtendedToolKitBPLibrary::ExecutionFile(FString FilePath)
{
	const TCHAR* PathTch = *FilePath;  //将FString转成TCHAR
	FPlatformProcess::CreateProc(PathTch, nullptr, true, false, false, nullptr, 0, nullptr, nullptr);
	return true;
}


// String转WString
std::wstring StringToWString(const std::string& s)
{
	std::wstring wszStr;

	int nLength = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, NULL);
	wszStr.resize(nLength);
	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength);
	wszStr = lpwszStr;
	delete[] lpwszStr;

	return wszStr;
}



//打开文件位置
FString UPeExtendedToolKitBPLibrary::OpenFile(FString FilePath, bool IsOpenFile)
{

	FString Str = "/e,/select,"; //打开目录选择文件的转意字符

	Str += FilePath;  //添加转意字符
	if (IsOpenFile)//如果打开文件为真则直接赋上路径
	{
		Str = FilePath;
	}


	const TCHAR* PathTch = *Str;  //将FString转成TCHAR
	wstring bob(&PathTch[0]); //把TCHAR转成Wstring
	LPCWSTR Path = bob.c_str(); //将Wstring转化成 LPCWSTR

	ShellExecute(NULL, _T("open"), _T("explorer.exe"), Path, NULL, SW_SHOWDEFAULT);  //打开文件所在位置

	return Str;
}


//打开选择文件窗口
FString UPeExtendedToolKitBPLibrary::OpenFileBrowserWindow(FString DefaultPath, PeFileBrowserFilterType Filter,FString HandleWindowName)
{
	if (DefaultPath == "")
	{
		DefaultPath = "C:\\Users\\admin\\Desktop";
	}


	const TCHAR* bw = *HandleWindowName;  //(标题)将FString转成TCHAR并储存
	wstring g(&bw[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR name = g.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存

	HWND pWnd = FindWindowW(NULL, name);


	const TCHAR* PathTch = *DefaultPath;  //(默认路径)将FString转成TCHAR并储存
	wstring PathZ(&PathTch[0]); //(默认路径)把TCHAR转成Wstring并储存
	LPCWSTR DefPathEnd = PathZ.c_str(); //(默认路径)将Wstring转化成 LPCWSTR并储存

	TCHAR szBuffer[MAX_PATH] = { 0 }; //存放选择的路径的变量
	OPENFILENAME FileBrowser = { 0 };




	FileBrowser.lStructSize = sizeof(FileBrowser);
	FileBrowser.hwndOwner = pWnd;
	
	switch (Filter)
	{
	case PeFileBrowserFilterType::EXE_ALL:FileBrowser.lpstrFilter = _T("EXE文件(*.exe)\0*.exe\0所有文件(*.*)\0*.*\0");
		break;
	case PeFileBrowserFilterType::TXT_ALL:FileBrowser.lpstrFilter = _T("TXT文件(*.txt)\0*.txt\0所有文件(*.*)\0*.*\0");
		break;
	case PeFileBrowserFilterType::PNG_ALL:FileBrowser.lpstrFilter = _T("PNG文件(*.png)\0*.png\0所有文件(*.*)\0*.*\0");
		break;
	case PeFileBrowserFilterType::JPG_ALL:FileBrowser.lpstrFilter = _T("JPG文件(*.jpg)\0*.jpg\0所有文件(*.*)\0*.*\0");
		break;
	case PeFileBrowserFilterType::ZIP_ALL:FileBrowser.lpstrFilter = _T("ZIP文件(*.zip)\0*.zip\0所有文件(*.*)\0*.*\0");
		break;
	case PeFileBrowserFilterType::RAR_ALL:FileBrowser.lpstrFilter = _T("RAR文件(*.rar)\0*.rar\0所有文件(*.*)\0*.*\0");
		break;
	case PeFileBrowserFilterType::EPR_ALL:FileBrowser.lpstrFilter = _T("EPR文件(*.epr)\0*.epr\0所有文件(*.*)\0*.*\0");
		break;
	case PeFileBrowserFilterType::PNG_JPG_ALL:FileBrowser.lpstrFilter = _T("PNG文件(*.png)\0*.png\0JPG文件(*.jpg)\0*.jpg\0所有文件(*.*)\0*.*\0");
		break;
	default:FileBrowser.lpstrFilter = _T("所有文件(*.*)\0*.*\0");
		break;
	}

	FileBrowser.lpstrInitialDir = DefPathEnd;//默认的文件路径   
	FileBrowser.lpstrFile = szBuffer;//存放文件的缓冲区   
	FileBrowser.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	FileBrowser.nFilterIndex = 0;
	FileBrowser.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;//标志如果是多选要加上OFN_ALLOWMULTISELECT  
	BOOL bSel = GetOpenFileName(&FileBrowser);


	return szBuffer;
}





//打开选择文件夹窗口
FString UPeExtendedToolKitBPLibrary::OpenFolderBrowserWindow(FString Title, FString HandleWindowName)
{
	const TCHAR* bw = *HandleWindowName;  //(标题)将FString转成TCHAR并储存
	wstring g(&bw[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR name = g.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存

	HWND pWnd = FindWindowW(NULL, name);


	const TCHAR* A = *Title;  //(标题)将FString转成TCHAR并储存
	wstring B(&A[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR C = B.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存

	TCHAR szBuffer[MAX_PATH] = { 0 };
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = pWnd;
	bi.pszDisplayName = szBuffer;
	bi.lpszTitle = C;
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (NULL == idl)
	{
		return"";
	}
	SHGetPathFromIDList(idl, szBuffer);


	return szBuffer;

}

LPCWSTR FStringToLPCWSTR(FString Text)
{
	const TCHAR* A = *Text;  //(标题)将FString转成TCHAR并储存
	wstring B(&A[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR C = B.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存
	return C;
}

//提示窗口
int32 UPeExtendedToolKitBPLibrary::PromptBox(FString Title, FString conten, int32 Mode)
{
	const TCHAR* A = *conten;  //(标题)将FString转成TCHAR并储存
	wstring B(&A[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR C = B.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存

	const TCHAR* Aa = *Title;  //(标题)将FString转成TCHAR并储存
	wstring Bb(&Aa[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR Cc = Bb.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存

	// MessageBox(NULL, 内容, 标题, 按钮模式);
	return MessageBox(NULL, C, Cc, Mode);
}

bool UPeExtendedToolKitBPLibrary::IsEditorMode()
{
	UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
	if (GameEngine != nullptr)
	{
		TSharedPtr<SWindow> GameViewportWindow = GameEngine->GameViewportWindow.Pin();
		if (GameViewportWindow.IsValid())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}
}

FText UPeExtendedToolKitBPLibrary::GetGameWindowTitle()
{

	FText T;
	UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
	if (GameEngine != nullptr)
	{
		TSharedPtr<SWindow> GameViewportWindow = GameEngine->GameViewportWindow.Pin();
		if (GameViewportWindow.IsValid())
		{
			return GameViewportWindow->GetTitle();
		}
		else
		{
     	return T;
		}
	}
	else
	{
		return T;
	}

}


void UPeExtendedToolKitBPLibrary::SetGameWindowSizeMode(WindowSizeMode Mode)
{
	UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);

	if (GameEngine != nullptr)
	{
		TSharedPtr<SWindow> GameViewportWindow = GameEngine->GameViewportWindow.Pin();
		if (GameViewportWindow.IsValid())
		{
			switch (Mode)
			{
			case WindowSizeMode::FixedSize:GameViewportWindow->SetSizingRule(ESizingRule::FixedSize);
				break;
			case WindowSizeMode::Autosized:GameViewportWindow->SetSizingRule(ESizingRule::Autosized);
				break;
			case WindowSizeMode::UserSized:GameViewportWindow->SetSizingRule(ESizingRule::UserSized);
				break;
			default:GameViewportWindow->SetSizingRule(ESizingRule::UserSized);
				break;
			}
		}
	}
}




int32 UPeExtendedToolKitBPLibrary::BPMessageBox(FString Title, FString Conten ,MessageBoxType ButtonType, MessageBoxICOType ICO,FString HandleWindowName)
{
	const TCHAR* A = *Conten;  //(标题)将FString转成TCHAR并储存
	wstring B(&A[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR C = B.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存

	const TCHAR* Aa = *Title;  //(标题)将FString转成TCHAR并储存
	wstring Bb(&Aa[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR Cc = Bb.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存

	int32 MesICO;
	int32 MesButton;
 
	

	const TCHAR* bw = *HandleWindowName;  //(标题)将FString转成TCHAR并储存
	wstring g(&bw[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR name = g.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存

	HWND pWnd = FindWindowW(NULL, name);
	switch (ButtonType)
	{
	case MessageBoxType::Button_OK:MesButton = MB_OK;
		break;
	case MessageBoxType::Button_OK_Cancel:MesButton = MB_OKCANCEL;
		break;
	case MessageBoxType::Button_Stop_Retry_Ignore:MesButton = MB_ABORTRETRYIGNORE;
		break;
	case MessageBoxType::Button_Yes_No_Cancel:MesButton = MB_YESNOCANCEL;
		break;
	case MessageBoxType::Button_Yes_No:MesButton = MB_YESNO;
		break;
	case MessageBoxType::Button_Retry_Cancel:MesButton = MB_RETRYCANCEL;
		break;
	default:MesButton = NULL;
		break;
	}
	switch (ICO)
	{
	case MessageBoxICOType::ICO_Error:MesICO = MB_ICONHAND;
		break;
	case MessageBoxICOType::ICO_Doubt:MesICO = MB_ICONQUESTION;
		break;
	case MessageBoxICOType::ICO_Sigh:MesICO = MB_ICONEXCLAMATION;
		break;
	case MessageBoxICOType::ICO_Info:MesICO = MB_ICONASTERISK;
		break;
	case MessageBoxICOType::ICO_User:MesICO = MB_USERICON;
		break;
	default:MesICO = NULL;
		break;
	}
	return MessageBox(pWnd, C, Cc, MesICO|MesButton);
}

//DesktopPlatformWindows

static ::INT CALLBACK BrowseCallbackProc(HWND hwnd, ::UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	// 初始化时设置开始路径的路径。
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		if (lpData)
		{
			SendMessageW(hwnd, BFFM_SETSELECTION, true, lpData);
		}
		break;
	}

	return 0;
}


//打开文件窗口
bool FileDialogShared(bool bSave, const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, bool Multiple, TArray<FString>& OutFilenames, int32& OutFilterIndex)
{

	WCHAR Filename[MAX_FILENAME_STR];
	FCString::Strcpy(Filename, MAX_FILENAME_STR, *(DefaultFile.Replace(TEXT("/"), TEXT("\\"))));

	// Convert the forward slashes in the path name to backslashes, otherwise it'll be ignored as invalid and use whatever is cached in the registry
	WCHAR Pathname[MAX_FILENAME_STR];
	FCString::Strcpy(Pathname, MAX_FILENAME_STR, *DefaultPath.Replace(TEXT("/"), TEXT("\\")));

	// Convert the "|" delimited list of filetypes to NULL delimited then add a second NULL character to indicate the end of the list
	WCHAR FileTypeStr[MAX_FILETYPES_STR];
	WCHAR* FileTypesPtr = NULL;
	const int32 FileTypesLen = FileTypes.Len();

	// Nicely formatted file types for lookup later and suitable to append to filenames without extensions
	TArray<FString> CleanExtensionList;

	// The strings must be in pairs for windows.
	// It is formatted as follows: Pair1String1|Pair1String2|Pair2String1|Pair2String2
	// where the second string in the pair is the extension.  To get the clean extensions we only care about the second string in the pair
	TArray<FString> UnformattedExtensions;
	FileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);
	for (int32 ExtensionIndex = 1; ExtensionIndex < UnformattedExtensions.Num(); ExtensionIndex += 2)
	{
		const FString& Extension = UnformattedExtensions[ExtensionIndex];
		// Assume the user typed in an extension or doesnt want one when using the *.* extension. We can't determine what extension they wan't in that case
		if (Extension != TEXT("*.*"))
		{
			// Add to the clean extension list, first removing the * wildcard from the extension
			int32 WildCardIndex = Extension.Find(TEXT("*"));
			CleanExtensionList.Add(WildCardIndex != INDEX_NONE ? Extension.RightChop(WildCardIndex + 1) : Extension);
		}
	}

	if (FileTypesLen > 0 && FileTypesLen - 1 < MAX_FILETYPES_STR)
	{
		FileTypesPtr = FileTypeStr;
		FCString::Strcpy(FileTypeStr, MAX_FILETYPES_STR, *FileTypes);

		TCHAR* Pos = FileTypeStr;
		while (Pos[0] != 0)
		{
			if (Pos[0] == '|')
			{
				Pos[0] = 0;
			}

			Pos++;
		}

		// Add two trailing NULL characters to indicate the end of the list
		FileTypeStr[FileTypesLen] = 0;
		FileTypeStr[FileTypesLen + 1] = 0;
	}

	OPENFILENAME ofn;
	FMemory::Memzero(&ofn, sizeof(OPENFILENAME));

	const TCHAR* bw = *UPeExtendedToolKitBPLibrary::GetGameWindowTitle().ToString();  //(获取标题)将FString转成TCHAR并储存
	wstring g(&bw[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR name = g.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存


	HWND pWnd = FindWindowW(NULL, name);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pWnd;//(HWND)ParentWindowHandle;
	ofn.lpstrFilter = FileTypesPtr;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = Filename;
	ofn.nMaxFile = MAX_FILENAME_STR;
	ofn.lpstrInitialDir = Pathname;
	ofn.lpstrTitle = *DialogTitle;
	if (FileTypesLen > 0)
	{
		ofn.lpstrDefExt = &FileTypeStr[0];
	}

	ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER;

	if (bSave)
	{
		ofn.Flags |= OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_NOVALIDATE;
	}
	else
	{
		ofn.Flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	}

	if (Multiple)
	{
		ofn.Flags |= OFN_ALLOWMULTISELECT;
	}

	bool bSuccess;
	if (bSave)
	{
		bSuccess = !!::GetSaveFileName(&ofn);
	}
	else
	{
		bSuccess = !!::GetOpenFileName(&ofn);
	}

	if (bSuccess)
	{
		// GetOpenFileName/GetSaveFileName changes the CWD on success. Change it back immediately.
		FPlatformProcess::SetCurrentWorkingDirectoryToBaseDir();

		if (Multiple)
		{
			// When selecting multiple files, the returned string is a NULL delimited list
			// where the first element is the directory and all remaining elements are filenames.
			// There is an extra NULL character to indicate the end of the list.
			FString DirectoryOrSingleFileName = FString(Filename);
			TCHAR* Pos = Filename + DirectoryOrSingleFileName.Len() + 1;

			if (Pos[0] == 0)
			{
				// One item selected. There was an extra trailing NULL character.
				OutFilenames.Add(DirectoryOrSingleFileName);
			}
			else
			{
				// Multiple items selected. Keep adding filenames until two NULL characters.
				FString SelectedFile;
				do
				{
					SelectedFile = FString(Pos);
					new(OutFilenames) FString(DirectoryOrSingleFileName / SelectedFile);
					Pos += SelectedFile.Len() + 1;
				} while (Pos[0] != 0);
			}
		}
		else
		{
			new(OutFilenames) FString(Filename);
		}

		// The index of the filter in OPENFILENAME starts at 1.
		OutFilterIndex = ofn.nFilterIndex - 1;

		// Get the extension to add to the filename (if one doesnt already exist)
		FString Extension = CleanExtensionList.IsValidIndex(OutFilterIndex) ? CleanExtensionList[OutFilterIndex] : TEXT("");

		// Make sure all filenames gathered have their paths normalized and proper extensions added
		for (auto OutFilenameIt = OutFilenames.CreateIterator(); OutFilenameIt; ++OutFilenameIt)
		{
			FString& OutFilename = *OutFilenameIt;

			OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);

			if (FPaths::GetExtension(OutFilename).IsEmpty() && !Extension.IsEmpty())
			{
				// filename does not have an extension. Add an extension based on the filter that the user chose in the dialog
				OutFilename += Extension;
			}

			FPaths::NormalizeFilename(OutFilename);
		}
	}
	else
	{
		uint32 Error = ::CommDlgExtendedError();
		if (Error != ERROR_SUCCESS)
		{
			// UE_LOG(LogDesktopPlatform, Warning, TEXT("Error reading results of file dialog. Error: 0x%04X"), Error);
		}
	}

	return bSuccess;
}

//打开文件夹选择窗口
bool OpenDirectoryDialog(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName)
{

	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));

	TCHAR FolderName[MAX_PATH];
	ZeroMemory(FolderName, sizeof(TCHAR) * MAX_PATH);

	const FString PathToSelect = DefaultPath.Replace(TEXT("/"), TEXT("\\"));


	const TCHAR* bw = *UPeExtendedToolKitBPLibrary::GetGameWindowTitle().ToString();  //(获取标题)将FString转成TCHAR并储存
	wstring g(&bw[0]); //(标题)把TCHAR转成Wstring并储存
	LPCWSTR name = g.c_str(); //(标题)将Wstring转化成 LPCWSTR并储存


	HWND pWnd = FindWindowW(NULL, name);



	bi.hwndOwner = pWnd;// (HWND)ParentWindowHandle;

	bi.pszDisplayName = FolderName;
	bi.lpszTitle = *DialogTitle;
	bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS | BIF_SHAREABLE;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)(*PathToSelect);
	bool bSuccess = false;
	LPCITEMIDLIST Folder = ::SHBrowseForFolder(&bi);
	if (Folder)
	{
		bSuccess = (::SHGetPathFromIDList(Folder, FolderName) ? true : false);
		if (bSuccess)
		{
			OutFolderName = FolderName;
			FPaths::NormalizeFilename(OutFolderName);
		}
		else
		{
			//UE_LOG(LogDesktopPlatform, Warning, TEXT("Error converting the folder path to a string."));
		}
	}
	else
	{
		//UE_LOG(LogDesktopPlatform, Warning, TEXT("Error reading results of folder dialog."));
	}

	return bSuccess;
}

TArray<FString>UPeExtendedToolKitBPLibrary::UOpenFileDialog(bool IsSavdDialog ,FString Title, FString DefaultFile, FString TypeList, FString InitialPath, bool Multiple,int& OutFilterIndex)
{
	TArray<FString> selectedFiles;
	FileDialogShared(IsSavdDialog,GetDesktopWindow(), Title, InitialPath, DefaultFile, TypeList, Multiple, selectedFiles, OutFilterIndex);
	return selectedFiles;
}

FString UPeExtendedToolKitBPLibrary::UOpenDirectoryDialog(FString Title, FString DefaultPath)
{
	FString OutPath;
 
	OpenDirectoryDialog(GetDesktopWindow(), Title, DefaultPath, OutPath);
 
	return OutPath;
}




/**
bool SaveGame(bool bAttemptToUseUI, const TCHAR* Name, const int32 UserIndex, const TArray<uint8>& Data)
{
	return FFileHelper::SaveArrayToFile(Data, Name);
}
*/

//自定义路径保存存档
bool UPeExtendedToolKitBPLibrary::SaveGameToPath(USaveGame* SaveGameObject, const FString& SavedPath, const int32 UserIndex)
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	// If we have a system and an object to save and a save name...  检测是否有效
	if (SaveSystem && SaveGameObject && (SavedPath.Len() > 0))
	{
		TArray<uint8> ObjectBytes;
		FMemoryWriter MemoryWriter(ObjectBytes, true);

		// write file type tag. identifies this file type and indicates it's using proper versioning
		// since older UE4 versions did not version this data.
		int32 FileTypeTag = UE4_SAVEGAME_FILE_TYPE_TAG;
		MemoryWriter << FileTypeTag;

		// Write version for this file format
		int32 SavegameFileVersion = FSaveGameFileVersion::LatestVersion;
		MemoryWriter << SavegameFileVersion;

		// Write out engine and UE4 version information
		int32 PackageFileUE4Version = GPackageFileUE4Version;
		MemoryWriter << PackageFileUE4Version;
		FEngineVersion SavedEngineVersion = FEngineVersion::Current();
		MemoryWriter << SavedEngineVersion;

		// Write out custom version data
		ECustomVersionSerializationFormat::Type const CustomVersionFormat = ECustomVersionSerializationFormat::Latest;
		int32 CustomVersionFormatInt = static_cast<int32>(CustomVersionFormat);
		MemoryWriter << CustomVersionFormatInt;
		FCustomVersionContainer CustomVersions = FCustomVersionContainer::GetRegistered();
		CustomVersions.Serialize(MemoryWriter, CustomVersionFormat);

		// Write the class name so we know what class to load to
		FString SaveGameClassName = SaveGameObject->GetClass()->GetName();
		MemoryWriter << SaveGameClassName;

		// Then save the object state, replacing object refs and names with strings
		FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
		SaveGameObject->Serialize(Ar);

		// Stuff that data into the save system with the desired file name
		//return SaveGame(false, *SavedPath, UserIndex, ObjectBytes);

		return FFileHelper::SaveArrayToFile(ObjectBytes, *SavedPath);

	}
	return false;
}


//自定义路径加载存档-加载
bool LoadGame(bool bAttemptToUseUI, const TCHAR* Name, const int32 UserIndex, TArray<uint8>& Data)
{
	FString Conversion = Name;
	return FFileHelper::LoadFileToArray(Data, *Conversion);
}

//自定义路径加载存档
USaveGame* UPeExtendedToolKitBPLibrary::LoadGameSaved(const FString& SavedPath, const int32 UserIndex)
{
	USaveGame* OutSaveGameObject = NULL;

	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	// If we have a save system and a valid name..
	if (SaveSystem && (SavedPath.Len() > 0))
	{
		// Load raw data from slot
		TArray<uint8> ObjectBytes;
		bool bSuccess = LoadGame(false, *SavedPath, UserIndex, ObjectBytes);
		if (bSuccess)
		{
			FMemoryReader MemoryReader(ObjectBytes, true);

			int32 FileTypeTag;
			MemoryReader << FileTypeTag;

			int32 SavegameFileVersion;
			if (FileTypeTag != UE4_SAVEGAME_FILE_TYPE_TAG)
			{
				// this is an old saved game, back up the file pointer to the beginning and assume version 1
				MemoryReader.Seek(0);
				SavegameFileVersion = FSaveGameFileVersion::InitialVersion;

				// Note for 4.8 and beyond: if you get a crash loading a pre-4.8 version of your savegame file and 
				// you don't want to delete it, try uncommenting these lines and changing them to use the version 
				// information from your previous build. Then load and resave your savegame file.
				//MemoryReader.SetUE4Ver(MyPreviousUE4Version);				// @see GPackageFileUE4Version
				//MemoryReader.SetEngineVer(MyPreviousEngineVersion);		// @see FEngineVersion::Current()
			}
			else
			{
				// Read version for this file format
				MemoryReader << SavegameFileVersion;

				// Read engine and UE4 version information
				int32 SavedUE4Version;
				MemoryReader << SavedUE4Version;

				FEngineVersion SavedEngineVersion;
				MemoryReader << SavedEngineVersion;

				MemoryReader.SetUE4Ver(SavedUE4Version);
				MemoryReader.SetEngineVer(SavedEngineVersion);

				if (SavegameFileVersion >= FSaveGameFileVersion::AddedCustomVersions)
				{
					int32 CustomVersionFormat;
					MemoryReader << CustomVersionFormat;

					FCustomVersionContainer CustomVersions;
					CustomVersions.Serialize(MemoryReader, static_cast<ECustomVersionSerializationFormat::Type>(CustomVersionFormat));
					MemoryReader.SetCustomVersions(CustomVersions);
				}
			}

			// Get the class name
			FString SaveGameClassName;
			MemoryReader << SaveGameClassName;

			// Try and find it, and failing that, load it
			UClass* SaveGameClass = FindObject<UClass>(ANY_PACKAGE, *SaveGameClassName);
			if (SaveGameClass == NULL)
			{
				SaveGameClass = LoadObject<UClass>(NULL, *SaveGameClassName);
			}

			// If we have a class, try and load it.
			if (SaveGameClass != NULL)
			{
				OutSaveGameObject = NewObject<USaveGame>(GetTransientPackage(), SaveGameClass);

				FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
				OutSaveGameObject->Serialize(Ar);
			}
		}
	}

	return OutSaveGameObject;
}

//查找向量
int32 UPeExtendedToolKitBPLibrary::FindVector(TArray<FVector>InArray, FVector InVector, float ErrorTolerance)
{
	FVector TraceVector;
	// for 循环判断
	for (int ID = 0; ID<InArray.Num() - 1; ID = ID++)
	{
		TraceVector = InArray[ID];
		//判断是否等于InVector
		if (TraceVector.Equals(InVector, ErrorTolerance))
		{
			return ID;
		}
		if (ID == InArray.Num() - 1)
		{
			return -1;
		}
	}
	return -1;
}

//查找浮点型
int32 UPeExtendedToolKitBPLibrary::FindFloat(TArray<float>InArray, float InFloat, float ErrorTolerance)
{
	// for 循环判断
	float NearestDist = -1.0f;
	int32 NearestID = -1;
	for (int ID = 0; ID < InArray.Num(); ++ID)
	{
		float Tracefloat = InArray[ID];

		//判断是否等于
		float Dist = FGenericPlatformMath::Abs<float>(Tracefloat - InFloat);
		if (FMath::IsNearlyZero(Dist, ErrorTolerance))
		{
			if (NearestDist < 0 || Dist < NearestDist)
			{
				NearestDist = Dist;
				NearestID = ID;
			}
		}
	}

	return NearestID;
}


//设置StaticMeshActor移动性
void UPeExtendedToolKitBPLibrary::SetStaticMeshMobility(AStaticMeshActor* StaticMeshActor, EComponentMobility::Type Mobility)
{
	StaticMeshActor->SetMobility(Mobility);
}


//创建窗口
void UPeExtendedToolKitBPLibrary::WindowCreate
(
	FText Title,
	bool AutoCenter,
	bool IsInitiallyMaximized,
	FVector2D Postion,
	FVector2D ClientSize,
	UWidget* Widget,
	bool CreateTitleBar,
	bool SupportsMaximize,
	bool SupportsMinimize,
	bool HasCloseButton,
	bool UseOSWindowBorder,
	bool ShouldPreserveAspectRatio,
	float InitialOpacity
	)
{
	TSharedPtr<SWindow> SlateWin;


	if (AutoCenter)
	{
		SlateWin = SNew(SWindow)
			.AutoCenter(EAutoCenter::PreferredWorkArea)//自动中心
			.Title(Title)//窗口标题
			.IsInitiallyMaximized(IsInitiallyMaximized)//默认最大化
			.ScreenPosition(Postion)//位置
			.ClientSize(ClientSize)//窗口大小
			.CreateTitleBar(CreateTitleBar)//是否创建标题栏
			.SizingRule(ESizingRule::UserSized)//尺寸模式
			.SupportsMaximize(SupportsMaximize) //最大化按钮
			.SupportsMinimize(SupportsMinimize)//最小化按钮
			.HasCloseButton(HasCloseButton)//关闭按钮
			.Type(EWindowType::Normal) //窗口模式
			.UseOSWindowBorder(UseOSWindowBorder)//使用Windows风格窗口边框
			.ShouldPreserveAspectRatio(ShouldPreserveAspectRatio)//自动比例
			.SupportsTransparency(EWindowTransparency::PerWindow)//透明模式
			.InitialOpacity(InitialOpacity);


	}
	else
	{
		SlateWin = SNew(SWindow)
			.AutoCenter(EAutoCenter::None)//自动中心
			.Title(Title)//窗口标题
			.IsInitiallyMaximized(IsInitiallyMaximized)//默认最大化
			.ScreenPosition(Postion)//位置
			.ClientSize(ClientSize)//窗口大小
			.CreateTitleBar(CreateTitleBar)//是否创建标题栏
			.SizingRule(ESizingRule::UserSized)//尺寸模式
			.SupportsMaximize(SupportsMaximize) //最大化按钮
			.SupportsMinimize(SupportsMinimize)//最小化按钮
			.HasCloseButton(HasCloseButton)//关闭按钮
			.Type(EWindowType::Normal) //窗口模式
			.UseOSWindowBorder(UseOSWindowBorder)//使用Windows风格窗口边框
			.ShouldPreserveAspectRatio(ShouldPreserveAspectRatio)//自动比例
			.SupportsTransparency(EWindowTransparency::PerWindow)//透明模式
			.InitialOpacity(InitialOpacity);
	}




	TSharedRef<SWindow> SlateWinRef = SlateWin.ToSharedRef();
	FSlateApplication & SlateApp = FSlateApplication::Get();

	//判断widget是否有效
	if (Widget)
	{
		TSharedRef<SBorder> ToolTip = SNew(SBorder)
			.BorderBackgroundColor(FLinearColor(1, 1, 1, 0))
			[
				Widget->TakeWidget()
			];
		SlateWinRef->SetContent(ToolTip);
	}
	else
	{
		//无效则直接add一个border
		SlateWinRef->SetContent(SNew(SBorder).BorderBackgroundColor(FLinearColor(1, 1, 1, 0)));
	}

	//创建窗口
	SlateApp.AddWindow(SlateWinRef, true);
}


FVector2D UPeExtendedToolKitBPLibrary::GetMouseLocation()
{
	POINT pt;
	GetCursorPos(&pt);
	return FVector2D(pt.x, pt.y);
}

FVector2D UPeExtendedToolKitBPLibrary::GetLocalMouseLocation()
{
	//获取窗口位置
	FVector2D WindowPosition;
	if (GEngine && GEngine->GameViewport)
	{
		WindowPosition = GEngine->GameViewport->GetWindow()->GetPositionInScreen();
	}

	POINT pt;
	GetCursorPos(&pt);
	return FVector2D(pt.x - WindowPosition.X, pt.y - WindowPosition.Y);

}

void UPeExtendedToolKitBPLibrary::SetMouseLocation(FVector2D Location)
{
	POINT pt;
	pt.x = Location.X;
	pt.y = Location.Y;
	SetCursorPos(Location.X, Location.Y);
}


void UPeExtendedToolKitBPLibrary::SetGameWindowPosition(FVector2D Location)
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetWindow()->MoveWindowTo(Location);
	}
}

FVector2D UPeExtendedToolKitBPLibrary::GetGameWindowPosition()
{
	FVector2D pos;
	if (GEngine && GEngine->GameViewport)
	{
		pos = GEngine->GameViewport->GetWindow()->GetPositionInScreen();
	}
	return pos;
}

FVector2D UPeExtendedToolKitBPLibrary::GetGameWindowSize()
{
	FVector2D pos;
	if (GEngine && GEngine->GameViewport)
	{
		pos = GEngine->GameViewport->GetWindow()->GetSizeInScreen();
	}
	return pos;
}



//鼠标右键是否按下
bool UPeExtendedToolKitBPLibrary::IsMouseRightClick()
{
	return KEY_DOWN(MOUSE_EVENT);
}
//鼠标左键是否按下
bool UPeExtendedToolKitBPLibrary::IsMouseLeftClick()
{
	return KEY_DOWN(MOUSE_MOVED);
}
//鼠标中键是否按下
bool UPeExtendedToolKitBPLibrary::IsMouseMiddleClick()
{
	return KEY_DOWN(MOUSE_WHEELED);
}

//Ctrl是否按下
bool UPeExtendedToolKitBPLibrary::IsCtrlClick()
{
	return KEY_DOWN(VK_CONTROL);
}
//Ctrl是否按下
bool UPeExtendedToolKitBPLibrary::IsAltClick()
{
	return KEY_DOWN(VK_MENU);
}



//设置SoundLoping
void UPeExtendedToolKitBPLibrary::SetSoundLooping(USoundWave* Sound, bool IsLooping)
{
	Sound->bLooping = (uint32)IsLooping;
}
//设置SoundLoping
bool UPeExtendedToolKitBPLibrary::GetSoundLooping(USoundWave* Sound)
{
	return (bool)Sound->bLooping;
}




void UPeExtendedToolKitBPLibrary::PrintOut(UObject* WorldContextObject, const FString& InString, bool bPrintToScreen, bool bPrintToLog, FLinearColor TextColor, float Duration)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) // Do not Print in Shipping or Test

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	FString Prefix;
	if (World)
	{
		if (World->WorldType == EWorldType::PIE)
		{
			switch (World->GetNetMode())
			{
			case NM_Client:
				Prefix = FString::Printf(TEXT("Client %d: "), GPlayInEditorID - 1);
				break;
			case NM_DedicatedServer:
			case NM_ListenServer:
				Prefix = FString::Printf(TEXT("Server: "));
				break;
			case NM_Standalone:
				break;
			}
		}
	}

	const FString FinalDisplayString = Prefix + InString;
	FString FinalLogString = FinalDisplayString;

	static const FBoolConfigValueHelper DisplayPrintStringSource(TEXT("Kismet"), TEXT("bLogPrintStringSource"), GEngineIni);
	if (DisplayPrintStringSource)
	{
		const FString SourceObjectPrefix = FString::Printf(TEXT("[%s] "), *GetNameSafe(WorldContextObject));
		FinalLogString = SourceObjectPrefix + FinalLogString;
	}

	if (bPrintToLog)
	{
		UE_LOG(LogBlueprintUserMessages, Log, TEXT("%s"), *FinalLogString);

		APlayerController* PC = (WorldContextObject ? UGameplayStatics::GetPlayerController(WorldContextObject, 0) : NULL);
		ULocalPlayer* LocalPlayer = (PC ? Cast<ULocalPlayer>(PC->Player) : NULL);
		if (LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->ViewportConsole)
		{
			LocalPlayer->ViewportClient->ViewportConsole->OutputText(FinalDisplayString);
		}
	}
	else
	{
		UE_LOG(LogBlueprintUserMessages, Verbose, TEXT("%s"), *FinalLogString);
	}

	// Also output to the screen, if possible
	if (bPrintToScreen)
	{
		if (GAreScreenMessagesEnabled)
		{
			if (GConfig && Duration < 0)
			{
				GConfig->GetFloat(TEXT("Kismet"), TEXT("PrintStringDuration"), Duration, GEngineIni);
			}
			GEngine->AddOnScreenDebugMessage((uint64)-1, Duration, TextColor.ToFColor(true), FinalDisplayString);
		}
		else
		{
			UE_LOG(LogBlueprint, VeryVerbose, TEXT("Screen messages disabled (!GAreScreenMessagesEnabled).  Cannot print to screen."));
		}
	}
#endif
}







AActor* UPeExtendedToolKitBPLibrary::SpawnActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass , FVector Location, FRotator Rotation, bool bNoCollisionFail)
{
	AActor* NewActor = NULL;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (World && *ActorClass)
	{
		FActorSpawnParameters ActorSpawnParams;

		ActorSpawnParams.SpawnCollisionHandlingOverride = bNoCollisionFail ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		NewActor = World->SpawnActor<AActor>(*ActorClass, Location, Rotation, ActorSpawnParams);

	}

	return NewActor;
}





//UMG绘制样线条
void UPeExtendedToolKitBPLibrary::DrawSpline(FPaintContext Context, int32 Layer, FVector2D StartPoint, FVector2D StartDir, FVector2D EndPoint, FVector2D EndDir, float Thickness, FLinearColor Color)
{
	FSlateDrawElement::MakeSpline(Context.OutDrawElements, Layer, Context.AllottedGeometry.ToPaintGeometry(), StartPoint, StartDir, EndPoint, EndDir,Thickness, ESlateDrawEffect::None, Color);
}

int32 UPeExtendedToolKitBPLibrary::GetContextMaxLayer(FPaintContext Context)
{
	return Context.MaxLayer;

}




void UPeExtendedToolKitBPLibrary::DrawGradient(FPaintContext Context, int32 Layer, FVector2D Position, FVector2D Size, TArray<FBPSlateGradientStop>Gradient, bool IsHorizontal)
{
	TArray<FSlateGradientStop> InGradientStops;

	EOrientation InGradientType;

	for (int i = 0; i < Gradient.Num()-1; i++)
	{
		InGradientStops.Add(FSlateGradientStop(Gradient[i].Position, Gradient[i].Color));
	}

	if (IsHorizontal)
	{
		InGradientType = EOrientation::Orient_Horizontal;
	}
	else
	{
		InGradientType = EOrientation::Orient_Vertical;
	}

	FSlateDrawElement::MakeGradient(
		Context.OutDrawElements,
		Layer,
		Context.AllottedGeometry.ToPaintGeometry(Position, Size),
		InGradientStops,
		InGradientType,
		ESlateDrawEffect::None);
}


void UPeExtendedToolKitBPLibrary::DrawRotationBox(FPaintContext Context, int32 Layer, FVector2D Position, FVector2D Size, float Angle, FVector2D InRotationPoint, bool RelativeToWorld, USlateBrushAsset* Brush, FLinearColor Tint)
{

	FSlateDrawElement::ERotationSpace RotationSpace;

	if (RelativeToWorld)
	{
		RotationSpace = FSlateDrawElement::ERotationSpace::RelativeToWorld;
	}
	else
	{
		RotationSpace = FSlateDrawElement::ERotationSpace::RelativeToElement;
	}

	FSlateRect InClippingRect;

	if (Brush)
	{
		/*FSlateDrawElement::MakeRotatedBox(
			Context.OutDrawElements,
			Layer,
			Context.AllottedGeometry.ToPaintGeometry(Position, Size),
			&Brush->Brush,
			Context.MyCullingRect,
			ESlateDrawEffect::None,
			Angle,
			InRotationPoint,
			FSlateDrawElement::ERotationSpace::RelativeToElement,
			Tint);*/
		FSlateDrawElement::MakeRotatedBox(
			Context.OutDrawElements,
			Layer,
			Context.AllottedGeometry.ToPaintGeometry(Position, Size),
			&Brush->Brush,
			ESlateDrawEffect::None,
			Angle,
			InRotationPoint,
			FSlateDrawElement::ERotationSpace::RelativeToElement,
			Tint);
	}
}


void UPeExtendedToolKitBPLibrary::DrawGradientSpline(FPaintContext Context, int32 InLayer, const FVector2D StartPostion, const FVector2D StartDir, const FVector2D EndPostion, const FVector2D EndDir, TArray<FBPSlateGradientStop>Gradient, float Thickness)
{

	TArray<FSlateGradientStop> InGradientStops;

	for (int i = 0; i < Gradient.Num() - 1; i++)
	{
		InGradientStops.Add(FSlateGradientStop(Gradient[i].Position, Gradient[i].Color));
	}

	/*FSlateDrawElement::MakeDrawSpaceGradientSpline(
		Context.OutDrawElements,
		InLayer,
		StartPostion,
		StartDir,
		EndPostion,
		EndDir,
		Context.MyCullingRect,
		InGradientStops,
		Thickness,
		ESlateDrawEffect::None);*/

	FSlateDrawElement::MakeDrawSpaceGradientSpline(
		Context.OutDrawElements,
		InLayer,
		StartPostion,
		StartDir,
		EndPostion,
		EndDir,
		InGradientStops,
		Thickness,
		ESlateDrawEffect::None);
}

FVector2D UPeExtendedToolKitBPLibrary::GetDisplayScreenSize()
{
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);


	FVector2D MyscreenSize(cx,cy);
	return MyscreenSize;

}
