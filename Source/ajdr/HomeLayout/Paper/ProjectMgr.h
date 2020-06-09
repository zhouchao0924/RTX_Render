// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "ProjectPaper.h"
#include "ProjectMgr.generated.h"


UCLASS(BlueprintType)
class AJDR_API UProjectManager : public UObject
{
	GENERATED_BODY()
	
public:
	UProjectManager();

	void InitGameInst(class UCEditorGameInstance *InGameInst);

// you can only get a paper from project manager
	UProjectPaper* CreateNewPaper();

	void SwitchPaper(UProjectPaper *NewPaper);

	UFUNCTION(BlueprintPure)
	UProjectPaper* GetProjectPaper();
	
protected:
	UPROPERTY(BlueprintReadOnly)
	UProjectPaper		*ThisPaper;
	UPROPERTY(BlueprintReadOnly)
	UProjectPaper		*LastPaper;

	UPROPERTY()
	class UCEditorGameInstance *GameInst;
};
