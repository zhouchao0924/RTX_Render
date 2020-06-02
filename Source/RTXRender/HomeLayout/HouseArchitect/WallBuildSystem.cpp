

#include "WallBuildSystem.h"
#include "Building/FeatureWalls.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include "CGALWrapper/PolygonAlg.h"
#include "CGALWrapper/CgDataConvUtility.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
#include "EditorGameInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Building/BuildingSystem.h"
#include "DRGameMode.h"
#include "ProceduralMeshComponent.h"
#include "Building/DRModelFactory.h"
#include "Building/DRFunLibrary.h"
#include "Building/DRStruct.h"
#include "AJBlueprintFunctionLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HomeLayout/Commands/DrawHouse/DrawWallCmd.h"
#include "Building/HouseComponent/ComponentManagerActor.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRVirtualWallAdapter.h"
#include "Data/Adapter/DRHoleAdapter.h"
#include "Data/Adapter/DRSolidWallAdapter.h"
#include "Data/Adapter/DRAreaAdapter.h"
#include "Data/Adapter/DRCornerAdapter.h"
#include "Data/FunctionLibrary/DROperationHouseFunctionLibrary.h"
class IObject;
// Sets default values for this component's properties
UWallBuildSystem::UWallBuildSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	DefaultWallSetting();

	PntGridTol = 0.1f;	// 1mm is enough
	ObjectSnapTol = 5.0f;
	WallThickness = 10.0f;
	bShowConnectedPnts = true;
}

const TArray<ALineWallActor*>& UWallBuildSystem::GetWallsInScene()
{
	WallsInScene.Empty();
	GetBPAllWall(WallsInScene);
	return WallsInScene;
}

TArray<ACornerActor*>& UWallBuildSystem::GetCornersInScene()
{
	return WallCorners;
}

// Called when the game starts
void UWallBuildSystem::BeginPlay()
{
	Super::BeginPlay();
	TenmpID = 0;
	WallLinkage = NewObject<UWallLinkageComponent>(this);
	WallLinkage->WallSystem = this;
	WallLinkage->RegisterComponent();
	DRGameMode = Cast<ADRGameMode>(GetWorld()->GetAuthGameMode());
	GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	BuildingSystem = GameInst->GetBuildingSystemInstance();
	LastpointID = FirstWallID = LastWallID = INDEX_NONE;
}


// Called every frame
void UWallBuildSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWallBuildSystem::BuildWallGeometry()
{
	UpdateWallPositionInfo();

	MakeWallCorners();

	CalcuateWallGeometry();
}

void UWallBuildSystem::ComputeGeomFromData()
{
	MakeWallCorners();
	CalcuateWallGeometry();
}

void UWallBuildSystem::UpdateWallActors()
{

}

void UWallBuildSystem::SpawnWallByNode(const FSavedDataNode &WallSavedData)
{

}

void UWallBuildSystem::UpdateWallRulers(UPARAM(ref) FPaintContext &Context)
{

}

void UWallBuildSystem::UpdateWallPositionInfo()
{
	GetWallsInScene();
	for (auto &WallInScene : WallsInScene)
	{
		WallInScene->UpdateWallPos();
	}
}

struct FCornerInfo
{
public:
	FCornerNode CornerNode;
	TSet<FString> PointNameList;
	TSet<FString> WallList;
};

bool IsValidString(const FString &Name)
{
	return !Name.IsEmpty() && (Name != TEXT("None"));
}

void MakeCornerInfo(TArray<FCornerInfo> &OutCornerInfoList, FString &OutCornerName,
	const FString &WallTagName, const FString &RelateWallName,
	const FString &WallPointName, const FVector2D &WallCornerPos)
{
	bool bCornerFound = false;
	for (auto &CornerWall : OutCornerInfoList)
	{
		if (IsValidString(WallTagName) && CornerWall.WallList.Find(WallTagName)
			&& IsValidString(RelateWallName) && CornerWall.WallList.Find(RelateWallName))
		{
			bCornerFound = true;
		}
		else
		{
			if (IsValidString(WallPointName) && CornerWall.PointNameList.Find(WallPointName))
			{
				bCornerFound = true;
				CornerWall.WallList.Add(WallTagName);
			}
			else
			{
				if (WallCornerPos.Equals(CornerWall.CornerNode.Position, CornerSnap))
				{
					bCornerFound = true;
					CornerWall.WallList.Add(WallTagName);
					if (IsValidString(WallPointName))
					{
						CornerWall.PointNameList.Add(WallPointName);
					}
				}
			}
		}
		if (bCornerFound)
		{
			OutCornerName = CornerWall.CornerNode.Name;
			break;
		}
	}
	if (!bCornerFound)
	{
		FCornerInfo NewCornerWall;
		NewCornerWall.CornerNode.Position = WallCornerPos;
		NewCornerWall.WallList.Add(WallTagName);
		if (IsValidString(WallPointName))
		{
			NewCornerWall.PointNameList.Add(WallPointName);
		}
		if (IsValidString(RelateWallName))
		{
			NewCornerWall.WallList.Add(RelateWallName);
		}
		OutCornerInfoList.Add(NewCornerWall);
		OutCornerName = NewCornerWall.CornerNode.Name;
	}
}

void UWallBuildSystem::MakeWallCorners()
{
}

FVector2D UWallBuildSystem::GetDrawWallPosByMousePnt(ALineWallActor *&OutSnapWall, ACornerActor* &OutSnapX, ACornerActor* &OutSnapY,
	const FVector2D &InMousePnt, const TArray<ACornerActor*> &InIgnoredCorners, const TArray<ALineWallActor*> &InIgnoredWalls, bool IsShowToast)
{
	return FVector2D();
}

FString UWallBuildSystem::AddNewCorner(const FVector2D &CornerPos, int32 ObjectId)
{
	UWorld *World = GetWorld();
	ACornerActor *CornerActor = World->SpawnActor<ACornerActor>(ACornerActor::StaticClass(), FTransform());
	CornerActor->CornerProperty.Position = CornerPos;
	CornerActor->CornerProperty.ObjectId = ObjectId;
	WallCorners.Add(CornerActor);

	return CornerActor->CornerProperty.Name;
}

void UWallBuildSystem::MergeCorner(const FString &Corner2Merge, const FString &Corner2Del, bool bUpdate)
{
}

struct FCornerWall
{
	FCornerWall(FWallNode &InWall, bool bInStartCorner = true)
		: WallData(InWall), bStartCorner(bInStartCorner)
	{
		if (bStartCorner)
		{
			DirFromCorner = WallData.Start2EndDir;
		}
		else
		{
			DirFromCorner = -WallData.Start2EndDir;
		}

		DirFromCorner.Normalize();
	}

	float GetCornerLeftThick() const
	{
		return bStartCorner ? WallData.LeftThick : WallData.RightThick;
	}

	float GetCornerRightThick() const
	{
		return bStartCorner ? WallData.RightThick : WallData.LeftThick;
	}

	FVector2D GetDirFromCorner2End() const
	{
		return WallData.WallLength * DirFromCorner;
	}

	FVector2D GetCornerPosition() const
	{
		return bStartCorner ? WallData.StartPos : WallData.EndPos;
	}

	FWallNode &WallData;
	bool bStartCorner;
	FVector2D DirFromCorner;
};

void UWallBuildSystem::DragMoveWallCorner(const FString &CornerName, const FVector2D &NewCornerPos)
{
}

void UWallBuildSystem::CalcuateWallGeometry(bool bCreateCollison)
{
	GetWallsInScene();
}

void UWallBuildSystem::UpdateWallsByCorners(const TArray<FString>& CornersModified, bool bCreateCollison)
{
	GetWallsInScene();
}

void UWallBuildSystem::DefaultWallSetting()
{
	DefaultWallProp.bIsDelete = false;
	DefaultWallProp.Type = EWallTypeCPP::Wall;

	FMaterialNode DefaultWallMaterial;
	DefaultWallMaterial.ModelID = UDROperationHouseFunctionLibrary::GetConfigModelIDByType(EDConfigType::EDRWallConfig);
	DefaultWallMaterial.ResID = "";
	DefaultWallMaterial.Url = "";
	DefaultWallMaterial.CraftID = 301;
	DefaultWallMaterial.RoomClassID = 16;
	//DefaultWallMaterial.bIsProjectMat = false;
	//DefaultWallMaterial.Path = DEFAULT_WALL_MATERIAL;
	DefaultWallProp.WallNode.WallMaterial.Init(DefaultWallMaterial, 7);

	FMaterialNode DefaultAreaMaterial;
	//DefaultAreaMaterial.bIsProjectMat = false;
	//DefaultAreaMaterial.Path = DEFAULT_AREA_MATERIAL;

	DefaultWallProp.WallNode.TJXMat.Init(DefaultAreaMaterial, 7);

	DefaultWallProp.WallNode.AreaMat.Add(DefaultAreaMaterial);
	DefaultWallProp.WallNode.AreaMat.Add(DefaultWallMaterial);
	DefaultWallProp.WallNode.AreaMat.Add(DefaultWallMaterial);

	DefaultWallProp.WallNode.Color = WALL_DEFAULT_COLOR;

	DefaultWallProp.WallNode.LeftThick = DefaultWallProp.WallNode.RightThick = DEF_WALL_CUBE_SIZE.Y*0.5f;

	DefaultWallProp.WallNode.Height = DEF_WALL_CUBE_SIZE.Z;
}

bool UWallBuildSystem::CheckCollisionBP(TArray<FVector> obb1, FVector2D max, FVector2D min)
{
	TArray<FVector2D> postlist2D;
	for (size_t j = 0; j < obb1.Num(); j++)
	{
		postlist2D.Add(FVector2D(obb1[j].X, obb1[j].Y));
	}
	return CheckCollision(postlist2D, max, min);
}

bool UWallBuildSystem::CheckCollision(TArray<FVector2D> obb1, FVector2D max, FVector2D min)
{
	OBBQuadrilateral a = OBBQuadrilateral(obb1);
	OBBQuadrilateral b = OBBQuadrilateral(min, max);
	FVector2D nv = a.centerPoint - b.centerPoint;
	FVector2D axisA1 = a.axes[0];
	if (a.getProjectionRadius(axisA1) + b.getProjectionRadius(axisA1) <= abs(FVector2D::DotProduct(nv, axisA1))) return false;
	FVector2D axisA2 = a.axes[1];
	if (a.getProjectionRadius(axisA2) + b.getProjectionRadius(axisA2) <= abs(FVector2D::DotProduct(nv, axisA2))) return false;
	FVector2D axisB1 = b.axes[0];
	if (a.getProjectionRadius(axisB1) + b.getProjectionRadius(axisB1) <= abs(FVector2D::DotProduct(nv, axisB1))) return false;
	FVector2D axisB2 = b.axes[1];
	if (a.getProjectionRadius(axisB2) + b.getProjectionRadius(axisB2) <= abs(FVector2D::DotProduct(nv, axisB2))) return false;
	return true;
}


void UWallBuildSystem::RemoveWall(ALineWallActor *Wall2Remove)
{
	WallsInScene.Remove(Wall2Remove);

	Wall2Remove->Destroy();
	CalcuateWallGeometry();
}

ALineWallActor* UWallBuildSystem::CuttingWallByPosition(ALineWallActor *Wall2Cut, const FVector2D &PosOnWall)
{
	return nullptr;
}

TArray<FRoomPath> UWallBuildSystem::GetInnerRoomPathArray() const
{
	TArray<FRoomPath> RoomPathArray;
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	for (auto RegionInWorld : GameInst->AreaSystem->GetRegionsInWorld())
	{
		RoomPathArray.Add(RegionInWorld->GetRoomPath());
	}

	return RoomPathArray;
}

TArray<ALineWallActor*> UWallBuildSystem::GetRelatedCornerWall(const FString &Corner)
{
	GetWallsInScene();

	return TArray<ALineWallActor*>();
}

TArray<ALineWallActor*> UWallBuildSystem::GetAdjWallsByCorner(const FString &CornerID)
{
	GetWallsInScene();

	return TArray<ALineWallActor*>();
}

TArray<FString> UWallBuildSystem::GetAdjCorners(const FString &Corner)
{
	GetWallsInScene();
	TArray<FString> AdjCorners;

	return AdjCorners;
}

int32 UWallBuildSystem::IsSelectWallAndShowCorner(const FVector2D &Location)
{
	int32 ObjectId = INDEX_NONE;
	if (BuildingSystem)
	{
		FVector2D _OutPos;
		ObjectId = BuildingSystem->HitTest(Location);
		if (ObjectId != INDEX_NONE)
		{
			UBuildingData* Data = BuildingSystem->GetData(ObjectId);
			if (Data)
			{
				float ccc = Data->GetFloat("ThickLeft");
				int a = Data->GetInt("P0");
				int b = Data->GetInt("P1");
				if (ShowCorners.Num() > 0)
					ShowCorners.Empty();
				for (int i = 0; i < WallCorners.Num(); ++i)
				{
					if (WallCorners[i])
					{
						bool flag1 = (a != b) && (WallCorners[i]->CornerProperty.ObjectId == a
							|| WallCorners[i]->CornerProperty.ObjectId == b);
						int32 c = Data->GetObjectType();
						bool flag2 = (Data->GetObjectType() == EObjectType::ECorner) && (WallCorners[i]->CornerProperty.ObjectId == ObjectId);
						if (flag1 || flag2)
						{
							WallCorners[i]->ShowOutterCircle(true);
							WallCorners[i]->ShowInnerCircel(true);
							ShowCorners.Add(WallCorners[i]);
						}
						else
						{
							WallCorners[i]->ShowOutterCircle(false);
							WallCorners[i]->ShowInnerCircel(false);
							WallCorners[i]->bIsSelect = false;
						}
					}
				}
			}
		}
		else
		{
			ClearSlectCornner();
		}
	}
	return ObjectId;
}

void UWallBuildSystem::ClearSlectCornner()
{
	if (ShowCorners.Num() > 0)
	{
		for (int i = ShowCorners.Num() - 1; i >= 0; --i)
		{
			if (ShowCorners[i])
			{
				ShowCorners[i]->ShowOutterCircle(false);
				ShowCorners[i]->ShowInnerCircel(false);
				ShowCorners[i]->bIsSelect = false;
				ShowCorners.RemoveAt(i);
			}
		}
		ShowCorners.Empty();
	}
}

void UWallBuildSystem::UpdateWallCornerByPos(ALineWallActor *Wall2Update)
{
}

ALineWallActor* UWallBuildSystem::GetWallActorByID(const FString &WallID)
{
	ALineWallActor *WallFound = nullptr;
	GetWallsInScene();
	return WallFound;
}

ACornerActor* UWallBuildSystem::GetCornerActorByID(const FString &CornerID)
{
	auto CornerFoundPtr = WallCorners.FindByPredicate([CornerID](ACornerActor *InCornerActor)
	{
		return InCornerActor->CornerProperty.Name == CornerID;
	});

	return CornerFoundPtr ? (*CornerFoundPtr) : nullptr;
}

int32 UWallBuildSystem::GetDegreeOfCorner(const FString &InCornerID)
{
	int32 Degree = 0;
	GetWallsInScene();
	return Degree;
}

ALineWallActor* UWallBuildSystem::SearchWallByEndPntMethod2(const FVector2D &StartPnt, const FVector2D &EndPnt)
{
	ALineWallActor *OwnerWall = nullptr;
	ACornerActor *StartCorner = GetNearestCornerByPnt(StartPnt);
	ACornerActor *EndCorner = GetNearestCornerByPnt(EndPnt);
	if (StartCorner && EndCorner)
	{
		OwnerWall = GetWallByCorner(StartCorner->CornerProperty.Name, EndCorner->CornerProperty.Name);
	}

	return OwnerWall;
}

void UWallBuildSystem::SearchWallByEndPnt(TArray<ALineWallActor*> &OutWalls, const FVector2D &StartPnt, const FVector2D &EndPnt)
{
	GetWallsInScene();
}

void UWallBuildSystem::SearchWallsByEndPnt(TArray<ALineWallActor*> &OutWalls, const FVector2D &StartPnt, const FVector2D &EndPnt)
{
}

void UWallBuildSystem::SearchSideWallsByEndPnt(TArray<ALineWallActor*> &OutWalls, const FVector2D &StartPnt, const FVector2D &EndPnt)
{
}

ACornerActor* UWallBuildSystem::GetNearestCornerByPnt(const FVector2D &Pnt) const
{
	ACornerActor *NearestCorner = nullptr;
	float NearestCornerDist = -1;
	for (auto &WallCorner : WallCorners)
	{
		float Dist = FVector2D::Distance(Pnt, WallCorner->CornerProperty.Position);
		if (NearestCornerDist < 0 || Dist < NearestCornerDist)
		{
			NearestCorner = WallCorner;
			NearestCornerDist = Dist;
		}
	}

	return NearestCorner;
}

ALineWallActor* UWallBuildSystem::GetWallByCorner(const FString &StartCorner, const FString &EndCorner)
{
	GetWallsInScene();
	return nullptr;
}

ARoomActor* UWallBuildSystem::GetSegmentOwnerRoom(const FVector2D &Start, const FVector2D &End)
{
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	const TArray<ARoomActor*>& RoomActors = GameInst->AreaSystem->GetRegionsInWorld();
	ARoomActor *NearestRoom = nullptr;
	float RoomDist2Segment = 0.0f;
	for (ARoomActor *RoomActor : RoomActors)
	{
		float CurRoomDist = RoomActor->GetSegmentDist2Room(Start, End);
		if (!NearestRoom || CurRoomDist < RoomDist2Segment)
		{
			RoomDist2Segment = CurRoomDist;
			NearestRoom = RoomActor;
		}
	}

	return NearestRoom;
}

TArray<FVector2D> UWallBuildSystem::FindLocationOfCorners(TArray<FString> CornerIDs)
{
	int32 CornerFoundCnt = CornerIDs.Num();
	TArray<FVector2D> CornersLocation;
	CornersLocation.SetNum(CornerFoundCnt);
	for (auto& Corner : WallCorners)
	{
		auto CornerName = Corner->CornerProperty.Name;
		auto CornerLocation = Corner->CornerProperty.Position;
		for (int32 i = CornerIDs.Num() - 1; i >= 0; --i)
		{
			if (CornerIDs[i] == CornerName)
			{
				CornersLocation[i] = CornerLocation;
				CornerIDs.RemoveAt(i);
			}
		}
	}

	return CornersLocation;
}

const FSavedDataNode& UWallBuildSystem::GetDefaultWallSetting() const
{
	return DefaultWallProp;
}

void UWallBuildSystem::CheckAlignPoint(ALineWallActor *Wall, float ErrorTolerance, float& XPos, float& YPos, float& XLen, float& YLen)
{
}

void UWallBuildSystem::CreateTestWallActor()
{
}

int32 UWallBuildSystem::IsOverCornner(const FVector2D &Loc)
{
	int32 BestId = INDEX_NONE;
	if (ShowCorners.Num() > 0 && BuildingSystem)
	{
		FVector2D BestLoc;
		BestId = BuildingSystem->IsHitCornner(Loc);
		if (BestId != INDEX_NONE)
		{
			for (int i = 0; i < ShowCorners.Num(); ++i)
			{
				if (ShowCorners[i])
				{
					if (ShowCorners[i]->CornerProperty.ObjectId == BestId)
					{
						ShowCorners[i]->bIsSelect = true;
					}
					else
					{
						ShowCorners[i]->bIsSelect = false;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < ShowCorners.Num(); ++i)
			{
				if (ShowCorners[i])
					ShowCorners[i]->bIsSelect = false;
			}
		}
	}
	return BestId;
}
FVector2D UWallBuildSystem::LocateMouseSnapState(const int CornerID, const FVector2D &Loc)
{
	FVector2D BestLoc = Loc;
	if (BuildingSystem)
	{
		TArray<int32> CornnerIDs;
		GetAllObject(CornnerIDs, EObjectType::ECorner, false);
		for (int i = 0; i < CornnerIDs.Num(); ++i)
		{
			if (CornnerIDs[i] != CornerID)
			{
				IObject *pObj = BuildingSystem->GetObject(CornnerIDs[i]);
				if (pObj)
				{
					FVector2D CurrentCornerPos = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
					FVector2D Vec = Loc - CurrentCornerPos;
					if (std::abs(Vec.X) < ObjectSnapTol)
					{
						BestLoc = FVector2D(CurrentCornerPos.X, Loc.Y);
					}
					if (std::abs(Vec.Y) < ObjectSnapTol)
					{
						BestLoc = FVector2D(Loc.X, CurrentCornerPos.Y);
					}
				}
			}
		}
	}
	return BestLoc;
}

bool UWallBuildSystem::CornerMovable(const int32 objectID, const FVector2D &Loc)
{
	bool MoveSuccess = true;

	FVector2D MoveDelta = Loc - PreMouseLoction;
	FVector2D CurrentCornerPos, NewCornerPos;
	if (objectID != INDEX_NONE && bIsDragCorner&&BuildingSystem)
	{
		UBuildingData *CornerData = BuildingSystem->GetData(objectID);
		IObject *pObj = BuildingSystem->GetObject(objectID);
		if (pObj)
		{
			CurrentCornerPos = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
			NewCornerPos = CurrentCornerPos + MoveDelta;
		}
		FVector2D BestPos = LocateMouseSnapState(objectID, NewCornerPos);
		IValue *v = nullptr;
		v = BuildingSystem->GetProperty(objectID, "Walls");
		if (v&&CornerData&&pObj)
		{
			kArray<int> WallIDs = v->IntArrayValue();
			for (int i = 0; i < WallIDs.size(); ++i)
			{
				UBuildingData *WallData = BuildingSystem->GetData(WallIDs[i]);
				if (WallData)
				{
					IValue *Wallv = nullptr;
					Wallv = BuildingSystem->GetProperty(WallIDs[i], "Holes");
					if (Wallv)
					{
						int num = Wallv->GetArrayCount();
						if (num > 0)
						{
							FVector2D Range = WallData->GetVector2D("Range");
							float border = Range.Y;
							if (border > 0)
							{
								int P0 = WallData->GetInt("P0");
								int P1 = WallData->GetInt("P1");
								int TempP = 0;
								if (objectID == P0)
								{
									TempP = P1;
								}
								else
									if (objectID == P1)
									{
										TempP = P0;
									}
									else
										break;
								UBuildingData *CornerData1 = BuildingSystem->GetData(objectID);
								UBuildingData *CornerData2 = BuildingSystem->GetData(TempP);
								if (CornerData1 && CornerData2)
								{
									FVector2D StartPos = CornerData2->GetVector2D("Location");
									FVector2D EndPos = CornerData1->GetVector2D("Location");
									if (objectID == P1 && (BestPos - StartPos).Size() <= border + 5)
									{
										IObject *pObj = BuildingSystem->GetObject(objectID);
										if (pObj)
										{
											/*for (int i = 0; i < WallCorners.Num(); ++i)
											{
												if (WallCorners[i])
												{
													WallCorners[i]->ShowOutterCircle(true);
													WallCorners[i]->ShowInnerCircel(false);
													if (WallCorners[i]->CornerProperty.ObjectId == objectID)
													{
														WallCorners[i]->ShowInnerCircel(true);
														WallCorners[i]->CornerProperty.Position = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
													}
												}
											}*/
										}
										return false;
									}
									else
										if (objectID == P0 && (border + 5) > (BestPos - StartPos).Size())
										{
											/*	IObject *pObj = BuildingSystem->GetObject(objectID);
												if (pObj)
												{
													for (int i = 0; i < WallCorners.Num(); ++i)
													{
														if (WallCorners[i])
														{
															WallCorners[i]->ShowOutterCircle(true);
															WallCorners[i]->ShowInnerCircel(false);
															if (WallCorners[i]->CornerProperty.ObjectId == objectID)
															{
																WallCorners[i]->ShowInnerCircel(true);
																WallCorners[i]->CornerProperty.Position = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
															}
														}
													}
												}*/
											return false;
										}
								}
							}
						}
					}
				}
			}
		}
	}
	return MoveSuccess;
}

bool UWallBuildSystem::MoveCornner(const int32 objectID, const FVector2D &Loc)
{
	bool MoveSuccess = false;
	if (LastpointID != objectID)
	{
		PreMouseLoction = Loc;
		LastpointID = objectID;
	}

	FVector2D MoveDelta = Loc - PreMouseLoction;
	FVector2D CurrentCornerPos, NewCornerPos;
	if (objectID != INDEX_NONE && bIsDragCorner&&BuildingSystem)
	{
		UBuildingData *CornerData = BuildingSystem->GetData(objectID);
		IObject *pObj = BuildingSystem->GetObject(objectID);
		if (pObj)
		{
			CurrentCornerPos = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
			NewCornerPos = CurrentCornerPos + MoveDelta;
		}
		FVector2D BestPos = LocateMouseSnapState(objectID, NewCornerPos);
		IValue *v = nullptr;
		v = BuildingSystem->GetProperty(objectID, "Walls");
		if (v&&CornerData&&pObj)
		{
			kArray<int> WallIDs = v->IntArrayValue();
			for (int i = 0; i < WallIDs.size(); ++i)
			{
				UBuildingData *WallData = BuildingSystem->GetData(WallIDs[i]);
				if (WallData)
				{
					IValue *Wallv = nullptr;
					Wallv = BuildingSystem->GetProperty(WallIDs[i], "Holes");
					if (Wallv)
					{
						int num = Wallv->GetArrayCount();
						if (num > 0)
						{
							FVector2D Range = WallData->GetVector2D("Range");
							float border = Range.Y;
							if (border > 0)
							{
								int P0 = WallData->GetInt("P0");
								int P1 = WallData->GetInt("P1");
								int TempP = 0;
								if (objectID == P0)
								{
									TempP = P1;
								}
								else
									if (objectID == P1)
									{
										TempP = P0;
									}
									else
										break;
								UBuildingData *CornerData1 = BuildingSystem->GetData(objectID);
								UBuildingData *CornerData2 = BuildingSystem->GetData(TempP);
								if (CornerData1 && CornerData2)
								{
									FVector2D StartPos = CornerData2->GetVector2D("Location");
									FVector2D EndPos = CornerData1->GetVector2D("Location");
									if (objectID == P1 && (BestPos - StartPos).Size() <= border + 5)
									{
										IObject *pObj = BuildingSystem->GetObject(objectID);
										if (pObj)
										{
											for (int i = 0; i < WallCorners.Num(); ++i)
											{
												if (WallCorners[i])
												{
													WallCorners[i]->ShowOutterCircle(true);
													WallCorners[i]->ShowInnerCircel(false);
													if (WallCorners[i]->CornerProperty.ObjectId == objectID)
													{
														WallCorners[i]->ShowInnerCircel(true);
														WallCorners[i]->CornerProperty.Position = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
													}
												}
											}
										}
										return false;
									}
									else
										if (objectID == P0 && (border + 5) > (BestPos - StartPos).Size())
										{
											IObject *pObj = BuildingSystem->GetObject(objectID);
											if (pObj)
											{
												for (int i = 0; i < WallCorners.Num(); ++i)
												{
													if (WallCorners[i])
													{
														WallCorners[i]->ShowOutterCircle(true);
														WallCorners[i]->ShowInnerCircel(false);
														if (WallCorners[i]->CornerProperty.ObjectId == objectID)
														{
															WallCorners[i]->ShowInnerCircel(true);
															WallCorners[i]->CornerProperty.Position = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
														}
													}
												}
											}
											return false;
										}
								}
							}
						}
					}
				}
			}
		}

		FVector2D DeltaMove = FVector2D( Round(100.0f*(BestPos.X - CurrentCornerPos.X))/100.0f, Round(100.0f*(BestPos.Y- CurrentCornerPos.Y) )/ 100.0f);
		if (bMoveBorderWallCorner)
		{
			FVector2D TempLoc = MoveDelta;
			UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
			if (projectDataManager == nullptr) {
				return false;
			}
			//if (BorderCornerID_1 == -1 || BorderCornerID_2 == -1)
			//{
			//	return false;
			//}
			if (WallCornerPos_1 == FVector2D::ZeroVector || WallCornerPos_2 == FVector2D::ZeroVector)
			{
				return false;
			}

			FVector2D WallDriection = WallCornerPos_2 - WallCornerPos_1;
			FVector2D DeltaPos = FVector2D::DotProduct(TempLoc, WallDriection) / (WallDriection.X * WallDriection.X + WallDriection.Y * WallDriection.Y) * WallDriection;
			MoveSuccess = BuildingSystem->Move(objectID, DeltaPos);
		}
		else
			MoveSuccess = BuildingSystem->Move(objectID, DeltaMove);
		PreMouseLoction = Loc;

		for (int i = 0; i < WallCorners.Num(); ++i)
		{
			if (WallCorners[i])
			{
				WallCorners[i]->ShowOutterCircle(true);
				WallCorners[i]->ShowInnerCircel(false);
				if (WallCorners[i]->CornerProperty.ObjectId == objectID)
				{
					WallCorners[i]->ShowInnerCircel(true);
					WallCorners[i]->CornerProperty.Position = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
				}
			}
		}
	}
	return MoveSuccess;
}

void UWallBuildSystem::ClearAllCornnerUMG()
{
	for (int i = 0; i < WallCorners.Num(); ++i)
	{
		if (WallCorners[i])
		{
			WallCorners[i]->ShowOutterCircle(false);
			WallCorners[i]->ShowInnerCircel(false);
		}
	}
	for (int i = 0; i < ShowCorners.Num(); ++i)
	{
		if (ShowCorners[i])
		{
			ShowCorners[i]->ShowOutterCircle(true);
			ShowCorners[i]->ShowInnerCircel(true);
		}
	}
	bIsDragCorner = false;
}

FVector2D UWallBuildSystem::GetCurrentCornerViewPortPos(const int32 &objectID, FVector &WorldPos)
{
	FVector2D WidgetPos;
	if (objectID != INDEX_NONE && BuildingSystem)
	{
		IObject *pObj = BuildingSystem->GetObject(objectID);
		if (pObj)
		{
			FVector2D Loc = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
			WorldPos = FVector(Loc, 0);
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(WorldPos, WidgetPos);
		}
	}
	return WidgetPos;
}

ESelectObjectType UWallBuildSystem::GetSlectObjectType(const FVector2D &Location, int32 &objectID)
{
	if (BuildingSystem)
		objectID = BuildingSystem->HitTest(Location);
	if (objectID != INDEX_NONE && BuildingSystem)
	{
		UBuildingData *data = BuildingSystem->GetData(objectID);
		if (data)
		{
			EObjectType InClass = (EObjectType)data->GetObjectType();
			switch (InClass)
			{
			case ECorner:
				return ESelectObjectType::Corner_Object;
			case ESolidWall:
				return ESelectObjectType::Wall_Object;
			case ERoom:
			case EPolygonArea:
				return ESelectObjectType::Room_Object;
			default:
				return ESelectObjectType::None_Object;
			}
		}
		else
			return ESelectObjectType::None_Object;
	}
	else
		return ESelectObjectType::None_Object;
}

bool UWallBuildSystem::IsCorner(const int32 &objectID)
{
	if (objectID != INDEX_NONE && BuildingSystem)
	{
		UBuildingData *data = BuildingSystem->GetData(objectID);
		if (data)
		{
			EObjectType InClass = (EObjectType)data->GetObjectType();
			if (InClass == EObjectType::ECorner)
				return true;
		}
	}
	return false;
}

void UWallBuildSystem::ShowSlectCorner(int32 objectID)
{
	if (objectID != INDEX_NONE)
	{
		if (ShowCorners.Num() > 0)
			ShowCorners.Empty();
		for (int i = 0; i < WallCorners.Num(); ++i)
		{
			if (WallCorners[i])
			{
				if (WallCorners[i]->CornerProperty.ObjectId == objectID)
				{
					WallCorners[i]->ShowOutterCircle(true);
					WallCorners[i]->ShowInnerCircel(true);
					ShowCorners.Add(WallCorners[i]);
				}
				else
				{
					WallCorners[i]->ShowOutterCircle(false);
					WallCorners[i]->ShowInnerCircel(false);
					WallCorners[i]->bIsSelect = false;
				}
			}
		}

	}
}
void UWallBuildSystem::ShowSlectWall(int32 objectID)
{
	if (objectID != INDEX_NONE && BuildingSystem)
	{
		UBuildingData* Data = BuildingSystem->GetData(objectID);
		if (Data)
		{
			int a = Data->GetInt("P0");
			int b = Data->GetInt("P1");
			if (ShowCorners.Num() > 0)
				ShowCorners.Empty();
			for (int i = 0; i < WallCorners.Num(); ++i)
			{
				if (WallCorners[i])
				{
					bool flag = (a != b) && (WallCorners[i]->CornerProperty.ObjectId == a
						|| WallCorners[i]->CornerProperty.ObjectId == b);
					if (flag)
					{
						WallCorners[i]->ShowOutterCircle(true);
						WallCorners[i]->ShowInnerCircel(true);
						ShowCorners.Add(WallCorners[i]);
					}
					else
					{
						WallCorners[i]->ShowOutterCircle(false);
						WallCorners[i]->ShowInnerCircel(false);
						WallCorners[i]->bIsSelect = false;
					}
				}
			}
		}

	}
}
FVector2D UWallBuildSystem::ShowSlectRoom(int32 objectID, TArray<FVector>& SlotPos)
{
	FVector2D CenterPos;
	if (objectID != INDEX_NONE && BuildingSystem)
	{
		TArray<FVector2D> TPolygons;
		SlotPos.Empty();
		int num = INDEX_NONE;
		num = BuildingSystem->GetPolygon(objectID, TPolygons, true);
		if (num > 2)
		{

			for (int i = 0; i < TPolygons.Num(); ++i)
			{
				SlotPos.Add(FVector(TPolygons[i], 0));
			}
			bool IsUseful = FPolygonAlg::CheckRegionUseness(TPolygons);
			if (!IsUseful)
			{
				float Ave_X = 0.0, Ave_Y = 0.0;
				for (int _CurI = 0; _CurI < TPolygons.Num(); ++_CurI)
				{
					Ave_X = Ave_X + TPolygons[_CurI].X;
					Ave_Y = Ave_Y + TPolygons[_CurI].Y;
				}
				Ave_X = Ave_X / TPolygons.Num();
				Ave_Y = Ave_Y / TPolygons.Num();
				return FVector2D(Ave_X, Ave_Y);
			}
			FPolygonAlg::LocateSuitableCenter(TPolygons, CenterPos);
		}

	}
	return CenterPos;
}

void UWallBuildSystem::DeleteSomeActors()
{
	int32 num = INDEX_NONE;
	if (BuildingSystem)
	{
		int32* pResults = nullptr;
		FVector2D Min, Max;
		if (DeleteMinMousePos.X < DeleteMaxMousePos.X)
		{
			Min.X = DeleteMinMousePos.X;
			Max.X = DeleteMaxMousePos.X;
		}
		else
		{
			Min.X = DeleteMaxMousePos.X;
			Max.X = DeleteMinMousePos.X;
		}
		if (DeleteMinMousePos.Y < DeleteMaxMousePos.Y)
		{
			Min.Y = DeleteMinMousePos.Y;
			Max.Y = DeleteMaxMousePos.Y;
		}
		else
		{
			Min.Y = DeleteMaxMousePos.Y;
			Max.Y = DeleteMinMousePos.Y;
		}

		num = BuildingSystem->HitTestMinMax(Min, Max, pResults);
		if (num > 0)
		{
			TArray<int32> VirWallIDs;
			bool IsDeleteVirWall = false;
			for (int i = 0; i < num; ++i)
			{
				EObjectType InClass;
				UBuildingData* Data = BuildingSystem->GetData(pResults[i]);
				if (Data)
				{
					InClass = (EObjectType)Data->GetObjectType();
				}
				if (IsDeleteWalls())
					DeleteSlectActor(pResults[i]);
				else
					if (IsDeleteHoles())
					{
						if (InClass == EObjectType::EWallHole || InClass == EObjectType::EDoorHole || InClass == EObjectType::EWindow)
							DeleteHole(pResults[i]);
					}
				if (IsDeleteWalls() && InClass == EObjectType::EEdge)
				{
					IsDeleteVirWall = true;
					VirWallIDs.Add(pResults[i]);
				}
			}
			if (IsDeleteVirWall)
			{
				OnlyDelateVirWall(VirWallIDs);
			}
		}
		for (TMap<int32, AWallPlaneBase*>::TIterator It(WallPlaneMap); It; ++It)
		{
			if (It.Value() != nullptr)
			{
				AWallPlaneBase* WallPlane = It.Value();
				if (WallPlane->StructIndex != -1)
				{
					TArray<FVector> OutPosList;
					OutPosList=WallPlane->VerListPos;
					TArray<FVector2D> postlist2D;
					for (size_t j = 0; j < OutPosList.Num(); j++)
					{
						postlist2D.Add(FVector2D(OutPosList[j].X, OutPosList[j].Y));
					}
					if (CheckCollision(postlist2D, Max, Min))
					{
						WallPlane->Delete();

					}
				}


			}

		}
		CheckDeleteWallCollision(Max, Min);
		//..Destroy All House Component 
		FVector2D InMin, InMax;
		UWorld *MyWorld = GetWorld();
		if (MyWorld)
		{
			MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(DeleteMinMousePos, 280), InMin);
			MyWorld->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(DeleteMaxMousePos, 280), InMax);
			FVector2D temp;
			if (InMax < InMin)
			{
				temp = InMin;
				InMin = InMax;
				InMax = temp;
			}
		}
		TArray<AActor*> OutActor;
		UGameplayStatics::GetAllActorsOfClass(this, AComponentManagerActor::StaticClass(), OutActor);
		for (int i = 0; i < OutActor.Num(); ++i)
		{
			AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(OutActor[i]);
			if (ComponentManager)
			{
				ComponentManager->DeleteSelectRangeComponent(InMin, InMax);
			}

		}
		BoxSelectDeleteHole(DeleteMinMousePos, DeleteMaxMousePos);
	}
}


void UWallBuildSystem::AddVirtualWallCorner(const FVector2D &Loc)
{
	/*if (BuildingSystem&&bIsCanCutArea)
	{
		FVector2D BestLoc = LocateMouseSnapState(Loc);
		NewCorner = INDEX_NONE;
		if (CutAreaPos_Tarray.Num() == 0 || bIsCutAreaDown)
		{
			float Zpos;
			TArray<FVector>WallNodes;
			if (bIsCutAreaDown)
			{

				UBuildingData* Data = BuildingSystem->GetData(LastWallID);
				if (Data)
				{
					EObjectType Type = (EObjectType)Data->GetObjectType();
					if (Type == EObjectType::ESolidWall)
						BuildingSystem->GetWallBorderLines(LastWallID, WallNodes, Zpos);
					else
					{
						BestLoc = Data->GetVector2D("Location");
					}
				}
			}
			else
			{
				UBuildingData* Data = BuildingSystem->GetData(FirstWallID);
				if (Data)
				{
					EObjectType Type = (EObjectType)Data->GetObjectType();
					if (Type == EObjectType::ESolidWall)
						BuildingSystem->GetWallBorderLines(FirstWallID, WallNodes, Zpos);
					else
					{
						BestLoc = Data->GetVector2D("Location");
					}
				}

			}
			if (WallNodes.Num() == 6)
				BestLoc = FVector2D(FMath::ClosestPointOnSegment(FVector(BestLoc, 0), WallNodes[0], WallNodes[3]));
		}
		//NewCorner = BuildingSystem->AddCorner(BestLoc);
		TempVirtualLoc.Add(BestLoc);
		AddNewCorner(BestLoc, -1);
		FCutAreaPos _CutAreaPos;
		_CutAreaPos.ObjectId = -1;
		_CutAreaPos.Loc = BestLoc;
		CutAreaPos_Tarray.Add(_CutAreaPos);
		if (WallCorners.Num() > 0)
		{
			WallCorners[WallCorners.Num() - 1]->ShowOutterCircle(true);
			WallCorners[WallCorners.Num() - 1]->ShowInnerCircel(false);
		}
		//LastCorner = NewCorner;
	}
	if (bIsCutAreaDown&&BuildingSystem)
	{
		if (FirstWallID == LastWallID)
		{
			CutAreaFailed();
		}
		else
		{
			bool IsFail = false;
			if (CutAreaPos_Tarray.Num() > 2)
			{
				for (int i = 0; i < CutAreaPos_Tarray.Num(); ++i)
				{
					if (i + 4 > CutAreaPos_Tarray.Num() || IsFail)
					{
						break;
					}
					for (int j = i + 2;j < CutAreaPos_Tarray.Num() - 1;++j)
					{
						FVector2D a1 = CutAreaPos_Tarray[i].Loc;
						FVector2D a2 = CutAreaPos_Tarray[i + 1].Loc;
						FVector2D b1 = CutAreaPos_Tarray[j].Loc;
						FVector2D b2 = CutAreaPos_Tarray[j + 1].Loc;
						if (((a1.X > a2.X ? a1.X : a2.X) < (b1.X < b2.X ? b1.X : b2.X) ||
							(a1.Y > a2.Y ? a1.Y : a2.Y) < (b1.Y < b2.Y ? b1.Y : b2.Y)) ||
							((b1.X > b2.X ? b1.X : b2.X) < (a1.X < a2.X ? a1.X : a2.X) ||
							(b1.Y > b2.Y ? b1.Y : b2.Y) < (a1.Y < a2.Y ? a1.Y : a2.Y)))
						{
							continue;
						}
						double aaa = FVector2D::CrossProduct((a1 - b1).GetSafeNormal(), (a2 - b1).GetSafeNormal());
						double bbb = FVector2D::CrossProduct((a1 - b2).GetSafeNormal(), (a2 - b2).GetSafeNormal());
						if (aaa*bbb < 0)
						{
							IsFail = true;
							break;
						}
					}
				}
			}
			if (IsFail)
			{
				CutAreaFailed();
				ShowAllCornerUMG();
			}
			else
			{
				++TenmpID;
				for (int i = 0; i < CutAreaPos_Tarray.Num(); ++i)
				{
					CutAreaPos_Tarray[i].ObjectId = BuildingSystem->AddCorner(CutAreaPos_Tarray[i].Loc);
				}
				for (int i = 0; i < CutAreaPos_Tarray.Num() - 1; ++i)
				{
					TArray<int32> WallIDs;
					WallIDs = AddVirtualWall(CutAreaPos_Tarray[i].ObjectId, CutAreaPos_Tarray[i + 1].ObjectId);
					for (int k = 0;k < WallIDs.Num();++k)
					{
						IValue & GroundName = UBuildingSystem::GetValueFactory()->Create(TenmpID);
						IObject * _ObjGround = BuildingSystem->GetObject(WallIDs[k]);
						if (_ObjGround)
						{
							_ObjGround->SetValue("GroundName", &GroundName);
							IValue *  _V = _ObjGround->FindValue("GroundName");
						}
					}
				}
				ClearAllCornerData();
				UpdataCornerUMGData();
				CutAreaPos_Tarray.Empty();
				TempVirtualLoc.Empty();
				BuildAreaWithVirWalls();
				ShowAllCornerUMG();
			}
		}
		FirstWallID = LastWallID = LastCorner = INDEX_NONE;
		bIsCanCutArea = bIsCutAreaDown = false;
	}*/
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (!Local_DRProjData)
		return;
	if (bIsCanCutArea)
	{
		FVector2D Local_BestLoc = LocateMouseSnapState(Loc);
		int32	  Local_BestID = INDEX_NONE;
		if (CutAreaPos_Tarray.Num() == 0 || bIsCutAreaDown)
		{
			TArray<FVector>WallNodes;
			if (bIsCutAreaDown)
			{

				UBuildingData* Data = BuildingSystem->GetData(LastWallID);
				if (Data)
				{
					EObjectType Type = (EObjectType)Data->GetObjectType();
					float Local_WallThickness = GetHitWallThickness(LastWallID);
					if (Type == EObjectType::ESolidWall || Type == EObjectType::ECorner)
					{
						Local_BestLoc = Loc;
						FVector2D Local_Loc = CutAreaPos_Tarray[CutAreaPos_Tarray.Num() - 1].Loc;
						Local_BestID = LastWallID;
						if (Type == EObjectType::ESolidWall)
						{
							int32 Local_Snap = -1;
							CalculateBestEdgeCornerPos(Local_BestLoc, Local_Loc, Local_BestID, Local_Snap);
						}
						else
						{
							UDRCornerAdapter* Local_CornerAdatpter = Cast<UDRCornerAdapter>(Local_DRProjData->GetAdapter(LastWallID));
							TArray<int32> Local_Walls = Local_CornerAdatpter->GetWalls();
							FVector2D Local_BestLoc0 = Local_BestLoc;
							int32 Local_BestID0 = LastWallID;
							float Local_Length = 1000000.0f;
							for (int32 WallIndex = 0; WallIndex < Local_Walls.Num(); ++WallIndex)
							{
								int32 Local_Snap = -1;
								FVector2D Temp_BestLoc = Local_BestLoc;
								int32 Temp_BestID = Local_Walls[WallIndex];
								CalculateBestEdgeCornerPos(Temp_BestLoc, Local_Loc, Temp_BestID, Local_Snap);
								if (Temp_BestLoc != Local_BestLoc && FVector2D::Distance(Temp_BestLoc, Local_Loc) <= Local_Length)
								{
									Local_Length = FVector2D::Distance(Temp_BestLoc, Local_Loc);
									Local_BestLoc0 = Temp_BestLoc;
									Local_BestID0 = Temp_BestID;
								}
							}
							Local_BestLoc = Local_BestLoc0;
							Local_BestID = Local_BestID0;
						}
					}
					else
					{
						Local_BestLoc = Data->GetVector2D("Location");
					}
				}
			}
			else
			{
				UBuildingData* Data = BuildingSystem->GetData(FirstWallID);
				if (Data)
				{
					EObjectType Type = (EObjectType)Data->GetObjectType();
					float Local_WallThickness = GetHitWallThickness(FirstWallID);
					if (Type == EObjectType::ESolidWall)
						BuildingSystem->CutAreaSnap(Loc, -1, Local_BestLoc, Local_BestID, Local_WallThickness, Local_WallThickness);
					else if (Type == EObjectType::ECorner)
					{
						BuildingSystem->CutAreaSnap(Loc, -1, Local_BestLoc, Local_BestID, Local_WallThickness, Local_WallThickness);
					}
					else
					{
						Local_BestLoc = Data->GetVector2D("Location");
					}
				}

			}
		}


		FCutAreaPos _CutAreaPos;
		_CutAreaPos.ObjectId = -1;
		_CutAreaPos.ObjectId = Local_BestID;
		_CutAreaPos.OriginalLoc = Loc;
		if (_CutAreaPos.ObjectId == -1)
		{
			_CutAreaPos.Loc = Loc;
			TempVirtualLoc.Add(Loc);
		}
		else
		{
			_CutAreaPos.Loc = Local_BestLoc;
			TempVirtualLoc.Add(Local_BestLoc);
		}

		CutAreaPos_Tarray.Add(_CutAreaPos);
	}
	if (bIsCutAreaDown)
	{
		if (FirstWallID == LastWallID)
		{
			CutAreaFailed();
		}
		else
		{
			bool IsFail = false;
			if (CutAreaPos_Tarray.Num() > 2)
			{
				for (int i = 0; i < CutAreaPos_Tarray.Num(); ++i)
				{
					if (i + 4 > CutAreaPos_Tarray.Num() || IsFail)
					{
						break;
					}
					for (int j = i + 2; j < CutAreaPos_Tarray.Num() - 1; ++j)
					{
						FVector2D a1 = CutAreaPos_Tarray[i].Loc;
						FVector2D a2 = CutAreaPos_Tarray[i + 1].Loc;
						FVector2D b1 = CutAreaPos_Tarray[j].Loc;
						FVector2D b2 = CutAreaPos_Tarray[j + 1].Loc;
						if (((a1.X > a2.X ? a1.X : a2.X) < (b1.X < b2.X ? b1.X : b2.X) ||
							(a1.Y > a2.Y ? a1.Y : a2.Y) < (b1.Y < b2.Y ? b1.Y : b2.Y)) ||
							((b1.X > b2.X ? b1.X : b2.X) < (a1.X < a2.X ? a1.X : a2.X) ||
							(b1.Y > b2.Y ? b1.Y : b2.Y) < (a1.Y < a2.Y ? a1.Y : a2.Y)))
						{
							continue;
						}
						double aaa = FVector2D::CrossProduct((a1 - b1).GetSafeNormal(), (a2 - b1).GetSafeNormal());
						double bbb = FVector2D::CrossProduct((a1 - b2).GetSafeNormal(), (a2 - b2).GetSafeNormal());
						if (aaa*bbb < 0)
						{
							IsFail = true;
							break;
						}
					}
				}
			}
			if (IsFail)
			{
				CutAreaFailed();
				ShowAllCornerUMG();
			}
			else
			{
				++TenmpID;
				for (int i = 0; i < CutAreaPos_Tarray.Num() - 1; ++i)
				{
					TArray<int32> WallIDs;
					CalculateBestEdgeCornerPos(CutAreaPos_Tarray[i].Loc, CutAreaPos_Tarray[i + 1].Loc, CutAreaPos_Tarray[i].ObjectId, CutAreaPos_Tarray[i + 1].ObjectId);
					AddEdgeCorner(CutAreaPos_Tarray[i].Loc, CutAreaPos_Tarray[i].ObjectId);
					AddEdgeCorner(CutAreaPos_Tarray[i + 1].Loc, CutAreaPos_Tarray[i + 1].ObjectId);
					WallIDs = AddEdge(CutAreaPos_Tarray[i].Loc, CutAreaPos_Tarray[i + 1].Loc, CutAreaPos_Tarray[i].ObjectId, CutAreaPos_Tarray[i + 1].ObjectId, false);
					UDRVirtualWallAdapter* Local_VirtualWallAdapter = nullptr;
					for (int k = 0; k < WallIDs.Num(); ++k)
					{
						Local_VirtualWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(WallIDs[k]));
						if (Local_VirtualWallAdapter)
						{
							Local_VirtualWallAdapter->SetAssociationID(TenmpID);
						}
						else
						{
							FillWallDefaultProperties(WallIDs[k]);
							if (GameInst&&GameInst->DrawHouse_Type == EDrawHouseType::ChaigaiDrawType)
							{
								if (IsinterlinkNewWall(WallIDs[k]))
								{
									if (Local_DRProjData)
									{
										UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(WallIDs[k]));
										if (solidWallAdapter)
										{
											solidWallAdapter->SetSolidWallType(ESolidWallType::EDRNewWall);
											solidWallAdapter->SetNewWallType(ENewWallType::EPlasterWall);
										}
									}
								}
							}
						}
					}
				}
				ClearAllCornerData();
				UpdataCornerUMGData();
				CutAreaPos_Tarray.Empty();
				TempVirtualLoc.Empty();
				BuildAreaWithVirWalls();
				ShowAllCornerUMG();
			}
		}
		FirstWallID = LastWallID = LastCorner = INDEX_NONE;
		bIsCanCutArea = bIsCutAreaDown = false;
	}
}

void UWallBuildSystem::CutAreaTick(const FVector2D &Loc)
{
	FVector2D BestLoc = LocateMouseSnapState(Loc);
	CurrentTempVirtualLoc.Empty();
	if (TempVirtualLoc.Num() > 0)
	{
		CurrentTempVirtualLoc.Add(TempVirtualLoc[TempVirtualLoc.Num() - 1]);
		CurrentTempVirtualLoc.Add(BestLoc);
	}
}

FVector2D UWallBuildSystem::LocateMouseSnapState(const FVector2D &Loc)
{
	FVector2D  MousePos = Loc;
	FVector2D  BestLoc;
	int32		BestID = INDEX_NONE;
	int32 Pt = BuildingSystem->Snap(Loc, -1, BestLoc, BestID, ObjectSnapTol, ObjectSnapTol*2);
	if (Pt == kESnapType::kEPt)
	{
		MousePos = Loc;
	}
	else
	{
		if (Pt == kESnapType::kEDirY)
		{
			if (LastCorner != INDEX_NONE)
			{
				IObject *pCorner = BuildingSystem->GetObject(LastCorner);
				if (pCorner)
				{
					FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
					FVector2D Vec = MousePos - Location;
					if (std::abs(Vec.Y) < ObjectSnapTol)
					{
						MousePos = FVector2D(BestLoc.X, Location.Y);
					}
					else
						MousePos = FVector2D(BestLoc.X, MousePos.Y);
				}
			}
			else
			{
				MousePos = FVector2D(BestLoc.X, MousePos.Y);
			}
		}
		else
			if (Pt == kESnapType::kEDirX)
			{
				if (LastCorner != INDEX_NONE)
				{
					IObject *pCorner = BuildingSystem->GetObject(LastCorner);
					if (pCorner)
					{
						FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
						FVector2D Vec = MousePos - Location;
						if (std::abs(Vec.X) < ObjectSnapTol)
						{
							MousePos = FVector2D(Location.X, BestLoc.Y);
						}
						else
							MousePos = FVector2D(MousePos.X, BestLoc.Y);
					}
				}
				else
				{
					MousePos = FVector2D(MousePos.X, BestLoc.Y);
				}
			}
			else
			{
				if (LastCorner != INDEX_NONE)
				{
					IObject *pCorner = BuildingSystem->GetObject(LastCorner);
					if (pCorner)
					{
						FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
						FVector2D Vec = MousePos - Location;
						if (std::abs(Vec.X) < ObjectSnapTol)
						{
							MousePos = FVector2D(Location.X, MousePos.Y);
						}
						if (std::abs(Vec.Y) < ObjectSnapTol)
						{
							MousePos = FVector2D(MousePos.X, Location.Y);
						}
					}
				}
			}
		for (int i = 0; i < CutAreaPos_Tarray.Num(); ++i)
		{
			FVector2D Vec = MousePos - CutAreaPos_Tarray[i].Loc;
			if (std::abs(Vec.X) < ObjectSnapTol)
			{
				MousePos = FVector2D(CutAreaPos_Tarray[i].Loc.X, MousePos.Y);
			}
			if (std::abs(Vec.Y) < ObjectSnapTol)
			{
				MousePos = FVector2D(MousePos.X, CutAreaPos_Tarray[i].Loc.Y);
			}
		}
	}
	return MousePos;
}

void UWallBuildSystem::CutAreaFailed()
{
	FirstWallID = LastWallID = LastCorner = INDEX_NONE;
	bIsCanCutArea = bIsCutAreaDown = false;
	CurrentTempVirtualLoc.Empty();
	ClearAllCornerData();
	UpdataCornerUMGData();
	CutAreaPos_Tarray.Empty();
	TempVirtualLoc.Empty();
	ShowAllCornerUMG();
}

void UWallBuildSystem::CutArea(const int32 &WallID, const FVector2D &Loc)
{
	/*if (BuildingSystem)
	{
		if (FirstWallID == INDEX_NONE && WallID != INDEX_NONE)
		{
			UBuildingData *Data = BuildingSystem->GetData(WallID);
			if (Data)
			{
				EObjectType Type = (EObjectType)Data->GetObjectType();
				if (Type == EObjectType::ECorner || Type == EObjectType::ESolidWall)
				{
					FirstWallID = WallID;
					bIsCanCutArea = true;
				}
				else
					if (Type == EObjectType::EDoorHole || Type == EObjectType::EWindow)
					{
						ShowToastDrawWall();
						CutAreaFailed();
						ShowAllCornerUMG();
					}
			}
		}
		else
			if (WallID != INDEX_NONE)
			{

				UBuildingData *Data = BuildingSystem->GetData(WallID);
				if (Data)
				{
					EObjectType Type = (EObjectType)Data->GetObjectType();
					if (Type == EObjectType::ECorner || Type == EObjectType::ESolidWall)
					{
						LastWallID = WallID;
						bIsCutAreaDown = true;
					}
					else
						if (Type == EObjectType::EDoorHole || Type == EObjectType::EWindow)
						{
							ShowToastDrawWall();
							CutAreaFailed();
							ShowAllCornerUMG();
						}
				}
			}
		AddVirtualWallCorner(Loc);
	}*/
	int32 Local_WallID = WallID;
	FVector2D Local_Loc = Loc;
	if (WallID == INDEX_NONE)
	{
		BuildingSystem->CutAreaSnap(Loc, -1, Local_Loc, Local_WallID, WallThickness);
	}
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (!Local_DRProjData)
		return;
	if (FirstWallID == INDEX_NONE && Local_WallID != INDEX_NONE)
	{
		UDataAdapter* Local_Adapter = Local_DRProjData->GetAdapter(Local_WallID);
		if (Local_Adapter)
		{
			if (Local_Adapter->GetObjectType() == EDR_ObjectType::EDR_Corner || Local_Adapter->GetObjectType() == EDR_ObjectType::EDR_SolidWall)
			{
				FirstWallID = Local_WallID;
				bIsCanCutArea = true;
			}
			else if (Local_Adapter->GetObjectType() == EDR_ObjectType::EDR_DoorHole || Local_Adapter->GetObjectType() == EDR_ObjectType::EDR_Window)
			{
				ShowToastDrawWall();
				CutAreaFailed();
				ShowAllCornerUMG();
			}
		}
	}
	else if (Local_WallID != INDEX_NONE)
	{
		UDataAdapter* Local_Adapter = Local_DRProjData->GetAdapter(Local_WallID);
		if (Local_Adapter)
		{
			if (Local_Adapter->GetObjectType() == EDR_ObjectType::EDR_Corner || Local_Adapter->GetObjectType() == EDR_ObjectType::EDR_SolidWall)
			{
				LastWallID = Local_WallID;
				bIsCutAreaDown = true;
			}
			else if (Local_Adapter->GetObjectType() == EDR_ObjectType::EDR_DoorHole || Local_Adapter->GetObjectType() == EDR_ObjectType::EDR_Window)
			{
				ShowToastDrawWall();
				CutAreaFailed();
				ShowAllCornerUMG();
			}
		}
	}
	AddVirtualWallCorner(Loc);
}

void UWallBuildSystem::CancelCutArea()
{
	if (BuildingSystem)
	{
		if (CutAreaPos_Tarray.Num() > 0)
		{
			int num = WallCorners.Num() - 1;
			if (num > 0)
			{
				WallCorners[num]->ShowOutterCircle(false);
				WallCorners[num]->ShowInnerCircel(false);
				WallCorners.RemoveAt(num);
			}
			BuildingSystem->DeleteObject(CutAreaPos_Tarray[CutAreaPos_Tarray.Num() - 1].ObjectId);
			if (CutAreaPos_Tarray.Num() - 1 > 0)
				LastCorner = CutAreaPos_Tarray[CutAreaPos_Tarray.Num() - 2].ObjectId;
			else
				LastCorner = INDEX_NONE;
			CutAreaPos_Tarray.RemoveAt(CutAreaPos_Tarray.Num() - 1);
		}
	}
}

TArray<int32> UWallBuildSystem::AddVirtualWall(int32 StartCorner, int32 EndCornner)
{
	TArray<int32> WallIDs;
	if (BuildingSystem)
	{
		WallIDs = BuildingSystem->AddVirtualWall(StartCorner, EndCornner);
	}
	return WallIDs;
}

int32 UWallBuildSystem::AddWindow(int32 WallID, const FVector2D &Location, float zPos, float Width, float Height, const FString &WindowResID, const FString &SillsResID)
{
	int32 num = INDEX_NONE;
	if (BuildingSystem)
	{
		num = BuildingSystem->AddWindow(WallID, Location, zPos, Width, Height, TCHAR_TO_ANSI(*WindowResID), TCHAR_TO_ANSI(*SillsResID));
	}
	return num;
}

int32 UWallBuildSystem::AddDoor(int32 WallID, const FVector2D &Location, float zPos, float Width, float Height)
{
	int32 num = INDEX_NONE;
	if (BuildingSystem)
	{
		num = BuildingSystem->AddDoor(WallID, Location, Width, Height, zPos);
	}
	return num;
}

void  UWallBuildSystem::GetWallVectorToView(int32 WallID, FVector2D &P0, FVector2D &P1, FVector2D &Right)
{
	if (BuildingSystem)
	{
		BuildingSystem->GetWallVector(WallID, P0, P1, Right);
		FVector StartPos = FVector(P0, 2800);
		FVector EndPos = FVector(P1, 2800);
		GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(StartPos, P0);
		GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(EndPos, P1);
	}
}

bool UWallBuildSystem::IsLastCornerFreeAndClear()
{
	if (BuildingSystem && LastCorner == INDEX_NONE)
	{
		if (FirstWallID != INDEX_NONE)
			FirstWallID = INDEX_NONE;
		else
		{
			bIsCanCutArea = bIsCutAreaDown = false;
			CutAreaFailed();
			return true;
		}
	}
	return false;
}

void UWallBuildSystem::ShowAllCornerUMG()
{
	for (int i = 0; i < WallCorners.Num(); ++i)
	{
		if (WallCorners[i])
		{
			WallCorners[i]->ShowOutterCircle(true);
			WallCorners[i]->ShowInnerCircel(false);
		}
	}
}

void UWallBuildSystem::DeleteSlectActor(int32 objectID, bool IsDeleteWallPlane, bool bIsBillingBoolean)
{
	if (objectID != INDEX_NONE && BuildingSystem)
	{
		UBuildingData* Data = BuildingSystem->GetData(objectID);
		if (Data)
		{
			EObjectType InClass = (EObjectType)Data->GetObjectType();
			if (InClass == EObjectType::ESolidWall /*|| InClass == EObjectType::EVirtualWal*/)
			{
				if (BooleanBase.Num() > 0)
				{
					for (int i = 0; i < BooleanBase.Num(); i++)
					{
						if (BooleanBase[i])
						{
							if (IsSetThisWall(objectID, FVector2D(BooleanBase[i]->DeleteHoleInfo.Loc)))
							{
								BooleanBase[i]->RemoveFromParent();
								BooleanBase[i] = nullptr;
							}
						}
					}	
				}

				if (IsDeleteWallPlane)
				{
					AWallPlaneBase** _WallPlane = WallPlaneMap.Find(objectID);
					if (_WallPlane)
					{
						AWallPlaneBase* WallPlane = *_WallPlane;
						if (WallPlane)
						{
							if (bIsBillingBoolean&&GameInst&&GameInst->DrawHouse_Type == EDrawHouseType::ChaigaiDrawType)
							{

								UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
								if (Local_DRProjData)
								{
									UDRSolidWallAdapter* Local_SolidWallAdapter = nullptr;
									Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(objectID));
									if (Local_SolidWallAdapter)
									{
										if (Local_SolidWallAdapter->GetSolidWallType() == ESolidWallType::EDROriginalWall)
										{
											WallPlane->SetWallPlaneMaterial(EWallType::NewWall, true);
											WallPlane->AddDeleteWall();
											TArray<int32> HoleIDs= Local_SolidWallAdapter->GetHoles();
											for (int i = 0; i < HoleIDs.Num(); ++i)
											{
												UObject** ObjPtr=Local_DRProjData->DoorHoleMap.Find(HoleIDs[i]);
												if (ObjPtr)
												{
													UObject* Obj = *ObjPtr;
													if (Obj)
													{
														AWall_Boolean_Base* BooleanActor = Cast<AWall_Boolean_Base>(Obj);
														if (BooleanActor)
														{
															BooleanActor->DeleteHoleInfo=CalculateRemoveHoleInfo(HoleIDs[i]);
															continue;
														}
													}
												}
												ObjPtr = Local_DRProjData->WindowHoleMap.Find(HoleIDs[i]);
												if (ObjPtr)
												{
													UObject* Obj = *ObjPtr;
													if (Obj)
													{
														AWall_Boolean_Base* BooleanActor = Cast<AWall_Boolean_Base>(Obj);
														if (BooleanActor)
														{
															BooleanActor->DeleteHoleInfo = CalculateRemoveHoleInfo(HoleIDs[i]);
														}
													}
												}
											}
										}
										else
										{
											WallPlane->Destroy();
											WallPlane = nullptr;
											WallPlaneMap.Remove(objectID);
										}
									}
								}
							}
							else
							{
								WallPlane->Destroy();
								WallPlane = nullptr;
								WallPlaneMap.Remove(objectID);
							}
						}
					}
				}
				int P0 = Data->GetInt("P0");
				int P1 = Data->GetInt("P1");
				int32 *pConnectedWalls = nullptr;
				bool flag1 = false;
				bool flag2 = false;
				int num = 0;
				num = BuildingSystem->GetConnectWalls(P0, pConnectedWalls);
				if (num == 1)
				{
					flag1 = true;
				}
				num = BuildingSystem->GetConnectWalls(P1, pConnectedWalls);
				if (num == 1)
				{
					flag2 = true;
				}
				if (WallCorners.Num() > 0)
				{
					for (int i = WallCorners.Num() - 1; i >= 0; --i)
					{
						if (WallCorners[i] &&
							(
							(flag1 && WallCorners[i]->CornerProperty.ObjectId == P0)
								|| (flag2 && WallCorners[i]->CornerProperty.ObjectId == P1)
								)
							)
						{
							WallCorners[i]->ShowInnerCircel(false);
							WallCorners[i]->ShowOutterCircle(false);
							WallCorners[i]->Destroy();
							WallCorners[i] = nullptr;
							WallCorners.RemoveAt(i);
						}
					}
				}
				BuildingSystem->DeleteObject(objectID);
				if (flag1)
				{
					BuildingSystem->DeleteObject(P0);
				}
				if (flag2)
				{
					BuildingSystem->DeleteObject(P1);
				}
				ClearSlectCornner();
			}
			else
				if (InClass == EObjectType::ECorner)
				{
					if (BooleanBase.Num() > 0)
					{
						for (int i = 0; i < BooleanBase.Num(); i++)
						{
							if (BooleanBase[i])
							{
								if (IsSetThisWall(objectID, FVector2D(BooleanBase[i]->DeleteHoleInfo.Loc)))
								{
									BooleanBase[i]->RemoveFromParent();
									BooleanBase[i] = nullptr;
								}
							}
						}
					}

					if (BuildingSystem->IsFree(objectID))
					{
						BuildingSystem->DeleteObject(objectID);
						for (int i = 0; i < WallCorners.Num(); ++i)
						{
							if (WallCorners[i] && WallCorners[i]->CornerProperty.ObjectId == objectID)
							{
								WallCorners[i]->ShowInnerCircel(false);
								WallCorners[i]->ShowOutterCircle(false);
								WallCorners.RemoveAt(i);
								break;
							}
						}
						for (int i = 0; i < ShowCorners.Num(); ++i)
						{
							if (ShowCorners[i] == nullptr)
							{
								ShowCorners.RemoveAt(i);
								break;
							}
						}
					}
				}
		}
	}
}

void UWallBuildSystem::ComputePolygonS(int32 objectID, float & OutS)
{
	if (objectID != INDEX_NONE && BuildingSystem)
	{
		TArray<FVector2D> TPolygons;
		int num = INDEX_NONE;
		num = BuildingSystem->GetPolygon(objectID, TPolygons, true);
		OutS = 0;
		if (TPolygons.Num() > 2)
		{
			for (int i = 0; i < TPolygons.Num(); ++i)
			{
				OutS += FVector2D::CrossProduct(TPolygons[i], TPolygons[(i + 1) % TPolygons.Num()]);
			}
		}
		OutS /= 2;
	}
}

void UWallBuildSystem::ReadyClearAllActor()
{
	if (BuildingSystem)
	{
		AllWallArray.Empty();
		AllCornerArray.Empty();
		IObject** ppOutObject = nullptr;
		EObjectType InClass = EObjectType::ESolidWall;
		int num = 0;
		num = BuildingSystem->GetAllObjects(ppOutObject, InClass);
		if (num > 0)
		{
			for (int i = 0; i < num; ++i)
			{
				if (ppOutObject[i])
					AllWallArray.Add(ppOutObject[i]->GetID());
			}
		}
		num = 0;
		InClass = EObjectType::ECorner;
		ppOutObject = nullptr;
		num = BuildingSystem->GetAllObjects(ppOutObject, InClass);
		if (num > 0)
		{
			for (int i = 0; i < num; ++i)
			{
				if (ppOutObject[i])
					AllCornerArray.Add(ppOutObject[i]->GetID());
			}
		}
	}
}

void UWallBuildSystem::ClearAllActor()
{
	if (BuildingSystem)
	{
		UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
		if (projectDataManager == nullptr) {
			return;
		}

		TMap<int32, UObject*> solidWallList = projectDataManager->SolidWallMap;
		for (const TPair<int32, UObject*>& currentPair : solidWallList)
		{
			BuildingSystem->DeleteObject(currentPair.Key);
		}

		for (int i = 0; i < AllCornerArray.Num(); ++i)
		{
			BuildingSystem->DeleteObject(AllCornerArray[i]);
		}
		BuildingSystem->Build();
		if (WallCorners.Num() > 0)
		{
			for (int i = 0; i < WallCorners.Num(); ++i)
			{
				if (WallCorners[i])
				{
					WallCorners[i]->ShowInnerCircel(false);
					WallCorners[i]->ShowOutterCircle(false);
					WallCorners[i]->Destroy();
					WallCorners[i] = nullptr;
				}
			}
			WallCorners.Empty();
		}
		for (TMap<int32, AWallPlaneBase*>::TIterator It(WallPlaneMap); It; ++It)
		{
			if (It.Value() != nullptr)
			{
				It.Value()->Destroy();
				It.Value() = nullptr;
			}
		}
		TArray<int32> EdgeIDs;
		GetAllObject(EdgeIDs, EObjectType::EEdge, false);
		for (int i = 0; i < EdgeIDs.Num(); ++i)
		{
			BuildingSystem->DeleteObject(EdgeIDs[i]);
		}
		ClearFreeCorner();
		WallPlaneMap.Empty();
		AllVirtualLoc.Empty();
		ShowCorners.Empty();
		LastCorner = INDEX_NONE;
		TenmpID = 0;
		for (int i = 0; i < DeletedWallPlaneArry.Num(); ++i)
		{
			if (DeletedWallPlaneArry[i] != nullptr)
			{
				DeletedWallPlaneArry[i]->Destroy();
				DeletedWallPlaneArry[i] = nullptr;
			}
		}
		DeletedWallPlaneArry.Empty();


		//..Destroy All House Component 
		TArray<AActor*> OutActor;
		UGameplayStatics::GetAllActorsOfClass(this, AComponentManagerActor::StaticClass(), OutActor);
		for (int i = 0; i < OutActor.Num(); ++i)
		{
			AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(OutActor[i]);
			if (ComponentManager)
			{
				ComponentManager->ManualDestroyHouseComponent();
			}

		}

		TArray<UUserWidget*> FoundWidgets;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets, UBooleanBase::StaticClass());
		for (int i = 0; i < FoundWidgets.Num(); ++i)
		{
			if (FoundWidgets[i])
			{
				UBooleanBase* BooleanUMG = Cast<UBooleanBase>(FoundWidgets[i]);
				if (BooleanUMG)
				{
					BooleanUMG->RemoveFromParent();
				}
			}
		}
	}
}

bool UWallBuildSystem::GetWallBorderPos(int32 objectID, TArray<FVector>&WallNodes, float &ZPos)
{
	if (BuildingSystem)
	{
		BuildingSystem->GetWallBorderLines(objectID, WallNodes, ZPos);
	}
	return false;
}

void UWallBuildSystem::GetWallHole(UBooleanBase* booleanActor, const FVector2D& Deltavector, const bool IsFirst, int32 objectID, int32 WallID, const float CurWidth, FVector2D &Loc, float& Width, FVector2D &StartLoc, FVector2D &EndLoc, float &Angle, FVector2D& Size, bool& Isaway, float &Thickness)
{
	float ZPos;
	bool bFlip;
	FVector2D Right, Forward;
	if (BuildingSystem&&WallID != INDEX_NONE)
	{
		Isaway = false;
		UBuildingData *WallData = BuildingSystem->GetData(WallID);
		if (WallData)
		{
			Width = CurWidth;
			Thickness = 0;
			float ThickRight, ThickLeft;
			ThickRight = WallData->GetFloat("ThickRight");
			ThickLeft = WallData->GetFloat("ThickLeft");
			Thickness = ThickRight + ThickLeft;
			int P0 = WallData->GetInt("P0");
			int P1 = WallData->GetInt("P1");
			IObject *CornerData1 = BuildingSystem->GetObject(P0);
			IObject *CornerData2 = BuildingSystem->GetObject(P1);
			TArray<FVector> WallNodes;
			BuildingSystem->GetWallBorderLines(WallID, WallNodes, ZPos);
			ZPos = WallData->GetFloat("Height");
			if (CornerData1&&CornerData2)
			{
				FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
				FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
				FVector2D Temp = StartCornerPos + FVector2D(0, 100);
				float theta = atan2(Temp.X - StartCornerPos.X, Temp.Y - StartCornerPos.Y) - atan2(EndCornerPos.X - StartCornerPos.X, EndCornerPos.Y - StartCornerPos.Y);
				if (theta > PI)
					theta -= 2 * PI;
				if (theta < -PI)
					theta += 2 * PI;
				Angle = theta * 180.0 / PI;
				FVector2D RightTemp = (EndCornerPos - StartCornerPos).GetRotated(90);
				RightTemp.Normalize();
				Right = FVector2D(RightTemp.Y, RightTemp.X);
				FVector2D ForwardTemp = (EndCornerPos - StartCornerPos).GetSafeNormal();
				Forward = FVector2D(ForwardTemp.Y, ForwardTemp.X);
				FVector StartPos;
				FVector EndPos;
				UDRFunLibrary::CalculateBooleanMaxMinLoction(WallNodes, StartPos, EndPos);
				if (objectID != INDEX_NONE)
				{
					UBuildingData *HoleData = BuildingSystem->GetData(objectID);
					if (HoleData)
					{
						bFlip = HoleData->GetBool("bFlip");
						Width = HoleData->GetFloat("Width");
						Loc = FVector2D(HoleData->GetVector("Location"));
						Loc = FVector2D(FMath::ClosestPointOnSegment(FVector(Loc, WallNodes[0].Z), StartPos + FVector(ForwardTemp* (Width / 2 + 5), 0), EndPos - FVector(ForwardTemp* (Width / 2 + 5), 0)));
						/*if (Loc != TempHoleLoc)
						{
							int32 CornerID = HoleData->GetInt("CornerID");
							if (CornerID != INDEX_NONE)
							{
								UBuildingData *PinCornerData = BuildingSystem->GetData(CornerID);
								if (PinCornerData)
								{
									BuildingSystem->Move(CornerID, Loc - TempHoleLoc);
								}
							}
						}*/
					}
				}
				else
				{
					if (bMoving)
					{
						Loc = KeepHolesPosition(WallID, booleanActor->DistLength);
					}
					else
					{
						FVector MouseWorldPos, MouseDir;
						FVector2D SMouse = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
						GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(SMouse.X, SMouse.Y, MouseWorldPos, MouseDir);
						//GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseWorldPos, MouseDir);
						MouseWorldPos.Z = WallNodes[0].Z;
						FVector2D TempLoc = FVector2D(FMath::ClosestPointOnSegment(MouseWorldPos + FVector(Deltavector, 0), StartPos + FVector(ForwardTemp* (Width / 2 + 5), 0), (EndPos - FVector(ForwardTemp* (Width / 2 + 5), 0))));
						if (booleanActor)
							booleanActor->Deltavector = TempLoc - FVector2D(MouseWorldPos);
						if (IsFirst)
						{
							FVector2D SceenPos;
							FVector TempDirSize = FVector(Width, 0, 0);
							FVector2D TempSceenDirSize;
							FVector ZeroTemp = FVector(0, 0, 0);
							FVector2D TempZeroSceen;
							GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(ZeroTemp, TempZeroSceen);
							GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(TempDirSize, TempSceenDirSize);
							GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(TempLoc, MouseWorldPos.Z), SceenPos);
							if ((SMouse - SceenPos).Size() > 1.2*(TempSceenDirSize - TempZeroSceen).Size())
							{
								/*UKismetSystemLibrary::DrawDebugString(GetWorld(),FVector(MouseWorldPos.X, MouseWorldPos.Y,500.f), MouseWorldPos.ToString(),(AActor*)this,FLinearColor(1.f,0.f,0.f,1.f),0.2f);
								UKismetSystemLibrary::DrawDebugString(GetWorld(), FVector(MouseWorldPos.X, MouseWorldPos.Y, 500.f), MouseWorldPos.ToString(), (AActor*)this, FLinearColor(1.f, 0.f, 0.f, 1.f), 0.2f);
								UKismetSystemLibrary::DrawDebugString(GetWorld(), FVector(0, 0, 500.f), FString::SanitizeFloat(aaaaa), (AActor*)this, FLinearColor(1.f, 0.f, 0.f, 1.f), 0.2f);*/
								Isaway = true;
							}
						}
						Loc = TempLoc;
					}
				}
				if (ThickRight != ThickLeft)
				{
					Loc += RightTemp * (ThickRight - ThickLeft) / 2;
				}
				if (objectID != INDEX_NONE && booleanActor&&booleanActor->bISet)
				{
					UBuildingData *HoleData = BuildingSystem->GetData(objectID);
					if (HoleData)
					{
						FVector2D TempHoleLoc = FVector2D(HoleData->GetVector("Location"));
						if (Loc != TempHoleLoc && FVector2D::Distance(TempHoleLoc, Loc) > 5.0f)
						{
							int32 CornerID = HoleData->GetInt("CornerID");
							if (CornerID != INDEX_NONE)
							{
								UBuildingData *PinCornerData = BuildingSystem->GetData(CornerID);
								if (PinCornerData)
								{
									BuildingSystem->Move(CornerID, Loc - TempHoleLoc);
								}
							}
							/*AWall_Boolean_Base* HoleActor = FindBooleanActorByHoleID(objectID);
							if (HoleActor)
							{
								HoleActor->SetWidth(CurWidth-2);
							}*/
						}
					}
				}
				FVector2D Point1, Point2, Point3, Point4;
				Point1 = Loc + ForwardTemp * Width / 2 + RightTemp * Thickness / 2;
				Point2 = Loc - ForwardTemp * Width / 2 - RightTemp * Thickness / 2;
				Point3 = Loc - ForwardTemp * Width / 2 + RightTemp * Thickness / 2;
				Point4 = Loc + ForwardTemp * Width / 2 - RightTemp * Thickness / 2;
				StartLoc = Loc - ForwardTemp * Width / 2;
				EndLoc = Loc + ForwardTemp * Width / 2;
				GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Loc, 282), Loc);
				GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Point1, 282), Point1);
				GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Point2, 282), Point2);
				GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Point3, 282), Point3);
				GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Point4, 282), Point4);
				GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(StartLoc, 282), StartLoc);
				GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(EndLoc, 282), EndLoc);
				Size.X = (Point3 - Point1).Size();
				Size.Y = (Point3 - Point2).Size();
			}

		}
	}
	else
		if (BuildingSystem&&WallID == INDEX_NONE)
		{
			Width = CurWidth;
			Thickness = 20;
			Angle = 0;
			FVector MouseWorldPos, MouseDir;
			//GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseWorldPos, MouseDir);
			//GetWorld()->GetFirstPlayerController()->GetMousePosition(MouseWorldPos.X, MouseWorldPos.Y);
			FVector2D SMouse = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
			if (GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(SMouse.X, SMouse.Y, MouseWorldPos, MouseDir))
			{
				int a;
				a = 2;
			}
			Loc = FVector2D(MouseWorldPos);
			FVector2D ForwardTemp = FVector2D(0, 1);
			FVector2D RightTemp = FVector2D(0, 1).GetRotated(90);
			FVector2D Point1, Point2, Point3, Point4;
			Point1 = Loc + ForwardTemp * Width / 2 + RightTemp * Thickness / 2;
			Point2 = Loc - ForwardTemp * Width / 2 - RightTemp * Thickness / 2;
			Point3 = Loc - ForwardTemp * Width / 2 + RightTemp * Thickness / 2;
			Point4 = Loc + ForwardTemp * Width / 2 - RightTemp * Thickness / 2;
			StartLoc = Loc - ForwardTemp * Width / 2;
			EndLoc = Loc + ForwardTemp * Width / 2;
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Loc, 282), Loc);
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Point1, 282), Point1);
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Point2, 282), Point2);
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Point3, 282), Point3);
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(Point4, 282), Point4);
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(StartLoc, 282), StartLoc);
			GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(EndLoc, 282), EndLoc);
			Size.X = (Point3 - Point1).Size();
			Size.Y = (Point3 - Point2).Size();
		}

}

FVector2D UWallBuildSystem::GetRotated2D(const FVector2D & Vec, const float &Angle)
{
	return Vec.GetRotated(Angle);
}

FVector2D UWallBuildSystem::GetBestHolePostion(int32 WallID, float Width, UBooleanBase* booleanActor)
{
	FVector2D Postion = FVector2D::ZeroVector;
	if (WallID != INDEX_NONE && BuildingSystem)
	{
		UBuildingData *WallData = BuildingSystem->GetData(WallID);
		if (WallData)
		{
			int P0 = WallData->GetInt("P0");
			int P1 = WallData->GetInt("P1");
			IObject *CornerData1 = BuildingSystem->GetObject(P0);
			IObject *CornerData2 = BuildingSystem->GetObject(P1);
			if (CornerData1&&CornerData2)
			{
				FVector2D Start = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
				FVector2D End = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
				FVector ForwardTemp = FVector((End - Start).GetSafeNormal(), 0);
				float ZPos;
				TArray<FVector> WallNodes;
				BuildingSystem->GetWallBorderLines(WallID, WallNodes, ZPos);
				FVector StartCornerPos;
				FVector EndCornerPos;
				UDRFunLibrary::CalculateBooleanMaxMinLoction(WallNodes, StartCornerPos, EndCornerPos);
				/*FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
				FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
				FVector2D ForwardTemp = (EndCornerPos - StartCornerPos).GetSafeNormal();*/
				FVector MouseWorldPos, MouseDir;
				GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseWorldPos, MouseDir);
				MouseWorldPos.Z = 0;
				if (booleanActor)
				{
					MouseWorldPos += FVector(booleanActor->Deltavector, 0);
				}
				Postion = FVector2D(FMath::ClosestPointOnSegment(MouseWorldPos, StartCornerPos + ForwardTemp * (Width / 2 + 5), (EndCornerPos - ForwardTemp * (Width / 2 + 5))));
			}
		}
	}
	return Postion;
}

void UWallBuildSystem::MoveBooleanActor(int32 HoleID, FVector2D Loc)
{
	if (BuildingSystem&&HoleID != INDEX_NONE)
	{
		UBuildingData *HoleData = BuildingSystem->GetData(HoleID);
		if (HoleData)
		{
			int32 WallID = HoleData->GetInt("WallID");
			int32 CornerID = HoleData->GetInt("CornerID");
			if (CornerID != INDEX_NONE)
			{
				UBuildingData *PinCornerData = BuildingSystem->GetData(CornerID);
				if (PinCornerData)
				{
					FVector2D Pos1 = FVector2D(PinCornerData->GetVector("Location"));
					UBuildingData *WallData = BuildingSystem->GetData(WallID);
					if (WallData)
					{
						FVector dir = WallData->GetVector("Forward");
						Loc = FVector2D(UKismetMathLibrary::FindClosestPointOnLine(FVector(Loc, 0), FVector(Pos1, 0), dir));
						BuildingSystem->Move(CornerID, Loc - Pos1);
						//PinCornerData->SetVector2D("Location", Loc);
						FVector Pos2 = PinCornerData->GetVector("Location");
						int aaa;
						aaa = 2;
					}
				}

			}

		}
	}
}

void UWallBuildSystem::GetAllObject(TArray<int32>& TobjectIDArray, EObjectType InClass, bool bIncludeDeriveType)
{
	if (BuildingSystem)
	{
		TobjectIDArray.Empty();
		int num = 0;
		IObject** ppOutObject = nullptr;
		num = BuildingSystem->GetAllObjects(ppOutObject, InClass, bIncludeDeriveType);
		if (InClass == EObjectType::ERoom || InClass == EObjectType::EPolygonArea)
		{
			for (int i = 0; i < num; ++i)
			{
				if (ppOutObject[i])
				{
					TArray<FVector2D> Pos;
					int32 n = BuildingSystem->GetPolygon(ppOutObject[i]->GetID(), Pos, bIncludeDeriveType);
					if (n > 2)
					{
						float S = 0;
						ComputePolygonS(ppOutObject[i]->GetID(), S);
						if (S >= 5000)
							TobjectIDArray.Add(ppOutObject[i]->GetID());
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < num; ++i)
			{
				if (ppOutObject[i])
				{
					TobjectIDArray.Add(ppOutObject[i]->GetID());
				}
			}
		}
	}
}

void UWallBuildSystem::GetAllHoleObject(TArray<int32>& TobjectIDArray)
{
	TobjectIDArray.Empty();
	TArray<int32> WindowHoles;
	GetAllObject(WindowHoles, EObjectType::EWindow);
	TArray<int32> DoorHoles;
	GetAllObject(DoorHoles, EObjectType::EDoorHole);
	TobjectIDArray.Append(WindowHoles);
	TobjectIDArray.Append(DoorHoles);
}

void UWallBuildSystem::UpdataCornerUMGData()
{
	if (BuildingSystem)
	{
		TArray<int32> CornerID;
		GetAllObject(CornerID, EObjectType::ECorner, false);
		WallCorners.Empty();
		ShowCorners.Empty();
		for (int i = 0; i < CornerID.Num(); ++i)
		{
			IObject* CornerData = BuildingSystem->GetObject(CornerID[i]);
			//UBuildingData *CornerData = BuildingSystem->GetData(CornerID[i]);
			if (CornerData)
			{
				FVector2D Loc = ToVector2D(CornerData->GetPropertyValue("Location").Vec2Value());
				AddNewCorner(Loc, CornerID[i]);
			}
		}
	}
}

float UWallBuildSystem::GetWallRotate(FVector2D StartPos, FVector2D EndPos)
{
	FVector2D Temp = StartPos + FVector2D(0, 100);
	float theta = atan2(Temp.X - StartPos.X, Temp.Y - StartPos.Y) - atan2(EndPos.X - StartPos.X, EndPos.Y - StartPos.Y);
	if (theta > PI)
		theta -= 2 * PI;
	if (theta < -PI)
		theta += 2 * PI;
	return theta * 180.0 / PI;
}


void UWallBuildSystem::SearchWallsBySegPoints(TArray<int32>& OutWalls, FVector2D StartPos, FVector2D EndPos)
{

	using FSegment2D = FExactDataConv::FP2CSegment2D;
	FSegment2D GivenSegment(FExactDataConv::Vector2D2CgPointConv(StartPos), FExactDataConv::Vector2D2CgPointConv(EndPos));
	const double AreaSquareTol = WALL_AREA_SNAP_TOL * WALL_AREA_SNAP_TOL;
	FVector2D Start2End = EndPos - StartPos;
	FVector2D OriginalVec = Start2End;
	Start2End.Normalize();
	TArray<int32> AllWalls;
	if (BuildingSystem)
	{
		GetAllObject(AllWalls, ESolidWall);
		for (auto &WallID : AllWalls)
		{
			UBuildingData* WallData = BuildingSystem->GetData(WallID);
			int P0 = WallData->GetInt("P0");
			int P1 = WallData->GetInt("P1");
			IObject *CornerData1 = BuildingSystem->GetObject(P0);
			IObject *CornerData2 = BuildingSystem->GetObject(P1);
			FVector2D WallStart = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());;
			FVector2D WallEnd = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());

			FVector2D ForwardVec = WallData->GetVector2D("Forward");
			FVector2D ForwardVecNormal = ForwardVec.GetSafeNormal();
			FVector2D RightVec = WallData->GetVector2D("Right");
			FVector2D RightVecNormal = RightVec.GetSafeNormal();

			float ThickLeft = WallData->GetFloat("ThickLeft");
			float ThickRight = WallData->GetFloat("ThickRight");
			float WallThick = ThickLeft + ThickRight;
			float WallThickSquare = WallThick * WallThick;
			FVector2D WallStartLeft = WallStart - RightVecNormal * ThickLeft;
			FVector2D WallStartRight = WallStart + RightVecNormal * ThickRight;

			FVector2D WallEndLeft = WallEnd - RightVecNormal * ThickLeft;
			FVector2D WallEndRight = WallEnd + RightVecNormal * ThickRight;

			if (FVector::CrossProduct(FVector(Start2End, 0), FVector(ForwardVecNormal, 0)).Size() < SmallPositiveNumber)
			{
				//float PointToLeftSquare  = FMath::PointDistToSegmentSquared(FVector(0.5*(StartPos+EndPos), 0), FVector(WallStartLeft,0),FVector(WallEndLeft,0));
				//float PointToRightSquare = FMath::PointDistToSegmentSquared(FVector(0.5*(StartPos+EndPos), 0), FVector(WallStartRight, 0), FVector(WallEndRight, 0));
				FSegment2D WallLeftSegment(FExactDataConv::Vector2D2CgPointConv(WallStartLeft),
					FExactDataConv::Vector2D2CgPointConv(WallEndLeft));
				FSegment2D WallRightSegment(FExactDataConv::Vector2D2CgPointConv(WallStartRight),
					FExactDataConv::Vector2D2CgPointConv(WallEndRight));
				float LeftWallDist = CGAL::squared_distance(WallLeftSegment, GivenSegment);
				float RightWallDist = CGAL::squared_distance(WallRightSegment, GivenSegment);
				if (((LeftWallDist < WallThickSquare) && (RightWallDist < WallThickSquare)) ||
					(std::min(LeftWallDist, RightWallDist) < AreaSquareTol))
				{
					OutWalls.Add(WallID);
				}
			}
		}
	}
}

void UWallBuildSystem::AddVirtualWallUMG(const int32 &WallID)
{
	/*if (BuildingSystem)
	{
		UBuildingData* WallData = BuildingSystem->GetData(WallID);
		if (WallData)
		{
			int P0 = WallData->GetInt("P0");
			int P1 = WallData->GetInt("P1");
			IObject *CornerData1 = BuildingSystem->GetObject(P0);
			IObject *CornerData2 = BuildingSystem->GetObject(P1);
			IObject *WallObj = BuildingSystem->GetObject(WallID);
			if (CornerData1&&CornerData2&&WallObj)
			{
				IValue *  _V = WallObj->FindValue("GroundName");
				if (_V)
				{
					int Temp = _V->IntValue();

					WallData->TenmpID = Temp;
				}
				FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
				FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
				FVirtualWallInfo info;
				info.WallID = WallID;
				info.SartPos = info.OriginalSartPos =StartCornerPos;
				info.EndtPos = info.OriginalEndtPos =EndCornerPos;
				info.AssociationID = WallData->TenmpID;
				IValue *v = nullptr;
				v = BuildingSystem->GetProperty(P0, "Walls");
				if (v)
				{
					kArray<int> WallIDs = v->IntArrayValue();
					for (int i = 0;i < WallIDs.size();++i)
					{
						UBuildingData* Data = BuildingSystem->GetData(WallIDs[i]);
						if (Data)
						{
							EObjectType Type = (EObjectType)Data->GetObjectType();
							if (Type == EObjectType::ESolidWall)
							{
								TArray<FVector> WallNodes;
								float Zpos;
								BuildingSystem->GetWallBorderLines(WallIDs[i], WallNodes, Zpos);
								if (WallNodes.Num() == 6)
								{
									FVector2D a1 = StartCornerPos;
									FVector2D a2 = EndCornerPos;
									FVector2D b1 = FVector2D(WallNodes[1]);
									FVector2D b2 = FVector2D(WallNodes[2]);
									if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
									{

										info.SartPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
										break;
									}
									else
									{
										b1 = FVector2D(WallNodes[4]);
										b2 = FVector2D(WallNodes[5]);
										if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
										{
											info.SartPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
											break;
										}
									}

								}

							}
						}
					}
				}
				v = BuildingSystem->GetProperty(P1, "Walls");
				if (v)
				{
					kArray<int> WallIDs = v->IntArrayValue();
					for (int i = 0;i < WallIDs.size();++i)
					{
						UBuildingData* Data = BuildingSystem->GetData(WallIDs[i]);
						if (Data)
						{
							EObjectType Type = (EObjectType)Data->GetObjectType();
							if (Type == EObjectType::ESolidWall)
							{
								TArray<FVector> WallNodes;
								float Zpos;
								BuildingSystem->GetWallBorderLines(WallIDs[i], WallNodes, Zpos);
								if (WallNodes.Num() == 6)
								{
									FVector2D a1 = StartCornerPos;
									FVector2D a2 = EndCornerPos;
									FVector2D b1 = FVector2D(WallNodes[1]);
									FVector2D b2 = FVector2D(WallNodes[2]);
									if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
									{

										info.EndtPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
										break;
									}
									else
									{
										b1 = FVector2D(WallNodes[4]);
										b2 = FVector2D(WallNodes[5]);
										if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
										{
											info.EndtPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
											break;
										}
									}

								}

							}
						}
					}
				}
				AllVirtualLoc.Add(info);
			}
		}
	}*/
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (Local_DRProjData)
	{
		FilterEdgeInfo();
		if (FilterShortEdge(WallID))
			return;
		UDRVirtualWallAdapter* Local_VirtualWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(WallID));
		if (Local_VirtualWallAdapter)
		{
			FVirtualWallInfo Local_VriWallInfo;
			FVector2D Local_StartCornerPos = Local_VirtualWallAdapter->GetOriginalSartPos();
			FVector2D Local_EndCornerPos = Local_VirtualWallAdapter->GetOriginalEndtPos();
			Local_VriWallInfo.WallID = WallID;
			Local_VriWallInfo.SartPos = Local_VriWallInfo.OriginalSartPos = Local_StartCornerPos;
			Local_VriWallInfo.EndtPos = Local_VriWallInfo.OriginalEndtPos = Local_EndCornerPos;
			Local_VriWallInfo.AssociationID = Local_VirtualWallAdapter->GetAssociationID();
			FVector2D TempPos;
			BuildingSystem->CutAreaSnap(Local_StartCornerPos, -1, TempPos, Local_VriWallInfo.SnapObj0);
			BuildingSystem->CutAreaSnap(Local_EndCornerPos, -1, TempPos, Local_VriWallInfo.SnapObj1);

			int32 Index = 0;
			TArray<int32> Local_AllWall = Local_VirtualWallAdapter->GetAllWallByP0();
			for (Index = 0; Index < Local_AllWall.Num(); ++Index)
			{
				UDRSolidWallAdapter* Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(Local_AllWall[Index]));
				if (Local_SolidWallAdapter)
				{
					FDRWallPosition Local_WallPosition = Local_SolidWallAdapter->GetWallPositionData();
					if (IsTwoLineSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.LeftStartPos), FVector2D(Local_WallPosition.LeftEndPos)))
					{
						Local_VriWallInfo.SartPos = GetwoSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.LeftStartPos), FVector2D(Local_WallPosition.LeftEndPos));
						break;
					}
					else if (IsTwoLineSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.RightEndPos), FVector2D(Local_WallPosition.RightStartPos)))
					{
						Local_VriWallInfo.SartPos = GetwoSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.RightEndPos), FVector2D(Local_WallPosition.RightStartPos));
						break;
					}
				}
			}
			Local_AllWall = Local_VirtualWallAdapter->GetAllWallByP1();
			for (Index = 0; Index < Local_AllWall.Num(); ++Index)
			{
				UDRSolidWallAdapter* Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(Local_AllWall[Index]));
				if (Local_SolidWallAdapter)
				{
					FDRWallPosition Local_WallPosition = Local_SolidWallAdapter->GetWallPositionData();
					if (IsTwoLineSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.LeftStartPos), FVector2D(Local_WallPosition.LeftEndPos)))
					{
						Local_VriWallInfo.EndtPos = GetwoSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.LeftStartPos), FVector2D(Local_WallPosition.LeftEndPos));
						break;
					}
					else if (IsTwoLineSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.RightEndPos), FVector2D(Local_WallPosition.RightStartPos)))
					{
						Local_VriWallInfo.EndtPos = GetwoSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.RightEndPos), FVector2D(Local_WallPosition.RightStartPos));
						break;
					}
				}
			}
			AllVirtualLoc.Add(Local_VriWallInfo);
		}
	}
}

void UWallBuildSystem::UpdateVirtualWallUMG(const int32 &WallID)
{
	/*if (BuildingSystem)
	{
		UBuildingData* WallData = BuildingSystem->GetData(WallID);
		if (WallData)
		{
			int P0 = WallData->GetInt("P0");
			int P1 = WallData->GetInt("P1");
			IObject *CornerData1 = BuildingSystem->GetObject(P0);
			IObject *CornerData2 = BuildingSystem->GetObject(P1);
			if (CornerData1&&CornerData2)
			{
				FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
				FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
				FVirtualWallInfo info;
				info.WallID = WallID;
				info.SartPos = info.OriginalSartPos = StartCornerPos;
				info.EndtPos = info.OriginalEndtPos = EndCornerPos;
				IValue *v = nullptr;
				v = BuildingSystem->GetProperty(P0, "Walls");
				if (v)
				{
					kArray<int> WallIDs = v->IntArrayValue();
					for (int i = 0;i < WallIDs.size();++i)
					{
						UBuildingData* Data = BuildingSystem->GetData(WallIDs[i]);
						if (Data)
						{
							EObjectType Type = (EObjectType)Data->GetObjectType();
							if (Type == EObjectType::ESolidWall)
							{
								TArray<FVector> WallNodes;
								float Zpos;
								BuildingSystem->GetWallBorderLines(WallIDs[i], WallNodes, Zpos);
								if (WallNodes.Num() == 6)
								{
									FVector2D a1 = StartCornerPos;
									FVector2D a2 = EndCornerPos;
									FVector2D b1 = FVector2D(WallNodes[1]);
									FVector2D b2 = FVector2D(WallNodes[2]);
									if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
									{

										info.SartPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
										break;
									}
									else
									{
										b1 = FVector2D(WallNodes[4]);
										b2 = FVector2D(WallNodes[5]);
										if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
										{
											info.SartPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
											break;
										}
									}

								}

							}
						}
					}
				}
				v = BuildingSystem->GetProperty(P1, "Walls");
				if (v)
				{
					kArray<int> WallIDs = v->IntArrayValue();
					for (int i = 0;i < WallIDs.size();++i)
					{
						UBuildingData* Data = BuildingSystem->GetData(WallIDs[i]);
						if (Data)
						{
							EObjectType Type = (EObjectType)Data->GetObjectType();
							if (Type == EObjectType::ESolidWall)
							{
								TArray<FVector> WallNodes;
								float Zpos;
								BuildingSystem->GetWallBorderLines(WallIDs[i], WallNodes, Zpos);
								if (WallNodes.Num() == 6)
								{
									FVector2D a1 = StartCornerPos;
									FVector2D a2 = EndCornerPos;
									FVector2D b1 = FVector2D(WallNodes[1]);
									FVector2D b2 = FVector2D(WallNodes[2]);
									if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
									{

										info.EndtPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
										break;
									}
									else
									{
										b1 = FVector2D(WallNodes[4]);
										b2 = FVector2D(WallNodes[5]);
										if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
										{
											info.EndtPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
											break;
										}
									}

								}

							}
						}
					}
				}
				for (int i = 0;i < AllVirtualLoc.Num();++i)
				{
					if (AllVirtualLoc[i].WallID == WallID)
					{
						AllVirtualLoc[i].SartPos = info.SartPos;
						AllVirtualLoc[i].EndtPos = info.EndtPos;
						AllVirtualLoc[i].OriginalSartPos = info.OriginalSartPos;
						AllVirtualLoc[i].OriginalEndtPos = info.OriginalEndtPos;
						break;
					}
				}
			}
		}
	}*/

	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (Local_DRProjData)
	{
		UDRVirtualWallAdapter* Local_VirtualWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(WallID));
		if (Local_VirtualWallAdapter)
		{
			FVirtualWallInfo Local_VriWallInfo;
			int32 P0 = Local_VirtualWallAdapter->GetP0();
			int32 P1 = Local_VirtualWallAdapter->GetP1();

			FVector2D Local_StartCornerPos = Local_VirtualWallAdapter->GetOriginalSartPos();
			FVector2D Local_EndCornerPos = Local_VirtualWallAdapter->GetOriginalEndtPos();
			Local_VriWallInfo.WallID = WallID;
			Local_VriWallInfo.SartPos = Local_VriWallInfo.OriginalSartPos = Local_StartCornerPos;
			Local_VriWallInfo.EndtPos = Local_VriWallInfo.OriginalEndtPos = Local_EndCornerPos;
			Local_VriWallInfo.AssociationID = Local_VirtualWallAdapter->GetAssociationID();

			/*int32 Index = 0;
			TArray<int32> Local_AllWall = Local_VirtualWallAdapter->GetAllWallByP0();
			for (Index = 0; Index < Local_AllWall.Num(); ++Index)
			{
				UDRSolidWallAdapter* Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(Local_AllWall[Index]));
				if (Local_SolidWallAdapter)
				{
					FDRWallPosition Local_WallPosition = Local_SolidWallAdapter->GetWallPositionData();
					if (IsTwoLineSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.LeftStartPos), FVector2D(Local_WallPosition.LeftEndPos)))
					{
						Local_VriWallInfo.SartPos = GetwoSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.LeftStartPos), FVector2D(Local_WallPosition.LeftEndPos));
						break;
					}
					else if (IsTwoLineSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.RightEndPos), FVector2D(Local_WallPosition.RightStartPos)))
					{
						Local_VriWallInfo.SartPos = GetwoSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.RightEndPos), FVector2D(Local_WallPosition.RightStartPos));
						break;
					}
				}
			}
			Local_AllWall = Local_VirtualWallAdapter->GetAllWallByP1();
			for (Index = 0; Index < Local_AllWall.Num(); ++Index)
			{
				UDRSolidWallAdapter* Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(Local_AllWall[Index]));
				if (Local_SolidWallAdapter)
				{
					FDRWallPosition Local_WallPosition = Local_SolidWallAdapter->GetWallPositionData();
					if (IsTwoLineSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.LeftStartPos), FVector2D(Local_WallPosition.LeftEndPos)))
					{
						Local_VriWallInfo.EndtPos = GetwoSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.LeftStartPos), FVector2D(Local_WallPosition.LeftEndPos));
						break;
					}
					else if (IsTwoLineSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.RightEndPos), FVector2D(Local_WallPosition.RightStartPos)))
					{
						Local_VriWallInfo.EndtPos = GetwoSegmentsIntersect(Local_StartCornerPos, Local_EndCornerPos, FVector2D(Local_WallPosition.RightEndPos), FVector2D(Local_WallPosition.RightStartPos));
						break;
					}
				}
			}*/
			for (FVirtualWallInfo& CurrentVirtualWallInfo : AllVirtualLoc)
			{
				if (CurrentVirtualWallInfo.WallID == Local_VriWallInfo.WallID)
				{
					CurrentVirtualWallInfo.SartPos = Local_VriWallInfo.SartPos;
					CurrentVirtualWallInfo.EndtPos = Local_VriWallInfo.EndtPos;
					CurrentVirtualWallInfo.AssociationID = Local_VriWallInfo.AssociationID;
					CurrentVirtualWallInfo.OriginalSartPos = Local_VriWallInfo.OriginalSartPos;
					CurrentVirtualWallInfo.OriginalEndtPos = Local_VriWallInfo.OriginalEndtPos;
				}
			}
		}
	}
}

void UWallBuildSystem::DeleteVirtualWallUMG(const int32 &WallID)
{
	for (int i = 0; i < AllVirtualLoc.Num(); ++i)
	{
		if (AllVirtualLoc[i].WallID == WallID)
		{
			AllVirtualLoc.RemoveAt(i);
			break;
		}
	}
	BuildRoom();
}

void UWallBuildSystem::ViewFilter(bool IsShow)
{
	//for (TMap<int32, AWallPlaneBase*>::TIterator It(WallPlaneMap); It; ++It)
	//{
	//	if (It.Value())
	//	{
	//		It.Value()->SetActorHiddenInGame(IsShow);
	//	}
	//}
}

void UWallBuildSystem::ClearAllCornerData()
{
	for (int i = 0; i < WallCorners.Num(); ++i)
	{
		if (WallCorners[i])
		{
			WallCorners[i]->ShowInnerCircel(false);
			WallCorners[i]->ShowOutterCircle(false);
			WallCorners[i]->Destroy();
			WallCorners[i] = nullptr;
		}
	}
	WallCorners.Empty();
}

void UWallBuildSystem::DeleteHole(const int32& HoleID)
{
	if (BuildingSystem)
	{
		if (HoleID != INDEX_NONE)
		{
			BuildingSystem->DeleteObject(HoleID);
		}
	}
}

void UWallBuildSystem::ShowWallUI(bool IsShow)
{
	for (TMap<int32, AWallPlaneBase*>::TIterator It(WallPlaneMap); It; ++It)
	{
		if (It.Value())
		{
			It.Value()->SetActorHiddenInGame(!IsShow);
			It.Value()->SetRuler(false);
		}
	}
}

void UWallBuildSystem::ShowAllWallUI()
{
	for (TMap<int32, AWallPlaneBase*>::TIterator It(WallPlaneMap); It; ++It)
	{
		if (It.Value())
		{
			It.Value()->SetActorHiddenInGame(false);
			It.Value()->SetRuler(true);
		}
	}
}


void  UWallBuildSystem::TestCornerLoc(TArray<FVector> &Corners, TArray<int32> &CornerID)
{
	if (BuildingSystem)
	{
		Corners.Empty();
		CornerID.Empty();
		GetAllObject(CornerID, EObjectType::ECorner, false);
		for (int i = 0; i < CornerID.Num(); ++i)
		{
			UBuildingData *data = BuildingSystem->GetData(CornerID[i]);
			if (data)
			{
				Corners.Add(FVector(data->GetVector2D("Location"), 0));
			}
		}

	}
}

void UWallBuildSystem::ReturnSnapCornerID(const bool IsCalculate, const FVector2D &StartPos, const FVector2D &EndPos, TMap<FVector2D, FVector2D>TailMapDirs, FVector2D& OutStartPos, FVector2D&OutEndPos)
{
	bool flag1 = true;
	bool flag2 = true;
	int32 TargetStartWall = 0, TargetEndWall = 0;
	float TargetStartWallThick = 10.0, TargetEndWallThick = 10.0;
	if (BuildingSystem)
	{
		OutStartPos = OutEndPos = FVector2D::ZeroVector;
		TArray<int32> CornerID;
		GetAllObject(CornerID, EObjectType::ECorner, false);
		for (int i = 0; i < CornerID.Num(); ++i)
		{
			UBuildingData* CornerData = BuildingSystem->GetData(CornerID[i]);
			if (CornerData)
			{
				FVector2D Temp = CornerData->GetVector2D("Location");
				if (flag1)
				{
					if ((Temp - StartPos).Size() <= 20)
					{
						OutStartPos = Temp;
						flag1 = false;

					}
				}
				if (flag2)
				{
					if ((Temp - EndPos).Size() <= 20)
					{
						OutEndPos = Temp;
						flag2 = false;
					}
				}
			}
		}
		TArray<int32> WallID;
		GetAllObject(WallID, EObjectType::ESolidWall, false);
		for (int i = 0; i < WallID.Num(); ++i)
		{
			IObject *WallObj = BuildingSystem->GetObject(WallID[i]);
			if (WallObj)
			{
				int a;
				a = 2;
			}
			UBuildingData* WallData = BuildingSystem->GetData(WallID[i]);
			if (WallData)
			{
				TArray<FVector>WallNodes;
				float HeightPos = 0;
				GetWallBorderPos(WallData->GetID(), WallNodes, HeightPos);
				if (WallNodes.Num() == 6)
				{
					FVector Loc1 = WallNodes[0];
					FVector Loc2 = WallNodes[3];
					FVector dir = Loc2 - Loc1;
					if (flag1)
					{
						FVector Temp = UKismetMathLibrary::FindClosestPointOnLine(FVector(StartPos, 0), Loc1, dir);
						if ((FVector2D(Temp) - StartPos).Size() < 20)
						{
							if ((abs((Temp.X - Loc1.X) * (Loc2.Y - Loc1.Y) - ((Loc2.X - Loc1.X) * (Temp.Y - Loc1.Y))) <= 0.1f)
								&& std::min(Loc1.X, Loc2.X) <= Temp.X && Temp.X <= std::max(Loc1.X, Loc2.X)
								&& std::min(Loc1.Y, Loc2.Y) <= Temp.Y && Temp.Y <= std::max(Loc1.Y, Loc2.Y))
							{
								OutStartPos = FVector2D(Temp);
								flag1 = false;
								TargetStartWall = i;
								TargetStartWallThick = 0.5*(WallData->GetFloat("ThickRight") + WallData->GetFloat("ThickLeft"));
							}
						}
					}
					if (flag2)
					{
						FVector Temp = UKismetMathLibrary::FindClosestPointOnLine(FVector(EndPos, 0), Loc1, dir);
						if ((FVector2D(Temp) - EndPos).Size() < 20)
						{
							if (abs((Temp.X - Loc1.X) * (Loc2.Y - Loc1.Y) - (Loc2.X - Loc1.X) * (Temp.Y - Loc1.Y)) <= 0.1f
								&& std::min(Loc1.X, Loc2.X) <= Temp.X && Temp.X <= std::max(Loc1.X, Loc2.X)
								&& std::min(Loc1.Y, Loc2.Y) <= Temp.Y && Temp.Y <= std::max(Loc1.Y, Loc2.Y))
							{
								OutEndPos = FVector2D(Temp);
								flag2 = false;
								TargetEndWall = i;
								TargetStartWallThick = 0.5*(WallData->GetFloat("ThickRight") + WallData->GetFloat("ThickLeft"));
							}
						}
					}
				}
			}
		}

	}
	if (flag1)
	{
		OutStartPos = StartPos;
	}
	if (flag2)
	{
		OutEndPos = EndPos;
	}
	if (IsCalculate && (!flag1) && (!flag2))
	{
		FVector2D OutVec = OutEndPos - OutStartPos;
		FVector2D OriginalVec = EndPos - StartPos;
		OutVec = OutVec.GetSafeNormal();
		OriginalVec = OriginalVec.GetSafeNormal();
		float JudgeParallel = abs(OutVec.X*OriginalVec.Y - OriginalVec.X*OutVec.Y);
		if (JudgeParallel > 0.01)
		{
			float DisToSeg1 = FMath::PointDistToLine(FVector(OutStartPos, 0), FVector(OriginalVec, 0), FVector(StartPos, 0));
			float DisToSeg2 = FMath::PointDistToLine(FVector(OutEndPos, 0), FVector(OriginalVec, 0), FVector(EndPos, 0));

			if ((DisToSeg1 >= 0.1) && (DisToSeg2 >= 0.1))
			{
				FVector2D StartToOutStartDir = OutStartPos - StartPos;
				FVector2D EndToOutEndDir = OutEndPos - EndPos;
				StartToOutStartDir = StartToOutStartDir.GetSafeNormal();
				EndToOutEndDir = EndToOutEndDir.GetSafeNormal();

				float Value1 = FVector2D::DotProduct(StartToOutStartDir, TailMapDirs[StartPos]);
				float Value2 = FVector2D::DotProduct(EndToOutEndDir, TailMapDirs[EndPos]);

				if ((Value1 > 0) && (Value2 > 0))
				{
					if (DisToSeg1 >= DisToSeg2)
					{
						FVector2D TempNode = EndPos + DisToSeg1 * TailMapDirs[EndPos];
						OutEndPos = TempNode + 0.5*TargetEndWallThick*OriginalVec;
					}
					else
					{

						FVector2D TempNode = StartPos + DisToSeg2 * TailMapDirs[StartPos];
						OutStartPos = TempNode - 0.5*TargetStartWallThick*OriginalVec;
					}
				}
				else if ((Value1 > 0) && (Value2 < 0))
				{
					FVector2D EndToOutStart = EndPos - OutStartPos;
					float TempH = abs(FVector2D::DotProduct(EndToOutStart, TailMapDirs[StartPos]));
					FVector2D TempNode = EndPos + TempH * TailMapDirs[EndPos];
					OutEndPos = TempNode + 0.5*TargetEndWallThick*OriginalVec;
				}
				else if ((Value1 < 0) && (Value2 > 0))
				{
					FVector2D StartToOutEnd = StartPos - OutEndPos;
					float TempH = abs(FVector2D::DotProduct(StartToOutEnd, TailMapDirs[EndPos]));
					FVector2D TempNode = StartPos + TempH * TailMapDirs[StartPos];
					OutStartPos = TempNode - 0.5*TargetStartWallThick*OriginalVec;
				}
			}
			else if ((DisToSeg1 < 0.1) && (DisToSeg2 >= 0.1))
			{
				FVector2D TempNode1 = StartPos + DisToSeg2 * TailMapDirs[StartPos];
				FVector2D OutStartPos1 = TempNode1 - 0.5*TargetStartWallThick*OriginalVec;
				FVector2D TempNode2 = StartPos - DisToSeg2 * TailMapDirs[StartPos];
				FVector2D OutStartPos2 = TempNode2 - 0.5*TargetStartWallThick*OriginalVec;
				FVector2D TempVec1 = OutStartPos1 - OutEndPos;
				TempVec1 = TempVec1.GetSafeNormal();
				float TempJudgeValue = FVector2D::DotProduct(TempVec1, TailMapDirs[StartPos]);
				if (abs(TempJudgeValue) <= 0.01)
				{
					OutStartPos = OutStartPos1;
				}
				else
				{
					OutStartPos = OutStartPos2;
				}
			}
			else if ((DisToSeg1 >= 0.1) && (DisToSeg2 < 0.1))
			{
				FVector2D TempNode1 = EndPos + DisToSeg1 * TailMapDirs[EndPos];
				FVector2D OutEndPos1 = TempNode1 + 0.5*TargetEndWallThick*OriginalVec;
				FVector2D TempNode2 = EndPos - DisToSeg1 * TailMapDirs[EndPos];
				FVector2D OutEndPos2 = TempNode2 + 0.5*TargetEndWallThick*OriginalVec;
				FVector2D TempVec = OutStartPos - OutEndPos1;
				TempVec = TempVec.GetSafeNormal();
				float TempJudgeValue = FVector2D::DotProduct(TempVec, TailMapDirs[EndPos]);
				if (abs(TempJudgeValue) <= 0.01)
				{
					OutEndPos = OutEndPos1;
				}
				else
				{
					OutEndPos = OutEndPos2;
				}

			}
		}

	}
}

const float UWallBuildSystem::GetWallThickness(const int32& WallID)
{
	float Thickness = 0.0f;
	if (BuildingSystem&&WallID != INDEX_NONE)
	{
		UBuildingData *HoleData = BuildingSystem->GetData(WallID);
		if (HoleData)
		{
			int ID = HoleData->GetInt("WallID");
			UBuildingData *WallData = BuildingSystem->GetData(ID);
			if (WallData)
			{
				float ThickRight, ThickLeft;
				ThickRight = WallData->GetFloat("ThickRight");
				ThickLeft = WallData->GetFloat("ThickLeft");
				Thickness = ThickRight + ThickLeft;
			}
		}
	}
	return Thickness;
}

FVector2D UWallBuildSystem::FindBestLoc(const FVector2D & Loc, const int32& ObjID, const float& Width, float& OutWidth, bool& IsSuccess)
{
	FVector2D vec = Loc;
	IsSuccess = false;
	if (BuildingSystem&&ObjID != INDEX_NONE)
	{
		OutWidth = Width;
		UBuildingData *WallData = BuildingSystem->GetData(ObjID);
		if (WallData)
		{
			TArray<FVector> WallNodes;
			float ZPos;
			BuildingSystem->GetWallBorderLines(ObjID, WallNodes, ZPos);
			if (WallNodes.Num() == 6)
			{
				FVector StartPos;
				FVector EndPos;
				FVector2D ForwardTemp = FVector2D(WallData->GetVector("Forward"));
				UDRFunLibrary::CalculateBooleanMaxMinLoction(WallNodes, StartPos, EndPos);
				if ((EndPos - StartPos).Size() < 30)
				{
					IsSuccess = false;
					return vec;
				}
				else
					if ((EndPos - StartPos).Size() - 10 < Width)
					{
						OutWidth = (EndPos - StartPos).Size() - 10;
					}
				vec = FVector2D(FMath::ClosestPointOnSegment(FVector(Loc, 0), StartPos + FVector(ForwardTemp* (OutWidth / 2 + 5), 0), EndPos - FVector(ForwardTemp* (OutWidth / 2 + 5), 0)));
			}
		}
		IsSuccess = true;
	}
	return vec;
}

bool UWallBuildSystem::CreateVirtualWall(const TArray<FVector2D> & PolygonFirst, const TArray<FVector2D> & PolygonSecond, TArray<FVector2D>& VirtualWallPs, TMap<FVector2D, FVector2D>&TailPsMapDir)
{
	return FPolygonAlg::CreateVirtualWallWithDir(PolygonFirst, PolygonSecond, VirtualWallPs, TailPsMapDir);
}

void UWallBuildSystem::WorldPosToView(const TArray<FVector2D>& WorldPos, TArray<FVector2D>& ViewPos)
{
	ViewPos.Empty();
	for (int i = 0; i < WorldPos.Num(); ++i)
	{
		FVector2D WidgetPos;
		GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(WorldPos[i], 0), WidgetPos);
		ViewPos.Add(WidgetPos);
	}
}

void UWallBuildSystem::TwoLineIntersection(const FVector2D&StartPos, const FVector2D& EndPos, const FVector2D& Temp, FVector2D& _OutPos)
{
	FVector2D Temp1 = FVector2D::ZeroVector;
	FVector2D Temp2 = FVector2D::ZeroVector;
	Temp1 = FVector2D(StartPos.X, EndPos.Y);
	Temp2 = FVector2D(EndPos.X, StartPos.Y);
	if ((Temp1 - Temp).Size() < (Temp2 - Temp).Size())
	{
		_OutPos = Temp1;
	}
	else
		_OutPos = Temp2;
}

bool UWallBuildSystem::JudgePointInPolygon(const TArray<FVector> InPnts, const FVector2D ToJudgePoint)
{
	TArray<FVector2D> Pnts2D;
	for (int i = 0; i < InPnts.Num(); ++i)
	{
		Pnts2D.Add(FVector2D(InPnts[i]));
	}
	return FPolygonAlg::JudgePointInPolygon(Pnts2D, ToJudgePoint);
}

void UWallBuildSystem::VirWallWorldPosToView(const FVector2D &StartPos, const FVector2D &EndPos, FVector2D &_OutStartPos, FVector2D &_Out_EndPos)
{
	GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(StartPos, 100), _OutStartPos);
	GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(FVector(EndPos, 100), _Out_EndPos);
}

bool UWallBuildSystem::IsTwoLineSegmentsIntersect(const FVector2D& a1, const FVector2D& a2, const FVector2D& b1, const FVector2D& b2)
{
	if (((a1.X > a2.X ? a1.X : a2.X) < (b1.X < b2.X ? b1.X : b2.X) ||
		(a1.Y > a2.Y ? a1.Y : a2.Y) < (b1.Y < b2.Y ? b1.Y : b2.Y)) ||
		((b1.X > b2.X ? b1.X : b2.X) < (a1.X < a2.X ? a1.X : a2.X) ||
		(b1.Y > b2.Y ? b1.Y : b2.Y) < (a1.Y < a2.Y ? a1.Y : a2.Y)))
	{
		float dis = FPolygonAlg::GetShortestdistanceBySegments(a1, a2, b1, b2);
		if (dis > 0.001f)
			return false;
	}
	float Number[4];
	Number[0] = FVector2D::CrossProduct((a1 - b1).GetSafeNormal(), (a2 - b1).GetSafeNormal());
	Number[1] = FVector2D::CrossProduct((a1 - b2).GetSafeNormal(), (a2 - b2).GetSafeNormal());
	Number[2] = FVector2D::CrossProduct((b1 - a1).GetSafeNormal(), (b2 - a1).GetSafeNormal());
	Number[3] = FVector2D::CrossProduct((b1 - a2).GetSafeNormal(), (b2 - a2).GetSafeNormal());
	for (int i = 0; i < 4; ++i)
	{
		if (FMath::Abs(Number[i]) < 0.005f)
		{
			Number[i] = 0;
		}
	}
	if (Number[0] * Number[1] <= 0 && Number[2] * Number[3] <= 0)
	{
		return true;
	}
	return false;
}

FVector2D UWallBuildSystem::GetwoSegmentsIntersect(const FVector2D& a1, const FVector2D& a2, const FVector2D& b1, const FVector2D& b2)
{
	FVector2D base = b2 - b1;
	double d1 = std::abs(FVector2D::CrossProduct(base, a1 - b1));
	double  d2 = std::abs(FVector2D::CrossProduct(base, a2 - b1));
	if (0 == d1 && 0 == d2)
	{
		if (a1 == b1 || a1 == b2)
			return a1;
		if (a2 == b1 || a2 == b2)
			return a2;
	}
	double t = d1 / (d1 + d2);
	FVector2D temp = (a2 - a1)*t;
	return a1 + temp;
}

void UWallBuildSystem::LoadAllVirtualWall()
{
	/*if (BuildingSystem&&IsLoadVirWall())
	{
		TArray<AActor*> _RoomRes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomActor::StaticClass(), _RoomRes);
		if (_RoomRes.Num() > 1)
			for (int i = _RoomRes.Num() - 1; i > 0; --i)
			{
				TArray<FVector2D> SlotPos1;
				TArray<FVector2D> SlotPos2;
				for (int j = i - 1; j >= 0; --j)
				{
					ARoomActor * _Room1 = Cast<ARoomActor>(_RoomRes[i]);
					ARoomActor * _Room2 = Cast<ARoomActor>(_RoomRes[j]);
					if (_Room1&&_Room2)
					{
						if (_Room1->GetAreaVerList(SlotPos1) && _Room2->GetAreaVerList(SlotPos2))
						{
							TArray<FVector2D> VirtualWallPs;
							TMap<FVector2D, FVector2D> TailPsMapDir;
							if (CreateVirtualWall(SlotPos1, SlotPos2, VirtualWallPs, TailPsMapDir))
							{
								FVector2D OutStartPos;
								FVector2D OutEndPos;
								++TenmpID;
								if (VirtualWallPs.Num() == 2)
								{
									ReturnSnapCornerID(true, VirtualWallPs[0], VirtualWallPs[1], TailPsMapDir, OutStartPos, OutEndPos);
									int32 StartCorner = BuildingSystem->AddCorner(OutStartPos);
									int32 EndCorner = BuildingSystem->AddCorner(OutEndPos);
									TArray<int32> WallIDs;
									WallIDs = BuildingSystem->AddVirtualWall(StartCorner, EndCorner);
									for (int k = 0; k < WallIDs.Num(); ++k)
									{
										IValue & GroundName = UBuildingSystem::GetValueFactory()->Create(TenmpID);
										IObject * _ObjGround = BuildingSystem->GetObject(WallIDs[k]);
										if (_ObjGround && _ObjGround->GetType() == EObjectType::EVirtualWall)
										{
											_ObjGround->SetValue("GroundName", &GroundName);
										}
									}
								}
								else if (VirtualWallPs.Num() == 3)
								{
									FVector2D OutTempPos;
									ReturnSnapCornerID(false, VirtualWallPs[0], VirtualWallPs[2], TailPsMapDir, OutStartPos, OutEndPos);
									TwoLineIntersection(OutStartPos, OutEndPos, VirtualWallPs[1], OutTempPos);
									int32 StartCorner = BuildingSystem->AddCorner(OutStartPos);
									int32 TemptCorner = BuildingSystem->AddCorner(OutTempPos);
									int32 EndCorner = BuildingSystem->AddCorner(OutEndPos);
									TArray<int32> WallIDs;
									WallIDs = BuildingSystem->AddVirtualWall(StartCorner, TemptCorner);
									for (int k = 0; k < WallIDs.Num(); ++k)
									{
										IValue & GroundName = UBuildingSystem::GetValueFactory()->Create(TenmpID);
										IObject * _ObjGround = BuildingSystem->GetObject(WallIDs[k]);
										if (_ObjGround && _ObjGround->GetType() == EObjectType::EVirtualWall)
										{
											_ObjGround->SetValue("GroundName", &GroundName);
										}
									}
									WallIDs = BuildingSystem->AddVirtualWall(TemptCorner, EndCorner);
									for (int k = 0; k < WallIDs.Num(); ++k)
									{
										IValue & GroundName = UBuildingSystem::GetValueFactory()->Create(TenmpID);
										IObject * _ObjGround = BuildingSystem->GetObject(WallIDs[k]);
										if (_ObjGround && _ObjGround->GetType() == EObjectType::EVirtualWall)
										{
											_ObjGround->SetValue("GroundName", &GroundName);
										}
									}
								}
							}
						}
					}
				}
			}
	}*/

	if (!BuildingSystem)
		return;
	BuildingSystem->ForceUpdateSuit();
	BuildingSystem->SetConfigTolerance(2.f);
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (IsLoadVirWall() && Local_DRProjData)
	{
		TArray<FVector2DArrayStruct> Local_AllAreaPoints;
		GetAllAreaPoints(Local_AllAreaPoints);

		TArray<FVector2D> Local_SlotPos1, Local_SlotPos2;
		for (int32 Index = Local_AllAreaPoints.Num() - 1; Index > 0; --Index)
		{
			for (int32 IndexTwo = Index - 1; IndexTwo >= 0; --IndexTwo)
			{
				Local_SlotPos1 = Local_AllAreaPoints[Index].Vector2DArray;
				Local_SlotPos2 = Local_AllAreaPoints[IndexTwo].Vector2DArray;
				if (Local_SlotPos1.Num() >= 3 && Local_SlotPos2.Num() >= 3)
				{
					TArray<FVector2D> Local_VirtualWallPoints;
					TMap<FVector2D, FVector2D> Local_TailPsMapDir;
					if (CreateVirtualWall(Local_SlotPos1, Local_SlotPos2, Local_VirtualWallPoints, Local_TailPsMapDir))
					{
						FVector2D Local_OutStartPos, Local_OutEndPos, Local_OutTempPos, Local_TempPos;
						//int32 Local_CornerIDStart, Local_CornerIDEnd, Local_TempCornerID;
						UDRVirtualWallAdapter* Local_VirtualWallAdapter = nullptr;
						++TenmpID;
						if (Local_VirtualWallPoints.Num() == 2)
						{
							ReturnSnapCornerID(true, Local_VirtualWallPoints[0], Local_VirtualWallPoints[1], Local_TailPsMapDir, Local_OutStartPos, Local_OutEndPos);
							int32 SnapWallID_0 = -1;
							int32 SnapWallID_1 = -1;
							BuildingSystem->CutAreaSnap(Local_VirtualWallPoints[0], -1, Local_TempPos, SnapWallID_0);
							BuildingSystem->CutAreaSnap(Local_VirtualWallPoints[1], -1, Local_TempPos, SnapWallID_1);
							//TArray<int32> Local_VirtualWallIds = AddVirtualWall(Local_CornerIDStart, Local_CornerIDEnd);
							CalculateBestEdgeCornerPos(Local_VirtualWallPoints[0], Local_VirtualWallPoints[1], SnapWallID_0, SnapWallID_1);
							if (SnapWallID_0 == SnapWallID_1)
								continue;
							TArray<int32> Local_VirtualWallIds = AddEdge(Local_VirtualWallPoints[0], Local_VirtualWallPoints[1], SnapWallID_0, SnapWallID_1);
							for (int32 WallIndex = 0; WallIndex < Local_VirtualWallIds.Num(); ++WallIndex)
							{
								Local_VirtualWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(Local_VirtualWallIds[WallIndex]));
								if (Local_VirtualWallAdapter)
								{
									Local_VirtualWallAdapter->SetAssociationID(TenmpID);
								}
							}
						}
						else if (Local_VirtualWallPoints.Num() == 3)
						{
							ReturnSnapCornerID(false, Local_VirtualWallPoints[0], Local_VirtualWallPoints[2], Local_TailPsMapDir, Local_OutStartPos, Local_OutEndPos);
							TwoLineIntersection(Local_OutStartPos, Local_OutEndPos, Local_VirtualWallPoints[1], Local_OutTempPos);
							//Local_CornerIDStart = BuildingSystem->AddCorner(Local_OutStartPos);
							//Local_TempCornerID = BuildingSystem->AddCorner(Local_OutTempPos);
							//Local_CornerIDEnd = BuildingSystem->AddCorner(Local_OutEndPos);
							//TArray<int32> Local_VirtualWallIds = AddVirtualWall(Local_CornerIDStart, Local_TempCornerID);
							int32 SnapWallID_0 = -1;
							int32 SnapWallID_1 = -1;
							BuildingSystem->CutAreaSnap(Local_OutStartPos, -1, Local_TempPos, SnapWallID_0);
							BuildingSystem->CutAreaSnap(Local_OutTempPos, -1, Local_TempPos, SnapWallID_1);
							CalculateBestEdgeCornerPos(Local_OutStartPos, Local_OutTempPos, SnapWallID_0, SnapWallID_1);
							if (SnapWallID_0 == SnapWallID_1)
								continue;
							TArray<int32> Local_VirtualWallIds = AddEdge(Local_OutStartPos, Local_OutTempPos, SnapWallID_0, SnapWallID_1);
							for (int32 WallIndex = 0; WallIndex < Local_VirtualWallIds.Num(); ++WallIndex)
							{
								Local_VirtualWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(Local_VirtualWallIds[WallIndex]));
								if (Local_VirtualWallAdapter)
								{
									Local_VirtualWallAdapter->SetAssociationID(TenmpID);
								}
							}
							//Local_VirtualWallIds = AddVirtualWall(Local_TempCornerID, Local_CornerIDEnd);
							int32 SnapWallID_2 = -1;
							int32 SnapWallID_3 = -1;
							BuildingSystem->CutAreaSnap(Local_OutTempPos, -1, Local_TempPos, SnapWallID_2);
							BuildingSystem->CutAreaSnap(Local_OutEndPos, -1, Local_TempPos, SnapWallID_3);
							CalculateBestEdgeCornerPos(Local_OutTempPos, Local_OutEndPos, SnapWallID_2, SnapWallID_3);
							//UpdateEdgeCornerPos(Local_OutTempPos, Local_OutEndPos, SnapWallID_2, SnapWallID_3);
							if (SnapWallID_2 == SnapWallID_3)
								continue;
							Local_VirtualWallIds = AddEdge(Local_OutTempPos, Local_OutEndPos, SnapWallID_2, SnapWallID_3);
							for (int32 WallIndex = 0; WallIndex < Local_VirtualWallIds.Num(); ++WallIndex)
							{
								Local_VirtualWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(Local_VirtualWallIds[WallIndex]));
								if (Local_VirtualWallAdapter)
								{
									Local_VirtualWallAdapter->SetAssociationID(TenmpID);
								}
							}
						}
					}
				}
			}
		}
	}
}

bool UWallBuildSystem::OnlyGetAreaSlots(const int32& AreaID, TArray<FVector2D>& SlotPos)
{
	if (AreaID != INDEX_NONE && BuildingSystem)
	{
		SlotPos.Empty();
		int num = INDEX_NONE;
		num = BuildingSystem->GetPolygon(AreaID, SlotPos, true);
		if (num > 2)
		{
			return true;
		}

	}
	return false;
}

FSavedDataNode UWallBuildSystem::GetDefaultData()
{
	FSavedDataNode Result;
	Result.bIsDelete = DefaultWallProp.bIsDelete;
	Result.Type = DefaultWallProp.Type;
	//Result.bIsDelete = false;
	//Result.Type = EWallTypeCPP::Wall;

	for (FMaterialNode& Item : DefaultWallProp.WallNode.WallMaterial)
	{
		FMaterialNode DefaultWallMaterial;
		DefaultWallMaterial.ModelID = Item.ModelID;
		DefaultWallMaterial.ResID = Item.ResID;
		DefaultWallMaterial.Url = Item.Url;
		GetDefaultRoomClassAndCraft(Item.ModelID, DefaultWallMaterial.RoomClassID, DefaultWallMaterial.CraftID);
		//DefaultWallMaterial.CraftID = Item.CraftID;
		//DefaultWallMaterial.RoomClassID = 16;
		Result.WallNode.WallMaterial.Add(DefaultWallMaterial);
	}
	//DefaultWallMaterial.ModelID = 22314;
	//DefaultWallMaterial.ResID = "";
	//DefaultWallMaterial.Url = "";
	//DefaultWallMaterial.CraftID = 301;
	//DefaultWallMaterial.RoomClassID = 16;
	////DefaultWallMaterial.bIsProjectMat = false;
	////DefaultWallMaterial.Path = DEFAULT_WALL_MATERIAL;
	//Result.WallNode.WallMaterial.Init(DefaultWallMaterial, 7);


	for (FMaterialNode& TJXItem : DefaultWallProp.WallNode.TJXMat)
	{
		FMaterialNode TempTJXMat;
		TempTJXMat.ModelID = TJXItem.ModelID;
		TempTJXMat.ResID = TJXItem.ResID;
		TempTJXMat.Url = TJXItem.Url;
		GetDefaultRoomClassAndCraft(TJXItem.ModelID, TempTJXMat.RoomClassID, TempTJXMat.CraftID);
		//TempTJXMat.CraftID = TJXItem.CraftID;
		//TempTJXMat.RoomClassID = TJXItem.RoomClassID;
		Result.WallNode.TJXMat.Add(TempTJXMat);
	}
	//FMaterialNode DefaultAreaMaterial;
	////DefaultAreaMaterial.bIsProjectMat = false;
	////DefaultAreaMaterial.Path = DEFAULT_AREA_MATERIAL;
	//Result.WallNode.TJXMat.Init(DefaultAreaMaterial, 7);
	for (FMaterialNode& AreaItem : DefaultWallProp.WallNode.AreaMat)
	{
		FMaterialNode TempAreaMat;
		TempAreaMat.ModelID = AreaItem.ModelID;
		TempAreaMat.ResID = AreaItem.ResID;
		TempAreaMat.Url = AreaItem.Url;
		GetDefaultRoomClassAndCraft(AreaItem.ModelID, TempAreaMat.RoomClassID, TempAreaMat.CraftID);
		//TempAreaMat.CraftID = AreaItem.CraftID;
		//TempAreaMat.RoomClassID = AreaItem.RoomClassID;
		Result.WallNode.AreaMat.Add(TempAreaMat);
	}
	//Result.WallNode.AreaMat.Add(DefaultAreaMaterial);
	//Result.WallNode.AreaMat.Add(DefaultWallMaterial);
	//Result.WallNode.AreaMat.Add(DefaultWallMaterial);

	Result.WallNode.Color = DefaultWallProp.WallNode.Color;

	Result.WallNode.LeftThick = DefaultWallProp.WallNode.LeftThick;
	Result.WallNode.RightThick = DefaultWallProp.WallNode.RightThick;

	Result.WallNode.Height = DefaultWallProp.WallNode.Height;
	return Result;
}

void UWallBuildSystem::OnlyDelateVirWall(const TArray<int32>& VirWallIDS)
{
	/*if (BuildingSystem)
	{
		TArray<int32> AllVirWallIDS;
		GetAllObject(AllVirWallIDS, EObjectType::EVirtualWall, false);
		for (int i = 0;i < VirWallIDS.Num();++i)
		{
			UBuildingData * VirData = BuildingSystem->GetData(VirWallIDS[i]);
			if (VirData)
			{
				int Temp = VirData->TenmpID;
				for (int j = 0;j < AllVirWallIDS.Num();++j)
				{
					UBuildingData * Data = BuildingSystem->GetData(AllVirWallIDS[j]);
					if (Data)
					{
						int VirID = Data->TenmpID;
						if (VirID == Temp)
						{
							BuildingSystem->DeleteObject(AllVirWallIDS[j]);
						}
					}
				}
			}
		}
		ClearFreeCorner();
		ClearAllCornerData();
		UpdataCornerUMGData();
	}*/
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (Local_DRProjData)
	{
		int32 TempIndex = -1;
		TArray<int32> Local_AllVriWallObjId;
		TArray<int32> Local_DelAllVriWallObjId;
		UDRVirtualWallAdapter* Local_CurrentVirWallAdapter = nullptr;
		GetAllObject(Local_AllVriWallObjId, EObjectType::EEdge, false);
		for (int32 TempCurrentVirWallObjId : VirWallIDS)
		{
			Local_CurrentVirWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(TempCurrentVirWallObjId));
			if (Local_CurrentVirWallAdapter)
			{
				TempIndex = Local_CurrentVirWallAdapter->GetAssociationID();
				for (int32 CurrentVirWallObjId : Local_AllVriWallObjId)
				{
					Local_CurrentVirWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(CurrentVirWallObjId));
					if (Local_CurrentVirWallAdapter && TempIndex == Local_CurrentVirWallAdapter->GetAssociationID())
					{
						Local_DelAllVriWallObjId.AddUnique(Local_CurrentVirWallAdapter->GetObjID());
					}
				}
			}

		}
		for (int32 CurrentIndex : Local_DelAllVriWallObjId)
		{
			BuildingSystem->DeleteObject(CurrentIndex);
		}
		ClearFreeCorner();
		ClearAllCornerData();
		UpdataCornerUMGData();
		CollectRoomInfoAfterDelete();
	}
}

void UWallBuildSystem::SetWallIsClosed(const TArray<int32> WallIDS, bool IsClosed)
{
	if (WallIDS.Num() > 0 && BuildingSystem)
	{
		IObject * WallObj = BuildingSystem->GetObject(WallIDS[0]);
		if (WallObj)
		{
			IValue & BoolValue = UBuildingSystem::GetValueFactory()->Create(IsClosed);
			WallObj->SetPropertyValue("bRoomWall", &BoolValue);
			IValue *  _V = WallObj->FindValue("bRoomWall");
			if (_V)
			{
				bool ssssss = _V->BoolValue();
				ssssss = false;
			}
		}
	}
}

void UWallBuildSystem::ClearFreeCorner()
{
	TArray<int32>CornerIDs;
	GetAllObject(CornerIDs, EObjectType::ECorner, false);
	for (int i = 0; i < CornerIDs.Num(); ++i)
	{
		IObject *CornerObj = BuildingSystem->GetObject(CornerIDs[i]);
		if (CornerObj)
		{
			if (BuildingSystem->IsFree(CornerIDs[i]))
			{
				BuildingSystem->DeleteObject(CornerIDs[i]);
			}
		}
	}
}

bool UWallBuildSystem::IsLoadVirWall()
{
	/*UWorld *MyWorld = GetWorld();
	UWallBuildSystem *WallSystem = nullptr;
	if (MyWorld)
	{
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
		if (GameInst&&BuildingSystem)
		{
			TArray <FDRVirtualWallStruct> LoadStruct= GameInst->VirtualWallData;
			if (LoadStruct.Num() == 0)
			{
				return true;
			}
			else
			{
				for (int i = 0;i < LoadStruct.Num();++i)
				{
					TArray<FVector2D> Postions = LoadStruct[i].Position;
					if (Postions.Num() > 0 && Postions.Num() % 2 == 0)
					{
						++TenmpID;
						int num = Postions.Num() / 2;
						for (int j = 0;j < Postions.Num();j+=2)
						{
							int CornerIDStart=BuildingSystem->AddCorner(Postions[j]);
							int CornerIDEnd = BuildingSystem->AddCorner(Postions[j+1]);
							TArray<int32> WallIDs;
							WallIDs = AddVirtualWall(CornerIDStart, CornerIDEnd);
							for (int k = 0;k < WallIDs.Num();++k)
							{
								IValue & GroundName = UBuildingSystem::GetValueFactory()->Create(TenmpID);
								IObject * _ObjGround = BuildingSystem->GetObject(WallIDs[k]);
								if (_ObjGround)
								{
									_ObjGround->SetValue("GroundName", &GroundName);
								}
							}
						}
					}
				}
			}
		}


	}
	return false;*/

	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	UWorld* Local_World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);

	if (Local_World && Local_DRProjData)
	{
		UCEditorGameInstance* Local_EditorGameInstance = Cast<UCEditorGameInstance>(Local_World->GetGameInstance());
		if (Local_EditorGameInstance && BuildingSystem)
		{
			TArray <FDRVirtualWallStruct> LoadStruct = Local_EditorGameInstance->VirtualWallData;
			if (LoadStruct.Num() == 0)
			{
				return true;
			}
			else
			{
				for (int32 Index = 0; Index < LoadStruct.Num(); ++Index)
				{
					TArray<FVector2D> Local_Postions = LoadStruct[Index].Position;
					if (Local_Postions.Num() > 0 && Local_Postions.Num() % 2 == 0)
					{
						++TenmpID;
						FVector2D Temp = FVector2D(0, 0);
						for (int j = 0; j < Local_Postions.Num(); j += 2)
						{
							if (FVector2D::Distance(Local_Postions[j], Local_Postions[j + 1]) < 0.01)
								continue;
							int32 SnapObj1 = -1;
							int32 HitObj = -1;
							SnapObj1 = BuildingSystem->HitTest(Local_Postions[j]);
							HitObj = SnapObj1;
							IObject * HitObj_0 = BuildingSystem->GetObject(SnapObj1);
							int TempType_0 = -1;
							if (HitObj_0)
							{
								TempType_0 = HitObj_0->GetType();
								if (TempType_0 == EObjectType::ESolidWall || TempType_0 == EObjectType::ECorner)
								{
									float TempWallThickness = GetHitWallThickness(SnapObj1);
									if (TempType_0 == EObjectType::ECorner)
									{
										if (CheckIsWallCorner(HitObj))
										{
											UDRCornerAdapter* Local_CornerAdapter = Cast<UDRCornerAdapter>(Local_DRProjData->GetAdapter(HitObj));
											TArray<int32> Local_WallIDs = Local_CornerAdapter->GetWalls();
											FVector2D Local_TempPos0 = Local_Postions[j];
											FVector2D Local_TempPos1 = Local_Postions[j];
											int32 Local_SnapObj = SnapObj1;
											float Local_Length = 1000000.0f;
											for (int WallIndex = 0; WallIndex < Local_WallIDs.Num(); ++WallIndex)
											{
												Local_TempPos0 = Local_Postions[j];
												int32 Local_Snap = -1;
												int32 Local_TempSnap = Local_WallIDs[WallIndex];
												CalculateBestEdgeCornerPos(Local_TempPos0, Local_Postions[j + 1], Local_TempSnap, Local_Snap);
												if (Local_TempPos0 != Local_TempPos1 && FVector2D::Distance(Local_TempPos0, Local_Postions[j + 1]) <= Local_Length)
												{
													Local_Length = FVector2D::Distance(Local_TempPos0, Local_Postions[j + 1]);
													Local_TempPos1 = Local_TempPos0;
													Local_SnapObj = Local_TempSnap;
												}
											}
											SnapObj1 = Local_SnapObj;
											Local_Postions[j] = Local_TempPos1;
										}
										else
											SnapObj1 = -1;
									}
									else
										BuildingSystem->CutAreaSnap(Local_Postions[j], -1, Local_Postions[j], SnapObj1, 2.0f, 2.0f);
								}
								else
									BuildingSystem->CutAreaSnap(Local_Postions[j], -1, Local_Postions[j], SnapObj1, 2.0f, 2.0f);
							}
							else
								BuildingSystem->CutAreaSnap(Local_Postions[j], -1, Local_Postions[j], SnapObj1, 2.0f, 2.0f);
							int32 SnapObj2 = -1;
							HitObj = -1;
							SnapObj2 = BuildingSystem->HitTest(Local_Postions[j + 1]);
							HitObj = SnapObj2;
							IObject * HitObj_1 = BuildingSystem->GetObject(SnapObj2);
							int TempType_1 = -1;
							if (HitObj_1)
							{
								TempType_1 = HitObj_1->GetType();
								if (TempType_1 == EObjectType::ESolidWall || TempType_1 == EObjectType::ECorner)
								{
									float TempWallThickness = GetHitWallThickness(SnapObj2);
									if (TempType_1 == EObjectType::ECorner)
									{
										if (CheckIsWallCorner(HitObj))
										{
											UDRCornerAdapter* Local_CornerAdapter = Cast<UDRCornerAdapter>(Local_DRProjData->GetAdapter(HitObj));
											TArray<int32> Local_WallIDs = Local_CornerAdapter->GetWalls();
											FVector2D Local_TempPos0 = Local_Postions[j + 1];
											FVector2D Local_TempPos1 = Local_Postions[j + 1];
											int32 Local_SnapObj = SnapObj2;
											float Local_Length = 1000000.0f;
											for (int WallIndex = 0; WallIndex < Local_WallIDs.Num(); ++WallIndex)
											{
												Local_TempPos0 = Local_Postions[j + 1];
												int32 Local_Snap = -1;
												int32 Local_TempSnap = Local_WallIDs[WallIndex];
												CalculateBestEdgeCornerPos(Local_Postions[j], Local_TempPos0, Local_Snap, Local_TempSnap);
												if (Local_TempPos0 != Local_TempPos1 && FVector2D::Distance(Local_TempPos0, Local_Postions[j]) <= Local_Length)
												{
													Local_Length = FVector2D::Distance(Local_TempPos0, Local_Postions[j]);
													Local_TempPos1 = Local_TempPos0;
													Local_SnapObj = Local_TempSnap;
												}
											}
											SnapObj2 = Local_SnapObj;
											Local_Postions[j + 1] = Local_TempPos1;
										}
										else
											SnapObj2 = -1;
									}
									else
										BuildingSystem->CutAreaSnap(Local_Postions[j + 1], -1, Local_Postions[j + 1], SnapObj2, 2.0f, 2.0f);
								}
								else
									BuildingSystem->CutAreaSnap(Local_Postions[j + 1], -1, Local_Postions[j + 1], SnapObj2, 2.0f, 2.0f);
							}
							else
								BuildingSystem->CutAreaSnap(Local_Postions[j + 1], -1, Local_Postions[j + 1], SnapObj2, 2.0f, 2.0f);
							TArray<int32> WallIDs;
							WallIDs = AddEdge(Local_Postions[j], Local_Postions[j + 1], SnapObj1, SnapObj2);
							for (int32 WallIndex = 0; WallIndex < WallIDs.Num(); ++WallIndex)
							{
								UDRVirtualWallAdapter* Local_VirtualWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(WallIDs[WallIndex]));
								if (Local_VirtualWallAdapter)
								{
									Local_VirtualWallAdapter->SetAssociationID(TenmpID);
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

void UWallBuildSystem::SaveVirWall()
{
	UWorld *MyWorld = GetWorld();
	UWallBuildSystem *WallSystem = nullptr;
	if (MyWorld)
	{
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
		if (GameInst)
		{
			GameInst->VirtualWallData.Empty();
			int num = 1;
			//for (int i = 0; i < TenmpID; ++i)
			//{
			//	FDRVirtualWallStruct Temp;
			//	bool IsValid = false;
			//	Temp.Index = num;
			//	for (int j = 0; j < AllVirtualLoc.Num(); ++j)
			//	{
			//		if (AllVirtualLoc[j].AssociationID == num)
			//		{
			//			IsValid = true;
			//			//FDRVirtualWallStruct Temp;
			//			Temp.Position.Add(AllVirtualLoc[j].OriginalSartPos);
			//			Temp.Position.Add(AllVirtualLoc[j].OriginalEndtPos);
			//			//GameInst->VirtualWallData.Add(Temp);
			//		}
			//	}
			//	if (IsValid)
			//		GameInst->VirtualWallData.Add(Temp);
			//	++num;
			//}
			for (int j = 0; j < AllVirtualLoc.Num(); ++j)
			{


				FDRVirtualWallStruct Temp;
				Temp.Index = num;
				Temp.Position.Add(AllVirtualLoc[j].OriginalSartPos);
				Temp.Position.Add(AllVirtualLoc[j].OriginalEndtPos);
				GameInst->VirtualWallData.Add(Temp);

				++num;
			}
		}
	}
}

void UWallBuildSystem::FilterAreaSlot(const TArray<FVector>& InPnts, TArray<int32>&OutIndex)
{
	OutIndex.Empty();
	TArray<FVector2D> TempVector2D;
	TArray<int32> TempIndex;
	for (int i = 0; i < InPnts.Num(); ++i)
	{
		TempVector2D.Add(FVector2D(InPnts[i]));
	}
	bool IsUseful = FPolygonAlg::CheckRegionUseness(TempVector2D);
	if (!IsUseful)
	{
		return;
	}
	FPolygonAlg::PolygonTriangulate(TempVector2D, TempIndex);
	for (int i = TempIndex.Num() - 1; i >= 0; --i)
	{
		OutIndex.Add(TempIndex[i]);
	}
}

ESelectObjectType UWallBuildSystem::IsWallType(const int32 &objectID, int32 &OutID)
{
	OutID = objectID;
	if (BuildingSystem)
	{
		UBuildingData *Data = BuildingSystem->GetData(objectID);
		if (Data)
		{
			EObjectType Type = (EObjectType)Data->GetObjectType();
			if (Type == EObjectType::ESolidWall)
			{
				return ESelectObjectType::Wall_Object;
			}
			else
				if (Type == EObjectType::EDoorHole || Type == EObjectType::EWindow)
				{
					int WallID = Data->GetInt("WallID");
					if (WallID != INDEX_NONE)
					{
						OutID = WallID;
					}
					return  ESelectObjectType::Wall_Object;
				}
		}
	}
	return ESelectObjectType::None_Object;
}

void UWallBuildSystem::CutWallByMousePos(const FVector2D &Loc)
{
	if (BuildingSystem)
	{
		TempHoleIDs.Empty();
		FVector2D BestLoc = LocateMouseSnapState(Loc);
		int32 ObjectID = INDEX_NONE;
		ObjectID = BuildingSystem->HitTest(BestLoc);
		if (ObjectID != INDEX_NONE)
		{
			UBuildingData* Data = BuildingSystem->GetData(ObjectID);
			if (Data)
			{
				EObjectType Type = (EObjectType)Data->GetObjectType();

				if (Type == EObjectType::EDoorHole || Type == EObjectType::EWindow)
				{
					ShowToastCutWall();
					return;
				}
				if (Type == EObjectType::ESolidWall)
				{
					IObject * WallObject = BuildingSystem->GetObject(ObjectID);
					if (WallObject)
					{
						/*	IValue & _Value = WallObject->GetPropertyValue("Holes");
							const int num = _Value.GetNumFields();
							for (int i = 0; i < num; ++i)
							{
								IValue & _Hole = _Value.GetField(i);
								IValue & _CValue = _Hole.GetField("HoleID");
								int _ID = _CValue.IntValue();
								TempHoleIDs.Add(_ID);
							}
							RecordHoleData(ObjectID);*/
						TArray<FVirtualWallInfo> TempVirInfo;
						for (int i = 0; i < AllVirtualLoc.Num(); ++i)
						{
							if (AllVirtualLoc[i].SnapObj0 == ObjectID || AllVirtualLoc[i].SnapObj1 == ObjectID)
							{
								TempVirInfo.Add(AllVirtualLoc[i]);
							}
						}
						int P0 = Data->GetInt("P0");
						int P1 = Data->GetInt("P1");
						IObject *CornerData1 = BuildingSystem->GetObject(P0);
						IObject *CornerData2 = BuildingSystem->GetObject(P1);
						if (CornerData1&&CornerData2)
						{
							float ThickRight, ThickLeft;
							ThickRight = Data->GetFloat("ThickRight");
							ThickLeft = Data->GetFloat("ThickLeft");
							float ZPos;
							FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
							FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
							BestLoc = FVector2D(FMath::ClosestPointOnSegment(FVector(BestLoc, 0), FVector(StartCornerPos, 0), FVector(EndCornerPos, 0)));
							TArray<FVector> WallNodes;
							BuildingSystem->GetWallBorderLines(ObjectID, WallNodes, ZPos);
							if (WallNodes.Num() == 6)
							{
								float TempLeftStartLength = (WallNodes[0] - WallNodes[1]).SizeSquared();
								float TempLeftEndLength = (WallNodes[2] - WallNodes[3]).SizeSquared();
								float TempRightStartLength = (WallNodes[5] - WallNodes[0]).SizeSquared();
								float TempRightEndtLength = (WallNodes[3] - WallNodes[4]).SizeSquared();
								float TempStartMidLength = (BestLoc - StartCornerPos).SizeSquared();
								float TempEndMidLength = (BestLoc - EndCornerPos).SizeSquared();
								if (TempStartMidLength < TempLeftStartLength - ThickLeft * ThickLeft + 25
									|| TempStartMidLength < TempRightStartLength - ThickRight * ThickRight + 25
									|| TempEndMidLength < TempLeftEndLength - ThickLeft * ThickLeft + 25
									|| TempEndMidLength < TempRightEndtLength - ThickRight * ThickRight + 25)
								{
									ShowUniteToast(_TEXT("拆分位置无效"), 1, 2);
									return;
								}
								IValue & _Value = WallObject->GetPropertyValue("Holes");
								const int num = _Value.GetArrayCount();
								for (int i = 0; i < num; ++i)
								{
									IValue & _Hole = _Value.GetField(i);
									IValue & _CValue = _Hole.GetField("HoleID");
									int _ID = _CValue.IntValue();
									TempHoleIDs.Add(_ID);
								}
								RecordHoleData(ObjectID);
								DeleteSlectActor(ObjectID);
								BuildingSystem->ForceUpdateSuit();
								int CornerID1, CornerID2, CornerID3;
								CornerID1 = BuildingSystem->AddCorner(StartCornerPos);
								AddNewCorner(StartCornerPos, CornerID1);
								CornerID2 = BuildingSystem->AddCorner(EndCornerPos);
								AddNewCorner(EndCornerPos, CornerID2);
								CornerID3 = BuildingSystem->AddCorner(BestLoc);
								AddNewCorner(BestLoc, CornerID3);
								TArray<int32> WallIDs;
								int32 WallID1 = -1;
								int32 WallID2 = -1;
								WallIDs = BuildingSystem->AddWall(CornerID1, CornerID3, ThickLeft, ThickRight, ZPos);
								if (WallIDs.Num() > 0)
								{
									WallID1 = WallIDs[0];
								}
								WallIDs = BuildingSystem->AddWall(CornerID3, CornerID2, ThickLeft, ThickRight, ZPos);
								if (WallIDs.Num() > 0)
								{
									WallID2 = WallIDs[0];
								}
								for (int i = 0; i < TempVirInfo.Num(); ++i)
								{
									TenmpID++;
									BuildingSystem->DeleteObject(TempVirInfo[i].WallID);
									if (TempVirInfo[i].SnapObj0 != -1)
										BuildingSystem->CutAreaSnap(TempVirInfo[i].SartPos, -1, TempVirInfo[i].SartPos, TempVirInfo[i].SnapObj0, WallThickness / 2);
									if (TempVirInfo[i].SnapObj1 != -1)
										BuildingSystem->CutAreaSnap(TempVirInfo[i].EndtPos, -1, TempVirInfo[i].EndtPos, TempVirInfo[i].SnapObj1, WallThickness / 2);
									TArray<int32> WallIDs;
									WallIDs = AddEdge(TempVirInfo[i].SartPos, TempVirInfo[i].EndtPos, TempVirInfo[i].SnapObj0, TempVirInfo[i].SnapObj1);
									for (int k = 0; k < WallIDs.Num(); ++k)
									{
										IValue & GroundName = UBuildingSystem::GetValueFactory()->Create(TenmpID);
										IObject * _ObjGround = BuildingSystem->GetObject(WallIDs[k]);
										if (_ObjGround)
										{
											_ObjGround->SetValue("GroundName", &GroundName);
											IValue *  _V = _ObjGround->FindValue("GroundName");
										}
									}
								}
								AddRecordedHoleData(WallID1, WallID2);
								BuildAreaWithVirWalls();
								ShowAllCornerUMG();
								return;
							}
						}
					}
				}
			}
		}
	}
	ShowUniteToast(TEXT("拆分位置无效"), 1, 2);
}

void UWallBuildSystem::UpdataWallPlaneMaterial(bool IsOpenNewWallType)
{
	TArray<int32> WallIDs;
	GetAllObject(WallIDs, EObjectType::ESolidWall, false);
	for (int i = 0; i < WallIDs.Num(); ++i)
	{
		FindWallAndPlaneByID(WallIDs[i], IsOpenNewWallType);
	}
}

void UWallBuildSystem::MergeWall(const int32 &objectID)
{
	if (BuildingSystem)
	{
		UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
		if (projectDataManager == nullptr)
		{
			return;
		}
		UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(objectID));
		if (CornerAdapter == nullptr)
		{
			return;
		}
		TArray<int32> WallIDs = CornerAdapter->GetWalls();
		if (WallIDs.Num() == 2)
		{
			UBuildingData *Walldata1 = BuildingSystem->GetData(WallIDs[0]);
			if (Walldata1)
			{
				EObjectType InClass = (EObjectType)Walldata1->GetObjectType();
				if (InClass != EObjectType::ESolidWall)
					return;
			}
			UBuildingData *Walldata2 = BuildingSystem->GetData(WallIDs[1]);
			if (Walldata2)
			{
				EObjectType InClass = (EObjectType)Walldata2->GetObjectType();
				if (InClass != EObjectType::ESolidWall)
					return;
			}
			if (Walldata1&&Walldata2)
			{
				int Wall1P0 = Walldata1->GetInt("P0");
				int Wall1P1 = Walldata1->GetInt("P1");
				int Wall2P0 = Walldata2->GetInt("P0");
				int Wall2P1 = Walldata2->GetInt("P1");
				int Zpos1 = Walldata1->GetFloat("Height");
				int Zpos2 = Walldata2->GetFloat("Height");
				IObject *CornerData1 = BuildingSystem->GetObject(Wall1P0);
				IObject *CornerData2 = BuildingSystem->GetObject(Wall1P1);
				IObject *CornerData3 = BuildingSystem->GetObject(Wall2P0);
				IObject *CornerData4 = BuildingSystem->GetObject(Wall2P1);
				if (CornerData1&&CornerData2&&CornerData3&&CornerData4)
				{
					FVector2D StartCornerPos1 = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
					FVector2D EndCornerPos1 = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
					FVector2D StartCornerPos2 = ToVector2D(CornerData3->GetPropertyValue("Location").Vec2Value());
					FVector2D EndCornerPos2 = ToVector2D(CornerData4->GetPropertyValue("Location").Vec2Value());
					if (FMath::Abs(FVector2D::CrossProduct((EndCornerPos1 - StartCornerPos1).GetSafeNormal(), (EndCornerPos2 - StartCornerPos2).GetSafeNormal())) < 0.02)
					{
						float Wall1LeftThick = Walldata1->GetFloat("ThickLeft");
						float Wall1RightThick = Walldata1->GetFloat("ThickRight");
						float Wall2LeftThick = Walldata2->GetFloat("ThickLeft");;
						float Wall2RightThick = Walldata2->GetFloat("ThickRight");
						if (Zpos1 == Zpos2 && Wall1LeftThick + Wall1RightThick == Wall2LeftThick + Wall2RightThick)
						{
							bool Istrue = false;
							int LastStart, LastEnd;
							FVector2D LastStartCornerPos;
							FVector2D LastEndCornerPos;
							float NewWallLeftThick, NewWallRightThick;
							if (Wall1P0 == Wall2P0)
							{
								if (Wall1LeftThick == Wall2RightThick && Wall1RightThick == Wall2LeftThick)
								{
									LastStart = Wall1P1;
									LastStartCornerPos = EndCornerPos1;
									LastEnd = Wall2P1;
									LastEndCornerPos = EndCornerPos2;
									NewWallLeftThick = Wall2LeftThick;
									NewWallRightThick = Wall2RightThick;
									Istrue = true;
								}
							}
							else
								if (Wall1P1 == Wall2P1)
								{
									if (Wall1LeftThick == Wall2RightThick && Wall1RightThick == Wall2LeftThick)
									{
										LastStart = Wall1P0;
										LastStartCornerPos = StartCornerPos1;
										LastEnd = Wall2P0;
										LastEndCornerPos = StartCornerPos2;
										NewWallLeftThick = Wall1LeftThick;
										NewWallRightThick = Wall1RightThick;
										Istrue = true;
									}
								}
								else
									if (Wall1P0 == Wall2P1)
									{
										if (Wall1LeftThick == Wall2LeftThick && Wall1RightThick == Wall2RightThick)
										{
											LastStart = Wall2P0;
											LastStartCornerPos = StartCornerPos2;
											LastEnd = Wall1P1;
											LastEndCornerPos = EndCornerPos1;
											NewWallLeftThick = Wall1LeftThick;
											NewWallRightThick = Wall1RightThick;
											Istrue = true;
										}
									}
									else
										if (Wall1P1 == Wall2P0)
										{
											if (Wall1LeftThick == Wall2LeftThick && Wall1RightThick == Wall2RightThick)
											{
												LastStart = Wall1P0;
												LastStartCornerPos = StartCornerPos1;
												LastEnd = Wall2P1;
												LastEndCornerPos = EndCornerPos2;
												NewWallLeftThick = Wall1LeftThick;
												NewWallRightThick = Wall1RightThick;
												Istrue = true;
											}
										}
							if (Istrue)
							{
								if (IsCanMergeWallBP(WallIDs[0], WallIDs[1]))
								{
									TempHoleIDs.Empty();
									IObject * WallObject1 = BuildingSystem->GetObject(WallIDs[0]);
									IObject * WallObject2 = BuildingSystem->GetObject(WallIDs[1]);
									if (WallObject1&&WallObject2)
									{
										IValue & _Value1 = WallObject1->GetPropertyValue("Holes");
										int num = _Value1.GetArrayCount();
										for (int i = 0; i < num; ++i)
										{
											IValue & _Hole1 = _Value1.GetField(i);
											IValue & _CValue1 = _Hole1.GetField("HoleID");
											int _ID1 = _CValue1.IntValue();
											TempHoleIDs.Add(_ID1);
										}
										IValue &_Value2 = WallObject2->GetPropertyValue("Holes");
										num = _Value2.GetArrayCount();
										for (int i = 0; i < num; ++i)
										{
											IValue & _Hole2 = _Value2.GetField(i);
											IValue & _CValue2 = _Hole2.GetField("HoleID");
											int _ID2 = _CValue2.IntValue();
											TempHoleIDs.Add(_ID2);
										}
										RecordHoleData(WallIDs[0]);
										SetIsDeleteTempArea();
										DeleteSlectActor(WallIDs[0]);
										DeleteSlectActor(WallIDs[1]);
										if (objectID == Wall1P0)
										{
											LastStart = Wall1P1;
										}
										else
											LastStart = Wall1P0;
										if (objectID == Wall2P0)
										{
											LastEnd = Wall2P1;
										}
										else
											LastEnd = Wall2P0;
										BuildingSystem->ForceUpdateSuit();
										LastStart = BuildingSystem->AddCorner(LastStartCornerPos);
										//AddNewCorner(LastStartCornerPos, LastStart);
										LastEnd = BuildingSystem->AddCorner(LastEndCornerPos);
										//AddNewCorner(LastEndCornerPos, LastEnd);
										TArray<int32> NewWallIDS = BuildingSystem->AddWall(LastStart, LastEnd, NewWallLeftThick, NewWallRightThick, Zpos1);
										if (NewWallIDS.Num() > 0)
										{
											AddRecordedHoleData(NewWallIDS[0], -1);
											BuildAreaWithVirWalls();
										}
										UpdataCornerUMGData();
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

FVector2D UWallBuildSystem::OrthogonalDraw(const FVector2D &Loc, const int32 & LastID, bool IsOpen)
{
	FVector2D MousePos = Loc;
	if (IsOpen)
	{
		if (bNormalDraw)
		{
			if (LastID != INDEX_NONE)
			{
				IObject *pCorner = BuildingSystem->GetObject(LastID);
				if (pCorner)
				{
					FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
					FVector2D Vec = MousePos - Location;
					FVector2D VecNomal = UAJBlueprintFunctionLibrary::DirectionInWorld(MousePos - Location);
					float dx = FVector2D::DotProduct(Vec, VecNomal);
					MousePos = FVector2D(VecNomal.X*dx, VecNomal.Y*dx) + Location;
				}
			}
		}
	}
	return MousePos;
}

void UWallBuildSystem::MergeAllPolygon(TArray<FPolygonInfo>&Out_Polygons)
{
	Out_Polygons.Empty();
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	TArray<FRoomPath> AreaPathArray = GameInstance->WallBuildSystem->GetInnerRoomPathArray();
	TArray<FRoomPath> TempArea;
	TArray<TArray<FVector2D>> AllInnerRegions;
	TArray<FString> AllAreaTag;
	TMap<int32, TArray<TArray<FVector2D>>> IndexMapInnerPs;
	TMap<int32, TArray<int32>> RegionMapRegions;
	for (auto &AreaPath : AreaPathArray)
	{
		TArray<FVector2D> TempPostArea;
		for (int _CurP = 0; _CurP < AreaPath.InnerRoomPath.Num(); ++_CurP)
		{
			TempPostArea.AddUnique(AreaPath.InnerRoomPath[_CurP]);
		}
		AllInnerRegions.Add(TempPostArea);
		AllAreaTag.Add(AreaPath.AreaTag);
	}

	for (int32 I = 0; I < AllInnerRegions.Num(); ++I)
	{
		TArray<TArray<FVector2D>> InnerBoundaryPSets;
		TArray<int32> CollectPairRegionIndexs;
		for (int32 J = 0; J < AllInnerRegions.Num(); ++J)
		{
			if (J != I)
			{
				TArray<FVector2D> UpdatePolygonFirst, UpdatePolygonSecond, InterSectPs, CombinedPolygon;
				bool ISuccess = FPolygonAlg::CalculateTwoPolygonInterSect(AllInnerRegions[I], AllInnerRegions[J], UpdatePolygonFirst,
					UpdatePolygonSecond, InterSectPs, CombinedPolygon);
				if (ISuccess)
				{
					InnerBoundaryPSets.Add(InterSectPs);
					CollectPairRegionIndexs.AddUnique(J);
				}
			}
		}
		if (InnerBoundaryPSets.Num() > 0)
		{
			IndexMapInnerPs.Add(I, InnerBoundaryPSets);
			RegionMapRegions.Add(I, CollectPairRegionIndexs);
		}
	}


	TArray<int32> CalculatedIndexs;
	if (IndexMapInnerPs.Num() != 0)
	{
		for (int32 RegionIndex = 0; RegionIndex < AllInnerRegions.Num(); ++RegionIndex)
		{
			TArray<FVector2D> UpdateTempRegionPs;
			UpdateTempRegionPs = AllInnerRegions[RegionIndex];
			if (!CalculatedIndexs.Contains(RegionIndex))
			{
				if (IndexMapInnerPs.Contains(RegionIndex))
				{
					TArray<TArray<FVector2D>> TempGroupInnerPs = IndexMapInnerPs[RegionIndex];
					TArray<int32> PairedRegionIndexs = RegionMapRegions[RegionIndex];
					if (PairedRegionIndexs.Num() == 1)
					{
						CalculatedIndexs.AddUnique(RegionIndex);
						TArray<int32> TempPair = RegionMapRegions[PairedRegionIndexs[0]];
						if (TempPair.Num() == 1)
						{
							TArray<int32> CollectInts;
							CalculatedIndexs.AddUnique(TempPair[0]);
							CollectInts.AddUnique(RegionIndex);
							CollectInts.AddUnique(TempPair[0]);
							TArray<FVector2D> BasePolygon, TPolygonA, TPolygonB, Polygons, InterSectPs;

							BasePolygon = UpdateTempRegionPs;
							TArray<FVector2D> TempPairRegion = AllInnerRegions[PairedRegionIndexs[0]];

							if (!IsPolyClockWise(BasePolygon))
							{
								ChangeClockwise(BasePolygon);
							}
							if (!IsPolyClockWise(TempPairRegion))
							{
								ChangeClockwise(TempPairRegion);
							}
							bool IsInterSect = FPolygonAlg::JudgePolygonInterSect(BasePolygon, TempPairRegion);
							bool HasCommonPs = FPolygonAlg::CalculateTwoPolygonInterSect(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, InterSectPs, Polygons);;
							if ((IsInterSect) && (!HasCommonPs))
							{
								TPolygonA.Empty();
								TPolygonB.Empty();
								Polygons.Empty();
								FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, Polygons);

							}
							else if ((!IsInterSect) && (HasCommonPs))
							{
								TPolygonA.Empty();
								TPolygonB.Empty();
								InterSectPs.Empty();
								Polygons.Empty();
								FPolygonAlg::CalculateTwoPolygonInterSect(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, InterSectPs, Polygons);
							}
							else if ((!IsInterSect) && (!HasCommonPs))
							{
								TPolygonA.Empty();
								TPolygonB.Empty();
								Polygons.Empty();
								FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, Polygons);
							}

							if (Polygons.Num() != 0)
							{
								FPolygonInfo TempPolygonInfo;
								TempPolygonInfo.RegionalPoints = Polygons;
								TempPolygonInfo.AreaTags.Add(AllAreaTag[RegionIndex]);
								TempPolygonInfo.AreaTags.Add(AllAreaTag[PairedRegionIndexs[0]]);
								Out_Polygons.Add(TempPolygonInfo);
							}
						}
						else if (TempPair.Num() >= 2)
						{
							TArray<int32> CollectInts;
							CollectInts.AddUnique(RegionIndex);
							for (int32 I = 0; I < TempPair.Num(); ++I)
							{
								CollectInts.AddUnique(TempPair[I]);
								CalculatedIndexs.AddUnique(TempPair[I]);
								TArray<int32> TempNextPair = RegionMapRegions[TempPair[I]];
								for (int32 J = 0; J < TempNextPair.Num(); ++J)
								{
									CollectInts.AddUnique(TempNextPair[J]);
									CalculatedIndexs.AddUnique(TempNextPair[J]);
								}
							}

							TArray<FVector2D> BasePolygon;
							TArray<FVector2D> Polygons;
							BasePolygon = UpdateTempRegionPs;
							for (int32 _NumF = 0; _NumF < CollectInts.Num(); ++_NumF)
							{
								Polygons.Empty();
								TArray<FVector2D> TempPairRegion = AllInnerRegions[CollectInts[_NumF]];
								if (CollectInts[_NumF] != RegionIndex)
								{
									if ((BasePolygon.Num() > 0) && (TempPairRegion.Num() > 0))
									{
										if (!IsPolyClockWise(BasePolygon))
										{
											ChangeClockwise(BasePolygon);
										}
										if (!IsPolyClockWise(TempPairRegion))
										{
											ChangeClockwise(TempPairRegion);
										}
										FPolygonAlg::CGPolygonCombine(BasePolygon, TempPairRegion, Polygons);
										float S1 = FPolygonAlg::GetAreaOfRegion(BasePolygon);
										float S2 = FPolygonAlg::GetAreaOfRegion(TempPairRegion);
										float S3 = FPolygonAlg::GetAreaOfRegion(Polygons);
										float AreaDiffer = abs(S1 + S2 - S3);
										float DifferRatio = AreaDiffer / (S1 + S2);
										bool IsCombinedUseful = false;
										if (DifferRatio >= 0.1)
										{
											TArray<FVector2D>  NewFirstPoly, NewSecondPoly, PolygonUnion;
											FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, NewFirstPoly, NewSecondPoly, PolygonUnion);
											if (PolygonUnion.Num() > 0)
											{
												BasePolygon = PolygonUnion;
												Polygons = PolygonUnion;
												IsCombinedUseful = true;
											}
										}

										if (!IsCombinedUseful)
										{
											BasePolygon = Polygons;
										}
									}
								}
							}
							if (Polygons.Num() != 0)
							{
								FPolygonInfo TempPolygonInfo;
								TempPolygonInfo.RegionalPoints = Polygons;
								
								for (int _CurIndex = 0; _CurIndex < CollectInts.Num(); ++_CurIndex)
								{
									TempPolygonInfo.AreaTags.Add(AllAreaTag[CollectInts[_CurIndex]]);
								}
								Out_Polygons.Add(TempPolygonInfo);
							}
						}
					}
					else if (PairedRegionIndexs.Num() >= 2)
					{
						TArray<int32> CollectInts;
						CollectInts.AddUnique(RegionIndex);
						CalculatedIndexs.AddUnique(RegionIndex);
						int32 BaseCount = 1, EndCount = 0;
						TArray<int32> UpdateRegionIndexs;
						while (EndCount != BaseCount)
						{
							for (int32 I = 0; I < PairedRegionIndexs.Num(); ++I)
							{
								CollectInts.AddUnique(PairedRegionIndexs[I]);
								CalculatedIndexs.AddUnique(PairedRegionIndexs[I]);
								BaseCount = CollectInts.Num();
								TArray<int32> TempNextPair = RegionMapRegions[PairedRegionIndexs[I]];
								for (int32 P = 0; P < TempNextPair.Num(); ++P)
								{
									if (!CollectInts.Contains(TempNextPair[P]))
									{
										CollectInts.AddUnique(TempNextPair[P]);
										CalculatedIndexs.AddUnique(TempNextPair[P]);
										UpdateRegionIndexs.AddUnique(TempNextPair[P]);
									}
								}
							}

							EndCount = CollectInts.Num();
							if (UpdateRegionIndexs.Num() != 0)
							{
								PairedRegionIndexs.Empty();
								for (int32 J = 0; J < UpdateRegionIndexs.Num(); ++J)
								{
									TArray<int32> TempArray = RegionMapRegions[UpdateRegionIndexs[J]];
									for (int32 K = 0; K < TempArray.Num(); ++K)
									{
										PairedRegionIndexs.AddUnique(TempArray[K]);
									}
								}
							}
						}

						TArray<FVector2D> BasePolygon;
						TArray<FVector2D> Polygons;
						BasePolygon = UpdateTempRegionPs;
						for (int32 _NumF = 0; _NumF < CollectInts.Num(); ++_NumF)
						{
							Polygons.Empty();
							TArray<FVector2D> TempPairRegion = AllInnerRegions[CollectInts[_NumF]];
							if (CollectInts[_NumF] != RegionIndex)
							{
								if (!IsPolyClockWise(BasePolygon))
								{
									ChangeClockwise(BasePolygon);
								}
								if (!IsPolyClockWise(TempPairRegion))
								{
									ChangeClockwise(TempPairRegion);
								}
								FPolygonAlg::CGPolygonCombine(BasePolygon, TempPairRegion, Polygons);
								float S1 = FPolygonAlg::GetAreaOfRegion(BasePolygon);
								float S2 = FPolygonAlg::GetAreaOfRegion(TempPairRegion);
								float S3 = FPolygonAlg::GetAreaOfRegion(Polygons);
								float AreaDiffer = abs(S1 + S2 - S3);
								float DifferRatio = AreaDiffer / (S1 + S2);
								bool IsCombinedUseful = false;
								if (DifferRatio >= 0.1)
								{
									TArray<FVector2D>  NewFirstPoly, NewSecondPoly, PolygonUnion;
									FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, NewFirstPoly, NewSecondPoly, PolygonUnion);
									if (PolygonUnion.Num() > 0)
									{
										BasePolygon = PolygonUnion;
										Polygons = PolygonUnion;
										IsCombinedUseful = true;
									}
								}

								if (!IsCombinedUseful)
								{
									BasePolygon = Polygons;
								}
							}
						}
						if (Polygons.Num() != 0)
						{
							FPolygonInfo TempPolygonInfo;
							TempPolygonInfo.RegionalPoints = Polygons;
							
							for (int _CurIndex = 0; _CurIndex < CollectInts.Num(); ++_CurIndex)
							{
								TempPolygonInfo.AreaTags.Add(AllAreaTag[CollectInts[_CurIndex]]);
							}
							Out_Polygons.Add(TempPolygonInfo);
						}
					}
				}
			}
		}

	}
	else
	{
		FPolygonInfo TempPolygonInfo;
		for (auto &AreaPath : AreaPathArray)
		{
			if (AreaPath.bIsLivingOrDiningRoom)
			{
				TempPolygonInfo.AreaTags.Add(AreaPath.AreaTag);
				TempArea.Add(AreaPath);
			}
		}
		if (TempArea.Num() == 2)
		{
			TArray<FVector2D> AreaArray;
			TArray<FVector2D> Intersence;

			TArray<FVector2D> PolygonA, PolygonB;
			if (TempArea[0].InnerRoomPath.Num() < TempArea[1].InnerRoomPath.Num())
			{
				PolygonA = TempArea[0].InnerRoomPath;
				PolygonB = TempArea[1].InnerRoomPath;
			}
			else if (TempArea[0].InnerRoomPath.Num() > TempArea[1].InnerRoomPath.Num())
			{
				PolygonA = TempArea[1].InnerRoomPath;
				PolygonB = TempArea[0].InnerRoomPath;
			}
			else
			{
				float L1 = 0.0, L2 = 0.0;
				int32 NextI = 0;
				for (int32 I = 0; I < TempArea[0].InnerRoomPath.Num(); I++)
				{
					if (I == TempArea[0].InnerRoomPath.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					L1 = L1 + FVector2D::Distance(TempArea[0].InnerRoomPath[I], TempArea[0].InnerRoomPath[NextI]);
				}
				int32 NextJ = 0;
				for (int32 J = 0; J < TempArea[1].InnerRoomPath.Num(); J++)
				{
					if (J == TempArea[1].InnerRoomPath.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					L2 = L2 + FVector2D::Distance(TempArea[1].InnerRoomPath[J], TempArea[1].InnerRoomPath[NextJ]);
				}

				if (L1 < L2)
				{
					PolygonA = TempArea[0].InnerRoomPath;
					PolygonB = TempArea[1].InnerRoomPath;
				}
				else
				{
					PolygonA = TempArea[1].InnerRoomPath;
					PolygonB = TempArea[0].InnerRoomPath;
				}
			}

			TArray<FVector2D> TPolygonA;
			TArray<FVector2D> TPolygonB;
			TArray<FVector2D> Polygons;
			if (!IsPolyClockWise(PolygonA))
			{
				ChangeClockwise(PolygonA);
			}
			if (!IsPolyClockWise(PolygonB))
			{
				ChangeClockwise(PolygonB);
			}


			ARoomActor::PolygonsUnionFunction(PolygonA,
				PolygonB,
				TPolygonA,
				TPolygonB,
				Polygons);


			if (Polygons.Num() != 0)
			{
				TArray<FVector2D> Out_Pints;
				FPolygonAlg::MergeCollinearPoints(Polygons, Out_Pints);
				TempPolygonInfo.RegionalPoints = Out_Pints;
				Out_Polygons.Add(TempPolygonInfo);
			}
		}
	}
}

bool UWallBuildSystem::IsPolyClockWise(const TArray<FVector2D> & Polygons)
{
	double d = 0.f;

	for (int i = 0; i < Polygons.Num(); i++)
	{
		d += -0.5f * ((Polygons[(i + 1) % Polygons.Num()].Y + Polygons[i].Y) * (Polygons[(i + 1) % Polygons.Num()].X - Polygons[i].X));
	}
	return d < 0.f;
}
void UWallBuildSystem::ChangeClockwise(TArray<FVector2D> & Polygons)
{
	TArray<FVector2D> Temp;
	Temp.Add(Polygons[0]);
	for (int i = Polygons.Num() - 1; i > 0; --i)
	{
		Temp.Add(Polygons[i]);
	}
	Polygons.Reserve(Temp.Num());
	Polygons = Temp;
}

void UWallBuildSystem::MergeOnePolygon(const TArray<FString>& AreaTag, TArray<FVector2D>&Out_Points)
{
	Out_Points.Empty();
	TArray<FRoomPath> TempArea;
	TArray<FRoomPath> AreaPathArray = GetInnerRoomPathArray();
	TArray<TArray<FVector2D>> AllInnerRegions;
	for (auto &AreaPath : AreaPathArray)
	{
		for (int i = 0; i < AreaTag.Num(); ++i)
		{
			if (AreaTag[i] == AreaPath.AreaTag)
			{
				AllInnerRegions.Add(AreaPath.InnerRoomPath);
				break;
			}
		}
	}
	TMap<int32, TArray<TArray<FVector2D>>> IndexMapInnerPs;
	TMap<int32, TArray<int32>> RegionMapRegions;
	for (int32 I = 0; I < AllInnerRegions.Num(); ++I)
	{
		TArray<TArray<FVector2D>> InnerBoundaryPSets;
		TArray<int32> CollectPairRegionIndexs;
		for (int32 J = 0; J < AllInnerRegions.Num(); ++J)
		{
			if (J != I)
			{
				TArray<FVector2D> UpdatePolygonFirst, UpdatePolygonSecond, InterSectPs, CombinedPolygon;
				bool ISuccess = FPolygonAlg::CalculateTwoPolygonInterSect(AllInnerRegions[I], AllInnerRegions[J], UpdatePolygonFirst,
					UpdatePolygonSecond, InterSectPs, CombinedPolygon);
				if (ISuccess)
				{
					InnerBoundaryPSets.Add(InterSectPs);
					CollectPairRegionIndexs.AddUnique(J);
				}
			}
		}
		if (InnerBoundaryPSets.Num() > 0)
		{
			IndexMapInnerPs.Add(I, InnerBoundaryPSets);
			RegionMapRegions.Add(I, CollectPairRegionIndexs);
		}
	}


	TArray<int32> CalculatedIndexs;
	if (IndexMapInnerPs.Num() != 0)
	{
		for (int32 RegionIndex = 0; RegionIndex < AllInnerRegions.Num(); ++RegionIndex)
		{
			TArray<FVector2D> UpdateTempRegionPs;
			UpdateTempRegionPs = AllInnerRegions[RegionIndex];
			if (!CalculatedIndexs.Contains(RegionIndex))
			{
				if (IndexMapInnerPs.Contains(RegionIndex))
				{
					TArray<TArray<FVector2D>> TempGroupInnerPs = IndexMapInnerPs[RegionIndex];
					TArray<int32> PairedRegionIndexs = RegionMapRegions[RegionIndex];
					if (PairedRegionIndexs.Num() == 1)
					{
						CalculatedIndexs.AddUnique(RegionIndex);
						TArray<int32> TempPair = RegionMapRegions[PairedRegionIndexs[0]];
						if (TempPair.Num() == 1)
						{
							TArray<int32> CollectInts;
							CalculatedIndexs.AddUnique(TempPair[0]);
							CollectInts.AddUnique(RegionIndex);
							CollectInts.AddUnique(TempPair[0]);
							TArray<FVector2D> BasePolygon, TPolygonA, TPolygonB, Polygons, InterSectPs;

							BasePolygon = UpdateTempRegionPs;
							TArray<FVector2D> TempPairRegion = AllInnerRegions[PairedRegionIndexs[0]];

							if (!IsPolyClockWise(BasePolygon))
							{
								ChangeClockwise(BasePolygon);
							}
							if (!IsPolyClockWise(TempPairRegion))
							{
								ChangeClockwise(TempPairRegion);
							}
							bool IsInterSect = FPolygonAlg::JudgePolygonInterSect(BasePolygon, TempPairRegion);
							bool HasCommonPs = FPolygonAlg::CalculateTwoPolygonInterSect(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, InterSectPs, Polygons);;
							if ((IsInterSect) && (!HasCommonPs))
							{
								TPolygonA.Empty();
								TPolygonB.Empty();
								Polygons.Empty();
								FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, Polygons);

							}
							else if ((!IsInterSect) && (HasCommonPs))
							{
								TPolygonA.Empty();
								TPolygonB.Empty();
								InterSectPs.Empty();
								Polygons.Empty();
								FPolygonAlg::CalculateTwoPolygonInterSect(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, InterSectPs, Polygons);
							}
							else if ((!IsInterSect) && (!HasCommonPs))
							{
								TPolygonA.Empty();
								TPolygonB.Empty();
								Polygons.Empty();
								FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, TPolygonA, TPolygonB, Polygons);
							}

							if (Polygons.Num() != 0)
							{
								Out_Points = Polygons;
							}
						}
						else if (TempPair.Num() >= 2)
						{
							TArray<int32> CollectInts;
							CollectInts.AddUnique(RegionIndex);
							for (int32 I = 0; I < TempPair.Num(); ++I)
							{
								CollectInts.AddUnique(TempPair[I]);
								CalculatedIndexs.AddUnique(TempPair[I]);
								TArray<int32> TempNextPair = RegionMapRegions[TempPair[I]];
								for (int32 J = 0; J < TempNextPair.Num(); ++J)
								{
									CollectInts.AddUnique(TempNextPair[J]);
									CalculatedIndexs.AddUnique(TempNextPair[J]);
								}
							}

							TArray<FVector2D> BasePolygon;
							TArray<FVector2D> Polygons;
							BasePolygon = UpdateTempRegionPs;
							for (int32 _NumF = 0; _NumF < CollectInts.Num(); ++_NumF)
							{
								Polygons.Empty();
								TArray<FVector2D> TempPairRegion = AllInnerRegions[CollectInts[_NumF]];
								if (CollectInts[_NumF] != RegionIndex)
								{
									if ((BasePolygon.Num() > 0) && (TempPairRegion.Num() > 0))
									{
										if (!IsPolyClockWise(BasePolygon))
										{
											ChangeClockwise(BasePolygon);
										}
										if (!IsPolyClockWise(TempPairRegion))
										{
											ChangeClockwise(TempPairRegion);
										}
										FPolygonAlg::CGPolygonCombine(BasePolygon, TempPairRegion, Polygons);
										float S1 = FPolygonAlg::GetAreaOfRegion(BasePolygon);
										float S2 = FPolygonAlg::GetAreaOfRegion(TempPairRegion);
										float S3 = FPolygonAlg::GetAreaOfRegion(Polygons);
										float AreaDiffer = abs(S1 + S2 - S3);
										float DifferRatio = AreaDiffer / (S1 + S2);
										bool IsCombinedUseful = false;
										if (DifferRatio >= 0.1)
										{
											TArray<FVector2D>  NewFirstPoly, NewSecondPoly, PolygonUnion;
											FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, NewFirstPoly, NewSecondPoly, PolygonUnion);
											if (PolygonUnion.Num() > 0)
											{
												BasePolygon = PolygonUnion;
												Polygons = PolygonUnion;
												IsCombinedUseful = true;
											}
										}

										if (!IsCombinedUseful)
										{
											BasePolygon = Polygons;
										}
									}
								}
							}
							if (Polygons.Num() != 0)
							{
								Out_Points = Polygons;
							}
						}
					}
					else if (PairedRegionIndexs.Num() >= 2)
					{
						TArray<int32> CollectInts;
						CollectInts.AddUnique(RegionIndex);
						CalculatedIndexs.AddUnique(RegionIndex);
						int32 BaseCount = 1, EndCount = 0;
						TArray<int32> UpdateRegionIndexs;
						while (EndCount != BaseCount)
						{
							for (int32 I = 0; I < PairedRegionIndexs.Num(); ++I)
							{
								CollectInts.AddUnique(PairedRegionIndexs[I]);
								CalculatedIndexs.AddUnique(PairedRegionIndexs[I]);
								BaseCount = CollectInts.Num();
								TArray<int32> TempNextPair = RegionMapRegions[PairedRegionIndexs[I]];
								for (int32 P = 0; P < TempNextPair.Num(); ++P)
								{
									if (!CollectInts.Contains(TempNextPair[P]))
									{
										CollectInts.AddUnique(TempNextPair[P]);
										CalculatedIndexs.AddUnique(TempNextPair[P]);
										UpdateRegionIndexs.AddUnique(TempNextPair[P]);
									}
								}
							}

							EndCount = CollectInts.Num();
							if (UpdateRegionIndexs.Num() != 0)
							{
								PairedRegionIndexs.Empty();
								for (int32 J = 0; J < UpdateRegionIndexs.Num(); ++J)
								{
									TArray<int32> TempArray = RegionMapRegions[UpdateRegionIndexs[J]];
									for (int32 K = 0; K < TempArray.Num(); ++K)
									{
										PairedRegionIndexs.AddUnique(TempArray[K]);
									}
								}
							}
						}

						TArray<FVector2D> BasePolygon;
						TArray<FVector2D> Polygons;
						BasePolygon = UpdateTempRegionPs;
						for (int32 _NumF = 0; _NumF < CollectInts.Num(); ++_NumF)
						{
							Polygons.Empty();
							TArray<FVector2D> TempPairRegion = AllInnerRegions[CollectInts[_NumF]];
							if (CollectInts[_NumF] != RegionIndex)
							{
								if (!IsPolyClockWise(BasePolygon))
								{
									ChangeClockwise(BasePolygon);
								}
								if (!IsPolyClockWise(TempPairRegion))
								{
									ChangeClockwise(TempPairRegion);
								}
								FPolygonAlg::CGPolygonCombine(BasePolygon, TempPairRegion, Polygons);
								float S1 = FPolygonAlg::GetAreaOfRegion(BasePolygon);
								float S2 = FPolygonAlg::GetAreaOfRegion(TempPairRegion);
								float S3 = FPolygonAlg::GetAreaOfRegion(Polygons);
								float AreaDiffer = abs(S1 + S2 - S3);
								float DifferRatio = AreaDiffer / (S1 + S2);
								bool IsCombinedUseful = false;
								if (DifferRatio >= 0.1)
								{
									TArray<FVector2D>  NewFirstPoly, NewSecondPoly, PolygonUnion;
									FPolygonAlg::CreatePolygonUnion(BasePolygon, TempPairRegion, NewFirstPoly, NewSecondPoly, PolygonUnion);
									if (PolygonUnion.Num() > 0)
									{
										BasePolygon = PolygonUnion;
										Polygons = PolygonUnion;
										IsCombinedUseful = true;
									}
								}

								if (!IsCombinedUseful)
								{
									BasePolygon = Polygons;
								}
							}
						}
						if (Polygons.Num() != 0)
						{
							Out_Points = Polygons;
						}
					}
				}
			}
		}

	}
	else
	{
		for (auto &AreaPath : AreaPathArray)
		{
			for (int i = 0; i < AreaTag.Num(); ++i)
			{
				if (AreaTag[i] == AreaPath.AreaTag)
				{
					TempArea.Add(AreaPath);
					break;
				}
			}
		}
		if (TempArea.Num() == 2)
		{
			TArray<FVector2D> AreaArray;
			TArray<FVector2D> Intersence;

			TArray<FVector2D> PolygonA, PolygonB;
			if (TempArea[0].InnerRoomPath.Num() < TempArea[1].InnerRoomPath.Num())
			{
				PolygonA = TempArea[0].InnerRoomPath;
				PolygonB = TempArea[1].InnerRoomPath;
			}
			else if (TempArea[0].InnerRoomPath.Num() > TempArea[1].InnerRoomPath.Num())
			{
				PolygonA = TempArea[1].InnerRoomPath;
				PolygonB = TempArea[0].InnerRoomPath;
			}
			else
			{
				float L1 = 0.0, L2 = 0.0;
				int32 NextI = 0;
				for (int32 I = 0; I < TempArea[0].InnerRoomPath.Num(); I++)
				{
					if (I == TempArea[0].InnerRoomPath.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					L1 = L1 + FVector2D::Distance(TempArea[0].InnerRoomPath[I], TempArea[0].InnerRoomPath[NextI]);
				}
				int32 NextJ = 0;
				for (int32 J = 0; J < TempArea[1].InnerRoomPath.Num(); J++)
				{
					if (J == TempArea[1].InnerRoomPath.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					L2 = L2 + FVector2D::Distance(TempArea[1].InnerRoomPath[J], TempArea[1].InnerRoomPath[NextJ]);
				}

				if (L1 < L2)
				{
					PolygonA = TempArea[0].InnerRoomPath;
					PolygonB = TempArea[1].InnerRoomPath;
				}
				else
				{
					PolygonA = TempArea[1].InnerRoomPath;
					PolygonB = TempArea[0].InnerRoomPath;
				}
			}

			TArray<FVector2D> TPolygonA;
			TArray<FVector2D> TPolygonB;
			TArray<FVector2D> Polygons;

			if (!IsPolyClockWise(PolygonA))
			{
				ChangeClockwise(PolygonA);
			}
			if (!IsPolyClockWise(PolygonB))
			{
				ChangeClockwise(PolygonB);
			}


			ARoomActor::PolygonsUnionFunction(PolygonA,
				PolygonB,
				TPolygonA,
				TPolygonB,
				Polygons);


			if (Polygons.Num() != 0)
			{
				TArray<FVector2D> Temp_Pints;
				FPolygonAlg::MergeShortEdgesPreProcess(Polygons, Temp_Pints);
				Out_Points = Temp_Pints;
			}
		}
	}
}

void UWallBuildSystem::CalculateWallIntersection(const TArray<FWallPints>& In_Points, TArray<FWallPints>&Out_Points)
{
	Out_Points = In_Points;
	if (Out_Points.Num() > 1)
	{
		for (int i = 0; i < Out_Points.Num() - 1; ++i)
		{
			for (int j = i + 1; j < Out_Points.Num(); ++j)
			{
				if (IsTwoLineSegmentsIntersect(Out_Points[i].StartLoc, Out_Points[i].EndLoc, Out_Points[j].StartLoc, Out_Points[j].EndLoc))
				{
					FVector2D OutPos;
					OutPos = GetwoSegmentsIntersect(Out_Points[i].StartLoc, Out_Points[i].EndLoc, Out_Points[j].StartLoc, Out_Points[j].EndLoc);
					float length1 = (OutPos - Out_Points[i].StartLoc).SizeSquared();
					float length2 = (OutPos - Out_Points[i].EndLoc).SizeSquared();
					if (isnan(length1) || isnan(length2))
					{
						;
					}
					else
					{
						if (length1 < length2)
						{
							Out_Points[i].StartLoc = OutPos;
						}
						else
							Out_Points[i].EndLoc = OutPos;
						length1 = (OutPos - Out_Points[j].StartLoc).SizeSquared();
						length2 = (OutPos - Out_Points[j].EndLoc).SizeSquared();
						if (length1 < length2)
						{
							Out_Points[j].StartLoc = OutPos;
						}
						else
							Out_Points[j].EndLoc = OutPos;
					}
				}
			}
		}
	}
}

void UWallBuildSystem::IsNeedSegmentAreaByAIData(const TArray<FWallPints>& In_Points, const TArray<FRoomInfo>& Area, TArray<FWallPints>& Out_Points)
{
	Out_Points = In_Points;
	TArray<FVector> AreaPos;
	for (int j = 0; j < Area.Num(); ++j)
	{
		if (Area[j].roomName == "客卫")
		{
			AreaPos = Area[j].AreaPos;
			break;
		}
	};
	if (AreaPos.Num() > 2)
	{
		for (int i = 0; i < In_Points.Num(); ++i)
		{
			FVector2D CenterPos = 0.5*(In_Points[i].StartLoc + In_Points[i].EndLoc);
			if (JudgePointInPolygon(Area[i].AreaPos, CenterPos))
			{
				Out_Points[i].IsRoomWall = false;
			}
		}
	}
}

FString UWallBuildSystem::GetvalidString(const FString Tag)
{
	FString OutStr;
	OutStr.Empty();
	if (Tag.IsEmpty())
	{
		return OutStr;
	}
	TCHAR  _Str = L' ';
	bool aaa = false;
	for (int i = 0; i < Tag.Len(); ++i)
	{
		if (aaa)
		{
			OutStr.Append(&Tag[i], 1);
		}

		if (Tag[i] != _Str && !aaa)
		{
			aaa = true;
			OutStr.Append(&Tag[i], 1);
		}
	}
	if (OutStr.Len() == 1)
	{
		if (OutStr[0] >= L'0'&&OutStr[0] <= L'9')
		{
			OutStr.Empty();
		}
	}
	return OutStr;
}

bool UWallBuildSystem::IsCanMoveWall(const int32& ConnerID)
{
	MoveHousePluginArr.Empty();
	if (BuildingSystem)
	{
		IValue *v = nullptr;
		v = BuildingSystem->GetProperty(ConnerID, "Walls");
		if (v)
		{
			kArray<int> WallIDs = v->IntArrayValue();
			if (WallIDs.size() > 0)
			{
				TArray<AActor*> HousePlugins;
				HousePlugins = UDRFunLibrary::GetAllHousePlugins(this);
				TArray<UBooleanBase*> TempBoolean = GetAllDeleteHoles();
				if (HousePlugins.Num() > 0 || TempBoolean.Num() > 0)
				{
					for (int i = 0; i < WallIDs.size(); ++i)
					{
						for (int k = 0; k < TempBoolean.Num(); ++k)
						{
							if (IsSetThisWall(WallIDs[i], FVector2D(TempBoolean[k]->DeleteHoleInfo.Loc)))
							{
								return false;
							}
						}
						TArray<AComponentManagerActor*> OutHousePluginArr;
						UBuildingData* WallData = BuildingSystem->GetData(WallIDs[i]);
						if (WallData)
						{
							int P0 = WallData->GetInt("P0");
							int P1 = WallData->GetInt("P1");
							IObject *CornerObj1 = BuildingSystem->GetObject(P0);
							IObject *CornerObj2 = BuildingSystem->GetObject(P1);
							if (CornerObj1&&CornerObj2)
							{
								bool IsP0;
								if (P0 == ConnerID)
								{
									IsP0 = true;
								}
								if (GetHousePluginOnSurfaceWall(IsP0, WallIDs[i], HousePlugins, OutHousePluginArr))
								{
									FVector2D StartCornerPos = ToVector2D(CornerObj1->GetPropertyValue("Location").Vec2Value());
									FVector2D EndCornerPos = ToVector2D(CornerObj2->GetPropertyValue("Location").Vec2Value());
									for (int j = 0; j < OutHousePluginArr.Num(); ++j)
									{
										if (OutHousePluginArr[j])
										{
											FComponenetInfo HouseCompInfo = OutHousePluginArr[j]->GetHouseComponentInfo();
											if (HouseCompInfo.WallID.Num() > 1)
											{
												MoveHousePluginArr.Empty();
												return false;
											}

											float StringLength = (HouseCompInfo.Loc - StartCornerPos).Size();
											float EdgeLength = UKismetMathLibrary::GetPointDistanceToSegment(FVector(HouseCompInfo.Loc, 0), FVector(StartCornerPos, 0), FVector(EndCornerPos, 0));
											float aaaa = StringLength / EdgeLength;
											if (StringLength / EdgeLength < 2)
											{
												MoveHousePluginArr.Empty();
												return false;
											}
											StringLength = (HouseCompInfo.Loc - EndCornerPos).Size();
											aaaa = StringLength / EdgeLength;
											if (StringLength / EdgeLength < 2)
											{
												MoveHousePluginArr.Empty();
												return false;
											}
											MoveHousePluginArr.AddUnique(OutHousePluginArr[j]);
										}
									}
								}
								else
								{
									MoveHousePluginArr.Empty();
									return false;
								}
							}
						}
					}
				}
			}
		}
	}
	return true;
}


void UWallBuildSystem::RefreshAlignmentPoint(TArray<float>&TempX, TArray<float>&TempY, float& Snap)
{
	TempX.Empty();
	TempY.Empty();
	for (int i = 0; i < WallCorners.Num(); ++i)
	{
		if (WallCorners[i])
		{
			TempX.Add(WallCorners[i]->CornerProperty.Position.X);
			TempY.Add(WallCorners[i]->CornerProperty.Position.Y);
		}
	}
	Snap = ObjectSnapTol;
}

bool UWallBuildSystem::GetHousePluginOnSurfaceWall(const bool& IsP0, const int32& WallID, const TArray<AActor*>&HousePlugins, TArray<AComponentManagerActor*>&OutHousePluginArr)
{
	if (BuildingSystem)
	{
		TArray<FVector> WallNodes;
		float height;
		BuildingSystem->GetWallBorderLines(WallID, WallNodes, height);
		if (WallNodes.Num() == 6)
		{
			FVector StartPos1, EndPos1;
			FVector StartPos2, EndPos2;
			StartPos1 = WallNodes[5];
			EndPos1 = WallNodes[4];
			StartPos2 = WallNodes[1];
			EndPos2 = WallNodes[2];
			for (int i = 0; i < HousePlugins.Num(); ++i)
			{
				if (HousePlugins[i])
				{
					AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(HousePlugins[i]);
					if (ComponentManager)
					{
						FComponenetInfo HouseCompInfo = ComponentManager->GetHouseComponentInfo();
						if (HouseCompInfo.ComponenetType != EComponenetType::_Sewer)
						{
							float length = UKismetMathLibrary::GetPointDistanceToSegment(FVector(HouseCompInfo.Loc, 0), StartPos1, EndPos1);
							if (HouseCompInfo.WallID.Num() == 1 && length < HouseCompInfo.Width*0.5 + 2)
							{
								TArray<int32> WallIDs;
								WallIDs.Add(WallID);
								ComponentManager->SetWallIdsInfo(WallIDs);
								ComponentManager->AlongWallLength = length;
								ComponentManager->MoveWallPos = FVector2D(UKismetMathLibrary::FindClosestPointOnSegment(FVector(HouseCompInfo.Loc, 0), StartPos1, EndPos1));
								if (IsP0)
								{
									ComponentManager->WallIndex = 5;
									ComponentManager->WallCornnerLength = (ComponentManager->MoveWallPos - FVector2D(StartPos1)).Size();
								}
								else
								{
									ComponentManager->WallIndex = 4;
									ComponentManager->WallCornnerLength = (ComponentManager->MoveWallPos - FVector2D(EndPos1)).Size();
								}
								OutHousePluginArr.AddUnique(ComponentManager);
							}
							else
								if (HouseCompInfo.WallID.Num() > 1)
								{
									if (length < HouseCompInfo.Length*0.5 + 2 || length < HouseCompInfo.Width*0.5 + 2)
									{
										TArray<int32> WallIDs;
										WallIDs.Add(WallID);
										WallIDs.Add(HouseCompInfo.WallID[1]);
										ComponentManager->SetWallIdsInfo(WallIDs);
										ComponentManager->AlongWallLength = length;
										ComponentManager->MoveWallPos = FVector2D(UKismetMathLibrary::FindClosestPointOnSegment(FVector(HouseCompInfo.Loc, 0), StartPos1, EndPos1));
										if (IsP0)
										{
											ComponentManager->WallIndex = 5;
											ComponentManager->WallCornnerLength = (ComponentManager->MoveWallPos - FVector2D(StartPos1)).Size();
										}
										else
										{
											ComponentManager->WallIndex = 4;
											ComponentManager->WallCornnerLength = (ComponentManager->MoveWallPos - FVector2D(EndPos1)).Size();
										}
										OutHousePluginArr.AddUnique(ComponentManager);
										return false;
									}
								}
							length = UKismetMathLibrary::GetPointDistanceToSegment(FVector(HouseCompInfo.Loc, 0), StartPos2, EndPos2);
							if (HouseCompInfo.WallID.Num() == 1 && length < HouseCompInfo.Width*0.5 + 2)
							{
								TArray<int32> WallIDs;
								WallIDs.Add(WallID);
								ComponentManager->SetWallIdsInfo(WallIDs);
								ComponentManager->AlongWallLength = length;
								ComponentManager->MoveWallPos = FVector2D(UKismetMathLibrary::FindClosestPointOnSegment(FVector(HouseCompInfo.Loc, 0), StartPos2, EndPos2));
								if (IsP0)
								{
									ComponentManager->WallIndex = 1;
									ComponentManager->WallCornnerLength = (ComponentManager->MoveWallPos - FVector2D(StartPos2)).Size();
								}
								else
								{
									ComponentManager->WallIndex = 2;
									ComponentManager->WallCornnerLength = (ComponentManager->MoveWallPos - FVector2D(EndPos2)).Size();
								}
								OutHousePluginArr.AddUnique(ComponentManager);
							}
							else
								if (HouseCompInfo.WallID.Num() > 1)
								{
									if (length < HouseCompInfo.Length*0.5 + 2 || length < HouseCompInfo.Width*0.5 + 2)
									{
										TArray<int32> WallIDs;
										WallIDs.Add(WallID);
										WallIDs.Add(HouseCompInfo.WallID[1]);
										ComponentManager->SetWallIdsInfo(WallIDs);
										ComponentManager->AlongWallLength = length;
										ComponentManager->MoveWallPos = FVector2D(UKismetMathLibrary::FindClosestPointOnSegment(FVector(HouseCompInfo.Loc, 0), StartPos2, EndPos2));
										if (IsP0)
										{
											ComponentManager->WallIndex = 1;
											ComponentManager->WallCornnerLength = (ComponentManager->MoveWallPos - FVector2D(StartPos2)).Size();
										}
										else
										{
											ComponentManager->WallIndex = 2;
											ComponentManager->WallCornnerLength = (ComponentManager->MoveWallPos - FVector2D(EndPos2)).Size();
										}
										OutHousePluginArr.AddUnique(ComponentManager);
										return false;
									}
								}
						}
					}
				}
			}
		}
	}
	return true;
}

void UWallBuildSystem::UpdateMoveHousePlugin()
{
	for (int i = 0; i < MoveHousePluginArr.Num(); ++i)
	{
		if (MoveHousePluginArr[i])
		{
			MoveHousePluginArr[i]->UpdateArea(MoveHousePluginArr[i]->GetComponenetInfo().RoomID);
			MoveHousePluginArr[i]->UpdateObject(MoveHousePluginArr[i]->GetComponenetInfo().WallID[0]);
		}
	}
}

void UWallBuildSystem::OnlyUpdateMoveHousePluginLoc()
{
	for (int i = 0; i < MoveHousePluginArr.Num(); ++i)
	{
		//MoveHousePluginArr[i]->UpdateArea(MoveHousePluginArr[i]->GetComponenetInfo().RoomID);
		MoveHousePluginArr[i]->UpdateObject(MoveHousePluginArr[i]->GetComponenetInfo().WallID[0]);
	}
}

void UWallBuildSystem::ClearMoveHousePluginArray()
{
	for (int i = 0; i < MoveHousePluginArr.Num(); ++i)
	{
		if (MoveHousePluginArr[i])
		{
			MoveHousePluginArr[i]->UpdateArea(MoveHousePluginArr[i]->GetComponenetInfo().RoomID);
			MoveHousePluginArr[i]->UpdateObject(MoveHousePluginArr[i]->GetComponenetInfo().WallID[0]);
		}
	}
	MoveHousePluginArr.Empty();
}

bool UWallBuildSystem::IsSelectHousePlugin()
{
	TArray<AActor*> HousePlugins;
	HousePlugins = UDRFunLibrary::GetAllHousePlugins(this);
	if (HousePlugins.Num() > 0)
	{
		for (int i = 0; i < HousePlugins.Num(); ++i)
		{
			if (HousePlugins[i])
			{
				AComponentManagerActor* ComponentManager = Cast<AComponentManagerActor>(HousePlugins[i]);
				if (ComponentManager)
				{
					if (ComponentManager->GetState() == EComponenetState::_Select)
						return true;
				}
			}
		}
	}
	return false;
}

void UWallBuildSystem::ShowLastWallLengthText(int32 SelectedWallID)
{
	SelectWallText = -1;
	HideAllWallLengthText();

	UDRProjData* ProjectDataManager = UDRProjData::GetProjectDataManager(this);
	if (ProjectDataManager == nullptr)
	{
		return;
	}

	UDRSolidWallAdapter* WallAdapter = Cast<UDRSolidWallAdapter>(ProjectDataManager->GetAdapter(SelectedWallID));
	if (WallAdapter == nullptr)
	{
		return;
	}
	int32 P0 = WallAdapter->GetP0();
	int32 P1 = WallAdapter->GetP1();
	UDRCornerAdapter* CornerAdapter_1 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(P0));
	if (CornerAdapter_1 == nullptr)
	{
		return;
	}
	TArray<int32> WallArray_1 = CornerAdapter_1->GetWalls();
	FVector2D CornerPos_P0 = CornerAdapter_1->GetLocaltion();

	UDRCornerAdapter* CornerAdapter_2 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(P1));
	if (CornerAdapter_2 == nullptr)
	{
		return;
	}
	TArray<int32> WallArray_2 = CornerAdapter_2->GetWalls();
	FVector2D CornerPos_P1 = CornerAdapter_2->GetLocaltion();
	if (WallArray_2.Num() != 1 && WallArray_1.Num() != 1)
	{
		return;
	}

	AWallPlaneBase** _TempWallPlane = WallPlaneMap.Find(SelectedWallID);
	if (_TempWallPlane)
	{
		AWallPlaneBase* TempWallPlane = *_TempWallPlane;
		if (TempWallPlane)
		{
			if (TempWallPlane->InputRulerWidget != nullptr)
			{
				SelectWallText = SelectedWallID;
				TempWallPlane->InputRulerWidget->SetVisibility(ESlateVisibility::Visible);
				TempWallPlane->InputRulerWidget->SetLabelTextLengthData(FVector2D::Distance(CornerPos_P0, CornerPos_P1));
				//TempWallPlane->InputRulerWidget->GetWidgetFromName("SpinBox_0")->SetKeyboardFocus();
			}
		}
	}
}

void UWallBuildSystem::HideAllWallLengthText()
{
	SelectWallText = -1;
	TArray<int32> WallIDs;
	GetAllObject(WallIDs, EObjectType::ESolidWall, false);
	for (int i = 0; i < WallIDs.Num(); i++)
	{
		UBuildingData *WallData = BuildingSystem->GetData(WallIDs[i]);
		if (WallData)
		{
			AWallPlaneBase** _TempWallPlane = WallPlaneMap.Find(WallData->GetID());
			if (_TempWallPlane)
			{
				AWallPlaneBase* TempWallPlane = *_TempWallPlane;
				if (TempWallPlane)
				{
					if (TempWallPlane->InputRulerWidget != nullptr)
					{
						TempWallPlane->InputRulerWidget->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
		}
	}
}

int32 UWallBuildSystem::UpdateTheWallLength(float CurrentLength)
{
	int32 LastPointIndex = -1;

	if (DrawWallCmd)
	{
		if (DrawWallCmd->WallPlaneTemp)
		{
			LastPointIndex = DrawWallCmd->WallPlaneTemp->LastCornerID;
		}

		if (LastPointIndex != -1)
		{
			UDRProjData* ProjectDataManager = UDRProjData::GetProjectDataManager(this);
			if (ProjectDataManager == nullptr)
			{
				CancelDrawWallOpertaion();
				return -1;
			}
			UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(LastPointIndex));
			if (CornerAdapter == nullptr)
			{
				CancelDrawWallOpertaion();
				return -1;
			}
			FVector2D CornerPos = CornerAdapter->GetLocaltion();

			if (!GetWorld() || !GetWorld()->GetFirstPlayerController())
			{
				CancelDrawWallOpertaion();
				return -1;
			}
			FVector MouseWorldPos, MouseDir;
			GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseWorldPos, MouseDir);
			FVector2D NewCornerPos = OrthogonalDraw(FVector2D(MouseWorldPos), LastPointIndex, true);
			FVector2D CornerDir = (NewCornerPos - CornerPos);
			CornerDir.Normalize();
			NewCornerPos = CornerPos + CornerDir * (CurrentLength / 10);

			FVector2D	BestLoc;
			int32		BestID = INDEX_NONE;
			int32 Pt = BuildingSystem->Snap(NewCornerPos, LastPointIndex, BestLoc, BestID, ObjectSnapTol, ObjectSnapTol * 2);
			if (Pt == kESnapType::kEPt)
			{
				DrawWallCmd->_SnapPoint.bActive = true;
				DrawWallCmd->_SnapPoint.BestID = BestID;
				DrawWallCmd->_SnapPoint.Loc = BestLoc;
				NewCornerPos = BestLoc;
				for (ACornerActor *CornerActor : GetCornersInScene())
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
					if (LastPointIndex != INDEX_NONE)
					{
						IObject *pCorner = BuildingSystem->GetObject(LastPointIndex);
						if (pCorner)
						{
							FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
							FVector2D Vec = NewCornerPos - Location;
							if (std::abs(Vec.Y) < ObjectSnapTol)
							{
								NewCornerPos = FVector2D(BestLoc.X, Location.Y);
							}
							else
								NewCornerPos = FVector2D(BestLoc.X, NewCornerPos.Y);
						}
					}
					else
					{
						NewCornerPos = FVector2D(BestLoc.X, NewCornerPos.Y);
					}
				}
				else if (Pt == kESnapType::kEDirX)
				{
					if (LastPointIndex != INDEX_NONE)
					{
						IObject *pCorner = BuildingSystem->GetObject(LastPointIndex);
						if (pCorner)
						{
							FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
							FVector2D Vec = NewCornerPos - Location;
							if (std::abs(Vec.X) < ObjectSnapTol)
							{
								NewCornerPos = FVector2D(Location.X, BestLoc.Y);
							}
							else
								NewCornerPos = FVector2D(NewCornerPos.X, BestLoc.Y);
						}
					}
					else
					{
						NewCornerPos = FVector2D(NewCornerPos.X, BestLoc.Y);
					}
				}
				else
				{
					if (LastPointIndex != INDEX_NONE)
					{
						IObject *pCorner = BuildingSystem->GetObject(LastPointIndex);
						if (pCorner)
						{
							FVector2D Location = ToVector2D(pCorner->GetPropertyValue("Location").Vec2Value());
							FVector2D Vec = NewCornerPos - Location;
							if (std::abs(Vec.X) < ObjectSnapTol)
							{
								NewCornerPos = FVector2D(Location.X, NewCornerPos.Y);
							}
							if (std::abs(Vec.Y) < ObjectSnapTol)
							{
								NewCornerPos = FVector2D(NewCornerPos.X, Location.Y);
							}
						}
					}
				}
			}

			int32 NewCornerIndex = -1;
			DrawWallCmd->CheckAndAddCornerWall(NewCornerPos, NewCornerIndex);
			CancelDrawWallOpertaion();
			return NewCornerIndex;
		}
	}

	if (LastPointIndex == -1 && SelectWallText != -1 && (DrawWallCmd == nullptr || DrawWallCmd->WallPlaneTemp == nullptr))
	{
		UDRProjData* ProjectDataManager = UDRProjData::GetProjectDataManager(this);
		if (ProjectDataManager == nullptr)
		{
			CancelDrawWallOpertaion();
			return -1;
		}

		UDRSolidWallAdapter* WallAdapter = Cast<UDRSolidWallAdapter>(ProjectDataManager->GetAdapter(SelectWallText));
		if (WallAdapter == nullptr)
		{
			CancelDrawWallOpertaion();
			return -1;
		}
		int32 P0 = WallAdapter->GetP0();
		int32 P1 = WallAdapter->GetP1();
		UDRCornerAdapter* CornerAdapter_1 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(P0));
		if (CornerAdapter_1 == nullptr)
		{
			CancelDrawWallOpertaion();
			return -1;
		}
		TArray<int32> WallArray_P0 = CornerAdapter_1->GetWalls();
		FVector2D CornerPos_P0 = CornerAdapter_1->GetLocaltion();

		UDRCornerAdapter* CornerAdapter_2 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(P1));
		if (CornerAdapter_2 == nullptr)
		{
			CancelDrawWallOpertaion();
			return -1;
		}
		TArray<int32> WallArray_P1 = CornerAdapter_2->GetWalls();
		FVector2D CornerPos_P1 = CornerAdapter_2->GetLocaltion();
		FVector2D MoveDelta;
		int32 MoveID = -1;
		bool MoveSuccess = false;
		if (WallArray_P0.Num() != 1 || (WallArray_P0.Num() == 1 && WallArray_P1.Num() == 1))
		{
			MoveID = P1;
			FVector2D CornerDir = (CornerPos_P1 - CornerPos_P0);
			//CornerDir.Normalize();
			CornerDir = CornerDir.GetSafeNormal();
			MoveDelta = CornerPos_P0 + CornerDir * (CurrentLength / 10.f) - CornerPos_P1;
			DeleteComponentInRange(SelectWallText, P1, CornerPos_P1 + MoveDelta);
			//MoveSuccess = BuildingSystem->Move(P1, MoveDelta);
			RebuildWall(SelectWallText, CornerPos_P0, CornerPos_P1 + MoveDelta);
		}
		if (WallArray_P1.Num() != 1)
		{
			MoveID = P0;
			//FVector2D CornerDir = (CornerPos_P1 - CornerPos_P0).GetSafeNormal();
			FVector2D CornerDir = (CornerPos_P0 - CornerPos_P1);
			//CornerDir.Normalize();
			CornerDir = CornerDir.GetSafeNormal();
			MoveDelta = CornerPos_P1 + CornerDir * (CurrentLength / 10.f) - CornerPos_P0;
			DeleteComponentInRange(SelectWallText, P0, CornerPos_P0 + MoveDelta);
			//MoveSuccess = BuildingSystem->Move(P0, MoveDelta);
			RebuildWall(SelectWallText, CornerPos_P1, CornerPos_P0 + MoveDelta);
		}
		BuildingSystem->Move(MoveID, FVector2D(0, 0));
		if (MoveSuccess)
		{
			ClearAllCornnerUMG();
			for (int i = 0; i < WallCorners.Num(); ++i)
			{
				if (WallCorners[i])
				{
					if (WallCorners[i]->CornerProperty.ObjectId == MoveID)
					{
						UDRCornerAdapter* TempCornerAdapter = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(WallCorners[i]->CornerProperty.ObjectId));
						if (TempCornerAdapter == nullptr)
							return -1;
						WallCorners[i]->CornerProperty.Position = TempCornerAdapter->GetLocaltion();
						WallCorners[i]->ShowOutterCircle(true);
						WallCorners[i]->ShowInnerCircel(true);
					}
				}
			}
			return MoveID;
		}
	}
	CancelDrawWallOpertaion();
	return -1;
}

void UWallBuildSystem::CancelDrawWallOpertaion()
{
	if (DrawWallCmd)
	{
		DrawWallCmd->OnRightMouseButtonDown();
		DrawWallCmd->OnRightMouseButtonUp();
	}
}

int32 UWallBuildSystem::GetDragWallID(FVector2D CurrentMousePos, int32 HitObjID)
{
	if (IsOverCornner(CurrentMousePos) != -1)
	{
		return -1;
	}
	if (LastMousePos.Equals(CurrentMousePos, 1))
	{
		return -1;
	}
	else if (LastMousePos == FVector2D(0, 0))
	{
		LastMousePos = CurrentMousePos;
	}

	if (HitObjID == INVALID_OBJID)
	{
		return -1;
	}

	int32 Local_SnapObj = BuildingSystem->HitTest(CurrentMousePos);
	if (Local_SnapObj != HitObjID)
	{
		return -1;
	}

	if (LastDragWallID != HitObjID)
	{
		LastMousePos = CurrentMousePos;
		LastDragWallID = HitObjID;
	}

	UBuildingData* WallData = BuildingSystem->GetData(HitObjID);
	if (!WallData)
	{
		return -1;
	}

	//UE_LOG(LogTemp, Log,TEXT("%d"), WallData->GetObjectType());
	if (WallData->GetObjectType() != EObjectType::ESolidWall && WallData->GetObjectType() != EObjectType::EEdge)
	{
		return -1;
	}

	MovingWallCorner.Empty();
	MovingVirtualWallCorner.Empty();
	int P0 = WallData->GetInt("P0");
	int P1 = WallData->GetInt("P1");
	IObject *CornerData1 = BuildingSystem->GetObject(P0);
	IObject *CornerData2 = BuildingSystem->GetObject(P1);
	if (!(IsCanMoveWall(P0) && IsCanMoveWall(P1)))
	{
		bIsDragCorner = false;
		return -1;
	}
	if (CornerData1 && CornerData2)
	{
		if (WallData->GetObjectType() == EObjectType::EEdge)
		{
			//hide selected wall corner
			ShowCorners.Empty();
			MovingVirtualWallCorner.Add(P0);
			MovingVirtualWallCorner.Add(P1);

			if (MovingVirtualWallCorner.Num() != 2)
				return -1;

			float DeltaPos = -9999;
			FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
			FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
			FVector2D DeltaFvector = (EndCornerPos - StartCornerPos);
			DeltaFvector.Normalize();
			float DotProduct = FVector2D::DotProduct(DeltaFvector, FVector2D(1, 0));
			int32 Angle = int32(Acos(DotProduct) / 3.14 * 180);
			//UE_LOG(LogTemp, Log, TEXT("%f"), Acos(DotProduct));

			if (((FMath::Abs(Angle) % 180) >= 165 && (FMath::Abs(Angle) % 180) <= 179) || ((FMath::Abs(Angle) % 180) < 15 && ((FMath::Abs(Angle) % 180) >= 0)))
			{
				DeltaPos = (CurrentMousePos.Y - LastMousePos.Y);
			}
			else if ((FMath::Abs(Angle) % 180) <= 105 && (FMath::Abs(Angle) % 180) >= 75)
			{
				DeltaPos = (CurrentMousePos.X - LastMousePos.X);
			}

			if (DeltaPos != -9999)
			{
				bIsDragCorner = true;
				PreMouseLoction = LastMousePos;
				bool bCorner1Move = CornerMovable(P0, CurrentMousePos);
				bool bCorner2Move = CornerMovable(P1, CurrentMousePos);
				if (bCorner1Move && bCorner2Move)
				{
					bIsDragCorner = false;
					SetIsDeleteTempArea(true);
					return HitObjID;
				}
			}
		}
		else
		{
			for (int i = 0; i < WallCorners.Num(); ++i)
			{
				if (WallCorners[i])
				{
					//WallCorners[i]->ShowOutterCircle(true);
					//WallCorners[i]->ShowInnerCircel(false);
					if (WallCorners[i]->CornerProperty.ObjectId == P0 || WallCorners[i]->CornerProperty.ObjectId == P1)
					{
						MovingWallCorner.Add(WallCorners[i]);
					}
				}
			}

			if (MovingWallCorner.Num() != 2)
				return -1;
			if (MovingWallCorner[0] && MovingWallCorner[1])
			{
				float DeltaPos = -9999;
				FVector2D StartCornerPos = MovingWallCorner[0]->CornerProperty.Position;
				FVector2D EndCornerPos = MovingWallCorner[1]->CornerProperty.Position;
				FVector2D DeltaFvector = (EndCornerPos - StartCornerPos);
				DeltaFvector.Normalize();
				float DotProduct = FVector2D::DotProduct(DeltaFvector, FVector2D(1, 0));
				int32 Angle = int32(Acos(DotProduct) / 3.14 * 180);
				//UE_LOG(LogTemp, Log, TEXT("%f"), Acos(DotProduct));

				if (((FMath::Abs(Angle) % 180) >= 165 && (FMath::Abs(Angle) % 180) <= 179) || ((FMath::Abs(Angle) % 180) < 15 && ((FMath::Abs(Angle) % 180) >= 0)))
				{
					DeltaPos = (CurrentMousePos.Y - LastMousePos.Y);
				}
				else if ((FMath::Abs(Angle) % 180) <= 105 && (FMath::Abs(Angle) % 180) >= 75)
				{
					DeltaPos = (CurrentMousePos.X - LastMousePos.X);
				}

				if (DeltaPos != -9999)
				{
					bIsDragCorner = true;
					PreMouseLoction = LastMousePos;
					bool bCorner1Move = CornerMovable(MovingWallCorner[0]->CornerProperty.ObjectId, CurrentMousePos);
					bool bCorner2Move = CornerMovable(MovingWallCorner[1]->CornerProperty.ObjectId, CurrentMousePos);
					if (bCorner1Move && bCorner2Move)
					{
						bIsDragCorner = false;
						return HitObjID;
					}
				}
			}
		}

	}
	bIsDragCorner = false;
	return -1;
}

void UWallBuildSystem::DragTheWallMove(FVector2D CurrentMousePos, int32 WallID)
{
	float DeltaPos = -9999;
	bool bDeltaX = false;
	if (MovingWallCorner.Num() != 2)
		return;

	if (MovingWallCorner[0] && MovingWallCorner[1])
	{
		//The wall is deleted in moving action or the corner is deleted.
		if (nullptr == BuildingSystem->GetObject(WallID)
			|| nullptr == BuildingSystem->GetObject(MovingWallCorner[0]->CornerProperty.ObjectId) || nullptr == BuildingSystem->GetObject(MovingWallCorner[1]->CornerProperty.ObjectId))
		{
			return;
		}
		FVector2D StartCornerPos = MovingWallCorner[0]->CornerProperty.Position;
		FVector2D EndCornerPos = MovingWallCorner[1]->CornerProperty.Position;
		FVector2D DeltaFvector = (EndCornerPos - StartCornerPos);
		DeltaFvector.Normalize();
		float DotProduct = FVector2D::DotProduct(DeltaFvector, FVector2D(1, 0));
		int32 Angle = int32(Acos(DotProduct) / 3.14 * 180);
		//UE_LOG(LogTemp, Log, TEXT("%f"), Acos(DotProduct));

		if (((FMath::Abs(Angle) % 180) >= 165 && (FMath::Abs(Angle) % 180) <= 179) || ((FMath::Abs(Angle) % 180) < 15 && ((FMath::Abs(Angle) % 180) >= 0)))
		{
			DeltaPos = (CurrentMousePos.Y - LastMousePos.Y);
			bDeltaX = false;
		}
		else if ((FMath::Abs(Angle) % 180) <= 105 && (FMath::Abs(Angle) % 180) >= 75)
		{
			DeltaPos = (CurrentMousePos.X - LastMousePos.X);
			bDeltaX = true;
		}

		//if(DeltaPos != 0)
		if (DeltaPos != -9999 && DeltaPos != 0)
		{
			//bIsDragCorner = true;
			//PreMouseLoction = LastMousePos;
			//bool bCorner1Move = CornerMovable(MovingWallCorner[0]->CornerProperty.ObjectId, CurrentMousePos);
			//bool bCorner2Move = CornerMovable(MovingWallCorner[1]->CornerProperty.ObjectId, CurrentMousePos);
			//if (bCorner1Move && bCorner2Move)
			{
				FVector2D DeltaVector;
				bool bCor1Success = false;
				bool bCor2Success = false;
				SetHoleWidgetMoveStates(true);
				if (bDeltaX)
				{
					DeltaVector = FVector2D(DeltaPos, 0);
					IsInRangeWallComponent(WallID, MovingWallCorner[0]->CornerProperty.ObjectId, StartCornerPos + DeltaVector + FVector2D(10, 0));
					bCor1Success = BuildingSystem->Move(MovingWallCorner[0]->CornerProperty.ObjectId, DeltaVector);
					BuildingSystem->Move(MovingWallCorner[0]->CornerProperty.ObjectId, FVector2D(0, 0));
					IsInRangeWallComponent(WallID, MovingWallCorner[1]->CornerProperty.ObjectId, EndCornerPos + DeltaVector + FVector2D(10, 0));
					bCor2Success = BuildingSystem->Move(MovingWallCorner[1]->CornerProperty.ObjectId, DeltaVector);
					BuildingSystem->Move(MovingWallCorner[1]->CornerProperty.ObjectId, FVector2D(0, 0));


				}
				else
				{
					DeltaVector = FVector2D(0, DeltaPos);
					IsInRangeWallComponent(WallID, MovingWallCorner[0]->CornerProperty.ObjectId, StartCornerPos + DeltaVector + FVector2D(0, 10));
					bCor1Success = BuildingSystem->Move(MovingWallCorner[0]->CornerProperty.ObjectId, DeltaVector);
					BuildingSystem->Move(MovingWallCorner[0]->CornerProperty.ObjectId, FVector2D(0, 0));
					IsInRangeWallComponent(WallID, MovingWallCorner[1]->CornerProperty.ObjectId, EndCornerPos + DeltaVector + FVector2D(0, 10));
					bCor2Success = BuildingSystem->Move(MovingWallCorner[1]->CornerProperty.ObjectId, DeltaVector);
					BuildingSystem->Move(MovingWallCorner[1]->CornerProperty.ObjectId, FVector2D(0, 0));
				}
				if (!bCor2Success && bCor1Success)
				{
					BuildingSystem->Move(MovingWallCorner[0]->CornerProperty.ObjectId, -DeltaVector);
					BuildingSystem->Move(MovingWallCorner[0]->CornerProperty.ObjectId, FVector2D(0, 0));
				}
				if (!bCor1Success && bCor2Success)
				{
					BuildingSystem->Move(MovingWallCorner[1]->CornerProperty.ObjectId, -DeltaVector);
					BuildingSystem->Move(MovingWallCorner[1]->CornerProperty.ObjectId, FVector2D(0,0));
				}
				//if (bCor2Success && bCor1Success)
				//{
					LastMousePos = CurrentMousePos;
				//}
				for (int32 i = 0; i < MovingWallCorner.Num(); i++)
				{
					for (int j = 0; j < WallCorners.Num(); ++j)
					{
						if (WallCorners[j]->CornerProperty.ObjectId == MovingWallCorner[i]->CornerProperty.ObjectId)
						{
							MovingWallCorner[i]->ShowOutterCircle(true);
							MovingWallCorner[i]->ShowInnerCircel(false);
							IObject *pObj = BuildingSystem->GetObject(MovingWallCorner[i]->CornerProperty.ObjectId);
							if (pObj != nullptr)
							{
								MovingWallCorner[i]->CornerProperty.Position = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
							}
							break;
						}
					}
				}
			}
		}
	}
	//bIsDragCorner = false;
}

bool UWallBuildSystem::DragTheVirtualWallMove(FVector2D CurrentMousePos, int32 WallID)
{
	float DeltaPos = -9999;
	bool bDeltaX = false;
	bool isMoved = false;

	if (MovingVirtualWallCorner.Num() != 2)
		return isMoved;

	//The wall is deleted in moving action or the corner is deleted.
	if (nullptr == BuildingSystem->GetObject(WallID)
		|| nullptr == BuildingSystem->GetObject(MovingVirtualWallCorner[0]) || nullptr == BuildingSystem->GetObject(MovingVirtualWallCorner[1]))
	{
		return isMoved;
	}

	IObject *CornerData1 = BuildingSystem->GetObject(MovingVirtualWallCorner[0]);
	IObject *CornerData2 = BuildingSystem->GetObject(MovingVirtualWallCorner[1]);

	FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
	FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
	FVector2D DeltaFvector = (EndCornerPos - StartCornerPos);
	DeltaFvector.Normalize();
	float DotProduct = FVector2D::DotProduct(DeltaFvector, FVector2D(1, 0));
	int32 Angle = int32(Acos(DotProduct) / 3.14 * 180);
	//UE_LOG(LogTemp, Log, TEXT("%f"), Acos(DotProduct));

	if (((FMath::Abs(Angle) % 180) >= 165 && (FMath::Abs(Angle) % 180) <= 179) || ((FMath::Abs(Angle) % 180) < 15 && ((FMath::Abs(Angle) % 180) >= 0)))
	{
		DeltaPos = (CurrentMousePos.Y - LastMousePos.Y);
		bDeltaX = false;
	}
	else if ((FMath::Abs(Angle) % 180) <= 105 && (FMath::Abs(Angle) % 180) >= 75)
	{
		DeltaPos = (CurrentMousePos.X - LastMousePos.X);
		bDeltaX = true;
	}

	if (DeltaPos != -9999 && DeltaPos != 0)
	{
		bool bCorner1Move = true;
		bool bCorner2Move = true;
		bIsDragCorner = true;
		int32 TempID_0, TempID_1;
		if (!GetReleatedCorner(MovingVirtualWallCorner[0], TempID_0))
		{
			return false;
		}
		if (!GetReleatedCorner(MovingVirtualWallCorner[1], TempID_1))
		{
			return false;
		}

		TArray<int32> MoveVirtualWallCorner;
		if (TempID_0 != -1)
		{
			MoveVirtualWallCorner.Add(TempID_0);
		}
		else
		{
			MoveVirtualWallCorner.Add(MovingVirtualWallCorner[0]);
		}
		if (TempID_1 != -1)
		{
			MoveVirtualWallCorner.Add(TempID_1);
		}
		else
		{
			MoveVirtualWallCorner.Add(MovingVirtualWallCorner[1]);
		}

		//if (bCorner1Move)
		//	bCorner1Move = CornerMovable(MoveVirtualWallCorner[0], StartCornerPos);
		//if (bCorner2Move)
		//	bCorner2Move = CornerMovable(MoveVirtualWallCorner[1], EndCornerPos);
		if (bCorner1Move && bCorner2Move)
		{
			FVector2D DeltaVector;
			bool bCor1Success = false;
			bool bCor2Success = false;
			SetHoleWidgetMoveStates(true);
			if (bDeltaX)
			{
				DeltaVector = FVector2D(DeltaPos, 0);
				if (MoveVirtualWallCorner[0] != MovingVirtualWallCorner[0])
				{
					IObject *CornerData = BuildingSystem->GetObject(MoveVirtualWallCorner[0]);
					if (CornerData)
					{
						FVector2D CornerPos = ToVector2D(CornerData->GetPropertyValue("Location").Vec2Value());
						if (!IsInRangeWallComponent(MoveVirtualWallCorner[0], CornerPos + DeltaVector + FVector2D(10, 0)))
						{
							bCor1Success = BuildingSystem->Move(MoveVirtualWallCorner[0], DeltaVector);
						}
						else
						{
							bCor1Success = false;
						}
					}
					else
					{
						bCor1Success = false;
					}
				}
				else
				{
					bCor1Success = BuildingSystem->Move(MoveVirtualWallCorner[0], DeltaVector);
				}

				if (MoveVirtualWallCorner[1] != MovingVirtualWallCorner[1])
				{
					IObject *CornerData = BuildingSystem->GetObject(MoveVirtualWallCorner[1]);
					if (CornerData)
					{
						FVector2D CornerPos = ToVector2D(CornerData->GetPropertyValue("Location").Vec2Value());
						if (!IsInRangeWallComponent(MoveVirtualWallCorner[1], CornerPos + DeltaVector + FVector2D(10, 0)))
						{
							bCor2Success = BuildingSystem->Move(MoveVirtualWallCorner[1], DeltaVector);
						}
						else
						{
							bCor2Success = false;
						}
					}
					else
					{
						bCor2Success = false;
					}
				}
				else
				{
					bCor2Success = BuildingSystem->Move(MoveVirtualWallCorner[1], DeltaVector);
				}
			}
			else
			{
				DeltaVector = FVector2D(0, DeltaPos);
				if (MoveVirtualWallCorner[0] != MovingVirtualWallCorner[0])
				{
					IObject *CornerData = BuildingSystem->GetObject(MoveVirtualWallCorner[0]);
					if (CornerData)
					{
						FVector2D CornerPos = ToVector2D(CornerData->GetPropertyValue("Location").Vec2Value());
						if (!IsInRangeWallComponent(MoveVirtualWallCorner[0], CornerPos + DeltaVector + FVector2D(0, 10)))
						{
							bCor1Success = BuildingSystem->Move(MoveVirtualWallCorner[0], DeltaVector);
						}
						else
						{
							bCor1Success = false;
						}
					}
					else
					{
						bCor1Success = false;
					}
				}
				else
				{
					bCor1Success = BuildingSystem->Move(MoveVirtualWallCorner[0], DeltaVector);
				}

				if (MoveVirtualWallCorner[1] != MovingVirtualWallCorner[1])
				{
					IObject *CornerData = BuildingSystem->GetObject(MoveVirtualWallCorner[1]);
					if (CornerData)
					{
						FVector2D CornerPos = ToVector2D(CornerData->GetPropertyValue("Location").Vec2Value());
						if (!IsInRangeWallComponent(MoveVirtualWallCorner[1], CornerPos + DeltaVector + FVector2D(0, 10)))
						{
							bCor2Success = BuildingSystem->Move(MoveVirtualWallCorner[1], DeltaVector);
						}
						else
						{
							bCor2Success = false;
						}
					}
					else
					{
						bCor2Success = false;
					}
				}
				else
				{
					bCor2Success = BuildingSystem->Move(MoveVirtualWallCorner[1], DeltaVector);
				}
			}
			if (!bCor2Success && bCor1Success)
			{
				isMoved = false;
				BuildingSystem->Move(MoveVirtualWallCorner[0], -DeltaVector);
			}
			if (!bCor1Success && bCor2Success)
			{
				isMoved = false;
				BuildingSystem->Move(MoveVirtualWallCorner[1], -DeltaVector);
			}
			if (bCor2Success && bCor1Success)
			{
				isMoved = true;
			}
			LastMousePos = CurrentMousePos;
			for (int32 i = 0; i < MoveVirtualWallCorner.Num(); i++)
			{
				for (int j = 0; j < WallCorners.Num(); ++j)
				{
					if (WallCorners[j]->CornerProperty.ObjectId == MoveVirtualWallCorner[i])
					{
						WallCorners[j]->ShowOutterCircle(true);
						WallCorners[j]->ShowInnerCircel(false);
						IObject *pObj = BuildingSystem->GetObject(MoveVirtualWallCorner[i]);
						if (pObj != nullptr)
						{
							WallCorners[j]->CornerProperty.Position = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
						}
						break;
					}
				}
			}

		}
	}

	CorrectCornerPos(MovingVirtualWallCorner[0]);
	CorrectCornerPos(MovingVirtualWallCorner[1]);
	bIsDragCorner = false;
	return isMoved;
}

void UWallBuildSystem::IsInRangeWallComponent(int32 WallID, int32 CornerID, FVector2D Loc)
{
	IValue *v = nullptr;
	v = BuildingSystem->GetProperty(CornerID, "Walls");
	if (v)
	{
		kArray<int> WallIDs = v->IntArrayValue();
		for (int i = 0; i < WallIDs.size(); i++)
		{
			if (WallIDs[i] != WallID)
			{
				UBuildingData *WallData = BuildingSystem->GetData(WallIDs[i]);
				if (WallData)
				{
					int P0 = WallData->GetInt("P0");
					IObject *CornerData1 = BuildingSystem->GetObject(P0);
					if (CornerData1)
					{
						bool bDeleteMin = false;
						FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
						FVector2D Range = WallData->GetVector2D("Range");
						if (!Range.Equals(FVector2D::ZeroVector))
						{
							float Dist = FVector2D::Distance(Loc, StartCornerPos);
							if (Dist <Range.X || Dist > Range.Y)
							{
								continue;
							}
							if ((FVector2D::Distance(Loc, StartCornerPos)) <= Range.Y && FVector2D::Distance(Loc, StartCornerPos) > (Range.Y - 20))
							{
								bDeleteMin = false;
							}
							else if (FVector2D::Distance(Loc, StartCornerPos) >= Range.X)
							{
								bDeleteMin = true;
							}
							IValue *Wallv = nullptr;
							Wallv = BuildingSystem->GetProperty(WallIDs[i], "Holes");
							if (Wallv)
							{
								int num = Wallv->GetArrayCount();
								if (num > 0)
								{
									for (size_t i = 0; i < num; i++)
									{
										UFeatureWall::WallAnnex hole;
										IValue& val = Wallv->GetField(i);
										const EObjectType objType = (EObjectType)(val.GetField("ObjType").IntValue());
										const int objID = val.GetField("HoleID").IntValue();
										IObject *CompData = BuildingSystem->GetObject(objID);
										if (nullptr == CompData)
											continue;

										FVector2D CompVector = ToVector2D(CompData->GetPropertyValue("Location").Vec2Value());
										float CompDist = FVector2D::Distance(StartCornerPos, CompVector);
										float CompWidth = CompData->GetPropertyValue("Width").FloatValue() / 2;
										if (bDeleteMin)
										{
											if (CompDist - CompWidth == Range.X)
											{
												BuildingSystem->DeleteObject(objID);
											}
										}
										else
										{
											if (CompDist + CompWidth == Range.Y)
											{
												BuildingSystem->DeleteObject(objID);
											}
										}
									}
								}
							}
						}
						else
						{
							continue;
						}
					}
				}
			}
		}
	}
}

bool UWallBuildSystem::IsInRangeWallComponent(int32 CornerID, FVector2D Loc)
{
	IValue *v = nullptr;
	v = BuildingSystem->GetProperty(CornerID, "Walls");
	if (v)
	{
		kArray<int> WallIDs = v->IntArrayValue();
		for (int i = 0; i < WallIDs.size(); i++)
		{
			UBuildingData *WallData = BuildingSystem->GetData(WallIDs[i]);
			if (WallData)
			{
				int P0 = WallData->GetInt("P0");
				IObject *CornerData1 = BuildingSystem->GetObject(P0);
				if (CornerData1)
				{
					bool bDeleteMin = false;
					FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
					FVector2D Range = WallData->GetVector2D("Range");
					if (!Range.Equals(FVector2D::ZeroVector))
					{
						float Dist = FVector2D::Distance(Loc, StartCornerPos);
						if (Dist <Range.X || Dist > Range.Y)
						{
							continue;
						}
						if ((FVector2D::Distance(Loc, StartCornerPos)) <= Range.Y && FVector2D::Distance(Loc, StartCornerPos) > (Range.Y - 20))
						{
							bDeleteMin = false;
						}
						else if (FVector2D::Distance(Loc, StartCornerPos) >= Range.X)
						{
							bDeleteMin = true;
						}
						IValue *Wallv = nullptr;
						Wallv = BuildingSystem->GetProperty(WallIDs[i], "Holes");
						if (Wallv)
						{
							int num = Wallv->GetArrayCount();
							if (num > 0)
							{
								for (size_t i = 0; i < num; i++)
								{
									UFeatureWall::WallAnnex hole;
									IValue& val = Wallv->GetField(i);
									const EObjectType objType = (EObjectType)(val.GetField("ObjType").IntValue());
									const int objID = val.GetField("HoleID").IntValue();
									IObject *CompData = BuildingSystem->GetObject(objID);
									if (nullptr == CompData)
										continue;

									FVector2D CompVector = ToVector2D(CompData->GetPropertyValue("Location").Vec2Value());
									float CompDist = FVector2D::Distance(StartCornerPos, CompVector);
									float CompWidth = CompData->GetPropertyValue("Width").FloatValue() / 2;
									if (bDeleteMin)
									{
										if (CompDist - CompWidth >= Range.X)
										{
											return true;
										}
									}
									else
									{
										if (CompDist + CompWidth <= Range.Y)
										{
											return true;
										}
									}
								}
							}
						}
					}
					else
					{
						continue;
					}
				}
			}
		}
	}
	return false;
}

void UWallBuildSystem::DeleteComponentInRange(int32 WallID, int32 CornerID, FVector2D Loc) const
{
	IValue *v = nullptr;
	v = BuildingSystem->GetProperty(CornerID, "Walls");
	if (v)
	{
		kArray<int> WallIDs = v->IntArrayValue();
		if (WallIDs.size() != 1)
			return;
		else if (WallIDs[0] != WallID)
			return;


		UDRProjData* ProjectDataManager = UDRProjData::GetProjectDataManager(this);
		if (ProjectDataManager == nullptr)
			return;

		UDRSolidWallAdapter* WallAdapter = Cast<UDRSolidWallAdapter>(ProjectDataManager->GetAdapter(WallID));
		if (WallAdapter == nullptr)
			return;
		int32 StartCornerID = WallAdapter->GetP0();
		int32 EndCornerID = WallAdapter->GetP1();
		UDRCornerAdapter* CornerAdapter_P0 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(StartCornerID));
		if (CornerAdapter_P0 == nullptr)
			return;
		FVector2D StartCornerPos = CornerAdapter_P0->GetLocaltion();
		UDRCornerAdapter* CornerAdapter_P1 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(EndCornerID));
		if (CornerAdapter_P1 == nullptr)
			return;
		FVector2D EndCornerPos = CornerAdapter_P1->GetLocaltion();
		bool bNarrow;
		if (StartCornerID == CornerID)
		{
			if (FVector2D::Distance(EndCornerPos, Loc) < FVector2D::Distance(EndCornerPos, StartCornerPos))
				bNarrow = true;
			else
				return;
		}
		else if (EndCornerID == CornerID)
		{
			if (FVector2D::Distance(StartCornerPos, Loc) < FVector2D::Distance(StartCornerPos, EndCornerPos))
				bNarrow = true;
			else
				return;
		}
		else
			return;

		TArray<int32>HolesArray = WallAdapter->GetHoles();
		for (int32 i = 0; i < HolesArray.Num(); i++)
		{
			UDRHoleAdapter* HoleAdapter = Cast<UDRHoleAdapter>(ProjectDataManager->GetAdapter(HolesArray[i]));
			if (HoleAdapter == nullptr)
				return;
			FVector2D HoleLoc = FVector2D(HoleAdapter->GetTransform().GetLocation());
			int32 HoleWidth = HoleAdapter->GetWidth();
			if (StartCornerID != CornerID)
			{
				if (FVector2D::Distance(StartCornerPos, Loc) <= (FVector2D::Distance(StartCornerPos, HoleLoc) + HoleWidth / 2))
					BuildingSystem->DeleteObject(HolesArray[i]);
			}
			else if (EndCornerID != CornerID)
			{
				if (FVector2D::Distance(EndCornerPos, Loc) <= (FVector2D::Distance(EndCornerPos, HoleLoc) + HoleWidth / 2))
					BuildingSystem->DeleteObject(HolesArray[i]);
			}
		}
	}
}

FVector2D UWallBuildSystem::KeepHolesPosition(int32 WallID, float Length)
{
	//float DeltaPos = -9999;
	IValue *Wallv = nullptr;
	UBuildingData* WallData = BuildingSystem->GetData(WallID);
	if (WallData->GetObjectType() == EObjectType::ESolidWall)
	{
		bool bDeltaX = false;
		int P0 = WallData->GetInt("P0");
		int P1 = WallData->GetInt("P1");
		IObject *CornerData1 = BuildingSystem->GetObject(P0);
		IObject *CornerData2 = BuildingSystem->GetObject(P1);
		if (CornerData1 && CornerData2)
		{
			FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
			FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
			FVector2D DeltaFvector = (EndCornerPos - StartCornerPos);
			DeltaFvector.Normalize();
			float DotProduct = FVector2D::DotProduct(DeltaFvector, FVector2D(1, 0));
			//int32 Angle = int32(Acos(DotProduct) / 3.14 * 180);
			float Angle;
			if ((DotProduct > 0 && DeltaFvector.X >= 0 && DeltaFvector.Y >= 0) || (DotProduct < 0 && DeltaFvector.X <= 0 && DeltaFvector.Y >= 0)
				|| (DotProduct == 0 && DeltaFvector.Y >= 0) || (DotProduct == 1 && DeltaFvector.X >= 0))
			{
				Angle = float(Acos(DotProduct));
				FVector2D CompCurrentVec = FVector2D(StartCornerPos.X + cos(Angle) * (Length), StartCornerPos.Y + sin(Angle)* (Length));
				return CompCurrentVec;
			}
			else if ((DotProduct < 0 && DeltaFvector.X <= 0 && DeltaFvector.Y <= 0) || (DotProduct > 0 && DeltaFvector.X >= 0 && DeltaFvector.Y <= 0)
				|| (DotProduct == 0 && DeltaFvector.Y < 0) || (DotProduct == -1 && DeltaFvector.X < 0))
			{
				Angle = float(-Acos(DotProduct));
				FVector2D CompCurrentVec = FVector2D(StartCornerPos.X + cos(Angle) * (Length), StartCornerPos.Y + sin(Angle)* (Length));
				return CompCurrentVec;
			}
		}
	}
	return FVector2D::ZeroVector;
}

void UWallBuildSystem::GetHolesPosition(int32 WallID)
{
	HolesPosition.Empty();
	UBuildingData *WallData = BuildingSystem->GetData(WallID);
	if (WallData)
	{
		int P0 = WallData->GetInt("P0");
		IObject *CornerData1 = BuildingSystem->GetObject(P0);
		if (CornerData1)
		{
			bool bDeleteMin = false;
			FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
			FVector2D Range = WallData->GetVector2D("Range");
			if (!Range.Equals(FVector2D::ZeroVector))
			{
				IValue *Wallv = nullptr;
				Wallv = BuildingSystem->GetProperty(WallID, "Holes");
				if (Wallv)
				{
					int num = Wallv->GetArrayCount();
					if (num > 0)
					{
						for (size_t i = 0; i < num; i++)
						{
							UFeatureWall::WallAnnex hole;
							IValue& val = Wallv->GetField(i);
							const EObjectType objType = (EObjectType)(val.GetField("ObjType").IntValue());
							const int objID = val.GetField("HoleID").IntValue();
							IObject *CompData = BuildingSystem->GetObject(objID);
							if (nullptr == CompData)
								continue;

							FVector2D CompVector = ToVector2D(CompData->GetPropertyValue("Location").Vec2Value());
							HolesPosition.Add(objID, FVector2D::Distance(StartCornerPos, CompVector));
						}
					}
				}
			}
		}
	}
}

void UWallBuildSystem::UpdateHolesObjectPosition(int32 WallID)
{
	if (WallID == -1)
		return;
	if (!bMoving)
	{
		return;
	}
	IValue *Wallv = nullptr;
	UBuildingData* WallData = BuildingSystem->GetData(WallID);
	if (WallData == nullptr)
		return;
	if (WallData->GetObjectType() == EObjectType::ESolidWall)
	{
		bool bDeltaX = false;
		int P0 = WallData->GetInt("P0");
		int P1 = WallData->GetInt("P1");
		IObject *CornerData1 = BuildingSystem->GetObject(P0);
		IObject *CornerData2 = BuildingSystem->GetObject(P1);
		if (CornerData1 && CornerData2)
		{
			FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
			FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
			FVector2D DeltaFvector = (EndCornerPos - StartCornerPos);
			DeltaFvector.Normalize();
			float DotProduct = FVector2D::DotProduct(DeltaFvector, FVector2D(1, 0));
			//int32 Angle = int32(Acos(DotProduct) / 3.14 * 180);
			//float Angle = float(Acos(DotProduct));
			float Angle;
			if ((DotProduct > 0 && DeltaFvector.X >= 0 && DeltaFvector.Y >= 0) || (DotProduct < 0 && DeltaFvector.X <= 0 && DeltaFvector.Y >= 0)
				|| (DotProduct == 0 && DeltaFvector.Y >= 0) || (DotProduct == 1 && DeltaFvector.X >= 0))
			{
				Angle = float(Acos(DotProduct));
			}
			else if ((DotProduct < 0 && DeltaFvector.X <= 0 && DeltaFvector.Y <= 0) || (DotProduct > 0 && DeltaFvector.X >= 0 && DeltaFvector.Y <= 0)
				|| (DotProduct == 0 && DeltaFvector.Y < 0) || (DotProduct == -1 && DeltaFvector.X < 0))
			{
				Angle = float(-Acos(DotProduct));
			}

			for (TMap<int32, float>::TConstIterator TempObj(HolesPosition); TempObj;++TempObj)
			{
				UBuildingData *HoleData = BuildingSystem->GetData(TempObj->Key);
				if (HoleData)
				{
					FVector2D CompCurrentVec = FVector2D(StartCornerPos.X + cos(Angle) * (TempObj->Value), StartCornerPos.Y + sin(Angle)* (TempObj->Value));
					HoleData->SetVector2D("Location", CompCurrentVec);
				}
				//PinCornerData->SetVector2D("Location", Loc);
			}
		}
	}
}

bool UWallBuildSystem::IsPointOnLine(FVector2D PointVec, FVector2D LineStartPoint, FVector2D LineEndPoint)
{

	float k1;
	float b1;
	if ((LineStartPoint.X - LineEndPoint.X) > -0.0001 && (LineStartPoint.X - LineEndPoint.X) < 0.0001)
	{
		k1 = -99999;
		b1 = LineStartPoint.X;
	}
	else
	{
		k1 = (LineStartPoint.Y - LineEndPoint.Y) / (LineStartPoint.X - LineEndPoint.X);
		b1 = LineStartPoint.Y - k1 * LineStartPoint.X;
	}

	if (k1 == -99999)
	{
		return FMath::Abs(PointVec.X - b1) <= 1;
	}
	else if (k1 == 0)
	{
		return  FMath::Abs(PointVec.Y - b1) <= 1;
	}
	else
	{
		return FMath::Abs(PointVec.Y - (k1 * PointVec.X + b1)) <= 1;
	}
}

void UWallBuildSystem::GetAllHolesObject()
{
	AllHolesObject.Empty();
	for (TMap<int32, UObject*>::TConstIterator TempObj(UDRProjData::GetProjectDataManager(this)->DoorHoleMap); TempObj; ++TempObj)
	{
		AllHolesObject.Add(TempObj->Key, TempObj->Value);
	}

	for (TMap<int32, UObject*>::TConstIterator TempObj(UDRProjData::GetProjectDataManager(this)->WindowHoleMap); TempObj; ++TempObj)
	{
		AllHolesObject.Add(TempObj->Key, TempObj->Value);
	}
}

void UWallBuildSystem::GetCornerWindow(const int32 WallID)
{
	/*FVector MouseWorldPos, MouseDir;
		FVector2D SMouse = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
		GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(SMouse.X, SMouse.Y, MouseWorldPos, MouseDir);
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
		if (Local_DRProjData)
		{
			UDRSolidWallAdapter* Local_SolidWallAdapter = nullptr;
			Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(WallID));
			if (Local_SolidWallAdapter)
			{
				int RoomRightID = Local_SolidWallAdapter->GetRoomRightId();
				int RoomLeftID = Local_SolidWallAdapter->GetRoomLeftId();
				if ((RoomRightID > 0 && RoomLeftID <= 0) || (RoomRightID <= 0 && RoomLeftID > 0))
				{
					bool IsRight = true;
					FDRWallPosition DRWallPosition = Local_SolidWallAdapter->GetWallPositionData();
					if (RoomLeftID > 0)
					{
						IsRight = false;
					}
					if ((FVector2D(DRWallPosition.StartPos) - FVector2D(DRWallPosition.EndPos)).Size() > 30)
					{
						UDRCornerAdapter* Local_CornerStartAdapter = Cast<UDRCornerAdapter>(Local_DRProjData->GetAdapter(Local_SolidWallAdapter->GetP0()));
						UDRCornerAdapter* Local_CornerEndAdapter = Cast<UDRCornerAdapter>(Local_DRProjData->GetAdapter(Local_SolidWallAdapter->GetP1()));
						if (Local_CornerStartAdapter&&Local_CornerEndAdapter)
						{
							FVector StartPos = FVector(Local_SolidWallAdapter->GetStartPos(), 0);
							FVector EndPos = FVector(Local_SolidWallAdapter->GetEndPos(), 0);
							MouseWorldPos.Z = 0;
							FVector2D TempLoc = FVector2D(FMath::ClosestPointOnSegment(MouseWorldPos, StartPos, EndPos));
							float StartLength = (FVector2D(MouseWorldPos) - Local_CornerStartAdapter->GetLocaltion()).Size();
							float EndLength = (FVector2D(MouseWorldPos) - Local_CornerEndAdapter->GetLocaltion()).Size();
							UDRCornerAdapter* Local_BestCornerAdapter = nullptr;
							bool bIsStartCorner = true;
							if (StartLength > EndLength)
							{
								bIsStartCorner = false;
							}
							TArray<int32> Wallnum = Local_BestCornerAdapter->GetWalls();
							if (Wallnum.Num() == 2)
							{
								bool bSet = true;
								FVector2D CurrentWallDir = FVector2D::ZeroVector;
								FVector2D NextWallDir = FVector2D::ZeroVector;
								for (int i = 0; i < Wallnum.Num(); ++i)
								{
									if (Wallnum[i] != WallID)
									{
										UDRSolidWallAdapter*Local_NextWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(Wallnum[i]));
										if (Local_NextWallAdapter)
										{
											if (bIsStartCorner)
											{
												if (Local_NextWallAdapter->GetP0() == Local_SolidWallAdapter->GetP0())
												{
													NextWallDir = (Local_NextWallAdapter->GetEndPos() - Local_NextWallAdapter->GetStartPos()).GetSafeNormal();
												}
												else
													if (Local_NextWallAdapter->GetP1() == Local_SolidWallAdapter->GetP0())
													{
														NextWallDir = (Local_NextWallAdapter->GetStartPos() - Local_NextWallAdapter->GetEndPos()).GetSafeNormal();
													}
													else
													{
														bSet = false;
													}
												CurrentWallDir = FVector2D(EndPos - StartPos);
											}
											else
											{
												if (Local_NextWallAdapter->GetP0() == Local_SolidWallAdapter->GetP1())
												{
													NextWallDir = (Local_NextWallAdapter->GetEndPos() - Local_NextWallAdapter->GetStartPos()).GetSafeNormal();
												}
												else
													if (Local_NextWallAdapter->GetP1() == Local_SolidWallAdapter->GetP1())
													{
														NextWallDir = (Local_NextWallAdapter->GetStartPos() - Local_NextWallAdapter->GetEndPos()).GetSafeNormal();
													}
													else
													{
														bSet = false;
													}
												CurrentWallDir = FVector2D(StartPos - EndPos);
											}

										}
										else
										{
											bSet = false;
										}
										break;
									}
								}
								if (bSet)
								{
									float CosVal=FVector2D::DotProduct(NextWallDir, CurrentWallDir);
									if (FMath::Abs(CosVal) < 0.01f)
									{

									}
								}
							}
						}
					}
				}

			}
		}*/
}

void UWallBuildSystem::FillWallDefaultProperties(int32 WallId)
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
	solidWallAdapter->SetHeight(280.0f);

	solidWallAdapter->SetFrontUVScale(FVector2D(100.0f, 100.0f));
	solidWallAdapter->SetBackUVScale(FVector2D(100.0f, 100.0f));
	solidWallAdapter->SetSideUVScale(FVector2D(100.0f, 100.0f));
}

FVector2D UWallBuildSystem::GetWallCenterLoc(int32 objectID, int32 WallID, float Width)
{

	FVector2D Postion = FVector2D::ZeroVector;
	if (WallID != INDEX_NONE && BuildingSystem)
	{
		UBuildingData *WallData = BuildingSystem->GetData(WallID);
		if (WallData)
		{
			int P0 = WallData->GetInt("P0");
			int P1 = WallData->GetInt("P1");
			IObject *CornerData1 = BuildingSystem->GetObject(P0);
			IObject *CornerData2 = BuildingSystem->GetObject(P1);
			if (CornerData1&&CornerData2)
			{
				FVector2D Start = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
				FVector2D End = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());
				Postion = FMath::Lerp(Start, End, 0.5f);
			}
		}
	}
	return Postion;

}

FDeleteHoleInfo UWallBuildSystem::CalculateRemoveHoleInfo(const int32 HoleID)
{
	FDeleteHoleInfo Local_DeleteHoleInfo;
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (Local_DRProjData)
	{
		UDRHoleAdapter* Local_HoleAdapter = nullptr;
		Local_HoleAdapter = Cast<UDRHoleAdapter>(Local_DRProjData->GetAdapter(HoleID));
		if (Local_HoleAdapter)
		{
			Local_HoleAdapter->SetHoleStatusType(EHolestatusType::EDRDeleteHole);
			int32 WallID= Local_HoleAdapter->GetDependWallId();
			UDRSolidWallAdapter* Local_SolidWallAdapter = nullptr;
			Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(WallID));
			if (Local_SolidWallAdapter)
			{
				FVector2D StartPos = FVector2D(Local_SolidWallAdapter->GetWallPositionData().StartPos);
				FVector2D EndPos = FVector2D(Local_SolidWallAdapter->GetWallPositionData().EndPos);
				Local_DeleteHoleInfo.Angle = GetWallRotate(StartPos, EndPos);
			}
			Local_DeleteHoleInfo.HoleType = Local_HoleAdapter->GetHoleType();
			Local_DeleteHoleInfo.Loc = Local_HoleAdapter->GetTransform().GetLocation();
			Local_DeleteHoleInfo.Width = Local_HoleAdapter->GetWidth();
			Local_DeleteHoleInfo.Thickness = Local_HoleAdapter->GetThickness();
			Local_DeleteHoleInfo.Height = Local_HoleAdapter->GetHeight();
			Local_DeleteHoleInfo.Length = 60.0f;
			Local_DeleteHoleInfo.GroundHeight = Local_HoleAdapter->GetGroundHeight();
			Local_DeleteHoleInfo.Horizontalflip = Local_HoleAdapter->GetHorizontalFlip();
			Local_DeleteHoleInfo.Verticalflip = Local_HoleAdapter->GetVerticalFlip();
			Local_DeleteHoleInfo.EFillWallType = ENewWallType::EPlasterWall;
		}
	}
	return Local_DeleteHoleInfo;
}

bool UWallBuildSystem::IsSeleteDeleteHole(UBooleanBase*& Boolean)
{
	Boolean = nullptr;
	UWorld* MyWorld = GetWorld();
	if (MyWorld)
	{
		FVector MouseWorldPos, MouseDir;
		MyWorld->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseWorldPos, MouseDir);
		TArray<UUserWidget*> FoundWidgets;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets,UBooleanBase::StaticClass());
		for (int i = 0; i < FoundWidgets.Num(); ++i)
		{
			if (FoundWidgets[i])
			{
				UBooleanBase* BooleanUMG = Cast<UBooleanBase>(FoundWidgets[i]);
				if (BooleanUMG)
				{
					if (BooleanUMG->HolestatusType == EHolestatusType::EDRDeleteHole)
					{
						FDeleteHoleInfo Local_DeleteHoleInfo = BooleanUMG->DeleteHoleInfo;
						TArray<FVector> Points;
						FVector2D TempPDirWidth = FVector2D(0, 1);
						FVector2D TempPDirThickness = FVector2D(1, 0);
						FVector TempPoint= Local_DeleteHoleInfo.Loc- FVector(TempPDirWidth.GetRotated(Local_DeleteHoleInfo.Angle),0)*Local_DeleteHoleInfo.Width/2+ FVector(TempPDirThickness.GetRotated(Local_DeleteHoleInfo.Angle), 0)*Local_DeleteHoleInfo.Thickness / 2;
						Points.Add(TempPoint);
						TempPoint = Local_DeleteHoleInfo.Loc + FVector(TempPDirWidth.GetRotated(Local_DeleteHoleInfo.Angle), 0)*Local_DeleteHoleInfo.Width / 2 + FVector(TempPDirThickness.GetRotated(Local_DeleteHoleInfo.Angle), 0)*Local_DeleteHoleInfo.Thickness / 2;
						Points.Add(TempPoint);
						TempPoint = Local_DeleteHoleInfo.Loc + FVector(TempPDirWidth.GetRotated(Local_DeleteHoleInfo.Angle), 0)*Local_DeleteHoleInfo.Width / 2 - FVector(TempPDirThickness.GetRotated(Local_DeleteHoleInfo.Angle), 0)*Local_DeleteHoleInfo.Thickness / 2;
						Points.Add(TempPoint);
						TempPoint = Local_DeleteHoleInfo.Loc - FVector(TempPDirWidth.GetRotated(Local_DeleteHoleInfo.Angle), 0)*Local_DeleteHoleInfo.Width / 2 - FVector(TempPDirThickness.GetRotated(Local_DeleteHoleInfo.Angle), 0)*Local_DeleteHoleInfo.Thickness / 2;
						Points.Add(TempPoint);
						if (JudgePointInPolygon(Points, FVector2D(MouseWorldPos)))
						{
							BooleanUMG->bIsSelect = true;
							Boolean = BooleanUMG;
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool UWallBuildSystem::GetWallTypeByHole(const UBooleanBase* DeleteBoolean, const AWall_Boolean_Base*Boolean, EDRAllStatusWallType& WallType)
{
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (Local_DRProjData)
	{
		if (Boolean)
		{
			UDRHoleAdapter* Local_HoleAdapter = nullptr;
			Local_HoleAdapter = Cast<UDRHoleAdapter>(Local_DRProjData->GetAdapter(Boolean->ObjectID));
			if (Local_HoleAdapter)
			{
				int32 WallID = Local_HoleAdapter->GetDependWallId();
				UDRSolidWallAdapter* Local_WallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(WallID));
				if (Local_WallAdapter)
				{
					WallType = (EDRAllStatusWallType)Local_WallAdapter->GetSolidWallType();
					return true;
				}
			}
		}
		if (DeleteBoolean&&GameInst)
		{
			FVector2D Center = FVector2D(DeleteBoolean->DeleteHoleInfo.Loc);
			for (int i = 0; i < GameInst->DRDeleteWall.Num(); ++i)
			{
				if (!GameInst->DRDeleteWall[i].isDelete)
				{
					if (JudgePointInPolygon(GameInst->DRDeleteWall[i].Point, Center))
					{
						WallType = EDRAllStatusWallType::EDRDeleteWall;
						return true;
					}
				}
			}
			TArray<int32>WallIDs;
			GetAllObject(WallIDs, EObjectType::ESolidWall);
			for (int i = 0; i < WallIDs.Num(); ++i)
			{
				UDRSolidWallAdapter* Local_WallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(WallIDs[i]));
				if (Local_WallAdapter)
				{
					if (Local_WallAdapter->GetSolidWallType() == ESolidWallType::EDROriginalWall)
					{
						FDRWallPosition LocalPosition=Local_WallAdapter->GetWallPositionData();
						TArray<FVector>InPoints;
						InPoints.Add(LocalPosition.StartPos);
						InPoints.Add(LocalPosition.LeftStartPos);
						InPoints.Add(LocalPosition.LeftEndPos);
						InPoints.Add(LocalPosition.EndPos);
						InPoints.Add(LocalPosition.RightEndPos);
						InPoints.Add(LocalPosition.RightStartPos);
						if (JudgePointInPolygon(InPoints, Center))
						{
							WallType = EDRAllStatusWallType::EDROriginalWall;
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

void UWallBuildSystem::GetAllHoleTypeByWall(const int32 WallID, const int32 StructIndex, TArray<AWall_Boolean_Base*>& OutBoolean, TArray<UBooleanBase*>& OutDeleteBoolean)
{
	if (WallID > 0)
	{
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
		if (Local_DRProjData)
		{
			UDRSolidWallAdapter* Local_WallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(WallID));
			if (Local_WallAdapter)
			{
				TArray<int32> HoleIDs = Local_WallAdapter->GetHoles();
				for (int i = 0; i < HoleIDs.Num(); ++i)
				{
					UObject** ObjPtr = Local_DRProjData->DoorHoleMap.Find(HoleIDs[i]);
					if (ObjPtr)
					{
						UObject* Obj = *ObjPtr;
						if (Obj)
						{
							AWall_Boolean_Base* BooleanActor = Cast<AWall_Boolean_Base>(Obj);
							if (BooleanActor)
							{
								OutBoolean.Add(BooleanActor);
								continue;
							}
						}
					}
					ObjPtr = Local_DRProjData->WindowHoleMap.Find(HoleIDs[i]);
					if (ObjPtr)
					{
						UObject* Obj = *ObjPtr;
						if (Obj)
						{
							AWall_Boolean_Base* BooleanActor = Cast<AWall_Boolean_Base>(Obj);
							if (BooleanActor)
							{
								OutBoolean.Add(BooleanActor);
							}
						}
					}
				}
				if (Local_WallAdapter->GetSolidWallType() == ESolidWallType::EDROriginalWall)
				{

					TArray<UUserWidget*> FoundWidgets;
					UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets, UBooleanBase::StaticClass());
					for (int i = 0; i < FoundWidgets.Num(); ++i)
					{
						if (FoundWidgets[i])
						{
							UBooleanBase* BooleanUMG = Cast<UBooleanBase>(FoundWidgets[i]);
							if (BooleanUMG)
							{
								if (BooleanUMG->HolestatusType == EHolestatusType::EDRDeleteHole)
								{
									FDRWallPosition LocalPosition = Local_WallAdapter->GetWallPositionData();
									TArray<FVector>InPoints;
									InPoints.Add(LocalPosition.StartPos);
									InPoints.Add(LocalPosition.LeftStartPos);
									InPoints.Add(LocalPosition.LeftEndPos);
									InPoints.Add(LocalPosition.EndPos);
									InPoints.Add(LocalPosition.RightEndPos);
									InPoints.Add(LocalPosition.RightStartPos);
									if (JudgePointInPolygon(InPoints, FVector2D(BooleanUMG->DeleteHoleInfo.Loc)))
									{
										OutDeleteBoolean.Add(BooleanUMG);
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
		if (GameInst&&GameInst->DRDeleteWall.IsValidIndex(StructIndex))
		{
			UWorld* MyWorld = GetWorld();
			if (MyWorld)
			{
				FVector MouseWorldPos, MouseDir;
				MyWorld->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseWorldPos, MouseDir);
				TArray<UUserWidget*> FoundWidgets;
				UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets, UBooleanBase::StaticClass());
				for (int i = 0; i < FoundWidgets.Num(); ++i)
				{
					if (FoundWidgets[i])
					{
						UBooleanBase* BooleanUMG = Cast<UBooleanBase>(FoundWidgets[i]);
						if (BooleanUMG)
						{
							if (BooleanUMG->HolestatusType == EHolestatusType::EDRDeleteHole)
							{
								TArray<FVector>InPoints= GameInst->DRDeleteWall[StructIndex].Point;
								if (JudgePointInPolygon(InPoints, FVector2D(BooleanUMG->DeleteHoleInfo.Loc)))
								{
									OutDeleteBoolean.Add(BooleanUMG);
								}
							}
						}
					}
				}
			}
		}
	}
}

bool UWallBuildSystem::IsSetThisWall(const int32 WallID, const FVector2D Center)
{
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (Local_DRProjData)
	{
		UDRSolidWallAdapter* Local_WallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(WallID));
		if (Local_WallAdapter)
		{
			FDRWallPosition LocalPosition = Local_WallAdapter->GetWallPositionData();
			TArray<FVector>InPoints;
			InPoints.Add(LocalPosition.StartPos);
			InPoints.Add(LocalPosition.LeftStartPos);
			InPoints.Add(LocalPosition.LeftEndPos);
			InPoints.Add(LocalPosition.EndPos);
			InPoints.Add(LocalPosition.RightEndPos);
			InPoints.Add(LocalPosition.RightStartPos);
			if (JudgePointInPolygon(InPoints, Center))
			{
				return true;
			}
		}
	}
	return false;
}

bool UWallBuildSystem::IsErrorPoints(const TArray<FVector2D>InPoints)
{
	if (InPoints.Num() == 4)
	{
		for (int i = 0; i < InPoints.Num(); ++i)
		{
			FVector2D dir1 = (InPoints[(i + 1) % 4] - InPoints[i]).GetSafeNormal();
			FVector2D dir2 = (InPoints[(i + 2) % 4] - InPoints[(i + 1) % 4]).GetSafeNormal();
			if (dir1 == FVector2D(0, 0) || dir2 == FVector2D(0, 0))
			{
				return true;
			}
			if (FMath::Abs(FVector2D::DotProduct(dir1, dir2)) > 0.02)
			{
				return true;
			}
		}
		return false;
	}
	else
		return true;
}

void UWallBuildSystem::DrawSlash(const bool IsDelete, const TArray<FVector2D>&InPoints, const float SizeX, TArray<FLinePos>&OutPoints,bool IsScreen, float error)
{
	if (InPoints.Num() == 4)
	{
		if (SizeX == 0 || IsErrorPoints(InPoints))
		{
			return;
		}

		if (error==0)
		{
			error = 20;
		}
		float Angle;
		if (IsScreen)
			Angle = 45;
		else
			Angle = -45;
		OutPoints.Empty();
		FVector2D forword = (InPoints[1] - InPoints[0]).GetSafeNormal();
		FVector2D Dir= forword.GetRotated(Angle);
		FVector2D TempStartPos = InPoints[0];
		while (true)
		{
			if ((TempStartPos - InPoints[0]).Size() >= SizeX)
			{
				break;
			}
			FVector2D TempEndPos;
			if (FPolygonAlg::IntersectionByRayAndSegment(TempStartPos, Dir, InPoints[2], InPoints[3], TempEndPos))
			{
				FLinePos TempLinePos;
				TempLinePos.Start = TempStartPos;
				TempLinePos.End = TempEndPos;
				OutPoints.Add(TempLinePos);
				TempStartPos = TempStartPos + error * forword;
				continue;
			}
			if (FPolygonAlg::IntersectionByRayAndSegment(TempStartPos, Dir, InPoints[1], InPoints[2], TempEndPos))
			{
				FLinePos TempLinePos;
				TempLinePos.Start = TempStartPos;
				TempLinePos.End = TempEndPos;
				OutPoints.Add(TempLinePos);
				TempStartPos = TempStartPos + error * forword;
				continue;
			}
			break;
		}
		if (OutPoints.Num() > 0)
		{
			TempStartPos = OutPoints[0].End - error * forword;
			while (true)
			{
				FVector2D TempEndPos;
				if (FPolygonAlg::IntersectionByRayAndSegment(TempStartPos, -Dir, InPoints[3], InPoints[0], TempEndPos))
				{
					FLinePos TempLinePos;
					TempLinePos.Start = TempStartPos;
					TempLinePos.End = TempEndPos;
					OutPoints.Add(TempLinePos);
					TempStartPos = TempStartPos + error * forword;
					continue;
				}
				break;
			}
		}
		if (IsDelete)
		{
			TArray<FLinePos> TempLinsPosList;
			forword = (InPoints[0] - InPoints[1]).GetSafeNormal();
			FVector2D Dir = forword.GetRotated(-Angle);
			FVector2D TempStartPos = InPoints[1];
			while (true)
			{
				if ((TempStartPos - InPoints[1]).Size() >= SizeX)
				{
					break;
				}
				FVector2D TempEndPos;
				if (FPolygonAlg::IntersectionByRayAndSegment(TempStartPos, Dir, InPoints[2], InPoints[3], TempEndPos))
				{
					FLinePos TempLinePos;
					TempLinePos.Start = TempStartPos;
					TempLinePos.End = TempEndPos;
					TempLinsPosList.Add(TempLinePos);
					TempStartPos = TempStartPos + error * forword;
					continue;
				}
				if (FPolygonAlg::IntersectionByRayAndSegment(TempStartPos, Dir, InPoints[0], InPoints[3], TempEndPos))
				{
					FLinePos TempLinePos;
					TempLinePos.Start = TempStartPos;
					TempLinePos.End = TempEndPos;
					TempLinsPosList.Add(TempLinePos);
					TempStartPos = TempStartPos + error * forword;
					continue;
				}
				break;
			}
			if (TempLinsPosList.Num() > 0)
			{
				TempStartPos = TempLinsPosList[0].End - error * forword;
				while (true)
				{
					FVector2D TempEndPos;
					if (FPolygonAlg::IntersectionByRayAndSegment(TempStartPos, -Dir, InPoints[1], InPoints[2], TempEndPos))
					{
						FLinePos TempLinePos;
						TempLinePos.Start = TempStartPos;
						TempLinePos.End = TempEndPos;
						TempLinsPosList.Add(TempLinePos);
						TempStartPos = TempStartPos + error * forword;
						continue;
					}
					break;
				}
			}
			OutPoints.Append(TempLinsPosList);
		}
	}
}

void UWallBuildSystem::BoxSelectDeleteHole(const FVector2D Main, const FVector2D Max)
{
	TArray<UUserWidget*> FoundWidgets;
	TArray<FVector> Points1;
	Points1.Add(FVector(Main,0));
	Points1.Add(FVector(Main.X, Max.Y,0));
	Points1.Add(FVector(Max,0));
	Points1.Add(FVector(Max.X, Main.Y,0));
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets, UBooleanBase::StaticClass());
	for (int i = 0; i < FoundWidgets.Num(); ++i)
	{
		if (FoundWidgets[i])
		{
			UBooleanBase* BooleanUMG = Cast<UBooleanBase>(FoundWidgets[i]);
			if (BooleanUMG)
			{
				if (BooleanUMG->HolestatusType == EHolestatusType::EDRDeleteHole)
				{
					FDeleteHoleInfo TempHoleInfo = BooleanUMG->DeleteHoleInfo;
					float TempWidth = TempHoleInfo.Width*0.5;
					float TempThickness = TempHoleInfo.Thickness*0.5;
					FVector Dir = FVector(0, 1,0);
					FVector OutDir = FVector(FVector2D(Dir).GetRotated(90+ TempHoleInfo.Angle),0);
					Dir = FVector(FVector2D(Dir).GetRotated(TempHoleInfo.Angle),0);
					TArray<FVector> Points2;
					Points2.Add(TempHoleInfo.Loc - TempWidth * Dir + TempThickness * OutDir);
					Points2.Add(TempHoleInfo.Loc + TempWidth * Dir + TempThickness * OutDir);
					Points2.Add(TempHoleInfo.Loc + TempWidth * Dir - TempThickness * OutDir);
					Points2.Add(TempHoleInfo.Loc - TempWidth * Dir - TempThickness * OutDir);
					for (int k = 0; k < Points1.Num(); ++k)
					{
						if (JudgePointInPolygon(Points2, FVector2D(Points1[k])))
						{
							BooleanUMG->RemoveFromParent();
							BooleanUMG = nullptr;
							break;
						}
					}
					if (BooleanUMG)
					{
						for (int k = 0; k < Points2.Num(); ++k)
						{
							if (JudgePointInPolygon(Points1, FVector2D(Points2[k])))
							{
								BooleanUMG->RemoveFromParent();
								BooleanUMG = nullptr;
								break;
							}
						}
					}
				}
			}
		}
	}
}

TArray<UBooleanBase*> UWallBuildSystem::GetAllDeleteHoles()
{
	TArray<UBooleanBase*> OutBoolenInfo;
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets, UBooleanBase::StaticClass());
	for (int i = 0; i < FoundWidgets.Num(); ++i)
	{
		if (FoundWidgets[i])
		{
			UBooleanBase* BooleanUMG = Cast<UBooleanBase>(FoundWidgets[i]);
			if (BooleanUMG)
			{
				if (BooleanUMG->HolestatusType == EHolestatusType::EDRDeleteHole)
				{
					OutBoolenInfo.Add(BooleanUMG);
				}
			}
		}
	}
	return OutBoolenInfo;
}

bool UWallBuildSystem::IsinterlinkNewWall(int32 WallID)
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager)
	{
		UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallID));
		if (solidWallAdapter)
		{
			FVector2D Dir = (solidWallAdapter->GetOriginalEndtPos() - solidWallAdapter->GetOriginalSartPos()).GetSafeNormal();
			int32 P0 = solidWallAdapter->GetP0();
			int32 P1 = solidWallAdapter->GetP1();
			if (IsinterlinkNewWallByCornner(P0, Dir, WallID))
			{
				return true;
			}
			else
				if (IsinterlinkNewWallByCornner(P1, Dir, WallID))
				{
					return true;
				}
		}
	}
	return false;
}

bool UWallBuildSystem::IsinterlinkNewWallByCornner(const int32 CornerID, const FVector2D Dir,const int32 WallID)
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager)
	{
		UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(CornerID));
		if (CornerAdapter)
		{
			TArray<int32> Walls = CornerAdapter->GetWalls();
			for (int i = 0; i < Walls.Num(); ++i)
			{
				if (Walls[i] != WallID)
				{
					UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(Walls[i]));
					if (solidWallAdapter)
					{
						FVector2D TempDir = (solidWallAdapter->GetOriginalEndtPos() - solidWallAdapter->GetOriginalSartPos()).GetSafeNormal();
						float angle = FVector2D::CrossProduct(Dir,TempDir);
						if (FMath::Abs(angle) < 0.02)
						{
							if (solidWallAdapter->GetSolidWallType() == ESolidWallType::EDRNewWall)
							{
								return true;
							}
						}
					}
				}

			}

		}
	}
	return false;
}

bool UWallBuildSystem::IsNeedDeleteWindowSill(const FDeleteHoleInfo& CurrentHoleInfo, const FVector Center)
{
	float TempWidth = CurrentHoleInfo.Width*0.5;
	float TempThickness = CurrentHoleInfo.Thickness*0.5;
	FVector Dir = FVector(0, 1, 0);
	FVector OutDir = FVector(FVector2D(Dir).GetRotated(90 + CurrentHoleInfo.Angle), 0);
	Dir = FVector(FVector2D(Dir).GetRotated(CurrentHoleInfo.Angle), 0);
	TArray<FVector> Points;
	if (CurrentHoleInfo.HoleType == EModelType::EDRBayWindow)
	{
		float length=60;
		if (CurrentHoleInfo.Verticalflip)
		{
			Points.Add(CurrentHoleInfo.Loc - TempWidth * Dir + (TempThickness + length) * OutDir);
			Points.Add(CurrentHoleInfo.Loc + TempWidth * Dir + (TempThickness + length) * OutDir);
			Points.Add(CurrentHoleInfo.Loc + TempWidth * Dir - TempThickness * OutDir);
			Points.Add(CurrentHoleInfo.Loc - TempWidth * Dir - TempThickness * OutDir);
		}
		else
		{
			Points.Add(CurrentHoleInfo.Loc - TempWidth * Dir + TempThickness * OutDir);
			Points.Add(CurrentHoleInfo.Loc + TempWidth * Dir + TempThickness * OutDir);
			Points.Add(CurrentHoleInfo.Loc + TempWidth * Dir - (TempThickness+ length) * OutDir);
			Points.Add(CurrentHoleInfo.Loc - TempWidth * Dir - (TempThickness + length) * OutDir);
		}
	}
	else
	{
		Points.Add(CurrentHoleInfo.Loc - TempWidth * Dir + TempThickness * OutDir);
		Points.Add(CurrentHoleInfo.Loc + TempWidth * Dir + TempThickness * OutDir);
		Points.Add(CurrentHoleInfo.Loc + TempWidth * Dir - TempThickness * OutDir);
		Points.Add(CurrentHoleInfo.Loc - TempWidth * Dir - TempThickness * OutDir);
	}
	return JudgePointInPolygon(Points, FVector2D (Center));
}

TArray<int32> UWallBuildSystem::AddEdge(const FVector2D &P0, const FVector2D &P1, int32 SnapObj0, int32 SnapObj1, bool bBreak)
{
	TArray<int32> num;
	if (BuildingSystem)
	{
		num = BuildingSystem->AddEdge(P0, P1, SnapObj0, SnapObj1, bBreak);
	}
	return num;
}

void UWallBuildSystem::UpdateEdgeCornerPos(FVector2D& StartPos, FVector2D& EndPos, int32 SnapObj0, int32 SnapObj1)
{

	IValue *v = nullptr;
	v = BuildingSystem->GetProperty(SnapObj0, "Walls");
	if (v)
	{
		kArray<int> WallIDs = v->IntArrayValue();
		for (int i = 0; i < WallIDs.size(); ++i)
		{
			UBuildingData* Data = BuildingSystem->GetData(WallIDs[i]);
			if (Data)
			{
				EObjectType Type = (EObjectType)Data->GetObjectType();
				if (Type == EObjectType::ESolidWall)
				{
					TArray<FVector> WallNodes;
					float Zpos;
					BuildingSystem->GetWallBorderLines(WallIDs[i], WallNodes, Zpos);
					if (WallNodes.Num() == 6)
					{
						FVector2D a1 = StartPos;
						FVector2D a2 = EndPos;
						FVector2D b1 = FVector2D(WallNodes[1]);
						FVector2D b2 = FVector2D(WallNodes[2]);
						if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
						{

							StartPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
							break;
						}
						else
						{
							b1 = FVector2D(WallNodes[4]);
							b2 = FVector2D(WallNodes[5]);
							if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
							{
								StartPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
								break;
							}
						}

					}

				}
			}
		}
	}
	v = BuildingSystem->GetProperty(SnapObj1, "Walls");
	if (v)
	{
		kArray<int> WallIDs = v->IntArrayValue();
		for (int i = 0; i < WallIDs.size(); ++i)
		{
			UBuildingData* Data = BuildingSystem->GetData(WallIDs[i]);
			if (Data)
			{
				EObjectType Type = (EObjectType)Data->GetObjectType();
				if (Type == EObjectType::ESolidWall)
				{
					TArray<FVector> WallNodes;
					float Zpos;
					BuildingSystem->GetWallBorderLines(WallIDs[i], WallNodes, Zpos);
					if (WallNodes.Num() == 6)
					{
						FVector2D a1 = StartPos;
						FVector2D a2 = EndPos;
						FVector2D b1 = FVector2D(WallNodes[1]);
						FVector2D b2 = FVector2D(WallNodes[2]);
						if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
						{

							EndPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
							break;
						}
						else
						{
							b1 = FVector2D(WallNodes[4]);
							b2 = FVector2D(WallNodes[5]);
							if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
							{
								EndPos = GetwoSegmentsIntersect(a1, a2, b1, b2);
								break;
							}
						}

					}

				}
			}
		}
	}
}

bool UWallBuildSystem::CheckEdgeCornerPos(FVector2D& StartPos, FVector2D& EndPos, int32 SnapObj)
{
	IValue *v = nullptr;
	v = BuildingSystem->GetProperty(SnapObj, "Walls");
	if (v)
	{
		kArray<int> WallIDs = v->IntArrayValue();
		for (int i = 0; i < WallIDs.size(); ++i)
		{
			UBuildingData* Data = BuildingSystem->GetData(WallIDs[i]);
			if (Data)
			{
				EObjectType Type = (EObjectType)Data->GetObjectType();
				if (Type == EObjectType::ESolidWall)
				{
					TArray<FVector> WallNodes;
					float Zpos;
					BuildingSystem->GetWallBorderLines(WallIDs[i], WallNodes, Zpos);
					if (WallNodes.Num() == 6)
					{
						FVector2D a1 = StartPos;
						FVector2D a2 = EndPos;
						FVector2D b1 = FVector2D(WallNodes[1]);
						FVector2D b2 = FVector2D(WallNodes[2]);
						if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
						{
							return true;
						}
						else
						{
							b1 = FVector2D(WallNodes[4]);
							b2 = FVector2D(WallNodes[5]);
							if (IsTwoLineSegmentsIntersect(a1, a2, b1, b2))
							{
								return true;
							}
						}

					}

				}
			}
		}
	}
	return false;
}

void UWallBuildSystem::CalculateBestEdgeCornerPos(FVector2D& StartPos, FVector2D& EndPos, int32& SnapObj_0, int32& SnapObj_1)
{
	FVector2D Dir = StartPos - EndPos;
	UBuildingData* Data_0 = BuildingSystem->GetData(SnapObj_0);
	UBuildingData* Data_1 = BuildingSystem->GetData(SnapObj_1);
	if (Data_0)
	{
		int32 Local_SnapObj = -1;
		Dir.Normalize();
		Dir *= GetHitWallThickness(SnapObj_0) / 4;
		FVector2D Local_Pos = StartPos - Dir;
		Local_SnapObj = BuildingSystem->HitTest(Local_Pos);
		int TempType_0 = -1;
		IObject * Local_HitObj = BuildingSystem->GetObject(Local_SnapObj);
		if (Local_HitObj)
		{
			int Local_TempType = Local_HitObj->GetType();
			if ((TempType_0 == EObjectType::ESolidWall) && SnapObj_0 != Local_SnapObj)
			{
				StartPos = Local_Pos;
				SnapObj_0 = Local_SnapObj;
			}
		}
	}
	if (Data_1)
	{
		int32 Local_SnapObj = -1;
		Dir.Normalize();
		Dir *= GetHitWallThickness(SnapObj_1) / 4;
		FVector2D Local_Pos = EndPos + Dir;
		Local_SnapObj = BuildingSystem->HitTest(Local_Pos);
		IObject * Local_HitObj = BuildingSystem->GetObject(Local_SnapObj);
		int TempType_0 = -1;
		if (Local_HitObj)
		{
			int Local_TempType = Local_HitObj->GetType();
			if ((TempType_0 == EObjectType::ESolidWall) && SnapObj_0 != Local_SnapObj)
			{
				EndPos = Local_Pos;
				SnapObj_1 = Local_SnapObj;
			}
		}
	}
	if (Data_0)
	{
		EObjectType Type = (EObjectType)Data_0->GetObjectType();
		if (Type == EObjectType::ESolidWall)
		{

			TArray<FVector> WallNodes;
			float Zpos;
			BuildingSystem->GetWallBorderLines(SnapObj_0, WallNodes, Zpos);
			if (WallNodes.Num() == 6)
			{
				FVector2D a1 = StartPos;
				FVector2D a2 = EndPos;
				FVector2D b0 = FVector2D(WallNodes[0]);
				FVector2D b1 = FVector2D(WallNodes[1]);
				FVector2D b2 = FVector2D(WallNodes[2]);
				FVector2D b3 = FVector2D(WallNodes[3]);
				FVector2D b4 = FVector2D(WallNodes[4]);
				FVector2D b5 = FVector2D(WallNodes[5]);
				FVector2D Temp;
				bool IsChange = false;
				float Local_WallThickness = GetHitWallThickness(SnapObj_0);
				float Local_Tol = 2;
				float Local_EdgeTol = 2;
				if (IsTwoLineSegmentsIntersect(StartPos, EndPos, b2, b4) && FMath::Abs(FVector2D::CrossProduct((StartPos - EndPos).GetSafeNormal(), (b2 - b4).GetSafeNormal())) > 0.02)
				{
					Temp = GetwoSegmentsIntersect(StartPos, EndPos, b2, b4);
					if (FVector2D::Distance(Temp, EndPos) < FVector2D::Distance(StartPos, EndPos))
					{
						StartPos = Temp;
						IsChange = true;
						Local_Tol = Local_WallThickness;
						Local_EdgeTol = Local_WallThickness;
					}
				}

				if (IsTwoLineSegmentsIntersect(StartPos, EndPos, b1, b5) && FMath::Abs(FVector2D::CrossProduct((StartPos - EndPos).GetSafeNormal(), (b1 - b5).GetSafeNormal())) > 0.02)
				{
					Temp = GetwoSegmentsIntersect(StartPos, EndPos, b1, b5);
					if (FVector2D::Distance(Temp, EndPos) < FVector2D::Distance(StartPos, EndPos))
					{
						StartPos = Temp;
						IsChange = true;
						Local_Tol = Local_WallThickness;
						Local_EdgeTol = Local_WallThickness;
					}
				}
				if (IsTwoLineSegmentsIntersect(StartPos, EndPos, b1, b2) && FMath::Abs(FVector2D::CrossProduct((StartPos - EndPos).GetSafeNormal(), (b1 - b2).GetSafeNormal())) > 0.02)
				{
					Temp = GetwoSegmentsIntersect(StartPos, EndPos, b1, b2);
					if (FVector2D::Distance(Temp, EndPos) < FVector2D::Distance(StartPos, EndPos))
					{
						StartPos = Temp;
						IsChange = true;
						Local_Tol = 2;
						Local_EdgeTol = 2;
					}
				}
				if (IsTwoLineSegmentsIntersect(StartPos, EndPos, b4, b5) && FMath::Abs(FVector2D::CrossProduct((StartPos - EndPos).GetSafeNormal(), (b4 - b5).GetSafeNormal())) > 0.02)
				{
					Temp = GetwoSegmentsIntersect(StartPos, EndPos, b4, b5);
					if (FVector2D::Distance(Temp, EndPos) < FVector2D::Distance(StartPos, EndPos))
					{
						StartPos = Temp;
						IsChange = true;
						Local_Tol = 2;
						Local_EdgeTol = 2;
					}
				}
				/*if (IsChange)*/
					BuildingSystem->CutAreaSnap(StartPos, -1, StartPos, SnapObj_0, Local_Tol, Local_EdgeTol);
			}

		}
	}
	if (Data_1)
	{
		EObjectType Type = (EObjectType)Data_1->GetObjectType();
		if (Type == EObjectType::ESolidWall)
		{
			TArray<FVector> WallNodes;
			float Zpos;
			BuildingSystem->GetWallBorderLines(SnapObj_1, WallNodes, Zpos);
			if (WallNodes.Num() == 6)
			{
				FVector2D b0 = FVector2D(WallNodes[0]);
				FVector2D b1 = FVector2D(WallNodes[1]);
				FVector2D b2 = FVector2D(WallNodes[2]);
				FVector2D b3 = FVector2D(WallNodes[3]);
				FVector2D b4 = FVector2D(WallNodes[4]);
				FVector2D b5 = FVector2D(WallNodes[5]);
				FVector2D Temp;
				bool IsChange = false;
				float Local_WallThickness = GetHitWallThickness(SnapObj_1);
				float Local_Tol = 2;
				float Local_EdgeTol = 2;
				if (IsTwoLineSegmentsIntersect(StartPos, EndPos, b2, b4) && FMath::Abs(FVector2D::CrossProduct((StartPos - EndPos).GetSafeNormal(), (b2 - b4).GetSafeNormal())) > 0.02)
				{
					Temp = GetwoSegmentsIntersect(StartPos, EndPos, b2, b4);
					if (FVector2D::Distance(Temp, StartPos) < FVector2D::Distance(StartPos, EndPos))
					{
						EndPos = Temp;
						IsChange = true;
						Local_Tol = Local_WallThickness;
						Local_EdgeTol = Local_WallThickness;
					}
				}

				if (IsTwoLineSegmentsIntersect(StartPos, EndPos, b1, b5) && FMath::Abs(FVector2D::CrossProduct((StartPos - EndPos).GetSafeNormal(), (b1 - b5).GetSafeNormal())) > 0.02)
				{
					Temp = GetwoSegmentsIntersect(StartPos, EndPos, b1, b5);
					if (FVector2D::Distance(Temp, StartPos) < FVector2D::Distance(StartPos, EndPos))
					{
						EndPos = Temp;
						IsChange = true;
						Local_Tol = Local_WallThickness;
						Local_EdgeTol = Local_WallThickness;
					}
				}
				if (IsTwoLineSegmentsIntersect(StartPos, EndPos, b1, b2) && FMath::Abs(FVector2D::CrossProduct((StartPos - EndPos).GetSafeNormal(), (b1 - b2).GetSafeNormal())) > 0.02)
				{
					Temp = GetwoSegmentsIntersect(StartPos, EndPos, b1, b2);
					if (FVector2D::Distance(Temp, StartPos) < FVector2D::Distance(StartPos, EndPos))
					{
						EndPos = Temp;
						IsChange = true;
						Local_Tol = 2;
						Local_EdgeTol = 2;
					}
				}
				if (IsTwoLineSegmentsIntersect(StartPos, EndPos, b4, b5) && FMath::Abs(FVector2D::CrossProduct((StartPos - EndPos).GetSafeNormal(), (b4 - b5).GetSafeNormal())) > 0.02)
				{
					Temp = GetwoSegmentsIntersect(StartPos, EndPos, b4, b5);
					if (FVector2D::Distance(Temp, StartPos) < FVector2D::Distance(StartPos, EndPos))
					{
						EndPos = Temp;
						IsChange = true;
						Local_Tol = 2;
						Local_EdgeTol = 2;
					}
				}
				/*if (IsChange)*/
					BuildingSystem->CutAreaSnap(EndPos, -1, EndPos, SnapObj_1, Local_Tol, Local_EdgeTol);
			}

		}
	}
}

void UWallBuildSystem::FilterEdgeInfo()
{
	TArray<int32> EdgeIDs;
	GetAllObject(EdgeIDs, EObjectType::EEdge, false);
	TArray<FVirtualWallInfo> TempInfo;
	for (int i = 0; i < EdgeIDs.Num(); ++i)
	{
		for (int j = 0; j < AllVirtualLoc.Num(); ++j)
			if (AllVirtualLoc[j].WallID == EdgeIDs[i])
				TempInfo.Add(AllVirtualLoc[j]);
	}
	AllVirtualLoc = TempInfo;
}

void UWallBuildSystem::CheckEdgeCorner(int32 ObjID)
{
	TArray<int32> EdgeIDs;
	GetAllObject(EdgeIDs, EObjectType::EEdge, false);
	UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(this);
	if (Local_DRProjData)
	{
		for (int32 i = 0; i < EdgeIDs.Num(); ++i)
		{
			UDRVirtualWallAdapter* Local_VirtualWallAdapter = Cast<UDRVirtualWallAdapter>(Local_DRProjData->GetAdapter(EdgeIDs[i]));
			if (Local_VirtualWallAdapter)
			{
				FVirtualWallInfo Local_VriWallInfo;
				int32 P0 = Local_VirtualWallAdapter->GetP0();
				int32 P1 = Local_VirtualWallAdapter->GetP1();
				UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(Local_DRProjData->GetAdapter(P0));
				if (P0 == ObjID)
				{
					BuildingSystem->DeleteObject(EdgeIDs[i]);
				}
				if (P1 == ObjID)
				{
					BuildingSystem->DeleteObject(EdgeIDs[i]);
				}
			}
		}
	}
}

int32 UWallBuildSystem::AddEdgeCorner(FVector2D &Location, int32 WallID)
{
	int32 ObjID = WallID;
	if (BuildingSystem)
	{
		UDRProjData* ProjectDataManager = UDRProjData::GetProjectDataManager(this);
		UDRSolidWallAdapter* WallAdapter = Cast<UDRSolidWallAdapter>(ProjectDataManager->GetAdapter(WallID));

		if (WallAdapter)
		{
			FVector2D Pos_0, Pos_1;
			float LeftThickness = WallAdapter->GetLeftThick();
			float RightThickness = WallAdapter->GetRightThick();
			float WallThickness = LeftThickness + RightThickness;
			int32 P0 = WallAdapter->GetP0();
			int32 P1 = WallAdapter->GetP1();
			UDRCornerAdapter* CornerData_0 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(P0));
			if (CornerData_0)
			{
				Pos_0 = CornerData_0->GetLocaltion();
			}
			else
				return -1;
			UDRCornerAdapter* CornerData_1 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(P1));
			if (CornerData_1)
			{
				Pos_1 = CornerData_1->GetLocaltion();
			}
			else
				return -1;
			float Distance_0 = FVector2D::Distance(Pos_0, Location);
			float Distance_1 = FVector2D::Distance(Pos_1, Location);
			if (Distance_0 < WallThickness || Distance_1 < WallThickness)
				return -1;
			IObject* Obj = BuildingSystem->GetObject(WallID);
			if (Obj)
			{
				ObjID = BuildingSystem->Break(Obj->GetID(), Location);
				if (ObjID == -1)
				{
					return -1;
				}
				UDRProjData* ProjectDataManager = UDRProjData::GetProjectDataManager(this);
				UDRSolidWallAdapter* New_WallAdapter = Cast<UDRSolidWallAdapter>(ProjectDataManager->GetAdapter(ObjID));
				TArray<int32> Holes = New_WallAdapter->GetHoles();
				for (int i = 0; i < Holes.Num(); ++i)
				{
					BuildingSystem->Move(Holes[i], FVector2D(0, 0));
				}
				if (GameInst&&GameInst->DrawHouse_Type == EDrawHouseType::ChaigaiDrawType)
				{
					if (IsinterlinkNewWall(ObjID))
					{
						if (ProjectDataManager)
						{
							UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(ProjectDataManager->GetAdapter(ObjID));
							if (solidWallAdapter)
							{
								solidWallAdapter->SetSolidWallType(ESolidWallType::EDRNewWall);
								solidWallAdapter->SetNewWallType(ENewWallType::EPlasterWall);
							}
						}
					}
				}
			}
		}
	}
	return ObjID;
}

bool UWallBuildSystem::FilterShortEdge(int32 EdgeID)
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager == nullptr) {
		return false;
	}
	UDRVirtualWallAdapter* EdgeAdapter = Cast<UDRVirtualWallAdapter>(projectDataManager->GetAdapter(EdgeID));
	if (EdgeAdapter == nullptr) {
		return false;
	}
	int32 P0 = EdgeAdapter->GetP0();
	int32 P1 = EdgeAdapter->GetP1();
	UDRCornerAdapter* CornerAdapter_0 = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(P0));
	if (CornerAdapter_0 == nullptr) {
		return false;
	}
	FVector2D P0_Pos = CornerAdapter_0->GetLocaltion();
	UDRCornerAdapter* CornerAdapter_1 = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(P1));
	if (CornerAdapter_1 == nullptr) {
		return false;
	}
	FVector2D P1_Pos = CornerAdapter_1->GetLocaltion();
	if (FVector2D::Distance(P0_Pos, P1_Pos) < 0.02&&BuildingSystem) {
		BuildingSystem->DeleteObject(EdgeID);
		return true;
	}
	return false;
}

bool UWallBuildSystem::CheckIsWallCorner(int32 CornerID)
{
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager == nullptr) {
		return false;
	}
	UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(CornerID));
	if (CornerAdapter == nullptr) {
		return false;
	}
	TArray<int32> Walls = CornerAdapter->GetWalls();
	for (int i = 0; i < Walls.Num(); ++i)
	{
		UDRSolidWallAdapter* SolidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(Walls[i]));
		if (SolidWallAdapter)
			return true;
	}
	return false;
}

int32 UWallBuildSystem::GetClickedVirtualWall(FVector2D CurrentMousePos)
{
	SelectedVirtualWallPos.Empty();
	VirtualWallIDArray.Empty();
	IObject** ppOutObject = nullptr;
	int num = BuildingSystem->GetAllObjects(ppOutObject, EObjectType::EEdge, false);
	if (num > 0)
	{
		for (int i = 0; i < num; ++i)
		{
			if (ppOutObject[i])
				VirtualWallIDArray.Add(ppOutObject[i]->GetID());
		}
	}

	for (int32 i = 0; i < VirtualWallIDArray.Num(); i++)
	{
		UBuildingData* WallData = BuildingSystem->GetData(VirtualWallIDArray[i]);
		if (!WallData)
		{
			continue;
		}
		int P0 = WallData->GetInt("P0");
		int P1 = WallData->GetInt("P1");
		IObject *CornerData1 = BuildingSystem->GetObject(P0);
		IObject *CornerData2 = BuildingSystem->GetObject(P1);

		if (CornerData1 && CornerData2)
		{
			FVector2D StartCornerPos = ToVector2D(CornerData1->GetPropertyValue("Location").Vec2Value());
			FVector2D EndCornerPos = ToVector2D(CornerData2->GetPropertyValue("Location").Vec2Value());

			//UKismetMathLibrary::FindClosestPointOnLine(FVector Point, FVector LineOrigin, FVector LineDirection)
			FVector2D DeltaFvector = (EndCornerPos - StartCornerPos);
			DeltaFvector.Normalize();
			FVector2D Temp = FVector2D(UKismetMathLibrary::FindClosestPointOnLine(FVector(CurrentMousePos, 0), FVector(StartCornerPos, 0), FVector(DeltaFvector, 0)));

			//Check Temp Between Start and end
			FVector2D VecStartToTemp = StartCornerPos - Temp;
			FVector2D VecStartToEnd = StartCornerPos - EndCornerPos;
			float Kst = FVector2D::DotProduct(VecStartToEnd, VecStartToTemp);
			float Kse = FVector2D::DotProduct(VecStartToEnd, VecStartToEnd);
			if (Kst < 0 || Kse < Kst)
			{
				continue;
			}
			if (FVector2D::Distance(CurrentMousePos, StartCornerPos) < 10.0 || FVector2D::Distance(CurrentMousePos, EndCornerPos) < 10.0)
			{
				continue;
			}

			float Length = FVector2D::Distance(CurrentMousePos, Temp);
			if (Length < 10.0)
			{
				SelectedVirtualWallPos.Add(StartCornerPos);
				SelectedVirtualWallPos.Add(EndCornerPos);
				return VirtualWallIDArray[i];
			}
		}
	}

	return -1;
}

bool UWallBuildSystem::GetReleatedCorner(int32 BorderCornerId, int32& CornerID)
{
	IObject *CornerData = BuildingSystem->GetObject(BorderCornerId);
	if (!CornerData)
	{
		CornerID = -1;
		return false;
	}
	FVector2D CornerPos = ToVector2D(CornerData->GetPropertyValue("Location").Vec2Value());
	FVector2D  BestLoc;
	int32		BestID = INDEX_NONE;
	//BuildingSystem->CutAreaSnap(CornerPos, -1, BestLoc, BestID, 20.0, 40.0);
	//BestID = BuildingSystem->FindCloseWall(CornerPos, 50.0f, BestLoc, 100.0);
	GetWallIDFromPosition(CornerPos, -1, BestLoc, BestID, 20.0, 40.0);
	if (BestID == -1)
	{
		CornerID = -1;
		return true;
	}

	UDRProjData* ProjectDataManager = UDRProjData::GetProjectDataManager(this);
	if (ProjectDataManager == nullptr)
	{
		CornerID = -1;
		return true;
	}
	UDRSolidWallAdapter* WallAdapter = Cast<UDRSolidWallAdapter>(ProjectDataManager->GetAdapter(BestID));

	if (WallAdapter)
	{
		FVector2D Pos_0, Pos_1;
		float LeftThickness = WallAdapter->GetLeftThick();
		float RightThickness = WallAdapter->GetRightThick();
		float WallThickness = LeftThickness + RightThickness;
		int32 P0 = WallAdapter->GetP0();
		int32 P1 = WallAdapter->GetP1();
		//CorrectCornerPos(BestID);
		UDRCornerAdapter* CornerData_0 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(P0));
		if (CornerData_0)
		{
			Pos_0 = CornerData_0->GetLocaltion();
		}
		else
		{
			CornerID = -1;
			return true;
		}
		UDRCornerAdapter* CornerData_1 = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(P1));
		if (CornerData_1)
		{
			Pos_1 = CornerData_1->GetLocaltion();
		}
		else
		{
			CornerID = -1;
			return true;
		}
		float Dist1 = FVector2D::Distance(CornerPos, Pos_0);
		float Dist2 = FVector2D::Distance(CornerPos, Pos_1);

		if (Dist1 <= WallThickness)
		{
			IValue *v = nullptr;
			v = BuildingSystem->GetProperty(P0, "Walls");
			if (v)
			{
				kArray<int> WallIDs = v->IntArrayValue();
				if (WallIDs.size() <= 2)
				{
					if (WallIDs.size() == 2)
					{
						UBuildingData* WallData1 = BuildingSystem->GetData(WallIDs[0]);
						UBuildingData* WallData2 = BuildingSystem->GetData(WallIDs[1]);
						if (WallData1 && WallData2)
						{
							int Wall1P0 = WallData1->GetInt("P0");
							int Wall1P1 = WallData1->GetInt("P1");
							int Wall2P0 = WallData2->GetInt("P0");
							int Wall2P1 = WallData2->GetInt("P1");
							IObject *Wall1_CornerData1 = BuildingSystem->GetObject(Wall1P0);
							IObject *Wall1_CornerData2 = BuildingSystem->GetObject(Wall1P1);
							IObject *Wall2_CornerData1 = BuildingSystem->GetObject(Wall2P0);
							IObject *Wall2_CornerData2 = BuildingSystem->GetObject(Wall2P1);

							if (Wall1_CornerData1 && Wall1_CornerData1 && Wall2_CornerData1 && Wall2_CornerData2)
							{
								FVector2D Wall1_StartPos = ToVector2D(Wall1_CornerData1->GetPropertyValue("Location").Vec2Value());
								FVector2D Wall1_EndPos = ToVector2D(Wall1_CornerData2->GetPropertyValue("Location").Vec2Value());
								FVector2D Wall2_StartPos = ToVector2D(Wall2_CornerData1->GetPropertyValue("Location").Vec2Value());
								FVector2D Wall2_EndPos = ToVector2D(Wall2_CornerData2->GetPropertyValue("Location").Vec2Value());
								FVector2D Wall1_Nor = Wall1_EndPos - Wall1_StartPos;
								FVector2D Wall2_Nor = Wall2_EndPos - Wall2_StartPos;
								if (FMath::Abs(FVector2D::CrossProduct(Wall1_Nor.GetSafeNormal(), Wall2_Nor.GetSafeNormal())) < 0.02)
								{
									CornerID = P0;
									return true;
								}
							}
						}
					}

				}
			}
			CornerID = P0;
			return false;
		}
		if (Dist2 <= WallThickness)
		{

			IValue *v = nullptr;
			v = BuildingSystem->GetProperty(P1, "Walls");
			if (v)
			{
				kArray<int> WallIDs = v->IntArrayValue();
				if (WallIDs.size() <= 2)
				{
					if (WallIDs.size() == 2)
					{
						UBuildingData* WallData1 = BuildingSystem->GetData(WallIDs[0]);
						UBuildingData* WallData2 = BuildingSystem->GetData(WallIDs[1]);
						if (WallData1 && WallData2)
						{
							int Wall1P0 = WallData1->GetInt("P0");
							int Wall1P1 = WallData1->GetInt("P1");
							int Wall2P0 = WallData2->GetInt("P0");
							int Wall2P1 = WallData2->GetInt("P1");
							IObject *Wall1_CornerData1 = BuildingSystem->GetObject(Wall1P0);
							IObject *Wall1_CornerData2 = BuildingSystem->GetObject(Wall1P1);
							IObject *Wall2_CornerData1 = BuildingSystem->GetObject(Wall2P0);
							IObject *Wall2_CornerData2 = BuildingSystem->GetObject(Wall2P1);

							if (Wall1_CornerData1 && Wall1_CornerData1 && Wall2_CornerData1 && Wall2_CornerData2)
							{
								FVector2D Wall1_StartPos = ToVector2D(Wall1_CornerData1->GetPropertyValue("Location").Vec2Value());
								FVector2D Wall1_EndPos = ToVector2D(Wall1_CornerData2->GetPropertyValue("Location").Vec2Value());
								FVector2D Wall2_StartPos = ToVector2D(Wall2_CornerData1->GetPropertyValue("Location").Vec2Value());
								FVector2D Wall2_EndPos = ToVector2D(Wall2_CornerData2->GetPropertyValue("Location").Vec2Value());
								FVector2D Wall1_Nor = Wall1_EndPos - Wall1_StartPos;
								FVector2D Wall2_Nor = Wall2_EndPos - Wall2_StartPos;
								if (FMath::Abs(FVector2D::CrossProduct(Wall1_Nor.GetSafeNormal(), Wall2_Nor.GetSafeNormal())) < 0.02)
								{
									CornerID = P1;
									return true;
								}
							}
						}
					}
				}
			}
			CornerID = P1;
			return false;
		}
	}
	else
	{
		UDRCornerAdapter* TempCornerAdapter = Cast<UDRCornerAdapter>(ProjectDataManager->GetAdapter(BestID));
		if (TempCornerAdapter != nullptr)
		{
			CornerID = BestID;
			return true;
		}
	}
	CornerID = -1;
	return true;
}

void UWallBuildSystem::IsShowVirtualWallCorner(int32 WallID, bool bShow)
{
	if (WallID != -1)
	{
		UBuildingData* WallData = BuildingSystem->GetData(WallID);
		if (!WallData)
		{
			return;
		}
		int P0 = WallData->GetInt("P0");
		int P1 = WallData->GetInt("P1");
		IObject *CornerData1 = BuildingSystem->GetObject(P0);
		IObject *CornerData2 = BuildingSystem->GetObject(P1);
		if (CornerData1 && CornerData2)
		{
			int32 TempCornerID_1, TempCornerID_2;
			GetReleatedCorner(P0, TempCornerID_1);
			GetReleatedCorner(P1, TempCornerID_2);
			if (TempCornerID_1 == -1)
			{
				TempCornerID_1 = P0;
			}
			if (TempCornerID_2 == -1)
			{
				TempCornerID_2 = P1;
			}
			for (int i = 0; i < WallCorners.Num(); ++i)
			{
				if (WallCorners[i])
				{
					if (WallCorners[i]->CornerProperty.ObjectId == TempCornerID_1 || WallCorners[i]->CornerProperty.ObjectId == TempCornerID_2)
					{
						WallCorners[i]->ShowOutterCircle(bShow);
						WallCorners[i]->ShowInnerCircel(bShow);
					}
					else
					{
						WallCorners[i]->ShowOutterCircle(false);
						WallCorners[i]->ShowInnerCircel(false);
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < WallCorners.Num(); ++i)
		{
			if (WallCorners[i])
			{
				WallCorners[i]->ShowOutterCircle(false);
				WallCorners[i]->ShowInnerCircel(false);
			}
		}
	}

}

bool UWallBuildSystem::IsMoveBorderWallCorner(int32 CornerID, FVector2D& WallCornerPos_1, FVector2D& WallCornerPos_2)
{
	WallCornerPos_1 = FVector2D::ZeroVector;
	WallCornerPos_2 = FVector2D::ZeroVector;
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager == nullptr) {
		return false;
	}

	UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(CornerID));
	if (CornerAdapter == nullptr) {
		return false;
	}

	FVector2D TargetPos = CornerAdapter->GetLocaltion();

	TArray<int32> Walls = CornerAdapter->GetWalls();
	if (Walls.Num() != 2)
	{
		return false;
	}
	UDRSolidWallAdapter* WallAdapter_1 = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(Walls[0]));
	if (WallAdapter_1 == nullptr)
	{
		return false;
	}
	int32 WallCorner1 = -1;
	float LeftThickness_1 = WallAdapter_1->GetLeftThick();
	float RightThickness_1 = WallAdapter_1->GetRightThick();
	float WallThickness_1 = LeftThickness_1 + RightThickness_1;

	if (WallAdapter_1->GetP0() != CornerID)
	{
		WallCorner1 = WallAdapter_1->GetP0();

	}
	else
	{
		WallCorner1 = WallAdapter_1->GetP1();
	}

	UDRCornerAdapter* CornerData_1 = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(WallCorner1));
	if (CornerData_1)
	{
		WallCornerPos_1 = CornerData_1->GetLocaltion();
	}

	UDRSolidWallAdapter* WallAdapter_2 = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(Walls[1]));
	if (WallAdapter_2 == nullptr)
	{
		return false;
	}
	int32 WallCorner2 = -1;
	float LeftThickness_2 = WallAdapter_2->GetLeftThick();
	float RightThickness_2 = WallAdapter_2->GetRightThick();
	float WallThickness_2 = LeftThickness_2 + RightThickness_2;

	if (WallAdapter_2->GetP0() != CornerID)
	{
		WallCorner2 = WallAdapter_2->GetP0();
	}
	else
	{
		WallCorner2 = WallAdapter_2->GetP1();
	}

	UDRCornerAdapter* CornerData_2 = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(WallCorner2));
	if (CornerData_2)
	{
		WallCornerPos_2 = CornerData_1->GetLocaltion();
	}

	TArray<int32> BorderCornerArray;
	TArray<int32> VirtualWallArray = BuildingSystem->GetAllTypedObjects(EDR_ObjectType::EDR_Edge, false);
	for (size_t i = 0; i < VirtualWallArray.Num(); i++)
	{
		UDRVirtualWallAdapter* TempEdge = Cast<UDRVirtualWallAdapter>(projectDataManager->GetAdapter(VirtualWallArray[i]));
		if (TempEdge)
		{
			UDRCornerAdapter* TempCornerAdapter1 = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(TempEdge->GetP0()));
			if (TempCornerAdapter1)
			{
				BorderCornerArray.Add(TempEdge->GetP0());
			}

			UDRCornerAdapter* TempCornerAdapter2 = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(TempEdge->GetP1()));
			if (TempCornerAdapter2)
			{
				BorderCornerArray.Add(TempEdge->GetP1());
			}
		}
	}
	for (int32 i = 0; i < BorderCornerArray.Num(); i++)
	{
		UDRCornerAdapter* TempCornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(BorderCornerArray[i]));
		if (TempCornerAdapter != nullptr)
		{
			FVector2D Pos_0 = TempCornerAdapter->GetLocaltion();
			if (FVector2D::Distance(TargetPos, Pos_0) <= WallThickness_1 || FVector2D::Distance(TargetPos, Pos_0) <= WallThickness_2)
			{
				return true;
			}
		}
	}

	return false;
}

void UWallBuildSystem::CorrectCornerPos(int32 CornerID)
{
	if (CornerID == -1)
	{
		return;
	}

	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager == nullptr) {
		return;
	}
	UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(CornerID));
	if (CornerAdapter == nullptr)
	{
		return;
	}

	int32 BestID;
	FVector2D BestLoc;
	//BuildingSystem->CutAreaSnap(CornerAdapter->GetLocaltion(), -1, BestLoc, BestID, 20.0, 40.0);
	GetWallIDFromPosition(CornerAdapter->GetLocaltion(), -1, BestLoc, BestID, 20.0, 40.0);

	UDRSolidWallAdapter* WallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(BestID));

	if (WallAdapter == nullptr) {
		UDRCornerAdapter* TempCornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(BestID));
		if (TempCornerAdapter != nullptr)
		{
			BuildingSystem->Move(BestID, FVector2D(0, 0));
			return;
		}
		return;
	}

	int32 P0 = WallAdapter->GetP0();
	int32 P1 = WallAdapter->GetP1();

	BuildingSystem->Move(P0, FVector2D(0, 0));

	for (int i = 0; i < WallCorners.Num(); ++i)
	{
		if (WallCorners[i])
		{
			if (WallCorners[i]->CornerProperty.ObjectId == P0)
			{
				IObject *pObj = BuildingSystem->GetObject(P0);
				if (pObj != nullptr)
				{
					WallCorners[i]->CornerProperty.Position = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
					break;
				}
			}
		}
	}

	BuildingSystem->Move(P1, FVector2D(0, 0));

	for (int i = 0; i < WallCorners.Num(); ++i)
	{
		if (WallCorners[i])
		{
			if (WallCorners[i]->CornerProperty.ObjectId == P1)
			{
				IObject *pObj = BuildingSystem->GetObject(P1);
				if (pObj != nullptr)
				{
					WallCorners[i]->CornerProperty.Position = ToVector2D(pObj->GetPropertyValue("Location").Vec2Value());
					break;
				}
			}
		}
	}
}

float UWallBuildSystem::GetHitWallThickness(int32 Obj)
{
	float Thickness = 10.0f;
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(this);
	if (projectDataManager == nullptr) {
		return Thickness;
	}
	IObject * HitObj = BuildingSystem->GetObject(Obj);
	EObjectType TempType = HitObj->GetType();
	if (TempType == EObjectType::ESolidWall)
	{
		UDRSolidWallAdapter* WallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(Obj));
		if (WallAdapter == nullptr)
		{
			return Thickness;
		}
		float LeftThickness = WallAdapter->GetLeftThick();
		float RightThickness = WallAdapter->GetRightThick();
		Thickness = (LeftThickness + RightThickness) / 2.0f;
	}
	else if (TempType == EObjectType::ECorner)
	{
		UDRCornerAdapter* CornerAdapter = Cast<UDRCornerAdapter>(projectDataManager->GetAdapter(Obj));
		if (CornerAdapter == nullptr) {
			return Thickness;
		}

		FVector2D TargetPos = CornerAdapter->GetLocaltion();

		TArray<int32> Walls = CornerAdapter->GetWalls();
		if (Walls.Num() == 0)
		{
			return Thickness;
		}
		UDRSolidWallAdapter* WallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(Walls[0]));
		if (WallAdapter == nullptr)
		{
			return Thickness;
		}
		float LeftThickness = WallAdapter->GetLeftThick();
		float RightThickness = WallAdapter->GetRightThick();
		Thickness = (LeftThickness + RightThickness) / 2.0f;
	}
	return Thickness;
}


void UWallBuildSystem::GetWallIDFromPosition(const FVector2D &Loc, int32 Ignore, FVector2D &BestLoc, int32 &BestID, float Torlerance, float ToleranceEdge)
{
	if (BuildingSystem)
	{
		if (BuildingSystem->Suite)
		{
			const unsigned int Op_AddWallFilter = ESnapFilter::ESFWallBorder | ESnapFilter::ESFCorner;
			int32 IsSnap = BuildingSystem->Suite->FindSnapLocation(FORCE_TYPE(kPoint, Loc), FORCE_TYPE(kPoint, BestLoc), BestID, Ignore, ESnapFilter(Op_AddWallFilter), Torlerance, ToleranceEdge);
		}
	}
}

void UWallBuildSystem::RebuildWall(int32 WallID, FVector2D CornerPos, FVector2D TargetPos)
{
	if (BuildingSystem)
	{
		TempHoleIDs.Empty();
		if (WallID != INDEX_NONE)
		{
			UBuildingData* Data = BuildingSystem->GetData(WallID);
			if (Data)
			{
				EObjectType Type = (EObjectType)Data->GetObjectType();

				if (Type == EObjectType::EDoorHole || Type == EObjectType::EWindow)
				{
					ShowToastCutWall();
					return;
				}
				if (Type == EObjectType::ESolidWall)
				{
					IObject * WallObject = BuildingSystem->GetObject(WallID);
					if (WallObject)
					{
						int P0 = Data->GetInt("P0");
						int P1 = Data->GetInt("P1");
						IObject *CornerData1 = BuildingSystem->GetObject(P0);
						IObject *CornerData2 = BuildingSystem->GetObject(P1);
						if (CornerData1&&CornerData2)
						{
							float ThickRight, ThickLeft;
							ThickRight = Data->GetFloat("ThickRight");
							ThickLeft = Data->GetFloat("ThickLeft");
							float ZPos;
							TArray<FVector> WallNodes;
							BuildingSystem->GetWallBorderLines(WallID, WallNodes, ZPos);

								IValue & _Value = WallObject->GetPropertyValue("Holes");
								const int num = _Value.GetArrayCount();
								for (int i = 0; i < num; ++i)
								{
									IValue & _Hole = _Value.GetField(i);
									IValue & _CValue = _Hole.GetField("HoleID");
									int _ID = _CValue.IntValue();
									TempHoleIDs.Add(_ID);
								}
								RecordHoleData(WallID);
								SetIsDeleteTempArea(true);
								DeleteSlectActor(WallID);
								BuildingSystem->ForceUpdateSuit();
								int32 NewCornerStart = -1, NewCornerEnd = -1;
								NewCornerStart = BuildingSystem->AddCorner(CornerPos);
								AddNewCorner(CornerPos, NewCornerStart);
								NewCornerEnd = BuildingSystem->AddCorner(TargetPos);
								AddNewCorner(TargetPos, NewCornerEnd);
								TArray<int32> WallIDs;
								WallIDs = BuildingSystem->AddWall(NewCornerStart, NewCornerEnd, ThickLeft, ThickRight, ZPos);
								BuildingSystem->ForceUpdateSuit();
								NewAddRecordedHoleData(WallIDs);
								BuildAreaWithVirWalls();
								ShowAllCornerUMG();
								return;
						}
					}
				}
			}
		}
	}
}