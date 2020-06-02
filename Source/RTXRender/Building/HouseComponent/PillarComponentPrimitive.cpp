
#include "PillarComponentPrimitive.h"
#include "WidgetBlueprintLibrary.h"

UPillarComponentPrimitive::UPillarComponentPrimitive()
	:FrameColor(FLinearColor(1, 0.48, 0,1))
{
	ComponenetInfo.ComponenetType = EComponenetType::_Pillar;
	ComponenetInfo.Width = 20.0f;
	ComponenetInfo.Length = 25.0f;
}

void UPillarComponentPrimitive::Draw(FPaintContext& InContext) const
{
	//¿ò
	InContext.MaxLayer++;
	if (ComponenetState == EComponenetState::_Select)
	{
		DrawBox(InContext, FramePrimitive, FrameColor, FLinearColor::Green, ComponenetInfo.AngleVal);
	}
	else
	{
		DrawBox(InContext, FramePrimitive, FrameColor, FLinearColor::Black, ComponenetInfo.AngleVal);
	}	

	if (ComponenetState == EComponenetState::_Add || ComponenetState == EComponenetState::_Select)
	{
		DrawAuxLine(InContext);
	}


	//FVector2D Start, End;
	//for (int i = 0; i < TempX.Num(); ++i)
	//{
	//	if (FMath::Abs(ComponenetInfo.Loc.X - TempX[i]) <= ObjectSnapTol)
	//	{
	//		Start = FVector2D(ComponenetInfo.Loc.X, ComponenetInfo.Loc.Y - 1000.0f);
	//		End = FVector2D(ComponenetInfo.Loc.X, ComponenetInfo.Loc.Y + 1000.0f);
	//		UWidgetBlueprintLibrary::DrawLine(InContext, Start, End, FLinearColor::White);
	//	}
	//	if (FMath::Abs(ComponenetInfo.Loc.Y - TempY[i]) <= ObjectSnapTol)
	//	{
	//		Start = FVector2D(ComponenetInfo.Loc.X - 1000.0f, ComponenetInfo.Loc.Y);
	//		End = FVector2D(ComponenetInfo.Loc.X + 1000.0f, ComponenetInfo.Loc.Y);
	//		UWidgetBlueprintLibrary::DrawLine(InContext, Start, End, FLinearColor::White);
	//	}
	//}
}

void UPillarComponentPrimitive::PrimitiveVertexDataGenerate(UWorld* MyWorld ,const FVector2D& mousePos)
{
	ComponenetInfo.Loc = mousePos;
	//WorldLocationToScreen(MyWorld, mousePos, ComponenetInfo.Length, ComponenetInfo.Width, FramePrimitive.LeftTopPos, FramePrimitive.RightBottomPos);
	FramePrimitive = WorldLocationToScreen(MyWorld, mousePos, ComponenetInfo.Length, ComponenetInfo.Width);
	//GetOtherPos(FramePrimitive.LeftTopPos, FramePrimitive.RightBottomPos, FramePrimitive.RightTopPos, FramePrimitive.LeftBottomPos);
	//CenterPos = (FramePrimitive.LeftTopPos + FramePrimitive.RightBottomPos) / 2.0f;
	//HitPrimitive = WorldLocationToScreen(MyWorld, mousePos, ComponenetInfo.Length + 3, ComponenetInfo.Width + 3);
	if (MyWorld)
	{
		MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(ComponenetInfo.Loc, 280), CenterPos);
	}
}

void UPillarComponentPrimitive::SetFrameColor(const FLinearColor Color)
{
	if (ComponenetInfo.ComponenetType == EComponenetType::_Pillar)
	{
		FrameColor=Color;
	}
}
