// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ScreenShootAsyncTask.generated.h"
/**
*
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScreenShootDelegate,FString,OutPath);


UENUM(BlueprintType)
enum class EImageFormatType : uint8
{
	/** Portable Network Graphics. */
	PNG,

	/** Joint Photographic Experts Group. */
	JPEG,

	/** OpenEXR (HDR) image file format. */
	EXR,
};


UCLASS()
class  UScreenShootAsyncTask : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Excel|Utilities")
		static UScreenShootAsyncTask* ScreenShoot(FString InputName, EImageFormatType Format, int32 InWidth, int32 InHeight, int Quality);

public:

	UPROPERTY(BlueprintAssignable)
		FScreenShootDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FScreenShootDelegate OnFail;

public:

	void Start(FString InputName, int32 InWidth, int32 InHeight);

private:
	FDelegateHandle delegateHandle;
	FString fileName;
	EImageFormatType ImageFormat;
	int ImageQuality;

	/** Handles image requests coming from the web */
	void OnScreenShootComplete(int32 Width, int32 Height, const TArray<FColor>& Bitmap);
};
