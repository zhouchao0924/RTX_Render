// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "UObject/NoExportTypes.h"
#include "BaseCmd.h"
#include "CommandMgr.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AJDR_API UCommandMgr : public UObject
{
	GENERATED_BODY()
	
public:
	UCommandMgr();

	static UCommandMgr* InitInstance(UObject *Outer);

	UFUNCTION(BlueprintCallable, Category = "Manager", meta =(DisplayName = "GetCommandMgr"))
	static UCommandMgr* GetInstance();

	UFUNCTION(BlueprintCallable, Category = "CommandMgr | Excute", meta = (WorldContext = "WorldContextObject"))
	static UBaseCmd* ExecuteCommand(UObject *WorldContextObject, TSubclassOf<UBaseCmd> CmdClass);

	UFUNCTION(BlueprintPure)
	UBaseCmd* GetCurrentCmd();

protected:
	UBaseCmd* ExecuteMyCommand(UObject *WorldContextObject, TSubclassOf<UBaseCmd> CmdClass);

protected:
friend class UBaseCmd;
	UPROPERTY()
	UBaseCmd *CurrentCommand;
	
	static UCommandMgr *CmdMgr;
};
