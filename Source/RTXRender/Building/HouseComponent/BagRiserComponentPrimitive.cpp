
#include "BagRiserComponentPrimitive.h"
#include "WidgetBlueprintLibrary.h"

UBagRiserComponentPrimitive::UBagRiserComponentPrimitive()
	:FrameColor(FLinearColor(0.40724f, 0.40724f, 0.40724f,0.5f))
	, proportion(0.7f)
{
	ComponenetInfo.ComponenetType = EComponenetType::_BagRiser;
	ComponenetInfo.Width = 20.0f;
	ComponenetInfo.Length = 25.0f;
}

void UBagRiserComponentPrimitive::Draw(FPaintContext& InContext) const
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
	
	float W = FVector2D::Distance(FramePrimitive.LeftTopPos, FramePrimitive.RightTopPos);
	float L = FVector2D::Distance(FramePrimitive.LeftTopPos, FramePrimitive.LeftBottomPos);
	float r = (L > W ? W : L)*proportion*0.5f;
	DrawCircle(InContext, CenterPos, r, FLinearColor::Black);
	/*if (ComponenetState == EComponenetState::_Select)
	{
		DrawSelectStatePrimitive(InContext,FramePrimitive);
	}*/

	if (ComponenetState == EComponenetState::_Add || ComponenetState == EComponenetState::_Select)
	{
		DrawAuxLine(InContext);
	}
}

void UBagRiserComponentPrimitive::PrimitiveVertexDataGenerate(UWorld* MyWorld ,const FVector2D& mousePos)
{
	ComponenetInfo.Loc = mousePos;
	FramePrimitive=WorldLocationToScreen(MyWorld, mousePos, ComponenetInfo.Length, ComponenetInfo.Width);
	//GetOtherPos(FramePrimitive.LeftTopPos, FramePrimitive.RightBottomPos, FramePrimitive.RightTopPos, FramePrimitive.LeftBottomPos);
	//CenterPos = (FramePrimitive.LeftTopPos + FramePrimitive.RightBottomPos) / 2.0f;
	//HitPrimitive = WorldLocationToScreen(MyWorld, mousePos, ComponenetInfo.Length + 3, ComponenetInfo.Width + 3);
	if (MyWorld)
	{
		MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(ComponenetInfo.Loc, 280), CenterPos);
	}
}
