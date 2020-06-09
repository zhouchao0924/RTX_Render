#pragma once
#include "CoreMinimal.h"
#include "base/common.h"
#include "CabinetShell.h"
#include "Actor/TableActor.h"
#include "Actor/SplineActor.h"
#include "CabinetMgr.generated.h"

UCLASS(BlueprintType)
class UCabinetMgr : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	static UCabinetMgr *Get();
	static UCabinetMgr& Instance();
	static UCabinetMgr *CreateInstance(UObject *Outer);
	
	bool Init();
	void UnInit();

	ATableActor* NewTable();

	ASplineActor* NewSpline();

	UWorld *GetWorld() const override;

	int32 GenId() { return ++mIdx; }
	void SetId(int32 Id) { mIdx = Id; }
protected:
	static UCabinetMgr *s_CabinetMgr;
	int32 mIdx;
	int32 mSplineIdx;
};

#define gCabinetMgr UCabinetMgr::Instance()