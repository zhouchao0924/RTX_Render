// Copyright© 2017 ihomefnt All Rights Reserved.


#include "ProjectMgr.h"


//UProjectManager::UProjectManager(const FObjectInitializer &ObjectInitializer)
//	:Super(ObjectInitializer)
//{
//	LastPaper = ThisPaper = nullptr;
//}

UProjectManager::UProjectManager()
{
	LastPaper = ThisPaper = nullptr;
}

void UProjectManager::InitGameInst(class UCEditorGameInstance *InGameInst)
{
	GameInst = InGameInst;
}

UProjectPaper* UProjectManager::CreateNewPaper()
{
	UProjectPaper *NewPaper = NewObject<UProjectPaper>(this);
	NewPaper->InitGameInst(GameInst);

	return NewPaper;
}

void UProjectManager::SwitchPaper(UProjectPaper *NewPaper)
{
	ThisPaper = NewPaper;
}

UProjectPaper* UProjectManager::GetProjectPaper()
{
	return ThisPaper;
}
