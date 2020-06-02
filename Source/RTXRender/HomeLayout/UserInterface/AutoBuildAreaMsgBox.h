// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "AutoBuildAreaMsgBox.generated.h"

/**
 * 
 */
UCLASS()
class AJDR_API UAutoBuildAreaMsgBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UAutoBuildAreaMsgBox(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void ReserveRoom(bool bInAcceptedValue);
	
	UFUNCTION(BlueprintPure)
	bool DoReserveRoom();
	
public:
	static bool bReserveRoom;
};
