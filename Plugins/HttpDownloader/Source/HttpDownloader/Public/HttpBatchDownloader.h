// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpDownloadAction.h"
#include "HAL/CriticalSection.h"

#include "HttpBatchDownloader.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FBatchDownloadResult
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Download)
	FDownloadActionInfo		ActionInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Download)
	bool					bSuccess;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FBatchDownloadCompletedDelegate, const TArray<FBatchDownloadResult>&, Result);
DECLARE_DYNAMIC_DELEGATE_OneParam(FBatchDownloadProcessDelegate, FBatchDownloadResult, ActionInfo);

UCLASS(BlueprintType)
class HTTPDOWNLOADER_API UHttpBatchDownloader : public UObject, public TSharedFromThis<UHttpBatchDownloader>
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = Download)
	static UHttpBatchDownloader* BatchDownloadFiles(const TArray<FDownloadActionInfo>& InFiles, const FBatchDownloadCompletedDelegate& DownloadCompletedCallback, const FBatchDownloadProcessDelegate& ProcessingCallback);

	UFUNCTION(BlueprintCallable, Category = Download)
	void StopAllAction();

	void StartBackgroundTask(const TArray<FDownloadActionInfo>& InFiles);

	void HanldeOnDownloadActionProcessing(FDownloadActionInfo ActionInfo, bool bSucceeded);
public:

	FBatchDownloadCompletedDelegate		OnDownloadCompleted;

	FBatchDownloadProcessDelegate		OnDownloadProcessing;

protected:

	TArray<FBatchDownloadResult>		CompletedQueue;

	TMap<FString, UHttpDownloadAction*>	PendingQueue;

	FCriticalSection					Mutex;

	int32								ActionCount;
};
