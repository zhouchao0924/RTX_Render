// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once


#include "Components/ActorComponent.h"
#include "../SceneEntity/CornerActor.h"
#include "../SceneEntity/LineWallActor.h"
#include "../SceneEntity/RoomActor.h"

#include "WallLinkage.generated.h"


UCLASS(BlueprintType, Blueprintable)
class RTXRENDER_API UWallLinkageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallLinkageComponent();

	void InitLinkage();

	UFUNCTION(BlueprintCallable)
	void AutoBuildAreaByWalls(bool bReserveCurArea);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	
protected:
	friend class UWallBuildSystem;
	friend class FAutoBuildArea;
	UPROPERTY()
	class UWallBuildSystem* WallSystem;

	void *WallArrangement;
	void *WallArrObserver;

protected:
	//ALineWallActor* FindWallByPoint(const class FExactPoint2D& FirstPnt, const class FExactPoint2D& SecondPnt, bool &bSameDir);

	//class FExactPoint2D GetPointByCorner(const FString &CornerID);

	//FCornerNode GetCornerByPoint(const class FExactPoint2D&PointOfCorner);
};
