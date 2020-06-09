// Copyright? 2017 ihomefnt All Rights Reserved.


#include "HouseFunctionLibrary.h"
#include "IImageWrapperModule.h"
#include "Runtime/Engine/Classes/Kismet/KismetStringLibrary.h"
#include "EditorGameInStance.h"

bool UHouseFunctionLibrary::GetChangeImageData(UObject * WorldContextObject, TArray<FWall>& OutData1, TArray<FWall>& OutData2)
{
	auto World = WorldContextObject->GetWorld();
	UCEditorGameInstance* Instance = Cast<UCEditorGameInstance>(World->GetGameInstance());
	auto Walls = Instance->WallBuildSystem->GetWallsInScene();
	auto Rooms = Instance->WallBuildSystem->GetInnerRoomPathArray();

	if (Walls.Num() < 3 || Rooms.Num() < 1)
		return false;

	TMap<ALineWallActor*, FVector2D> WallLine;
	for (auto wa : Walls)
	{
		FLine line(wa->TempWallNode.StartPos, wa->TempWallNode.EndPos);

		FVector2D p1, p2, cnor;
		line.GetCossPos(20.f, p1, p2, cnor);
		int index = 0;
		for (auto i : Rooms)
		{
			if (ChekColseInterval(p1, i.InnerRoomPath))
			{
				index += 1;
				cnor = -cnor;
			}

			if (ChekColseInterval(p2, i.InnerRoomPath))
				index += 1;

		}

		if (index == 1)
			WallLine.Add(wa, cnor);
	}

	if (WallLine.Num() == 0)
		return false;

	TArray<TMap<ALineWallActor*, FVector2D>> TypeLine;
	TypeLine.SetNum(4);
	for (auto i : WallLine)
	{
		if (i.Value.Equals(FVector2D(0, 1.f), 0.01f))
			TypeLine[0].Add(i.Key, FVector2D(0, 1.f));

		else if (i.Value.Equals(FVector2D(0, -1.f), 0.01f))
			TypeLine[1].Add(i.Key, FVector2D(0, -1.f));

		else if (i.Value.Equals(FVector2D(1.f, 0), 0.01f))
			TypeLine[2].Add(i.Key, FVector2D(1.f, 0));

		else if (i.Value.Equals(FVector2D(-1.f, 0), 0.01f))
			TypeLine[3].Add(i.Key, FVector2D(-1.f, 0));
	}

	for (int i = 0; i < 4; ++i)
	{
		TArray<ALineWallActor*> cleardata;
		for (auto j : TypeLine[i])
		{
			for (auto h : TypeLine[i])
			{
				if (j.Key == h.Key)
					continue;

				FLine line(h.Key->TempWallNode.StartPos, h.Key->TempWallNode.EndPos);
				auto  teline = line.ShrinkLine(10.f);
				if (IsPointAsLine(j.Key->TempWallNode.StartPos, teline) || IsPointAsLine(j.Key->TempWallNode.EndPos, teline))
				{
					cleardata.Add(j.Key);
					break;
				}

			}
		}

		for (auto del : cleardata)
			TypeLine[i].Remove(del);

		float maxw = 0, minw = 0, h = 0;
		int maxda = 1, minda = 1;
		FVector2D sta, en, tnor, tpos;

		for (TMap<ALineWallActor*, FVector2D>::TIterator it(TypeLine[i]); it; ++it)
		{
			auto j = *it;

			//float th = (j.Key->TempWallNode.StartPos * j.Value).Size();
			if (h == 0)
			{
				tpos = j.Key->TempWallNode.StartPos *  FVector2D(FMath::Abs(j.Value.X), FMath::Abs(j.Value.Y));
				h = tpos.X + tpos.Y;
			}
			else
			{
				auto tp = j.Key->TempWallNode.StartPos *  FVector2D(FMath::Abs(j.Value.X), FMath::Abs(j.Value.Y));
				if ((tp - tpos).GetSafeNormal().Equals(j.Value, 0.01f))
				{
					h = tp.X + tp.Y;
					tpos = tp;
				}
			}


			tnor = j.Value;

			FLine linetr;
			float len1 = FVector2D::Distance(j.Key->TempWallNode.StartLeft, j.Key->TempWallNode.EndLeft);
			float len2 = FVector2D::Distance(j.Key->TempWallNode.StartRight, j.Key->TempWallNode.EndRight);
			if (len1 > len2)
				linetr.Set(j.Key->TempWallNode.StartLeft, j.Key->TempWallNode.EndLeft);
			else
				linetr.Set(j.Key->TempWallNode.StartRight, j.Key->TempWallNode.EndRight);

			auto zt = linetr.Start * FVector2D(FMath::Abs(j.Value.Y), FMath::Abs(j.Value.X));
			float tw = zt.X + zt.Y;

			if (tw > maxw || maxw == 0)
			{
				maxw = tw;
				if ((zt.X + zt.Y) < 0)
					maxda = -1;
				else
					maxda = 1;
			}

			if (minw == 0 || tw < minw)
			{
				minw = tw;
				if ((zt.X + zt.Y) < 0)
					minda = -1;
				else
					minda = 1;
			}

			zt = linetr.End * FVector2D(FMath::Abs(j.Value.Y), FMath::Abs(j.Value.X));
			tw = zt.X + zt.Y;

			if (tw > maxw || maxw == 0)
			{
				maxw = tw;
				if ((zt.X + zt.Y) < 0)
					maxda = -1;
				else
					maxda = 1;
			}

			if (minw == 0 || tw < minw)
			{
				minw = tw;
				if ((zt.X + zt.Y) < 0)
					minda = -1;
				else
					minda = 1;
			}

		}

		if (maxw == minw)
			return false;
		//h = h * (tnor.X + tnor.Y);
		if (tnor.X != 0)
		{

			sta = FVector2D(h, minw);
			en.Set(h, maxw);
		}
		else
		{
			sta.Set(minw, h);
			en.Set(maxw, h);
		}

		//UKismetSystemLibrary::DrawDebugLine(World, FVector(sta.X, sta.Y, 500.f), FVector(en.X, en.Y, 500.f),FColor::Red,1000,10);
		//UKismetSystemLibrary::DrawDebugString(World, FVector(sta.X, sta.Y, 500.f), FString::FromInt(minw), nullptr, FColor::Black, 1000.f);
		//UKismetSystemLibrary::DrawDebugString(World, FVector(en.X, en.Y, 500.f), FString::FromInt(maxw), nullptr, FColor::Green, 1000.f);

		sta = sta + tnor * 250;
		en = en + tnor * 250;
		FLine templine(sta, en);
		OutData2.Add(FWall(sta, en));

		TArray<FWall> data;
		for (TMap<ALineWallActor*, FVector2D>::TIterator it(TypeLine[i]); it; ++it)
		{
			auto j = *it;

			FLine linetr;
			float len1 = FVector2D::Distance(j.Key->TempWallNode.StartLeft, j.Key->TempWallNode.EndLeft);
			float len2 = FVector2D::Distance(j.Key->TempWallNode.StartRight, j.Key->TempWallNode.EndRight);

			FVector2D s1, e1;
			if (len1 < len2)
			{
				//FVector stt(j.Key->TempWallNode.StartLeft.X, j.Key->TempWallNode.StartLeft.Y,500);
			//	FVector enn(j.Key->TempWallNode.EndLeft.X, j.Key->TempWallNode.EndLeft.Y, 500);
			//	UKismetSystemLibrary::DrawDebugLine(World, stt, enn, FColor::Red, 1000, 10);
				FVector stt(s1.X, s1.Y, 700);
				FVector enn(e1.X, e1.Y, 700);
				UKismetSystemLibrary::DrawDebugLine(World, stt, enn, FColor::Red, 1000, 10);

				GetSubPoint(j.Key->TempWallNode.StartLeft, templine, s1);
				GetSubPoint(j.Key->TempWallNode.EndLeft, templine, e1);
			}
			else
			{
				//FVector stt(j.Key->TempWallNode.StartRight.X, j.Key->TempWallNode.StartRight.Y, 500);
				//FVector enn(j.Key->TempWallNode.EndRight.X, j.Key->TempWallNode.EndRight.Y, 500);
				//UKismetSystemLibrary::DrawDebugLine(World, stt, enn, FColor::Red, 1000, 10);

				GetSubPoint(j.Key->TempWallNode.StartRight, templine, s1);
				GetSubPoint(j.Key->TempWallNode.EndRight, templine, e1);

				FVector stt(s1.X, s1.Y, 700);
				FVector enn(e1.X, e1.Y, 700);
				UKismetSystemLibrary::DrawDebugLine(World, stt, enn, FColor::Red, 1000, 10);
			}

			s1 = s1 + tnor * 200;
			e1 = e1 + tnor * 200;
			OutData1.Add(FWall(s1, e1));
		}

	}

	return true;
}

void UHouseFunctionLibrary::GetAreaNorEx(TArray<FAreaInnerWall> Data, TArray<FVector2D>& outData)
{

	TArray<FVector2D> PointList;
	int Size = Data.Num();
	for (int i = 0; i < Size; ++i)
	{
		if (PointList.Find(Data[i].SegStart) == INDEX_NONE)
			PointList.Add(Data[i].SegStart);

		if (PointList.Find(Data[i].SegEnd) == INDEX_NONE)
			PointList.Add(Data[i].SegEnd);
	}

	for (int i = 0; i < Size; ++i)
	{
		FLine line(Data[i].SegStart, Data[i].SegEnd);
		FVector2D Pos, nor;
		line.GetCossPos(5.f, Pos, nor);

		if (!ChekColseInterval(Pos, PointList))
			nor = -nor;

		outData.Add(nor);
	}

}



void UHouseFunctionLibrary::GetAreaNor(TArray<FAreaInnerWall> Data, TMap<int32, FVector2D>& outData)
{
	TArray<FVector2D> PointList;
	int Size = Data.Num();
	for (int i = 0; i < Size; ++i)
	{
		if (PointList.Find(Data[i].SegStart) == INDEX_NONE)
			PointList.Add(Data[i].SegStart);

		if (PointList.Find(Data[i].SegEnd) == INDEX_NONE)
			PointList.Add(Data[i].SegEnd);
	}

	for (int i = 0; i < Size; ++i)
	{
		FLine line(Data[i].SegStart, Data[i].SegEnd);
		FVector2D Pos, nor;
		line.GetCossPos(5.f, Pos, nor);

		if (!ChekColseInterval(Pos, PointList))
			nor = -nor;

		outData.Add(Data[i].WallID, nor);
	}
}

bool UHouseFunctionLibrary::ChekColseInterval(const FVector2D & point, const TArray<FVector2D>& List)
{
	FVector2D Dir(0.f, 1.f);
	FLine Tmline(point, point + Dir * 10000.f);
	int AddIntersect = 0;
	for (int i = 0, Len = List.Num(); i < Len; ++i)
	{
		FLine Tl(List[i], List[(i + 1) % Len]);
		if (LineIntersect(Tl, Tmline))
			AddIntersect += 1;
	}

	return (AddIntersect % 2 == 1);
}

bool UHouseFunctionLibrary::LineIntersect(const FLine & line1, const FLine & line2)
{
	if (line1.Start == line2.End || line1.End == line2.Start || line1.Start == line2.Start || line1.End == line2.End)
		return false;
	if (Min(line1.Start.X, line1.End.X) <= Max(line2.Start.X, line2.End.X) &&
		Min(line2.Start.X, line2.End.X) <= Max(line1.Start.X, line1.End.X) &&
		Min(line1.Start.Y, line1.End.Y) <= Max(line2.Start.Y, line2.End.Y) &&
		Min(line2.Start.Y, line2.End.Y) <= Max(line1.Start.Y, line1.End.Y))
	{
		double u, v, w, z;
		u = (line2.Start.X - line1.Start.X)*(line1.End.Y - line1.Start.Y) - (line1.End.X - line1.Start.X)*(line2.Start.Y - line1.Start.Y);
		v = (line2.End.X - line1.Start.X)*(line1.End.Y - line1.Start.Y) - (line1.End.X - line1.Start.X)*(line2.End.Y - line1.Start.Y);
		w = (line1.Start.X - line2.Start.X)*(line2.End.Y - line2.Start.Y) - (line2.End.X - line2.Start.X)*(line1.Start.Y - line2.Start.Y);
		z = (line1.End.X - line2.Start.X)*(line2.End.Y - line2.Start.Y) - (line2.End.X - line2.Start.X)*(line1.End.Y - line2.Start.Y);
		if (u == 0.f && v == 0.f && w == 0.f && z == 0.f)
			return false;

		return (u*v <= 0.00000001 && w*z <= 0.00000001);
	}
	return false;
}

bool UHouseFunctionLibrary::ChekColseIntervalEx(const TArray<FVector2D>& points, const TArray<FVector2D>& List)
{
	for (int i = 0; i < points.Num(); ++i)
	{
		if (ChekColseInterval(points[i], List))
			return true;
	}

	return false;
}

void UHouseFunctionLibrary::GetSubPoint(FVector2D Pos, FLine line2, FVector2D & OutPos)
{
	FLine line1(line2.Start, Pos);
	float Dot = FVector2D::DotProduct(line1.GetNorm(), line2.GetNorm());
	OutPos = (line1.GetLineLength() * Dot) * -line2.GetNorm() + line2.Start;
}

bool UHouseFunctionLibrary::IsPointAsLine(FVector2D & point, FLine & line)
{
	FVector2D outPos;
	if (point.X == line.Start.X || point.Y == line.Start.Y)
		outPos = point;
	else
		GetSubPoint(point, line, outPos);

	float Length = line.GetLineLength();
	float Slen = (outPos - line.Start).Size();
	float Elen = (outPos - line.End).Size();

	return ERRORBAND((Slen + Elen), Length);
}

float UHouseFunctionLibrary::Min(float a, float b)
{
	return a < b ? a : b;
}

float UHouseFunctionLibrary::Max(float a, float b)
{
	return a > b ? a : b;
}


//解压
bool DecompressImage(const TArray< uint8 >& InCompressedData, const int32 InWidth, const int32 InHeight, TArray< uint8 >& OutUncompressedData)
{
	bool bSucceeded = false;
	OutUncompressedData.Reset();
	if (InCompressedData.Num() > 0)
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(&InCompressedData[0], InCompressedData.Num()))
		{
			check(ImageWrapper->GetWidth() == InWidth);
			check(ImageWrapper->GetHeight() == InHeight);
			TArray<uint8> RawData;
			if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, RawData))
			{
				OutUncompressedData = RawData;
				bSucceeded = true;
			}
		}
	}

	return bSucceeded;
}


void UHouseFunctionLibrary::ChangeIamgeCompound(FString path, int32 H, int32 W, int32 OffsetH, int32 OffsetW, UTexture2D* image)
{
	TArray<uint8> RawFileData;
	FFileHelper::LoadFileToArray(RawFileData, path.GetCharArray().GetData());
	FObjectThumbnail Temp;
	Temp.SetImageSize(W, H);
	Temp.AccessCompressedImageData() = RawFileData;
	Temp.DecompressImageData();
	TArray<uint8> Ary;

	DecompressImage(RawFileData, W, H, Ary);

	if (Ary.Num() == 0)
		return;

	UTexture2D* tex = nullptr;
	if (image)
		tex = image;
	else
	{
		FString PathToLoad = "/Game/UMG/CreateWidget/qiang1";
		tex = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(PathToLoad)));
	}
	if (!tex)
		return;

	uint8* MipData = static_cast<uint8*>(tex->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	int32 OpenW = tex->GetImportedSize().X;
	int32 OpenH = tex->GetImportedSize().Y;

	TArray<FColor> TestColor, InColors1;
	for (int i = 0; i < OpenW* OpenH * sizeof(FColor); i += 4)
	{
		FColor co(MipData[i], MipData[i + 1], MipData[i + 2], MipData[i + 3]);
		TestColor.Add(co);
	}

	tex->PlatformData->Mips[0].BulkData.Unlock();
	tex->UpdateResource();


	int32 OffsetSize = sizeof(FColor);
	int32 InWidth_s = W;
	int32 InHeight_S = H;

	for (int i = 0; i < InWidth_s* InHeight_S * sizeof(FColor); i += 4)
	{
		FColor co(Ary[i], Ary[i + 1], Ary[i + 2], Ary[i + 3]);
		InColors1.Add(co);
	}



	int32 Width = InWidth_s - OpenW;
	int32 Height = InHeight_S - OpenH;

	for (int H_Begin = Height; H_Begin < InHeight_S; ++H_Begin)
	{
		for (int W_Begin = Width; W_Begin < InWidth_s; ++W_Begin)
		{

			FColor c1 = InColors1[FMath::Abs(H_Begin - OffsetH) * InWidth_s + FMath::Abs(W_Begin - OffsetW)];
			FColor c2 = TestColor[(H_Begin - Height) * OpenW + (W_Begin - Width)];
			FColor c3;
			float a1 = c2.A / 255.0;
			float a2 = c1.A / 255.0;
			float a = 1 - (1 - a1)*(1 - a2);

			c3.R = (a1*c2.R + (1 - a1)*a2*c1.R) / a;
			c3.G = (a1*c2.G + (1 - a1)*a2*c1.G) / a;
			c3.B = (a1*c2.B + (1 - a1)*a2*c1.B) / a;
			c3.A = a * 255;
			InColors1[FMath::Abs(H_Begin - OffsetH) * InWidth_s + FMath::Abs(W_Begin - OffsetW)] = c3;
		}
	}

	TArray<uint8>CompressedBitmap;
	FImageUtils::CompressImageArray(W, H, InColors1, CompressedBitmap);
	FFileHelper::SaveArrayToFile(CompressedBitmap, *path);

}

int32 UHouseFunctionLibrary::GetTypeID(int32 CategoryID, int32 sleftypeid)
{
	int32 Type = 0;
	switch (CategoryID)
	{
	case 200028: Type = 0; break; // 橱柜

	case 200051:Type = (sleftypeid != 200062 ? 3 : 21); break;// 卫浴柜
	case 200076: Type = 4; break;// 榻榻米
	case 200015: Type = ((sleftypeid == 200025 || sleftypeid == 200026) ? 20 : 5); break; //衣柜
	case 200116: Type = 6; break;// 餐边柜
	case 200115: Type = 7; break;// 电视柜
	case 200063: Type = 8; break;// 书柜	
	case 200008: Type = 9; break;//玄关柜
	case 200101: Type = 10; break;//洗衣柜
	case 200109: Type = 11; break;// 储物柜
	case 200119: Type = 12; break;// 组合床
	case 200073: Type = 13; break;// 书桌	
	case 200106: Type = 14; break;// 吊柜
	case 200082: Type = ((sleftypeid >= 200091 && sleftypeid <= 200098) ? 22 : 15); break;// 衣帽间
	case 200124: Type = 16; break;// 飘窗柜
	case 200128: Type = 17; break;// 收纳柜
	case 200123: Type = 18; break;// 层板
	case 200129: Type = 19; break;// 床头柜
	}
	return Type;
}

TArray<int32>TableBoard = { 105135,105111 ,105055 ,105119 ,105032 ,105089,2910 };
TArray<int32>AngularLine = { 1621 ,2248 };
TArray<int32>CeiLing = { 105118 ,105031 ,105128 ,105110 ,105103 ,105096 ,105082, 105064,105054 ,105046 ,105039 ,105136 };

bool UHouseFunctionLibrary::IsTableBoard(int32 CategoryID)
{
	return TableBoard.Find(CategoryID) != -1;
}

bool UHouseFunctionLibrary::IsAngularLine(int32 CategoryID)
{
	return AngularLine.Find(CategoryID) != -1;
}

FVector UHouseFunctionLibrary::GetRotationPos(FRotator rot, FVector Pos)
{
	auto rotation = rot.Quaternion();
	FTransform const Trabsform(rotation);
	FVector Temp = Trabsform.TransformPosition(Pos);
	return Temp;
}

FVector UHouseFunctionLibrary::ABSVector(FVector Pos)
{

	Pos.X = FMath::Abs(Pos.X);
	Pos.Y = FMath::Abs(Pos.Y);
	Pos.Y = FMath::Abs(Pos.Y);
	return Pos;
}

bool  UHouseFunctionLibrary::IsCeiline(int32 CategoryID)
{
	return CeiLing.Find(CategoryID) != -1;
}


int32 UHouseFunctionLibrary::IsTableBoardAndAngularLine(int32 CategoryID)
{
	if (IsTableBoard(CategoryID))
		return 1;

	if (IsAngularLine(CategoryID))
		return 2;

	if (IsCeiline(CategoryID))
		return 3;

	return 0;
}

bool UHouseFunctionLibrary::GetLinePoint(const float& Length, const FVector2D & objStart, const FVector2D & objEnd, const FVector2D & CurrentStart, const FVector2D & CurrentEnd, FVector2D & OutStart, FVector2D & OutEnd)
{
	FLine line(CurrentStart, CurrentEnd);
	FLine objline(objStart, objEnd);
	FLine TempLine = line;
	TempLine.SetLineLen(Length);
	float Startlen = 0, Endlen = 0;
	if (!IsPointAsLine(TempLine.Start, objline))
	{
		OutStart = objline.Start;
		OutEnd = -objline.GetNorm() * Length + objline.Start;
		Startlen = (objline.Start - TempLine.Start).Size();
		return true;
	}
	else
		OutStart = TempLine.Start;

	if (!IsPointAsLine(TempLine.End, objline))
	{
		OutEnd = objline.End;
		OutStart = objline.GetNorm() * Length + objline.End;
		Endlen = (objline.End - TempLine.End).Size();
		return true;
	}
	else
		OutEnd = TempLine.End;

	if (Length < objline.GetLineLength())
	{
		FLine Outline(OutStart, OutEnd);

		if (Startlen > 0.f)
			OutEnd = Outline.GetPointExLength(false, Startlen);
		else if (Endlen > 0.f)
			OutStart = Outline.GetPointExLength(true, Endlen);
	}

	return true;
}

bool UHouseFunctionLibrary::IsMovePoint(const FVector2D& LastMovePoint, const FVector2D& CurrentMovePoint, const FVector2D& objStart, const FVector2D& objEnd, const FVector2D& CurrentStart, const FVector2D& CurrentEnd, FVector2D& OutStart, FVector2D& OutEnd)
{
	FLine ObjeLine(objStart, objEnd);
	FVector2D SubStart, SubEnd;

	GetSubPoint(CurrentMovePoint, ObjeLine, SubStart);
	GetSubPoint(LastMovePoint, ObjeLine, SubEnd);
	FVector2D  offset = SubStart - SubEnd;

	FLine Moveline;
	Moveline.Start = offset + CurrentStart;
	Moveline.End = offset + CurrentEnd;

	FLine Templine = GetMoveLine(Moveline, ObjeLine, offset.GetSafeNormal());
	OutStart = Templine.Start;
	OutEnd = Templine.End;

	return true;
}

TArray<int32> specialTemplates = { 200036 , 200086, 200090 , 200094 , 200075 };

void UHouseFunctionLibrary::CustomPlayGroup(UObject* WorldContextObject, TArray<AHouseCustomActor*> HousecustomActor, EObjectTypeQuery type, EDrawDebugTrace::Type drawtype)
{
	TArray<AHouseCustomActor*> Templist;
	for (int i = 0; i < HousecustomActor.Num(); ++i)
	{
		if (Templist.Find(HousecustomActor[i]) == -1)
		{
			Templist.Add(HousecustomActor[i]);
			FVector orige, extent;	
			HousecustomActor[i]->GetActorBounds(true, orige, extent);
			
			TArray<TEnumAsByte<EObjectTypeQuery>> Typelist;
			Typelist.Add(type);
			TArray <AActor*> actorlist;
			actorlist.Add(HousecustomActor[i]);
			TArray<FHitResult> OutHits;
			extent = (HousecustomActor[i]->TemplateSize *  HousecustomActor[i]->GetActorScale()) / 20.f + 5.f;
			bool bIsTrace = false;
			//转角处理
			if (specialTemplates.Find(HousecustomActor[i]->TemplateCategoryId) > -1)
			{
				FVector torige, textent,tcenter, tsize;
				HousecustomActor[i]->GetActorBounds(true, torige, textent);
				TArray<UhouseCustomTemplete*> Outconpoment;
				HousecustomActor[i]->FindComponent(TEXT("柜身"), Outconpoment);
				HousecustomActor[i]->compoundElementInterface->GetMeshcCenter(Outconpoment[0]->ElementID, tcenter, "mesh_color1");
				tcenter = LayoutMath::GetInstance()->GetPosToRotator(HousecustomActor[i]->GetActorRotation(), tcenter);
				tcenter = (torige + tcenter);
				tcenter.Z = torige.Z;

				if (Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1"))
					tsize = (*Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1")) / 20.f;
				else
				{
					HousecustomActor[i]->compoundElementInterface->GetMeshSize(Outconpoment[0]->ElementID, tsize, "mesh_color1");
					tsize /= 2.f;
				}
				tsize = tsize + 5.f;
				UKismetSystemLibrary::BoxTraceMultiForObjects(WorldContextObject, tcenter, tcenter, tsize, HousecustomActor[i]->GetActorRotation(), Typelist, false, actorlist, drawtype, OutHits, true, FLinearColor::Red, FLinearColor::Green, 100.f);
				auto temphits = OutHits;

				HousecustomActor[i]->compoundElementInterface->GetMeshcCenter(Outconpoment[0]->ElementID, tcenter, "mesh_color2");
				tcenter = LayoutMath::GetInstance()->GetPosToRotator(HousecustomActor[i]->GetActorRotation(), tcenter);
				tcenter = (torige + tcenter);
				tcenter.Z = torige.Z;

			
				if (Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color2"))
					tsize = (*Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color2")) / 20.f;
				else
				{
					HousecustomActor[i]->compoundElementInterface->GetMeshSize(Outconpoment[0]->ElementID, tsize, "mesh_color2");
					tsize /= 2.f;
				}
				tsize = tsize + 5.f;
				UKismetSystemLibrary::BoxTraceMultiForObjects(WorldContextObject, tcenter, tcenter, tsize, HousecustomActor[i]->GetActorRotation(), Typelist, false, actorlist, drawtype, OutHits, true, FLinearColor::Red, FLinearColor::Green, 100.f);
				OutHits += temphits;

				bIsTrace = OutHits.Num() > 0 ? true : false;
			}
			else
				bIsTrace = UKismetSystemLibrary::BoxTraceMultiForObjects(WorldContextObject, orige, orige, extent, HousecustomActor[i]->GetActorRotation(), Typelist, false, actorlist, drawtype, OutHits, true, FLinearColor::Red, FLinearColor::Green, 100.f);
			
			HousecustomActor[i]->PlaygroupID = FString::FromInt(HousecustomActor[i]->ActorId);
			if (bIsTrace)
			{
				for (int j = 0; j < OutHits.Num(); ++j)
				{
					auto tempactor = Cast<AHouseCustomActor>(OutHits[j].GetActor());
					actorlist.Add(tempactor);
					if (Templist.Find(tempactor) == -1)
						PlayGroupTraceBox(WorldContextObject,tempactor, Templist, HousecustomActor[i]->PlaygroupID, type, drawtype);
					
				}
			}

		}
	}

}

void UHouseFunctionLibrary::PlayGroupTraceBox(UObject* WorldContextObject, AHouseCustomActor * HousecustomActor, TArray<AHouseCustomActor*>& Templist, FString& palyid, EObjectTypeQuery type, EDrawDebugTrace::Type drawtype)
{
	Templist.Add(HousecustomActor);
	FVector orige, extent;
	HousecustomActor->GetActorBounds(true, orige, extent);
	extent = (HousecustomActor->TemplateSize *  HousecustomActor->GetActorScale()) / 20.f + 5.f;
	TArray<TEnumAsByte<EObjectTypeQuery>> Typelist;
	Typelist.Add(type);
	TArray <AActor*> actorlist;
	actorlist.Add(HousecustomActor);
	TArray<FHitResult> OutHits;
	bool bIsTrace = false;
	//转角处理
	if (specialTemplates.Find(HousecustomActor->TemplateCategoryId) > -1)
	{
		FVector torige, textent, center, size;
		HousecustomActor->GetActorBounds(true, torige, textent);
		TArray<UhouseCustomTemplete*> Outconpoment;
		HousecustomActor->FindComponent(TEXT("柜身"), Outconpoment);
		HousecustomActor->compoundElementInterface->GetMeshcCenter(Outconpoment[0]->ElementID, center, "mesh_color1");
		center = LayoutMath::GetInstance()->GetPosToRotator(HousecustomActor->GetActorRotation(), center);
		
		center = (torige + center);
		center.Z = torige.Z;

		if (Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1"))
			size = (*Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color1")) / 20.f;
		else
		{
			HousecustomActor->compoundElementInterface->GetMeshSize(Outconpoment[0]->ElementID, size, "mesh_color1");
			size /= 2.f;
		}
		size = size + 5.f;
		UKismetSystemLibrary::BoxTraceMultiForObjects(WorldContextObject, center, center, size, HousecustomActor->GetActorRotation(), Typelist, false, actorlist, drawtype, OutHits, true, FLinearColor::Red, FLinearColor::Green, 100.f);
		auto temphits = OutHits;

		HousecustomActor->compoundElementInterface->GetMeshcCenter(Outconpoment[0]->ElementID, center, "mesh_color2");
		center = LayoutMath::GetInstance()->GetPosToRotator(HousecustomActor->GetActorRotation(), center);
		center = (torige + center);
		center.Z = torige.Z;

		
		if (Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color2"))
			size = (*Outconpoment[0]->associatedAnchor.MeshSize.Find("mesh_color2")) / 20.f;
		else
		{
			HousecustomActor->compoundElementInterface->GetMeshSize(Outconpoment[0]->ElementID, size, "mesh_color2");
			size /= 2.f;
		}
		size = size +  5.f;
		UKismetSystemLibrary::BoxTraceMultiForObjects(WorldContextObject, center, center, size, HousecustomActor->GetActorRotation() , Typelist, false, actorlist, drawtype, OutHits, true, FLinearColor::Red, FLinearColor::Green, 100.f);
		OutHits += temphits;
		bIsTrace = OutHits.Num() > 0 ? true : false;
	}
	else
		bIsTrace = UKismetSystemLibrary::BoxTraceMultiForObjects(WorldContextObject, orige, orige, extent, HousecustomActor->GetActorRotation(), Typelist, false, actorlist, drawtype, OutHits, true, FLinearColor::Red, FLinearColor::Green, 100.f);
	HousecustomActor->PlaygroupID = palyid;
	if (bIsTrace)
	{
		for (int j = 0; j < OutHits.Num(); ++j)
		{
			auto tempactor = Cast<AHouseCustomActor>(OutHits[j].GetActor());
			actorlist.Add(tempactor);
			if (Templist.Find(tempactor) == -1)
				PlayGroupTraceBox(WorldContextObject, tempactor, Templist, palyid, type, drawtype);
		}
	 }

}

FLine UHouseFunctionLibrary::GetMoveLine(FLine MoveLine, FLine ObjLine, FVector2D nor1)
{
	float MoveLen = MoveLine.GetLineLength();
	float ObjLen = ObjLine.GetLineLength();

	if (MoveLen >= ObjLen)
		return ObjLine;

	float Len;
	FVector2D nor;
	bool IsStart = IsPointAsLine(MoveLine.Start, ObjLine);
	bool IsEnd = IsPointAsLine(MoveLine.End, ObjLine);

	if (!IsStart && !IsEnd)
	{
		FVector2D tnor = ObjLine.GetNorm();

		if (nor1.Equals(tnor, 0.01f))
			GetOffsetData(MoveLine.Start, ObjLine, Len, nor);
		else
			GetOffsetData(MoveLine.End, ObjLine, Len, nor);
	}
	else if (!IsStart)
		GetOffsetData(MoveLine.Start, ObjLine, Len, nor);
	else if (!IsEnd)
		GetOffsetData(MoveLine.End, ObjLine, Len, nor);
	else
		return MoveLine;

	MoveLine.Start += nor * Len;
	MoveLine.End += nor * Len;
	return MoveLine;
}

void UHouseFunctionLibrary::GetOffsetData(FVector2D point, FLine line, float& Len, FVector2D& nor)
{
	float len1 = (point - line.Start).Size();
	float len2 = (point - line.End).Size();

	FVector2D	offset = len1 < len2 ? (line.Start - point) : (line.End - point);
	Len = offset.Size();
	nor = offset.GetSafeNormal();
}

bool UHouseFunctionLibrary::IsRect(FVector2D Pos1, FVector2D Size, FVector2D  pos2)
{
	FVector2D t1(Pos1.X - Size.X / 2.f, Pos1.Y - Size.Y / 2.f);
	FVector2D t2(Pos1.X + Size.X / 2.f, Pos1.Y - Size.Y / 2.f);
	FVector2D t3(Pos1.X + Size.X / 2.f, Pos1.Y + Size.Y / 2.f);
	FVector2D t4(Pos1.X - Size.X / 2.f, Pos1.Y + Size.Y / 2.f);
	TArray<FVector2D> Templist = {t1,t2,t3,t4};
	FVector2D pos(pos2.X, pos2.Y);

	return ChekColseInterval(pos, Templist);
}

FCabinetAreaRaw UHouseFunctionLibrary::RepairtheDeviation(FCabinetAreaRaw  Walldata)
{

	for (int i = 0; i < Walldata.mHoles.Num(); ++i)
	{
		if ((Walldata.mHoles[i].PStartLeft - Walldata.mHoles[i].PEndLeft).Size() < 60.f)
		{
			Walldata.mHoles.RemoveAt(i);
			--i;
		}
	}


	TArray<float>ChangeTemp, NoTemp;
	for (int i = 0; i < Walldata.mWallLines.Num(); ++i)
	{
		float offset = Walldata.mWallLines[i].SegStart.X - Walldata.mWallLines[i].SegEnd.X;
		float offset1 = Walldata.mWallLines[i].SegStart.Y - Walldata.mWallLines[i].SegEnd.Y;
		if (FMath::Abs(offset) && FMath::Abs(offset)  < 5.f)
		{
			if (offset > 0)
				ChangeDeviation(Walldata, Walldata.mWallLines[i].SegStart.X, Walldata.mWallLines[i].SegEnd.X, true);
			else
				ChangeDeviation(Walldata, Walldata.mWallLines[i].SegEnd.X, Walldata.mWallLines[i].SegStart.X, true);
		}
		if (FMath::Abs(offset1) > 0.08f && FMath::Abs(offset1) < 5.f)
		{
			if (offset1 > 0)
				ChangeDeviation(Walldata, Walldata.mWallLines[i].SegStart.Y, Walldata.mWallLines[i].SegEnd.Y, true);
			else
				ChangeDeviation(Walldata, Walldata.mWallLines[i].SegEnd.Y, Walldata.mWallLines[i].SegStart.Y, true);
		}

	
	}

	return Walldata;

}

void UHouseFunctionLibrary::SortLinedata(FCabinetAutoSet& SelectData)
{
	TArray<FCabinetWallLine> Temp;

	for (int i = 0; i < SelectData.mSelectWalls.Num(); ++i)
	{
		bool bIsInset = false;
		for (int j = 0; j < Temp.Num(); ++j)
		{
			float len = (SelectData.mSelectWalls[i].mStart - SelectData.mSelectWalls[i].mEnd).Size();
			float len1 = (Temp[j].mStart - Temp[j].mEnd).Size();
			if (len > len1)
			{
				Temp.Insert(SelectData.mSelectWalls[i], j);
				bIsInset = true;
				break;
			}
		}

		if (!bIsInset)
			Temp.Add(SelectData.mSelectWalls[i]);
	}

	SelectData.mSelectWalls = Temp;

}

void UHouseFunctionLibrary::ChangeDeviation(FCabinetAreaRaw& Walldata,  float SDiff,  float EDiff, bool bIs)
{
	float offset = 0.5f;
	for (int i = 0; i < Walldata.mWallLines.Num(); ++i)
	{
		if (bIs)
		{
			if (ERRORBANDEX(Walldata.mWallLines[i].SegStart.X,SDiff, offset))
				Walldata.mWallLines[i].SegStart.X = EDiff;

			if (ERRORBANDEX(Walldata.mWallLines[i].SegEnd.X, SDiff, offset))
				Walldata.mWallLines[i].SegEnd.X = EDiff;
		}
		else
		{
			if (ERRORBANDEX(Walldata.mWallLines[i].SegStart.Y, SDiff, offset))
				Walldata.mWallLines[i].SegStart.Y = EDiff;

			if (ERRORBANDEX(Walldata.mWallLines[i].SegEnd.Y, SDiff, offset))
				Walldata.mWallLines[i].SegEnd.Y = EDiff;

		}
	}

}

void UHouseFunctionLibrary::GetCustomWallData(int32 Index, UFeatureWall * Feature, FCustomWallData& outdata)
{
	if (Feature == nullptr)
		return;
	outdata.start.Set(Feature->GetInfo()[Index].start.X, Feature->GetInfo()[Index].start.Y);
	outdata.end.Set(Feature->GetInfo()[Index].end.X, Feature->GetInfo()[Index].end.Y);
	outdata.nor.Set(Feature->GetInfo()[Index].forward.X, Feature->GetInfo()[Index].forward.Y);
	outdata.bIs = Feature->GetInfo()[Index].inverse;
}

void UHouseFunctionLibrary::TestCustomsavedata(FCustomAllSaveData data)
{
	auto temp = data;
}

void UHouseFunctionLibrary::GetSubPointCustom(FWall line, FVector2D pos, FVector2D & OutPos)
{
	FLine Templine(line.start, line.end);
	GetSubPoint(pos, Templine, OutPos);
}

bool UHouseFunctionLibrary::IsPointAsLineCustom(FVector2D point, FWall line)
{
	FLine Templine(line.start, line.end);
	return IsPointAsLine(point, Templine);
}

FTransform UHouseFunctionLibrary::StringToTransform(FString str)
{
	FString Left, Right;
	UKismetStringLibrary::Split(str, "Scale", Right, Left, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	bool is;
	FVector Loc, Scale;
	FRotator Rotaion;
	UKismetStringLibrary::Conv_StringToVector(Left, Scale, is);

	Left = Right;
	UKismetStringLibrary::Split(str, "Rotation:", Right, Left, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	UKismetStringLibrary::Conv_StringToRotator(Left, Rotaion, is);

	Left = Right;
	UKismetStringLibrary::Split(str, "Translation:", Right, Left, ESearchCase::IgnoreCase, ESearchDir::FromStart);
	UKismetStringLibrary::Conv_StringToVector(Left, Loc, is);



	return FTransform(Rotaion, Loc, Scale);
}
