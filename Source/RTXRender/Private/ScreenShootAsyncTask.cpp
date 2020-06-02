// Copyright© 2017 ihomefnt All Rights Reserved.

#include "ScreenShootAsyncTask.h"
#include "IImageWrapperModule.h"


UScreenShootAsyncTask::UScreenShootAsyncTask(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UScreenShootAsyncTask* UScreenShootAsyncTask::ScreenShoot(FString InputName, EImageFormatType Format, int32 InWidth, int32 InHeight, int Quality)
{
	UScreenShootAsyncTask* ScreenShootTask = NewObject<UScreenShootAsyncTask>();
	ScreenShootTask->ImageFormat = Format;
	ScreenShootTask->ImageQuality = Quality;

	ScreenShootTask->Start(InputName, InWidth, InHeight);

	return ScreenShootTask;
}

void UScreenShootAsyncTask::Start(FString InputName, int32 InWidth, int32 InHeight)
{
#if !UE_SERVER
	FString ScreenShootPath;
	UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
	if (GameEngine != nullptr)
	{
		TSharedPtr<SWindow> GameViewportWindow = GameEngine->GameViewportWindow.Pin();
		if (GameViewportWindow.IsValid())
		{
			ScreenShootPath = "ScreenShots/WindowsNoEditor/";
		}
		else
		{
			ScreenShootPath = "Screenshots/Windows/";
		}
	}
	else
	{
		ScreenShootPath = "Screenshots/Windows/";
	}
	if (InputName == "")
	{
		InputName = "ScreenShoot";
	}
	fileName = FPaths::ProjectSavedDir() + ScreenShootPath + InputName;
	//FScreenshotRequest::RequestScreenshot(fileName, false, false);
	delegateHandle = UGameViewportClient::OnScreenshotCaptured().AddUObject(this, &UScreenShootAsyncTask::OnScreenShootComplete);
	
	if (InWidth && InHeight)
	{
		FString Paramter(TEXT("HighResShot ") + FString::FromInt(InWidth) + TEXT("x") + FString::FromInt(InHeight));
		GEngine->GameViewport->Exec(nullptr, *Paramter, *GLog);
}
	else
	{
		GEngine->GameViewport->Exec(nullptr, TEXT("HighResShot 1920x1080"), *GLog);
	}

#else
	// On the server we don't execute fail or success we just don't fire the request.
	RemoveFromRoot();
#endif
}

void UScreenShootAsyncTask::OnScreenShootComplete(int32 Width, int32 Height, const TArray<FColor>& Bitmap)
{
#if !UE_SERVER

	RemoveFromRoot();

	bool SaveImageSuccess = false;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	if (ImageFormat == EImageFormatType::PNG)
	{
		TArray<uint8> PNGData;
		TArray<FColor> BitmapCopy(Bitmap);
		FImageUtils::CompressImageArray(Width, Height, BitmapCopy, PNGData);

		fileName += ".png";
		SaveImageSuccess = FFileHelper::SaveArrayToFile(PNGData, *fileName);
	}
	else
	{
		TSharedPtr<IImageWrapper> ImageWrapper;
		ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		ImageWrapper->SetRaw(Bitmap.GetData(), Bitmap.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8);
		const TArray<uint8>& JPEGData = ImageWrapper->GetCompressed(ImageQuality);

		fileName += ".jpg";
		SaveImageSuccess = FFileHelper::SaveArrayToFile(JPEGData, *fileName);
	}

	if (SaveImageSuccess)
	{
		OnSuccess.Broadcast(fileName);
	}
	else
	{
		OnFail.Broadcast(TEXT(""));
	}
	UGameViewportClient::OnScreenshotCaptured().Remove(delegateHandle);
#endif
}
