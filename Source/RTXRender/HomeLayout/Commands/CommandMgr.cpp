// Copyright© 2017 ihomefnt All Rights Reserved.


#include "CommandMgr.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UCommandMgr * UCommandMgr::CmdMgr = NULL;

UCommandMgr::UCommandMgr()
{
	CurrentCommand = nullptr;
}

UCommandMgr* UCommandMgr::InitInstance(UObject *Outer)
{
	CmdMgr = NewObject<UCommandMgr>(Outer);

	return CmdMgr;
}

UCommandMgr* UCommandMgr::GetInstance()
{
	return CmdMgr;
}

UBaseCmd* UCommandMgr::ExecuteCommand(UObject *WorldContextObject, TSubclassOf<UBaseCmd> CmdClass)
{
	return CmdMgr->ExecuteMyCommand(WorldContextObject, CmdClass);
}

UBaseCmd* UCommandMgr::GetCurrentCmd()
{
	return CurrentCommand;
}

UBaseCmd* UCommandMgr::ExecuteMyCommand(UObject *WorldContextObject, TSubclassOf<UBaseCmd> CmdClass)
{
	UBaseCmd *LastCommand = CurrentCommand;
	CurrentCommand = nullptr;
	if (LastCommand)
	{
		LastCommand->CancelCommand();
	}

	UClass *BaseCmdClass = CmdClass.Get();
	if (BaseCmdClass)
	{
		CurrentCommand = NewObject<UBaseCmd>(WorldContextObject, BaseCmdClass);
		if (CurrentCommand->InitCmd(WorldContextObject))
		{
			CurrentCommand->CommandWillStart();
			CurrentCommand->ExecuteCommand();
		}
		else
		{
			CurrentCommand = nullptr;
		}
	}
	else
	{
		if (LastCommand)
		{
			LastCommand->CommandWillEnd();
		}
	}

	return CurrentCommand;
}

