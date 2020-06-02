// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Http.h"
#include "HAL/CriticalSection.h"
#include "HttpDownloadAction.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EActionStatus : uint8
{
	NotInit,
	NotStart,
	Processing,
	Failed,
	ConnectionError,
	Succeeded
};

USTRUCT(BlueprintType)
struct FDownloadActionInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Download)
	FString		URL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Download)
	FString		FileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Download)
	FString		SavePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Download)
	FString		ActionGuid;

public:

	FDownloadActionInfo()
	{
		FGuid	DirtyVal;
		FPlatformMisc::CreateGuid(DirtyVal);

		ActionGuid = DirtyVal.ToString();
	}
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FDownloadCompleteDelegate, FDownloadActionInfo, bool);

UCLASS()
class HTTPDOWNLOADER_API UHttpDownloadAction : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()
public:

	void StartDownload();

	void HandleDownloadCompleted(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void Stop();

	const FDownloadActionInfo& GetInfo() { return Info; };

	void SetActionInfo(const FDownloadActionInfo& InInfo);

	EActionStatus GetStatus();

public:

	FDownloadCompleteDelegate	DownloadCompleted;

protected:

	TSharedPtr<class IHttpRequest>	RequestHandle;

	FDownloadActionInfo				Info;

	FCriticalSection				Mutex;

	bool							bStoped;
};
