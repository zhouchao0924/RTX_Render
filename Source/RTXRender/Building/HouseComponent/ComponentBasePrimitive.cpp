
#include "ComponentBasePrimitive.h"
#include "BluePrint/WidgetBlueprintLibrary.h"
#include "DrawElements.h"



UComponentBasePrimitive::UComponentBasePrimitive()
{
	
}
void UComponentBasePrimitive::DrawBox(FPaintContext& InContext, const FPrimitive2D& inPrimitive, const FLinearColor& FrameColor, const FLinearColor& Tint, float Angle /*= 0.0f*/) const
{
	const FPrimitive2D PrimNodesPos = GetPrimitive2DConst(inPrimitive, Angle);
	size.X = (PrimNodesPos.RightBottomPos - PrimNodesPos.LeftBottomPos).Size();
	size.Y = (PrimNodesPos.RightBottomPos - PrimNodesPos.RightTopPos).Size();

	TOptional<FVector2D> InRotationPoint(FVector2D(0,0));
	FSlateDrawElement::MakeRotatedBox(
		InContext.OutDrawElements,
		InContext.MaxLayer,
		InContext.AllottedGeometry.ToPaintGeometry(PrimNodesPos.LeftTopPos, size),
		&Brush,
		ESlateDrawEffect::None,
		FMath::DegreesToRadians(Angle- ViewAngleVal),
		InRotationPoint,
		FSlateDrawElement::ERotationSpace::RelativeToElement,
		FrameColor
	);

	UWidgetBlueprintLibrary::DrawLine(InContext, PrimNodesPos.LeftTopPos, PrimNodesPos.RightTopPos, Tint);
	UWidgetBlueprintLibrary::DrawLine(InContext, PrimNodesPos.RightTopPos, PrimNodesPos.RightBottomPos, Tint);
	UWidgetBlueprintLibrary::DrawLine(InContext, PrimNodesPos.RightBottomPos, PrimNodesPos.LeftBottomPos, Tint);
	UWidgetBlueprintLibrary::DrawLine(InContext, PrimNodesPos.LeftBottomPos, PrimNodesPos.LeftTopPos, Tint);
}

//void UComponentBasePrimitive::GetOtherPos(const FVector2D& posLeftTop, const FVector2D& posRightBottom, FVector2D& posRightTop, FVector2D&posLeftBottom) const
//{
//	posRightTop = FVector2D(posRightBottom.X, posLeftTop.Y);
//	posLeftBottom = FVector2D(posLeftTop.X, posRightBottom.Y);
//}

FPrimitive2D UComponentBasePrimitive::WorldLocationToScreen(UWorld* MyWorld,const FVector2D& loc, int32 length, int32 width) const
{

	FPrimitive2D outFramePri2d;
	if (MyWorld)
	{

		FVector locLeftTop = FVector(loc.X + width / 2.0f, loc.Y - length / 2.0f, 280);
		FVector locRightBottom = FVector(loc.X - width / 2.0f, loc.Y + length / 2.0f, 280);
		FVector locRightTop = FVector(loc.X + width / 2.0f, loc.Y + length / 2.0f, 280);
		FVector locLeftBottom = FVector(loc.X - width / 2.0f, loc.Y - length / 2.0f, 280);
		//FVector locSize = FVector(width, length,280);
		MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(locLeftTop, outFramePri2d.LeftTopPos);
		MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(locRightBottom, outFramePri2d.RightBottomPos);
		MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(locRightTop, outFramePri2d.RightTopPos);
		MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(locLeftBottom, outFramePri2d.LeftBottomPos);
		//MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(locSize, size);

	}
	return outFramePri2d;
}

void UComponentBasePrimitive::DrawCircle(FPaintContext& InContext, const FVector2D& centerPos, int32 r, const FLinearColor& color, float Anglef) const
{
	TArray<FVector2D> PosNodes;
	FVector2D defalutDir = FVector2D(1, 0);
	for (float i = 0; i < 360; i+= Anglef)
	{
		FVector2D tempPos = centerPos + defalutDir.GetRotated(i) * r;
		PosNodes.Add(tempPos);
	}
	UWidgetBlueprintLibrary::DrawLines(InContext, PosNodes, color);
}

void UComponentBasePrimitive::DrawSelectStatePrimitive(FPaintContext& InContext, const FPrimitive2D& PrimitivePos) const
{
	//float R = FMath::Sqrt(m_iWidth*m_iWidth + m_iLegth * m_iLegth) + 2;
   const FPrimitive2D PrimNodesPos=GetPrimitive2DConst(PrimitivePos, ComponenetInfo.AngleVal);
	TArray<FVector2D> PosNodes;

	FVector2D _1Pos, _2Pos, _3Pos, _4Pos;
	_1Pos = (PrimNodesPos.LeftTopPos + PrimNodesPos.RightTopPos)/2.0f;
	_2Pos = (PrimNodesPos.RightTopPos + PrimNodesPos.RightBottomPos) / 2.0f;
	_3Pos = (PrimNodesPos.RightBottomPos + PrimNodesPos.LeftBottomPos) / 2.0f;
	_4Pos = (PrimNodesPos.LeftBottomPos + PrimNodesPos.LeftTopPos) / 2.0f;
/*
	DrawCircle(InContext, CenterPos, 3, FLinearColor::Green, 1);
	DrawCircle(InContext, PrimNodesPos.LeftTopPos, 3, FLinearColor::White);
	DrawCircle(InContext, PrimNodesPos.RightTopPos, 3, FLinearColor::White);
	DrawCircle(InContext, PrimNodesPos.RightBottomPos, 3, FLinearColor::White);
	DrawCircle(InContext, PrimNodesPos.LeftBottomPos, 3, FLinearColor::White);

	DrawCircle(InContext, _1Pos, 3, FLinearColor::Green);
	DrawCircle(InContext, _2Pos, 3, FLinearColor::Green);
	DrawCircle(InContext, _3Pos, 3, FLinearColor::Green);
	DrawCircle(InContext, _4Pos, 3, FLinearColor::Green);*/
	PosNodes.Add(PrimNodesPos.LeftTopPos);
	PosNodes.Add(PrimNodesPos.RightTopPos);
	PosNodes.Add(PrimNodesPos.RightBottomPos);
	PosNodes.Add(PrimNodesPos.LeftBottomPos);
	PosNodes.Add(PrimNodesPos.LeftTopPos);
	UWidgetBlueprintLibrary::DrawLines(InContext, PosNodes, FLinearColor::Green);

}



void UComponentBasePrimitive::UpdataComponenetInfo(const FComponenetInfo& Info)
{
	ComponenetInfo = Info;
}

FPrimitive2D UComponentBasePrimitive::GetPrimitive2DConst(const FPrimitive2D&  inPrimitive, float Angle /*= 0.0f*/) const
{

	FVector2D CenterPos = FVector2D(inPrimitive.LeftTopPos + inPrimitive.RightBottomPos) / 2.0f;
	float Len = FVector2D::Distance(inPrimitive.LeftTopPos, inPrimitive.RightBottomPos) / 2.0f;
	FVector2D normalLeftTop = inPrimitive.LeftTopPos - CenterPos;
	normalLeftTop.Normalize();

	FVector2D normalRightTop = inPrimitive.RightTopPos - CenterPos;
	normalRightTop.Normalize();

	FVector2D normalRightBottom = inPrimitive.RightBottomPos - CenterPos;
	normalRightBottom.Normalize();

	FVector2D normalLeftBottom = inPrimitive.LeftBottomPos - CenterPos;
	normalLeftBottom.Normalize();

	FVector2D leftTop = normalLeftTop.GetRotated(Angle)*Len + CenterPos;
	FVector2D rightTop = normalRightTop.GetRotated(Angle)*Len + CenterPos;
	FVector2D rightBottom = normalRightBottom.GetRotated(Angle)*Len + CenterPos;
	FVector2D leftBottom = normalLeftBottom.GetRotated(Angle)*Len + CenterPos;

	return FPrimitive2D(leftTop, rightTop, rightBottom, leftBottom);
}

void UComponentBasePrimitive::SetBrush(FSlateBrush B)
{
	Brush = B;
}

void UComponentBasePrimitive::SetBrushLineColor(const FLinearColor& color)
{
	Brush.TintColor = color;
}

void UComponentBasePrimitive::SetComponentDRActorID(int32 ID)
{
	ComponenetInfo.DRActorID = ID;
}

int32 UComponentBasePrimitive::GetComponentDRActorID() const
{
	return ComponenetInfo.DRActorID;
}

void UComponentBasePrimitive::SetComponentRoomID(int32 ID)
{
	ComponenetInfo.RoomID = ID;
}

int32 UComponentBasePrimitive::GetComponentRoomID() const
{
	return ComponenetInfo.RoomID;
}

bool UComponentBasePrimitive::GetHasWall() const
{
	//if(WallIDs->)
	if (ComponenetInfo.WallID.Num() > 0)
	{
		return true;
	}
	else
		return false;
}
void UComponentBasePrimitive::AddWallInfo(int32 id)
{
	if (ComponenetInfo.WallID.Num() < 2 && !ComponenetInfo.WallID.Contains(id))
		ComponenetInfo.WallID.Add(id);
}
void UComponentBasePrimitive::DestroyWall()
{
	if (ComponenetInfo.WallID.Num() == 2)
	{
		ComponenetInfo.WallID.RemoveAt(1);
	}

}

//void UComponentBasePrimitive::RemoveOtherWall(int32 id)
//{
//	if (ComponenetInfo.WallID.Num() == 2 && ComponenetInfo.WallID.Contains(id))
//	{
//		int32 index= ComponenetInfo.WallID.Find(id);
//		if (index == 0)
//		{
//			ComponenetInfo.WallID.RemoveAt(1);
//		}
//		else
//		{
//			ComponenetInfo.WallID.RemoveAt(0);
//		}
//	}
//}

void UComponentBasePrimitive::RenewSetWall(int32 ID0, int32 ID1/*=INDEX_NONE*/)
{
	if (ID0 != INDEX_NONE)
	{
		ComponenetInfo.WallID.Empty();
		ComponenetInfo.WallID.Add(ID0);
		if (ID1 != INDEX_NONE && !ComponenetInfo.WallID.Contains(ID1))
			ComponenetInfo.WallID.Add(ID1);
	}
	
}
void UComponentBasePrimitive::RemoveAll()
{
	ComponenetInfo.WallID.Empty();
}
TArray<int32> UComponentBasePrimitive::GetAllWallIdsInfo() const
{
	return ComponenetInfo.WallID;
}

void UComponentBasePrimitive::SetWallIdsInfo(const TArray<int32>& WallIds)
{
	ComponenetInfo.WallID = WallIds;
}

void UComponentBasePrimitive::SetCenterPos(FVector2D c)
{
	CenterPos = c;
}

FVector2D UComponentBasePrimitive::GetCenterPos() const
{

	return CenterPos;
}
void UComponentBasePrimitive::SetTPoylgon(const TArray<FVector2D>& datas)
{
	ComponenetInfo.TPoylgon = datas;
}
TArray<FVector2D> UComponentBasePrimitive::GetTPoylgon() const
{
	return ComponenetInfo.TPoylgon;
}
void UComponentBasePrimitive::SetWidth(float w)
{
	ComponenetInfo.Width = w;
}

void UComponentBasePrimitive::SetLength(float l)
{
	ComponenetInfo.Length = l;
}

void UComponentBasePrimitive::SetAngleVal(float a)
{
	ComponenetInfo.AngleVal = a;
}

void UComponentBasePrimitive::SetIsWidth(bool b)
{
	ComponenetInfo.IsWidth = b;
}

void UComponentBasePrimitive::SetComponenetType(EComponenetType type)
{
	ComponenetInfo.ComponenetType = type;
}


EComponenetType UComponentBasePrimitive::GetComponenetType() const
{
	return ComponenetInfo.ComponenetType;
}

bool UComponentBasePrimitive::GetIsWidth() const
{
	return ComponenetInfo.IsWidth;
}

float UComponentBasePrimitive::GetAngleVal() const
{
	return ComponenetInfo.AngleVal;
}

float UComponentBasePrimitive::GetWidth() const
{
	return ComponenetInfo.Width;
}

float UComponentBasePrimitive::GetLength() const
{
	return ComponenetInfo.Length;
}

void UComponentBasePrimitive::SetHeight(float H)
{
	ComponenetInfo.Height = H;
}

float UComponentBasePrimitive::GetHeight() const
{
	return ComponenetInfo.Height;
}

FVector2D UComponentBasePrimitive::GetLoc() const
{
	return ComponenetInfo.Loc;
}

FComponenetInfo UComponentBasePrimitive::GetComponenetInfo() const
{
	return ComponenetInfo;
}

void UComponentBasePrimitive::SetMaterialInfo(int32 MaterialChannel, const FString& ResID, int32 modelID, const int32& RoomClassID, const int32& CraftID)
{
	ComponenetInfo.MaterialInfo.Add(MaterialChannel,ResID);
	ComponenetInfo.modelID = modelID;
	ComponenetInfo.RoomClassID = RoomClassID;
	ComponenetInfo.CraftID = CraftID;
}

TMap<int32, FString> UComponentBasePrimitive::GetMaterialInfo() const
{
	return ComponenetInfo.MaterialInfo;
}

void UComponentBasePrimitive::SetModelID(int32 id)
{
	ComponenetInfo.modelID = id;
}

int32 UComponentBasePrimitive::GetModelID() const
{
	return ComponenetInfo.modelID;
}

void UComponentBasePrimitive::SetComponentUUID(FString uuid)
{
	ComponenetInfo.UUID = uuid;
}

FString UComponentBasePrimitive::GetComponentUUID()
{
	return ComponenetInfo.UUID;
}
FVector UComponentBasePrimitive::GetScale() const
{
	return ComponenetInfo.Scale;
}
void UComponentBasePrimitive::SetScale(FVector scale)
{
	ComponenetInfo.Scale = scale;
}
FVector UComponentBasePrimitive::GetOffset() const
{
	return ComponenetInfo.Offset;
}
void UComponentBasePrimitive::SetOffset(FVector offset)
{
	ComponenetInfo.Offset = offset;
}
float UComponentBasePrimitive::GetAngle() const
{
	return ComponenetInfo.Angle;
}
void UComponentBasePrimitive::SetAngle(float angle)
{
	ComponenetInfo.Angle = angle;
}

int32 UComponentBasePrimitive::GetRoomClassID() const
{
	return ComponenetInfo.RoomClassID;
}

FString UComponentBasePrimitive::GetSewerName() const
{
	return ComponenetInfo.SewerName;
}

void UComponentBasePrimitive::SetSewerName(FString Name)
{
	ComponenetInfo.SewerName = Name;
}

int32 UComponentBasePrimitive::GetCraftID() const
{
	return ComponenetInfo.CraftID;
}

void UComponentBasePrimitive::SetMaterialUVInfo(FVector Scale, FVector Offset, float Angle)
{
	ComponenetInfo.Scale = Scale;
	ComponenetInfo.Offset = Offset;
	ComponenetInfo.Angle = Angle;
}

void UComponentBasePrimitive::SetWorld(UWorld* World)
{
	MyWorld = World;
}

void UComponentBasePrimitive::SetDrawActive(const bool& b)
{
	bDrawActive = b;
}

void UComponentBasePrimitive::DrawAuxLine(FPaintContext& InContext)const
{
	if (bDrawActive&&MyWorld)
	{
		InContext.MaxLayer++;
		FVector2D Start, End;
		bool bFindX = true;
		bool bFindY = true;
		for (int i = 0; i < TempX.Num(); ++i)
		{
			if (bFindX)
			{
				if (FMath::Abs(ComponenetInfo.Loc.X - TempX[i]) <= ObjectSnapTol * 2)
				{
					Start = FVector2D(TempX[i], ComponenetInfo.Loc.Y - 100000.0f);
					End = FVector2D(TempX[i], ComponenetInfo.Loc.Y + 100000.0f);
					MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Start, 0), Start);
					MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(End, 0), End);
					UWidgetBlueprintLibrary::DrawLine(InContext, Start, End, FLinearColor::White);
					bFindX = false;
				}
			}
			if (bFindY)
			{
				if (FMath::Abs(ComponenetInfo.Loc.Y - TempY[i]) <= ObjectSnapTol * 2)
				{
					Start = FVector2D(ComponenetInfo.Loc.X - 100000.0f, TempY[i]);
					End = FVector2D(ComponenetInfo.Loc.X + 100000.0f, TempY[i]);
					MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Start, 0), Start);
					MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(End, 0), End);
					UWidgetBlueprintLibrary::DrawLine(InContext, Start, End, FLinearColor::White);
					bFindY = false;
				}
			}
		}
	}
}
