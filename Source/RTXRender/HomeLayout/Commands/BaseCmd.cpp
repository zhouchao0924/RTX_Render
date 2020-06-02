// Copyright© 2017 ihomefnt All Rights Reserved.


#include "BaseCmd.h"
#include "CommandMgr.h"

bool UBaseCmd::InitCmd(UObject *WorldContextObject)
{
	bool bInitOk = false;
	if (WorldContextObject)
	{
		MyWorldContextObject = WorldContextObject;
		MyWorld = GetWorld();
		if (MyWorld)
		{
			bInitOk = true;
			OwningPlayer = MyWorld->GetFirstPlayerController();
		}
	}

	return bInitOk;
}

void UBaseCmd::CommandWillStart()
{
	
}

void UBaseCmd::ExecuteCommand()
{
	
}

void UBaseCmd::CommandWillEnd()
{
	UCommandMgr *MyCmdMgr = UCommandMgr::GetInstance();
	MyCmdMgr->CurrentCommand = nullptr;

	Go2DefaultCommand();
}

void UBaseCmd::CancelCommand()
{
	
}

void UBaseCmd::Go2DefaultCommand()
{

}

void UBaseCmd::CommandTick(float DeltaTime)
{
	
}

void UBaseCmd::OnLeftMouseButtonDown()
{

}

void UBaseCmd::OnLeftMouseButtonUp()
{

}

void UBaseCmd::OnRightMouseButtonDown()
{

}

void UBaseCmd::OnRightMouseButtonUp()
{

}

void UBaseCmd::OnEscapeDown()
{

}

class UWorld* UBaseCmd::GetWorld() const
{
	return MyWorldContextObject->GetWorld();
}

