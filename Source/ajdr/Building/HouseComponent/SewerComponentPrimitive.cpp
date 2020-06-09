
#include "SewerComponentPrimitive.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

USewerComponentPrimitive::USewerComponentPrimitive()
	:FrameColor(FLinearColor(1, 1, 1, 0.5))
{
	ComponenetInfo.ComponenetType = EComponenetType::_Sewer;
	ComponenetInfo.Width = 5.0f;
	ComponenetInfo.Length = 5.0f;
	UTexture2D* Tex = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/UMG/img/Sewer.Sewer'"));
	if (Tex)
	{
		Brush.SetResourceObject(Tex);
	}
}

//void USewerComponentPrimitive::SetMyBreush()const
//{
//	UTexture2D* Tex = LoadObject<UTexture2D>(NULL, TEXT("Texture2D'/Game/UMG/img/Sewer.Sewer'"));
//	if (Tex)
//	{
//		Brush.SetResourceObject(Tex);
//	}
//}
void USewerComponentPrimitive::Draw(FPaintContext& InContext) const
{
	//circle
	//float radius = ComponenetInfo.Width;
	InContext.MaxLayer++;
	float radius = FVector2D::Distance(FramePrimitive.LeftTopPos, FramePrimitive.RightTopPos)*0.5;
	if (ComponenetState == EComponenetState::_Select)
	{
		//DrawCircle(InContext, CenterPos, radius, FLinearColor::Green);
		DrawBox(InContext, FramePrimitive, FrameColor, FLinearColor::Green, 0);
	}
	else
	{
		//DrawCircle(InContext, CenterPos, radius, FLinearColor::Black);
		DrawBox(InContext, FramePrimitive, FrameColor, FLinearColor::Black, 0);
	}

	if (ComponenetState == EComponenetState::_Add || ComponenetState == EComponenetState::_Select)
	{
		DrawAuxLine(InContext);
	}
}

void USewerComponentPrimitive::PrimitiveVertexDataGenerate(UWorld* InMyWorld, const FVector2D& mousePos)
{
	ComponenetInfo.Loc = mousePos;
	FramePrimitive = WorldLocationToScreen(InMyWorld, mousePos, ComponenetInfo.Width, ComponenetInfo.Width);
	//GetOtherPos(FramePrimitive.LeftTopPos, FramePrimitive.RightBottomPos, FramePrimitive.RightTopPos, FramePrimitive.LeftBottomPos);
	//CenterPos = (FramePrimitive.LeftTopPos + FramePrimitive.RightBottomPos) / 2.0f;
	//HitPrimitive = WorldLocationToScreen(MyWorld, mousePos, ComponenetInfo.Width + 3, ComponenetInfo.Width + 3);
	if (InMyWorld)
	{
		InMyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(ComponenetInfo.Loc, 280), CenterPos);
	}
}
