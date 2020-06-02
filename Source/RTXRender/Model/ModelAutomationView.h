// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResourceMgr.h"
#include "ObjFileAnsyTask.h"
#include "Editor/Model/PDIDrawComponent.h"
#include "ModelAutomationView.generated.h"

class USurfaceFile;
class UResourceMgr;
class UModelFileComponent;

UCLASS(BlueprintType)
class UModelAutomationView : public UUserWidget
{
	GENERATED_UCLASS_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "DR|ModelAutomation")
		void SetPreviewResource(const FString &resid);
	
	UFUNCTION()
		void OnResourceStateChanged(UResource *Resource, EResState ResState, UVaRestJsonObject * DRInfo);

	UFUNCTION(BlueprintImplementableEvent, Category = "DR|ModelAutomation")
		void OnEditorResource(UResource *Resource);

	UFUNCTION(BlueprintCallable, Category = "DR|Model")
		void QuarantineMaterialByIndex(int32 PartIndex, int32 SlotIndex, bool IsVisible);

protected:
	void OnPreviewResource(UResource *Resource);

protected:
	UPROPERTY(Transient)
		UResource					*PendingResource;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "DR|ModelAutomation")
		UResource					*PreviewResource;
	UPROPERTY(Transient)
		UModelFileComponent			*ModelFileComponent;
	UPROPERTY(Transient, BlueprintReadOnly, Category = "DR|ModelAutomation")
		AActor						*PreviewActor;

	UPDIDrawComponent				*DrawComponent;

};
