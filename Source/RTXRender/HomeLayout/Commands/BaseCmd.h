// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "UObject/NoExportTypes.h"
#include "BaseCmd.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RTXRENDER_API UBaseCmd : public UObject
{
	GENERATED_BODY()
public:
	bool InitCmd(UObject *WorldContextObject);

	virtual void CommandWillStart();
	
	virtual void ExecuteCommand();
	
	virtual void CommandWillEnd();

	virtual void CancelCommand();

protected:
	virtual void Go2DefaultCommand();

public:
	virtual void CommandTick(float DeltaTime);

	virtual void OnLeftMouseButtonDown();

	virtual void OnLeftMouseButtonUp();

	virtual void OnRightMouseButtonDown();

	virtual void OnRightMouseButtonUp();

	virtual void OnEscapeDown();

protected:
	virtual class UWorld* GetWorld() const;

protected:
	UPROPERTY()
	UObject *MyWorldContextObject;

	UPROPERTY()
	UWorld *MyWorld;

	UPROPERTY()
	APlayerController *OwningPlayer;
};
