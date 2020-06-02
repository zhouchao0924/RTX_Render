// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MouseSnapWidget.generated.h"

/**
 * 
 */
UCLASS()
class RTXRENDER_API UMouseSnapWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct();

public:
	void SetSnapMousePos(class ACornerActor *InSnapXCorner, class ACornerActor *InSnapYCorner, 
		class ALineWallActor *InSnapWall, const FVector2D &InSnapPnt);
	
protected:
	virtual void NativePaint(FPaintContext& InContext) const;

	void DrawDottedLine(FPaintContext& InContext, const FVector2D &StartPos, const FVector2D &EndPos, 
		const FLinearColor &LineColor, float DottedLineLength);

protected:
	UPROPERTY()
	class ACornerActor *SnapXCorner;
	UPROPERTY()
	class ACornerActor *SnapYCorner;
	UPROPERTY()
	class ALineWallActor *SnapWall;
	UPROPERTY()
	FVector2D SnapPnt;

	UPROPERTY()
	APlayerController *OwningPlayer;
};
