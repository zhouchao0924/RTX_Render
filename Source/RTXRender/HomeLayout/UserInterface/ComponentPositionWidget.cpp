// Copyright? 2017 ihomefnt All Rights Reserved.


#include "ComponentPositionWidget.h"
#include "Building/HouseComponent/ComponentManagerActor.h"
#include "Building/BuildingSystem.h"
#include "EditorGameInstance.h"

void UComponentPositionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	BuildingSystem = GameInst->GetBuildingSystemInstance();
	TopTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Top_TextBox")));
	BottomTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Bottom_TextBox")));
	LeftTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Left_TextBox")));
	RightTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Right_TextBox")));

	MainCanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasPanel_0")));
}

void UComponentPositionWidget::UpdateCompInfo()
{
	if (ObjectID == -1)
		return;

	IObject* CompObj = BuildingSystem->GetObject(ObjectID);
	if (CompObj == nullptr)
		return;

	if (this->Visibility != Visibility)
		SetVisibility(Visibility);

	if (!ToVector2D(CompObj->GetPropertyValue("Location").Vec2Value()).Equals(FVector2D::ZeroVector))
		TargetPosition = ToVector2D(CompObj->GetPropertyValue("Location").Vec2Value());
	FVector2D TempVec;
	if (GetWorld())
	{
		GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(TargetPosition, 280), TempVec);
	}
	SetWidgetToViewport(FVector2D(TempVec.X - 110, TempVec.Y - 85));
	SetWidgetAngle(Angle);

	//if (!bMoved && CurrentScreenPos == TempVec)
	//	return;
	//else
	//	CurrentScreenPos = TempVec;

	bool bInSegment = false;
	float Dist = -1;

	if (CompManager == nullptr)
		return;
	if (BuildingSystem == nullptr)
		return;
	if (CompManager->ComponentBaseWidget == nullptr)
		return;
	if (TopTextBox == nullptr)
		return;
	if (BottomTextBox == nullptr)
		return;
	if (LeftTextBox == nullptr)
		return;
	if (RightTextBox == nullptr)
		return;

	UComponentBasePrimitive* ComponentBase = CompManager->ComponentBaseWidget->GetComponentBase();
	if (ComponentBase == nullptr)
		return;
	if (ComponentBase->GetComponenetType() == EComponenetType::_Sewer || ComponentBase->GetComponenetType() == EComponenetType::_Pillar)
	{
		int32 ObjectID = BuildingSystem->HitTest(TargetPosition);
		if (ObjectID == -1)
		{
			TopTextBox->SetText(FText::FromString(FString::FromInt(0)));
			BottomTextBox->SetText(FText::FromString(FString::FromInt(0)));
			LeftTextBox->SetText(FText::FromString(FString::FromInt(0)));
			RightTextBox->SetText(FText::FromString(FString::FromInt(0)));
			return;
		}
		UBuildingData* Data = BuildingSystem->GetData(ObjectID);
		EObjectType Type = (EObjectType)Data->GetObjectType();
		if (Type == EObjectType::ERoom || Type == EObjectType::EArea || Type == EObjectType::EPolygonArea)
		{
			TArray<FVector2D> TempTPoylgon;
			int num = CompManager->GetPolygonNoCommonPoint(ObjectID, TempTPoylgon);
			if (TempTPoylgon.Num() >= 3)
			{
				TPoylgon = TempTPoylgon;
			}
			else
				return;
		}
		else
		{
			TopTextBox->SetText(FText::FromString(FString::FromInt(0)));
			BottomTextBox->SetText(FText::FromString(FString::FromInt(0)));
			LeftTextBox->SetText(FText::FromString(FString::FromInt(0)));
			RightTextBox->SetText(FText::FromString(FString::FromInt(0)));
			return;
		}
	}

	TArray<FVector2D> TempTopVec;
	TArray<FVector2D> TempBottomVec;
	TArray<FVector2D> TempLeftVec;
	TArray<FVector2D> TempRightVec;

	float MinTopDist = -1, MinBottomDist = -1, MinLeftDist = -1, MinRightDist = -1;
	float TempTopDist = -1, TempBottomDist = -1, TempLeftDist = -1, TempRightDist = -1;
	for (int32 i = 0; i < TPoylgon.Num(); i++)
	{
		FVector2D TempFVector2D, CompPoint1, CompPoint2;
		if (i < TPoylgon.Num() - 1 )
		{
			CompPoint1 = TPoylgon[i];
			CompPoint2 = TPoylgon[i + 1];
			TempFVector2D = TPoylgon[i + 1] - TPoylgon[i];
		}
		else if (i == TPoylgon.Num() - 1)
		{
			CompPoint1 = TPoylgon[i];
			CompPoint2 = TPoylgon[0];
			TempFVector2D = TPoylgon[0] - TPoylgon[i];
		}
		TempFVector2D.Normalize();
		float TempAngle = Angle;

		float TempWidth, TempLength;
		TempWidth = Width;
		TempLength = Length;
		
		FVector2D UpNor(1, 0), BottomNor(-1, 0), LeftNor(0, -1), Right(0, 1);

		FVector2D TempEndPos;
		FVector2D CurrentNor = FVector2D((UpNor.X * Cos(TempAngle/180 * PI) - UpNor.Y * Sin(TempAngle / 180 * PI)), (UpNor.Y * Cos(TempAngle / 180 * PI) + UpNor.X * Sin(TempAngle / 180 * PI)));
		if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, CompPoint1, CompPoint2, TempEndPos))
		{
			TopVec.Empty();
			//Top
			{
				TopVec.Add(CompPoint1);
				TopVec.Add(CompPoint2);
				if (TopTextBox)
				{
					float TempDist = FCString::Atof(*(TopTextBox->GetText().ToString()));
					TempTopDist = FVector2D::Distance((TargetPosition + CurrentNor * TempWidth / 2), TempEndPos);
					if (MinTopDist != -1)
					{
						if (TempTopDist <= MinTopDist)
						{
							TempTopVec.Empty();
							if (!bResetPos)
								MinTopDist = TempTopDist;
							TempTopVec.Add(CompPoint1);
							TempTopVec.Add(CompPoint2);
						}
						else
						{
							TopVec.Empty();
							if (!bResetPos)
								TempTopDist = MinTopDist;
							TopVec.Add(TempTopVec[0]);
							TopVec.Add(TempTopVec[1]);
						}
					}
					else
					{
						if (!bResetPos)
							MinTopDist = TempTopDist;
						TempTopVec.Add(CompPoint1);
						TempTopVec.Add(CompPoint2);
					}
				}
			}
		}

		CurrentNor = FVector2D((BottomNor.X * Cos(TempAngle / 180 * PI) - BottomNor.Y * Sin(TempAngle / 180 * PI)), (BottomNor.Y * Cos(TempAngle / 180 * PI) + BottomNor.X * Sin(TempAngle / 180 * PI)));
		if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, CompPoint1, CompPoint2, TempEndPos))
		{
			BottomVec.Empty();
			//Bottom
			{
				BottomVec.Add(CompPoint1);
				BottomVec.Add(CompPoint2);
				if (BottomTextBox)
				{
					float TempDist = FCString::Atof(*(BottomTextBox->GetText().ToString()));
					TempBottomDist = FVector2D::Distance((TargetPosition + CurrentNor * TempWidth / 2), TempEndPos);
					if (MinBottomDist != -1)
					{
						if (TempBottomDist <= MinBottomDist)
						{
							TempBottomVec.Empty();
							if (!bResetPos)
								MinBottomDist = TempBottomDist;
							TempBottomVec.Add(CompPoint1);
							TempBottomVec.Add(CompPoint2);
						}
						else
						{
							BottomVec.Empty();
							if (!bResetPos)
								TempBottomDist = MinBottomDist;
							BottomVec.Add(TempBottomVec[0]);
							BottomVec.Add(TempBottomVec[1]);
						}
					}
					else
					{
						if (!bResetPos)
							MinBottomDist = TempBottomDist;
						TempBottomVec.Add(CompPoint1);
						TempBottomVec.Add(CompPoint2);
					}
				}
			}
		}

		CurrentNor = FVector2D((LeftNor.X * Cos(TempAngle / 180 * PI) - LeftNor.Y * Sin(TempAngle / 180 * PI)), (LeftNor.Y * Cos(TempAngle / 180 * PI) + LeftNor.X * Sin(TempAngle / 180 * PI)));
		if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, CompPoint1, CompPoint2, TempEndPos))
		{
			LeftVec.Empty();
			//Left
			{
				LeftVec.Add(CompPoint1);
				LeftVec.Add(CompPoint2);
				if (LeftTextBox)
				{
					float TempDist = FCString::Atof(*(LeftTextBox->GetText().ToString()));
					TempLeftDist = FVector2D::Distance((TargetPosition + CurrentNor * TempLength / 2), TempEndPos);
					if (MinLeftDist != -1)
					{
						if (TempLeftDist <= MinLeftDist)
						{
							TempLeftVec.Empty();
							if (!bResetPos)
								MinLeftDist = TempLeftDist;
							TempLeftVec.Add(CompPoint1);
							TempLeftVec.Add(CompPoint2);
						}
						else
						{
							LeftVec.Empty();
							if (!bResetPos)
								TempLeftDist = MinLeftDist;
							LeftVec.Add(TempLeftVec[0]);
							LeftVec.Add(TempLeftVec[1]);
						}
					}
					else
					{
						if (!bResetPos)
							MinLeftDist = TempLeftDist;
						TempLeftVec.Add(CompPoint1);
						TempLeftVec.Add(CompPoint2);
					}
				}
			}
		}

		CurrentNor = FVector2D((Right.X * Cos(TempAngle / 180 * PI) - Right.Y * Sin(TempAngle / 180 * PI)), (Right.Y * Cos(TempAngle / 180 * PI) + Right.X * Sin(TempAngle / 180 * PI)));
		if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, CompPoint1, CompPoint2, TempEndPos))
		{
			RightVec.Empty();
			//Right
			{
				RightVec.Add(CompPoint1);
				RightVec.Add(CompPoint2);
				if (RightTextBox)
				{
					float TempDist = FCString::Atof(*(RightTextBox->GetText().ToString()));
					TempRightDist = FVector2D::Distance((TargetPosition + CurrentNor * TempLength / 2), TempEndPos);
					if (MinRightDist != -1)
					{
						if (TempRightDist <= MinRightDist)
						{
							TempRightVec.Empty();
							if (!bResetPos)
								MinRightDist = TempRightDist;
							TempRightVec.Add(CompPoint1);
							TempRightVec.Add(CompPoint2);
						}
						else
						{
							RightVec.Empty();
							if (!bResetPos)
								TempRightDist = MinRightDist;
							RightVec.Add(TempRightVec[0]);
							RightVec.Add(TempRightVec[1]);
						}
					}
					else
					{
						if (!bResetPos)
							MinRightDist = TempRightDist;
						TempRightVec.Add(CompPoint1);
						TempRightVec.Add(CompPoint2);
					}
				}
			}
		}
	}

	float TempDist = FCString::Atof(*(TopTextBox->GetText().ToString()));
	if ((TempDist != FMath::TruncToInt(TempTopDist *10.f) || LastTopDist != FMath::TruncToInt(TempTopDist*10.f)) && MinTopDist != -1 && !bResetPos && TopVec.Num() == 2)
	{
		LastTopDist = TempDist;
		TopTextBox->SetText(FText::FromString(FString::FromInt(int32(FMath::TruncToInt(MinTopDist * 10.f)))));
	}

	TempDist = FCString::Atof(*(BottomTextBox->GetText().ToString()));
	if ((TempDist != FMath::TruncToInt(TempBottomDist *10.f) || LastBottomDist != FMath::TruncToInt(TempBottomDist*10.f)) && MinBottomDist != -1 && !bResetPos)
	{
		LastBottomDist = TempDist;
		BottomTextBox->SetText(FText::FromString(FString::FromInt(int32(FMath::TruncToInt(MinBottomDist * 10.f)))));
	}

	TempDist = FCString::Atof(*(LeftTextBox->GetText().ToString()));
	if ((TempDist != FMath::TruncToInt(TempLeftDist *10.f) || LastLeftDist != FMath::TruncToInt(TempLeftDist*10.f)) && MinLeftDist != -1 && !bResetPos && LeftVec.Num() == 2)
	{
		LastLeftDist = TempDist;
		LeftTextBox->SetText(FText::FromString(FString::FromInt(int32(FMath::TruncToInt(MinLeftDist * 10.f)))));
	}

	TempDist = FCString::Atof(*(RightTextBox->GetText().ToString()));
	if ((TempDist != FMath::TruncToInt(TempRightDist *10.f) || LastRightDist != FMath::TruncToInt(TempRightDist*10.f)) && MinRightDist != -1 && !bResetPos && RightVec.Num() == 2)
	{
		LastRightDist = TempDist;
		RightTextBox->SetText(FText::FromString(FString::FromInt(int32(FMath::TruncToInt(MinRightDist * 10.f)))));
	}
}

void UComponentPositionWidget::UpDateTopLength(float InputLength)
{
	float TempAngle = Angle;
	FVector2D UpNor(1, 0), BottomNor(-1, 0), LeftNor(0, -1), Right(0, 1);
	float TopDist, BottomDist, LeftDist, RightDist;

	FVector2D TempEndPos;
	if (TopVec.Num() != 2)
		return;
	FVector2D CurrentNor = FVector2D((UpNor.X * Cos(TempAngle / 180 * PI) - UpNor.Y * Sin(TempAngle / 180 * PI)), (UpNor.Y * Cos(TempAngle / 180 * PI) + UpNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, TopVec[0], TopVec[1], TempEndPos))
	{
		TopDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Width / 2), TempEndPos) * 10.f);
	}
	else
		return;

	if (BottomVec.Num() != 2)
		return;
	CurrentNor = FVector2D((BottomNor.X * Cos(TempAngle / 180 * PI) - BottomNor.Y * Sin(TempAngle / 180 * PI)), (BottomNor.Y * Cos(TempAngle / 180 * PI) + BottomNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, BottomVec[0], BottomVec[1], TempEndPos))
	{
		BottomDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Width / 2), TempEndPos) * 10.f);
	}
	else
		return;
	
	if (LeftVec.Num() != 2)
		return;
	CurrentNor = FVector2D((LeftNor.X * Cos(TempAngle / 180 * PI) - LeftNor.Y * Sin(TempAngle / 180 * PI)), (LeftNor.Y * Cos(TempAngle / 180 * PI) + LeftNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, LeftVec[0], LeftVec[1], TempEndPos))
	{
		//Left
			LeftDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Length / 2), TempEndPos) * 10.f);

	}

	if (RightVec.Num() != 2)
		return;
	CurrentNor = FVector2D((Right.X * Cos(TempAngle / 180 * PI) - Right.Y * Sin(TempAngle / 180 * PI)), (Right.Y * Cos(TempAngle / 180 * PI) + Right.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, RightVec[0], RightVec[1], TempEndPos))
	{
		//Right
		{
			RightDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Length / 2), TempEndPos) * 10.f);
		}
	}
	if (InputLength <= 0)
	{
		if (!bResetPos)
			LastInputLength = 0;
		InputLength = 0;
	}
	else if (InputLength > (TopDist + BottomDist))
	{
		if (!bResetPos)
			LastInputLength = InputLength;
		InputLength = TopDist + BottomDist;
	}
	else
	{
		if (!bResetPos)
			LastInputLength = InputLength;
	}

	if (CompManager == nullptr)
		return;
	if (CompManager->ComponentBaseWidget == nullptr)
		return;
	UComponentBasePrimitive* ComponentBase = CompManager->ComponentBaseWidget->GetComponentBase();
	if (ComponentBase == nullptr)
		return;
	if (ComponentBase->GetComponenetType() == EComponenetType::_FLUE || ComponentBase->GetComponenetType() == EComponenetType::_BagRiser)
	{
		if (InputLength != 0 && InputLength != TopDist + BottomDist && LeftDist != 0 && RightDist != 0 )
			return;
	}


	CurrentNor = FVector2D((UpNor.X * Cos(TempAngle / 180 * PI) - UpNor.Y * Sin(TempAngle / 180 * PI)), (UpNor.Y * Cos(TempAngle / 180 * PI) + UpNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, TopVec[0], TopVec[1], TempEndPos))
	{
		CurrentNor.Normalize();
		FVector2D TempVec = TempEndPos -  (InputLength / 10.f + Width / 2.f)* CurrentNor;
		if (CompManager == nullptr)
			return;
		float RotationAngel = 0;
		if (!bResetPos)
		{
			RotationAngel = CheckCompAngel(TempVec);
		}
		if (RotationAngel == 90)
		{
			UpDateLeftLength(LastInputLength);
		}
		else if (RotationAngel == -90 || RotationAngel == 270)
		{
			UpDateRightLength(LastInputLength);
		}
		else if (RotationAngel == -180)
		{
			UpDateBottomLength(LastInputLength);
		}
		else
		{
			CompManager->NewComponent(GetWorld(), TempVec);
			UpdateComponentPos(TempVec);
		}
	}
	else
		return;
}

void UComponentPositionWidget::UpDateBottomLength(float InputLength)
{
	float TempAngle = Angle;
	FVector2D UpNor(1, 0), BottomNor(-1, 0), LeftNor(0, -1), Right(0, 1);
	float TopDist, BottomDist,LeftDist,RightDist;

	FVector2D TempEndPos;
	if (TopVec.Num() != 2)
		return;
	FVector2D CurrentNor = FVector2D((UpNor.X * Cos(TempAngle / 180 * PI) - UpNor.Y * Sin(TempAngle / 180 * PI)), (UpNor.Y * Cos(TempAngle / 180 * PI) + UpNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, TopVec[0], TopVec[1], TempEndPos))
	{
		TopDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Width / 2), TempEndPos) * 10.f);
	}
	else
		return;

	if (BottomVec.Num() != 2)
		return;
	CurrentNor = FVector2D((BottomNor.X * Cos(TempAngle / 180 * PI) - BottomNor.Y * Sin(TempAngle / 180 * PI)), (BottomNor.Y * Cos(TempAngle / 180 * PI) + BottomNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, BottomVec[0], BottomVec[1], TempEndPos))
	{
		BottomDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Width / 2), TempEndPos) * 10.f);
	}
	else
		return;

	if (LeftVec.Num() != 2)
		return;
	CurrentNor = FVector2D((LeftNor.X * Cos(TempAngle / 180 * PI) - LeftNor.Y * Sin(TempAngle / 180 * PI)), (LeftNor.Y * Cos(TempAngle / 180 * PI) + LeftNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, LeftVec[0], LeftVec[1], TempEndPos))
	{
		//Left
		LeftDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Length / 2), TempEndPos) * 10.f);

	}

	if (RightVec.Num() != 2)
		return;
	CurrentNor = FVector2D((Right.X * Cos(TempAngle / 180 * PI) - Right.Y * Sin(TempAngle / 180 * PI)), (Right.Y * Cos(TempAngle / 180 * PI) + Right.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, RightVec[0], RightVec[1], TempEndPos))
	{
		//Right
		{
			RightDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Length / 2), TempEndPos) * 10.f);
		}
	}

	if (InputLength <= 0)
	{
		if (!bResetPos)
			LastInputLength = 0;
		InputLength = 0;
	}
	else if (InputLength > (TopDist + BottomDist))
	{
		if (!bResetPos)
			LastInputLength = InputLength;
		InputLength = TopDist + BottomDist;
	}
	else
	{
		if (!bResetPos)
			LastInputLength = InputLength;
	}

	if (CompManager == nullptr)
		return;
	if (CompManager->ComponentBaseWidget == nullptr)
		return;
	UComponentBasePrimitive* ComponentBase = CompManager->ComponentBaseWidget->GetComponentBase();
	if (ComponentBase == nullptr)
		return;
	if (ComponentBase->GetComponenetType() == EComponenetType::_FLUE || ComponentBase->GetComponenetType() == EComponenetType::_BagRiser)
	{
		if (InputLength != 0 && InputLength != TopDist + BottomDist && LeftDist != 0 && RightDist != 0)
			return;
	}

	CurrentNor = FVector2D((BottomNor.X * Cos(TempAngle / 180 * PI) - BottomNor.Y * Sin(TempAngle / 180 * PI)), (BottomNor.Y * Cos(TempAngle / 180 * PI) + BottomNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, BottomVec[0], BottomVec[1], TempEndPos))
	{
		CurrentNor.Normalize();
		FVector2D TempVec = TempEndPos - (InputLength / 10.f + Width / 2.f)* CurrentNor;
		if (CompManager == nullptr)
			return;
		float RotationAngel = 0;
		if (!bResetPos)
		{
			RotationAngel = CheckCompAngel(TempVec);
		}
		if (RotationAngel == 90)
		{
			UpDateRightLength(LastInputLength);
		}
		else if (RotationAngel == -90 || RotationAngel == 270)
		{
			UpDateLeftLength(LastInputLength);
		}
		else if (RotationAngel == -180)
		{
			UpDateTopLength(LastInputLength);
		}
		else
		{
			CompManager->NewComponent(GetWorld(), TempVec);
			UpdateComponentPos(TempVec);
		}
	}
	else
		return;
}

void UComponentPositionWidget::UpDateLeftLength(float InputLength)
{
	float TempAngle = Angle;
	FVector2D  LeftNor(0, -1), RightNor(0, 1);
	float LeftDist, RightDist;

	FVector2D TempEndPos;
	if (LeftVec.Num() != 2)
		return;
	FVector2D CurrentNor = FVector2D((LeftNor.X * Cos(TempAngle / 180 * PI) - LeftNor.Y * Sin(TempAngle / 180 * PI)), (LeftNor.Y * Cos(TempAngle / 180 * PI) + LeftNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, LeftVec[0], LeftVec[1], TempEndPos))
	{
		LeftDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Length / 2), TempEndPos) * 10.f);
	}
	else
		return;

	if (RightVec.Num() != 2)
		return;
	CurrentNor = FVector2D((RightNor.X * Cos(TempAngle / 180 * PI) - RightNor.Y * Sin(TempAngle / 180 * PI)), (RightNor.Y * Cos(TempAngle / 180 * PI) + RightNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, RightVec[0], RightVec[1], TempEndPos))
	{
		RightDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Length / 2), TempEndPos) * 10.f);
	}
	else
		return;

	if (InputLength <= 0)
	{
		if (!bResetPos)
			LastInputLength = 0;
		InputLength = 0;
	}
	else if (InputLength > (LeftDist + RightDist))
	{
		if (!bResetPos)
			LastInputLength = InputLength;
		InputLength = LeftDist + RightDist;
	}
	else
	{
		if (!bResetPos)
			LastInputLength = InputLength;
	}

	CurrentNor = FVector2D((LeftNor.X * Cos(TempAngle / 180 * PI) - LeftNor.Y * Sin(TempAngle / 180 * PI)), (LeftNor.Y * Cos(TempAngle / 180 * PI) + LeftNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, LeftVec[0], LeftVec[1], TempEndPos))
	{
		CurrentNor.Normalize();
		FVector2D TempVec = TempEndPos - (InputLength / 10.f + Length / 2.f)* CurrentNor;
		if (CompManager == nullptr)
			return;
		float RotationAngel = 0;
		if (!bResetPos)
		{
			RotationAngel = CheckCompAngel(TempVec);
		}
		if (RotationAngel == 90 || RotationAngel == -270)
		{
			UpDateBottomLength(LastInputLength);
		}
		else if (RotationAngel == -90)
		{
			UpDateTopLength(LastInputLength);
		}
		else if (RotationAngel == -180)
		{
			UpDateRightLength(LastInputLength);
		}
		else
		{
			CompManager->NewComponent(GetWorld(), TempVec);
			UpdateComponentPos(TempVec);
		}
	}
	else
		return;
}

void UComponentPositionWidget::UpDateRightLength(float InputLength)
{
	float TempAngle = Angle;
	FVector2D  LeftNor(0, -1), RightNor(0, 1);
	float LeftDist, RightDist;

	FVector2D TempEndPos;
	if (LeftVec.Num() != 2)
		return;
	FVector2D CurrentNor = FVector2D((LeftNor.X * Cos(TempAngle / 180 * PI) - LeftNor.Y * Sin(TempAngle / 180 * PI)), (LeftNor.Y * Cos(TempAngle / 180 * PI) + LeftNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, LeftVec[0], LeftVec[1], TempEndPos))
	{
		LeftDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Length / 2), TempEndPos) * 10.f);
	}
	else
		return;

	if (RightVec.Num() != 2)
		return;
	CurrentNor = FVector2D((RightNor.X * Cos(TempAngle / 180 * PI) - RightNor.Y * Sin(TempAngle / 180 * PI)), (RightNor.Y * Cos(TempAngle / 180 * PI) + RightNor.X * Sin(TempAngle / 180 * PI)));
	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, RightVec[0], RightVec[1], TempEndPos))
	{
		RightDist = FMath::TruncToInt(FVector2D::Distance((TargetPosition + CurrentNor * Length / 2), TempEndPos) * 10.f);
	}
	else
		return;

	if(InputLength <= 0)
	{
		if (!bResetPos)
			LastInputLength = 0;
		InputLength = 0;
	}
	else if (InputLength > (LeftDist + RightDist))
	{
		if (!bResetPos)
			LastInputLength = InputLength;
		InputLength = LeftDist + RightDist;
	}
	else
	{
		if (!bResetPos)
			LastInputLength = InputLength;
	}

	if (FPolygonAlg::IntersectionByRayAndSegment(TargetPosition, CurrentNor, RightVec[0], RightVec[1], TempEndPos))
	{
		CurrentNor.Normalize();
		FVector2D TempVec = TempEndPos - (InputLength / 10.f + Length / 2.f)* CurrentNor;
		if (CompManager == nullptr)
			return;
		float RotationAngel = 0;
		if (!bResetPos)
		{
			RotationAngel = CheckCompAngel(TempVec);
		}
		if (RotationAngel == 90 || RotationAngel == -270)
		{
			UpDateTopLength(LastInputLength);
		}
		else if (RotationAngel == -90)
		{
			UpDateBottomLength(LastInputLength);
		}
		else if (RotationAngel == -180)
		{
			UpDateLeftLength(LastInputLength);
		}
		else
		{
			CompManager->NewComponent(GetWorld(), TempVec);
			UpdateComponentPos(TempVec);
		}
	}
	else
		return;
}

FString UComponentPositionWidget::FilterCharacter(FString InputStr)
{
	//InputStr.GetCharArray();
	TArray<TCHAR> CharArray;
	for (int32 i = 0; i < InputStr.GetCharArray().Num(); i++)
	{
		if (InputStr.GetCharArray()[i] >= '0' && InputStr.GetCharArray()[i] <= '9')
		{
			CharArray.Add(InputStr.GetCharArray()[i]);
		}
	}
	int32 CharstrNum = CharArray.Num();
	if (CharstrNum > 0)
	{
		TCHAR* OutTchar = &CharArray[0];
		FString Outstr(CharstrNum, OutTchar);
		return Outstr;
	}
	else 
		return FString();

	return FString();
}

void UComponentPositionWidget::UpdateComponentPos(FVector2D CurrentPos)
{
	if (CompManager)
	{
		if (CompManager->ComponentBaseWidget)
		{
			UComponentBasePrimitive* ComponentBase = CompManager->ComponentBaseWidget->GetComponentBase();

			switch (ComponentBase->GetComponenetType())
			{
			case EComponenetType::_FLUE:
				BuildingSystem->UpdateFlueProperty(ObjectID, CurrentPos, ComponentBase->GetAngleVal(), Width, Length);
				break;
			case EComponenetType::_BagRiser:
				BuildingSystem->UpdateTubeProperty(ObjectID, CurrentPos, ComponentBase->GetAngleVal(), Width, Length);
				break;
			case EComponenetType::_Pillar:
				BuildingSystem->UpdatePillarProperty(ObjectID, CurrentPos, ComponentBase->GetAngleVal(), Width, Length);
				break;
			case EComponenetType::_Sewer:
				BuildingSystem->UpdateSewerProperty(ObjectID, CurrentPos, ComponentBase->GetWidth());
				break;
			}
			ComponentBase->PrimitiveVertexDataGenerate(GetWorld(), CurrentPos);

			{
				int Areaid = BuildingSystem->HitTest(ComponentBase->GetLoc());
				ComponentBase->ComponenetState = EComponenetState::_Select;
				if (Areaid != INDEX_NONE)
				{
					ComponentBase->SetComponentRoomID(Areaid);
					CompManager->UpdateArea(Areaid);
					//NewUpdateArea(Areaid);
				}
			}
			if (!bResetPos)
				CompManager->UpdateComponent(GetWorld(), ObjectID, ComponentBase->GetLoc(), Width, Length);
		}
	}
}

void  UComponentPositionWidget::NewUpdateArea(int32 id)
{
	if (id != INDEX_NONE)
	{
		if (CompManager == nullptr)
			return;
		if (CompManager->ComponentBaseWidget == nullptr)
			return;

	}
}

float UComponentPositionWidget::CheckCompAngel(FVector2D LateTargetPos)
{
	if (CompManager == nullptr)
		return 0;
	int Areaid = BuildingSystem->HitTest(LateTargetPos);
	if (Areaid == INDEX_NONE)
		return 0;

	CompManager->UpdatePlygon(Areaid);
	if (CompManager->ComponentBaseWidget == nullptr)
		return 0;
	UComponentBasePrimitive *ComponentBase = CompManager->ComponentBaseWidget->GetComponentBase();
	if (ComponentBase)
	{
		for (int i = 0; i < CompManager->roomVecDatas.Num(); ++i)
		{
			float dis = CompManager->roomVecDatas[i] - LateTargetPos;
			dis = dis * 2.0f;
			if ((dis - Width < 4.0f) || (dis - Length < 4.0f))
			{
				ComponentBase->SetAngleVal(CompManager->roomVecDatas[i].AngleVal);
				float DeltaAngle = CompManager->roomVecDatas[i].AngleVal - Angle;
				if (DeltaAngle != 0)
				{
					bResetPos = true;
					Angle = CompManager->roomVecDatas[i].AngleVal;
					//UpdateCompInfo();
					ResetCompPos(DeltaAngle);
				}
				return DeltaAngle;
			}
		}
	}
	else
		return 0;

	if (ComponentBase)
	{
		ComponentBase->SetAngle(0);
		float DeltaAngle = -Angle;
		Angle = ComponentBase->GetAngle();
		UpdateCompInfo();
		return DeltaAngle;
	}
	return 0;
}

void UComponentPositionWidget::ResetCompPos(float RotateAngle)
{
	bResetPos = true;
	UpdateCompInfo();
	if (RotateAngle == 90 || RotateAngle == -270)
	{
		if (LastTopDist != -1 && LastTopDist == 0)
			UpDateLeftLength(0);
		if (LastBottomDist !=-1 && LastBottomDist == 0)
			UpDateRightLength(0);
		if (LastLeftDist != -1 && LastLeftDist == 0)
			UpDateBottomLength(0);
		if (LastRightDist != -1 && LastRightDist == 0)
			UpDateTopLength(0);
	}
	else if (RotateAngle == -90 || RotateAngle == 270)
	{
		if (LastTopDist != -1 && LastTopDist == 0)
			UpDateRightLength(0);
		if (LastBottomDist != -1 && LastBottomDist == 0)
			UpDateLeftLength(0);
		if (LastLeftDist != -1 && LastLeftDist == 0)
			UpDateTopLength(0);
		if (LastRightDist != -1 && LastRightDist == 0)
			UpDateBottomLength(0);
	}
	else if (RotateAngle == -180)
	{
		if (LastTopDist != -1 && LastTopDist == 0)
			UpDateBottomLength(0);
		if (LastBottomDist != -1 && LastBottomDist == 0)
			UpDateTopLength(0);
		if (LastLeftDist != -1 && LastLeftDist == 0)
			UpDateRightLength(0);
		if (LastRightDist != -1 && LastRightDist == 0)
			UpDateLeftLength(0);
	}

	if (!CompManager)
		return;
	if (CompManager->ComponentBaseWidget == nullptr)
		return;
	UComponentBasePrimitive *ComponentBase = CompManager->ComponentBaseWidget->GetComponentBase();
	if (ComponentBase)
		TargetPosition = ComponentBase->GetLoc();
	bResetPos = false;
}