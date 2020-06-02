
#include "SystemUtilsImpl.h"
#include "BuildingSDKSystem.h"
#include "Interfaces/IPluginManager.h"

static SystemUtilsImpl s_SystemUtils;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool SystemUtilsImpl::Copy(const char *SrcFile, const char *DstFile)
{
	FString SrcCopyFile = ANSI_TO_TCHAR(SrcFile);
	FString DstCopyFile = ANSI_TO_TCHAR(DstFile);
	if (COPY_OK == IFileManager::Get().Copy(*DstCopyFile, *SrcCopyFile))
	{
		return true;
	}
	return false;
}

const char *SystemUtilsImpl::GetCachedFilename(const char *SearchPath, const char *ResID, bool bCheckFileExsit)
{
	static std::string AnsiFilename;
	AnsiFilename = "";

	TArray<FString> FoundFiles;
	FString DirName = ANSI_TO_TCHAR(SearchPath);
	FString ID = ANSI_TO_TCHAR(ResID);
	FString Filter = FString::Printf(TEXT("*_%s.*"), *ID);
	IFileManager::Get().FindFilesRecursive(FoundFiles, *DirName, *Filter, true, false, false);

	if (FoundFiles.Num() > 0)
	{
		FString Filename = FPaths::ConvertRelativePathToFull(FoundFiles[0]);
		AnsiFilename = TCHAR_TO_ANSI(*Filename);
		return AnsiFilename.c_str();
	}

	return nullptr;
}

const char *SystemUtilsImpl::GetSaveDirectory()
{
	FString SavePath = FPaths::ProjectContentDir();
	static std::string sPath;
	sPath = TCHAR_TO_ANSI(*SavePath);
	return sPath.c_str();
}

static std::string tempStr;

const char * SystemUtilsImpl::GetPluginDirectory()
{
	FString LibraryPath;
	FString BaseDir = IPluginManager::Get().FindPlugin("BuildingSDK")->GetBaseDir();
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/BuildingSDKLibrary/Win64"));
#endif
	LibraryPath = FPaths::ConvertRelativePathToFull(LibraryPath);
	tempStr = TCHAR_TO_ANSI(*LibraryPath);
	return tempStr.c_str();
}

void *SystemUtilsImpl::LoadDllFunction(const char *DLLFilename, const char *FuncName)
{
	void *Func = nullptr;
	FString LibraryPath = ANSI_TO_TCHAR(DLLFilename);
	void *LibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	if (LibraryHandle)
	{
		FString FunctionName = ANSI_TO_TCHAR(FuncName);
		Func = FPlatformProcess::GetDllExport(LibraryHandle, *FunctionName);
	}
	return Func;
}

int SystemUtilsImpl::FindFiles(const char **&OutFiles, const char *Directory, const char *FileExt, bool bReclusive)
{
	TArray<FString> Filenames;
	FString StartDirectory = ANSI_TO_TCHAR(Directory);
	FString FilterName = FString::Printf(TEXT("*.%s"), ANSI_TO_TCHAR(FileExt)) ;
	IFileManager::Get().FindFilesRecursive(Filenames, *StartDirectory, *FilterName, true, false, false);
	
	TempStrPtr.resize(Filenames.Num());
	TempStrings.resize(Filenames.Num());

	for (int32 i = 0; i < Filenames.Num(); ++i)
	{
		std::string AnsiFilename = TCHAR_TO_ANSI(*Filenames[i]);
		TempStrings[i] = AnsiFilename;
		TempStrPtr[i] = TempStrings[i].c_str();
	}

	OutFiles = TempStrPtr.empty()? nullptr : &TempStrPtr[0];
	
	return (int)TempStrPtr.size();
}

bool SystemUtilsImpl::GetQueryURL(const char *ResID, const char *&URL, const char *&JsonPost)
{
	return false;
}

void SystemUtilsImpl::Log(ELOGLevel Level, const char *msgInfo)
{
	FString Temp= ANSI_TO_TCHAR(msgInfo);
	UE_LOG(LogTemp, Log, TEXT("building sdk use file[%s]"), *Temp);
}


