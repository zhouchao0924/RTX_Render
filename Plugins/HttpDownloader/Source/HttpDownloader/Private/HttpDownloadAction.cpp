// Fill out your copyright notice in the Description page of Project Settings.

#include "HttpDownloadAction.h"
#include "Http.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"

UHttpDownloadAction::UHttpDownloadAction(const FObjectInitializer& Initializer)
	:Super(Initializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}

	bStoped = false;
}

void UHttpDownloadAction::StartDownload()
{
	Mutex.Lock();

	if (!bStoped)
	{
		switch (GetStatus())
		{
		case EActionStatus::NotInit:
		{
			UE_LOG(LogTemp, Display, TEXT("Action %s is a invalid process."), *Info.ActionGuid);
			RemoveFromRoot();
			DownloadCompleted.Broadcast(Info, false);
			break;
		}

		case EActionStatus::NotStart:
		{
			if (Info.FileName.IsEmpty())
			{
				FString DirtyStr;

				Info.URL.Split(TEXT("/"), &DirtyStr, &Info.FileName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			}

			if (Info.SavePath.IsEmpty())
			{
				Info.SavePath = FPaths::ProjectSavedDir() / TEXT("Download/TempFile");
			}

			RequestHandle->OnProcessRequestComplete().BindUObject(this, &UHttpDownloadAction::HandleDownloadCompleted);
			RequestHandle->SetURL(Info.URL);
			RequestHandle->SetVerb(TEXT("Get"));
			RequestHandle->ProcessRequest();
			break;
		}

		case EActionStatus::Processing:
			break;

		default:
			DownloadCompleted.Broadcast(Info, GetStatus() == EActionStatus::Succeeded ? true : false);
			break;
		}
	}
	else
	{
		RemoveFromRoot();
	}

	Mutex.Unlock();
}

void UHttpDownloadAction::HandleDownloadCompleted(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	Mutex.Lock();

	RemoveFromRoot();

	if (!bStoped)
	{
		if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == EHttpResponseCodes::Ok && HttpResponse->GetContentLength() > 0)
		{
			if (IFileManager::Get().MakeDirectory(*Info.SavePath, true))
			{
				FString FullDownloadPath = Info.SavePath / Info.FileName;
				if (FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *FullDownloadPath))
				{
					UE_LOG(LogTemp, Display, TEXT("Action %s completed, the downloaded file is %s."), *Info.ActionGuid, *(Info.SavePath / Info.FileName));
					DownloadCompleted.Broadcast(Info, true);
					bStoped = true;
					return;
				}
			}
		}

		DownloadCompleted.Broadcast(Info, false);

		bStoped = true;
	}

	Mutex.Unlock();
}

void UHttpDownloadAction::Stop()
{
	Mutex.Lock();

	if (!bStoped)
	{
		if (RequestHandle.IsValid())
		{
			RequestHandle->CancelRequest();

			RequestHandle->OnProcessRequestComplete().Unbind();
		}

		bStoped = true;
	}

	UE_LOG(LogTemp, Display, TEXT("Action %s stopped."), *Info.ActionGuid);
	
	Mutex.Unlock();
}

void UHttpDownloadAction::SetActionInfo(const FDownloadActionInfo & InInfo)
{
	if (GetStatus() == EActionStatus::NotInit)
	{
		Info = InInfo;

		RequestHandle = MakeShareable(FPlatformHttp::ConstructRequest());
	}
}

EActionStatus UHttpDownloadAction::GetStatus()
{
	if (!Info.URL.IsEmpty() && !bStoped)
	{
		if (RequestHandle.IsValid())
		{
			switch (RequestHandle->GetStatus())
			{
			case EHttpRequestStatus::Type::NotStarted:
				return EActionStatus::NotStart;

			case EHttpRequestStatus::Type::Processing:
				return EActionStatus::Processing;

			case EHttpRequestStatus::Type::Failed:
				return EActionStatus::Failed;

			case EHttpRequestStatus::Type::Failed_ConnectionError:
				return EActionStatus::ConnectionError;

			case EHttpRequestStatus::Type::Succeeded:
				return EActionStatus::Succeeded;
			}
		}
	}

	return EActionStatus::NotInit;
}
