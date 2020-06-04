#pragma once
#include "CoreMinimal.h"
#include "ComponentBasePrimitive.h"
#include "BagRiserComponentPrimitive.generated.h"


UCLASS()
class UBagRiserComponentPrimitive :public UComponentBasePrimitive
{
	GENERATED_BODY()
public:
	UBagRiserComponentPrimitive();
	virtual void Draw(FPaintContext& InContext) const;
	virtual void PrimitiveVertexDataGenerate(UWorld* InMyWorld ,const FVector2D& mousePos);
public:
	UPROPERTY()
	FLinearColor FrameColor;
	UPROPERTY()
	float proportion;

};

