// Copyright© 2017 ihomefnt All Rights Reserved.


#include "CornerActor.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "../UserInterface/CornerSnapPntWidget.h"
#include "EditorGameInstance.h"

ACornerActor::ACornerActor(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));

	CornerHudWidget = nullptr;

	CornerHudRadius = 0.0f;

	bShowInnerCircle = bShowOuterCircle = false;
}

bool ACornerActor::IsCornerOuterCircleShow() const
{
	return bShowOuterCircle;
}

void ACornerActor::ShowInnerCircel(bool bShow /*= false*/)
{
	bShowInnerCircle = bShow;

	if (CornerHudWidget && bShowOuterCircle)
	{
		CornerHudWidget->ShowInnerCircel(bShowInnerCircle);
	}
}

void ACornerActor::ShowOutterCircle(bool bShow /*= false*/)
{
	bShowOuterCircle = bShow;

	UWorld *MyWorld = GetWorld();
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
	UWallBuildSystem *WallSystem = GameInst->WallBuildSystem;
	bool bShowWidget = WallSystem->bShowConnectedPnts && bShowOuterCircle;
	if (bShowWidget)
	{
		if (!CornerHudWidget)
		{
			UClass *WidgetClass = WallSystem->CornerHudWidgetClass.Get();
			if (WidgetClass && WidgetClass->IsChildOf(UCornerSnapPntWidget::StaticClass()))
			{
				CornerHudWidget = Cast<UCornerSnapPntWidget>(CreateWidget<UUserWidget>(GetWorld(), WallSystem->CornerHudWidgetClass));
				CornerHudWidget->AddToViewport(-1);

				CornerHudWidget->InitCornerActor(this);
				FVector2D CornerWidgetSize = CornerHudWidget->GetWidgetSize();
				CornerHudRadius = FGenericPlatformMath::Abs<float>(CornerWidgetSize.X * 0.5f);
			}
		}
	}
	else
	{
		if (CornerHudWidget)
		{
			CornerHudWidget->RemoveFromParent();
			CornerHudWidget = nullptr;
		}
	}

	if (!CornerHudWidget)
	{
		return;
	}

	CornerHudWidget->SetVisibility(ESlateVisibility::Visible);
	FVector CornerActorPos = FVector(CornerProperty.Position, 0.0f);
	SetActorLocation(CornerActorPos);

	if (CornerHudWidget)
	{
		FVector2D CornerScreenPos;
		GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(CornerActorPos, CornerScreenPos);
		CornerHudWidget->ShowOutterCircle(bShow);
	}
}

void ACornerActor::UpdateCornerCircleShow()
{
	ShowOutterCircle(bShowOuterCircle);

	ShowInnerCircel(bShowInnerCircle);
}

bool ACornerActor::IsPntInCornerWidget(const FVector2D &PosOnWorld)
{
	APlayerController *PlayerCtrl = GetWorld()->GetFirstPlayerController();
	FVector2D PosOnScreen;
	PlayerCtrl->ProjectWorldLocationToScreen(FVector(PosOnWorld, 0), PosOnScreen);
	FVector2D CornerPosOnScreen;
	PlayerCtrl->ProjectWorldLocationToScreen(FVector(CornerProperty.Position, 0), CornerPosOnScreen);
	return FVector2D::Distance(CornerPosOnScreen, PosOnScreen) < CornerHudRadius;
}

void ACornerActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACornerActor::BeginDestroy()
{
	Super::BeginDestroy();
}

void ACornerActor::Destroyed()
{
	if (CornerHudWidget)
	{
		CornerHudWidget->RemoveFromParent();
	}

	Super::Destroyed();
}

void ACornerActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
