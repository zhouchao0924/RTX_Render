// other infomation node for furniture model actor
#pragma once

#include "ModelConfigLight.h"
#include "FurnitureOtherInfoNode.generated.h"

// model saved config other
USTRUCT(Blueprintable)
struct FFurnitureOtherInfoNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FModelConfigLightCPP> Light;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bBoolSlot1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bBoolSlot2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bBoolSlot3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bBoolSlot4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FloatSlot1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FloatSlot2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FloatSlot3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FloatSlot4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString StringSlot1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString StringSlot2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString StringSlot3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString StringSlot4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform TransformSlot1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform TransformSlot2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform TransformSlot3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform TransformSlot4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor ColorSlot1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor ColorSlot2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor ColorSlot3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor ColorSlot4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UObject *ObjectSlot1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UObject *ObjectSlot2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UObject *ObjectSlot3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UObject *ObjectSlot4;
};