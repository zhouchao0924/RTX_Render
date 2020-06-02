// Copyright© 2017 ihomefnt All Rights Reserved.


#include "DrawWallCmd.h"
#include "HomeLayout/HouseArchitect/WallBuildSystem.h"
#include "EditorGameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EditorGameInstance.h"
#include "../../HouseArchitect/WallBuildSystem.h"
#include "../../SceneEntity/LineWallActor.h"
#include "../../UserInterface/MouseSnapWidget.h"
#include "CGALWrapper/CgDataConvUtility.h"
#include "Building/BuildingSystem.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "DRGameMode.h"
#include "AJBlueprintFunctionLibrary.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRSolidWallAdapter.h"
#include "Data/Adapter/DRCornerAdapter.h"
#include "Misc/Guid.h"
#include "Data/FunctionLibrary/DROperationHouseFunctionLibrary.h"
#include "HomeLayout/UserInterface/DrawLineWidget.h"
#define DRAW_WALL_NEW 1
void UDrawWallCmd::CommandWillStart()
{
	Super::CommandWillStart();
	DRGameMode = Cast<ADRGameMode>(GetWorld()->GetAuthGameMode());
	//BuildingSystem = DRGameMode->GetBuildingSystem();
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	WallSystem = GameInst->WallBuildSystem;
	WallSystem->DrawWallCmd = this;
	BuildingSystem = GameInst->GetBuildingSystemInstance();
	//MouseSnapWidget = Cast<UMouseSnapWidget>(UWidgetBlueprintLibrary::Create(MyWorldContextObject, UMouseSnapWidget::StaticClass(), OwningPlayer));
	//MouseSnapWidget->AddToViewport(-1);

	DrawWallContext.ChangeDrawWallState(EDrawWallState::StartDraw);

	bRightButtonDown = false;
	bMovedInRMBD = false;
	LastPoint = INDEX_NONE;
	WallSystem->UpdateWallHudCPP();
	for (ACornerActor *CornerActor : WallSystem->GetCornersInScene())
	{
		if (CornerActor)
			CornerActor->ShowOutterCircle(true);
	}
}

void UDrawWallCmd::CommandWillEnd()
{
	if (MouseSnapWidget)
	{
		MouseSnapWidget->RemoveFromParent();
		MouseSnapWidget = nullptr;
	}
	for (ACornerActor *CornerActor : WallSystem->GetCornersInScene())
	{
		CornerActor->ShowOutterCircle(false);
		CornerActor->ShowInnerCircel(false);
	}
	if (WallPlaneTemp)
	{
		WallPlaneTemp->Destroy();
		WallPlaneTemp = nullptr;
	}
	Super::CommandWillEnd();
}

void UDrawWallCmd::CommandTick(float DeltaTime)
{
	Super::CommandTick(DeltaTime);
	HideCornerSnapState();
	FVector2D MouseScreenPos;
	OwningPlayer->GetMousePosition(MouseScreenPos.X, MouseScreenPos.Y);
	if (!MouseScreenPos.Equals(LastMouseScreenPos, 1.0f))
	{
		bMovedInRMBD = true;
	}

	ALineWallActor *SnapWall = nullptr;
	ACornerActor *SnapCorner = nullptr;
	_SnapPoint.Reset();
	FVector2D MouseSnapPos = LocateMouseSnapState(SnapWall, SnapCorner);

	if (WallPlaneTemp != nullptr)
	{
		WallPlaneTemp->Plane->ClearAllMeshSections();
		TArray<FVector> WallNodes;
		FVector2D Forworld = (MouseSnapPos - LastLeftMousePos).GetSafeNormal();
		FVector2D Dir = Forworld.GetRotated(90);
		float wallthick = WallSystem ? WallSystem->WallThickness : 10;

		WallNodes.Add(FVector(LastLeftMousePos, 0));
		WallNodes.Add(FVector(LastLeftMousePos + wallthick * Dir, 0));
		WallNodes.Add(FVector(MouseSnapPos + wallthick * Dir, 0));
		WallNodes.Add(FVector(MouseSnapPos , 0));
		WallNodes.Add(FVector(MouseSnapPos - wallthick * Dir, 0));
		WallNodes.Add(FVector(LastLeftMousePos - wallthick * Dir, 0));

		float Angle = WallSystem->GetWallRotate(FVector2D(WallNodes[0]), FVector2D(WallNodes[3]));
		FRotator RulerRotator(90, Angle, 180);
		if ((90.0f < Angle && Angle < 180) || (-180 <= Angle && Angle < -90))
		{
			RulerRotator = FRotator(90, Angle - 180, 180);
		}
		FVector Temp = (WallNodes[2] + WallNodes[1]) / 2;
		FVector RightPos = Temp + FVector(Dir,0) * wallthick;
		Temp = (WallNodes[5] + WallNodes[4]) / 2;
		FVector LeftPos = Temp - FVector(Dir, 0) * wallthick;
		WallPlaneTemp->InitPlane(WallNodes, WallSystem->DefaultWallProp.WallNode.Height+0.5f, wallthick, wallthick,-1,false,true);
		WallPlaneTemp->RightLabelWidget->SetWorldLocationAndRotation(RightPos, RulerRotator);
		WallPlaneTemp->LeftLabelWidget->SetWorldLocationAndRotation(LeftPos, RulerRotator);
		float length = (WallNodes[5] - WallNodes[4]).Size();
		float TempAngle;
		if ((90.0f < Angle && Angle < 180) || (-180 <= Angle && Angle < -90))
		{
			TempAngle = Angle - 180;
		}
		else
		{
			TempAngle = Angle;
		}
		Cast<URulerLabelWidget>(WallPlaneTemp->RightLabelWidget->GetUserWidgetObject())->SetLabelTextData(length);
		//Cast<URulerLabelWidget>(WallPlaneTemp->RightLabelWidget->GetUserWidgetObject())->SetRenderAngle(FMath::Abs(Angle) == 180 ? 0 : Angle);
		length = (WallNodes[2] - WallNodes[1]).Size();
		Cast<URulerLabelWidget>(WallPlaneTemp->LeftLabelWidget->GetUserWidgetObject())->SetLabelTextData(length);
		//Cast<URulerLabelWidget>(WallPlaneTemp->LeftLabelWidget->GetUserWidgetObject())->SetRenderAngle(FMath::Abs(Angle) == 180 ? 0 : Angle);
	}
}

void UDrawWallCmd::CutWallByPnt(TArray<FString> &OutCorners2Update, class ACornerActor *&CutCorner,
	class ALineWallActor *InWall2Break, const FVector2D &CutPnt)
{
	if (!InWall2Break)
	{
		return;
	}
}

void UDrawWallCmd::OnLeftMouseButtonDown()
{
	Super::OnLeftMouseButtonDown();
	int32 NewPointIndex = INDEX_NONE;
	_SnapPoint.Reset();
	if (WallSystem&&BuildingSystem)
	{
		FVector2D MouseSnapPos = LocateMouseSnapState(DrawWallContext.SnapWall, DrawWallContext.SnapCorner, true);

		CheckAndAddCornerWall(MouseSnapPos, NewPointIndex);

		//if (NewPointIndex == INDEX_NONE)
		//{
		//	return;
		//}

		LastPoint = NewPointIndex;

		if (LastPoint != INDEX_NONE && WallPlaneTemp == nullptr)
		{
			UClass *WallPlaneActor = WallSystem->WallPlaneActor.Get();
			if (WallPlaneActor&&WallPlaneActor->IsChildOf(AWallPlaneBase::StaticClass()))
			{
				WallPlaneTemp = (AWallPlaneBase*)MyWorld->SpawnActor(WallPlaneActor, &FTransform::Identity);
			}
		}

		if (WallPlaneTemp != nullptr)
		{
			WallPlaneTemp->SetInputWidget(LastPoint);
		}

		LastLeftMousePos = MouseSnapPos;
	}
}

void UDrawWallCmd::SplitWallByIntersection(TArray<FString> &CornersNeeds2Update, class ALineWallActor *NewWall2Split)
{
}

void UDrawWallCmd::OnLeftMouseButtonUp()
{
	Super::OnLeftMouseButtonUp();
}

void UDrawWallCmd::OnRightMouseButtonDown()
{
	bRightButtonDown = true;
	bMovedInRMBD = false;
	OwningPlayer->GetMousePosition(LastMouseScreenPos.X, LastMouseScreenPos.Y);

	Super::OnRightMouseButtonDown();

}

void UDrawWallCmd::OnRightMouseButtonUp()
{
	Super::OnRightMouseButtonUp();

	bRightButtonDown = false;

	if (!bMovedInRMBD)
	{
		if (LastPoint != INDEX_NONE)
			CancelLastOperation();
		else
		{
			CommandWillEnd();
		}
	}

	if (WallPlaneTemp)
	{
		if (WallPlaneTemp->InputRulerWidget)
		{
			if (WallPlaneTemp->InputRulerWidget->GetWidgetFromName("SpinBox_0") != nullptr)
			{
				WallPlaneTemp->InputRulerWidget->GetWidgetFromName("SpinBox_0")->SetKeyboardFocus();
			}
		}
	}
}

void UDrawWallCmd::OnEscapeDown()
{
	Super::OnEscapeDown();

	if (LastPoint != INDEX_NONE)
		CancelLastOperation();
	else
	{
		CommandWillEnd();
	}
}

void UDrawWallCmd::CancelCommand()
{
	if (LastPoint != INDEX_NONE)
		CancelLastOperation();

	Super::CancelCommand();
}

void UDrawWallCmd::OnShowConnectedPntsChanged()
{
	for (ACornerActor *CornerActor : WallSystem->GetCornersInScene())
	{
		CornerActor->UpdateCornerCircleShow();
	}
}

void UDrawWallCmd::OnWallHeightValueChanged()
{
	if (DrawWallContext.DrawWallState == EDrawWallState::StartDrag)
	{
		if (WallsOnPath.Num() != 0)
		{
			ALineWallActor* DrawingWall = WallsOnPath.Last();
			UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(DrawingWall);
			if (projectDataManager == nullptr) {
				return;
			}

			UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(DrawingWall->ObjectId));
			if (solidWallAdapter == nullptr) {
				return;
			}

			solidWallAdapter->SetHeight(280.0f);
		}
	}
}

void UDrawWallCmd::OnWallThicknessValueChanged()
{
	if (DrawWallContext.DrawWallState == EDrawWallState::StartDrag)
	{
		if (WallsOnPath.Num() != 0)
		{
			ALineWallActor* DrawingWall = WallsOnPath.Last();
			UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(DrawingWall);
			if (projectDataManager == nullptr) {
				return;
			}

			UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(DrawingWall->ObjectId));
			if (solidWallAdapter == nullptr) {
				return;
			}
			solidWallAdapter->SetLeftThick(WallSystem->WallThickness);
			solidWallAdapter->SetRightThick(WallSystem->WallThickness);
		}
	}
}

void UDrawWallCmd::OnWallColorChanged()
{
	if (DrawWallContext.DrawWallState == EDrawWallState::StartDrag)
	{
		if (WallsOnPath.Num() != 0)
		{
		}
	}
}



void UDrawWallCmd::CancelLastOperation(bool bAlsoCancelThisCmd/* = false*/)
{
	if (BuildingSystem->IsFree(LastPoint))
	{
		BuildingSystem->DeleteObject(LastPoint);
		if (WallSystem->GetCornersInScene().Num() > 0)
		{
			WallSystem->GetCornersInScene()[WallSystem->GetCornersInScene().Num() - 1]->ShowInnerCircel(false);
			WallSystem->GetCornersInScene()[WallSystem->GetCornersInScene().Num() - 1]->ShowOutterCircle(false);
			WallSystem->GetCornersInScene().RemoveAt(WallSystem->GetCornersInScene().Num() - 1);
		}
	}
	if (WallPlaneTemp)
	{
		WallPlaneTemp->Destroy();
		WallPlaneTemp = nullptr;
	}
	LastPoint = INDEX_NONE;
}

FVector2D UDrawWallCmd::MousePosSnap2WorldPos()
{
	TArray<ACornerActor*> Corners2Ignore;
	TArray<ALineWallActor*> Wall2Ignore;
	ACornerActor *SnapXCorner = nullptr;
	ACornerActor *SnapYCorner = nullptr;
	ALineWallActor *SnapWall = nullptr;
	FVector2D MousePos = GetMouseWorldPos();
	FVector2D MouseSnapPos = WallSystem->GetDrawWallPosByMousePnt(SnapWall, SnapXCorner, SnapYCorner, MousePos, Corners2Ignore, Wall2Ignore);
	MouseSnapWidget->SetSnapMousePos(SnapXCorner, SnapYCorner, SnapWall, MouseSnapPos);

	return MouseSnapPos;
}

FVector2D UDrawWallCmd::LocateMouseSnapState(class ALineWallActor *&OutSnapWall, ACornerActor *&OutSnapCorner, bool IsShowToast)
{
	FVector2D MousePos = GetMouseWorldPos();
	FVector2D	BestLoc;
	int32		BestID = INDEX_NONE;
	FVector   WorldPos, WorldDir;
	if (WallSystem)
	{
		MousePos = WallSystem->OrthogonalDraw(MousePos, LastPoint, true);
	}
#if DRAW_WALL_NEW
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	TArray<int32> CornerIDs;
	WallSystem->GetAllObject(CornerIDs,EObjectType::ECorner,false);
	TArray<FCornerInfoBySnap> TempCorners;
	TArray<FCornerInfoBySnap> TempCornerX;
	TArray<FCornerInfoBySnap> TempCornerY;
	float SnapTop = WallSystem->ObjectSnapTol;
	float SnapTopSquare = SnapTop * SnapTop;
	for (int i = 0;i < CornerIDs.Num(); ++i)
	{
		UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(CornerIDs[i]));
		if (CornerAdapter)
		{
			float TempLength = (MousePos - CornerAdapter->GetLocaltion()).SizeSquared();
			if (TempLength <= SnapTopSquare)
			{
				FCornerInfoBySnap Info(CornerAdapter->GetLocaltion(), CornerIDs[i], TempLength);
				TempCorners.Add(Info);
			}
			else
			{
				float TempLengthXY= FMath::Abs((MousePos.X- CornerAdapter->GetLocaltion().X));
				if (TempLengthXY <= SnapTop)
				{
					FCornerInfoBySnap Info(CornerAdapter->GetLocaltion(), CornerIDs[i], TempLengthXY);
					TempCornerX.Add(Info);
				}
				TempLengthXY = FMath::Abs((MousePos.Y - CornerAdapter->GetLocaltion().Y));
				if (TempLengthXY <= SnapTop)
				{
					FCornerInfoBySnap Info(CornerAdapter->GetLocaltion(), CornerIDs[i], TempLengthXY);
					TempCornerY.Add(Info);
				}
			}
		}
	}
	if (TempCorners.Num() > 0)
	{
		int32 Index = 0;
		for (int i = 1; i < TempCorners.Num(); ++i)
		{
			if (TempCorners[i].Distance < TempCorners[Index].Distance)
			{
				Index = i;
			}
		}
		_SnapPoint.bActive = true;
		_SnapPoint.Loc = MousePos=TempCorners[Index].Loc;
		_SnapPoint.BestID = TempCorners[Index].ID;
		for (ACornerActor *CornerActor : WallSystem->GetCornersInScene())
		{
			if (CornerActor->CornerProperty.ObjectId == _SnapPoint.BestID)
			{
				CornerActor->ShowInnerCircel(true);
				break;
			}
		}
		if (WallSystem->DrawLineWidget)
		{
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(MousePos,0), WallSystem->DrawLineWidget->CenterPos);
			WallSystem->DrawLineWidget->X = 10000;
			WallSystem->DrawLineWidget->Y = 10000;
		}
		return MousePos;
	}
	else
	{
		if (WallSystem->DrawLineWidget)
		{
			WallSystem->DrawLineWidget->X = 10;
			WallSystem->DrawLineWidget->Y = 10;
		}
		if (TempCornerX.Num() > 0)
		{
			int32 Index = 0;
			for (int i = 1; i < TempCornerX.Num(); ++i)
			{
				if (TempCornerX[i].Distance < TempCornerX[Index].Distance)
				{
					Index = i;
				}
			}
			MousePos.X = TempCornerX[Index].Loc.X;
			if (WallSystem->DrawLineWidget)
			{
				WallSystem->DrawLineWidget->X = 10000;
			}
		}
		if (TempCornerY.Num() > 0)
		{
			int32 Index = 0;
			for (int i = 1; i < TempCornerY.Num(); ++i)
			{
				if (TempCornerY[i].Distance < TempCornerY[Index].Distance)
				{
					Index = i;
				}
			}
			MousePos.Y = TempCornerY[Index].Loc.Y;
			if (WallSystem->DrawLineWidget)
			{
				WallSystem->DrawLineWidget->Y = 10000;
			}
		}
		if (WallSystem->DrawLineWidget)
		{
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(MousePos, 0), WallSystem->DrawLineWidget->CenterPos);
		}
		return MousePos;
	}
#else
	int32 Pt = BuildingSystem->Snap(MousePos, -1, BestLoc, BestID, WallSystem->ObjectSnapTol, WallSystem->ObjectSnapTol * 2);
	if (Pt == kESnapType::kEPt)
	{
		_SnapPoint.bActive = true;
		_SnapPoint.BestID = BestID;
		_SnapPoint.Loc = BestLoc;
		MousePos = BestLoc;
		for (ACornerActor *CornerActor : WallSystem->GetCornersInScene())
		{
			if (CornerActor->CornerProperty.ObjectId == BestID)
			{
				CornerActor->ShowInnerCircel(true);
				break;
			}
		}
	}
	else
	{
		if (Pt == kESnapType::kEDirY)
		{
			MousePos = FVector2D(BestLoc.X, MousePos.Y);
		}
		else if (Pt == kESnapType::kEDirX)
		{
			MousePos = FVector2D(MousePos.X, BestLoc.Y);
		}
		else
			if (Pt == (kESnapType::kEDirX | kESnapType::kEDirY))
			{
				MousePos= FVector2D(BestLoc.X, BestLoc.Y);
			}
	}
#endif
	return MousePos;
}

FVector2D UDrawWallCmd::GetMouseWorldPos()
{
	FVector MouseWorldPos, MouseDir;
	if (!OwningPlayer->DeprojectMousePositionToWorld(MouseWorldPos, MouseDir))
	{
		MouseWorldPos = FVector(0, 0, 0);
	}

	return FVector2D(MouseWorldPos);
}

void UDrawWallCmd::HideCornerSnapState()
{
	if (WallSystem->GetCornersInScene().Num() > 0)
	{
		for (ACornerActor *CornerActor : WallSystem->GetCornersInScene())
		{
			CornerActor->ShowOutterCircle(true);
			CornerActor->ShowInnerCircel(false);
		}
	}
}

void UDrawWallCmd::ShowSnapStateOfCorner(class ACornerActor *InShowedCorner)
{
	if (!InShowedCorner)
	{
		return;
	}

	InShowedCorner->ShowInnerCircel(true);
}

void UDrawWallCmd::UpdataCorner()
{
	IObject**ppOutObject = nullptr;
	int nObjs=BuildingSystem->GetAllObjects(ppOutObject, EObjectType::ECorner,false);
	WallSystem->ClearAllCornerData();
	for (int i = 0; i < nObjs; ++i)
	{
		WallSystem->AddNewCorner(ToVector2D(ppOutObject[i]->GetPropertyValue("Location").Vec2Value()), ppOutObject[i]->GetID());
	}
}

void UDrawWallCmd::FillWallDefaultProperties(int32 WallId)
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager == nullptr) {
		return;
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallId));
	if (solidWallAdapter == nullptr) {
		return;
	}

	solidWallAdapter->SetLeftRuler(false);
	solidWallAdapter->SetRightRuler(false);

	TArray<FDRMaterial> wallMaterials;
	for (int i = 0; i < 6; ++i)
	{
		FDRMaterial TempMaterial;
		TempMaterial.ModelID = UDROperationHouseFunctionLibrary::GetConfigModelIDByType(EDConfigType::EDRWallConfig);
		TempMaterial.CraftID = 301;
		TempMaterial.RoomClassID = 16;
		wallMaterials.Add(TempMaterial);
	}
	solidWallAdapter->SetWallMaterials(wallMaterials);

	solidWallAdapter->SetSolidWallType(ESolidWallType::EDROriginalWall);
	solidWallAdapter->SetRoomWall(true);
	solidWallAdapter->SetMainWall(false);
	solidWallAdapter->SetWallTagName(FGuid::NewGuid().ToString());

	solidWallAdapter->SetFrontUVScale(FVector2D(100.0f, 100.0f));
	solidWallAdapter->SetBackUVScale(FVector2D(100.0f, 100.0f));
	solidWallAdapter->SetSideUVScale(FVector2D(100.0f, 100.0f));
}

void UDrawWallCmd::AddWallInsideCorner(FVector2D MouseSnapPos, int32& NewPointIndex)
{
	float height = WallSystem ? WallSystem->DefaultWallProp.WallNode.Height : 280;
	float leftthick = WallSystem ? WallSystem->WallThickness : 10;
	float rightthick = WallSystem ? WallSystem->WallThickness : 10;

	FVector2D Forworld = (MouseSnapPos - LastLeftMousePos).GetSafeNormal();
	FVector2D Dir = Forworld.GetRotated(90);
	float wallthick = leftthick + rightthick;

	FVector2D BestLoc;
	int32 BestID;
	FVector2D StartCornerPos, EndCornerPos, Crossoverpoint;

	int32 Pt = BuildingSystem->CutAreaSnap(LastLeftMousePos, -1, BestLoc, BestID, wallthick, 2* wallthick);
	if (Pt == 4)
	{
		UBuildingData* WallData = BuildingSystem->GetData(BestID);
		if (WallData)
		{
			//UE_LOG(LogTemp, Log, TEXT("%d"), WallData->GetObjectType());
			int32 P0 = WallData->GetInt("P0");
			int32 P1 = WallData->GetInt("P1");
			IObject *CornerData1 = BuildingSystem->GetObject(P0);
			IObject *CornerData2 = BuildingSystem->GetObject(P1);
			if (CornerData1 && CornerData2)
			{
				StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
				EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
				FVector2D Vector1 = EndCornerPos - StartCornerPos;
				FVector2D Vector2 = MouseSnapPos - LastLeftMousePos;
				Vector1.Normalize();
				Vector2.Normalize();
				//UE_LOG(LogTemp, Log, TEXT("%f"), (Vector1.X * Vector2.Y - Vector1.Y * Vector2.X));
				//if ((Vector1.X * Vector2.Y - Vector1.Y * Vector2.X) == 0 || -0.1 < (Vector1.X * Vector2.Y - Vector1.Y * Vector2.X) && (Vector1.X * Vector2.Y - Vector1.Y * Vector2.X) < 0.1)
				if (FMath::Abs(Vector1.X * Vector2.Y - Vector1.Y * Vector2.X) <= 0.1)
				{
					IValue *v = nullptr;
					v = BuildingSystem->GetProperty(LastPoint, "Walls");
					if (v)
					{
						kArray<int> WallIDs = v->IntArrayValue();
						if (WallIDs.size() < 1)
						{
							BuildingSystem->DeleteObject(LastPoint);
						}
					}
					if ((FMath::Abs((LastLeftMousePos - wallthick * Dir).X - StartCornerPos.X) < 1 || FMath::Abs((LastLeftMousePos - wallthick * Dir).Y - StartCornerPos.Y) < 1)
						&& WallSystem->IsPointOnLine((LastLeftMousePos - wallthick * Dir), StartCornerPos, EndCornerPos)
						&& (FVector2D::Distance(LastLeftMousePos - wallthick * Dir, StartCornerPos) < FVector2D::Distance(LastLeftMousePos - wallthick * Dir, EndCornerPos)))
					{
						LastPoint = WallData->GetInt("P0");
					}
					else if ((FMath::Abs((LastLeftMousePos - wallthick * Dir).X - EndCornerPos.X) < 1 || FMath::Abs((LastLeftMousePos - wallthick * Dir).Y - EndCornerPos.Y) < 1)
						&& WallSystem->IsPointOnLine((LastLeftMousePos - wallthick * Dir), StartCornerPos, EndCornerPos)
						&& (FVector2D::Distance(LastLeftMousePos - wallthick * Dir, EndCornerPos) < FVector2D::Distance(LastLeftMousePos - wallthick * Dir, StartCornerPos)))
					{
						LastPoint = WallData->GetInt("P1");
					}
					else
					{
						LastPoint = BuildingSystem->AddCorner(LastLeftMousePos - wallthick * Dir);
						WallSystem->AddNewCorner(LastLeftMousePos - wallthick * Dir, LastPoint);
					}
				}
				else
				{
					bool		bNearCorner = false;
					bool		bOutOfLine = true;
					FVector2D	LineBestLoc;
					int32		LineBestID = INDEX_NONE;
					int32       NearCornerID = -1;
					//Judge the CrossoverPoint is in WallCenterLine
					Crossoverpoint = BuildingSystem->CalculateCrossoverpoint(LastLeftMousePos - wallthick * Dir, MouseSnapPos - wallthick * Dir, StartCornerPos, EndCornerPos);
					//Crossoverpoint = BuildingSystem->CalculateCrossoverpoint(LastLeftMousePos, MouseSnapPos, StartCornerPos, EndCornerPos);
					bOutOfLine = 10 <= FMath::Abs(FVector2D::Distance(Crossoverpoint, StartCornerPos) + FVector2D::Distance(Crossoverpoint, EndCornerPos) - FVector2D::Distance(StartCornerPos, EndCornerPos));
					//UE_LOG(LogTemp, Log, TEXT("%f"), FMath::Abs(FVector2D::Distance(Crossoverpoint, StartCornerPos) + FVector2D::Distance(Crossoverpoint, EndCornerPos) - FVector2D::Distance(StartCornerPos, EndCornerPos)));
					float Dist = FVector2D::Distance(Crossoverpoint, StartCornerPos);
					//UE_LOG(LogTemp, Log, TEXT("%f"), Dist);
					float Dist2 = FVector2D::Distance(Crossoverpoint, EndCornerPos);
					//UE_LOG(LogTemp, Log, TEXT("%f"), Dist2);
					if (Dist < 50 && Dist < Dist2)
					{
						NearCornerID = 0;
						bNearCorner = true;
					}
					else if (Dist2 < 50 && Dist2 < Dist)
					{
						NearCornerID = 1;
						bNearCorner = true;
					}

					if (bNearCorner)
					{
						if (NearCornerID == 0)
						{
							IValue *v = nullptr;
							v = BuildingSystem->GetProperty(WallData->GetInt("P0"), "Walls");
							if (v)
							{
								kArray<int> WallIDs = v->IntArrayValue();
								if (WallIDs.size() <= 1)
								{
									BuildingSystem->DeleteObject(BestID);
									AWallPlaneBase** _WallPlane = WallSystem->WallPlaneMap.Find(BestID);
									if (_WallPlane)
									{
										AWallPlaneBase* WallPlane = *_WallPlane;
										if (WallPlane)
										{
											WallPlane->Destroy();
											WallPlane = nullptr;
											WallSystem->WallPlaneMap.Remove(BestID);
										}
									}

									if (LastPoint != WallData->GetInt("P0"))
									{
										BuildingSystem->DeleteObject(LastPoint);
									}

									BuildingSystem->DeleteObject(WallData->GetInt("P0"));
									LastPoint = BuildingSystem->AddCorner(Crossoverpoint);
									WallSystem->AddNewCorner(Crossoverpoint, LastPoint);

									BuildingSystem->AddWall(LastPoint, WallData->GetInt("P1"), leftthick, rightthick, height);
								}
								else
								{
									if (LastPoint != WallData->GetInt("P0"))
									{
										BuildingSystem->DeleteObject(LastPoint);
									}

									if (FVector2D::Distance(Crossoverpoint, StartCornerPos) <= 5)
									{
										LastPoint = WallData->GetInt("P0");
									}
									else
									{
										LastPoint = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, LastPoint);
									}
								}
							}
						}
						else if (NearCornerID == 1)
						{
							IValue *v = nullptr;
							v = BuildingSystem->GetProperty(WallData->GetInt("P1"), "Walls");
							if (v)
							{
								kArray<int> WallIDs = v->IntArrayValue();
								if (WallIDs.size() <= 1)
								{
									BuildingSystem->DeleteObject(BestID);
									AWallPlaneBase** _WallPlane = WallSystem->WallPlaneMap.Find(BestID);
									if (_WallPlane)
									{
										AWallPlaneBase* WallPlane = *_WallPlane;
										if (WallPlane)
										{
											WallPlane->Destroy();
											WallPlane = nullptr;
											WallSystem->WallPlaneMap.Remove(BestID);
										}
									}

									if (LastPoint != WallData->GetInt("P1"))
									{
										BuildingSystem->DeleteObject(LastPoint);
									}

									BuildingSystem->DeleteObject(WallData->GetInt("P1"));
									LastPoint = BuildingSystem->AddCorner(Crossoverpoint);
									WallSystem->AddNewCorner(Crossoverpoint, LastPoint);

									BuildingSystem->AddWall(WallData->GetInt("P0"), LastPoint, leftthick, rightthick, height);
								}
								else
								{
									if (LastPoint != WallData->GetInt("P1"))
									{
										BuildingSystem->DeleteObject(LastPoint);
									}

									if (FVector2D::Distance(Crossoverpoint, EndCornerPos) <= 5)
									{
										LastPoint = WallData->GetInt("P1");
									}
									else
									{
										LastPoint = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, LastPoint);
									}
								}
							}
						}
					}
					else
					{
						if (bOutOfLine)
						{
							if (FVector2D::Distance(Crossoverpoint, StartCornerPos) < FVector2D::Distance(MouseSnapPos, EndCornerPos))
							{
								IValue *v = nullptr;
								v = BuildingSystem->GetProperty(WallData->GetInt("P0"), "Walls");
								if (v)
								{
									kArray<int> WallIDs = v->IntArrayValue();
									if (WallIDs.size() <= 1)
									{
										BuildingSystem->DeleteObject(BestID);
										AWallPlaneBase** _WallPlane = WallSystem->WallPlaneMap.Find(BestID);
										if (_WallPlane)
										{
											AWallPlaneBase* WallPlane = *_WallPlane;
											if (WallPlane)
											{
												WallPlane->Destroy();
												WallPlane = nullptr;
												WallSystem->WallPlaneMap.Remove(BestID);
											}
										}

										if (LastPoint != WallData->GetInt("P0"))
										{
											BuildingSystem->DeleteObject(LastPoint);
										}

										BuildingSystem->DeleteObject(WallData->GetInt("P0"));
										LastPoint = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, LastPoint);

										BuildingSystem->AddWall(LastPoint, WallData->GetInt("P1"), leftthick, rightthick, height);
									}
									else
									{
										if (LastPoint != WallData->GetInt("P0"))
										{
											BuildingSystem->DeleteObject(LastPoint);
										}

										LastPoint = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, LastPoint);
										//LastPoint = WallData->GetInt("P0");
									}
								}
							}
							else
							{
								IValue *v = nullptr;
								v = BuildingSystem->GetProperty(WallData->GetInt("P1"), "Walls");
								if (v)
								{
									kArray<int> WallIDs = v->IntArrayValue();
									if (WallIDs.size() <= 1)
									{
										BuildingSystem->DeleteObject(BestID);
										AWallPlaneBase** _WallPlane = WallSystem->WallPlaneMap.Find(BestID);
										if (_WallPlane)
										{
											AWallPlaneBase* WallPlane = *_WallPlane;
											if (WallPlane)
											{
												WallPlane->Destroy();
												WallPlane = nullptr;
												WallSystem->WallPlaneMap.Remove(BestID);
											}
										}

										if (LastPoint != WallData->GetInt("P1"))
										{
											BuildingSystem->DeleteObject(LastPoint);
										}

										BuildingSystem->DeleteObject(WallData->GetInt("P1"));
										LastPoint = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, LastPoint);

										BuildingSystem->AddWall(WallData->GetInt("P0"), LastPoint, leftthick, rightthick, height);
									}
									else
									{
										if (LastPoint != WallData->GetInt("P1"))
										{
											BuildingSystem->DeleteObject(LastPoint);
										}

										LastPoint = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, LastPoint);
										//LastPoint = WallData->GetInt("P1");
									}
								}
							}
						}
						else
						{
							if (LastPoint != WallData->GetInt("P0") && LastPoint != WallData->GetInt("P1"))
							{
								BuildingSystem->DeleteObject(LastPoint);
								LastPoint = BuildingSystem->AddCorner(Crossoverpoint);
								WallSystem->AddNewCorner(Crossoverpoint, LastPoint);
							}
						}
					}
				}
			}
		}
		else
		{
			BuildingSystem->DeleteObject(LastPoint);
			LastPoint = BuildingSystem->AddCorner(LastLeftMousePos - wallthick * Dir);
			WallSystem->AddNewCorner(LastLeftMousePos - wallthick * Dir, LastPoint);
		}

	}
	else
	{
		BuildingSystem->DeleteObject(LastPoint);
		LastPoint = BuildingSystem->AddCorner(LastLeftMousePos - wallthick * Dir);
		WallSystem->AddNewCorner(LastLeftMousePos - wallthick * Dir, LastPoint);
		/*LastPoint = BuildingSystem->AddCorner(LastLeftMousePos);
		WallSystem->AddNewCorner(LastLeftMousePos, LastPoint);*/
	}

	Pt = BuildingSystem->CutAreaSnap(MouseSnapPos, -1, BestLoc, BestID, wallthick, 2 * wallthick);
	if (Pt == 4)
	{
		UBuildingData* WallData = BuildingSystem->GetData(BestID);
		if (WallData)
		{
			//UE_LOG(LogTemp, Log, TEXT("%d"), WallData->GetObjectType());
			int P0 = WallData->GetInt("P0");
			int P1 = WallData->GetInt("P1");
			IObject *CornerData1 = BuildingSystem->GetObject(P0);
			IObject *CornerData2 = BuildingSystem->GetObject(P1);
			if (CornerData1 && CornerData2)
			{
				StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
				EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());

				FVector2D Vector1 = EndCornerPos - StartCornerPos;
				FVector2D Vector2 = MouseSnapPos - LastLeftMousePos;
				Vector1.Normalize();
				Vector2.Normalize();
				//if ((Vector1.X * Vector2.Y - Vector1.Y * Vector2.X) == 0 || -0.1 < (Vector1.X * Vector2.Y - Vector1.Y * Vector2.X) && (Vector1.X * Vector2.Y - Vector1.Y * Vector2.X) < 0.1)
				if (FMath::Abs(Vector1.X * Vector2.Y - Vector1.Y * Vector2.X) <= 0.1)
				{
					IValue *v = nullptr;
					v = BuildingSystem->GetProperty(NewPointIndex, "Walls");
					if (v)
					{
						kArray<int> WallIDs = v->IntArrayValue();
						if (WallIDs.size() < 1)
						{
							BuildingSystem->DeleteObject(NewPointIndex);
						}
					}

					if ((FMath::Abs((MouseSnapPos - wallthick * Dir).X - StartCornerPos.X) < 1 || FMath::Abs((MouseSnapPos - wallthick * Dir).Y - StartCornerPos.Y) < 1)
						&& WallSystem->IsPointOnLine((MouseSnapPos - wallthick * Dir), StartCornerPos, EndCornerPos)
						&& (FVector2D::Distance(MouseSnapPos - wallthick * Dir, StartCornerPos) < FVector2D::Distance(MouseSnapPos - wallthick * Dir, EndCornerPos)))
					{
						NewPointIndex = WallData->GetInt("P0");
					}
					else if ((FMath::Abs((MouseSnapPos - wallthick * Dir).X - EndCornerPos.X) < 1 || FMath::Abs((MouseSnapPos - wallthick * Dir).Y - EndCornerPos.Y) < 1)
						&& WallSystem->IsPointOnLine((MouseSnapPos - wallthick * Dir), StartCornerPos, EndCornerPos)
						&& (FVector2D::Distance(MouseSnapPos - wallthick * Dir, EndCornerPos) < FVector2D::Distance(MouseSnapPos - wallthick * Dir, StartCornerPos)))
					{
						NewPointIndex = WallData->GetInt("P1");
					}
					else
					{
						NewPointIndex = BuildingSystem->AddCorner(MouseSnapPos - wallthick * Dir);
						WallSystem->AddNewCorner(MouseSnapPos - wallthick * Dir, NewPointIndex);
					}

					//NewPointIndex = BuildingSystem->AddCorner(MouseSnapPos - wallthick * Dir);
					//WallSystem->AddNewCorner(MouseSnapPos - wallthick * Dir, NewPointIndex);
					///*NewPointIndex = BuildingSystem->AddCorner(MouseSnapPos);
					//WallSystem->AddNewCorner(MouseSnapPos, NewPointIndex);*/
				}
				else
				{
					bool        bNearCorner = false;
					bool		bOutOfLine = true;
					FVector2D	LineBestLoc;
					int32		LineBestID = INDEX_NONE;
					int32       NearCornerID = -1;
					//Judge the CrossoverPoint is in WallCenterLine
					Crossoverpoint = BuildingSystem->CalculateCrossoverpoint(LastLeftMousePos - wallthick * Dir, MouseSnapPos - wallthick * Dir, StartCornerPos, EndCornerPos);
					//Crossoverpoint = BuildingSystem->CalculateCrossoverpoint(LastLeftMousePos, MouseSnapPos , StartCornerPos, EndCornerPos);
					bOutOfLine = 10 <= FMath::Abs(FVector2D::Distance(Crossoverpoint, StartCornerPos) + FVector2D::Distance(Crossoverpoint, EndCornerPos) - FVector2D::Distance(StartCornerPos, EndCornerPos));
					//UE_LOG(LogTemp, Log, TEXT("%f"), FMath::Abs(FVector2D::Distance(Crossoverpoint, StartCornerPos) + FVector2D::Distance(Crossoverpoint, EndCornerPos) - FVector2D::Distance(StartCornerPos, EndCornerPos)));
					float Dist = FVector2D::Distance(Crossoverpoint, StartCornerPos);
					//UE_LOG(LogTemp, Log, TEXT("%f"), Dist);
					float Dist2 = FVector2D::Distance(Crossoverpoint, EndCornerPos);
					//UE_LOG(LogTemp, Log, TEXT("%f"), Dist2);
					if (Dist < 50 && Dist < Dist2)
					{
						NearCornerID = 0;
						bNearCorner = true;
					}
					else if (Dist2 < 50 && Dist2 < Dist)
					{
						NearCornerID = 1;
						bNearCorner = true;
					}

					if (bNearCorner)
					{
						if (NearCornerID == 0)
						{
							IValue *v = nullptr;
							v = BuildingSystem->GetProperty(WallData->GetInt("P0"), "Walls");
							if (v)
							{
								kArray<int> WallIDs = v->IntArrayValue();
								if (WallIDs.size() <= 1)
								{
									BuildingSystem->DeleteObject(BestID);
									AWallPlaneBase** _WallPlane = WallSystem->WallPlaneMap.Find(BestID);
									if (_WallPlane)
									{
										AWallPlaneBase* WallPlane = *_WallPlane;
										if (WallPlane)
										{
											WallPlane->Destroy();
											WallPlane = nullptr;
											WallSystem->WallPlaneMap.Remove(BestID);
										}
									}

									if (NewPointIndex != WallData->GetInt("P0"))
									{
										BuildingSystem->DeleteObject(NewPointIndex);
									}

									BuildingSystem->DeleteObject(WallData->GetInt("P0"));
									NewPointIndex = BuildingSystem->AddCorner(Crossoverpoint);
									WallSystem->AddNewCorner(Crossoverpoint, NewPointIndex);

									BuildingSystem->AddWall(NewPointIndex, WallData->GetInt("P1"), leftthick, rightthick, height);
								}
								else
								{
									if (NewPointIndex != WallData->GetInt("P0"))
									{
										BuildingSystem->DeleteObject(NewPointIndex);
									}

									if (FVector2D::Distance(Crossoverpoint, StartCornerPos) <= 5)
									{
										NewPointIndex = WallData->GetInt("P0");
									}
									else
									{
										NewPointIndex = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, NewPointIndex);
									}
								}
							}
						}
						else if (NearCornerID == 1)
						{
							IValue *v = nullptr;
							v = BuildingSystem->GetProperty(WallData->GetInt("P1"), "Walls");
							if (v)
							{
								kArray<int> WallIDs = v->IntArrayValue();
								if (WallIDs.size() <= 1)
								{
									BuildingSystem->DeleteObject(BestID);
									AWallPlaneBase** _WallPlane = WallSystem->WallPlaneMap.Find(BestID);
									if (_WallPlane)
									{
										AWallPlaneBase* WallPlane = *_WallPlane;
										if (WallPlane)
										{
											WallPlane->Destroy();
											WallPlane = nullptr;
											WallSystem->WallPlaneMap.Remove(BestID);
										}
									}

									if (NewPointIndex != WallData->GetInt("P1"))
									{
										BuildingSystem->DeleteObject(NewPointIndex);
									}

									BuildingSystem->DeleteObject(WallData->GetInt("P1"));
									NewPointIndex = BuildingSystem->AddCorner(Crossoverpoint);
									WallSystem->AddNewCorner(Crossoverpoint, NewPointIndex);

									BuildingSystem->AddWall(WallData->GetInt("P0"), NewPointIndex, leftthick, rightthick, height);
								}
								else
								{
									if (NewPointIndex != WallData->GetInt("P1"))
									{
										BuildingSystem->DeleteObject(NewPointIndex);
									}

									if (FVector2D::Distance(Crossoverpoint, EndCornerPos) <= 5)
									{
										NewPointIndex = WallData->GetInt("P1");
									}
									else
									{
										NewPointIndex = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, NewPointIndex);
									}
								}
							}
						}
					}
					else
					{
						if (bOutOfLine)
						{
							if (FVector2D::Distance(Crossoverpoint, StartCornerPos) < FVector2D::Distance(Crossoverpoint, EndCornerPos))
							{
								IValue *v = nullptr;
								v = BuildingSystem->GetProperty(WallData->GetInt("P0"), "Walls");
								if (v)
								{
									kArray<int> WallIDs = v->IntArrayValue();
									if (WallIDs.size() <= 1)
									{
										BuildingSystem->DeleteObject(BestID);
										AWallPlaneBase** _WallPlane = WallSystem->WallPlaneMap.Find(BestID);
										if (_WallPlane)
										{
											AWallPlaneBase* WallPlane = *_WallPlane;
											if (WallPlane)
											{
												WallPlane->Destroy();
												WallPlane = nullptr;
												WallSystem->WallPlaneMap.Remove(BestID);
											}
										}

										if (NewPointIndex != WallData->GetInt("P0"))
										{
											BuildingSystem->DeleteObject(NewPointIndex);
										}

										BuildingSystem->DeleteObject(WallData->GetInt("P0"));
										NewPointIndex = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, NewPointIndex);

										BuildingSystem->AddWall(NewPointIndex, WallData->GetInt("P1"), leftthick, rightthick, height);
									}
									else
									{
										if (NewPointIndex != WallData->GetInt("P0"))
										{
											BuildingSystem->DeleteObject(NewPointIndex);
										}

										NewPointIndex = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, NewPointIndex);
									}
								}
							}
							else
							{
								IValue *v = nullptr;
								v = BuildingSystem->GetProperty(WallData->GetInt("P1"), "Walls");
								if (v)
								{
									kArray<int> WallIDs = v->IntArrayValue();
									if (WallIDs.size() <= 1)
									{
										BuildingSystem->DeleteObject(BestID);
										AWallPlaneBase** _WallPlane = WallSystem->WallPlaneMap.Find(BestID);
										if (_WallPlane)
										{
											AWallPlaneBase* WallPlane = *_WallPlane;
											if (WallPlane)
											{
												WallPlane->Destroy();
												WallPlane = nullptr;
												WallSystem->WallPlaneMap.Remove(BestID);
											}
										}

										if (NewPointIndex != WallData->GetInt("P1"))
										{
											BuildingSystem->DeleteObject(NewPointIndex);
										}

										BuildingSystem->DeleteObject(WallData->GetInt("P1"));
										NewPointIndex = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, NewPointIndex);

										BuildingSystem->AddWall(WallData->GetInt("P0"), NewPointIndex, leftthick, rightthick, height);
									}
									else
									{
										if (NewPointIndex != WallData->GetInt("P1"))
										{
											BuildingSystem->DeleteObject(NewPointIndex);
										}

										NewPointIndex = BuildingSystem->AddCorner(Crossoverpoint);
										WallSystem->AddNewCorner(Crossoverpoint, NewPointIndex);
									}
								}
							}
						}
						else
						{
							if (NewPointIndex != WallData->GetInt("P0") && NewPointIndex != WallData->GetInt("P1"))
							{
								BuildingSystem->DeleteObject(NewPointIndex);
								NewPointIndex = BuildingSystem->AddCorner(Crossoverpoint);
								WallSystem->AddNewCorner(Crossoverpoint, NewPointIndex);
							}
						}
					}
				}
			}
		}
		else
		{
			BuildingSystem->DeleteObject(NewPointIndex);
			NewPointIndex = BuildingSystem->AddCorner(MouseSnapPos - wallthick * Dir);
			WallSystem->AddNewCorner(MouseSnapPos - wallthick * Dir, NewPointIndex);
		}
	}
	else
	{
		BuildingSystem->DeleteObject(NewPointIndex);
		NewPointIndex = BuildingSystem->AddCorner(MouseSnapPos - wallthick * Dir);
		WallSystem->AddNewCorner(MouseSnapPos - wallthick * Dir, NewPointIndex);
	}
}

void UDrawWallCmd::CheckAndAddCornerWall(FVector2D MouseSnapPos, int32& NewPointIndex)
{
	if (_SnapPoint.bActive)
	{
		if (_SnapPoint.BestID != INDEX_NONE)
		{
			NewPointIndex = _SnapPoint.BestID;
		}
		else
		{
			NewPointIndex = BuildingSystem->AddCorner(_SnapPoint.Loc);
			WallSystem->AddNewCorner(_SnapPoint.Loc, NewPointIndex);
		}

		if (LastPoint != INDEX_NONE)
		{
			TArray<int32>TempWallIDs;
			IValue *v = nullptr;
			v = BuildingSystem->GetProperty(NewPointIndex, "Walls");
			if (v)
			{
				kArray<int> WallIDs = v->IntArrayValue();
				for (int i = 0; i < WallIDs.size(); ++i)
				{
					TempWallIDs.Add(WallIDs[i]);
				}
			}
			IValue *v2 = nullptr;
			v2 = BuildingSystem->GetProperty(LastPoint, "Walls");
			if (v2)
			{
				kArray<int> WallIDs = v2->IntArrayValue();
				for (int i = 0; i < WallIDs.size(); ++i)
				{
					TempWallIDs.Add(WallIDs[i]);
				}
			}
			TArray<int32> WallIDs;
			WallSystem->GetAllObject(WallIDs, EObjectType::ESolidWall, false);
			for (int i = 0; i < WallIDs.Num(); ++i)
			{
				UBuildingData *WallData = BuildingSystem->GetData(WallIDs[i]);
				if (WallData)
				{
					int P0 = WallData->GetInt("P0");
					int P1 = WallData->GetInt("P1");
					IObject *CornerData1 = BuildingSystem->GetObject(P0);
					IObject *CornerData2 = BuildingSystem->GetObject(P1);
					IObject *CornerDataLast = BuildingSystem->GetObject(LastPoint);
					if (CornerData1&&CornerData2&&CornerDataLast)
					{
						FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
						FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
						FVector2D LastCornerPos = ToVector2D(CornerDataLast->GetPropertyValue("Location").Vec2Value());
						if (WallSystem->IsTwoLineSegmentsIntersect(StartCornerPos, EndCornerPos, LastCornerPos, MouseSnapPos))
						{
							if (!TempWallIDs.Contains(WallIDs[i]) && std::abs(FVector2D::CrossProduct((MouseSnapPos - LastCornerPos).GetSafeNormal(), (StartCornerPos - EndCornerPos).GetSafeNormal()) - 0) < 0.001f)
							{
								WallSystem->ShowUniteToast(TEXT("墙体重叠，绘制无效"), 1, 2);
								CancelLastOperation();
								NewPointIndex = INDEX_NONE;
								return;
							}
							else
								if (TempWallIDs.Contains(WallIDs[i]))
								{
									FVector2D TempCornerPos1;
									FVector2D TempCornerPos2;
									if (LastPoint == P0)
									{
										TempCornerPos1 = EndCornerPos;
										TempCornerPos2 = LastCornerPos;
									}
									else
										if (LastPoint == P1)
										{
											TempCornerPos1 = StartCornerPos;
											TempCornerPos2 = LastCornerPos;
										}
										else
											if (NewPointIndex == P0)
											{
												TempCornerPos1 = StartCornerPos;
												TempCornerPos2 = EndCornerPos;
											}
											else
												if (NewPointIndex == P1)
												{
													TempCornerPos1 = EndCornerPos;
													TempCornerPos2 = StartCornerPos;
												}
									//float aaaa = FVector2D::DotProduct((MouseSnapPos - LastCornerPos).GetSafeNormal(), (TempCornerPos1 - TempCornerPos2).GetSafeNormal());
									if (std::abs(FVector2D::DotProduct((MouseSnapPos - LastCornerPos).GetSafeNormal(), (TempCornerPos1 - TempCornerPos2).GetSafeNormal()) - 1) < 0.001f)
									{
										WallSystem->ShowUniteToast(TEXT("墙体重叠，绘制无效"), 1, 2);
										CancelLastOperation();
										NewPointIndex = INDEX_NONE;
										return;
									}
								}
							FVector2D Loc = WallSystem->GetwoSegmentsIntersect(StartCornerPos, EndCornerPos, LastCornerPos, MouseSnapPos);
							IObject * WallObject = BuildingSystem->GetObject(WallIDs[i]);
							if (WallObject)
							{
								IValue & _Value = WallObject->GetPropertyValue("Holes");
								const int num = _Value.GetArrayCount();
								for (int i = 0; i < num; ++i)
								{
									IValue & _Hole = _Value.GetField(i);
									IValue & _CValue = _Hole.GetField("HoleID");
									int _ID = _CValue.IntValue();
									UBuildingData* HoleData = BuildingSystem->GetData(_ID);
									if (HoleData)
									{
										FVector2D HolePos = FVector2D(HoleData->GetVector(TEXT("Location")));
										float Width = HoleData->GetFloat(TEXT("Width"));
										if ((Loc - HolePos).Size() < Width / 2 + 5)
										{
											WallSystem->ShowToastDrawWall();
											if (LastPoint != INDEX_NONE)
											{
												CancelLastOperation();
												NewPointIndex = INDEX_NONE;
											}
											else
											{
												CommandWillEnd();
											}
											return;
										}
									}
								}
							}
						}

					}
				}
			}

		}



	}
	else
	{
		int Objid = INDEX_NONE;
		Objid = BuildingSystem->HitTest(MouseSnapPos);
		if (Objid != INDEX_NONE)
		{
			UBuildingData* Data = BuildingSystem->GetData(Objid);
			if (Data)
			{
				EObjectType Type = (EObjectType)Data->GetObjectType();

				if (Type == EObjectType::EDoorHole || Type == EObjectType::EWindow)
				{
					WallSystem->ShowToastDrawWall();
					if (LastPoint != INDEX_NONE)
						CancelLastOperation();
					else
					{
						CommandWillEnd();
					}
					return;
				}
				if (Type == EObjectType::ESolidWall)
				{
					IValue & _Value = BuildingSystem->GetObject(Objid)->GetPropertyValue("Holes");
					IObject * WallObject = BuildingSystem->GetObject(Objid);
					if (WallObject)
					{
						IValue & _Value = WallObject->GetPropertyValue("Holes");
						const int num = _Value.GetArrayCount();
						for (int i = 0; i < num; ++i)
						{
							IValue & _Hole = _Value.GetField(i);
							IValue & _CValue = _Hole.GetField("HoleID");
							int _ID = _CValue.IntValue();
							UBuildingData* HoleData = BuildingSystem->GetData(_ID);
							if (HoleData)
							{
								FVector2D HolePos = FVector2D(HoleData->GetVector(TEXT("Location")));
								float Width = HoleData->GetFloat(TEXT("Width"));
								if ((MouseSnapPos - HolePos).Size() < Width / 2 + 10)
								{
									WallSystem->ShowToastDrawWall();
									if (LastPoint != INDEX_NONE)
										CancelLastOperation();
									else
									{
										CommandWillEnd();
									}
									return;
								}
							}
						}
					}
				}
			}
		}
		if (LastPoint != INDEX_NONE)
		{
			TArray<int32>TempWallIDs;
			IValue *v = nullptr;
			v = BuildingSystem->GetProperty(LastPoint, "Walls");
			if (v)
			{
				kArray<int> WallIDs = v->IntArrayValue();
				for (int i = 0; i < WallIDs.size(); ++i)
				{
					TempWallIDs.Add(WallIDs[i]);
				}
			}
			TArray<int32> WallIDs;
			WallSystem->GetAllObject(WallIDs, EObjectType::ESolidWall, false);
			for (int i = 0; i < WallIDs.Num(); ++i)
			{
				UBuildingData *WallData = BuildingSystem->GetData(WallIDs[i]);
				if (WallData)
				{
					int P0 = WallData->GetInt("P0");
					int P1 = WallData->GetInt("P1");
					IObject *CornerData1 = BuildingSystem->GetObject(P0);
					IObject *CornerData2 = BuildingSystem->GetObject(P1);
					IObject *CornerDataLast = BuildingSystem->GetObject(LastPoint);
					if (CornerData1&&CornerData2&&CornerDataLast)
					{
						FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
						FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
						FVector2D LastCornerPos = ToVector2D(CornerDataLast->GetPropertyValue("Location").Vec2Value());
						if (WallSystem->IsTwoLineSegmentsIntersect(StartCornerPos, EndCornerPos, LastCornerPos, MouseSnapPos))
						{
							//float aaaa = FVector2D::CrossProduct((MouseSnapPos - LastCornerPos).GetSafeNormal(), (StartCornerPos - EndCornerPos).GetSafeNormal());
							if (!TempWallIDs.Contains(WallIDs[i]) && std::abs(FVector2D::CrossProduct((MouseSnapPos - LastCornerPos).GetSafeNormal(), (StartCornerPos - EndCornerPos).GetSafeNormal()) - 0) < 0.001f)
							{
								WallSystem->ShowUniteToast(TEXT("墙体重叠，绘制无效"), 1, 2);
								CancelLastOperation();
								return;
							}
							else
								if (TempWallIDs.Contains(WallIDs[i]))
								{
									FVector2D TempCornerPos;
									if (LastPoint == P0)
									{
										TempCornerPos = EndCornerPos;
									}
									else
										TempCornerPos = StartCornerPos;
									if (std::abs(FVector2D::DotProduct((MouseSnapPos - LastCornerPos).GetSafeNormal(), (TempCornerPos - LastCornerPos).GetSafeNormal()) - 1) < 0.001f)
									{
										WallSystem->ShowUniteToast(TEXT("墙体重叠，绘制无效"), 1, 2);
										CancelLastOperation();
										return;
									}
								}
							FVector2D Loc = WallSystem->GetwoSegmentsIntersect(StartCornerPos, EndCornerPos, LastCornerPos, MouseSnapPos);
							IObject * WallObject = BuildingSystem->GetObject(WallIDs[i]);
							if (WallObject)
							{
								IValue & _Value = WallObject->GetPropertyValue("Holes");
								const int num = _Value.GetArrayCount();
								for (int i = 0; i < num; ++i)
								{
									IValue & _Hole = _Value.GetField(i);
									IValue & _CValue = _Hole.GetField("HoleID");
									int _ID = _CValue.IntValue();
									UBuildingData* HoleData = BuildingSystem->GetData(_ID);
									if (HoleData)
									{
										FVector2D HolePos = FVector2D(HoleData->GetVector(TEXT("Location")));
										float Width = HoleData->GetFloat(TEXT("Width"));
										if ((Loc - HolePos).Size() < Width / 2 + 5)
										{
											WallSystem->ShowToastDrawWall();
											if (LastPoint != INDEX_NONE)
												CancelLastOperation();
											else
											{
												CommandWillEnd();
											}
											return;
										}
									}
								}
							}
						}

					}
				}
			}

		}
		NewPointIndex = BuildingSystem->AddCorner(MouseSnapPos);
		WallSystem->AddNewCorner(MouseSnapPos, NewPointIndex);
	}


	if (LastPoint != INDEX_NONE)
	{
		float height = WallSystem ? WallSystem->DefaultWallProp.WallNode.Height : 280;
		float leftthick = WallSystem ? WallSystem->WallThickness : 10;
		float rightthick = WallSystem ? WallSystem->WallThickness : 10;
		TArray<int32> wallObjectIds = BuildingSystem->AddWall(LastPoint, NewPointIndex, leftthick, rightthick, height);
		UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
		if (projectDataManager)
		{
			UWorld*MyWorld = GetWorld();
			if (MyWorld)
			{
				UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
				if (GameInst&&GameInst->DrawHouse_Type == EDrawHouseType::ChaigaiDrawType)
				{
					FVector2D StartPos;
					FVector2D EndPos;
					FVector2D LocalDir;
					UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(LastPoint));
					if (CornerAdapter)
					{
						StartPos = CornerAdapter->GetLocaltion();
					}
					CornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(NewPointIndex));
					if (CornerAdapter)
					{
						EndPos = CornerAdapter->GetLocaltion();
					}
					LocalDir = (EndPos - StartPos).GetSafeNormal();
					for (const int32& wallId : wallObjectIds)
					{
						UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(wallId));
						if (solidWallAdapter == nullptr) {
							continue;
						}
						FVector2D TempDir = (solidWallAdapter->GetOriginalEndtPos() - solidWallAdapter->GetOriginalSartPos()).GetSafeNormal();
						float angle = FVector2D::CrossProduct(TempDir, LocalDir);
						FillWallDefaultProperties(wallId);
						if (FMath::Abs(angle) < 0.02)
						{
							solidWallAdapter->SetSolidWallType(ESolidWallType::EDRNewWall);
							solidWallAdapter->SetNewWallType(ENewWallType::EPlasterWall);
						}
						else
							if (WallSystem->IsinterlinkNewWall(wallId))
							{
								solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(wallId));
								if (solidWallAdapter)
								{
									solidWallAdapter->SetSolidWallType(ESolidWallType::EDRNewWall);
									solidWallAdapter->SetNewWallType(ENewWallType::EPlasterWall);
								}
							}
					}
				}
				else
				{
					for (const int32& wallId : wallObjectIds)
					{

						UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(wallId));
						if (solidWallAdapter == nullptr) {
							continue;
						}
						FillWallDefaultProperties(wallId);
					}
				}
			}
		}
		WallSystem->BuildRoom();
		UpdataCorner();
	}
}