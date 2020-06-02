// Copyright? 2017 ihomefnt All Rights Reserved.


#include "ComponentManagerActor.h"
#include "../../EditorGameInstance.h"
#include "WidgetLayoutLibrary.h"
#include "FlueComponentPrimitive.h"
#include "UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "PillarComponentPrimitive.h"
#include "BagRiserComponentPrimitive.h"
#include "SewerComponentPrimitive.h"
#include "../AHousePluginActor.h"
#include "BuildingGroupMeshComponent.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include "../../HomeLayout/HouseArchitect/WallBuildSystem.h"

// Sets default values
AComponentManagerActor::AComponentManagerActor()
	:b(true)
	, bFindWall(false)
	, PreID(INDEX_NONE)
	, bSelect(false)
	, MoveWallPos(FVector2D(0,0))
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AComponentManagerActor::BeginPlay()
{
	Super::BeginPlay();

	InitBuildSystem();
	//b = true;
	//bFindWall = false;
	//PreID=INDEX_NONE
}


// Called every frame
void AComponentManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AComponentManagerActor::Destroyed()
{
	if (ComponentBaseWidget)
	{
		ComponentBaseWidget->RemoveFromParent();
		ComponentBaseWidget = nullptr;
	}

	if (CompPositionWidget)
	{
		CompPositionWidget->RemoveFromParent();
		CompPositionWidget = nullptr;
	}

	Super::Destroyed();
}

void AComponentManagerActor::InitBuildSystem()
{
	if (!BuildingSystem)
	{
		bLeftMouseActive = false;
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
		BuildingSystem = GameInst->GetBuildingSystemInstance();

	}

}

FVector2D AComponentManagerActor::GetMouse2D(const FVector2D& screenPos)
{
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		FVector worldPos, worldDir;

		if (UGameplayStatics::DeprojectScreenToWorld(MyWorld->GetFirstPlayerController(), screenPos, worldPos, worldDir))
		{
			return FVector2D(worldPos.X, worldPos.Y);
		}
		else
			return FVector2D(0, 0);
	}
	return FVector2D(0, 0);

}

EObjectType AComponentManagerActor::FindWalls(const FVector2D& Loc, int32& WallID)
{

	if (BuildingSystem)
	{
		WallID = BuildingSystem->HitTest(Loc);
		if (WallID != INDEX_NONE)
		{
			UBuildingData* Data = BuildingSystem->GetData(WallID);
			if (Data)
			{
				EObjectType Type = (EObjectType)Data->GetObjectType();
				if (EObjectType::ESolidWall == Type)
				{
					return EObjectType::ESolidWall;
				}
				else if (EObjectType::EDoorHole == Type || EObjectType::EWindow == Type)
				{
					int ID = Data->GetInt("WallID");
					if (ID != INDEX_NONE)
					{
						WallID = ID;
						return EObjectType::ESolidWall;
					}
				}
			}


		}
	}
	return EObjectType::EUnkownObject;
}

void AComponentManagerActor::NewComponent(UWorld* MyWorld, const FVector2D& screenPos)
{
	if (!(BuildingSystem&&ComponentBaseWidget))
		return;
	UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (!ComponentBase)
		return;
	if (ComponentBase->GetComponenetType() == EComponenetType::_Sewer)
	{
		ComponentBase->PrimitiveVertexDataGenerate(MyWorld, screenPos);
		return;
	}
	int32 id = BuildingSystem->HitTest(screenPos);
	if (bFindWall)
	{
		float dis = curSideData - screenPos;
		float d = FMath::Sqrt((ComponentBase->GetLength() / 2.0f)*(ComponentBase->GetLength() / 2.0f) +
			(ComponentBase->GetWidth() / 2.0f)*(ComponentBase->GetWidth() / 2.0f));
		if (abs(dis - d) > d * 2)
		{
			ComponentBase->RemoveAll();
			ComponentBase->SetAngleVal(0);
			bFindWall = false;
			ComponentBase->PrimitiveVertexDataGenerate(MyWorld, screenPos);
		}
		else
		{
			if (curSideData.wallId != INDEX_NONE)
			{
				MovePlugin(MyWorld, screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength());
			}
			else
				ComponentBase->PrimitiveVertexDataGenerate(MyWorld, screenPos);
		}
	}
	else
	{
		if (id != INDEX_NONE)
		{
			UBuildingData* data = BuildingSystem->GetData(id);
			if (data)
			{
				EObjectType type = (EObjectType)data->GetObjectType();
				if (type == EObjectType::ERoom || type == EObjectType::EArea || type == EObjectType::EPolygonArea)
				{
					if (PreID != id)
					{
						roomVecDatas.Empty();
						ComponentBase->RemoveAll();
						ComponentBase->SetAngleVal(0);
						PreID = id;
						UpdatePlygon(id);
						bFindWall = false;
					}
					if (roomVecDatas.Num() > 2)
					{
						float d = -1;
						d = FMath::Sqrt((ComponentBase->GetWidth()*ComponentBase->GetWidth() + ComponentBase->GetLength()*ComponentBase->GetLength()) / 4);
						for (int32 i = 0; i < roomVecDatas.Num(); ++i)
						{
							float dis = roomVecDatas[i] - screenPos;
							if (dis - d <= AdsorptionRange)
							{
								curSideData = roomVecDatas[i];
								if (i == 0)
								{
									preSideData = roomVecDatas[roomVecDatas.Num() - 1];
									nextSideData = roomVecDatas[1];
								}
								else if (i == roomVecDatas.Num() - 1)
								{
									preSideData = roomVecDatas[i - 1];
									nextSideData = roomVecDatas[0];
								}
								else
								{
									preSideData = roomVecDatas[i - 1];
									nextSideData = roomVecDatas[i + 1];
								}
								bFindWall = true;
								b = true;
								if (curSideData.wallId != INDEX_NONE)
								{
									MovePlugin(MyWorld, screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength());
								}
								else
									ComponentBase->PrimitiveVertexDataGenerate(MyWorld, screenPos);
								break;
							}
						}

					}
				}
			}
			ComponentBase->PrimitiveVertexDataGenerate(MyWorld, screenPos);
		}
		else
		{
			ComponentBase->PrimitiveVertexDataGenerate(MyWorld, screenPos);
		}

	}
}
int AComponentManagerActor::HitPolygon(int nvert, float *vertx, float *verty, float testx, float testy)
{
	int i, j, c = 0;
	for (i = 0, j = nvert - 1; i < nvert; j = i++) {
		if (((verty[i] > testy) != (verty[j] > testy)) &&
			(testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
			c = !c;
	}
	return c;
}


int AComponentManagerActor::GetPolygonNoCommonPoint(int32 ObjecID, TArray<FVector2D>& TPoylgon)
{
	if (BuildingSystem)
	{
		TPoylgon.Empty();
		BuildingSystem->GetPolygon(ObjecID, TPoylgon, true);
		TArray<FVector2D> needRemoveVec;
		if (TPoylgon.Num() > 0)
		{
			for (int i = 0; i < TPoylgon.Num(); ++i)
			{
				FVector2D preVec, curVec, nextVec;
				if (i == 0)
				{
					curVec = TPoylgon[i];
					preVec = TPoylgon[TPoylgon.Num() - 1];
					nextVec = TPoylgon[i + 1];
				}
				else if (i == TPoylgon.Num() - 1)
				{
					curVec = TPoylgon[i];
					preVec = TPoylgon[i - 1];
					nextVec = TPoylgon[0];
				}
				else
				{
					curVec = TPoylgon[i];
					preVec = TPoylgon[i - 1];
					nextVec = TPoylgon[i + 1];
				}
				FVector2D V1, V2;
				V1 = curVec - preVec;
				V2 = curVec - nextVec;
				V1.Normalize();
				V2.Normalize();
				float f = V1 ^ V2;
				if (FMath::Abs(f)<0.02f)
				{
					needRemoveVec.Add(curVec);
				}
			}
			for (auto& It : needRemoveVec)
			{
				TPoylgon.Remove(It);
			}

		}
	}
	return TPoylgon.Num();
}


int AComponentManagerActor::FindRoomVecData(const FRoomVecData& data)
{

	for (int32 i = 0; i < roomVecDatas.Num(); ++i)
	{
		if (roomVecDatas[i] == data)
			return i;
	}
	return INDEX_NONE;
}

void AComponentManagerActor::UpdatePlygon(int32 id)
{
	UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (ComponentBase)
	{
		if (id != INDEX_NONE)
		{
			TArray<FVector2D> TPoylgon;
			int num = GetPolygonNoCommonPoint(id, TPoylgon);
			if (num >= 3)
			{
				ComponentBase->SetTPoylgon(TPoylgon);
				roomVecDatas.Empty();
				for (int i = 0; i < TPoylgon.Num(); i++)
				{
					FRoomVecData vec;
					if (i == TPoylgon.Num() - 1)
					{
						//vec.startPos = TPoylgon[i];
						//vec.endPos = TPoylgon[0];
						vec.InitData(this, TPoylgon[i], TPoylgon[0]);
					}
					else
					{
						//vec.startPos = TPoylgon[i];
						//vec.endPos = TPoylgon[i + 1];
						vec.InitData(this, TPoylgon[i], TPoylgon[i + 1]);
					}
					roomVecDatas.Add(vec);
				}
			}

		}
	}

}

void AComponentManagerActor::GetNewPos(UComponentBasePrimitive* ComponentBase, const FRoomVecData& data, FVector2D& P0, FVector2D& P1)
{
	FVector2D dir = data.endPos - data.startPos;
	float dis = FVector2D::Distance(data.endPos, data.startPos);
	dir.Normalize();
	if (data.IsWidth)
	{
		P0 = data.endPos - dir * (dis - ComponentBase->GetWidth() / 2.0f);
		P1 = data.startPos + dir * (dis - ComponentBase->GetWidth() / 2.0f);
	}
	else
	{
		P0 = data.endPos - dir * (dis - ComponentBase->GetLength() / 2.0f);
		P1 = data.startPos + dir * (dis - ComponentBase->GetLength() / 2.0f);
	}
}

bool AComponentManagerActor::CalculateSegmentAngle(const FRoomVecData& Predata, const FRoomVecData& Curdata, float& Angle)
{
	FVector2D Vec1 = (Predata.endPos - Predata.startPos).GetSafeNormal();
	FVector2D Vec2 = (Curdata.endPos - Curdata.startPos).GetSafeNormal();
	float result = FVector2D::DotProduct(Vec1, Vec2);
	if (result < 0)
	{
		Angle = FMath::Acos(result)*180/PI;
		return true;
	}
	return false;
}

void  AComponentManagerActor::UpdateArea(int32 id)
{
	if (id != INDEX_NONE)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			UpdatePlygon(id);
			for (int i = 0; i < roomVecDatas.Num(); ++i)
			{
				float dis = roomVecDatas[i] - ComponentBase->GetLoc();
				dis = dis * 2.0f;
				if ((dis - ComponentBase->GetWidth() < 4.0f) || (dis - ComponentBase->GetLength() < 4.0f))
				{
					curSideData = roomVecDatas[i];
					if (i == 0)
					{
						preSideData = roomVecDatas[roomVecDatas.Num() - 1];
						nextSideData = roomVecDatas[1];
					}
					else if (i == roomVecDatas.Num() - 1)
					{
						preSideData = roomVecDatas[i - 1];
						nextSideData = roomVecDatas[0];
					}
					else
					{
						preSideData = roomVecDatas[i - 1];
						nextSideData = roomVecDatas[i + 1];
					}
					ComponentBase->SetAngleVal(curSideData.AngleVal);
					break;
				}
				//curSideData = roomVecDatas[i];
				//if (i == 0)
				//{
				//	preSideData = roomVecDatas[roomVecDatas.Num() - 1];
				//	nextSideData = roomVecDatas[i + 1];
				//}
				//else if (i == (roomVecDatas.Num() - 1))
				//{
				//	preSideData = roomVecDatas[i - 1];
				//	nextSideData = roomVecDatas[0];
				//}
				//else
				//{
				//	preSideData = roomVecDatas[i - 1];
				//	nextSideData = roomVecDatas[i + 1];
				//}
				//ComponentBase->SetAngleVal(curSideData.AngleVal);
				//break;
			}
		}
	}
}

void AComponentManagerActor::UpdateObject(/*int32 id,*/int32 wallID)
{
	UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (ComponentBase&&BuildingSystem)
	{
		TArray<FVector> WallNodes;
		float height;
		BuildingSystem->GetWallBorderLines(wallID, WallNodes, height);
		if (WallNodes.Num() == 6)
		{
			float PlugPrimitiveAng= ComponentBase->GetAngleVal();
			FVector2D PostAtWall, LocVect;
			FVector2D dir = FVector2D(WallNodes[3] - WallNodes[0]);
			FVector2D rightDir = dir.GetRotated(90);
			rightDir.Normalize();
			dir.Normalize();
			//float dirlength = 0;
			/*if (FMath::Abs(AlongWallLength - ComponentBase->GetWidth() * 2) < FMath::Abs(AlongWallLength - ComponentBase->GetLength() * 2))
			{
				dirlength = ComponentBase->GetWidth() / 2;
			}
			else
				dirlength = ComponentBase->GetLength() / 2;*/
			if (WallIndex == 5)
			{
				PostAtWall = FVector2D(WallNodes[5]) + dir * WallCornnerLength;
				LocVect = PostAtWall + rightDir* AlongWallLength;
			}
			else
				if (WallIndex == 1)
				{
					PostAtWall = FVector2D(WallNodes[1]) + dir * WallCornnerLength;
					LocVect = PostAtWall - rightDir* AlongWallLength;
				}
				else
					if (WallIndex == 4)
					{
						PostAtWall = FVector2D(WallNodes[4]) - dir * WallCornnerLength;
						LocVect = PostAtWall + rightDir* AlongWallLength;
					}
					else
						if (WallIndex == 2)
						{
							PostAtWall = FVector2D(WallNodes[2]) - dir * WallCornnerLength;
							LocVect = PostAtWall -rightDir* AlongWallLength;
						}

			UWorld *MyWorld = GetWorld();
			ComponentBase->PrimitiveVertexDataGenerate(MyWorld, LocVect);
			EComponenetType type = ComponentBase->GetComponenetType();
			switch (type)
			{
			case EComponenetType::_FLUE:
				BuildingSystem->UpdateFlue(ComponentBase->GetComponentDRActorID(), LocVect, PlugPrimitiveAng);
				break;
			case EComponenetType::_BagRiser:
				BuildingSystem->UpdateTube(ComponentBase->GetComponentDRActorID(), LocVect, PlugPrimitiveAng);
				break;
			case EComponenetType::_Pillar:
				BuildingSystem->UpdatePillar(ComponentBase->GetComponentDRActorID(), LocVect, PlugPrimitiveAng);
				break;
			}
		}
	}
}

void AComponentManagerActor::DeleteObject(int32 id)
{
	if (id != INDEX_NONE)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			if (id == ComponentBase->GetComponentRoomID())
			{
				ManualDestroyHouseComponent();
			}
		}
	}
}

//@liff add refresh roomvect data
void AComponentManagerActor::RefreshPolygonData(/*int32 polyId*/)
{
	UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (!ComponentBase)
		return;
	FVector2D screenPos = ComponentBase->GetLoc();
	int32 id = BuildingSystem->HitTest(screenPos);
	UBuildingData* data = BuildingSystem->GetData(id);
	if (data)
	{
		EObjectType type = (EObjectType)data->GetObjectType();
		if (type == EObjectType::ERoom || type == EObjectType::EArea || type == EObjectType::EPolygonArea)
		{
			if (PreID != id)
			{
				roomVecDatas.Empty();
				PreID = id;
				UpdatePlygon(id);
			}
		}
	}
	else
	{
		return;
	}
}
//liff add end


float AComponentManagerActor::GetPluginAreaAtWall(int32 id)
{
	if (BuildingSystem&&ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			TArray<int32> WallIds = ComponentBase->GetAllWallIdsInfo();
			if (INDEX_NONE != WallIds.Find(id))
			{
				return 0.f;
			}
		}
	}
	return 0.f;
}

void AComponentManagerActor::MovePlugin(UWorld* MyWorld, const FVector2D&Loc, float Width, float Length)
{
	if (BuildingSystem)
	{
		UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
		FVector2D CenterMouse2D;
		CenterMouse2D = ComponentBase->GetLoc();
		FVector2D newStartPos, newEndPos;
		float SegmentAngle = 0;
		GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
		if (CalculateSegmentAngle(preSideData, curSideData, SegmentAngle))
		{
			FVector2D dir = curSideData.endPos - curSideData.startPos;
			float dis = FVector2D::Distance(curSideData.endPos, curSideData.startPos);
			dir.Normalize();
			float tempAngle = 180 - SegmentAngle;
			newStartPos = curSideData.endPos - dir * ((dis - ComponentBase->GetLength() / 2.0f) - (ComponentBase->GetWidth() / FMath::Tan(tempAngle / 180 * PI)));

		}
		if (CalculateSegmentAngle(curSideData, nextSideData, SegmentAngle))
		{
			FVector2D dir = curSideData.endPos - curSideData.startPos;
			float dis = FVector2D::Distance(curSideData.endPos, curSideData.startPos);
			dir.Normalize();
			float tempAngle = 180 - SegmentAngle;
			newEndPos = curSideData.startPos + dir * ((dis - ComponentBase->GetLength() / 2.0f) - (ComponentBase->GetWidth() / FMath::Tan(tempAngle / 180 * PI)));
		}
		FVector2D MouseAtWall = FMath::ClosestPointOnSegment2D(Loc, newStartPos, newEndPos);
		FVector2D CenterAtWall = FMath::ClosestPointOnSegment2D(CenterMouse2D, newStartPos, newEndPos);
		FVector2D dir = Loc - CenterMouse2D;
		//FVector2D dir = MouseAtWall - CenterAtWall;
		dir.Normalize();
		FVector2D sidedir = curSideData.endPos - curSideData.startPos;
		sidedir.Normalize();
		FVector2D Temp = newEndPos - newStartPos;
		Temp.Normalize();
		FVector2D centerDir = Temp.GetRotated(90);
		centerDir.Normalize();
		if (b && (dir | sidedir) >= 0)
		{

			FVector2D newNextStartPos, newNextEndPos;
			GetNewPos(ComponentBase, nextSideData, newNextStartPos, newNextEndPos);
			float dis = FMath::PointDistToSegment(FVector(MouseAtWall, 0), FVector(newNextStartPos, 0), FVector(newNextEndPos, 0));

			if (dis <= AdsorptionRange && nextSideData.wallId != INDEX_NONE)
			{
				if (curSideData.IsWidth)
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
				else
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
			}
			else
			{
				if (curSideData.IsWidth)
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
				else
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
			}
			ComponentBase->SetAngleVal(curSideData.AngleVal);
			ComponentBase->AddWallInfo(curSideData.wallId);
			ComponentBase->SetIsWidth(curSideData.IsWidth);
		}
		else if (b && (dir | sidedir) < 0)
		{

			FVector2D newPreStartPos, newPreEndPos;
			GetNewPos(ComponentBase, preSideData, newPreStartPos, newPreEndPos);
			float dis = FMath::PointDistToSegment(FVector(MouseAtWall, 0), FVector(newPreStartPos, 0), FVector(newPreEndPos, 0));
			if (dis <= AdsorptionRange && preSideData.wallId != INDEX_NONE)
			{
				if (curSideData.IsWidth)
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
				else
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
			}
			else
			{
				if (curSideData.IsWidth)
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
				else
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
			}
			ComponentBase->SetAngleVal(curSideData.AngleVal);
			ComponentBase->AddWallInfo(curSideData.wallId);
			ComponentBase->SetIsWidth(curSideData.IsWidth);
		}
		else
		{
			float disStart = FVector2D::Distance(CenterMouse2D, curSideData.startPos);
			float disEnd = FVector2D::Distance(CenterMouse2D, curSideData.endPos);
			float d = FMath::Sqrt((ComponentBase->GetLength() / 2.0f)*(ComponentBase->GetLength() / 2.0f) +
				(ComponentBase->GetWidth() / 2.0f)*(ComponentBase->GetWidth() / 2.0f));
			//if (ComponentBase->GetComponenetType() == EComponenetType::_Pillar)
			//{
			/*	float mouseDis = FMath::PointDistToSegment(FVector(Loc, 0), FVector(newStartPos, 0), FVector(newEndPos, 0));
				if (abs(mouseDis - d) > d * 2)
				{
					ComponentBase->SetAngleVal(0);
					ComponentBase->RemoveAll();
					bFindWall = false;
				}*/
			//}
			//else
				//ComponentBase->RenewSetWall(curSideData.wallId);

			if (disStart - d <= AdsorptionRange)
			{
				FVector2D mouseDir = Loc - curSideData.startPos;
				FVector2D preSideDir = preSideData.startPos - preSideData.endPos;
				FVector2D curSideDir = curSideData.endPos - curSideData.startPos;
				float preSideLen = (mouseDir | preSideDir) / preSideDir.Size();
				float curSideLen = (mouseDir | curSideDir) / curSideDir.Size();
				if (preSideLen > d + AdsorptionRange && preSideData.wallId != INDEX_NONE)
				{
					ComponentBase->DestroyWall();
					b = true;
					curSideData = preSideData;
					int index = FindRoomVecData(curSideData);
					if (index != INDEX_NONE)
					{
						if (index == 0)
						{
							preSideData = roomVecDatas[roomVecDatas.Num() - 1];
							nextSideData = roomVecDatas[1];
						}
						else if (index == roomVecDatas.Num() - 1)
						{
							preSideData = roomVecDatas[index - 1];
							nextSideData = roomVecDatas[0];
						}
						else
						{
							preSideData = roomVecDatas[index - 1];
							nextSideData = roomVecDatas[index + 1];
						}
						ComponentBase->SetAngleVal(curSideData.AngleVal);
						ComponentBase->AddWallInfo(curSideData.wallId);
						FVector2D newStartPos, newEndPos;
						GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
						MouseAtWall = FMath::ClosestPointOnSegment2D(Loc, newStartPos, newEndPos);
						FVector2D Temp = newEndPos - newStartPos;
						Temp.Normalize();
						centerDir = Temp.GetRotated(90);
						centerDir.Normalize();
						if (curSideData.IsWidth)
							CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
						else
							CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
						ComponentBase->SetIsWidth(curSideData.IsWidth);
					}
				}
				else if (curSideLen > d + AdsorptionRange)
				{

					ComponentBase->DestroyWall();
					ComponentBase->SetAngleVal(curSideData.AngleVal);
					b = true;
					FVector2D newStartPos, newEndPos;
					GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
					FVector2D Temp = newEndPos - newStartPos;
					Temp.Normalize();
					centerDir = Temp.GetRotated(90);
					centerDir.Normalize();
					if (curSideData.IsWidth)
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
					else
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
					ComponentBase->SetIsWidth(curSideData.IsWidth);
				}
				else
				{
					b = false;
					FPrimitive2D outPri = GetWorldPosBySrceen(ComponentBase->FramePrimitive);
					float leftTopLen, rightTopLen, rightBottomLen, leftBottomLen;
					leftTopLen = FVector2D::Distance(outPri.LeftTopPos, curSideData.startPos);
					rightTopLen = FVector2D::Distance(outPri.RightTopPos, curSideData.startPos);
					rightBottomLen = FVector2D::Distance(outPri.RightBottomPos, curSideData.startPos);
					leftBottomLen = FVector2D::Distance(outPri.LeftBottomPos, curSideData.startPos);
					float AngleVal1 = 0;

					if (leftTopLen < rightTopLen&&leftTopLen < rightBottomLen&&leftTopLen < leftBottomLen)
					{
						AngleVal1 = 180;
					}
					else if (rightTopLen < leftTopLen&&rightTopLen < rightBottomLen&&rightTopLen < leftBottomLen)
					{
						AngleVal1 = -90;
					}
					else if (leftBottomLen < rightBottomLen&&leftBottomLen < rightTopLen&&leftBottomLen < leftTopLen)
					{
						AngleVal1 = 90;
					}
					else
					{
						AngleVal1 = 0.0f;
					}
					float L = ComponentBase->GetLength();
					float W = ComponentBase->GetWidth();
					if (AngleVal1 == 180.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.startPos.X - W / 2.0f, curSideData.startPos.Y + L / 2.0f));
					}
					else if (AngleVal1 == -90.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.startPos.X - L / 2.0f, curSideData.startPos.Y - W / 2.0f));

					}
					else if (AngleVal1 == 0.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.startPos.X + W / 2.0f, curSideData.startPos.Y - L / 2.0f));
					}
					else if (AngleVal1 == 90.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.startPos.X + L / 2.0f, curSideData.startPos.Y + W / 2.0f));
					}
					ComponentBase->SetAngleVal(AngleVal1);
					ComponentBase->rightBottomLoc = curSideData.startPos;
				}
			}
			else if (disEnd - d <= AdsorptionRange && nextSideData.wallId != INDEX_NONE)
			{
				FVector2D mouseDir = Loc - curSideData.endPos;
				/******/
				FVector2D nextSideDir = nextSideData.endPos - nextSideData.startPos;
				FVector2D curSideDir = curSideData.startPos - curSideData.endPos;
				float nextSideLen = (mouseDir | nextSideDir) / nextSideDir.Size();
				float curSideLen = (mouseDir | curSideDir) / curSideDir.Size();
				if (nextSideLen > d + AdsorptionRange)
				{
					ComponentBase->DestroyWall();
					b = true;
					curSideData = nextSideData;
					int index = FindRoomVecData(curSideData);
					if (index != INDEX_NONE)
					{
						if (index == 0)
						{
							preSideData = roomVecDatas[roomVecDatas.Num() - 1];
							nextSideData = roomVecDatas[1];
						}
						else if (index == roomVecDatas.Num() - 1)
						{
							preSideData = roomVecDatas[index - 1];
							nextSideData = roomVecDatas[0];
						}
						else
						{
							preSideData = roomVecDatas[index - 1];
							nextSideData = roomVecDatas[index + 1];
						}
					}
					ComponentBase->SetAngleVal(curSideData.AngleVal);
					ComponentBase->AddWallInfo(curSideData.wallId);
					FVector2D newStartPos, newEndPos;
					GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
					MouseAtWall = FMath::ClosestPointOnSegment2D(Loc, newStartPos, newEndPos);
					FVector2D Temp = newEndPos - newStartPos;
					Temp.Normalize();
					centerDir = Temp.GetRotated(90);
					centerDir.Normalize();
					if (curSideData.IsWidth)
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
					else
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
					ComponentBase->SetIsWidth(curSideData.IsWidth);
				}
				else if (curSideLen > d + AdsorptionRange)
				{
					ComponentBase->DestroyWall();
					b = true;
					ComponentBase->SetAngleVal(curSideData.AngleVal);
					FVector2D newStartPos, newEndPos;
					GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
					FVector2D Temp = newEndPos - newStartPos;
					Temp.Normalize();
					centerDir = Temp.GetRotated(90);
					centerDir.Normalize();
					if (curSideData.IsWidth)
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
					else
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
					ComponentBase->SetIsWidth(curSideData.IsWidth);
				}
				else
				{
					b = false;
					FPrimitive2D outPri = GetWorldPosBySrceen(ComponentBase->FramePrimitive);
					float leftTopLen, rightTopLen, rightBottomLen, leftBottomLen;
					leftTopLen = FVector2D::Distance(outPri.LeftTopPos, curSideData.endPos);
					rightTopLen = FVector2D::Distance(outPri.RightTopPos, curSideData.endPos);
					rightBottomLen = FVector2D::Distance(outPri.RightBottomPos, curSideData.endPos);
					leftBottomLen = FVector2D::Distance(outPri.LeftBottomPos, curSideData.endPos);
					float AngleVal1 = 0;
					if (leftTopLen < rightTopLen&&leftTopLen < rightBottomLen&&leftTopLen < leftBottomLen)
					{
						AngleVal1 = 180;
					}
					else if (rightTopLen < leftTopLen&&rightTopLen < rightBottomLen&&rightTopLen < leftBottomLen)
					{
						AngleVal1 = -90;
					}
					else if (leftBottomLen < rightBottomLen&&leftBottomLen < rightTopLen&&leftBottomLen < leftTopLen)
					{
						AngleVal1 = 90;
					}
					else
					{
						AngleVal1 = 0.0f;
					}

					float L = ComponentBase->GetLength();
					float W = ComponentBase->GetWidth();
					if (AngleVal1 == 180.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.endPos.X - W / 2.0f, curSideData.endPos.Y + L / 2.0f));

					}
					else if (AngleVal1 == -90.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.endPos.X - L / 2.0f, curSideData.endPos.Y - W / 2.0f));

					}
					else if (AngleVal1 == 0.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.endPos.X + W / 2.0f, curSideData.endPos.Y - L / 2.0f));
					}
					else if (AngleVal1 == 90.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.endPos.X + L / 2.0f, curSideData.endPos.Y + W / 2.0f));
					}
					ComponentBase->SetAngleVal(AngleVal1);
					ComponentBase->rightBottomLoc = curSideData.endPos;
				}
			}
		}
		ComponentBase->PrimitiveVertexDataGenerate(MyWorld, CenterMouse2D);
		ComponentBase->PosAtWall = FMath::ClosestPointOnSegment2D(CenterMouse2D, curSideData.startPos, curSideData.endPos);
		ComponentBase->NodeForWard = centerDir;




	}
}

void AComponentManagerActor::SetHousePlugin(AHousePluginActor * HousePl)
{
	if (HousePl)
	{
		UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			HousePluginActor = HousePl;
			HousePl->ComponentManager = this;
			if (HousePluginActor)
			{
				if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
				{
					HousePluginActor->modelID = ComponentBase->GetModelID();
					SetHousePluginActorMaterialInfo(ComponentBase->GetModelID(), ComponentBase->GetRoomClassID(), ComponentBase->GetCraftID());
					InitMaterial(ComponentBase->GetModelID());
					SetHousePluginActorDefaultScale();
					SetHousePluginActorMaterialUVInfo(ComponentBase->GetScale(), ComponentBase->GetOffset(), ComponentBase->GetAngle());
				}
				
			}
		}
	}
}

void AComponentManagerActor::UpdateHousePlugin(int32 id)
{
	if (BuildingSystem)
	{
		//BuildingSystem->UpdateFlue(id);
	}
}


void AComponentManagerActor::CalcuIsWidth(bool b, const FRoomVecData& firstData, const FRoomVecData&  secData)
{

	UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (ComponentBase)
	{
		ComponentBase->RemoveAll();
		if (firstData.IsWidth == !b)
		{
			ComponentBase->AddWallInfo(firstData.wallId);
			ComponentBase->AddWallInfo(secData.wallId);
		}
		else if (secData.IsWidth == !b)
		{
			ComponentBase->AddWallInfo(secData.wallId);
			ComponentBase->AddWallInfo(firstData.wallId);
		}
		ComponentBase->SetIsWidth(b);

	}
}

bool AComponentManagerActor::DetectHitPrimitive2D(const FPrimitive2D& primintivePos, const FVector2D& mousePos)
{
	FVector2D LeftTopPos = primintivePos.LeftTopPos;//GetMouse2D(primintivePos.LeftTopPos);
	FVector2D RightBottomPos = primintivePos.RightBottomPos;//GetMouse2D(primintivePos.RightBottomPos);
	FVector2D RightTopPos = primintivePos.RightTopPos;//GetMouse2D(primintivePos.LeftTopPos);
	FVector2D LeftBottomPos = primintivePos.LeftBottomPos;//GetMouse2D(primintivePos.RightBottomPos);
	FBox box;

	float minX = LeftTopPos.X < RightBottomPos.X ? LeftTopPos.X : RightBottomPos.X;
	minX = minX < RightTopPos.X ? minX : RightTopPos.X;
	minX = minX < LeftBottomPos.X ? minX : LeftBottomPos.X;

	float minY = LeftTopPos.Y < RightBottomPos.Y ? LeftTopPos.Y : RightBottomPos.Y;
	minY = minY < RightTopPos.Y ? minY : RightTopPos.Y;
	minY = minY < LeftBottomPos.Y ? minY : LeftBottomPos.Y;

	float maxX = LeftTopPos.X > RightBottomPos.X ? LeftTopPos.X : RightBottomPos.X;
	maxX = maxX > RightTopPos.X ? maxX : RightTopPos.X;
	maxX = maxX > LeftBottomPos.X ? maxX : LeftBottomPos.X;

	float maxY = LeftTopPos.Y > RightBottomPos.Y ? LeftTopPos.Y : RightBottomPos.Y;
	maxY = maxY > RightTopPos.Y ? maxY : RightTopPos.Y;
	maxY = maxY > LeftBottomPos.Y ? maxY : LeftBottomPos.Y;
	box.Min = FVector(minX, minY, 0);
	box.Max = FVector(maxX, maxY, 0);
	if (FMath::PointBoxIntersection(FVector(mousePos, 0), box))
	{
		return true;
	}
	return false;
}

void AComponentManagerActor::Create2dPrimitiveWidget(EComponenetType type)
{
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		if (!ComponentBaseWidget)
		{
			ComponentBaseWidget = Cast<UComponentBaseWidget>(CreateWidget(MyWorld, UComponentBaseWidget::StaticClass()));
			if (ComponentBaseWidget)
			{
				ComponentBaseWidget->SetComponentManger(this);
				ComponentBaseWidget->AddToViewport(-10);
				ComponentBaseWidget->CreateComponentPrimitive(type);
				UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
				if (ComponentBase)
				{
					ComponentBase->ComponenetState = EComponenetState::_Add;
					UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
					if (GameInst)
					{
						UWallBuildSystem* WallBuildSystem = GameInst->WallBuildSystem;
						if (WallBuildSystem)
						{
							WallBuildSystem->ShowAllCornerUMG();
							WallBuildSystem->RefreshAlignmentPoint(ComponentBase->TempX, ComponentBase->TempY, ComponentBase->ObjectSnapTol);
							ComponentBase->SetWorld(MyWorld);
							ComponentBase->SetDrawActive(true);
						}
					}
				}
			}
		}
	}

}

bool AComponentManagerActor::IsAllowPlacement()
{
	if (BuildingSystem&&ComponentBaseWidget)
	{
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
		if (GameInst)
		{
			UWallBuildSystem* WallBuildSystem = GameInst->WallBuildSystem;
			if (WallBuildSystem)
			{
				WallBuildSystem->ClearAllCornnerUMG();
			}
		}
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			ComponentBase->SetDrawActive(false);
			ComponentBase->SetFrameColor(FLinearColor(1, 0.48, 0, 0));
			FVector2D screenPos = ComponentBase->GetLoc();
			//screenPos = GetMouse2D(screenPos);
			int id = -1;
			int32 ObjID = BuildingSystem->HitTest(screenPos);
			if (ObjID != INDEX_NONE)
			{
				UBuildingData* Data = BuildingSystem->GetData(ObjID);
				if (Data)
				{
					EObjectType Type = (EObjectType)Data->GetObjectType();
					if (Type == EObjectType::ERoom || Type == EObjectType::EArea || Type == EObjectType::EPolygonArea)
					{
							TArray<int32> Walls = ComponentBase->GetAllWallIdsInfo();
							float height = 280;
							if (Walls.Num() == 0||(Walls.Num() == 1 && Walls[0] == INDEX_NONE))
							{
								for (int i = 0; i < roomVecDatas.Num(); ++i)
								{
									if (roomVecDatas[i].wallId != INDEX_NONE)
									{
										UBuildingData* WallData = BuildingSystem->GetData(roomVecDatas[i].wallId);
										height  = WallData->GetFloat("Height");
										break;
									}
								}
							}
							else if (Walls.Num() == 1&& Walls[0]!=INDEX_NONE)
							{
								UBuildingData* WallData = BuildingSystem->GetData(Walls[0]);
								height  = WallData->GetFloat("Height");
							}
							else if (Walls.Num() == 2)
							{
								if (Walls[0] != INDEX_NONE)
								{
									UBuildingData* WallData = BuildingSystem->GetData(Walls[0]);
									height = WallData->GetFloat("Height");
								}
								else if (Walls[1] != INDEX_NONE)
								{
									UBuildingData* WallData = BuildingSystem->GetData(Walls[1]);
									height = WallData->GetFloat("Height");
								}
				
							}
							ComponentBase->SetHeight(height);
							
						EComponenetType type = ComponentBase->GetComponenetType();
						switch (type)
						{
						case EComponenetType::_FLUE:
						case EComponenetType::_BagRiser:
							if (ComponentBase->GetHasWall())
							{
								ComponentBase->SetComponentRoomID(ObjID);
								ComponentBase->ComponenetState = EComponenetState::_Select;	
								SnapComponentLoc(ComponentBase->GetAllWallIdsInfo().Num());
								if (AddHouseComponent(screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength()))
								{
									SelectComponentEvent(ComponentBase->GetComponenetType());
									return true;
								}
							}
							break;
						case EComponenetType::_Pillar:
							{
								ComponentBase->SetComponentRoomID(ObjID);
								ComponentBase->ComponenetState = EComponenetState::_Select;
								SelectComponentEvent(ComponentBase->GetComponenetType());
								TArray<int32> PillarWalls = ComponentBase->GetAllWallIdsInfo();
								if (PillarWalls.Num() == 1)
								{
									if (PillarWalls[0] == INDEX_NONE)
									{
										PillarWalls.Empty();
										return AddHouseComponent(screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength());
									}
									else
									{
										SnapComponentLoc(ComponentBase->GetAllWallIdsInfo().Num());
										return AddHouseComponent(screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength());
									}
								}
								else if (PillarWalls.Num() == 2)
								{
									if (PillarWalls[0] == INDEX_NONE && PillarWalls[1] == INDEX_NONE)
									{
										PillarWalls.Empty();
										return AddHouseComponent(screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength());
									}
									else if (PillarWalls[0] == INDEX_NONE)
									{
										PillarWalls.RemoveAt(0);
										return AddHouseComponent(screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength());
									}
									else if (PillarWalls[1] == INDEX_NONE)
									{
										PillarWalls.RemoveAt(1);
										return AddHouseComponent(screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength());
									}
									else
									{
										return AddHouseComponent(screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength());
									}
								}
								else
								{
									SnapComponentLoc(0);
									return AddHouseComponent(screenPos, ComponentBase->GetWidth(), ComponentBase->GetLength());
								}
							}
							break;
						case EComponenetType::_Sewer:
							{
								ComponentBase->SetComponentRoomID(ObjID);
								ComponentBase->ComponenetState = EComponenetState::_Select;
								if (AddHouseComponent(screenPos, ComponentBase->GetWidth(), ComponentBase->GetWidth()))
								{
									SelectComponentEvent(ComponentBase->GetComponenetType());
									return true;
								}
							}
							break;
						 }
					}
				}
			}

		}
	}
	return false;
}

bool AComponentManagerActor::bAllowPlaceComp()
{
	if (BuildingSystem&&ComponentBaseWidget)
	{
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
		if (GameInst)
		{
			UWallBuildSystem* WallBuildSystem = GameInst->WallBuildSystem;
			if (WallBuildSystem)
			{
				WallBuildSystem->ClearAllCornnerUMG();
			}
		}
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			ComponentBase->SetDrawActive(false);
			ComponentBase->SetFrameColor(FLinearColor(1, 0.48, 0, 0));
			FVector2D screenPos = ComponentBase->GetLoc();
			//screenPos = GetMouse2D(screenPos);
			int id = -1;
			int32 ObjID = BuildingSystem->HitTest(screenPos);
			if (ObjID != INDEX_NONE)
			{
				UBuildingData* Data = BuildingSystem->GetData(ObjID);
				if (Data)
				{
					EObjectType Type = (EObjectType)Data->GetObjectType();
					if (Type == EObjectType::ERoom || Type == EObjectType::EArea || Type == EObjectType::EPolygonArea)
					{
						TArray<int32> Walls = ComponentBase->GetAllWallIdsInfo();
						float height = 280;
						if (Walls.Num() == 0 || (Walls.Num() == 1 && Walls[0] == INDEX_NONE))
						{
							for (int i = 0; i < roomVecDatas.Num(); ++i)
							{
								if (roomVecDatas[i].wallId != INDEX_NONE)
								{
									UBuildingData* WallData = BuildingSystem->GetData(roomVecDatas[i].wallId);
									height = WallData->GetFloat("Height");
									break;
								}
							}
						}
						else if (Walls.Num() == 1 && Walls[0] != INDEX_NONE)
						{
							UBuildingData* WallData = BuildingSystem->GetData(Walls[0]);
							height = WallData->GetFloat("Height");
						}
						else if (Walls.Num() == 2)
						{
							if (Walls[0] != INDEX_NONE)
							{
								UBuildingData* WallData = BuildingSystem->GetData(Walls[0]);
								height = WallData->GetFloat("Height");
							}
							else if (Walls[1] != INDEX_NONE)
							{
								UBuildingData* WallData = BuildingSystem->GetData(Walls[1]);
								height = WallData->GetFloat("Height");
							}

						}
						ComponentBase->SetHeight(height);

						EComponenetType type = ComponentBase->GetComponenetType();
						switch (type)
						{
						case EComponenetType::_FLUE:
						case EComponenetType::_BagRiser:
						{
							ComponentBase->SetComponentRoomID(ObjID);
							ComponentBase->ComponenetState = EComponenetState::_Select;
							SnapComponentLoc(ComponentBase->GetAllWallIdsInfo().Num());
							return true;
						}
						break;
						case EComponenetType::_Pillar:
						{
							ComponentBase->SetComponentRoomID(ObjID);
							ComponentBase->ComponenetState = EComponenetState::_Select;
							SelectComponentEvent(ComponentBase->GetComponenetType());
							TArray<int32> PillarWalls = ComponentBase->GetAllWallIdsInfo();
							if (PillarWalls.Num() == 1)
							{
								if (PillarWalls[0] == INDEX_NONE)
								{
									PillarWalls.Empty();
									return true;
								}
								else
								{
									SnapComponentLoc(ComponentBase->GetAllWallIdsInfo().Num());
									return true;
								}
							}
							else if (PillarWalls.Num() == 2)
							{
								if (PillarWalls[0] == INDEX_NONE && PillarWalls[1] == INDEX_NONE)
								{
									PillarWalls.Empty();
									return true;
								}
								else if (PillarWalls[0] == INDEX_NONE)
								{
									PillarWalls.RemoveAt(0);
									return true;
								}
								else if (PillarWalls[1] == INDEX_NONE)
								{
									PillarWalls.RemoveAt(1);
									return true;
								}
								else
								{
									return true;
								}
							}
							else
							{
								SnapComponentLoc(0);
								return true;
							}
						}
						break;
						case EComponenetType::_Sewer:
						{
							ComponentBase->SetComponentRoomID(ObjID);
							ComponentBase->ComponenetState = EComponenetState::_Select;
							return true;
						}
						break;
						}
					}
				}
			}

		}
	}
	return false;
}

void AComponentManagerActor::SnapComponentLoc(const int32& WallNum)
{
	UWorld* MyWorld = GetWorld();
	if (MyWorld&&ComponentBaseWidget)
	{
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
		if (GameInst)
		{
			UWallBuildSystem* WallBuildSystem = GameInst->WallBuildSystem;
			if (WallBuildSystem&&BuildingSystem)
			{
				UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
				if (ComponentBase)
				{
					FVector2D screenPos = ComponentBase->GetLoc();
					TArray<FVector2D> SnapLoc;
					BuildingSystem->GetAllCornerActorLoction(SnapLoc, -1);
					if (WallNum == 1)
					{
						bool bFindX = false;
						bool bFindY = false;
						float ComponentAngle = ComponentBase->GetAngleVal();
						if (FMath::Abs((FMath::Abs(ComponentAngle) - 90)) < 10)
						{
							bFindX = true;
						}
						else
							if (FMath::Abs(ComponentAngle) < 10|| FMath::Abs((FMath::Abs(ComponentAngle) - 180)) < 10)
							{
								bFindY = true;
							}
							else
							{
								bFindX = true;
								bFindY = true;
							}
						for (int i = 0; i < SnapLoc.Num(); ++i)
						{
							if (bFindX)
							{
								if (FMath::Abs(screenPos.X - SnapLoc[i].X) <= WallBuildSystem->ObjectSnapTol * 2)
								{
									FVector2D dir = FVector2D(0, 1);
									IObject* WallObj = BuildingSystem->GetObject(ComponentBase->GetAllWallIdsInfo()[0]);
									if (WallObj)
									{
										FVector2D forward = ToVector2D(WallObj->GetPropertyValue("Forward").Vec2Value());
										float length;
										float Angle = FVector2D::CrossProduct(forward, dir);
										if (Angle == 0)
										{
											length = 0;
										}
										else
											length = ((screenPos.X - SnapLoc[i].X)) / Angle;
										screenPos -= length * forward;
										ComponentBase->PrimitiveVertexDataGenerate(MyWorld, screenPos);
										ComponentBase->PosAtWall = FMath::ClosestPointOnSegment2D(screenPos, curSideData.startPos, curSideData.endPos);
										switch (ComponentBase->GetComponenetType())
										{
										case EComponenetType::_FLUE:
											BuildingSystem->UpdateFlueProperty(ComponentBase->GetComponentDRActorID(), screenPos, ComponentBase->GetAngleVal(), ComponentBase->GetWidth(), ComponentBase->GetLength());
											break;
										case EComponenetType::_BagRiser:
											BuildingSystem->UpdateTubeProperty(ComponentBase->GetComponentDRActorID(), screenPos, ComponentBase->GetAngleVal(), ComponentBase->GetWidth(), ComponentBase->GetLength());
											break;
										case EComponenetType::_Pillar:
											BuildingSystem->UpdatePillarProperty(ComponentBase->GetComponentDRActorID(), screenPos, ComponentBase->GetAngleVal(), ComponentBase->GetWidth(), ComponentBase->GetLength());
											break;
										}
									}
									break;
								}
							}
							if (bFindY)
							{
								if (FMath::Abs(screenPos.Y - SnapLoc[i].Y) <= WallBuildSystem->ObjectSnapTol * 2)
								{
									FVector2D dir = FVector2D(-1, 0);
									IObject* WallObj = BuildingSystem->GetObject(ComponentBase->GetAllWallIdsInfo()[0]);
									if (WallObj)
									{
										FVector2D forward = ToVector2D(WallObj->GetPropertyValue("Forward").Vec2Value());
										float length;
										float Angle = FVector2D::CrossProduct(forward, dir);
										if (Angle == 0)
										{
											length = 0;
										}
										else
											length = ((screenPos.Y - SnapLoc[i].Y)) / Angle;
										screenPos -= length * forward;
										ComponentBase->PrimitiveVertexDataGenerate(MyWorld, screenPos);
										ComponentBase->PosAtWall = FMath::ClosestPointOnSegment2D(screenPos, curSideData.startPos, curSideData.endPos);
										switch (ComponentBase->GetComponenetType())
										{
										case EComponenetType::_FLUE:
											BuildingSystem->UpdateFlueProperty(ComponentBase->GetComponentDRActorID(), screenPos, ComponentBase->GetAngleVal(), ComponentBase->GetWidth(), ComponentBase->GetLength());
											break;
										case EComponenetType::_BagRiser:
											BuildingSystem->UpdateTubeProperty(ComponentBase->GetComponentDRActorID(), screenPos, ComponentBase->GetAngleVal(), ComponentBase->GetWidth(), ComponentBase->GetLength());
											break;
										case EComponenetType::_Pillar:
											BuildingSystem->UpdatePillarProperty(ComponentBase->GetComponentDRActorID(), screenPos, ComponentBase->GetAngleVal(), ComponentBase->GetWidth(), ComponentBase->GetLength());
											break;
										}
									}
									break;
								}
							}
						}
					}
					else if (WallNum == 0)
					{
						bool bFindX = true;
						bool bFindY = true;
						for (int i = 0; i < SnapLoc.Num(); ++i)
						{
							if (bFindX)
							{
								if (FMath::Abs(screenPos.X - SnapLoc[i].X) <= WallBuildSystem->ObjectSnapTol * 2)
								{
									screenPos.X = SnapLoc[i].X;
									bFindX = false;
								}
							}
							if (bFindY)
							{
								if (FMath::Abs(screenPos.Y - SnapLoc[i].Y) <= WallBuildSystem->ObjectSnapTol * 2)
								{
									screenPos.Y = SnapLoc[i].Y;
									bFindY = false;
								}
							}
						}
						if (!(bFindX && bFindY))
						{
							ComponentBase->PrimitiveVertexDataGenerate(MyWorld, screenPos);
							ComponentBase->PosAtWall = FMath::ClosestPointOnSegment2D(screenPos, curSideData.startPos, curSideData.endPos);
							switch (ComponentBase->GetComponenetType())
							{
							case EComponenetType::_FLUE:
								BuildingSystem->UpdateFlueProperty(ComponentBase->GetComponentDRActorID(), screenPos, ComponentBase->GetAngleVal(), ComponentBase->GetWidth(), ComponentBase->GetLength());
								break;
							case EComponenetType::_BagRiser:
								BuildingSystem->UpdateTubeProperty(ComponentBase->GetComponentDRActorID(), screenPos, ComponentBase->GetAngleVal(), ComponentBase->GetWidth(), ComponentBase->GetLength());
								break;
							case EComponenetType::_Pillar:
								BuildingSystem->UpdatePillarProperty(ComponentBase->GetComponentDRActorID(), screenPos, ComponentBase->GetAngleVal(), ComponentBase->GetWidth(), ComponentBase->GetLength());
								break;
							}

						}
					}
				}
			}
		}
	}
}

void AComponentManagerActor::SetHousePluginDefMaterial(int32 ObjID, const FString &MaterialUri)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive * ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ObjID && ComponentBase)
		{
			for (int i = 0; i < 6; i++)
			{
				if (i != 1 && ComponentBase->GetComponenetType() != EComponenetType::_Pillar&&ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
					BuildingSystem->SetHousePluginSurface(ObjID, i, MaterialUri, 0);
			}
		}
	}
}

bool AComponentManagerActor::AddHouseComponent(const FVector2D &Location, float Width, float Length,  bool IsFirst)
{
	if (BuildingSystem)
	{
		UComponentBasePrimitive * ComponentBase = ComponentBaseWidget->GetComponentBase();
		int32 id = INDEX_NONE;
		if (ComponentBase)
		{
			float height = ComponentBase->GetHeight();
			float Angle = ComponentBase->GetAngleVal();
			switch (ComponentBase->GetComponenetType())
			{
			case EComponenetType::_FLUE:
				{
					id = BuildingSystem->AddFlue(Location, Width, Length, height);
					BuildingSystem->UpdateFlueProperty(id, Location, Angle);
					break;
				}
			case EComponenetType::_BagRiser:
				{
					id = BuildingSystem->AddTube(Location, Width, Length, height);
					BuildingSystem->UpdateTubeProperty(id, Location, Angle);
					break;
				}	
			case EComponenetType::_Pillar:
				{
					id = BuildingSystem->AddPillar(Location, Width, Length, height);
					BuildingSystem->UpdatePillarProperty(id, Location, Angle);				   
					break;
				}	
			case EComponenetType::_Sewer:
			{
				id = BuildingSystem->AddSewer(Location, Width);
				BuildingSystem->UpdateSewerProperty(id, Location, Width);
				break;
			}
			}
			if (id != INDEX_NONE)
			{
			
				ComponentBase->SetComponentDRActorID(id);
				UpdateComponentUUID(ComponentBase);
				if (IsFirst)
				{
					if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
					{
						SetHousePluginDefMaterial(id, "97191FAA4098DF2A660109A43EDF43B6");
					}
					else
					{
						ComponentBase->SetSewerName(TEXT("台盆/地漏下水口"));
						ComponentBase->SetWidth(5.0f);
						ComponentBase->SetMaterialInfo(0,"",0,0,0);
					}
				}
				return true;
			}
		}


	}
	return false;
}

void AComponentManagerActor::UpdateComponentUUID(UComponentBasePrimitive *base)
{
	if (base)
	{
		FString ComponentUUID = base->GetComponentUUID();
		if (ComponentUUID.Equals(""))
		{
			int32 Seconds;
			float PartialSeconds;
			UGameplayStatics::GetAccurateRealTime(this, Seconds, PartialSeconds);
			FString uuid = FString::Printf(TEXT("HousePlugin_%f"), Seconds + PartialSeconds);
			base->SetComponentUUID(uuid);
		}
	}
}

FString AComponentManagerActor::GetComponentUUID()
{
	FString uuid;
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive * ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			uuid = ComponentBase->GetComponentUUID();
		}
	}
	return uuid;
}

void AComponentManagerActor::GetComponentPrimitive2DPoints(TArray<FVector2D>& Points) const
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive * ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			FPrimitive2D Result = ComponentBase->GetPrimitive2DConst(ComponentBase->FramePrimitive, ComponentBase->GetComponenetInfo().AngleVal);
			Points.Add(Result.LeftTopPos);
			Points.Add(Result.RightTopPos);
			Points.Add(Result.RightBottomPos);
			Points.Add(Result.LeftBottomPos);
		}
	}
}

FBox2D AComponentManagerActor::GetComponentPrimitive2DBox() const
{
	TArray<FVector2D> Points;
	GetComponentPrimitive2DPoints(Points);

	FBox2D Ret(Points);

	return Ret;
}


bool AComponentManagerActor::IsHousePluginType()
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive * ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
			{
				return true;
			}
		}
	}
	return false;
}

void AComponentManagerActor::UpdateComponent(UWorld* MyWorld, int32 ObjectID, const FVector2D&Loc, float Width, float Length)
{
	if (!ComponentBaseWidget)
	{
		return;
	}
	UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (BuildingSystem&&ComponentBase)
	{
		if (ComponentBase->GetComponenetType()== EComponenetType::_Sewer)
		{
			ComponentBase->PrimitiveVertexDataGenerate(MyWorld, Loc);
			BuildingSystem->UpdateSewerProperty(ObjectID, Loc, ComponentBase->GetWidth());
			if (CompPositionWidget)
			{
				CompPositionWidget->ObjectID = ObjectID;
				CompPositionWidget->TargetPosition = ComponentBase->GetLoc();
				CompPositionWidget->Width = ComponentBase->GetWidth();
				CompPositionWidget->Length = ComponentBase->GetWidth();
				CompPositionWidget->TPoylgon = ComponentBase->GetTPoylgon();
				CompPositionWidget->Angle = Round(ComponentBase->GetAngleVal());
				CompPositionWidget->UpdateCompInfo();
			}
			return;
		}
		FVector2D CenterMouse2D;
		CenterMouse2D = ComponentBase->GetLoc();
		FVector2D newStartPos, newEndPos;
		///float Angelaaa = ComponentBase->GetAngleVal();
		////////////////
		float SegmentAngle = 0;
		GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
		if (CalculateSegmentAngle(preSideData, curSideData, SegmentAngle))
		{
			FVector2D dir = curSideData.endPos - curSideData.startPos;
			float dis = FVector2D::Distance(curSideData.endPos, curSideData.startPos);
			dir.Normalize();
			float tempAngle = 180 - SegmentAngle;
			newStartPos = curSideData.endPos - dir * ((dis - ComponentBase->GetLength() / 2.0f) - (ComponentBase->GetWidth() / FMath::Tan(tempAngle / 180 * PI)));

		}
		if (CalculateSegmentAngle(curSideData, nextSideData, SegmentAngle))
		{
			FVector2D dir = curSideData.endPos - curSideData.startPos;
			float dis = FVector2D::Distance(curSideData.endPos, curSideData.startPos);
			dir.Normalize();
			float tempAngle = 180 - SegmentAngle;
			newEndPos = curSideData.startPos + dir * ((dis - ComponentBase->GetLength() / 2.0f) - (ComponentBase->GetWidth() / FMath::Tan(tempAngle / 180 * PI)));
		}


		//////////////////







		FVector2D MouseAtWall = FMath::ClosestPointOnSegment2D(Loc, newStartPos, newEndPos);
		FVector2D CenterAtWall = FMath::ClosestPointOnSegment2D(CenterMouse2D, newStartPos, newEndPos);
		FVector2D dir = MouseAtWall - CenterAtWall;
		dir.Normalize();
		FVector2D sidedir = curSideData.endPos - curSideData.startPos;
		sidedir.Normalize();
		FVector2D Temp = newEndPos - newStartPos;
		Temp.Normalize();
		FVector2D centerDir = Temp.GetRotated(90);
		centerDir.Normalize();

		if (b && (dir | sidedir) > 0)
		{

			FVector2D newNextStartPos, newNextEndPos;
			GetNewPos(ComponentBase, nextSideData, newNextStartPos, newNextEndPos);
			float dis = FMath::PointDistToSegment(FVector(MouseAtWall, 0), FVector(newNextStartPos, 0), FVector(newNextEndPos, 0));

			if (dis <= AdsorptionRange && nextSideData.wallId != INDEX_NONE)
			{
				if (curSideData.IsWidth)
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
				else
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
			}
			else
			{
				if (curSideData.IsWidth)
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
				else
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
			}
			ComponentBase->SetAngleVal(curSideData.AngleVal);
			ComponentBase->SetIsWidth(curSideData.IsWidth);
			if (ComponentBase->GetComponenetType() == EComponenetType::_Pillar)
			{
				float mouseDis = FMath::PointDistToSegment(FVector(Loc, 0), FVector(newStartPos, 0), FVector(newEndPos, 0));
				if (mouseDis > ComponentBase->GetLength() + ComponentBase->GetWidth())
				{
					CenterMouse2D = Loc;
					ComponentBase->RemoveAll();
					ComponentBase->SetAngleVal(0);
				}
			}

		}
		else if (b && (dir | sidedir) < 0)
		{

			FVector2D newPreStartPos, newPreEndPos;
			GetNewPos(ComponentBase, preSideData, newPreStartPos, newPreEndPos);
			float dis = FMath::PointDistToSegment(FVector(MouseAtWall, 0), FVector(newPreStartPos, 0), FVector(newPreEndPos, 0));
			if (dis <= AdsorptionRange && preSideData.wallId != INDEX_NONE)
			{
				if (curSideData.IsWidth)
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
				else
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
			}
			else
			{
				if (curSideData.IsWidth)
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
				else
					CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
			}
			ComponentBase->SetAngleVal(curSideData.AngleVal);
			ComponentBase->SetIsWidth(curSideData.IsWidth);
			if (ComponentBase->GetComponenetType() == EComponenetType::_Pillar)
			{
				float mouseDis = FMath::PointDistToSegment(FVector(Loc, 0), FVector(newStartPos, 0), FVector(newEndPos, 0));
				if (mouseDis > ComponentBase->GetLength() + ComponentBase->GetWidth())
				{
					CenterMouse2D = Loc;
					ComponentBase->RemoveAll();
					//ComponentBase->SetAngleVal(0);
				}
			}
		}
		else
		{
			float disStart = FVector2D::Distance(CenterMouse2D, curSideData.startPos);
			float disEnd = FVector2D::Distance(CenterMouse2D, curSideData.endPos);
			float d = FMath::Sqrt((ComponentBase->GetLength() / 2.0f)*(ComponentBase->GetLength() / 2.0f) +
				(ComponentBase->GetWidth() / 2.0f)*(ComponentBase->GetWidth() / 2.0f));
			if (ComponentBase->GetComponenetType() == EComponenetType::_Pillar)
			{
				float mouseDis = FMath::PointDistToSegment(FVector(Loc, 0), FVector(newStartPos, 0), FVector(newEndPos, 0));
				if (mouseDis > ComponentBase->GetLength() + 5)
				{
					//ComponentBase->SetAngleVal(0);
					ComponentBase->RemoveAll();
				}
			}
			else
				ComponentBase->RenewSetWall(curSideData.wallId);
			if (disStart - d <= AdsorptionRange)
			{
				FVector2D mouseDir = Loc - curSideData.startPos;
				FVector2D preSideDir = preSideData.startPos - preSideData.endPos;
				FVector2D curSideDir = curSideData.endPos - curSideData.startPos;
				float preSideLen = (mouseDir | preSideDir) / preSideDir.Size();
				float curSideLen = (mouseDir | curSideDir) / curSideDir.Size();
				if (preSideLen > d + AdsorptionRange && preSideData.wallId != INDEX_NONE)
				{
					ComponentBase->DestroyWall();
					b = true;
					curSideData = preSideData;
					int index = FindRoomVecData(curSideData);
					if (index != INDEX_NONE)
					{
						if (index == 0)
						{
							preSideData = roomVecDatas[roomVecDatas.Num() - 1];
							nextSideData = roomVecDatas[1];
						}
						else if (index == roomVecDatas.Num() - 1)
						{
							preSideData = roomVecDatas[index - 1];
							nextSideData = roomVecDatas[0];
						}
						else
						{
							preSideData = roomVecDatas[index - 1];
							nextSideData = roomVecDatas[index + 1];
						}
						ComponentBase->SetAngleVal(curSideData.AngleVal);
						ComponentBase->AddWallInfo(curSideData.wallId);
						FVector2D newStartPos, newEndPos;
						GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
						MouseAtWall = FMath::ClosestPointOnSegment2D(Loc, newStartPos, newEndPos);
						FVector2D Temp = newEndPos - newStartPos;
						Temp.Normalize();
						centerDir = Temp.GetRotated(90);
						centerDir.Normalize();
						if (curSideData.IsWidth)
							CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
						else
							CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
						ComponentBase->SetIsWidth(curSideData.IsWidth);
					}
				}
				else if (curSideLen > d + AdsorptionRange)
				{

					ComponentBase->DestroyWall();
					ComponentBase->SetAngleVal(curSideData.AngleVal);
					b = true;
					FVector2D newStartPos, newEndPos;
					GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
					FVector2D Temp = newEndPos - newStartPos;
					Temp.Normalize();
					centerDir = Temp.GetRotated(90);
					centerDir.Normalize();
					if (curSideData.IsWidth)
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
					else
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
					ComponentBase->SetIsWidth(curSideData.IsWidth);
				}
				else
				{
					b = false;
					FPrimitive2D outPri = GetWorldPosBySrceen(ComponentBase->FramePrimitive);
					float leftTopLen, rightTopLen, rightBottomLen, leftBottomLen;
					leftTopLen = FVector2D::Distance(outPri.LeftTopPos, curSideData.startPos);
					rightTopLen = FVector2D::Distance(outPri.RightTopPos, curSideData.startPos);
					rightBottomLen = FVector2D::Distance(outPri.RightBottomPos, curSideData.startPos);
					leftBottomLen = FVector2D::Distance(outPri.LeftBottomPos, curSideData.startPos);
					float AngleVal1 = 0;
					if (leftTopLen < rightTopLen&&leftTopLen < rightBottomLen&&leftTopLen < leftBottomLen)
					{
						AngleVal1 = 180;
					}
					else if (rightTopLen < leftTopLen&&rightTopLen < rightBottomLen&&rightTopLen < leftBottomLen)
					{
						AngleVal1 = -90;
					}
					else if (leftBottomLen < rightBottomLen&&leftBottomLen < rightTopLen&&leftBottomLen < leftTopLen)
					{
						AngleVal1 = 90;
					}
					else
					{
						AngleVal1 = 0.0f;
					}

					float L = ComponentBase->GetLength();
					float W = ComponentBase->GetWidth();
					if (AngleVal1 == 180.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.startPos.X - W / 2.0f, curSideData.startPos.Y + L / 2.0f));

					}
					else if (AngleVal1 == -90.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.startPos.X - L / 2.0f, curSideData.startPos.Y - W / 2.0f));

					}
					else if (AngleVal1 == 0.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.startPos.X + W / 2.0f, curSideData.startPos.Y - L / 2.0f));
					}
					else if (AngleVal1 == 90.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.startPos.X + L / 2.0f, curSideData.startPos.Y + W / 2.0f));
					}
					ComponentBase->SetAngleVal(AngleVal1);
					ComponentBase->AddWallInfo(preSideData.wallId);

					ComponentBase->rightBottomLoc = curSideData.startPos;
				}
			}
			else if (disEnd - d <= AdsorptionRange && nextSideData.wallId != INDEX_NONE)
			{
				FVector2D mouseDir = Loc - curSideData.endPos;
				FVector2D nextSideDir = nextSideData.endPos - nextSideData.startPos;
				FVector2D curSideDir = curSideData.startPos - curSideData.endPos;
				float nextSideLen = (mouseDir | nextSideDir) / nextSideDir.Size();
				float curSideLen = (mouseDir | curSideDir) / curSideDir.Size();
				if (nextSideLen > d + AdsorptionRange)
				{

					ComponentBase->DestroyWall();
					b = true;
					curSideData = nextSideData;
					int index = FindRoomVecData(curSideData);
					if (index != INDEX_NONE)
					{
						if (index == 0)
						{
							preSideData = roomVecDatas[roomVecDatas.Num() - 1];
							nextSideData = roomVecDatas[1];
						}
						else if (index == roomVecDatas.Num() - 1)
						{
							preSideData = roomVecDatas[index - 1];
							nextSideData = roomVecDatas[0];
						}
						else
						{
							preSideData = roomVecDatas[index - 1];
							nextSideData = roomVecDatas[index + 1];
						}
					}
					ComponentBase->SetAngleVal(curSideData.AngleVal);
					ComponentBase->AddWallInfo(curSideData.wallId);
					FVector2D newStartPos, newEndPos;
					GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
					MouseAtWall = FMath::ClosestPointOnSegment2D(Loc, newStartPos, newEndPos);
					FVector2D Temp = newEndPos - newStartPos;
					Temp.Normalize();
					centerDir = Temp.GetRotated(90);
					centerDir.Normalize();
					if (curSideData.IsWidth)
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
					else
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
					ComponentBase->SetIsWidth(curSideData.IsWidth);
				}
				else if (curSideLen > d + AdsorptionRange)
				{

					ComponentBase->DestroyWall();
					b = true;
					ComponentBase->SetAngleVal(curSideData.AngleVal);
					FVector2D newStartPos, newEndPos;
					GetNewPos(ComponentBase, curSideData, newStartPos, newEndPos);
					FVector2D Temp = newEndPos - newStartPos;
					Temp.Normalize();
					centerDir = Temp.GetRotated(90);
					centerDir.Normalize();
					if (curSideData.IsWidth)
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetLength() / 2.0f);
					else
						CenterMouse2D = MouseAtWall + centerDir * (ComponentBase->GetWidth() / 2.0f);
					ComponentBase->SetIsWidth(curSideData.IsWidth);
				}
				else
				{
					b = false;
					FPrimitive2D outPri = GetWorldPosBySrceen(ComponentBase->FramePrimitive);
					float leftTopLen, rightTopLen, rightBottomLen, leftBottomLen;
					leftTopLen = FVector2D::Distance(outPri.LeftTopPos, curSideData.endPos);
					rightTopLen = FVector2D::Distance(outPri.RightTopPos, curSideData.endPos);
					rightBottomLen = FVector2D::Distance(outPri.RightBottomPos, curSideData.endPos);
					leftBottomLen = FVector2D::Distance(outPri.LeftBottomPos, curSideData.endPos);
					float AngleVal1 = 0;
					if (leftTopLen < rightTopLen&&leftTopLen < rightBottomLen&&leftTopLen < leftBottomLen)
					{
						AngleVal1 = 180;
					}
					else if (rightTopLen < leftTopLen&&rightTopLen < rightBottomLen&&rightTopLen < leftBottomLen)
					{
						AngleVal1 = -90;
					}
					else if (leftBottomLen < rightBottomLen&&leftBottomLen < rightTopLen&&leftBottomLen < leftTopLen)
					{
						AngleVal1 = 90;
					}
					else
					{
						AngleVal1 = 0.0f;
					}

					float L = ComponentBase->GetLength();
					float W = ComponentBase->GetWidth();
					if (AngleVal1 == 180.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.endPos.X - W / 2.0f, curSideData.endPos.Y + L / 2.0f));

					}
					else if (AngleVal1 == -90.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.endPos.X - L / 2.0f, curSideData.endPos.Y - W / 2.0f));

					}
					else if (AngleVal1 == 0.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.endPos.X + W / 2.0f, curSideData.endPos.Y - L / 2.0f));
					}
					else if (AngleVal1 == 90.0f)
					{
						CenterMouse2D = (FVector2D(curSideData.endPos.X + L / 2.0f, curSideData.endPos.Y + W / 2.0f));
					}
					ComponentBase->SetAngleVal(AngleVal1);
					ComponentBase->rightBottomLoc = curSideData.endPos;
					ComponentBase->AddWallInfo(nextSideData.wallId);

				}
			}
		}

		if (ComponentBase->GetComponenetType() == EComponenetType::_Pillar)
		{
			float mouseDis = FMath::PointDistToSegment(FVector(Loc, 0), FVector(newStartPos, 0), FVector(newEndPos, 0));
			if (mouseDis > ComponentBase->GetLength() + ComponentBase->GetWidth())
			{
				CenterMouse2D = Loc;
				ComponentBase->RemoveAll();
				ComponentBase->SetAngleVal(0);
			}

			if (ComponentBase->GetAllWallIdsInfo().Num() == 0)
			{
				CenterMouse2D = Loc;
				int AreaID = BuildingSystem->HitTest(CenterMouse2D);
				if (AreaID != INDEX_NONE && AreaID != ComponentBase->GetComponentRoomID())
				{
					UBuildingData* Areadata = BuildingSystem->GetData(AreaID);
					if (Areadata)
					{
						EObjectType type = (EObjectType)Areadata->GetObjectType();
						if (type == EObjectType::ERoom /*|| type == EObjectType::EArea || type == EObjectType::EPolygonArea*/)
						{
							ComponentBase->SetComponentRoomID(AreaID);
							UpdateArea(AreaID);
						}
					}
				}
				if (roomVecDatas.Num() > 2)
				{
					float d = -1;
					d = FMath::Sqrt((ComponentBase->GetWidth()*ComponentBase->GetWidth() + ComponentBase->GetLength()*ComponentBase->GetLength()) / 4);
					for (int32 i = 0; i < roomVecDatas.Num(); ++i)
					{
						float dis = roomVecDatas[i] - Loc;
						if (dis - d <= AdsorptionRange)
						{
							curSideData = roomVecDatas[i];
							if (i == 0)
							{
								preSideData = roomVecDatas[roomVecDatas.Num() - 1];
								nextSideData = roomVecDatas[1];
							}
							else if (i == roomVecDatas.Num() - 1)
							{
								preSideData = roomVecDatas[i - 1];
								nextSideData = roomVecDatas[0];
							}
							else
							{
								preSideData = roomVecDatas[i - 1];
								nextSideData = roomVecDatas[i + 1];
							}
							b = true;
							MovePlugin(MyWorld, Loc, ComponentBase->GetWidth(), ComponentBase->GetLength());
							break;
						}
					}

				}
			}
			if (curSideData.wallId == INDEX_NONE)
			{
				CenterMouse2D = Loc;
				ComponentBase->SetAngleVal(0);
				ComponentBase->RemoveAll();
			}
		}
		ComponentBase->PrimitiveVertexDataGenerate(MyWorld, CenterMouse2D);
		ComponentBase->PosAtWall = FMath::ClosestPointOnSegment2D(CenterMouse2D, curSideData.startPos, curSideData.endPos);
		ComponentBase->NodeForWard = centerDir;
		TArray<int32> WallID = ComponentBase->GetAllWallIdsInfo();
		//UE_LOG(LogTemp, Warning, TEXT("WALL is num=%d"), WallID.Num());
		switch (ComponentBase->GetComponenetType())
		{
		case EComponenetType::_FLUE:
			BuildingSystem->UpdateFlueProperty(ObjectID, CenterMouse2D, ComponentBase->GetAngleVal(), Width, Length);
			break;
		case EComponenetType::_BagRiser:
			BuildingSystem->UpdateTubeProperty(ObjectID, CenterMouse2D, ComponentBase->GetAngleVal(), Width, Length);
			break;
		case EComponenetType::_Pillar:
			BuildingSystem->UpdatePillarProperty(ObjectID, CenterMouse2D, ComponentBase->GetAngleVal(), Width, Length);
			break;
		case EComponenetType::_Sewer:
			BuildingSystem->UpdateSewerProperty(ObjectID, Loc, ComponentBase->GetWidth());
			break;
		}

		if (CompPositionWidget)
		{
			CompPositionWidget->ObjectID = ObjectID;
			CompPositionWidget->TargetPosition = ComponentBase->GetLoc();
			CompPositionWidget->Width = ComponentBase->GetWidth();
			CompPositionWidget->Length = ComponentBase->GetLength();
			CompPositionWidget->TPoylgon = ComponentBase->GetTPoylgon();
			CompPositionWidget->Angle = Round(ComponentBase->GetAngleVal());
			CompPositionWidget->UpdateCompInfo();
		}
	}

}

void AComponentManagerActor::ChangeName(const FString& Name)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *BaseComponent = ComponentBaseWidget->GetComponentBase();
		if (BaseComponent)
		{
			BaseComponent->SetSewerName(Name);
		}
	}
}

void AComponentManagerActor::ChangeSize(float Val)
{
	UWorld* MyWorld = GetWorld();
	if (MyWorld)
	{
		if (ComponentBaseWidget)
		{
			UComponentBasePrimitive *BaseComponent = ComponentBaseWidget->GetComponentBase();
			if (BaseComponent)
			{
				if (Val != BaseComponent->GetWidth())
				{
					BaseComponent->SetWidth(Val);
					BaseComponent->PrimitiveVertexDataGenerate(MyWorld, BaseComponent->GetLoc());
					BuildingSystem->UpdateSewerProperty(BaseComponent->GetComponentDRActorID(), BaseComponent->GetLoc(), Val);
				}

				if (CompPositionWidget)
				{
					CompPositionWidget->Width = BaseComponent->GetWidth();
					CompPositionWidget->Length = BaseComponent->GetComponenetType() != EComponenetType::_Sewer ? BaseComponent->GetLength() : BaseComponent->GetWidth();
				}
			}
		}
	}
}

void AComponentManagerActor::ChangleWidth(float Val)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *BaseComponent = ComponentBaseWidget->GetComponentBase();
		if (BaseComponent)
		{
			FVector2D CenterMouse2D;
			float AngleVal1 = BaseComponent->GetAngleVal();
			BaseComponent->SetWidth(Val);
			float L = BaseComponent->GetLength();
		    int32 WallNum = BaseComponent->GetAllWallIdsInfo().Num();
			if (WallNum == 1)
			{
				//FVector2D pos = BaseComponent->GetLoc();
				//UpdateComponent(GetWorld(), BaseComponent->GetComponentDRActorID(), pos, Val, L);

				if (BaseComponent->GetIsWidth())
				{
					CenterMouse2D = BaseComponent->PosAtWall + BaseComponent->NodeForWard * L / 2.0f;
				}
				else
				{
					CenterMouse2D = BaseComponent->PosAtWall + BaseComponent->NodeForWard * Val / 2.0f;
				}
			}
			else if(WallNum == 2)
			{
				if (AngleVal1 == 180.0f)
				{
					CenterMouse2D = (FVector2D(BaseComponent->rightBottomLoc.X - Val / 2.0f, BaseComponent->rightBottomLoc.Y + L / 2.0f));

				}
				else if (AngleVal1 == -90.0f)
				{
					CenterMouse2D = (FVector2D(BaseComponent->rightBottomLoc.X - L / 2.0f, BaseComponent->rightBottomLoc.Y - Val / 2.0f));

				}
				else if (AngleVal1 == 0.0f)
				{
					CenterMouse2D = (FVector2D(BaseComponent->rightBottomLoc.X + Val / 2.0f, BaseComponent->rightBottomLoc.Y - L / 2.0f));
				}
				else if (AngleVal1 == 90.0f)
				{
					CenterMouse2D = (FVector2D(BaseComponent->rightBottomLoc.X + L / 2.0f, BaseComponent->rightBottomLoc.Y + Val / 2.0f));
				}
				//BaseComponent->PosAtWall = FMath::ClosestPointOnSegment2D(CenterMouse2D, curSideData.startPos, curSideData.endPos);
			}
			else
			{
				CenterMouse2D = BaseComponent->GetLoc();
			}
			//FirstUpdate();
			BaseComponent->PrimitiveVertexDataGenerate(GetWorld(), CenterMouse2D);
			switch (BaseComponent->GetComponenetType())
			{
			case EComponenetType::_FLUE:
				BuildingSystem->UpdateFlueProperty(BaseComponent->GetComponentDRActorID(), CenterMouse2D, AngleVal1, Val, L);
				break;
			case EComponenetType::_BagRiser:
				BuildingSystem->UpdateTubeProperty(BaseComponent->GetComponentDRActorID(), CenterMouse2D, AngleVal1, Val, L);
				break;
			case EComponenetType::_Pillar:
				BuildingSystem->UpdatePillarProperty(BaseComponent->GetComponentDRActorID(), CenterMouse2D, AngleVal1, Val, L);
				break;
			}
			if (CompPositionWidget)
			{
				CompPositionWidget->Length = BaseComponent->GetLength();
				CompPositionWidget->Width = BaseComponent->GetWidth();
			}
		}
	}
}

void AComponentManagerActor::ChangleLength(float Val)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *BaseComponent = ComponentBaseWidget->GetComponentBase();
		if (BaseComponent)
		{
			BaseComponent->SetLength(Val);
			float AngleVal1 = BaseComponent->GetAngleVal();
			float W = BaseComponent->GetWidth();
			int32 WallNum = BaseComponent->GetAllWallIdsInfo().Num();
			FVector2D CenterMouse2D;
			if (WallNum == 1)
			{
				if (!BaseComponent->GetIsWidth())
				{

					CenterMouse2D = BaseComponent->PosAtWall + BaseComponent->NodeForWard * W / 2.0f;
				}
				else
				{
					//CenterMouse2D = BaseComponent->PosAtWall + BaseComponent->NodeForWard * W / 2.0f;
					CenterMouse2D = BaseComponent->PosAtWall + BaseComponent->NodeForWard * Val / 2.0f;
				}
			}
			else if (WallNum == 2)
			{
				if (AngleVal1 == 180.0f)
				{
					CenterMouse2D = (FVector2D(BaseComponent->rightBottomLoc.X - W / 2.0f, BaseComponent->rightBottomLoc.Y + Val / 2.0f));

				}
				else if (AngleVal1 == -90.0f)
				{
					CenterMouse2D = (FVector2D(BaseComponent->rightBottomLoc.X - Val / 2.0f, BaseComponent->rightBottomLoc.Y - W / 2.0f));

				}
				else if (AngleVal1 == 0.0f)
				{
					CenterMouse2D = (FVector2D(BaseComponent->rightBottomLoc.X + W / 2.0f, BaseComponent->rightBottomLoc.Y - Val / 2.0f));
				}
				else if (AngleVal1 == 90.0f)
				{
					CenterMouse2D = (FVector2D(BaseComponent->rightBottomLoc.X + Val / 2.0f, BaseComponent->rightBottomLoc.Y + W / 2.0f));
				}
				//BaseComponent->PosAtWall = FMath::ClosestPointOnSegment2D(CenterMouse2D, curSideData.startPos, curSideData.endPos);
			}
			else
			{
				CenterMouse2D = BaseComponent->GetLoc();
			}
			//FirstUpdate();
			BaseComponent->PrimitiveVertexDataGenerate(GetWorld(), CenterMouse2D);
			switch (BaseComponent->GetComponenetType())
			{
			case EComponenetType::_FLUE:
				BuildingSystem->UpdateFlueProperty(BaseComponent->GetComponentDRActorID(), CenterMouse2D, AngleVal1, W, Val);
				break;
			case EComponenetType::_BagRiser:
				BuildingSystem->UpdateTubeProperty(BaseComponent->GetComponentDRActorID(), CenterMouse2D, AngleVal1, W, Val);
				break;
			case EComponenetType::_Pillar:
				BuildingSystem->UpdatePillarProperty(BaseComponent->GetComponentDRActorID(), CenterMouse2D, AngleVal1, W, Val);
				break;
			}
			if (CompPositionWidget)
			{
				CompPositionWidget->Length = BaseComponent->GetLength();
				CompPositionWidget->Width = BaseComponent->GetWidth();
			}
		}
	}
}

void AComponentManagerActor::HiddenPrimitiveWidget(bool b)
{
	if (ComponentBaseWidget)
	{
		if (b)
		{
			ComponentBaseWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			ComponentBaseWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

	}
}

void AComponentManagerActor::RevertComponenet(const FComponenetInfo& Info)
{
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		if (!ComponentBaseWidget)
		{
			ComponentBaseWidget = Cast<UComponentBaseWidget>(CreateWidget(MyWorld, UComponentBaseWidget::StaticClass()));
			if (ComponentBaseWidget)
			{
				ComponentBaseWidget->AddToViewport(-10);
				ComponentBaseWidget->CreateComponentPrimitive(Info.ComponenetType);

				UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
				if (ComponentBase)
				{
					ComponentBase->UpdataComponenetInfo(Info);
					if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
					{
						//ComponentBase->RemoveAll();
						TArray<FVector2D> TPoylgon = ComponentBase->GetTPoylgon();
						roomVecDatas.Empty();
						for (int i = 0; i < TPoylgon.Num(); i++)
						{
							FRoomVecData vec;
							if (i == TPoylgon.Num() - 1)
							{
								//vec.startPos = TPoylgon[i];
								//vec.endPos = TPoylgon[0];
								vec.InitData(this, TPoylgon[i], TPoylgon[0]);
							}
							else
							{
								//vec.startPos = TPoylgon[i];
								//vec.endPos = TPoylgon[i + 1];
								vec.InitData(this, TPoylgon[i], TPoylgon[i + 1]);
							}
							roomVecDatas.Add(vec);
						}
						if (roomVecDatas.Num() >= 3)
						{
							for (int i = 0; i < roomVecDatas.Num(); i++)
							{
								float dis = roomVecDatas[i] - Info.Loc;
								dis = dis * 2.0f;
								if ((dis - ComponentBase->GetWidth() <= 5.0f) || (dis - ComponentBase->GetLength() <= 5.0f))
								{

									curSideData = roomVecDatas[i];
									//ComponentBase->AddWallInfo(curSideData.wallId);
									if (i == 0)
									{
										preSideData = roomVecDatas[roomVecDatas.Num() - 1];
										nextSideData = roomVecDatas[1];
									}
									else if (i == roomVecDatas.Num() - 1)
									{
										preSideData = roomVecDatas[i - 1];
										nextSideData = roomVecDatas[0];
									}
									else
									{
										preSideData = roomVecDatas[i - 1];
										nextSideData = roomVecDatas[i + 1];
									}
									//break;
								}
							}
						}
					}
					ComponentBase->PrimitiveVertexDataGenerate(MyWorld, Info.Loc);
					ComponentBase->ComponenetState = EComponenetState::_Default;
					AddHouseComponent(Info.Loc, Info.Width, Info.Length, false);
					ComponentBase->SetFrameColor(FLinearColor(1, 0.48, 0, 0));
					//TMap<int32, FString> MaterialInfo = ComponentBase->GetMaterialInfo();
					ComponentBaseWidget->SetComponentManger(this);
				}

			}
		}
	}

}


bool AComponentManagerActor::GetAllHouseComponent(TArray<FVector2D>& Nodes)
{
	if (BuildingSystem&&ComponentBaseWidget)
	{
		kVector3D BorderArray[4];
		UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
			{
				BuildingSystem->GetHousePluginBorder(ComponentBase->GetComponentDRActorID(), BorderArray);
				Nodes.Empty();
				for (int i = 0; i < 4; ++i)
				{
					Nodes.Add(FVector2D(BorderArray[i].x, BorderArray[i].y));
				}
				return true;
			}
		}
		
	}
	return false;
}

bool AComponentManagerActor::GetAllPillars(TArray<FVector2D>& Nodes)
{
	if (BuildingSystem&&ComponentBaseWidget)
	{
		kVector3D BorderArray[4];
		UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
			{
				BuildingSystem->GetHousePluginBorder(ComponentBase->GetComponentDRActorID(), BorderArray);
				Nodes.Empty();
				for (int i = 0; i < 4; ++i)
				{
					Nodes.Add(FVector2D(BorderArray[i].x, BorderArray[i].y));
				}
				return true;
			}
		}
		
	}
	return false;
}


bool AComponentManagerActor::GetHouseComponent(int32 RoomID, TArray<FVector2D>& Nodes)
{

	return false;
}

FPrimitive2D AComponentManagerActor::GetPrimitive2DWithAngle(const FPrimitive2D&  inPrimitive, float Angle /*= 0.0f*/)
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


FPrimitive2D AComponentManagerActor::GetWorldPosBySrceen(FPrimitive2D PosNodes)
{
	FVector2D  LeftBottomPosWorld = GetMouse2D(PosNodes.LeftBottomPos);
	FVector2D  LeftTopPosWorld = GetMouse2D(PosNodes.LeftTopPos);
	FVector2D  RightBottomPosWorld = GetMouse2D(PosNodes.RightBottomPos);
	FVector2D  RightTopPosWorld = GetMouse2D(PosNodes.RightTopPos);
	return FPrimitive2D(LeftTopPosWorld, RightTopPosWorld, RightBottomPosWorld, LeftBottomPosWorld);
}
FVector2D AComponentManagerActor::WorldLocationToScreen(UWorld* MyWorld, const FVector2D& PrimitiveNodes)
{
	FVector2D viewPos;
	if (MyWorld)
	{
		MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(PrimitiveNodes, 280), viewPos);
	}
	return viewPos;
}

void AComponentManagerActor::Update()
{
	if (ComponentBaseWidget&&BuildingSystem)
	{
		UWorld *MyWorld = GetWorld();
		UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (MyWorld&&ComponentBase)
		{
			FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
			FVector2D screenPos = GetMouse2D(mousePos);
			ComponentBase->ViewAngleVal = CameraAngleVal;
			if (ComponentBase->ComponenetState == EComponenetState::_Add)
			{
				NewComponent(MyWorld, screenPos);
			}
			else if (ComponentBase->ComponenetState == EComponenetState::_Default)
			{
				//int32 id = ComponentBase->GetComponentDRActorID();
				//if (id > 0)
				//{
				//						
				//	//TArray<float> PluginInfo = BuildingSystem->GetPluginTrans(id);
				//	float PluginInfo[5];
				//	float* PlugPt = PluginInfo;

				//	int InfoNum = BuildingSystem->GetPluginTrans(id, PlugPt);

				//	FVector2D pos(PluginInfo[3], PluginInfo[4]);
				//	//UE_LOG(LogTemp, Warning, TEXT("Original Loc:X=%f  Y=%f\n"), PluginInfo[3], PluginInfo[4]);
				//	if (PluginInfo[3] == 0 || PluginInfo[4] == 0)
				//	{
						ComponentBase->PrimitiveVertexDataGenerate(MyWorld, ComponentBase->GetComponenetInfo().Loc);
				//		return;
				//	}
				//	ComponentBase->PrimitiveVertexDataGenerate(MyWorld, pos);					
				//}
			}
			else if (ComponentBase->ComponenetState == EComponenetState::_Select)
			{

				int32 id = ComponentBase->GetComponentDRActorID();
				if (id > 0)
				{

					if (bLeftMouseActive)
					{
						FVector2D Loc = offset + screenPos;
						UpdateComponent(MyWorld, id, Loc);
					}
					else
					{
						//TArray<float> PluginInfo = BuildingSystem->GetPluginTrans(id);
					/*	float PluginInfo[5];
						float* PlugPt = PluginInfo;

						int InfoNum = BuildingSystem->GetPluginTrans(id, PlugPt);
						FVector2D pos(PluginInfo[3], PluginInfo[4]);
						if (abs(PluginInfo[3] - 14.0) < 1.0)
						{
							float Lx, Ly;
							Lx = PluginInfo[3];
							Ly = PluginInfo[4];
						}
						if (PluginInfo[3] == 0 || PluginInfo[4] == 0)
						{
							ComponentBase->PrimitiveVertexDataGenerate(MyWorld, ComponentBase->GetComponenetInfo().Loc);
							return;
						}
						ComponentBase->PrimitiveVertexDataGenerate(MyWorld, pos);*/
						ComponentBase->PrimitiveVertexDataGenerate(MyWorld, ComponentBase->GetComponenetInfo().Loc);
					}
				}

			}
		}

	}
}
bool AComponentManagerActor::getDataForLayout(FComponentData &data)
{
	if (ComponentBaseWidget) {
	data.componenttype= ComponentBaseWidget->GetComponentBase()->GetComponenetType();
	GetAllPillars(data.fourpoint);
	return true;
	}
	return false;
}
void AComponentManagerActor::ClearSelect()
{
	UWorld *MyWorld = GetWorld();
	FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (MyWorld&&ComponentBase)
		{
			EComponenetType type = ComponentBase->GetComponenetType();
			FPrimitive2D outPrimitive = GetPrimitive2DWithAngle(ComponentBase->FramePrimitive, ComponentBase->GetAngleVal());
			if (!DetectHitPrimitive2D(outPrimitive, mousePos))
			{
				if (ComponentBase->ComponenetState == EComponenetState::_Select)
				{
					ComponentBase->ComponenetState = EComponenetState::_Default;
					bSelect = false;
					CanCelComponentEvent(type);

				}
			}
		}
	}
}
void AComponentManagerActor::LeftMouseDown()
{
	//ClearSelect();
	/*TArray<AActor*> OutActor;
	UGameplayStatics::GetAllActorsOfClass(this, AComponentManagerActor::StaticClass(), OutActor);
	bool bOtherHousePluginSelect = false;
	for (int i = 0; i < OutActor.Num(); ++i)
	{
		AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(OutActor[i]);
		if (ComponentManager->bSelect)
		{
			UWorld *MyWorld = GetWorld();
			FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
			UComponentBasePrimitive* ComponentBase = ComponentManager->ComponentBaseWidget->GetComponentBase();
			if (MyWorld&&ComponentBase)
			{
				EComponenetType type = ComponentBase->GetComponenetType();
				FPrimitive2D outPrimitive = GetPrimitive2DWithAngle(ComponentBase->FramePrimitive, ComponentBase->GetAngleVal());
				if (DetectHitPrimitive2D(outPrimitive, mousePos))
				{
					ComponentManager->bSelect = true;

					ComponentManager->bLeftMouseActive = true;
					offset = ComponentBase->GetLoc() - GetMouse2D(mousePos);
					if (ComponentBase->ComponenetState == EComponenetState::_Default)
					{
						ComponentBase->ComponenetState = EComponenetState::_Select;
						ComponentManager->SelectComponentEvent(type);
						ComponentBase->SetBrushLineColor(FLinearColor(0, 0.417f, 1));
					}
					if (HousePluginActor)
					{
						HousePluginActor->SetActorHiddenInGame(true);
					}
				}
				else
				{
					if (ComponentBase->ComponenetState == EComponenetState::_Select)
					{
						ComponentManager->bSelect = false;
						ComponentManager->bLeftMouseActive = false;
						ComponentBase->ComponenetState = EComponenetState::_Default;
						ComponentManager->CanCelComponentEvent(type);
					}
				}
			}

			bOtherHousePluginSelect = ComponentManager->bSelect;
			break;
		}
	}*/
	if (ComponentBaseWidget&&BuildingSystem)
	{
		/*if (!bOtherHousePluginSelect)
		{*/
			UWorld *MyWorld = GetWorld();
			FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
			UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
			if (MyWorld&&ComponentBase)
			{
				EComponenetType type = ComponentBase->GetComponenetType();
				FPrimitive2D outPrimitive = GetPrimitive2DWithAngle(ComponentBase->FramePrimitive, ComponentBase->GetAngleVal());
				if (DetectHitPrimitive2D(outPrimitive, mousePos))
				{
					bSelect = true;
					bLeftMouseActive = true;
					offset = ComponentBase->GetLoc() - GetMouse2D(mousePos);
					if (ComponentBase->ComponenetState == EComponenetState::_Default)
					{
						int Areaid = BuildingSystem->HitTest(ComponentBase->GetLoc());
						ComponentBase->ComponenetState = EComponenetState::_Select;
						if (Areaid != INDEX_NONE)
						{
							ComponentBase->SetComponentRoomID(Areaid);
							UpdateArea(Areaid);
						}
						b = true;
						//ComponentBase->SetBrushLineColor(FLinearColor(0, 0.417f, 1));
						SelectComponentEvent(type);
						UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
						ComponentBase->SetFrameColor(FLinearColor(1, 0.48, 0, 1));
						if (GameInst)
						{
							UWallBuildSystem* WallBuildSystem = GameInst->WallBuildSystem;
							if (WallBuildSystem)
							{
								WallBuildSystem->ShowAllCornerUMG();
								WallBuildSystem->RefreshAlignmentPoint(ComponentBase->TempX, ComponentBase->TempY, ComponentBase->ObjectSnapTol);
								ComponentBase->SetWorld(MyWorld);
								ComponentBase->SetDrawActive(true);
							}
						}
					}
					else
						if (ComponentBase->ComponenetState == EComponenetState::_Select)
						{
							ComponentBase->SetFrameColor(FLinearColor(1, 0.48, 0, 1));
							UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
							if (GameInst)
							{
								UWallBuildSystem* WallBuildSystem = GameInst->WallBuildSystem;
								if (WallBuildSystem)
								{
									WallBuildSystem->ShowAllCornerUMG();
									WallBuildSystem->RefreshAlignmentPoint(ComponentBase->TempX, ComponentBase->TempY, ComponentBase->ObjectSnapTol);
									ComponentBase->SetWorld(MyWorld);
									ComponentBase->SetDrawActive(true);
								}
							}
						}
					if (HousePluginActor)
					{
						HousePluginActor->SetActorHiddenInGame(true);
					}
				}
				else
				{
					if (ComponentBase->ComponenetState == EComponenetState::_Select)
					{
						bSelect = false;
						bLeftMouseActive = false;
						ComponentBase->ComponenetState = EComponenetState::_Default;
						//ComponentBase->SetBrushLineColor(FLinearColor::White);
						CanCelComponentEvent(type);
						ComponentBase->SetDrawActive(false);
					}
				}
			}
		/*}*/

	}
}
void AComponentManagerActor::LeftMouseUp()
{

	if (HousePluginActor)
	{
		HousePluginActor->SetActorHiddenInGame(false);
	}
	if (bLeftMouseActive)
	{
		bLeftMouseActive = false;
		if (ComponentBaseWidget)
		{
			UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
			if (ComponentBase)
			{
				if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
					SnapComponentLoc(ComponentBase->GetAllWallIdsInfo().Num());
				ComponentBase->SetDrawActive(false);
				ComponentBase->SetFrameColor(FLinearColor(1, 0.48, 0, 0));
			}
		}
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
		if (GameInst)
		{
			UWallBuildSystem* WallBuildSystem = GameInst->WallBuildSystem;
			if (WallBuildSystem)
			{
				WallBuildSystem->ClearAllCornnerUMG();
			}
		}
	}
	/*if (ComponentBaseWidget&&BuildingSystem)
	{
		bLeftMouseActive = false;
		UWorld *MyWorld = GetWorld();
		FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (MyWorld&&ComponentBase)
		{
			EComponenetType type = ComponentBase->GetComponenetType();
			FPrimitive2D outPrimitive = GetPrimitive2DWithAngle(ComponentBase->FramePrimitive, ComponentBase->GetAngleVal());
			if (!DetectHitPrimitive2D(outPrimitive, mousePos))
			{
				if (ComponentBase->ComponenetState == EComponenetState::_Select)
				{
					ComponentBase->ComponenetState = EComponenetState::_Default;
					bSelect = false;
					CanCelComponentEvent(type);

				}
			}
			if (HousePluginActor)
			{
				HousePluginActor->SetActorHiddenInGame(false);
			}
		}
	}*/
}


FComponenetInfo AComponentManagerActor::GetHouseComponentInfo() const
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
			return ComponentBase->GetComponenetInfo();
	}
	return FComponenetInfo();
}

void AComponentManagerActor::SetPluginTopMaterial(bool b)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			if (ComponentBase->GetComponenetType() == EComponenetType::_Pillar)
			{
				if (HousePluginActor)
				{
					HousePluginActor->SetPluginTopMaterial(b);
				}
			}
		}
	}
}

void AComponentManagerActor::DeleteHouseComponent()
{
	if (ComponentBaseWidget&&BuildingSystem)
	{
		bLeftMouseActive = false;
		UWorld *MyWorld = GetWorld();
		FVector2D mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		if (MyWorld&&ComponentBaseWidget->GetComponentBase())
		{
			EComponenetType type = ComponentBaseWidget->GetComponentBase()->GetComponenetType();
			switch (type)
			{
			case EComponenetType::_FLUE:
			{
				UFlueComponentPrimitive *FlueComponent = Cast<UFlueComponentPrimitive>(ComponentBaseWidget->GetComponentBase());
				if (FlueComponent->ComponenetState == EComponenetState::_Select)
				{
					BuildingSystem->DeleteFlueObj(FlueComponent->GetComponentDRActorID());
					DeleteComponentEvent(EComponenetType::_FLUE);
				}
			}
			break;
			case  EComponenetType::_BagRiser:
			{
				UBagRiserComponentPrimitive *BagRiserComponent = Cast<UBagRiserComponentPrimitive>(ComponentBaseWidget->GetComponentBase());
				if (BagRiserComponent->ComponenetState == EComponenetState::_Select)
				{
					BuildingSystem->DeleteTubeObj(BagRiserComponent->GetComponentDRActorID());
					DeleteComponentEvent(EComponenetType::_BagRiser);
				}

			}
			break;
			case  EComponenetType::_Pillar:
			{
				UPillarComponentPrimitive *PillarComponent = Cast<UPillarComponentPrimitive>(ComponentBaseWidget->GetComponentBase());
				if (PillarComponent->ComponenetState == EComponenetState::_Select)
				{
					BuildingSystem->DeletePillarObj(PillarComponent->GetComponentDRActorID());
					DeleteComponentEvent(EComponenetType::_Pillar);
				}

			}
			break;
			case  EComponenetType::_Sewer:
			{
				USewerComponentPrimitive *SewerComponent = Cast<USewerComponentPrimitive>(ComponentBaseWidget->GetComponentBase());
				if (SewerComponent->ComponenetState == EComponenetState::_Select)
				{
					BuildingSystem->DeleteTubeObj(SewerComponent->GetComponentDRActorID());
					DeleteComponentEvent(EComponenetType::_Sewer);
				}

			}
			break;
			default:
				break;
			}
		}
	}
}

FComponenetInfo AComponentManagerActor::GetComponenetInfo() const
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			return ComponentBase->GetComponenetInfo();
		}
	}
	return FComponenetInfo();
}

void AComponentManagerActor::ManualDestroyHouseComponent()
{
	if (ComponentBaseWidget&&BuildingSystem)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			EComponenetType type = ComponentBase->GetComponenetType();
			switch (type)
			{
			case EComponenetType::_FLUE:
				BuildingSystem->DeleteFlueObj(ComponentBase->GetComponentDRActorID());
				DeleteComponentEvent(EComponenetType::_FLUE);
				break;
			case EComponenetType::_BagRiser:
				BuildingSystem->DeleteTubeObj(ComponentBase->GetComponentDRActorID());
				DeleteComponentEvent(EComponenetType::_BagRiser);
				break;
			case EComponenetType::_Pillar:
				BuildingSystem->DeletePillarObj(ComponentBase->GetComponentDRActorID());
				DeleteComponentEvent(EComponenetType::_Pillar);
				break;
			case EComponenetType::_Sewer:
				BuildingSystem->DeletePillarObj(ComponentBase->GetComponentDRActorID());
				DeleteComponentEvent(EComponenetType::_Sewer);
				break;

			}

		}

	}
}

void AComponentManagerActor::SetHousePluginMaterial(int32 Index, UMaterialInterface* InMaterial)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
			{
				if (HousePluginActor)
				{
					HousePluginActor->GroupComponent->SetMaterial(Index, InMaterial);
				}
			}
		}
	}
}

void AComponentManagerActor::GetDirByWall(FVector& RayStart, FVector& RayDir)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			FComponenetInfo Info = ComponentBase->GetComponenetInfo();
			RayStart = FVector(Info.Loc, 0);
			RayDir = FVector((ComponentBase->PosAtWall - Info.Loc), 0);
			RayDir.Normalize();
		}
	}
}

FString AComponentManagerActor::GetHousePluginArea(float acreage)
{
	if (!(BuildingSystem&&ComponentBaseWidget))
		return TEXT("");
	UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (ComponentBase)
	{
		float height = ComponentBase->GetHeight();
		float ComponentAreaByM2=0.f;// = ComponentBase->GetWidth()*ComponentBase->GetLength()* 0.0001f;
		if (ComponentBase->GetAllWallIdsInfo().Num() == 1)
		{
			if (ComponentBase->GetIsWidth())
				ComponentAreaByM2 = ComponentBase->GetLength() * height * 2 * 0.0001f;
			else
				ComponentAreaByM2 = ComponentBase->GetWidth() * height * 2 * 0.0001f;
		}
		else if (ComponentBase->GetAllWallIdsInfo().Num() == 2)
		{
			ComponentAreaByM2 = 0.f;
		}
		else if(ComponentBase->GetAllWallIdsInfo().Num() == 0)
		{
			ComponentAreaByM2 = (ComponentBase->GetLength() + ComponentBase->GetWidth()) * height * 2 * 0.0001f;
		}
		ComponentAreaByM2 = ComponentAreaByM2 + acreage*0.0001f;
		const int32 Precision = 3;
		int32 Result = (int32)ceil(ComponentAreaByM2 * ULinearEntityLibrary::PowerInt(10, Precision));
		FString ComponentAreaInt = FString::FromInt(Result);
		int32 NumberLen = ComponentAreaInt.Len();
		FString ComponentAreaOutput;
		if (NumberLen > Precision)
		{
			ComponentAreaOutput = ComponentAreaInt.Left(NumberLen - Precision);
			ComponentAreaOutput.AppendChar(_T('.'));
			ComponentAreaOutput.Append(ComponentAreaInt.Right(Precision));
		}
		else
		{
			ComponentAreaOutput = _T("0.");
			int32 ZeroCnt = Precision - NumberLen;
			for (int32 Index = 0; Index < ZeroCnt; ++Index)
			{
				ComponentAreaOutput.AppendChar(_T('0'));
			}
			ComponentAreaOutput.Append(ComponentAreaInt);
		}

		return ComponentAreaOutput;
	}
	return TEXT("");	
}

FString AComponentManagerActor::GetHousePluginAreaWithOutWall()
{
	if (!(BuildingSystem&&ComponentBaseWidget))
		return TEXT("");
	UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (ComponentBase)
	{
		float height = ComponentBase->GetHeight();
		float ComponentAreaByM2 = 0.f;// = ComponentBase->GetWidth()*ComponentBase->GetLength()* 0.0001f;
		ComponentAreaByM2 = (ComponentBase->GetLength() + ComponentBase->GetWidth()) * height * 2 * 0.0001f;
		FString ComponentAreaOutput;
		ComponentAreaOutput= FloatToStringBy3Point(ComponentAreaByM2);
		return ComponentAreaOutput;
	}
	return TEXT("");
}

void AComponentManagerActor::GetHousePluginAreaWithWall(const TArray<int32>&Wallindex, FString& AddArea)
{
	if (!(BuildingSystem&&ComponentBaseWidget))
		return;
	UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (ComponentBase)
	{
		float height = ComponentBase->GetHeight();
		float ComponentAreaByM2 = 0.f;
		if (Wallindex.Num() == 1)
		{
			ComponentAreaByM2 = (ComponentBase->GetWidth() * 2 + ComponentBase->GetLength()) * height  * 0.0001f;
		}
		else
			if (Wallindex.Num() == 2)
			{

				ComponentAreaByM2 = (ComponentBase->GetLength() + ComponentBase->GetWidth()) * height  * 0.0001f;

			}
		AddArea = FloatToStringBy3Point(ComponentAreaByM2);
	}
}

float AComponentManagerActor::GetHousePluginOneSurfaceArea(const FVector WallStart, const FVector WallEnd)
{
	if (!(BuildingSystem&&ComponentBaseWidget))
		return 0;
	UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (ComponentBase)
	{
		FVector2D Loc = ComponentBase->GetLoc();
		float height = ComponentBase->GetHeight();
		float ComponentAreaByM2 = 0.f;
		float distance = UKismetMathLibrary::GetPointDistanceToSegment(FVector(Loc, 0), WallStart, WallEnd);
		if (FMath::Abs(ComponentBase->GetWidth()- distance)< FMath::Abs(ComponentBase->GetLength() - distance))
		{
			ComponentAreaByM2 = ComponentBase->GetLength() * height  * 0.0001f;
		}
		else
		{
			ComponentAreaByM2 = ComponentBase->GetWidth() * height  * 0.0001f;
		}
		return ComponentAreaByM2;
	}
	return 0;
}

FString AComponentManagerActor::FloatToStringBy3Point(float num)
{
	const int32 Precision = 3;
	int32 Result = (int32)ceil(num * ULinearEntityLibrary::PowerInt(10, Precision));
	FString AreaInt = FString::FromInt(Result);
	int32 NumberLen = AreaInt.Len();
	FString OutFloat;
	if (NumberLen > Precision)
	{
		OutFloat = AreaInt.Left(NumberLen - Precision);
		OutFloat.AppendChar(_T('.'));
		OutFloat.Append(AreaInt.Right(Precision));
	}
	else
	{
		OutFloat = _T("0.");
		int32 ZeroCnt = Precision - NumberLen;
		for (int32 Index = 0; Index < ZeroCnt; ++Index)
		{
			OutFloat.AppendChar(_T('0'));
		}
		OutFloat.Append(AreaInt);
	}

	return OutFloat;
}

void AComponentManagerActor::SetHousePluginActorMaterialInfo(const int32 modelID, const int32& RoomClassID, const int32& CraftID)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
			{
				if (HousePluginActor)
				{
					HousePluginActor->modelID = modelID;
					HousePluginActor->RoomClassID = RoomClassID;
					HousePluginActor->CraftID = CraftID;
				}
			}
		}
	}
}

void AComponentManagerActor::SetMaterialInfo(int32 MaterialChannel, const FString& ResID, int32 modelID, const int32& RoomClassID, const int32& CraftID)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			if (ComponentBase->GetComponenetType() != EComponenetType::_Sewer)
			{
				ComponentBase->SetMaterialInfo(MaterialChannel, ResID, modelID, RoomClassID, CraftID);
			}
		}
	}
}

TMap<int32, FString> AComponentManagerActor::GetMaterialInfo() const
{
	TMap<int32, FString> Temp;
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			return ComponentBase->GetMaterialInfo();
		}
	}
	return Temp;
}

void AComponentManagerActor::DeleteSelectRangeComponent(const FVector2D& Min, const FVector2D& Max)
{
	if (ComponentBaseWidget&&BuildingSystem)
	{
		EComponenetType type = ComponentBaseWidget->GetComponentBase()->GetComponenetType();
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		switch (type)
		{
		case EComponenetType::_FLUE:
		{
			UFlueComponentPrimitive *FlueComponent = Cast<UFlueComponentPrimitive>(ComponentBaseWidget->GetComponentBase());
			if (IsInBoxRange(Min, Max, FlueComponent->FramePrimitive))
			{
				BuildingSystem->DeleteFlueObj(FlueComponent->GetComponentDRActorID());
				DeleteComponentEvent(EComponenetType::_FLUE);
			}
		}
		break;
		case  EComponenetType::_BagRiser:
		{
			UBagRiserComponentPrimitive *BagRiserComponent = Cast<UBagRiserComponentPrimitive>(ComponentBaseWidget->GetComponentBase());
			if (IsInBoxRange(Min, Max, BagRiserComponent->FramePrimitive))
			{
				BuildingSystem->DeleteTubeObj(BagRiserComponent->GetComponentDRActorID());
				DeleteComponentEvent(EComponenetType::_BagRiser);
			}

		}
		break;
		case  EComponenetType::_Pillar:
		{
			UPillarComponentPrimitive *PillarComponent = Cast<UPillarComponentPrimitive>(ComponentBaseWidget->GetComponentBase());
			if (IsInBoxRange(Min, Max, PillarComponent->FramePrimitive))
			{
				BuildingSystem->DeletePillarObj(PillarComponent->GetComponentDRActorID());
				DeleteComponentEvent(EComponenetType::_Pillar);
			}

		}
		break;
		case  EComponenetType::_Sewer:
		{
			USewerComponentPrimitive *SewerComponent = Cast<USewerComponentPrimitive>(ComponentBaseWidget->GetComponentBase());
			if (IsInBoxRange(Min, Max, SewerComponent->FramePrimitive))
			{
				BuildingSystem->DeletePillarObj(SewerComponent->GetComponentDRActorID());
				DeleteComponentEvent(EComponenetType::_Sewer);
			}

		}
		break;
		default:
			break;
		}
	}
}

bool AComponentManagerActor::IsInBoxRange(const FVector2D& Min, const FVector2D& Max, const FPrimitive2D& FramePrimitive)
{
	FBox box(FVector(Min, 0), FVector(Max, 0));
	return FMath::PointBoxIntersection(FVector(FramePrimitive.LeftTopPos, 0), box) || FMath::PointBoxIntersection(FVector(FramePrimitive.RightTopPos, 0), box) ||
		FMath::PointBoxIntersection(FVector(FramePrimitive.RightBottomPos, 0), box) || FMath::PointBoxIntersection(FVector(FramePrimitive.LeftBottomPos, 0), box);
}


void FRoomVecData::InitData(AComponentManagerActor* Manager, const FVector2D& P0, const FVector2D& P1)
{
	startPos = P0;
	endPos = P1;
	FVector2D dir = endPos - startPos;
	dir.Normalize();
	/**
	   X
	   ^
  4	   |  1
	   |
------------->Y
	   |
  3    |  2
	   |
	   */

	if (dir.X > 0 && FMath::Abs(dir.Y) < 0.00001f)//+X轴上
	{
		AngleVal = 90.0f;
		IsWidth = false;
	}
	else if (FMath::Abs(dir.X) < 0.00001f && dir.Y > 0)//+Y轴上
	{
		AngleVal = 180.0f;
		IsWidth = false;
	}
	else if (dir.X < 0 && FMath::Abs(dir.Y) < 0.00001f)//-X轴上
	{
		AngleVal = -90.0f;
		IsWidth = false;
	}
	else if (FMath::Abs(dir.X) < 0.00001f && dir.Y < 0)//-Y轴上
	{
		AngleVal = 0.0f;
		IsWidth = false;
	}
	else if (dir.X > 0 && dir.Y > 0)//第一象限
	{
		float CosVal = dir | FVector2D(0, 1);
		float AngleR = acos(CosVal);
		AngleVal = 180 - (AngleR / PI) * 180;
		IsWidth = false;
	}
	else if (dir.X < 0 && dir.Y > 0)//第二象限
	{
		float CosVal = dir | FVector2D(-1, 0);
		float AngleR = acos(CosVal);
		AngleVal = - (AngleR / PI) * 180-90;
		IsWidth = false;
	}
	else if (dir.X < 0 && dir.Y < 0)//第三象限
	{
		float CosVal = dir | FVector2D(0, -1);
		float AngleR = acos(CosVal);
		AngleVal = -(AngleR / PI) * 180;
		IsWidth = false;
	}
	else if (dir.X > 0 && dir.Y < 0)//第四象限
	{
		float CosVal = dir | FVector2D(1, 0);
		float AngleR = acos(CosVal);
		AngleVal = 90 - (AngleR / PI) * 180;
		IsWidth = false;
	}

	FVector2D centerPos0 = startPos + dir * 7;
	FVector2D centerPos1 = endPos - dir * 7;
	rightDir = dir.GetRotated(90);
	rightDir.Normalize();
	FVector2D Hit1, Hit2;
	Hit1 = centerPos0 - rightDir * 1.2f;
	Hit2 = centerPos1 - rightDir * 1.2f;
	int id = INDEX_NONE;
	if (EObjectType::ESolidWall == Manager->FindWalls(Hit1, id))
	{
		wallId = id;
	}
	else if (EObjectType::ESolidWall == Manager->FindWalls(Hit2, id))
	{
		wallId = id;
	}
}

void AComponentManagerActor::SetMaterialUVInfo(FVector Scale, FVector Offset, float Angle)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			ComponentBase->SetMaterialUVInfo(Scale, Offset, Angle);
		}
	}
}

void AComponentManagerActor::SetHousePluginActorMaterialUVInfo(FVector Scale, FVector Offset, float Angle)
{
	if (HousePluginActor)
	{
		HousePluginActor->Scale = Scale;
		HousePluginActor->Offset = Offset;
		HousePluginActor->Angle = Angle;
		FVector NewScale = Scale / 100.0f;
		float X = NewScale.X*HousePluginActor->DefaultScale.X;
		float Y = NewScale.Y*HousePluginActor->DefaultScale.Y;
		for (int i = 2; i < 6; ++i)
		{
			UMaterialInterface* MaterialInterface = HousePluginActor->GroupComponent->GetMaterial(i);
			if (MaterialInterface)
			{
				UMaterialInstanceDynamic * DynamicMaterial = Cast<UMaterialInstanceDynamic>(MaterialInterface);
				if (DynamicMaterial)
				{
					DynamicMaterial->SetScalarParameterValue(TEXT("密度U"), X);
					DynamicMaterial->SetScalarParameterValue(TEXT("密度V"), Y);
					DynamicMaterial->SetScalarParameterValue(TEXT("法线U"), X);
					DynamicMaterial->SetScalarParameterValue(TEXT("法线V"), Y);
					DynamicMaterial->SetScalarParameterValue(TEXT("位移X"), Offset.X);
					DynamicMaterial->SetScalarParameterValue(TEXT("位移Y"), Offset.Y);
					DynamicMaterial->SetScalarParameterValue(TEXT("贴图角度"), Angle);
					DynamicMaterial->SetScalarParameterValue(TEXT("法线角度"), Angle);
				}
			}
		}
	}
}

void AComponentManagerActor::ResetHousePluginMaterialUV(FVector Scale, FVector Offset, float Angle)
{
	SetMaterialUVInfo(Scale, Offset, Angle);
	SetHousePluginActorMaterialUVInfo(Scale, Offset, Angle);
}

void AComponentManagerActor::FirstUpdate()
{
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		UComponentBasePrimitive* ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			int32 id = ComponentBase->GetComponentDRActorID();
			FVector2D Loc = ComponentBase->GetLoc();
			if (id > 0)
			{
				UpdateComponent(MyWorld, id, Loc);
			}
		}
	}
}
void AComponentManagerActor::SetHousePluginActorDefaultScale()
{
	if (HousePluginActor&&HousePluginActor->GroupComponent)
	{
		UMaterialInterface*  MaterialInterface = HousePluginActor->GroupComponent->GetMaterial(2);
		if (MaterialInterface)
		{
			UMaterialInstanceDynamic* MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(MaterialInterface);
			if (MaterialInstanceDynamic)
			{
				FString ParamName = TEXT("密度U");
				const TCHAR* TParamName1 = *ParamName;
				MaterialInstanceDynamic->GetScalarParameterValue(FMaterialParameterInfo(TParamName1), HousePluginActor->DefaultScale.X);
				ParamName = TEXT("密度V");
				const TCHAR* TParamName2 = *ParamName;
				MaterialInstanceDynamic->GetScalarParameterValue(FMaterialParameterInfo(TParamName2), HousePluginActor->DefaultScale.Y);
			}
		}
	}
}

void AComponentManagerActor::SetHouseComponentInfoHeight(float Height)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			ComponentBase->SetHeight(Height);
		}
	}
}

void AComponentManagerActor::SetWallIdsInfo(const TArray<int32>&WallIDs)
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			ComponentBase->SetWallIdsInfo(WallIDs);
		}
	}
}

EComponenetState AComponentManagerActor::GetState()
{
	if (ComponentBaseWidget)
	{
		UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
		if (ComponentBase)
		{
			return ComponentBase->ComponenetState;
		}
	}
	return EComponenetState::_Default;
}

void AComponentManagerActor::ShowComponentPositionWidget()
{
	if (!IsValid(CompPositionWidget))
	{
		CompPositionWidget = CreateWidget<UComponentPositionWidget>(GetWorld(), CompPostitionWidgetClass);
		//CompPositionWidget->AddToViewport();
		AddWidgetToViewport();
	}

	UComponentBasePrimitive *ComponentBase = ComponentBaseWidget->GetComponentBase();
	if (!ComponentBase)
		return;

	FVector2D screenPos = ComponentBase->GetLoc();
	int32 ObjID = BuildingSystem->HitTest(screenPos);
	CompPositionWidget->ObjectID = ObjID;
	CompPositionWidget->CompManager = this;
	CompPositionWidget->TargetPosition = screenPos;
	CompPositionWidget->Width = ComponentBase->GetWidth();
	CompPositionWidget->Length = ComponentBase->GetComponenetType() == EComponenetType::_Sewer ? ComponentBase->GetWidth() : ComponentBase->GetLength();
	CompPositionWidget->TPoylgon = ComponentBase->GetTPoylgon();
	CompPositionWidget->Angle = Round(ComponentBase->GetAngleVal());

	CompPositionWidget->UpdateCompInfo();
}
