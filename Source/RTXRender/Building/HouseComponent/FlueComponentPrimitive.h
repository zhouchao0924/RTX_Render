#pragma once
#include "CoreMinimal.h"
#include "ComponentBasePrimitive.h"
#include "FlueComponentPrimitive.generated.h"


UCLASS()
class UFlueComponentPrimitive :public UComponentBasePrimitive
{
	GENERATED_BODY()
public:
	UFlueComponentPrimitive();
	virtual void Draw(FPaintContext& InContext) const;
	virtual void PrimitiveVertexDataGenerate(UWorld* MyWorld ,const FVector2D& mousePos);
	//void DrawSelectStatePrimitive(FPaintContext& InContext)const;
public:

	UPROPERTY()
	FPrimitive2D InnerframePrimitive;//内框


	UPROPERTY()
	FVector2D ArrowPos;//箭头
	UPROPERTY()
	float proportion;//比例
	UPROPERTY()
	FLinearColor outerFrameColor;
	UPROPERTY()
	FLinearColor innerFrameColor;
};

