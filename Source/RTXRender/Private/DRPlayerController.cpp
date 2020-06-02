// Copyright© 2017 ihomefnt All Rights Reserved.

#include "DRPlayerController.h"
#include "DRDebug/DRCheatManager.h"


ADRPlayerController::ADRPlayerController(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	CheatClass = UDRCheatManager::StaticClass();
}

