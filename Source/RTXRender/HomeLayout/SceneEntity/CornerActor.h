// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "../DataNodes/WallNode.h"
#include "CornerActor.generated.h"

/**
 * actor in interior scene
 * THome Src Class: 
 * Map Base Object
 */
UCLASS()
class AJDR_API ACornerActor : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	bool IsCornerOuterCircleShow() const;

	void ShowInnerCircel(bool bShow = false);

	void ShowOutterCircle(bool bShow = false);

	void UpdateCornerCircleShow();

	bool IsPntInCornerWidget(const FVector2D &PosOnWorld);

protected:
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	virtual void Destroyed() override;

	virtual void Tick(float DeltaSeconds) override;
	
public:
	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "Property"))
	FCornerNode CornerProperty;

	UPROPERTY()
	class UCornerSnapPntWidget *CornerHudWidget;

	UPROPERTY()
	float CornerHudRadius;

	UPROPERTY()
	bool bShowInnerCircle;
	UPROPERTY()
	bool bShowOuterCircle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsSelect;
};
