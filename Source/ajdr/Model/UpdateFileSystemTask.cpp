
#include "UpdateFileSystemTask.h"
#include "ResourceMgr.h"

FUpdateFileSystemAsyncTask::~FUpdateFileSystemAsyncTask()
{
}

void FUpdateFileSystemAsyncTask::DoWork()
{
	if (ResMgr != NULL)
	{
		TArray<FString> FoundDirectories;

		FoundDirectories.Add(FPaths::ProjectContentDir() + TEXT("_Basic/Materials/BaseSx/"));
		FoundDirectories.Add(FPaths::ProjectContentDir() + TEXT("Geometry/"));
#if	!(BUILD_WEBSERVER && UE_SERVER)
		FoundDirectories.Add(FPaths::ProjectSavedDir() + TEXT("Download/Resources/"));
#endif
		FoundDirectories.Add(FPaths::ProjectSavedDir() + TEXT("Resources/Local/"));

		for (int32 dirIndex = 0; dirIndex < FoundDirectories.Num(); ++dirIndex)
		{
			TArray<FString> FoundFiles;
			FString DirName = FoundDirectories[dirIndex];

			if (ResMgr->IsAbandonUpdateFile())
			{
				break;
			}

			for (int32 i = 0; i < EResType::EResUnknown; ++i)
			{
				if (ResMgr->IsAbandonUpdateFile())
				{
					break;
				}
				bool UseGZFile = false;
				bool success = GConfig->GetBool(TEXT("Iray"), TEXT("UseGZFile"), UseGZFile, GGameIni);
				FString FileFilter;
				if (success && UseGZFile)
				{
					FString Temp = FString(GetResTypeName(EResType(i))) + TEXT(".gz");
					FileFilter = FString::Printf(TEXT("*.%s"), *Temp);
				}
				else
				{ 
					FileFilter = FString::Printf(TEXT("*.%s"), GetResTypeName(EResType(i)));
				}
				IFileManager::Get().FindFilesRecursive(FoundFiles, *DirName, *FileFilter, true, false, false);
			}

			for (int32 i = 0; i < FoundFiles.Num(); ++i)
			{
				if (ResMgr->IsAbandonUpdateFile())
				{
					break;
				}
				FString Filename = FoundFiles[i];
				ResMgr->Preload(Filename, false, false);
			}
		}

		if (!ResMgr->IsAbandonUpdateFile())
		{
			ResMgr->FinishUpdateFile();
		}
	}
}

UResource * FUpdateFileSystemAsyncTask::LoadByID(UResourceMgr *ResMgr, const FString &ResID,int modelid)
{
	UResource *FoundRes = NULL;
	if (ResID.IsEmpty()&& modelid == 0)
	{
		return FoundRes;
	}

	TArray<FString> FoundDirectories;
	FoundDirectories.Add(FPaths::ProjectContentDir() + TEXT("_Basic/Materials/BaseSx/"));
	FoundDirectories.Add(FPaths::ProjectContentDir() + TEXT("Geometry/"));
	FoundDirectories.Add(FPaths::ProjectSavedDir() + TEXT("Download/Resources/"));
	FoundDirectories.Add(FPaths::ProjectSavedDir() + TEXT("Resources/Local/"));
	
	FString MXExternDir;
	if (modelid > 0)
	{
		MXExternDir = ResMgr->IrayMXDir;
		//if (GConfig->GetString(TEXT("Iray"), TEXT("IrayMXDir"), MXExternDir, GGameIni))
		{
			FoundDirectories.Add(MXExternDir);
		}
	}

	for (int32 dirIndex = 0; dirIndex < FoundDirectories.Num(); ++dirIndex)
	{
		TArray<FString> FoundFiles;
		FString DirName = FoundDirectories[dirIndex];
		bool UseGZFile = false;
		bool success = GConfig->GetBool(TEXT("Iray"), TEXT("UseGZFile"), UseGZFile, GGameIni);
		FString FileFilter;
		for (int32 i = 0; i < EResType::EResUnknown; ++i)
		{
			if (modelid > 0)
			{
				if (ResMgr->RunModeid == 1 && DirName == MXExternDir)//新资源调度 如果是共享盘
				{
					UE_LOG(LogResMgr, Log, TEXT("search "));
					FString pathOne = FString::Printf(TEXT("%d/"), modelid % 256);
					FString pathTwo = FString::Printf(TEXT("%d/"), modelid);
					if (success && UseGZFile)
					{
						FString Temp = FString(GetResTypeName(EResType(i))) + TEXT(".gz");
						FileFilter = FString::Printf(TEXT("%d_*.%s"), modelid, *Temp);
					}
					else
					{
						FileFilter = FString::Printf(TEXT("%d_*.%s"), modelid, GetResTypeName(EResType(i)));
					}
					//FString FileFilter = FString::Printf(TEXT("%d_*.%s"),modelid, GetResTypeName(EResType(i)));
					DirName = DirName + pathOne + pathTwo;
					IFileManager::Get().FindFilesRecursive(FoundFiles, *DirName, *FileFilter, true, false, false);
					UE_LOG(LogResMgr, Log, TEXT("RunModeid newpath :%s "), *FileFilter);
				}
				else
				{
					if (success && UseGZFile)
					{
						FString Temp = FString(GetResTypeName(EResType(i))) + TEXT(".gz");
						FileFilter = FString::Printf(TEXT("%d_*.%s"), modelid, *Temp);
					}
					else
					{
						FileFilter = FString::Printf(TEXT("%d_*.%s"), modelid, GetResTypeName(EResType(i)));
					}
					//FString FileFilter = FString::Printf(TEXT("%d_*.%s"), modelid, GetResTypeName(EResType(i)));
					IFileManager::Get().FindFilesRecursive(FoundFiles, *DirName, *FileFilter, true, false, false);
				}
				
			}
			else
			{
				if (success && UseGZFile)
				{
					FString Temp = FString(GetResTypeName(EResType(i))) + TEXT(".gz");
					FileFilter = FString::Printf(TEXT("*_%s.%s"), *ResID, *Temp);
				}
				else
				{
					FileFilter = FString::Printf(TEXT("*_%s.%s"), *ResID, GetResTypeName(EResType(i)));
				}
				//FString FileFilter = FString::Printf(TEXT("*_%s.%s"), *ResID, GetResTypeName(EResType(i)));
				IFileManager::Get().FindFilesRecursive(FoundFiles, *DirName, *FileFilter, true, false, false);
			}
			
		}

		if (FoundFiles.Num() > 0)
		{
			FString Filename = FoundFiles[0];
			FoundRes = ResMgr->Preload(Filename, true, false);
			if (!FoundRes)
			{
				UE_LOG(LogResMgr, Error, TEXT("Preload file :%s failed!"), *FoundFiles[0]);
			}
			return FoundRes;
		}
	}

	if (!FoundRes)
	{
		UE_LOG(LogResMgr, Error, TEXT("Preload file :%s not found!"), *ResID);
	}

	return FoundRes;
}

UResource *FUpdateFileSystemAsyncTask::LoadByFilename(UResourceMgr *ResMgr, const FString &Filename, bool bNeedHeader)
{
	UResource *Resource = NULL;
	if (IFileManager::Get().FileExists(*Filename))
	{
		Resource = ResMgr->Preload(Filename, bNeedHeader, false);
	}
	else
	{
		UE_LOG(LogResMgr, Error, TEXT("Preload file :%s failed!"), *Filename);
	}
	return Resource;
}


