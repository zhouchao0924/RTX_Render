// Copyright? 2017 ihomefnt All Rights Reserved.

#include "AutoLayoutFunctionLibrary.h"
#include "HAL/FileManager.h"
#include "Kismet/KismetMathLibrary.h"

FDateTime UAutoLayoutFunctionLibrary::GetFileCreatedTime(const FString filePath)
{
	IFileManager& FileManager = IFileManager::Get();
	return FileManager.GetTimeStamp(*filePath);
}

bool UAutoLayoutFunctionLibrary::DeleteOldestFile(UPARAM(Ref)TArray<FString>& fileList, FString & deletedFile, FString AppendPath)
{
	IFileManager& FileManager = IFileManager::Get();
	FDateTime dateTime = FDateTime::Now();
	TMap<FDateTime, FString> FileCreateTimeMap;

	if (!AppendPath.EndsWith("/"))
	{
		AppendPath += "/";
	}

	for (int i = 0; i < fileList.Num(); i++)
	{
		FString filePath = AppendPath + fileList[i];
		if (!FileManager.FileExists(*filePath))
		{
			UE_LOG(LogInit, Warning, TEXT("File Is Not Find : %s"), *filePath);
			return false;
		}
		FDateTime dt = GetFileCreatedTime(filePath);
		FileCreateTimeMap.Add(dt, filePath);

		dateTime = FMath::Min(dateTime,dt);
	}

	deletedFile = *FileCreateTimeMap.Find(dateTime);

	if (FileManager.Delete(*deletedFile))
	{
		FString Lfile,Rfile;
		deletedFile.Split("/", &Lfile, &Rfile, ESearchCase::Type::IgnoreCase, ESearchDir::FromEnd);
		fileList.Remove(Rfile);
		return true;
	}
	else
	{
		return false;
	}
}
