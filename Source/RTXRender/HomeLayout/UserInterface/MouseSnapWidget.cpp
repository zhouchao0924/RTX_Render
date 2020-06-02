// Copyright© 2017 ihomefnt All Rights Reserved.


#include "MouseSnapWidget.h"
#include "../SceneEntity/CornerActor.h"
#include "../SceneEntity/LineWallActor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRSolidWallAdapter.h"

void UMouseSnapWidget::NativeConstruct()
{
	SnapXCorner = SnapYCorner = nullptr;
	SnapWall = nullptr;
	OwningPlayer = GetWorld()->GetFirstPlayerController();
}

void UMouseSnapWidget::SetSnapMousePos(class ACornerActor *InSnapXCorner, class ACornerActor *InSnapYCorner, 
	class ALineWallActor *InSnapWall, const FVector2D &InSnapPnt)
{
	SnapXCorner = InSnapXCorner;
	SnapYCorner = InSnapYCorner;

	SnapWall = InSnapWall;

	SnapPnt = InSnapPnt;
}

void UMouseSnapWidget::NativePaint(FPaintContext& InContext) const
{
	FVector2D ScreenSnapPos;
	OwningPlayer->ProjectWorldLocationToScreen(FVector(SnapPnt, 0), ScreenSnapPos);
	const float LargeLineLength = 10000.0f;
	const float MouseLineLength = 20.0f;

	if (SnapXCorner)
	{
		UWidgetBlueprintLibrary::DrawLine(InContext, ScreenSnapPos - FVector2D(LargeLineLength, 0.0f), 
			ScreenSnapPos + FVector2D(LargeLineLength, 0.0f), DRAWLINE_SNAPCOLOR);
	}
	else
	{
		UWidgetBlueprintLibrary::DrawLine(InContext, ScreenSnapPos - FVector2D(MouseLineLength, 0.0f),
			ScreenSnapPos + FVector2D(MouseLineLength, 0.0f), FLinearColor::White);
	}

	if (SnapYCorner)
	{
		UWidgetBlueprintLibrary::DrawLine(InContext, ScreenSnapPos - FVector2D(0.0f, LargeLineLength),
			ScreenSnapPos + FVector2D(0.0f, LargeLineLength), DRAWLINE_SNAPCOLOR);
	}
	else
	{
		UWidgetBlueprintLibrary::DrawLine(InContext, ScreenSnapPos - FVector2D(0.0f, MouseLineLength),
			ScreenSnapPos + FVector2D(0.0f, MouseLineLength), FLinearColor::White);
	}

	if (SnapWall)
	{
		UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(SnapWall);
		if (projectDataManager == nullptr) {
			return;
		}

		UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(SnapWall->ObjectId));
		if (solidWallAdapter == nullptr) {
			return;
		}

		FVector2D wallStartPos(solidWallAdapter->GetWallPositionData().StartPos.X, solidWallAdapter->GetWallPositionData().StartPos.Y);
		FVector2D wallEndPos(solidWallAdapter->GetWallPositionData().EndPos.X, solidWallAdapter->GetWallPositionData().EndPos.Y);

		FVector2D SnapWallDir = wallEndPos - wallStartPos;
		SnapWallDir.Normalize();

		FVector2D ScreenWallStart, ScreenWallEnd;
		OwningPlayer->ProjectWorldLocationToScreen(solidWallAdapter->GetWallPositionData().StartPos, ScreenWallStart);
		OwningPlayer->ProjectWorldLocationToScreen(solidWallAdapter->GetWallPositionData().EndPos, ScreenWallEnd);

		UWidgetBlueprintLibrary::DrawLine(InContext, ScreenWallStart,
			ScreenWallEnd, DRAWLINE_SNAPCOLOR);

		//UWidgetBlueprintLibrary::DrawLine(InContext, ScreenSnapPos - LargeLineLength * SnapWallDir,
		//	ScreenSnapPos + LargeLineLength * SnapWallDir, DRAWLINE_SNAPCOLOR);
	}
}

void UMouseSnapWidget::DrawDottedLine(FPaintContext& InContext, const FVector2D &StartPos, const FVector2D &EndPos, const FLinearColor &LineColor, float DottedLineLength)
{

}
