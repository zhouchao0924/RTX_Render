#pragma once
#include "CoreMinimal.h"
#include "ComponentBasePrimitive.h"
#include "SewerComponentPrimitive.generated.h"

UCLASS()
class USewerComponentPrimitive : public UComponentBasePrimitive
{
	GENERATED_BODY()
public:
	USewerComponentPrimitive();
	virtual void Draw(FPaintContext& InContext) const;
	virtual void PrimitiveVertexDataGenerate(UWorld* InMyWorld, const FVector2D& mousePos);
	//void USewerComponentPrimitive::SetMyBreush()const;
public:
	UPROPERTY()
		FLinearColor FrameColor;
	UPROPERTY()
		float proportion;
};

