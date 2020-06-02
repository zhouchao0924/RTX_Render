// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpBatchDownloader.h"
#include "Kismet/KismetMathLibrary.h"

UHttpBatchDownloader::UHttpBatchDownloader(const FObjectInitializer& Initializer)
	:Super(Initializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UHttpBatchDownloader* UHttpBatchDownloader::BatchDownloadFiles(const TArray<FDownloadActionInfo>& InFiles, const FBatchDownloadCompletedDelegate& DownloadCompletedCallback,
												const FBatchDownloadProcessDelegate& ProcessingCallback)
{
	UHttpBatchDownloader* Result = NewObject<UHttpBatchDownloader>();

	Result->OnDownloadCompleted = DownloadCompletedCallback;
	Result->OnDownloadProcessing = ProcessingCallback;
	Result->StartBackgroundTask(InFiles);

	return Result;
}

void UHttpBatchDownloader::StopAllAction()
{
	Mutex.Lock();

	if (PendingQueue.Num() != 0)
	{
		TArray<FString> ActionGuidList;

		PendingQueue.GenerateKeyArray(ActionGuidList);

		for (int32 Index = 0; Index < ActionGuidList.Num(); Index++)
		{
			if (PendingQueue[ActionGuidList[Index]] != nullptr)
			{
				UE_LOG(LogTemp, Display, TEXT("Action %s is working, will stop it now."), *ActionGuidList[Index]);

				PendingQueue[ActionGuidList[Index]]->Stop();
			}
		}

		CompletedQueue.Empty();

		PendingQueue.Empty();

		RemoveFromRoot();
	}

	Mutex.Unlock();
}

void UHttpBatchDownloader::StartBackgroundTask(const TArray<FDownloadActionInfo>& InFiles)
{
	if (InFiles.IsValidIndex(0))
	{
		ActionCount = InFiles.Num();

		for (int32 Index = 0; Index < InFiles.Num(); Index++)
		{
			FBatchDownloadResult DirtyResult;
			DirtyResult.ActionInfo = InFiles[Index];
			DirtyResult.bSuccess = false;

			UHttpDownloadAction* DownloadAction = NewObject<UHttpDownloadAction>();

			DownloadAction->SetActionInfo(InFiles[Index]);
			PendingQueue.Add(InFiles[Index].ActionGuid, DownloadAction);

			DownloadAction->DownloadCompleted.AddUObject(this, &UHttpBatchDownloader::HanldeOnDownloadActionProcessing);
		}

		TArray<UHttpDownloadAction*> ActionList;

		PendingQueue.GenerateValueArray(ActionList);

		Mutex.Lock();

		for (int32 Index = 0; Index < UKismetMathLibrary::Clamp(ActionList.Num(), 0, 12); Index++)
		{
			ActionList[Index]->StartDownload();
		}

		Mutex.Unlock();

		return;
	}

	RemoveFromRoot();
	OnDownloadCompleted.ExecuteIfBound(CompletedQueue);
}

void UHttpBatchDownloader::HanldeOnDownloadActionProcessing(FDownloadActionInfo ActionInfo, bool bSucceeded)
{
	Mutex.Lock();

	if (PendingQueue.Num() != 0)
	{
		if (PendingQueue[ActionInfo.ActionGuid] != nullptr)
		{
			FBatchDownloadResult DirtyResult;
			DirtyResult.bSuccess = bSucceeded;
			DirtyResult.ActionInfo = ActionInfo;

			CompletedQueue.Add(DirtyResult);

			OnDownloadProcessing.ExecuteIfBound(DirtyResult);

			if (CompletedQueue.Num() == ActionCount)
			{
				UE_LOG(LogTemp, Display, TEXT("All action of batch downloader is finished."));

				RemoveFromRoot();
				OnDownloadCompleted.ExecuteIfBound(CompletedQueue);
			}
			else
			{
				PendingQueue.Remove(ActionInfo.ActionGuid);
				TArray<FString> GuidList;

				PendingQueue.GenerateKeyArray(GuidList);

				for (int32 Index = 0; Index < GuidList.Num(); Index++)
				{
					if (PendingQueue[GuidList[Index]]->GetStatus() == EActionStatus::NotStart)
					{
						PendingQueue[GuidList[Index]]->StartDownload();
						break;
					}
				}
			}
		}
	}

	Mutex.Unlock();
}
