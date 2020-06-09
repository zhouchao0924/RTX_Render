#pragma once
#include "CoreMinimal.h"
#include "ComponentBasePrimitive.h"
#include "PillarComponentPrimitive.generated.h"


UCLASS()
class UPillarComponentPrimitive :public UComponentBasePrimitive
{
	GENERATED_BODY()
public:
	UPillarComponentPrimitive();
	virtual void Draw(FPaintContext& InContext) const;
	virtual void PrimitiveVertexDataGenerate(UWorld* InMyWorld ,const FVector2D& mousePos);
	virtual void SetFrameColor(const FLinearColor Color);
public:

	UPROPERTY()
	FLinearColor FrameColor;
	UPROPERTY()
	float proportion;
};

