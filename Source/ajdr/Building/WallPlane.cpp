// Copyright? 2017 ihomefnt All Rights Reserved.

#include "WallPlane.h"
#include "DRFunLibrary.h"
#include "EditorGameInstance.h"
#include "Components/SpinBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRCornerAdapter.h"
#include "../HomeLayout/Commands/DrawHouse/DrawWallCmd.h"
// Sets default values
AWallPlaneBase::AWallPlaneBase()
	:DefaultMaterialInstance(LoadObject<UMaterialInstance>(nullptr, TEXT("MaterialInstanceConstant'/Game/GameEditor/EditorPrograms/MapWallSystem/Material/NoBearingWallMaterial_Inst.NoBearingWallMaterial_Inst'")))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	WallRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = WallRootComp;
	PrimaryActorTick.bCanEverTick = true;
	Plane = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Wall2dMesh"));
	Plane->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	LeftLabelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LeftLabelWidget"));
	LeftLabelWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RightLabelWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("RightLabelWidget"));
	RightLabelWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	StructIndex = INDEX_NONE;
}

// Called when the game starts or when spawned
void AWallPlaneBase::BeginPlay()
{
	Super::BeginPlay();
	TrianglesID = { 0,2,1,0,3,2,0,4,3,0,5,4 };
	for (int i = 0; i < 12; ++i)
	{
		NormalsDir.Add(FVector(0, 0, 1));
	}
	WallID = -1;
}

// Called every frame
void AWallPlaneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (InputRulerWidget != nullptr)
	{
		if (InputRulerWidget->bUseDisLengthFlag)

		{

			if (RightLabelWidget->GetUserWidgetObject())
				Cast<URulerLabelWidget>(RightLabelWidget->GetUserWidgetObject())->SetLabelTextData(InputRulerWidget->DisLength / 10);
			if (LeftLabelWidget->GetUserWidgetObject())
				Cast<URulerLabelWidget>(LeftLabelWidget->GetUserWidgetObject())->SetLabelTextData(InputRulerWidget->DisLength / 10);
			InputRulerWidget->bUseDisLengthFlag = false;

		}
	}*/

	if (InputRulerWidget != nullptr)
	{
		ShowInputRulerWidget();
	}
}

void AWallPlaneBase::InitPlane(const TArray<FVector> _TArrayPos, float ZPos, const float& LeftThickness, const float& RightThickness, int32 ObjectID, bool bgeneratecollision, bool bIsTick)
{
	//if (bgeneratecollision)
	//{
	if (!IsShowRuler())
	{
		LeftLabelWidget->SetVisibility(false);
		RightLabelWidget->SetVisibility(false);
	}
	else
	{
		LeftLabelWidget->SetVisibility(true);
		RightLabelWidget->SetVisibility(true);
	}
	//}
	bGeneratecollision = bgeneratecollision;
	VerListPos.Empty();
	for (int i = 0; i < _TArrayPos.Num(); ++i)
	{
		VerListPos.Add(FVector(_TArrayPos[i].X, _TArrayPos[i].Y, ZPos));
	}
	if (VerListPos.Num() > 2)
	{
		Thickness = LeftThickness+ RightThickness;
		ThicknessLeft = LeftThickness;
		ThicknessRight = RightThickness;
		WallID = ObjectID;
		Createplane();
	}

	////////辅助线
	LinePos.Empty();
	LeftLinePos.Empty();
	RightLinePos.Empty();
	FVector nor = (_TArrayPos[0] - _TArrayPos[3]).GetSafeNormal();
	nor.Z = 0;
	FVector Vnor = -FVector::CrossProduct(nor, FVector(0, 0, 1));
	if (bIsTick)
		Vnor = -Vnor;

	FVector Hnor = (_TArrayPos[0] - _TArrayPos[3]).GetSafeNormal();
	Hnor.Z = 0;

	int32 VLen = 22, Linewith = 1;

	//left
	//LinePos.Add(_TArrayPos[1]);
	//LinePos.Add(_TArrayPos[1] + VLen * Vnor);

	//right
	//LinePos.Add(_TArrayPos[5]);
	//LinePos.Add(_TArrayPos[5] + VLen * -Vnor);

	//left
	//LinePos.Add(_TArrayPos[2]);
	//LinePos.Add(_TArrayPos[2] + VLen * Vnor);

	//right
	//LinePos.Add(_TArrayPos[4]);
	//LinePos.Add(_TArrayPos[4] + VLen * -Vnor);

	float leftLen = (_TArrayPos[1] - _TArrayPos[2]).Size() / 2 - 30;
	float rightLen = (_TArrayPos[5] - _TArrayPos[4]).Size() / 2 - 30;

	//与墙垂直的线
	LeftLinePos.Add(_TArrayPos[1]);
	LeftLinePos.Add(_TArrayPos[1] + VLen * Vnor);
	LeftLinePos.Add(_TArrayPos[2]);
	LeftLinePos.Add(_TArrayPos[2] + VLen * Vnor);
	if (leftLen > 0.f)
	{
		

		//与墙水平的线
		LeftLinePos.Add(_TArrayPos[1] + (VLen / 2) * Vnor);
		LeftLinePos.Add((_TArrayPos[1] + (VLen / 2) * Vnor) + leftLen * -Hnor);

		LeftLinePos.Add(_TArrayPos[2] + (VLen / 2) * Vnor);
		LeftLinePos.Add(_TArrayPos[2] + (VLen / 2) * Vnor + leftLen * Hnor);

		LinePos.Add(_TArrayPos[1] + (VLen / 2) * Vnor);
		LinePos.Add((_TArrayPos[1] + (VLen / 2) * Vnor) + leftLen * -Hnor);

		LinePos.Add(_TArrayPos[2] + (VLen / 2) * Vnor);
		LinePos.Add(_TArrayPos[2] + (VLen / 2) * Vnor + leftLen * Hnor);
	}

	//与墙垂直的线
	RightLinePos.Add(_TArrayPos[5]);
	RightLinePos.Add(_TArrayPos[5] + VLen * -Vnor);
	RightLinePos.Add(_TArrayPos[4]);
	RightLinePos.Add(_TArrayPos[4] + VLen * -Vnor);
	if (rightLen > 0.f)
	{
		

		//与墙水平的线 
		RightLinePos.Add(_TArrayPos[5] + (VLen / 2) * -Vnor);
		RightLinePos.Add((_TArrayPos[5] + (VLen / 2) * -Vnor) + rightLen * -Hnor);

		RightLinePos.Add(_TArrayPos[4] + (VLen / 2) * -Vnor);
		RightLinePos.Add((_TArrayPos[4] + (VLen / 2) * -Vnor) + rightLen * Hnor);

		//LinePos.Add(_TArrayPos[5] + (VLen / 2) * -Vnor);
		//LinePos.Add((_TArrayPos[5] + (VLen / 2) * -Vnor) + rightLen * -Hnor);

		//LinePos.Add(_TArrayPos[4] + (VLen / 2) * -Vnor);
		//LinePos.Add((_TArrayPos[4] + (VLen / 2) * -Vnor) + rightLen * Hnor);
	}
}

void AWallPlaneBase::SetRuler(bool IsShow)
{
	if (LeftLabelWidget&&RightLabelWidget)
	{
		LeftLabelWidget->SetVisibility(IsShow);
		RightLabelWidget->SetVisibility(IsShow);
	}
}

void AWallPlaneBase::CalculateSpot(const TArray<FVector> &VerList, TArray<FVector>& OutPosList ,float& PlaneArea, float& Height, FVector& StartPos, FVector& EndPos)
{
	OutPosList.Empty();
	Height = 0;
	PlaneArea = 0;
	if (VerList.Num() == 6)
	{
		StartPos = VerList[0];
		EndPos = VerList[3];
		Height = VerList[0].Z - 1;
		float Zpos = VerList[0].Z - 2;
		FVector TempDir = VerList[3] - VerList[0];
		FVector TempLeftStart= UKismetMathLibrary::FindClosestPointOnLine(VerList[1], VerList[0], TempDir);
		FVector TempLeftEnd = UKismetMathLibrary::FindClosestPointOnLine(VerList[2], VerList[0], TempDir);
		FVector TempRightEnd = UKismetMathLibrary::FindClosestPointOnLine(VerList[4], VerList[0], TempDir);
		FVector TempRightStart = UKismetMathLibrary::FindClosestPointOnLine(VerList[5], VerList[0], TempDir);
		FVector TempPos= (VerList[0]+ VerList[3])/2;
		FVector MaxPos = VerList[3];
		FVector MinPos = VerList[0];
		float TempLength = (MinPos - TempPos).Size();
		if ((TempLeftStart - TempPos).Size() > TempLength&& (TempLeftStart - TempPos).Size()> (TempRightStart - TempPos).Size())
		{
			MinPos = TempLeftStart;
		}
		if ((TempRightStart - TempPos).Size() > TempLength && (TempRightStart - TempPos).Size() > (TempLeftStart - TempPos).Size())
		{
			MinPos = TempRightStart;
		}
	    TempLength = (MaxPos - TempPos).Size();
		if ((TempLeftEnd - TempPos).Size() > TempLength && (TempLeftEnd - TempPos).Size() > (TempRightEnd - TempPos).Size())
		{
			MaxPos = TempLeftEnd;
		}
		if ((TempRightEnd - TempPos).Size() > TempLength && (TempRightEnd - TempPos).Size() > (TempLeftEnd - TempPos).Size())
		{
			MaxPos = TempRightEnd;
		}
		FVector Temp1 = UKismetMathLibrary::FindClosestPointOnLine(MinPos, VerList[1], TempDir);
		FVector Temp2 = UKismetMathLibrary::FindClosestPointOnLine(MaxPos, VerList[1], TempDir);
		FVector Temp3 = UKismetMathLibrary::FindClosestPointOnLine(MaxPos, VerList[5], TempDir);
		FVector Temp4 = UKismetMathLibrary::FindClosestPointOnLine(MinPos, VerList[5], TempDir);
		OutPosList.Add((FVector(FVector2D(MinPos), Zpos)));
		OutPosList.Add(Temp1);
		OutPosList.Add(Temp2);
		OutPosList.Add(FVector(FVector2D(MaxPos), Zpos));
		OutPosList.Add(Temp3);
		OutPosList.Add(Temp4);
		float Length1 = (FVector2D(VerList[2])- FVector2D(VerList[1])).Size();
		float Length2 = (FVector2D(VerList[4]) - FVector2D(VerList[5])).Size();
		if (Length1 < Length2)
		{
			PlaneArea = Length2 * Height;
		}
		else
			PlaneArea = Length1 * Height;
	}
}

void AWallPlaneBase::InitDeleteWall()
{
	SetDeleteWallText();
	SetDeleteWallLocationAndRotation();
}

void AWallPlaneBase::SetDeleteWallText()
{
	if (VerListPos.Num() == 6)
	{
		float length = (VerListPos[2] - VerListPos[1]).Size2D();
		if (RightLabelWidget->GetUserWidgetObject())
		{
			Cast<URulerLabelWidget>(RightLabelWidget->GetUserWidgetObject())->SetLabelTextData(length);
		}
		if (LeftLabelWidget->GetUserWidgetObject())
		{
			Cast<URulerLabelWidget>(LeftLabelWidget->GetUserWidgetObject())->SetLabelTextData(length);
		}
	}
}

void AWallPlaneBase::SetDeleteWallLocationAndRotation()
{
	if (VerListPos.Num() == 6&& RightLabelWidget&&LeftLabelWidget)
	{
		float Angle = UDRFunLibrary::GetWallRotate(FVector2D(VerListPos[1]), FVector2D(VerListPos[2]));
		float TempAngle;
		FRotator RulerRotator(90, Angle, 180);
		if ((90.0f < Angle && Angle < 180) || (-180 <= Angle && Angle < -90))
		{
			TempAngle = Angle - 180;
		}
		else
		{
			TempAngle = Angle;
		}
		FVector RightDir = FVector(FVector2D(VerListPos[2] - VerListPos[1]).GetRotated(90),0).GetSafeNormal();
		FVector Temp = (VerListPos[4] + VerListPos[5]) / 2;
		FVector RightPos = Temp + RightDir * 10;
		Temp = (VerListPos[1] + VerListPos[2]) / 2;
		FVector LeftPos = Temp - RightDir * 10;
		RightLabelWidget->SetWorldLocationAndRotation(RightPos, RulerRotator);
		LeftLabelWidget->SetWorldLocationAndRotation(LeftPos, RulerRotator);
		if (RightLabelWidget->GetUserWidgetObject())
		{
			//Cast<URulerLabelWidget>(RightLabelWidget->GetUserWidgetObject())->SetRenderAngle(TempAngle);
		}
		if (LeftLabelWidget->GetUserWidgetObject())
		{
			//Cast<URulerLabelWidget>(LeftLabelWidget->GetUserWidgetObject())->SetRenderAngle(TempAngle);
		}
	}
}

void AWallPlaneBase::ShowInputRulerWidget()
{
	FVector MouseWorldPos, MouseDir;
	if (!GetWorld() || !GetWorld()->GetFirstPlayerController())
	{
		return;
	}
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseWorldPos, MouseDir);
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	if (!GameInst)
		return;
	FVector2D MousePos;
	if (mLastCornerID != -1)
	{
		if (GameInst->WallBuildSystem)
		{
			MousePos = FVector2D(GameInst->WallBuildSystem->OrthogonalDraw(FVector2D(MouseWorldPos), mLastCornerID, true));
		}
		else
			return;

		UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
		if (projectDataManager == nullptr) {
			return;
		}
		UDRCornerAdapter* CornerAdapter_0 = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(mLastCornerID));
		if (CornerAdapter_0 == nullptr)
		{
			return;
		}
		FVector2D CornerPos = CornerAdapter_0->GetLocaltion();

		if (FVector2D::Distance(CornerPos,MousePos) >= 10)
		{
			if (InputRulerWidget->GetVisibility() == ESlateVisibility::Hidden)
			{
				InputRulerWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			if (InputRulerWidget->GetVisibility() == ESlateVisibility::Visible)
			{
				InputRulerWidget->SetVisibility(ESlateVisibility::Hidden);
				return;
			}
		}

		if (CornerPos.Y == MousePos.Y)
			SpinBoxPos = FVector(((CornerPos.X + MousePos.X) / 2 + 20), ((CornerPos.Y + MousePos.Y) / 2), 0);
		else
			SpinBoxPos = FVector((CornerPos.X + MousePos.X) / 2, (CornerPos.Y + MousePos.Y) / 2, 0);

		if (GameInst->WallBuildSystem->DrawWallCmd)
		{
			if (GameInst->WallBuildSystem->DrawWallCmd->bRightButtonDown)
				return;
		}
		if (LastMousePos.Equals(MousePos))
		{
			if (InputRulerWidget)
			{
				if (InputRulerWidget->GetWidgetFromName("SpinBox_0"))
				{
					if (!InputRulerWidget->GetWidgetFromName("SpinBox_0")->HasKeyboardFocus())
					{
						USpinBox* SpinBox = Cast<USpinBox>(InputRulerWidget->GetWidgetFromName("SpinBox_0"));
						if (SpinBox)
						{
							FVector2D Min;
							FVector2D Max;
							GameInst->WallBuildSystem->GetMaxMinUMGSize(Min, Max);
							FVector2D TempScence;
							GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(MousePos, 0), TempScence);
							if (TempScence.X< Min.X || TempScence.Y< Min.Y || TempScence.X>Max.X || TempScence.Y>Max.Y)
								return;
							SpinBox->SetKeyboardFocus();
						}
					}
				}
			}
		}
		else
		{
			InputRulerWidget->SetLabelTextLengthData(FVector2D::Distance(CornerPos, MousePos));
			LastMousePos = MousePos;
		}
	}
}