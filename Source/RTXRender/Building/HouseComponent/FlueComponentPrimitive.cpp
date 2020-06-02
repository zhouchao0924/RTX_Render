
#include "FlueComponentPrimitive.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UFlueComponentPrimitive::UFlueComponentPrimitive()
	:proportion(0.7f)
	,outerFrameColor(FLinearColor(0.40724f, 0.40724f, 0.40724f,0.5f))
	, innerFrameColor(FLinearColor(0.879622f, 0.896269f, 0.938686f,0.5f))
{
	ComponenetInfo.ComponenetType = EComponenetType::_FLUE;
	ComponenetInfo.Width = 30.0f;
	ComponenetInfo.Length = 40.0f;
}

void UFlueComponentPrimitive::Draw(FPaintContext& InContext) const
{
	//外框
	InContext.MaxLayer++;
	//DrawBox(InContext, FramePrimitive, outerFrameColor, FLinearColor::Black, ComponenetInfo.AngleVal);
	if (ComponenetState == EComponenetState::_Select)
	{
		DrawBox(InContext, FramePrimitive, outerFrameColor, FLinearColor::Green, ComponenetInfo.AngleVal);
	}
	else
	{
		DrawBox(InContext, FramePrimitive, outerFrameColor, FLinearColor::Black, ComponenetInfo.AngleVal);
	}

	if (ComponenetState == EComponenetState::_Add || ComponenetState == EComponenetState::_Select)
	{
		DrawAuxLine(InContext);
	}

	//内框
	DrawBox(InContext, InnerframePrimitive, innerFrameColor, FLinearColor::Black, ComponenetInfo.AngleVal);

	//指向箭头
	int32 lengthNum = FVector2D::Distance(InnerframePrimitive.LeftTopPos, InnerframePrimitive.RightBottomPos) / 2.0f;
	//FVector2D arrowPos = FVector2D(InnerframePrimitive.LeftTopPos.X + numX / 2, InnerframePrimitive.RightBottomPos.Y + numY / 2);
	int arrowLength = FVector2D::Distance(ArrowPos, CenterPos);
	FVector2D posRightTop, posLeftBottom;
	//GetOtherPos(innerPosLeftTop, innerPosRightBottom, posRightTop, posLeftBottom);
	FVector2D arrowPos = ArrowPos - CenterPos;
	arrowPos.Normalize();
	const FPrimitive2D PosNodes = GetPrimitive2DConst(InnerframePrimitive, ComponenetInfo.AngleVal);
	posRightTop = PosNodes.RightTopPos;
	posLeftBottom =PosNodes.LeftBottomPos;
	FVector2D arrowPosAngle = arrowPos.GetRotated(ComponenetInfo.AngleVal)*arrowLength + CenterPos;
	UWidgetBlueprintLibrary::DrawLine(InContext, arrowPosAngle, posRightTop, FLinearColor::Black);
	UWidgetBlueprintLibrary::DrawLine(InContext, arrowPosAngle, posLeftBottom, FLinearColor::Black);

	/*if (ComponenetState == EComponenetState::_Select)
	{
		DrawSelectStatePrimitive(InContext,FramePrimitive);
	}*/
}

void UFlueComponentPrimitive::PrimitiveVertexDataGenerate(UWorld* MyWorld ,const FVector2D& mousePos)
{
	ComponenetInfo.Loc = mousePos;
	//WorldLocationToScreen(MyWorld, mousePos, ComponenetInfo.Length, ComponenetInfo.Width, FramePrimitive.LeftTopPos, FramePrimitive.RightBottomPos);
	//GetOtherPos(FramePrimitive.LeftTopPos, FramePrimitive.RightBottomPos, FramePrimitive.RightTopPos, FramePrimitive.LeftBottomPos);
	FramePrimitive = WorldLocationToScreen(MyWorld, mousePos, ComponenetInfo.Length, ComponenetInfo.Width);
	CenterPos = (FramePrimitive.LeftTopPos+ FramePrimitive.RightBottomPos) / 2.0f;
	FVector2D size = FramePrimitive.RightBottomPos-FramePrimitive.LeftTopPos;

	float innerLength = ComponenetInfo.Length * proportion;
	float innerWith = ComponenetInfo.Width * proportion;


	float ArrowX = innerWith * proportion;
	float ArrowY = innerLength* proportion;


	InnerframePrimitive = WorldLocationToScreen(MyWorld, mousePos, innerLength, innerWith);
	//HitPrimitive = WorldLocationToScreen(MyWorld, mousePos, ComponenetInfo.Length + 3, ComponenetInfo.Width + 3);
	if (MyWorld)
	{
		FVector locArrow = FVector(mousePos.X + ArrowX / 2.0f, mousePos.Y - ArrowY / 2.0f, 280);
		MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(locArrow, ArrowPos);

		MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(ComponenetInfo.Loc, 280), CenterPos);
	}

}
