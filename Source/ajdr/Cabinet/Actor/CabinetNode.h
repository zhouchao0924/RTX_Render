#pragma once
#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ModelFileComponent.h"
#include "../base/common.h"
#include "CabinetNode.generated.h"
//≤‚ ‘”√£¨ø……æ≥˝
UCLASS(BlueprintType)
class ACabinetNode : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	bool Init(TRectBase CabinetSet);
protected:
	UModelFileComponent *mModel;
};
