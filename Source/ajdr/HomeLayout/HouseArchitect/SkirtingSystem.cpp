// Copyright© 2017 ihomefnt All Rights Reserved.

#include "SkirtingSystem.h"
#include "EditorGameInstance.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include "HomeLayout/HouseArchitect/WallBuildSystem.h"
#include "Model/ResourceMgr.h"
#include "Model/ModelFileComponent.h"
#include "../SceneEntity/RoomActor.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Building/HouseComponent/ComponentManagerActor.h"


// Sets default values for this component's properties
ASkirtingSystem::ASkirtingSystem()
{

	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh> SkirtingLineMeshFinder(TEXT("/Game/FootLine/FootLine_03_mesh.FootLine_03_mesh"));
	SkirtingLineMesh = SkirtingLineMeshFinder.Object;
	ConstructorHelpers::FObjectFinder<UStaticMesh> SkirtingTopLineMeshFinder(TEXT("/Game/FootLine/PlasterLine_01.PlasterLine_01"));
	SkirtingTopLineMesh = SkirtingTopLineMeshFinder.Object;
	SkirtingLineMesh->AddToRoot();
	SkirtingTopLineMesh->AddToRoot();
}

void ASkirtingSystem::BeginPlay()
{
	Super::BeginPlay();
}
//创建角线时调用
USkirtingMeshComponent* ASkirtingSystem::GetSkirtingObjectBySnapPnt(const FVector2D &SnapPoint, UPARAM(ref) ESkirtingSnapType &SnapType, ESkirtingType InSkirtingType)
{
	//UE_LOG(LogTemp, Warning, TEXT("USkirtingMeshComponent* ASkirtingSystem::GetSkirtingObjectBySnapPnt"));
	auto SnapSkirtingMeshComponents = [SnapPoint, InSkirtingType, &SnapType](TArray<USkirtingMeshComponent*>& SkirtingComponents) -> USkirtingMeshComponent*
	{
		for (auto &SkirtingMesh : SkirtingComponents)
		{
			if (SkirtingMesh != nullptr)
			{
				ESkirtingSnapType CurSnapType = SkirtingMesh->GetPntSnap(SnapPoint, InSkirtingType);
				if (CurSnapType != ESkirtingSnapType::SnapNull)
				{
					SnapType = CurSnapType;
					return SkirtingMesh;
				}
			}
		}

		return nullptr;
	};

	SnapType = ESkirtingSnapType::SnapNull;
	USkirtingMeshComponent *FoundSkirting = nullptr;
	do
	{
		FoundSkirting = SnapSkirtingMeshComponents(SkirtingMeshComponents);
		if (FoundSkirting)
		{
			break;
		}
		FoundSkirting = SnapSkirtingMeshComponents(TopSkirtingMeshComponents);
		if (FoundSkirting)
		{
			break;
		}
		FoundSkirting = SnapSkirtingMeshComponents(SkirtingAreaMeshComps);
		if (FoundSkirting)
		{
			break;
		}
		FoundSkirting = SnapSkirtingMeshComponents(TopSkirtingAreaMeshComps);
		if (FoundSkirting)
		{
			break;
		}
	} while (false);

	return FoundSkirting;
}

USkirtingMeshComponent* ASkirtingSystem::GetSkirtingMeshByPnt(const FVector2D &SnapPnt)
{
	return nullptr;
}

USkirtingSegComponent* ASkirtingSystem::CreateSkirtingSegByNode(const FSkirtingSegNode &InSkirtingSegNode, UStaticMesh *SkirtingMesh)
{
	//UE_LOG(LogTemp, Warning, TEXT("USkirtingSegComponent* ASkirtingSystem::CreateSkirtingSegByNode"));
	// we should give a default static mesh
	if (nullptr == SkirtingMesh)
	{
	}

	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor *SkirtingSegActor = GetWorld()->SpawnActor<AActor>(SpawmParams);
	USkirtingSegComponent *SkirtingSeg = NewObject<USkirtingSegComponent>(SkirtingSegActor);
	if (SkirtingSeg != nullptr)
	{
		SkirtingSeg->ResetStaticMesh(SkirtingMesh);
		SkirtingSegActor->SetRootComponent(SkirtingSeg);
		//SkirtingSeg->AttachTo(, NAME_None, EAttachLocation::KeepWorldPosition);
		SkirtingSeg->GetRelatedNode() = InSkirtingSegNode;
		SkirtingSeg->RegisterComponent();
		SkirtingSeg->UpdateMeshByNode();
	}

	return SkirtingSeg;
}

void ASkirtingSystem::DelSkirtingComponentsByRect(const FVector2D &LeftTopPnt, const FVector2D &RightBottomPnt, ESkirtingType DelType)
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::DelSkirtingComponentsByRect"));
	auto checkDelSkirtingLine = [&](TArray<USkirtingMeshComponent*>& components) {
		TArray<USkirtingMeshComponent*> Skirtings2Del;
		FVector LeftBottom(FMath::Min(LeftTopPnt.X, RightBottomPnt.X), FMath::Min(LeftTopPnt.Y, RightBottomPnt.Y), 0);
		FVector RightTop(FMath::Max(LeftTopPnt.X, RightBottomPnt.X), FMath::Max(LeftTopPnt.Y, RightBottomPnt.Y), 0);
		FVector LeftTop(LeftBottom.X, RightTop.Y, 0);
		FVector RightBottom(RightTop.X, LeftBottom.Y, 0);
		FVector CrossTest;
		for (int32 i = 0; i < components.Num(); ++i)
		{
			USkirtingMeshComponent *com = components[i];
			if (com != nullptr)
			{
				FSkirtingNode& node = com->GetSkirtingSaveNode();

				if (node.SkirtingType != DelType)
				{
					continue;
				}

				for (int segIdx = 0; segIdx < node.SkirtingSegments.Num(); segIdx++)
				{
					FSkirtingSegNode &segNode = node.SkirtingSegments[segIdx];

					if ((FMath::IsWithinInclusive(segNode.SkirtingStartCorner.Position.X, LeftBottom.X, RightTop.X)
						&& FMath::IsWithinInclusive(segNode.SkirtingStartCorner.Position.Y, LeftBottom.Y, RightTop.Y))
						|| (FMath::IsWithinInclusive(segNode.SkirtingEndCorner.Position.X, LeftBottom.X, RightTop.X)
							&& FMath::IsWithinInclusive(segNode.SkirtingEndCorner.Position.Y, LeftBottom.Y, RightTop.Y))
						|| FMath::SegmentIntersection2D(FVector(segNode.SkirtingStartCorner.Position, 0), FVector(segNode.SkirtingEndCorner.Position, 0),
							LeftBottom, RightTop, CrossTest)
						|| FMath::SegmentIntersection2D(FVector(segNode.SkirtingStartCorner.Position, 0), FVector(segNode.SkirtingEndCorner.Position, 0),
							LeftTop, RightBottom, CrossTest))
					{
						FSkirtingNode newNode = node;



						//Shangshibo
						int segNodeT = 0;
						//上一个索引
						{
							if (segIdx - 1 == 0)
							{
								segNodeT = segIdx - 1;
								float SkirtingSize = FVector2D(node.SkirtingSegments[segNodeT].SkirtingStartCorner.Position - node.SkirtingSegments[segNodeT].SkirtingEndCorner.Position).Size();
								if (SkirtingSize < 1.f)
								{
									node.SkirtingSegments.RemoveAt(segIdx, node.SkirtingSegments.Num() - segIdx);
									node.SkirtingSegments.RemoveAt(segNodeT);
								}
								else
								{
									node.SkirtingSegments.RemoveAt(segIdx, node.SkirtingSegments.Num() - segIdx);
								}
							}
							else
							{
								node.SkirtingSegments.RemoveAt(segIdx, node.SkirtingSegments.Num() - segIdx);
							}


							if (node.SkirtingSegments.Num() == 0)
							{
								Skirtings2Del.Add(com);
							}
							else
							{
								com->UpdateMeshByNode();
							}
						}

						//下一个索引
						{
							if (segIdx == newNode.SkirtingSegments.Num() - 2)
							{
								segNodeT = (segIdx + 1) % newNode.SkirtingSegments.Num();
								float SkirtingSize = FVector2D(newNode.SkirtingSegments[segNodeT].SkirtingStartCorner.Position - newNode.SkirtingSegments[segNodeT].SkirtingEndCorner.Position).Size();
								if (SkirtingSize < 1.f)
								{
									newNode.SkirtingSegments.RemoveAt(segNodeT);
									newNode.SkirtingSegments.RemoveAt(0, segIdx);
								}
								else
								{
									newNode.SkirtingSegments.RemoveAt(0, segIdx + 1);
								}
							}
							else
							{
								newNode.SkirtingSegments.RemoveAt(0, segIdx + 1);
							}

							if (newNode.SkirtingSegments.Num() != 0)
							{
								CreateSkirtingComponentByNode(newNode);
							}
						}
						break;

					}
				}
			}
		}

		for (int32 i = components.Num() - 1; i >= 0; --i)
		{
			if (Skirtings2Del.Find(components[i]) >= 0)
			{
				if (components[i] != nullptr)
				{
					components[i]->GetOwner()->Destroy();
					components.RemoveAt(i);
				}
			}
		}
	};

	switch (DelType)
	{
	case ESkirtingType::SkirtingLine:
	{
		checkDelSkirtingLine(SkirtingMeshComponents);
		checkDelSkirtingLine(SkirtingAreaMeshComps);
	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{
		checkDelSkirtingLine(TopSkirtingMeshComponents);
		checkDelSkirtingLine(TopSkirtingAreaMeshComps);
	}
	break;
	default:
		break;
	}
}


void ASkirtingSystem::DeleteSkirtingSegs(const TArray<FVector2D> RegionPoints, ESkirtingType DelType)
{
	const float ExtendLength = 5.0f;
	if (DelType == ESkirtingType::SkirtingLine)
	{
		TArray<USkirtingMeshComponent*> Skirtings2Del;
		for (int32 i = 0; i < SkirtingMeshComponents.Num(); ++i)
		{
			USkirtingMeshComponent *com = SkirtingMeshComponents[i];
			if (com != nullptr)
			{
				FSkirtingNode& node = com->GetSkirtingSaveNode();

				TArray<int> ToDeleteSegIds;
				for (int segIdx = 0; segIdx < node.SkirtingSegments.Num(); segIdx++)
				{
					FSkirtingSegNode &segNode = node.SkirtingSegments[segIdx];
					//////////////////判定哪些线段在厨卫空间内部//////////////////////////
					FVector2D SegMiddle = 0.5*(segNode.SkirtingStartCorner.Position + segNode.SkirtingEndCorner.Position);
					FVector2D SegDir = segNode.SkirtingEndCorner.Position - segNode.SkirtingStartCorner.Position;
					SegDir.Normalize();
					FVector2D SegRotateVec = SegDir.GetRotated(90);
					FVector2D TestNode1 = SegMiddle + ExtendLength * SegRotateVec;
					FVector2D TestNode2 = SegMiddle - ExtendLength * SegRotateVec;

					bool Node1IsInRegion = FPolygonAlg::JudgePointInPolygon(RegionPoints, TestNode1);
					bool Node2IsInRegion = FPolygonAlg::JudgePointInPolygon(RegionPoints, TestNode2);
					if ((Node1IsInRegion) || (Node2IsInRegion))
					{
						ToDeleteSegIds.Add(segIdx);
					}
				}
				for (int Idx = ToDeleteSegIds.Num() - 1; Idx >= 0; Idx--)
				{
					node.SkirtingSegments.RemoveAt(ToDeleteSegIds[Idx]);
				}

				if (node.SkirtingSegments.Num() == 0)
				{
					Skirtings2Del.Add(com);
				}
				else
				{
					com->UpdateMeshByNode();
					CreateSkirtingComponentByNode(node);
				}
			}
		}



		for (int32 i = SkirtingMeshComponents.Num() - 1; i >= 0; --i)
		{
			if (Skirtings2Del.Find(SkirtingMeshComponents[i]) >= 0)
			{
				if (SkirtingMeshComponents[i] != nullptr)
				{
					SkirtingMeshComponents[i]->GetOwner()->Destroy();
					SkirtingMeshComponents.RemoveAt(i);
				}
			}
		}

		int FinalSegIdx = 0;
		for (int32 i = 0; i < SkirtingAreaMeshComps.Num(); ++i)
		{
			USkirtingMeshComponent *com = SkirtingAreaMeshComps[i];
			if (com != nullptr)
			{
				FSkirtingNode& node = com->GetSkirtingSaveNode();

				FSkirtingNode newNode = node;
				//TArray<int> ToDeleteSegIds;
				for (int segIdx = 0; segIdx < node.SkirtingSegments.Num(); segIdx++)
				{
					FSkirtingSegNode &segNode = node.SkirtingSegments[segIdx];
					//////////////////判定哪些线段在厨卫空间内部//////////////////////////
					FVector2D SegMiddle = 0.5*(segNode.SkirtingStartCorner.Position + segNode.SkirtingEndCorner.Position);
					FVector2D SegDir = segNode.SkirtingEndCorner.Position - segNode.SkirtingStartCorner.Position;
					SegDir.Normalize();
					FVector2D SegRotateVec = SegDir.GetRotated(90);
					FVector2D TestNode1 = SegMiddle + ExtendLength * SegRotateVec;
					FVector2D TestNode2 = SegMiddle - ExtendLength * SegRotateVec;

					bool Node1IsInRegion = FPolygonAlg::JudgePointInPolygon(RegionPoints, TestNode1);
					bool Node2IsInRegion = FPolygonAlg::JudgePointInPolygon(RegionPoints, TestNode2);
					if ((Node1IsInRegion) || (Node2IsInRegion))
					{
						//ToDeleteSegIds.Add(segIdx);
						FinalSegIdx = segIdx;
						node.SkirtingSegments.RemoveAt(FinalSegIdx, node.SkirtingSegments.Num() - FinalSegIdx);

						if (node.SkirtingSegments.Num() == 0)
						{
							Skirtings2Del.Add(com);
						}
						else
						{
							com->UpdateMeshByNode();
						}

						////////////////////////////////////////////////////////////////////////////////////
						newNode.SkirtingSegments.RemoveAt(0, FinalSegIdx + 1);
						if (newNode.SkirtingSegments.Num() != 0)
						{
							CreateSkirtingComponentByNode(newNode);
						}
						break;
					}
				}




			}
		}

		for (int32 i = SkirtingAreaMeshComps.Num() - 1; i >= 0; --i)
		{
			if (Skirtings2Del.Find(SkirtingAreaMeshComps[i]) >= 0)
			{
				if (SkirtingAreaMeshComps[i] != nullptr)
				{
					SkirtingAreaMeshComps[i]->GetOwner()->Destroy();
					SkirtingAreaMeshComps.RemoveAt(i);
				}
			}
		}

	}
}

void ASkirtingSystem::SyncAreaOutlinePathWithType(ESkirtingType InSkirtingType)
{

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::SyncAreaOutlinePathWithType"));
	//清楚已存在路径
	auto DestroyComponents = [](TArray<USkirtingMeshComponent*> &Components)
	{
		for (auto &Comp : Components)
		{
			if (Comp != nullptr)
			{
				Comp->GetOwner()->Destroy();
			}
		}
		Components.Empty();
	};
	//判断类型
	switch (InSkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{

		DestroyComponents(SkirtingMeshComponents);
		DestroyComponents(SkirtingAreaMeshComps);
	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{

		DestroyComponents(TopSkirtingMeshComponents);
		DestroyComponents(TopSkirtingAreaMeshComps);
	}
	break;
	default:
		break;
	}

	//区域角线模型计算
	TArray<AActor*> Components;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AComponentManagerActor::StaticClass(), Components);
	TArray<TArray<FVector2D>> TotalNodes;
	for (int I = 0; I < Components.Num(); ++I)
	{
		AComponentManagerActor * _TempCom = Cast<AComponentManagerActor>(Components[I]);
		if (_TempCom)
		{
			TArray<FVector2D> Nodes;
			bool GetSuccess = _TempCom->GetAllHouseComponent(Nodes);
			if (GetSuccess)
			{
				TotalNodes.Add(Nodes);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UCEditorGameInstance *GameInstance = Cast<UCEditorGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));




	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TArray<AActor*> _RoomRes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomActor::StaticClass(), _RoomRes);
	const FString Str3 = TEXT("厨房");
	const FString Str4 = TEXT("客卫");
	const FString Str5 = TEXT("主卫");

	TArray<FBoundaryPsOfRegion> RegionsToRemove;
	for (int i = 0; i < _RoomRes.Num(); ++i)
	{
		ARoomActor * _Room = Cast<ARoomActor>(_RoomRes[i]);
		if (_Room)
		{
			FString TempStr = _Room->RoomUseName;
			if ((TempStr.Contains(Str3)) || (TempStr.Contains(Str4)) || (TempStr.Contains(Str5)))
			{
				FRoomPath TempRoomPs = _Room->GetRoomPath();
				FBoundaryPsOfRegion TempBoundPs;
				TempBoundPs.BoundaryPs = TempRoomPs.InnerRoomPath;
				RegionsToRemove.Add(TempBoundPs);
			}
		}
	}







	TArray<FRoomPath> AreaPathArray = GameInstance->WallBuildSystem->GetInnerRoomPathArray();
	TArray<FRoomPath> TempArea;
	TArray<TArray<FVector2D>> AllInnerRegions;
	TMap<int32, TArray<TArray<FVector2D>>> IndexMapInnerPs;
	TMap<int32, TArray<int32>> RegionMapRegions;

	TMap<int, TArray<TArray<FVector2D>>> RoomMapComponents;
	TArray<FVector2D> TotalNoSnapComps;
	for (int I = 0; I < AreaPathArray.Num(); ++I)
	{
		TArray<FVector2D> TempPath = AreaPathArray[I].InnerRoomPath;

		TArray<TArray<FVector2D>> TempArrArr;
		AllInnerRegions.Add(TempPath);
		for (int J = 0; J < TotalNodes.Num(); ++J)
		{
			TArray<FVector2D> TempArr = TotalNodes[J];
			int NumOfArr = TempArr.Num() / 4;
			for (int K = 0; K < NumOfArr; ++K)
			{
				FVector2D TempCenter = 0.25*(TempArr[4 * K] + TempArr[4 * K + 1] + TempArr[4 * K + 2] + TempArr[4 * K + 3]);
				if (FPolygonAlg::JudgePointInPolygon(TempPath, TempCenter))
				{
					TempArrArr.Add(TempArr);
				}
			}
		}
		if (TempArrArr.Num() > 0)
		{
			RoomMapComponents.Add(I, TempArrArr);
		}

	}

	TArray<int> UnNecessaryIndexs;

	for (int J = 0; J < RegionsToRemove.Num(); ++J)
	{
		for (int32 I = 0; I < AllInnerRegions.Num(); ++I)
		{
			FVector2D TempCenter;
			FPolygonAlg::LocateSuitableCenter(RegionsToRemove[J].BoundaryPs, TempCenter);
			if (FPolygonAlg::JudgePointInPolygon(AllInnerRegions[I], TempCenter))
			{
				UnNecessaryIndexs.AddUnique(I);
				break;
			}
		}
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
	TArray<int> CollectedSurfIndexs;
	if (IndexMapInnerPs.Num() != 0)
	{
		for (int32 RegionIndex = 0; RegionIndex < AllInnerRegions.Num(); ++RegionIndex)
		{
			TArray<FVector2D> UpdateTempRegionPs;
			UpdateTempRegionPs = AllInnerRegions[RegionIndex];
			if ((!CalculatedIndexs.Contains(RegionIndex))&&(!UnNecessaryIndexs.Contains(RegionIndex)))
			{
				if (IndexMapInnerPs.Contains(RegionIndex))
				{
					TArray<TArray<FVector2D>> TempGroupInnerPs = IndexMapInnerPs[RegionIndex];
					TArray<int32> PairedRegionIndexs = RegionMapRegions[RegionIndex];
					TArray<int> CollectInts;
					if ((PairedRegionIndexs.Num() == 1)&&(!UnNecessaryIndexs.Contains(PairedRegionIndexs[0]))&&(!CalculatedIndexs.Contains(PairedRegionIndexs[0])))
					{
						
						TArray<int32> TempPair = RegionMapRegions[PairedRegionIndexs[0]];
						if ((TempPair.Num() == 1)&&(!UnNecessaryIndexs.Contains(TempPair[0])))
						{
							CalculatedIndexs.AddUnique(RegionIndex);
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

							if (Polygons.Num() > 0)
							{
								TArray<FVector2D> TempCombinedComps;

								for (int _CurInt = 0; _CurInt < CollectInts.Num(); ++_CurInt)
								{
									if (RoomMapComponents.Contains(CollectInts[_CurInt]))
									{
										TArray<TArray<FVector2D>> TempArrOfArr = RoomMapComponents[CollectInts[_CurInt]];
										for (int _CurJ = 0; _CurJ < TempArrOfArr.Num(); ++_CurJ)
										{
											TArray<FVector2D> TempA = TempArrOfArr[_CurJ];
											for (int _CurK = 0; _CurK < TempA.Num(); ++_CurK)
											{
												TempCombinedComps.Add(TempA[_CurK]);
											}
										}
									}
								}
								TArray<FVector2D> UpdatePolygons, NoSnapComps2;
								bool TotalSnap = FPolygonAlg::RegionSnapMoreComps(Polygons, TempCombinedComps, UpdatePolygons, NoSnapComps2);
								if (TotalSnap)
								{
									if (IsPolyClockWise(UpdatePolygons))
									{
										ChangeClockwise(UpdatePolygons);
									}
									FSkirtingNode SkirtingNode;
									SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
									SkirtingNode.InitFromAreaPath(UpdatePolygons, InSkirtingType);
									SetSkiringDefaultData(SkirtingNode);
									CreateSkirtingComponentByNode(SkirtingNode);

									for (int _CurS = 0; _CurS < NoSnapComps2.Num(); ++_CurS)
									{
										TotalNoSnapComps.AddUnique(NoSnapComps2[_CurS]);
									}
								}
								else
								{
									if (Polygons.Num() > 0)
									{
										if (IsPolyClockWise(Polygons))
										{
											ChangeClockwise(Polygons);
										}
										FSkirtingNode SkirtingNode;
										SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
										SkirtingNode.InitFromAreaPath(Polygons, InSkirtingType);
										SetSkiringDefaultData(SkirtingNode);
										CreateSkirtingComponentByNode(SkirtingNode);
									}

								}
							}
						}
						else if (TempPair.Num() >= 2)
						{
							CollectInts.Empty();
							CollectInts.AddUnique(RegionIndex);
							for (int32 I = 0; I < TempPair.Num(); ++I)
							{
								if ((!UnNecessaryIndexs.Contains(TempPair[I]))&&(!CalculatedIndexs.Contains(TempPair[I])))
								{
									CollectInts.AddUnique(TempPair[I]);
									CalculatedIndexs.AddUnique(TempPair[I]);
									TArray<int32> TempNextPair = RegionMapRegions[TempPair[I]];
									for (int32 J = 0; J < TempNextPair.Num(); ++J)
									{
										if ((!UnNecessaryIndexs.Contains(TempNextPair[J]))&&(!CalculatedIndexs.Contains(TempNextPair[J])))
										{
											CollectInts.AddUnique(TempNextPair[J]);
											CalculatedIndexs.AddUnique(TempNextPair[J]);
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
								TArray<FVector2D> TempCombinedComps;

								for (int _CurInt = 0; _CurInt < CollectInts.Num(); ++_CurInt)
								{
									if (RoomMapComponents.Contains(CollectInts[_CurInt]))
									{
										TArray<TArray<FVector2D>> TempArrOfArr = RoomMapComponents[CollectInts[_CurInt]];
										for (int _CurJ = 0; _CurJ < TempArrOfArr.Num(); ++_CurJ)
										{
											TArray<FVector2D> TempA = TempArrOfArr[_CurJ];
											for (int _CurK = 0; _CurK < TempA.Num(); ++_CurK)
											{
												TempCombinedComps.Add(TempA[_CurK]);
											}
										}
									}
								}
								TArray<FVector2D> UpdatePolygons, NoSnapComps2;
								bool TotalSnap = FPolygonAlg::RegionSnapMoreComps(Polygons, TempCombinedComps, UpdatePolygons, NoSnapComps2);
								if (TotalSnap)
								{
									if (IsPolyClockWise(UpdatePolygons))
									{
										ChangeClockwise(UpdatePolygons);
									}
									FSkirtingNode SkirtingNode;
									SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
									SkirtingNode.InitFromAreaPath(UpdatePolygons, InSkirtingType);
									SetSkiringDefaultData(SkirtingNode);
									CreateSkirtingComponentByNode(SkirtingNode);

									for (int _CurS = 0; _CurS < NoSnapComps2.Num(); ++_CurS)
									{
										TotalNoSnapComps.AddUnique(NoSnapComps2[_CurS]);
									}
								}
								else
								{
									if (Polygons.Num() > 0)
									{
										if (IsPolyClockWise(Polygons))
										{
											ChangeClockwise(Polygons);
										}
										FSkirtingNode SkirtingNode;
										SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
										SkirtingNode.InitFromAreaPath(Polygons, InSkirtingType);
										SetSkiringDefaultData(SkirtingNode);
										CreateSkirtingComponentByNode(SkirtingNode);
									}

								}
							}
						}
					}
					else if (PairedRegionIndexs.Num() >= 2)
					{
						CollectInts.Empty();
						CollectInts.AddUnique(RegionIndex);
						CalculatedIndexs.AddUnique(RegionIndex);
						int32 BaseCount = 1, EndCount = 0;
						TArray<int32> UpdateRegionIndexs;
						while (EndCount != BaseCount)
						{
							for (int32 I = 0; I < PairedRegionIndexs.Num(); ++I)
							{
								if (!UnNecessaryIndexs.Contains(PairedRegionIndexs[I]))
								{
									CollectInts.AddUnique(PairedRegionIndexs[I]);
									CalculatedIndexs.AddUnique(PairedRegionIndexs[I]);
									BaseCount = CollectInts.Num();
									TArray<int32> TempNextPair = RegionMapRegions[PairedRegionIndexs[I]];
									for (int32 P = 0; P < TempNextPair.Num(); ++P)
									{
										if (!UnNecessaryIndexs.Contains(TempNextPair[P]))
										{
											if (!CollectInts.Contains(TempNextPair[P]))
											{
												CollectInts.AddUnique(TempNextPair[P]);
												CalculatedIndexs.AddUnique(TempNextPair[P]);
												UpdateRegionIndexs.AddUnique(TempNextPair[P]);
											}
										}
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
							TArray<FVector2D> TempCombinedComps;

							for (int _CurInt = 0; _CurInt < CollectInts.Num(); ++_CurInt)
							{
								if (RoomMapComponents.Contains(CollectInts[_CurInt]))
								{
									TArray<TArray<FVector2D>> TempArrOfArr = RoomMapComponents[CollectInts[_CurInt]];
									for (int _CurJ = 0; _CurJ < TempArrOfArr.Num(); ++_CurJ)
									{
										TArray<FVector2D> TempA = TempArrOfArr[_CurJ];
										for (int _CurK = 0; _CurK < TempA.Num(); ++_CurK)
										{
											TempCombinedComps.Add(TempA[_CurK]);
										}
									}
								}
							}
							TArray<FVector2D> UpdatePolygons, NoSnapComps2;
							bool TotalSnap = FPolygonAlg::RegionSnapMoreComps(Polygons, TempCombinedComps, UpdatePolygons, NoSnapComps2);
							if (TotalSnap)
							{
								if (IsPolyClockWise(UpdatePolygons))
								{
									ChangeClockwise(UpdatePolygons);
								}
								FSkirtingNode SkirtingNode;
								SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
								SkirtingNode.InitFromAreaPath(UpdatePolygons, InSkirtingType);
								SetSkiringDefaultData(SkirtingNode);
								CreateSkirtingComponentByNode(SkirtingNode);

								for (int _CurS = 0; _CurS < NoSnapComps2.Num(); ++_CurS)
								{
									TotalNoSnapComps.AddUnique(NoSnapComps2[_CurS]);
								}
							}
							else
							{
								if (Polygons.Num() > 0)
								{
									if (IsPolyClockWise(Polygons))
									{
										ChangeClockwise(Polygons);
									}
									FSkirtingNode SkirtingNode;
									SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
									SkirtingNode.InitFromAreaPath(Polygons, InSkirtingType);
									SetSkiringDefaultData(SkirtingNode);
									CreateSkirtingComponentByNode(SkirtingNode);
								}

							}
						}
					}
				}
				else
				{
					CalculatedIndexs.Add(RegionIndex);
					if (RoomMapComponents.Contains(RegionIndex))
					{
						TArray<FVector2D> TempCombinedComps;
						if (RoomMapComponents.Contains(RegionIndex))
						{
							TArray<TArray<FVector2D>> TempArrOfArr = RoomMapComponents[RegionIndex];
							for (int _CurJ = 0; _CurJ < TempArrOfArr.Num(); ++_CurJ)
							{
								TArray<FVector2D> TempA = TempArrOfArr[_CurJ];
								for (int _CurK = 0; _CurK < TempA.Num(); ++_CurK)
								{
									TempCombinedComps.Add(TempA[_CurK]);
								}
							}
						}
						TArray<FVector2D> UpdatePolygons4, NoSnapComps4;
						bool TotalSnap = FPolygonAlg::RegionSnapMoreComps(UpdateTempRegionPs, TempCombinedComps, UpdatePolygons4, NoSnapComps4);
						if (TotalSnap)
						{
							if (IsPolyClockWise(UpdatePolygons4))
							{
								ChangeClockwise(UpdatePolygons4);
							}
							FSkirtingNode SkirtingNode;
							SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
							SkirtingNode.InitFromAreaPath(UpdatePolygons4, InSkirtingType);
							SetSkiringDefaultData(SkirtingNode);
							CreateSkirtingComponentByNode(SkirtingNode);

							for (int _CurS = 0; _CurS < NoSnapComps4.Num(); ++_CurS)
							{
								TotalNoSnapComps.AddUnique(NoSnapComps4[_CurS]);
							}
						}
						else
						{
							if (IsPolyClockWise(UpdateTempRegionPs))
							{
								ChangeClockwise(UpdateTempRegionPs);
							}
							FSkirtingNode SkirtingNode;
							SkirtingNode.InitFromAreaPath(UpdateTempRegionPs, InSkirtingType);
							SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
							SetSkiringDefaultData(SkirtingNode);
							CreateSkirtingComponentByNode(SkirtingNode);
						}
					}
					else
					{
						if (IsPolyClockWise(UpdateTempRegionPs))
						{
							ChangeClockwise(UpdateTempRegionPs);
						}
						FSkirtingNode SkirtingNode;
						SkirtingNode.InitFromAreaPath(UpdateTempRegionPs, InSkirtingType);
						SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						SetSkiringDefaultData(SkirtingNode);
						CreateSkirtingComponentByNode(SkirtingNode);
					}
				}

			}


		}
	}
	else
	{
		TArray<int> CollectNoLivingOrDiningIndexs;
		for (int I = 0; I < AreaPathArray.Num(); ++I)
		{
			FRoomPath TempRoom = AreaPathArray[I];
			TArray<FVector2D> TempRegion = TempRoom.InnerRoomPath;
			TArray<FVector2D> NewUpdateRegionPs, NewNoSnapComps;
			if (!TempRoom.bIsLivingOrDiningRoom)
			{
				if (RoomMapComponents.Contains(I))
				{
					TArray<TArray<FVector2D>> TempArrs = RoomMapComponents[I];
					TArray<FVector2D> NewComps;
					for (int J = 0; J < TempArrs.Num(); ++J)
					{
						for (int K = 0; K < TempArrs[J].Num(); ++K)
						{
							NewComps.Add(TempArrs[J][K]);
						}
					}
					bool NewSnap = FPolygonAlg::RegionSnapMoreComps(TempRegion, NewComps, NewUpdateRegionPs, NewNoSnapComps);
					if (NewSnap)
					{
						if (IsPolyClockWise(NewUpdateRegionPs))
						{
							ChangeClockwise(NewUpdateRegionPs);
						}

						FSkirtingNode SkirtingNode;
						SkirtingNode.InitFromAreaPath(NewUpdateRegionPs, InSkirtingType);
						SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						SetSkiringDefaultData(SkirtingNode);
						CreateSkirtingComponentByNode(SkirtingNode);

						for (int _CurS = 0; _CurS < NewNoSnapComps.Num(); ++_CurS)
						{
							TotalNoSnapComps.AddUnique(NewNoSnapComps[_CurS]);
						}
					}
				}
				else
				{

					if (IsPolyClockWise(TempRegion))
					{
						ChangeClockwise(TempRegion);
					}
					FSkirtingNode SkirtingNode;
					SkirtingNode.InitFromAreaPath(TempRegion, InSkirtingType);
					SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					SetSkiringDefaultData(SkirtingNode);
					CreateSkirtingComponentByNode(SkirtingNode);
				}

			}
			else
			{
				TempArea.Add(TempRoom);
				CollectNoLivingOrDiningIndexs.Add(I);
			}
		}


		if (TempArea.Num() == 2)
		{
			/////判断区域是否相邻
			bool IsClose = FPolygonAlg::JudgeHasClosePoint(TempArea[0].InnerRoomPath, TempArea[1].InnerRoomPath);
			if (IsClose)
			{
				TArray<TArray<FVector2D>> UpdateRegions;
				for (int I = 0; I < CollectNoLivingOrDiningIndexs.Num(); ++I)
				{
					TArray<FVector2D> TempRoom = TempArea[I].InnerRoomPath;
					if (RoomMapComponents.Contains(CollectNoLivingOrDiningIndexs[I]))
					{
						TArray<TArray<FVector2D>> TempArrArr = RoomMapComponents[CollectNoLivingOrDiningIndexs[I]];
						TArray<FVector2D> NewComps, UpdateRegion, UpdateNoSnapComps;
						for (int J = 0; J < TempArrArr.Num(); ++J)
						{
							for (int K = 0; K < TempArrArr[J].Num(); ++K)
							{
								NewComps.Add(TempArrArr[J][K]);
							}
						}
						bool NewSnap = FPolygonAlg::RegionSnapMoreComps(TempRoom, NewComps, UpdateRegion, UpdateNoSnapComps);
						if (NewSnap)
						{
							UpdateRegions.Add(UpdateRegion);
							for (int _CurS = 0; _CurS < UpdateNoSnapComps.Num(); ++_CurS)
							{
								TotalNoSnapComps.AddUnique(UpdateNoSnapComps[_CurS]);
							}
						}
					}
					else
					{
						UpdateRegions.Add(TempRoom);
					}
				}
				if (UpdateRegions.Num() == 2)
				{
					TArray<FVector2D> NewRegionPs0, NewRegionPs1, TempCombinedPs;
					bool IsCombined = FPolygonAlg::CreatePolygonUnion(UpdateRegions[0], UpdateRegions[1], NewRegionPs0, NewRegionPs1, TempCombinedPs);
					if (IsCombined)
					{
						//纠正方向
						if (IsPolyClockWise(NewRegionPs0))
						{
							ChangeClockwise(NewRegionPs0);
						}

						FSkirtingNode SkirtingNode1;
						SkirtingNode1.InitFromAreaPath(NewRegionPs0, InSkirtingType,false);
						SkirtingNode1.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						SetSkiringDefaultData(SkirtingNode1);
						CreateSkirtingComponentByNode(SkirtingNode1);
						
						if (IsPolyClockWise(NewRegionPs1))
						{
							ChangeClockwise(NewRegionPs1);
						}
						FSkirtingNode SkirtingNode2;
						SkirtingNode2.InitFromAreaPath(NewRegionPs1, InSkirtingType, false);
						SkirtingNode2.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						SetSkiringDefaultData(SkirtingNode2);
						CreateSkirtingComponentByNode(SkirtingNode2);
					}

				}

			}
			else
			{
				for (int I = 0; I < CollectNoLivingOrDiningIndexs.Num(); ++I)
				{
					TArray<FVector2D> TempRoom = TempArea[I].InnerRoomPath;
					if (RoomMapComponents.Contains(CollectNoLivingOrDiningIndexs[I]))
					{
						TArray<TArray<FVector2D>> TempArrArr = RoomMapComponents[CollectNoLivingOrDiningIndexs[I]];
						TArray<FVector2D> NewComps, UpdateRegion, UpdateNoSnapComps;
						for (int J = 0; J < TempArrArr.Num(); ++J)
						{
							for (int K = 0; K < TempArrArr[J].Num(); ++K)
							{
								NewComps.Add(TempArrArr[J][K]);
							}
						}
						bool NewSnap = FPolygonAlg::RegionSnapMoreComps(TempRoom, NewComps, UpdateRegion, UpdateNoSnapComps);
						if (NewSnap)
						{
							if (IsPolyClockWise(UpdateRegion))
							{
								ChangeClockwise(UpdateRegion);
							}
							FSkirtingNode SkirtingNode;
							SkirtingNode.InitFromAreaPath(UpdateRegion, InSkirtingType);
							SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
							SetSkiringDefaultData(SkirtingNode);
							CreateSkirtingComponentByNode(SkirtingNode);
							for (int _CurS = 0; _CurS < UpdateNoSnapComps.Num(); ++_CurS)
							{
								TotalNoSnapComps.AddUnique(UpdateNoSnapComps[_CurS]);
							}
						}
					}
					else
					{
						if (IsPolyClockWise(TempRoom))
						{
							ChangeClockwise(TempRoom);
						}
						FSkirtingNode SkirtingNode;
						SkirtingNode.InitFromAreaPath(TempRoom, InSkirtingType);
						SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						SetSkiringDefaultData(SkirtingNode);
						CreateSkirtingComponentByNode(SkirtingNode);
					}
				}
			}
		}
		else
		{
			for (int I = 0; I < CollectNoLivingOrDiningIndexs.Num(); ++I)
			{
				TArray<FVector2D> TempRoom = TempArea[I].InnerRoomPath;
				if (RoomMapComponents.Contains(CollectNoLivingOrDiningIndexs[I]))
				{
					TArray<TArray<FVector2D>> TempArrArr = RoomMapComponents[CollectNoLivingOrDiningIndexs[I]];
					TArray<FVector2D> NewComps, UpdateRegion, UpdateNoSnapComps;
					for (int J = 0; J < TempArrArr.Num(); ++J)
					{
						for (int K = 0; K < TempArrArr[J].Num(); ++K)
						{
							NewComps.Add(TempArrArr[J][K]);
						}
					}
					bool NewSnap = FPolygonAlg::RegionSnapMoreComps(TempRoom, NewComps, UpdateRegion, UpdateNoSnapComps);
					if (NewSnap)
					{
						if (IsPolyClockWise(UpdateRegion))
						{
							ChangeClockwise(UpdateRegion);
						}
						FSkirtingNode SkirtingNode;
						SkirtingNode.InitFromAreaPath(UpdateRegion, InSkirtingType);
						SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						SetSkiringDefaultData(SkirtingNode);
						CreateSkirtingComponentByNode(SkirtingNode);
						for (int _CurS = 0; _CurS < UpdateNoSnapComps.Num(); ++_CurS)
						{
							TotalNoSnapComps.AddUnique(UpdateNoSnapComps[_CurS]);
						}
					}
				}
				else
				{
					if (IsPolyClockWise(TempRoom))
					{
						ChangeClockwise(TempRoom);
					}
					FSkirtingNode SkirtingNode;
					SkirtingNode.InitFromAreaPath(TempRoom, InSkirtingType);
					SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					SetSkiringDefaultData(SkirtingNode);
					CreateSkirtingComponentByNode(SkirtingNode);
				}
			}
		}

	}



	int NumOfNoSnapComps = TotalNoSnapComps.Num() / 4;
	for (int _CurNoSnapI = 0; _CurNoSnapI < NumOfNoSnapComps; ++_CurNoSnapI)
	{
		TArray<FVector2D> TempGroupPs;
		TempGroupPs.Add(TotalNoSnapComps[0 + 4 * _CurNoSnapI]);
		TempGroupPs.Add(TotalNoSnapComps[1 + 4 * _CurNoSnapI]);
		TempGroupPs.Add(TotalNoSnapComps[2 + 4 * _CurNoSnapI]);
		TempGroupPs.Add(TotalNoSnapComps[3 + 4 * _CurNoSnapI]);


		//纠正组件轮廓
		if (IsPolyClockWise(TempGroupPs))
		{
			ChangeClockwise(TempGroupPs);
		}
		FSkirtingNode SkirtingNode;
		SkirtingNode.InitFromAreaPath(TempGroupPs, InSkirtingType);
		SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
		SetSkiringDefaultData(SkirtingNode, true);
		CreateSkirtingComponentByNodes(SkirtingNode, true);
	}


	for (int _CurR = 0; _CurR < RegionsToRemove.Num(); ++_CurR)
	{
		DeleteSkirtingSegs(RegionsToRemove[_CurR].BoundaryPs, InSkirtingType);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void ASkirtingSystem::LocalRegionCreateDecorateLine(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassID,
	int32 CraftID,int32 MaterialModelID, FString MaterialResID, ESkirtingType InSkirtingType)
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::LocalRegionCreateDecorateLine"));
	//清除已存在路径
	auto DestroyComponents = [](TArray<USkirtingMeshComponent*> &Components)
	{
		for (auto &Comp : Components)
		{
			if (Comp != nullptr)
			{
				Comp->GetOwner()->Destroy();
			}
		}
		Components.Empty();
	};

	TArray<FVector2D> AreaPath = RoomIDMapRegionPs(RoomID);
	int32 LivingRoomID = 0, DiningRoomID = 0;
	TArray<FVector2D> LivingRoomPs, DiningRoomPs;
	GetLivingRoomRegion(LivingRoomID, LivingRoomPs);
	GetDiningRoomRegion(DiningRoomID, DiningRoomPs);

	if ((RoomID != LivingRoomID) && (RoomID != DiningRoomID))
	{
		//判断类型
		switch (InSkirtingType)
		{
		case ESkirtingType::SkirtingLine:
		{
			TArray<USkirtingMeshComponent*> ToRemoveMeshCompts;
			TArray< USkirtingMeshComponent*> ToRemoveAreaCompts;
			for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
			{
				FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveMeshCompts.Add(SkirtingMeshComponents[I]);
					}
					break;
				}
			}

			for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
			{
				FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveAreaCompts.Add(SkirtingAreaMeshComps[K]);
					}
					break;
				}
			}


			DestroyComponents(ToRemoveMeshCompts);
			DestroyComponents(ToRemoveAreaCompts);
			for (int32 Index = SkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
			{
				if (SkirtingMeshComponents[Index] == nullptr)
				{
					SkirtingMeshComponents.RemoveAt(Index);
				}
			}
			for (int32 Index = SkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
			{
				if (SkirtingAreaMeshComps[Index] == nullptr)
				{
					SkirtingAreaMeshComps.RemoveAt(Index);
				}
			}

		}
		break;
		case ESkirtingType::SkirtingTopLine:
		{

			TArray<USkirtingMeshComponent*> ToRemoveTopMeshCompts;
			TArray< USkirtingMeshComponent*> ToRemoveTopAreaCompts;
			for (int32 I = 0; I < TopSkirtingMeshComponents.Num(); I++)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveTopMeshCompts.Add(TopSkirtingMeshComponents[I]);
					}
					break;
				}
			}
			for (int32 K = 0; K < TopSkirtingAreaMeshComps.Num(); K++)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveTopAreaCompts.Add(TopSkirtingAreaMeshComps[K]);
					}
					break;
				}
			}


			DestroyComponents(ToRemoveTopMeshCompts);
			DestroyComponents(ToRemoveTopAreaCompts);
			for (int32 Index = TopSkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
			{
				if (TopSkirtingMeshComponents[Index] == nullptr)
				{
					TopSkirtingMeshComponents.RemoveAt(Index);
				}
			}
			for (int32 Index = TopSkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
			{
				if (TopSkirtingAreaMeshComps[Index] == nullptr)
				{
					TopSkirtingAreaMeshComps.RemoveAt(Index);
				}
			}

		}
		break;
		default:
			break;
		}

		FSkirtingNode SkirtingNode;
		SkirtingNode.InitFromAreaPath(AreaPath, InSkirtingType);
		SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;

		SkirtingNode.SkirtingMeshModelID = ModelID;
		SkirtingNode.RoomClassID = RoomClassID;
		SkirtingNode.CraftId = CraftID;
		SkirtingNode.SkirtingMeshID = ModelResID;
		SkirtingNode.MaterialID = MaterialResID;
		CreateSkirtingComponentByNode(SkirtingNode);
	}
	else if ((RoomID == LivingRoomID) || (RoomID == DiningRoomID))
	{

		//判断类型
		switch (InSkirtingType)
		{
		case ESkirtingType::SkirtingLine:
		{
			TArray<USkirtingMeshComponent*> ToRemoveMeshCompts;
			TArray< USkirtingMeshComponent*> ToRemoveAreaCompts;
			for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
			{
				FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveMeshCompts.Add(SkirtingMeshComponents[I]);
					}
					break;
				}
			}
			for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
			{
				FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveAreaCompts.Add(SkirtingAreaMeshComps[K]);
					}
					break;
				}
			}


			DestroyComponents(ToRemoveMeshCompts);
			DestroyComponents(ToRemoveAreaCompts);
			for (int32 Index = SkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
			{
				if (SkirtingMeshComponents[Index] == nullptr)
				{
					SkirtingMeshComponents.RemoveAt(Index);
				}
			}
			for (int32 Index = SkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
			{
				if (SkirtingAreaMeshComps[Index] == nullptr)
				{
					SkirtingAreaMeshComps.RemoveAt(Index);
				}
			}
		}
		break;
		case ESkirtingType::SkirtingTopLine:
		{

			TArray<USkirtingMeshComponent*> ToRemoveTopMeshCompts;
			TArray< USkirtingMeshComponent*> ToRemoveTopAreaCompts;
			for (int32 I = 0; I < TopSkirtingMeshComponents.Num(); I++)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveTopMeshCompts.Add(TopSkirtingMeshComponents[I]);
					}
					break;
				}
			}
			for (int32 K = 0; K < TopSkirtingAreaMeshComps.Num(); K++)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveTopAreaCompts.Add(TopSkirtingAreaMeshComps[K]);
					}
					break;
				}
			}


			DestroyComponents(ToRemoveTopMeshCompts);
			DestroyComponents(ToRemoveTopAreaCompts);
			for (int32 Index = TopSkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
			{
				if (TopSkirtingMeshComponents[Index] == nullptr)
				{
					TopSkirtingMeshComponents.RemoveAt(Index);
				}
			}
			for (int32 Index = TopSkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
			{
				if (TopSkirtingAreaMeshComps[Index] == nullptr)
				{
					TopSkirtingAreaMeshComps.RemoveAt(Index);
				}
			}

		}
		break;
		default:
			break;
		}

		TArray<FVector2D> PolygonA, PolygonB;
		if (LivingRoomPs.Num() < DiningRoomPs.Num())
		{
			PolygonA = LivingRoomPs;
			PolygonB = DiningRoomPs;
		}
		else if (LivingRoomPs.Num() > DiningRoomPs.Num())
		{
			PolygonA = DiningRoomPs;
			PolygonB = LivingRoomPs;
		}
		else
		{
			float L1 = 0.0, L2 = 0.0;
			int32 NextI = 0;
			for (int32 I = 0; I < LivingRoomPs.Num(); I++)
			{
				if (I == LivingRoomPs.Num() - 1)
				{
					NextI = 0;
				}
				else
				{
					NextI = I + 1;
				}
				L1 = L1 + FVector2D::Distance(LivingRoomPs[I], LivingRoomPs[NextI]);
			}
			int32 NextJ = 0;
			for (int32 J = 0; J < DiningRoomPs.Num(); J++)
			{
				if (J == DiningRoomPs.Num() - 1)
				{
					NextJ = 0;
				}
				else
				{
					NextJ = J + 1;
				}
				L2 = L2 + FVector2D::Distance(DiningRoomPs[J], DiningRoomPs[NextJ]);
			}

			if (L1 < L2)
			{
				PolygonA = LivingRoomPs;
				PolygonB = DiningRoomPs;
			}
			else
			{
				PolygonA = DiningRoomPs;
				PolygonB = LivingRoomPs;
			}
		}

		TArray<FVector2D> TPolygonA;
		TArray<FVector2D> TPolygonB;
		TArray<FVector2D> Polygons;
		//将顶点改为顺时针
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

		int32 NumCount = 0;
		for (int32 Index = 0; Index < AreaPath.Num(); Index++)
		{
			if (TPolygonA.Contains(AreaPath[Index]))
			{
				NumCount++;
			}
		}
		if (NumCount >= 3)
		{
			FSkirtingNode SkirtingNodeA;
			SkirtingNodeA.InitFromAreaPath(TPolygonA, InSkirtingType, false);
			SkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;

			SkirtingNodeA.SkirtingMeshModelID = ModelID;
			SkirtingNodeA.RoomClassID = RoomClassID;
			SkirtingNodeA.CraftId = CraftID;
			SkirtingNodeA.SkirtingMeshID = ModelResID;
			SkirtingNodeA.MaterialID = MaterialResID;
			CreateSkirtingComponentByNode(SkirtingNodeA);


		}
		else
		{
			FSkirtingNode SkirtingNodeB;
			SkirtingNodeB.InitFromAreaPath(TPolygonB, InSkirtingType, false);
			SkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;

			SkirtingNodeB.SkirtingMeshModelID = ModelID;
			SkirtingNodeB.RoomClassID = RoomClassID;
			SkirtingNodeB.CraftId = CraftID;
			SkirtingNodeB.SkirtingMeshID = ModelResID;
			SkirtingNodeB.MaterialID = MaterialResID;
			CreateSkirtingComponentByNode(SkirtingNodeB);
		}
	}

}

void ASkirtingSystem::CreateLocalSingleRegionNode(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassID, int32 CraftID, int32 SkuID,
	int32 MaterialModelID, FString MaterialResID, FSkirtingNode& InSkirtingNode, ESkirtingType InSkirtingType)
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::CreateLocalSingleRegionNode"));
	//清除已存在路径
	auto DestroyComponents = [](TArray<USkirtingMeshComponent*> &Components)
	{
		for (auto &Comp : Components)
		{
			if (Comp != nullptr)
			{
				Comp->GetOwner()->Destroy();
			}
		
		}
		Components.Empty();
	};

	TArray<FVector2D> AreaPath = RoomIDMapRegionPs(RoomID);

	//判断类型
	switch (InSkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{
		TArray<USkirtingMeshComponent*> ToRemoveMeshCompts;
		TArray< USkirtingMeshComponent*> ToRemoveAreaCompts;
		for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
		{
			if (SkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveMeshCompts.Add(SkirtingMeshComponents[I]);
					}
					break;
				}

			}

		}
		for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
		{
			if (SkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveAreaCompts.Add(SkirtingAreaMeshComps[K]);
					}
					break;
				}
			}

		}
		DestroyComponents(ToRemoveMeshCompts);
		DestroyComponents(ToRemoveAreaCompts);
		for (int32 Index = SkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
		{
			if (SkirtingMeshComponents[Index] == nullptr)
			{
				SkirtingMeshComponents.RemoveAt(Index);
			}
		}
		for (int32 Index = SkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
		{
			if (SkirtingAreaMeshComps[Index] == nullptr)
			{
				SkirtingAreaMeshComps.RemoveAt(Index);
			}
		}

	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{

		TArray<USkirtingMeshComponent*> ToRemoveTopMeshCompts;
		TArray< USkirtingMeshComponent*> ToRemoveTopAreaCompts;
		for (int32 I = 0; I < TopSkirtingMeshComponents.Num(); I++)
		{
			if (TopSkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveTopMeshCompts.Add(TopSkirtingMeshComponents[I]);
					}
					break;
				}
			}

		}
		for (int32 K = 0; K < TopSkirtingAreaMeshComps.Num(); K++)
		{
			if (TopSkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveTopAreaCompts.Add(TopSkirtingAreaMeshComps[K]);
					}
					break;
				}
			}

		}
		DestroyComponents(ToRemoveTopMeshCompts);
		DestroyComponents(ToRemoveTopAreaCompts);
		for (int32 Index = TopSkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
		{
			if (TopSkirtingMeshComponents[Index] == nullptr)
			{
				TopSkirtingMeshComponents.RemoveAt(Index);
			}
		}
		for (int32 Index = TopSkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
		{
			if (TopSkirtingAreaMeshComps[Index] == nullptr)
			{
				TopSkirtingAreaMeshComps.RemoveAt(Index);
			}
		}

	}
	break;
	default:
		break;
	}
	InSkirtingNode.InitFromAreaPath(AreaPath, InSkirtingType);
	InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;

	InSkirtingNode.SkirtingMeshModelID = ModelID;
	InSkirtingNode.RoomClassID = RoomClassID;
	InSkirtingNode.CraftId = CraftID;
	InSkirtingNode.SkirtingMeshID = ModelResID;
	InSkirtingNode.SKUID = SkuID;
	InSkirtingNode.MaterialID = MaterialResID;
	InSkirtingNode.MaterialModelID = MaterialModelID;
	InSkirtingNode.HasComponent = false;
	int32 Seconds;
	float PartialSeconds;
	UGameplayStatics::GetAccurateRealTime(this, Seconds, PartialSeconds);
	FString uuid = FString::Printf(TEXT("%f"), Seconds + PartialSeconds);
	InSkirtingNode.Uuid = uuid;
	UE_LOG(LogTemp, Warning, TEXT("CreateLocalSingleRegionNode uuid = %s"), *InSkirtingNode.Uuid);
}

void ASkirtingSystem::CreateLocalSingleRegionNodeWithCom(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassID, int32 CraftID, int32 SkuID,
	int32 MaterialModelID, FString MaterialResID, FSkirtingNode& InSkirtingNode, FSkirtingNode& InSkirtingComNode, ESkirtingType InSkirtingType)
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::CreateLocalSingleRegionNode"));
	//清除已存在路径
	auto DestroyComponents = [](TArray<USkirtingMeshComponent*> &Components)
	{
		for (auto &Comp : Components)
		{
			if (Comp != nullptr)
			{
				Comp->GetOwner()->Destroy();
			}

		}
		Components.Empty();
	};

	TArray<FVector2D> AreaPath = RoomIDMapRegionPs(RoomID);

	//判断类型
	switch (InSkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{
		TArray<USkirtingMeshComponent*> ToRemoveMeshCompts;
		TArray< USkirtingMeshComponent*> ToRemoveAreaCompts;
		for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
		{
			if (SkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveMeshCompts.Add(SkirtingMeshComponents[I]);
						break;
					}

				}

			}

		}
		for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
		{
			if (SkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveAreaCompts.Add(SkirtingAreaMeshComps[K]);
						break;
					}

				}
			}

		}
		DestroyComponents(ToRemoveMeshCompts);
		DestroyComponents(ToRemoveAreaCompts);
		for (int32 Index = SkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
		{
			if (SkirtingMeshComponents[Index] == nullptr)
			{
				SkirtingMeshComponents.RemoveAt(Index);
			}
		}
		for (int32 Index = SkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
		{
			if (SkirtingAreaMeshComps[Index] == nullptr)
			{
				SkirtingAreaMeshComps.RemoveAt(Index);
			}
		}

	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{

		TArray<USkirtingMeshComponent*> ToRemoveTopMeshCompts;
		TArray< USkirtingMeshComponent*> ToRemoveTopAreaCompts;
		for (int32 I = 0; I < TopSkirtingMeshComponents.Num(); I++)
		{
			if (TopSkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveTopMeshCompts.Add(TopSkirtingMeshComponents[I]);
						break;
					}

				}
			}

		}
		for (int32 K = 0; K < TopSkirtingAreaMeshComps.Num(); K++)
		{
			if (TopSkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					if (InOrOnBoundary)
					{
						ToRemoveTopAreaCompts.Add(TopSkirtingAreaMeshComps[K]);
						break;
					}

				}
			}

		}
		DestroyComponents(ToRemoveTopMeshCompts);
		DestroyComponents(ToRemoveTopAreaCompts);
		for (int32 Index = TopSkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
		{
			if (TopSkirtingMeshComponents[Index] == nullptr)
			{
				TopSkirtingMeshComponents.RemoveAt(Index);
			}
		}
		for (int32 Index = TopSkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
		{
			if (TopSkirtingAreaMeshComps[Index] == nullptr)
			{
				TopSkirtingAreaMeshComps.RemoveAt(Index);
			}
		}

	}
	break;
	default:
		break;
	}


	TArray<AActor*> Components;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AComponentManagerActor::StaticClass(), Components);
	TArray<TArray<FVector2D>> TotalNodes;
	for (int I = 0; I < Components.Num(); ++I)
	{
		AComponentManagerActor * _TempCom = Cast<AComponentManagerActor>(Components[I]);
		if (_TempCom)
		{
			TArray<FVector2D> Nodes;
			bool GetSuccess = _TempCom->GetAllHouseComponent(Nodes);
			if (GetSuccess)
			{
				FVector2D TempCenter = 0.25*(Nodes[0] + Nodes[1] + Nodes[2] + Nodes[3]);
				bool IsInRegion = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempCenter);
				if (IsInRegion)
				{
					TotalNodes.Add(Nodes);
				}

			}
		}
	}
	TArray<FVector2D> TempAllComPonents, UpdateRegion, UpdateNoSnapComps;
	for (int _CurI = 0; _CurI < TotalNodes.Num(); ++_CurI)
	{
		TArray<FVector2D> TempOneCom = TotalNodes[_CurI];
		for (int _CurJ = 0; _CurJ < TempOneCom.Num(); ++_CurJ)
		{
			TempAllComPonents.Add(TempOneCom[_CurJ]);
		}
	}

	bool SnapSuccess = FPolygonAlg::RegionSnapMoreComps(AreaPath, TempAllComPonents, UpdateRegion, UpdateNoSnapComps);
	if (SnapSuccess)
	{
		InSkirtingNode.InitFromAreaPath(UpdateRegion, InSkirtingType);
		InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;

		InSkirtingNode.SkirtingMeshModelID = ModelID;
		InSkirtingNode.RoomClassID = RoomClassID;
		InSkirtingNode.CraftId = CraftID;
		InSkirtingNode.SkirtingMeshID = ModelResID;
		InSkirtingNode.SKUID = SkuID;
		InSkirtingNode.MaterialID = MaterialResID;
		InSkirtingNode.MaterialModelID = MaterialModelID;

		int NumOfComponents = UpdateNoSnapComps.Num() / 4;
		if (NumOfComponents > 0)
		{

			for (int _CurCom = 0; _CurCom < NumOfComponents; ++_CurCom)
			{
				TArray<FVector2D> TempOneCom;
				for (int _CurP = 0 + 4 * _CurCom; _CurP < 4 + 4 * _CurCom; ++_CurP)
				{
					TempOneCom.Add(UpdateNoSnapComps[_CurP]);
				}

				if (!IsPolyClockWise(TempOneCom))
				{
					ChangeClockwise(TempOneCom);
				}
				InSkirtingComNode.InitFromAreaPath(TempOneCom, InSkirtingType, true);
				InSkirtingComNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
				InSkirtingComNode.SkirtingMeshModelID = ModelID;
				InSkirtingComNode.RoomClassID = RoomClassID;
				InSkirtingComNode.CraftId = CraftID;
				InSkirtingComNode.SkirtingMeshID = ModelResID;
				InSkirtingComNode.SKUID = SkuID;
				InSkirtingComNode.MaterialID = MaterialResID;
				InSkirtingComNode.MaterialModelID = MaterialModelID;
				InSkirtingComNode.HasComponent = true;
			}

		}
	}

}

void ASkirtingSystem::RemoveRegionNodes(int32 RoomID, ESkirtingType InSkirtingType)
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::RemoveRegionNodes"));
	//清除已存在路径
	auto DestroyComponents = [](TArray<USkirtingMeshComponent*> &Components)
	{
		for (auto &Comp : Components)
		{
			if (Comp != nullptr)
			{
				Comp->GetOwner()->Destroy();
			}
		}
		Components.Empty();
	};

	TArray<FVector2D> AreaPath = RoomIDMapRegionPs(RoomID);

	//判断类型
	switch (InSkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{
		TArray<USkirtingMeshComponent*> ToRemoveMeshCompts;
		TArray< USkirtingMeshComponent*> ToRemoveAreaCompts;
		for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
		{
			if (SkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					bool EndInOrOnBoundary=FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingEndCorner.Position);
					if ((StartInOrOnBoundary)||(EndInOrOnBoundary))
					{
						ToRemoveMeshCompts.Add(SkirtingMeshComponents[I]);
					}
					break;
				}

			}

		}
		for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
		{
			if (SkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingEndCorner.Position);
					if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
					{
						ToRemoveAreaCompts.Add(SkirtingAreaMeshComps[K]);
					}
					break;
				}
			}

		}
		DestroyComponents(ToRemoveMeshCompts);
		DestroyComponents(ToRemoveAreaCompts);
		for (int32 Index = SkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
		{
			if (SkirtingMeshComponents[Index] == nullptr)
			{
				SkirtingMeshComponents.RemoveAt(Index);
			}
		}
		for (int32 Index = SkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
		{
			if (SkirtingAreaMeshComps[Index] == nullptr)
			{
				SkirtingAreaMeshComps.RemoveAt(Index);
			}
		}

	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{

		TArray<USkirtingMeshComponent*> ToRemoveTopMeshCompts;
		TArray< USkirtingMeshComponent*> ToRemoveTopAreaCompts;
		for (int32 I = 0; I < TopSkirtingMeshComponents.Num(); I++)
		{
			if (TopSkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingEndCorner.Position);
					if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
					{
						ToRemoveTopMeshCompts.Add(TopSkirtingMeshComponents[I]);
					}
					break;
				}
			}

		}
		for (int32 K = 0; K < TopSkirtingAreaMeshComps.Num(); K++)
		{
			if (TopSkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingEndCorner.Position);
					if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
					{
						ToRemoveTopAreaCompts.Add(TopSkirtingAreaMeshComps[K]);
					}
					break;
				}
			}

		}
		DestroyComponents(ToRemoveTopMeshCompts);
		DestroyComponents(ToRemoveTopAreaCompts);

		for (int32 Index = TopSkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
		{
			if (TopSkirtingMeshComponents[Index] == nullptr)
			{
				TopSkirtingMeshComponents.RemoveAt(Index);
			}
		}
		for (int32 Index = TopSkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
		{
			if (TopSkirtingAreaMeshComps[Index] == nullptr)
			{
				TopSkirtingAreaMeshComps.RemoveAt(Index);
			}
		}

	}
	break;
	default:
		break;
	}
}

USkirtingMeshComponent* ASkirtingSystem::LocalCreateLinesByModelFile(const FSkirtingNode &InSkirtingNode, FString ModelResID, FString MaterialResID, UStaticMesh *InSkirtingMesh)
{
	//UE_LOG(LogTemp, Warning, TEXT("USkirtingMeshComponent* ASkirtingSystem::LocalCreateLinesByModelFile"));
	UModelFile *InSkirtingModel = nullptr;
	InSkirtingModel = ForceLoadMXByID(ModelResID);
	if (!InSkirtingModel)
	{
		InSkirtingModel = GetDefSkirtingModelByType(InSkirtingNode.SkirtingType);
	}
	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASkirtingActor *SkirtingActor = GetWorld()->SpawnActor<ASkirtingActor>(SpawmParams);
	SkirtingActor->Tags.Add(FName(TEXT("SkirtingLineActor")));
	USkirtingMeshComponent *SkirtingComponent = NewObject<USkirtingMeshComponent>(SkirtingActor);
	if (SkirtingComponent != nullptr)
	{
		SkirtingComponent->RegisterComponent();
		SkirtingComponent->ResetMeshType(InSkirtingModel);
		SkirtingComponent->ResetStaticMeshType(InSkirtingMesh);
		SkirtingComponent->GetSkirtingSaveNode() = InSkirtingNode;
		SkirtingComponent->AttachToComponent(SkirtingActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		SkirtingComponent->ChangeMatByID(MaterialResID);
		SkirtingComponent->UpdateMeshByNode();
	}
	

	switch (InSkirtingNode.SkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{
		switch (InSkirtingNode.SrcType)
		{
		case ESkirtingSrcType::SrcFormDrawing:
		{
			SkirtingMeshComponents.Add(SkirtingComponent);
		}
		break;
		case ESkirtingSrcType::SrcFromSyncPath:
		{
			SkirtingAreaMeshComps.Add(SkirtingComponent);
		}
		break;
		default:
			break;
		}
	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{
		switch (InSkirtingNode.SrcType)
		{
		case ESkirtingSrcType::SrcFormDrawing:
		{
			TopSkirtingMeshComponents.Add(SkirtingComponent);
		}
		break;
		case ESkirtingSrcType::SrcFromSyncPath:
		{
			TopSkirtingAreaMeshComps.Add(SkirtingComponent);
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}

	return SkirtingComponent;
}

bool ASkirtingSystem::UpdateCurrentDecorateLines(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassId, int32 CraftId, int32 SkuID, int32 MaterialModelID, FString MaterialResID, ESkirtingType LineType)
{
	//UE_LOG(LogTemp, Warning, TEXT("bool ASkirtingSystem::UpdateCurrentDecorateLines"));
	if (ModelResID.IsEmpty())
	{
		return false;
	}
	int32 LivingRoomID = 0, DiningRoomID = 0;
	TArray<FVector2D> LivingRoomPs, DiningRoomPs;
	GetLivingRoomRegion(LivingRoomID, LivingRoomPs);
	GetDiningRoomRegion(DiningRoomID, DiningRoomPs);

	if ((LivingRoomID != 0) && (DiningRoomID != 0))
	{
		if ((RoomID != LivingRoomID) && (RoomID != DiningRoomID))
		{
			FSkirtingNode InSkirtingNode;
			CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
				MaterialModelID, MaterialResID, InSkirtingNode, LineType);
			LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
			return true;
		}
		else if ((RoomID == LivingRoomID) || (RoomID == DiningRoomID))
		{
			TArray<FVector2D> AreaPath = RoomIDMapRegionPs(RoomID);
			TArray<FVector2D> PolygonA, PolygonB;
			if (LivingRoomPs.Num() < DiningRoomPs.Num())
			{
				PolygonA = LivingRoomPs;
				PolygonB = DiningRoomPs;
			}
			else if (LivingRoomPs.Num() > DiningRoomPs.Num())
			{
				PolygonA = DiningRoomPs;
				PolygonB = LivingRoomPs;
			}
			else
			{
				float L1 = 0.0, L2 = 0.0;
				int32 NextI = 0;
				for (int32 I = 0; I < LivingRoomPs.Num(); I++)
				{
					if (I == LivingRoomPs.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					L1 = L1 + FVector2D::Distance(LivingRoomPs[I], LivingRoomPs[NextI]);
				}
				int32 NextJ = 0;
				for (int32 J = 0; J < DiningRoomPs.Num(); J++)
				{
					if (J == DiningRoomPs.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					L2 = L2 + FVector2D::Distance(DiningRoomPs[J], DiningRoomPs[NextJ]);
				}

				if (L1 < L2)
				{
					PolygonA = LivingRoomPs;
					PolygonB = DiningRoomPs;
				}
				else
				{
					PolygonA = DiningRoomPs;
					PolygonB = LivingRoomPs;
				}
			}

			TArray<FVector2D> TPolygonA;
			TArray<FVector2D> TPolygonB;
			TArray<FVector2D> Polygons;
			TArray<FVector2D>InterSectPs;
			//将顶点改为顺时针
			if (!IsPolyClockWise(PolygonA))
			{
				ChangeClockwise(PolygonA);
			}
			if (!IsPolyClockWise(PolygonB))
			{
				ChangeClockwise(PolygonB);
			}
			bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
			bool HasCommonPoints= FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
			if ((CGInterSect)&&(!HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
						MaterialModelID, MaterialResID, InSkirtingNode, LineType);
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}
			else if((HasCommonPoints)&&(!CGInterSect))
			{
		
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
			}
			else if ((CGInterSect) && (HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
						MaterialModelID, MaterialResID, InSkirtingNode, LineType);
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}
			else if((!CGInterSect)&&(!HasCommonPoints))
			{
			   TPolygonA.Empty();
			   TPolygonB.Empty();
			   Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
						MaterialModelID, MaterialResID, InSkirtingNode, LineType);
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}
		}
	}
	else if ((LivingRoomID == 0) && (DiningRoomID != 0) && (RoomID == DiningRoomID))
	{
		FSkirtingNode InSkirtingNode;
		CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
			MaterialModelID, MaterialResID, InSkirtingNode, LineType);
		LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
	}
	else if ((LivingRoomID != 0) && (DiningRoomID == 0) && (RoomID == LivingRoomID))
	{
		FSkirtingNode InSkirtingNode;
		CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
			MaterialModelID, MaterialResID, InSkirtingNode, LineType);
		LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
	}
	return true;
}

bool ASkirtingSystem::UpdateCurrentDecorateLinesWithComponents(int32 RoomID, int32 ModelID, FString ModelResID, int32 RoomClassId, int32 CraftId, int32 SkuID, int32 MaterialModelID, FString MaterialResID, ESkirtingType LineType)
{
	if (ModelResID.IsEmpty())
	{
		return false;
	}

	int32 LivingRoomID = 0, DiningRoomID = 0;
	TArray<FVector2D> LivingRoomPs, DiningRoomPs;
	GetLivingRoomRegion(LivingRoomID, LivingRoomPs);
	GetDiningRoomRegion(DiningRoomID, DiningRoomPs);

	/////////////////组件////////////////////////
	TArray<AActor*> Components;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AComponentManagerActor::StaticClass(), Components);
	TArray<TArray<FVector2D>> TotalNodes;
	for (int I = 0; I < Components.Num(); ++I)
	{
		AComponentManagerActor * _TempCom = Cast<AComponentManagerActor>(Components[I]);
		if (_TempCom)
		{
			TArray<FVector2D> Nodes;
			bool GetSuccess = _TempCom->GetAllHouseComponent(Nodes);
			if (GetSuccess)
			{
				TotalNodes.Add(Nodes);
			}
		}
	}

	TArray<FVector2D> AreaPath = RoomIDMapRegionPs(RoomID);
	TArray<TArray<FVector2D>> TempArrArr;

	for (int J = 0; J < TotalNodes.Num(); ++J)
	{
		TArray<FVector2D> TempArr = TotalNodes[J];
		int NumOfArr = TempArr.Num() / 4;
		for (int K = 0; K < NumOfArr; ++K)
		{
			FVector2D TempCenter = 0.25*(TempArr[4 * K] + TempArr[4 * K + 1] + TempArr[4 * K + 2] + TempArr[4 * K + 3]);
			if (FPolygonAlg::JudgePointInPolygon(AreaPath, TempCenter))
			{
				TempArrArr.Add(TempArr);
			}
		}
	}


	TArray<TArray<FVector2D>> LivingArrArr, DiningArrArr;
	for (int J = 0; J < TotalNodes.Num(); ++J)
	{
		TArray<FVector2D> TempArr = TotalNodes[J];
		int NumOfArr = TempArr.Num() / 4;
		for (int K = 0; K < NumOfArr; ++K)
		{
			FVector2D TempCenter = 0.25*(TempArr[4 * K] + TempArr[4 * K + 1] + TempArr[4 * K + 2] + TempArr[4 * K + 3]);
			if (FPolygonAlg::JudgePointInPolygon(LivingRoomPs, TempCenter))
			{
				LivingArrArr.Add(TempArr);
			}
			else if (FPolygonAlg::JudgePointInPolygon(DiningRoomPs, TempCenter))
			{
				DiningArrArr.Add(TempArr);
			}
		}
	}


	TArray<FVector2D> CombinedArr;
	for (int _CurI = 0; _CurI < TempArrArr.Num(); ++_CurI)
	{
		TArray<FVector2D> TempG = TempArrArr[_CurI];
		for (int _CurJ = 0; _CurJ < TempG.Num(); ++_CurJ)
		{
			CombinedArr.Add(TempG[_CurJ]);
		}
	}

	TArray<FVector2D> CombinedArrOfLiving, CombinedArrOfDining;
	for (int _CurI = 0; _CurI < LivingArrArr.Num(); ++_CurI)
	{
		TArray<FVector2D> TempG = LivingArrArr[_CurI];
		for (int _CurJ = 0; _CurJ < TempG.Num(); ++_CurJ)
		{
			CombinedArrOfLiving.Add(TempG[_CurJ]);
		}
	}
	for (int _CurI = 0; _CurI < DiningArrArr.Num(); ++_CurI)
	{
		TArray<FVector2D> TempG = DiningArrArr[_CurI];
		for (int _CurJ = 0; _CurJ < TempG.Num(); ++_CurJ)
		{
			CombinedArrOfDining.Add(TempG[_CurJ]);
		}
	}

	/////////////////////////////////////////////////////


	TArray<FVector2D> TotalNoSnapComPs;


	if (TempArrArr.Num() == 0)
	{
		if ((LivingRoomID != RoomID) && (DiningRoomID != RoomID))
		{
			FSkirtingNode InSkirtingNode;
			CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
				MaterialModelID, MaterialResID, InSkirtingNode, LineType);
			LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
		}
		else if ((LivingRoomID == 0) && (DiningRoomID != 0) && (RoomID == DiningRoomID))
		{
			FSkirtingNode InSkirtingNode;
			CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
				MaterialModelID, MaterialResID, InSkirtingNode, LineType);
			LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
		}
		else if ((LivingRoomID != 0) && (DiningRoomID == 0) && (RoomID == LivingRoomID))
		{
			FSkirtingNode InSkirtingNode;
			CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
				MaterialModelID, MaterialResID, InSkirtingNode, LineType);
			LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
		}
		else if ((LivingRoomID == RoomID) && (DiningRoomID != 0))
		{
			TArray<FVector2D> PolygonA, PolygonB;
			if (LivingRoomPs.Num() < DiningRoomPs.Num())
			{
				PolygonA = LivingRoomPs;
				PolygonB = DiningRoomPs;
			}
			else if (LivingRoomPs.Num() > DiningRoomPs.Num())
			{
				PolygonA = DiningRoomPs;
				PolygonB = LivingRoomPs;
			}
			else
			{
				float L1 = 0.0, L2 = 0.0;
				int32 NextI = 0;
				for (int32 I = 0; I < LivingRoomPs.Num(); I++)
				{
					if (I == LivingRoomPs.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					L1 = L1 + FVector2D::Distance(LivingRoomPs[I], LivingRoomPs[NextI]);
				}
				int32 NextJ = 0;
				for (int32 J = 0; J < DiningRoomPs.Num(); J++)
				{
					if (J == DiningRoomPs.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					L2 = L2 + FVector2D::Distance(DiningRoomPs[J], DiningRoomPs[NextJ]);
				}

				if (L1 < L2)
				{
					PolygonA = LivingRoomPs;
					PolygonB = DiningRoomPs;
				}
				else
				{
					PolygonA = DiningRoomPs;
					PolygonB = LivingRoomPs;
				}
			}

			TArray<FVector2D> TPolygonA;
			TArray<FVector2D> TPolygonB;
			TArray<FVector2D> Polygons;
			TArray<FVector2D>InterSectPs;
			//将顶点改为顺时针
			if (IsPolyClockWise(PolygonA))
			{
				ChangeClockwise(PolygonA);
			}
			if (IsPolyClockWise(PolygonB))
			{
				ChangeClockwise(PolygonB);
			}
			bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
			bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
			if ((CGInterSect) && (!HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					RemoveRegionNodes(LivingRoomID, LineType);
					InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
					InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNode.SkirtingMeshModelID = ModelID;
					InSkirtingNode.RoomClassID = RoomClassId;
					InSkirtingNode.CraftId = CraftId;
					InSkirtingNode.SkirtingMeshID = ModelResID;
					InSkirtingNode.SKUID = SkuID;
					InSkirtingNode.MaterialID = MaterialResID;
					InSkirtingNode.MaterialModelID = MaterialModelID;
					InSkirtingNode.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}
			else if ((HasCommonPoints) && (!CGInterSect))
			{

				RemoveRegionNodes(LivingRoomID, LineType);
				RemoveRegionNodes(DiningRoomID, LineType);
				FSkirtingNode InSkirtingNodeA;
				InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
				InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
				InSkirtingNodeA.SkirtingMeshModelID = ModelID;
				InSkirtingNodeA.RoomClassID = RoomClassId;
				InSkirtingNodeA.CraftId = CraftId;
				InSkirtingNodeA.SkirtingMeshID = ModelResID;
				InSkirtingNodeA.SKUID = SkuID;
				InSkirtingNodeA.MaterialID = MaterialResID;
				InSkirtingNodeA.MaterialModelID = MaterialModelID;
				InSkirtingNodeA.HasComponent = false;
				LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

				FSkirtingNode InSkirtingNodeB;
				InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
				InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
				InSkirtingNodeB.SkirtingMeshModelID = ModelID;
				InSkirtingNodeB.RoomClassID = RoomClassId;
				InSkirtingNodeB.CraftId = CraftId;
				InSkirtingNodeB.SkirtingMeshID = ModelResID;
				InSkirtingNodeB.SKUID = SkuID;
				InSkirtingNodeB.MaterialID = MaterialResID;
				InSkirtingNodeB.MaterialModelID = MaterialModelID;
				InSkirtingNodeB.HasComponent = false;
				LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
			}
			else if ((CGInterSect) && (HasCommonPoints))
			{
				if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else
				{
					FSkirtingNode InSkirtingNode;
					RemoveRegionNodes(LivingRoomID, LineType);
					InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
					InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNode.SkirtingMeshModelID = ModelID;
					InSkirtingNode.RoomClassID = RoomClassId;
					InSkirtingNode.CraftId = CraftId;
					InSkirtingNode.SkirtingMeshID = ModelResID;
					InSkirtingNode.SKUID = SkuID;
					InSkirtingNode.MaterialID = MaterialResID;
					InSkirtingNode.MaterialModelID = MaterialModelID;
					InSkirtingNode.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
			}
			else if ((!CGInterSect) && (!HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					RemoveRegionNodes(LivingRoomID, LineType);
					InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
					InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNode.SkirtingMeshModelID = ModelID;
					InSkirtingNode.RoomClassID = RoomClassId;
					InSkirtingNode.CraftId = CraftId;
					InSkirtingNode.SkirtingMeshID = ModelResID;
					InSkirtingNode.SKUID = SkuID;
					InSkirtingNode.MaterialID = MaterialResID;
					InSkirtingNode.MaterialModelID = MaterialModelID;
					InSkirtingNode.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}
		}
		else if ((DiningRoomID == RoomID) && (LivingRoomID != 0))
		{
			TArray<FVector2D> PolygonA, PolygonB;
			if (LivingRoomPs.Num() < DiningRoomPs.Num())
			{
				PolygonA = LivingRoomPs;
				PolygonB = DiningRoomPs;
			}
			else if (LivingRoomPs.Num() > DiningRoomPs.Num())
			{
				PolygonA = DiningRoomPs;
				PolygonB = LivingRoomPs;
			}
			else
			{
				float L1 = 0.0, L2 = 0.0;
				int32 NextI = 0;
				for (int32 I = 0; I < LivingRoomPs.Num(); I++)
				{
					if (I == LivingRoomPs.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					L1 = L1 + FVector2D::Distance(LivingRoomPs[I], LivingRoomPs[NextI]);
				}
				int32 NextJ = 0;
				for (int32 J = 0; J < DiningRoomPs.Num(); J++)
				{
					if (J == DiningRoomPs.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					L2 = L2 + FVector2D::Distance(DiningRoomPs[J], DiningRoomPs[NextJ]);
				}

				if (L1 < L2)
				{
					PolygonA = LivingRoomPs;
					PolygonB = DiningRoomPs;
				}
				else
				{
					PolygonA = DiningRoomPs;
					PolygonB = LivingRoomPs;
				}
			}

			TArray<FVector2D> TPolygonA;
			TArray<FVector2D> TPolygonB;
			TArray<FVector2D> Polygons;
			TArray<FVector2D>InterSectPs;
			//纠正方向
			if (IsPolyClockWise(PolygonA))
			{
				ChangeClockwise(PolygonA);
			}
			if (IsPolyClockWise(PolygonB))
			{
				ChangeClockwise(PolygonB);
			}
			bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
			bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
			if ((CGInterSect) && (!HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					RemoveRegionNodes(DiningRoomID, LineType);
					InSkirtingNode.InitFromAreaPath(DiningRoomPs, LineType);
					InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNode.SkirtingMeshModelID = ModelID;
					InSkirtingNode.RoomClassID = RoomClassId;
					InSkirtingNode.CraftId = CraftId;
					InSkirtingNode.SkirtingMeshID = ModelResID;
					InSkirtingNode.SKUID = SkuID;
					InSkirtingNode.MaterialID = MaterialResID;
					InSkirtingNode.MaterialModelID = MaterialModelID;
					InSkirtingNode.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}
			else if ((HasCommonPoints) && (!CGInterSect))
			{

				RemoveRegionNodes(LivingRoomID, LineType);
				RemoveRegionNodes(DiningRoomID, LineType);
				FSkirtingNode InSkirtingNodeA;
				InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
				InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
				InSkirtingNodeA.SkirtingMeshModelID = ModelID;
				InSkirtingNodeA.RoomClassID = RoomClassId;
				InSkirtingNodeA.CraftId = CraftId;
				InSkirtingNodeA.SkirtingMeshID = ModelResID;
				InSkirtingNodeA.SKUID = SkuID;
				InSkirtingNodeA.MaterialID = MaterialResID;
				InSkirtingNodeA.MaterialModelID = MaterialModelID;
				InSkirtingNodeA.HasComponent = false;
				LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

				FSkirtingNode InSkirtingNodeB;
				InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
				InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
				InSkirtingNodeB.SkirtingMeshModelID = ModelID;
				InSkirtingNodeB.RoomClassID = RoomClassId;
				InSkirtingNodeB.CraftId = CraftId;
				InSkirtingNodeB.SkirtingMeshID = ModelResID;
				InSkirtingNodeB.SKUID = SkuID;
				InSkirtingNodeB.MaterialID = MaterialResID;
				InSkirtingNodeB.MaterialModelID = MaterialModelID;
				InSkirtingNodeB.HasComponent = false;
				LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
			}
			else if ((CGInterSect) && (HasCommonPoints))
			{
				if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else
				{
					FSkirtingNode InSkirtingNode;
					RemoveRegionNodes(LivingRoomID, LineType);
					InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
					InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNode.SkirtingMeshModelID = ModelID;
					InSkirtingNode.RoomClassID = RoomClassId;
					InSkirtingNode.CraftId = CraftId;
					InSkirtingNode.SkirtingMeshID = ModelResID;
					InSkirtingNode.SKUID = SkuID;
					InSkirtingNode.MaterialID = MaterialResID;
					InSkirtingNode.MaterialModelID = MaterialModelID;
					InSkirtingNode.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
			}
			else if ((!CGInterSect) && (!HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					RemoveRegionNodes(DiningRoomID, LineType);
					InSkirtingNode.InitFromAreaPath(DiningRoomPs, LineType);
					InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNode.SkirtingMeshModelID = ModelID;
					InSkirtingNode.RoomClassID = RoomClassId;
					InSkirtingNode.CraftId = CraftId;
					InSkirtingNode.SkirtingMeshID = ModelResID;
					InSkirtingNode.SKUID = SkuID;
					InSkirtingNode.MaterialID = MaterialResID;
					InSkirtingNode.MaterialModelID = MaterialModelID;
					InSkirtingNode.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}
		}
	}
	else if ((LivingRoomID == RoomID) && (DiningRoomID != 0))
	{
		/////客厅区域内含有组件，餐厅区域也含有组件时
		if ((CombinedArrOfLiving.Num() != 0) && (CombinedArrOfDining.Num() != 0))
		{
			TArray<FVector2D> NewLivingRegion, NoSnapPsOfLiving, NewDiningRegion, NoSnapPsOfDining;
			bool SnapSuccess1 = FPolygonAlg::RegionSnapMoreComps(LivingRoomPs, CombinedArrOfLiving, NewLivingRegion, NoSnapPsOfLiving);
			bool SnapSuccess2 = FPolygonAlg::RegionSnapMoreComps(DiningRoomPs, CombinedArrOfDining, NewDiningRegion, NoSnapPsOfDining);
			if ((SnapSuccess1) && (SnapSuccess2))
			{
				TArray<FVector2D> PolygonA, PolygonB;
				if (NewLivingRegion.Num() < NewDiningRegion.Num())
				{
					PolygonA = NewLivingRegion;
					PolygonB = NewDiningRegion;
				}
				else if (LivingRoomPs.Num() > DiningRoomPs.Num())
				{
					PolygonA = NewDiningRegion;
					PolygonB = NewLivingRegion;
				}
				else
				{
					float L1 = 0.0, L2 = 0.0;
					int32 NextI = 0;
					for (int32 I = 0; I < NewLivingRegion.Num(); I++)
					{
						if (I == NewLivingRegion.Num() - 1)
						{
							NextI = 0;
						}
						else
						{
							NextI = I + 1;
						}
						L1 = L1 + FVector2D::Distance(NewLivingRegion[I], NewLivingRegion[NextI]);
					}
					int32 NextJ = 0;
					for (int32 J = 0; J < NewDiningRegion.Num(); J++)
					{
						if (J == NewDiningRegion.Num() - 1)
						{
							NextJ = 0;
						}
						else
						{
							NextJ = J + 1;
						}
						L2 = L2 + FVector2D::Distance(NewDiningRegion[J], NewDiningRegion[NextJ]);
					}

					if (L1 < L2)
					{
						PolygonA = NewLivingRegion;
						PolygonB = NewDiningRegion;
					}
					else
					{
						PolygonA = NewDiningRegion;
						PolygonB = NewLivingRegion;
					}
				}

				TArray<FVector2D> TPolygonA;
				TArray<FVector2D> TPolygonB;
				TArray<FVector2D> Polygons;
				TArray<FVector2D>InterSectPs;
				//纠正方向
				if (IsPolyClockWise(PolygonA))
				{
					ChangeClockwise(PolygonA);
				}
				if (IsPolyClockWise(PolygonB))
				{
					ChangeClockwise(PolygonB);
				}
				bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
				bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
				if ((CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(NewLivingRegion, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}
				else if ((HasCommonPoints) && (!CGInterSect))
				{

					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else if ((CGInterSect) && (HasCommonPoints))
				{
					if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
					else
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
				}
				else if ((!CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(NewLivingRegion, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}


			}
			for (int _CurP = 0; _CurP < NoSnapPsOfLiving.Num(); ++_CurP)
			{
				TotalNoSnapComPs.Add(NoSnapPsOfLiving[_CurP]);
			}
			for (int _CurP = 0; _CurP < NoSnapPsOfDining.Num(); ++_CurP)
			{
				TotalNoSnapComPs.Add(NoSnapPsOfDining[_CurP]);
			}
		}
		//////客厅区域内含有组件，餐厅区域不含有组件时
		else if ((CombinedArrOfLiving.Num() != 0) && (CombinedArrOfDining.Num() == 0))
		{
			TArray<FVector2D> NewLivingRegion, NoSnapPsOfLiving, NewDiningRegion, NoSnapPsOfDining;
			bool SnapSuccess1 = FPolygonAlg::RegionSnapMoreComps(LivingRoomPs, CombinedArrOfLiving, NewLivingRegion, NoSnapPsOfLiving);
			if (SnapSuccess1)
			{
				TArray<FVector2D> PolygonA, PolygonB;
				if (NewLivingRegion.Num() < DiningRoomPs.Num())
				{
					PolygonA = NewLivingRegion;
					PolygonB = DiningRoomPs;
				}
				else if (NewLivingRegion.Num() > DiningRoomPs.Num())
				{
					PolygonA = DiningRoomPs;
					PolygonB = NewLivingRegion;
				}
				else
				{
					float L1 = 0.0, L2 = 0.0;
					int32 NextI = 0;
					for (int32 I = 0; I < NewLivingRegion.Num(); I++)
					{
						if (I == NewLivingRegion.Num() - 1)
						{
							NextI = 0;
						}
						else
						{
							NextI = I + 1;
						}
						L1 = L1 + FVector2D::Distance(NewLivingRegion[I], NewLivingRegion[NextI]);
					}
					int32 NextJ = 0;
					for (int32 J = 0; J < DiningRoomPs.Num(); J++)
					{
						if (J == DiningRoomPs.Num() - 1)
						{
							NextJ = 0;
						}
						else
						{
							NextJ = J + 1;
						}
						L2 = L2 + FVector2D::Distance(DiningRoomPs[J], DiningRoomPs[NextJ]);
					}

					if (L1 < L2)
					{
						PolygonA = NewLivingRegion;
						PolygonB = DiningRoomPs;
					}
					else
					{
						PolygonA = DiningRoomPs;
						PolygonB = NewLivingRegion;
					}
				}

				TArray<FVector2D> TPolygonA;
				TArray<FVector2D> TPolygonB;
				TArray<FVector2D> Polygons;
				TArray<FVector2D>InterSectPs;
				//纠正方向
				if (IsPolyClockWise(PolygonA))
				{
					ChangeClockwise(PolygonA);
				}
				if (IsPolyClockWise(PolygonB))
				{
					ChangeClockwise(PolygonB);
				}
				bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
				bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
				if ((CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(NewLivingRegion, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}
				else if ((HasCommonPoints) && (!CGInterSect))
				{

					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else if ((CGInterSect) && (HasCommonPoints))
				{
					if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
					else
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
				}
				else if ((!CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(NewLivingRegion, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}


			}
			for (int _CurP = 0; _CurP < NoSnapPsOfLiving.Num(); ++_CurP)
			{
				TotalNoSnapComPs.Add(NoSnapPsOfLiving[_CurP]);
			}
		}
		/////客厅区域不含有组件，餐厅区域含有组件时
		else if ((CombinedArrOfLiving.Num() == 0) && (CombinedArrOfDining.Num() != 0))
		{
			TArray<FVector2D> NewDiningRegion, NoSnapPsOfDining;
			bool SnapSuccess2 = FPolygonAlg::RegionSnapMoreComps(DiningRoomPs, CombinedArrOfDining, NewDiningRegion, NoSnapPsOfDining);
			if (SnapSuccess2)
			{
				TArray<FVector2D> PolygonA, PolygonB;
				if (LivingRoomPs.Num() < NewDiningRegion.Num())
				{
					PolygonA = LivingRoomPs;
					PolygonB = NewDiningRegion;
				}
				else if (LivingRoomPs.Num() > DiningRoomPs.Num())
				{
					PolygonA = NewDiningRegion;
					PolygonB = LivingRoomPs;
				}
				else
				{
					float L1 = 0.0, L2 = 0.0;
					int32 NextI = 0;
					for (int32 I = 0; I < LivingRoomPs.Num(); I++)
					{
						if (I == LivingRoomPs.Num() - 1)
						{
							NextI = 0;
						}
						else
						{
							NextI = I + 1;
						}
						L1 = L1 + FVector2D::Distance(LivingRoomPs[I], LivingRoomPs[NextI]);
					}
					int32 NextJ = 0;
					for (int32 J = 0; J < NewDiningRegion.Num(); J++)
					{
						if (J == NewDiningRegion.Num() - 1)
						{
							NextJ = 0;
						}
						else
						{
							NextJ = J + 1;
						}
						L2 = L2 + FVector2D::Distance(NewDiningRegion[J], NewDiningRegion[NextJ]);
					}

					if (L1 < L2)
					{
						PolygonA = LivingRoomPs;
						PolygonB = NewDiningRegion;
					}
					else
					{
						PolygonA = NewDiningRegion;
						PolygonB = LivingRoomPs;
					}
				}

				TArray<FVector2D> TPolygonA;
				TArray<FVector2D> TPolygonB;
				TArray<FVector2D> Polygons;
				TArray<FVector2D>InterSectPs;
				//纠正方向
				if (IsPolyClockWise(PolygonA))
				{
					ChangeClockwise(PolygonA);
				}
				if (IsPolyClockWise(PolygonB))
				{
					ChangeClockwise(PolygonB);
				}
				bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
				bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
				if ((CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}
				else if ((HasCommonPoints) && (!CGInterSect))
				{

					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else if ((CGInterSect) && (HasCommonPoints))
				{
					if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
					else
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
				}
				else if ((!CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}


			}

			for (int _CurP = 0; _CurP < NoSnapPsOfDining.Num(); ++_CurP)
			{
				TotalNoSnapComPs.Add(NoSnapPsOfDining[_CurP]);
			}


		}
		/////客厅区域不含有组件且餐厅区域也不含有组件时
		else if ((CombinedArrOfLiving.Num() == 0) && (CombinedArrOfDining.Num() == 0))
		{

			TArray<FVector2D> AreaPath = RoomIDMapRegionPs(RoomID);
			TArray<FVector2D> PolygonA, PolygonB;
			if (LivingRoomPs.Num() < DiningRoomPs.Num())
			{
				PolygonA = LivingRoomPs;
				PolygonB = DiningRoomPs;
			}
			else if (LivingRoomPs.Num() > DiningRoomPs.Num())
			{
				PolygonA = DiningRoomPs;
				PolygonB = LivingRoomPs;
			}
			else
			{
				float L1 = 0.0, L2 = 0.0;
				int32 NextI = 0;
				for (int32 I = 0; I < LivingRoomPs.Num(); I++)
				{
					if (I == LivingRoomPs.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					L1 = L1 + FVector2D::Distance(LivingRoomPs[I], LivingRoomPs[NextI]);
				}
				int32 NextJ = 0;
				for (int32 J = 0; J < DiningRoomPs.Num(); J++)
				{
					if (J == DiningRoomPs.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					L2 = L2 + FVector2D::Distance(DiningRoomPs[J], DiningRoomPs[NextJ]);
				}

				if (L1 < L2)
				{
					PolygonA = LivingRoomPs;
					PolygonB = DiningRoomPs;
				}
				else
				{
					PolygonA = DiningRoomPs;
					PolygonB = LivingRoomPs;
				}
			}

			TArray<FVector2D> TPolygonA;
			TArray<FVector2D> TPolygonB;
			TArray<FVector2D> Polygons;
			TArray<FVector2D>InterSectPs;
			//纠正方向
			if (IsPolyClockWise(PolygonA))
			{
				ChangeClockwise(PolygonA);
			}
			if (IsPolyClockWise(PolygonB))
			{
				ChangeClockwise(PolygonB);
			}
			bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
			bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
			if ((CGInterSect) && (!HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
						MaterialModelID, MaterialResID, InSkirtingNode, LineType);
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}
			else if ((HasCommonPoints) && (!CGInterSect))
			{

				RemoveRegionNodes(LivingRoomID, LineType);
				RemoveRegionNodes(DiningRoomID, LineType);
				FSkirtingNode InSkirtingNodeA;
				InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
				InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
				InSkirtingNodeA.SkirtingMeshModelID = ModelID;
				InSkirtingNodeA.RoomClassID = RoomClassId;
				InSkirtingNodeA.CraftId = CraftId;
				InSkirtingNodeA.SkirtingMeshID = ModelResID;
				InSkirtingNodeA.SKUID = SkuID;
				InSkirtingNodeA.MaterialID = MaterialResID;
				InSkirtingNodeA.MaterialModelID = MaterialModelID;
				LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

				FSkirtingNode InSkirtingNodeB;
				InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
				InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
				InSkirtingNodeB.SkirtingMeshModelID = ModelID;
				InSkirtingNodeB.RoomClassID = RoomClassId;
				InSkirtingNodeB.CraftId = CraftId;
				InSkirtingNodeB.SkirtingMeshID = ModelResID;
				InSkirtingNodeB.SKUID = SkuID;
				InSkirtingNodeB.MaterialID = MaterialResID;
				InSkirtingNodeB.MaterialModelID = MaterialModelID;
				LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
			}
			else if ((CGInterSect) && (HasCommonPoints))
			{
				if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else
				{
					FSkirtingNode InSkirtingNode;
					RemoveRegionNodes(LivingRoomID, LineType);
					InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
					InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNode.SkirtingMeshModelID = ModelID;
					InSkirtingNode.RoomClassID = RoomClassId;
					InSkirtingNode.CraftId = CraftId;
					InSkirtingNode.SkirtingMeshID = ModelResID;
					InSkirtingNode.SKUID = SkuID;
					InSkirtingNode.MaterialID = MaterialResID;
					InSkirtingNode.MaterialModelID = MaterialModelID;
					InSkirtingNode.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
			}
			else if ((!CGInterSect) && (!HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
						MaterialModelID, MaterialResID, InSkirtingNode, LineType);
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}

		}
	}
	else if ((DiningRoomID == RoomID) && (LivingRoomID != 0))
	{
		/////客厅区域内含有组件，餐厅区域也含有组件时
		if ((CombinedArrOfLiving.Num() != 0) && (CombinedArrOfDining.Num() != 0))
		{
			TArray<FVector2D> NewLivingRegion, NoSnapPsOfLiving, NewDiningRegion, NoSnapPsOfDining;
			bool SnapSuccess1 = FPolygonAlg::RegionSnapMoreComps(LivingRoomPs, CombinedArrOfLiving, NewLivingRegion, NoSnapPsOfLiving);
			bool SnapSuccess2 = FPolygonAlg::RegionSnapMoreComps(DiningRoomPs, CombinedArrOfDining, NewDiningRegion, NoSnapPsOfDining);
			if ((SnapSuccess1) && (SnapSuccess2))
			{
				TArray<FVector2D> PolygonA, PolygonB;
				if (NewLivingRegion.Num() < NewDiningRegion.Num())
				{
					PolygonA = NewLivingRegion;
					PolygonB = NewDiningRegion;
				}
				else if (LivingRoomPs.Num() > DiningRoomPs.Num())
				{
					PolygonA = NewDiningRegion;
					PolygonB = NewLivingRegion;
				}
				else
				{
					float L1 = 0.0, L2 = 0.0;
					int32 NextI = 0;
					for (int32 I = 0; I < NewLivingRegion.Num(); I++)
					{
						if (I == NewLivingRegion.Num() - 1)
						{
							NextI = 0;
						}
						else
						{
							NextI = I + 1;
						}
						L1 = L1 + FVector2D::Distance(NewLivingRegion[I], NewLivingRegion[NextI]);
					}
					int32 NextJ = 0;
					for (int32 J = 0; J < NewDiningRegion.Num(); J++)
					{
						if (J == NewDiningRegion.Num() - 1)
						{
							NextJ = 0;
						}
						else
						{
							NextJ = J + 1;
						}
						L2 = L2 + FVector2D::Distance(NewDiningRegion[J], NewDiningRegion[NextJ]);
					}

					if (L1 < L2)
					{
						PolygonA = NewLivingRegion;
						PolygonB = NewDiningRegion;
					}
					else
					{
						PolygonA = NewDiningRegion;
						PolygonB = NewLivingRegion;
					}
				}

				TArray<FVector2D> TPolygonA;
				TArray<FVector2D> TPolygonB;
				TArray<FVector2D> Polygons;
				TArray<FVector2D>InterSectPs;
				//纠正方向
				if (IsPolyClockWise(PolygonA))
				{
					ChangeClockwise(PolygonA);
				}
				if (IsPolyClockWise(PolygonB))
				{
					ChangeClockwise(PolygonB);
				}
				bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
				bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
				if ((CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(NewDiningRegion, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}
				else if ((HasCommonPoints) && (!CGInterSect))
				{

					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else if ((CGInterSect) && (HasCommonPoints))
				{
					if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
					else
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
				}
				else if ((!CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(NewDiningRegion, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}


			}
			for (int _CurP = 0; _CurP < NoSnapPsOfLiving.Num(); ++_CurP)
			{
				TotalNoSnapComPs.Add(NoSnapPsOfLiving[_CurP]);
			}
			for (int _CurP = 0; _CurP < NoSnapPsOfDining.Num(); ++_CurP)
			{
				TotalNoSnapComPs.Add(NoSnapPsOfDining[_CurP]);
			}
		}
		//////客厅区域内含有组件，餐厅区域不含有组件时
		else if ((CombinedArrOfLiving.Num() != 0) && (CombinedArrOfDining.Num() == 0))
		{
			TArray<FVector2D> NewLivingRegion, NoSnapPsOfLiving, NewDiningRegion, NoSnapPsOfDining;
			bool SnapSuccess1 = FPolygonAlg::RegionSnapMoreComps(LivingRoomPs, CombinedArrOfLiving, NewLivingRegion, NoSnapPsOfLiving);
			if (SnapSuccess1)
			{
				TArray<FVector2D> PolygonA, PolygonB;
				if (NewLivingRegion.Num() < DiningRoomPs.Num())
				{
					PolygonA = NewLivingRegion;
					PolygonB = DiningRoomPs;
				}
				else if (NewLivingRegion.Num() > DiningRoomPs.Num())
				{
					PolygonA = DiningRoomPs;
					PolygonB = NewLivingRegion;
				}
				else
				{
					float L1 = 0.0, L2 = 0.0;
					int32 NextI = 0;
					for (int32 I = 0; I < NewLivingRegion.Num(); I++)
					{
						if (I == NewLivingRegion.Num() - 1)
						{
							NextI = 0;
						}
						else
						{
							NextI = I + 1;
						}
						L1 = L1 + FVector2D::Distance(NewLivingRegion[I], NewLivingRegion[NextI]);
					}
					int32 NextJ = 0;
					for (int32 J = 0; J < DiningRoomPs.Num(); J++)
					{
						if (J == DiningRoomPs.Num() - 1)
						{
							NextJ = 0;
						}
						else
						{
							NextJ = J + 1;
						}
						L2 = L2 + FVector2D::Distance(DiningRoomPs[J], DiningRoomPs[NextJ]);
					}

					if (L1 < L2)
					{
						PolygonA = NewLivingRegion;
						PolygonB = DiningRoomPs;
					}
					else
					{
						PolygonA = DiningRoomPs;
						PolygonB = NewLivingRegion;
					}
				}

				TArray<FVector2D> TPolygonA;
				TArray<FVector2D> TPolygonB;
				TArray<FVector2D> Polygons;
				TArray<FVector2D>InterSectPs;
				//纠正方向
				if (IsPolyClockWise(PolygonA))
				{
					ChangeClockwise(PolygonA);
				}
				if (IsPolyClockWise(PolygonB))
				{
					ChangeClockwise(PolygonB);
				}
				bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
				bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
				if ((CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(DiningRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}
				else if ((HasCommonPoints) && (!CGInterSect))
				{

					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else if ((CGInterSect) && (HasCommonPoints))
				{
					if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
					else
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
				}
				else if ((!CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(DiningRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}


			}
			for (int _CurP = 0; _CurP < NoSnapPsOfLiving.Num(); ++_CurP)
			{
				TotalNoSnapComPs.Add(NoSnapPsOfLiving[_CurP]);
			}
		}
		/////客厅区域不含有组件，餐厅区域含有组件时
		else if ((CombinedArrOfLiving.Num() == 0) && (CombinedArrOfDining.Num() != 0))
		{
			TArray<FVector2D> NewDiningRegion, NoSnapPsOfDining;
			bool SnapSuccess2 = FPolygonAlg::RegionSnapMoreComps(DiningRoomPs, CombinedArrOfDining, NewDiningRegion, NoSnapPsOfDining);
			if (SnapSuccess2)
			{
				TArray<FVector2D> PolygonA, PolygonB;
				if (LivingRoomPs.Num() < NewDiningRegion.Num())
				{
					PolygonA = LivingRoomPs;
					PolygonB = NewDiningRegion;
				}
				else if (LivingRoomPs.Num() > DiningRoomPs.Num())
				{
					PolygonA = NewDiningRegion;
					PolygonB = LivingRoomPs;
				}
				else
				{
					float L1 = 0.0, L2 = 0.0;
					int32 NextI = 0;
					for (int32 I = 0; I < LivingRoomPs.Num(); I++)
					{
						if (I == LivingRoomPs.Num() - 1)
						{
							NextI = 0;
						}
						else
						{
							NextI = I + 1;
						}
						L1 = L1 + FVector2D::Distance(LivingRoomPs[I], LivingRoomPs[NextI]);
					}
					int32 NextJ = 0;
					for (int32 J = 0; J < NewDiningRegion.Num(); J++)
					{
						if (J == NewDiningRegion.Num() - 1)
						{
							NextJ = 0;
						}
						else
						{
							NextJ = J + 1;
						}
						L2 = L2 + FVector2D::Distance(NewDiningRegion[J], NewDiningRegion[NextJ]);
					}

					if (L1 < L2)
					{
						PolygonA = LivingRoomPs;
						PolygonB = NewDiningRegion;
					}
					else
					{
						PolygonA = NewDiningRegion;
						PolygonB = LivingRoomPs;
					}
				}

				TArray<FVector2D> TPolygonA;
				TArray<FVector2D> TPolygonB;
				TArray<FVector2D> Polygons;
				TArray<FVector2D>InterSectPs;
				//纠正方向
				if (IsPolyClockWise(PolygonA))
				{
					ChangeClockwise(PolygonA);
				}
				if (IsPolyClockWise(PolygonB))
				{
					ChangeClockwise(PolygonB);
				}
				bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
				bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
				if ((CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(NewDiningRegion, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}
				else if ((HasCommonPoints) && (!CGInterSect))
				{

					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else if ((CGInterSect) && (HasCommonPoints))
				{
					if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
					else
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
				}
				else if ((!CGInterSect) && (!HasCommonPoints))
				{
					TPolygonA.Empty();
					TPolygonB.Empty();
					Polygons.Empty();
					bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
						PolygonB,
						TPolygonA,
						TPolygonB,
						Polygons);

					if (!InterSectSuccess)
					{
						FSkirtingNode InSkirtingNode;
						RemoveRegionNodes(LivingRoomID, LineType);
						InSkirtingNode.InitFromAreaPath(NewDiningRegion, LineType);
						InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNode.SkirtingMeshModelID = ModelID;
						InSkirtingNode.RoomClassID = RoomClassId;
						InSkirtingNode.CraftId = CraftId;
						InSkirtingNode.SkirtingMeshID = ModelResID;
						InSkirtingNode.SKUID = SkuID;
						InSkirtingNode.MaterialID = MaterialResID;
						InSkirtingNode.MaterialModelID = MaterialModelID;
						InSkirtingNode.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
					}
					else
					{
						RemoveRegionNodes(LivingRoomID, LineType);
						RemoveRegionNodes(DiningRoomID, LineType);
						FSkirtingNode InSkirtingNodeA;
						InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
						InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeA.SkirtingMeshModelID = ModelID;
						InSkirtingNodeA.RoomClassID = RoomClassId;
						InSkirtingNodeA.CraftId = CraftId;
						InSkirtingNodeA.SkirtingMeshID = ModelResID;
						InSkirtingNodeA.SKUID = SkuID;
						InSkirtingNodeA.MaterialID = MaterialResID;
						InSkirtingNodeA.MaterialModelID = MaterialModelID;
						InSkirtingNodeA.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

						FSkirtingNode InSkirtingNodeB;
						InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
						InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
						InSkirtingNodeB.SkirtingMeshModelID = ModelID;
						InSkirtingNodeB.RoomClassID = RoomClassId;
						InSkirtingNodeB.CraftId = CraftId;
						InSkirtingNodeB.SkirtingMeshID = ModelResID;
						InSkirtingNodeB.SKUID = SkuID;
						InSkirtingNodeB.MaterialID = MaterialResID;
						InSkirtingNodeB.MaterialModelID = MaterialModelID;
						InSkirtingNodeB.HasComponent = false;
						LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
					}
				}


			}

			for (int _CurP = 0; _CurP < NoSnapPsOfDining.Num(); ++_CurP)
			{
				TotalNoSnapComPs.Add(NoSnapPsOfDining[_CurP]);
			}


		}
		/////客厅区域不含有组件且餐厅区域也不含有组件时
		else if ((CombinedArrOfLiving.Num() == 0) && (CombinedArrOfDining.Num() == 0))
		{

			TArray<FVector2D> AreaPath = RoomIDMapRegionPs(RoomID);
			TArray<FVector2D> PolygonA, PolygonB;
			if (LivingRoomPs.Num() < DiningRoomPs.Num())
			{
				PolygonA = LivingRoomPs;
				PolygonB = DiningRoomPs;
			}
			else if (LivingRoomPs.Num() > DiningRoomPs.Num())
			{
				PolygonA = DiningRoomPs;
				PolygonB = LivingRoomPs;
			}
			else
			{
				float L1 = 0.0, L2 = 0.0;
				int32 NextI = 0;
				for (int32 I = 0; I < LivingRoomPs.Num(); I++)
				{
					if (I == LivingRoomPs.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					L1 = L1 + FVector2D::Distance(LivingRoomPs[I], LivingRoomPs[NextI]);
				}
				int32 NextJ = 0;
				for (int32 J = 0; J < DiningRoomPs.Num(); J++)
				{
					if (J == DiningRoomPs.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					L2 = L2 + FVector2D::Distance(DiningRoomPs[J], DiningRoomPs[NextJ]);
				}

				if (L1 < L2)
				{
					PolygonA = LivingRoomPs;
					PolygonB = DiningRoomPs;
				}
				else
				{
					PolygonA = DiningRoomPs;
					PolygonB = LivingRoomPs;
				}
			}

			TArray<FVector2D> TPolygonA;
			TArray<FVector2D> TPolygonB;
			TArray<FVector2D> Polygons;
			TArray<FVector2D>InterSectPs;
			//纠正方向
			if (IsPolyClockWise(PolygonA))
			{
				ChangeClockwise(PolygonA);
			}
			if (IsPolyClockWise(PolygonB))
			{
				ChangeClockwise(PolygonB);
			}
			bool CGInterSect = FPolygonAlg::JudgePolygonInterSect(PolygonA, PolygonB);
			bool HasCommonPoints = FPolygonAlg::CalculateTwoPolygonInterSect(PolygonA, PolygonB, TPolygonA, TPolygonB, InterSectPs, Polygons);
			if ((CGInterSect) && (!HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
						MaterialModelID, MaterialResID, InSkirtingNode, LineType);
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}
			else if ((HasCommonPoints) && (!CGInterSect))
			{

				RemoveRegionNodes(LivingRoomID, LineType);
				RemoveRegionNodes(DiningRoomID, LineType);
				FSkirtingNode InSkirtingNodeA;
				InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
				InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
				InSkirtingNodeA.SkirtingMeshModelID = ModelID;
				InSkirtingNodeA.RoomClassID = RoomClassId;
				InSkirtingNodeA.CraftId = CraftId;
				InSkirtingNodeA.SkirtingMeshID = ModelResID;
				InSkirtingNodeA.SKUID = SkuID;
				InSkirtingNodeA.MaterialID = MaterialResID;
				InSkirtingNodeA.MaterialModelID = MaterialModelID;
				LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

				FSkirtingNode InSkirtingNodeB;
				InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
				InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
				InSkirtingNodeB.SkirtingMeshModelID = ModelID;
				InSkirtingNodeB.RoomClassID = RoomClassId;
				InSkirtingNodeB.CraftId = CraftId;
				InSkirtingNodeB.SkirtingMeshID = ModelResID;
				InSkirtingNodeB.SKUID = SkuID;
				InSkirtingNodeB.MaterialID = MaterialResID;
				InSkirtingNodeB.MaterialModelID = MaterialModelID;
				LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
			}
			else if ((CGInterSect) && (HasCommonPoints))
			{
				if ((TPolygonA.Num() > 0) && (TPolygonB.Num() > 0))
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					InSkirtingNodeA.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					InSkirtingNodeB.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
				else
				{
					FSkirtingNode InSkirtingNode;
					RemoveRegionNodes(LivingRoomID, LineType);
					InSkirtingNode.InitFromAreaPath(LivingRoomPs, LineType);
					InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNode.SkirtingMeshModelID = ModelID;
					InSkirtingNode.RoomClassID = RoomClassId;
					InSkirtingNode.CraftId = CraftId;
					InSkirtingNode.SkirtingMeshID = ModelResID;
					InSkirtingNode.SKUID = SkuID;
					InSkirtingNode.MaterialID = MaterialResID;
					InSkirtingNode.MaterialModelID = MaterialModelID;
					InSkirtingNode.HasComponent = false;
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
			}
			else if ((!CGInterSect) && (!HasCommonPoints))
			{
				TPolygonA.Empty();
				TPolygonB.Empty();
				Polygons.Empty();
				bool InterSectSuccess = ARoomActor::PolygonsUnionFunction(PolygonA,
					PolygonB,
					TPolygonA,
					TPolygonB,
					Polygons);

				if (!InterSectSuccess)
				{
					FSkirtingNode InSkirtingNode;
					CreateLocalSingleRegionNode(RoomID, ModelID, ModelResID, RoomClassId, CraftId, SkuID,
						MaterialModelID, MaterialResID, InSkirtingNode, LineType);
					LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
				}
				else
				{
					RemoveRegionNodes(LivingRoomID, LineType);
					RemoveRegionNodes(DiningRoomID, LineType);
					FSkirtingNode InSkirtingNodeA;
					InSkirtingNodeA.InitFromAreaPath(TPolygonA, LineType, false);
					InSkirtingNodeA.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeA.SkirtingMeshModelID = ModelID;
					InSkirtingNodeA.RoomClassID = RoomClassId;
					InSkirtingNodeA.CraftId = CraftId;
					InSkirtingNodeA.SkirtingMeshID = ModelResID;
					InSkirtingNodeA.SKUID = SkuID;
					InSkirtingNodeA.MaterialID = MaterialResID;
					InSkirtingNodeA.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeA, ModelResID, MaterialResID);

					FSkirtingNode InSkirtingNodeB;
					InSkirtingNodeB.InitFromAreaPath(TPolygonB, LineType, false);
					InSkirtingNodeB.SrcType = ESkirtingSrcType::SrcFromSyncPath;
					InSkirtingNodeB.SkirtingMeshModelID = ModelID;
					InSkirtingNodeB.RoomClassID = RoomClassId;
					InSkirtingNodeB.CraftId = CraftId;
					InSkirtingNodeB.SkirtingMeshID = ModelResID;
					InSkirtingNodeB.SKUID = SkuID;
					InSkirtingNodeB.MaterialID = MaterialResID;
					InSkirtingNodeB.MaterialModelID = MaterialModelID;
					LocalCreateLinesByModelFile(InSkirtingNodeB, ModelResID, MaterialResID);
				}
			}

		}
	}
	else if ((LivingRoomID != RoomID) && (DiningRoomID != RoomID))
	{
		TArray<FVector2D> NewRegion, NoSnapPs;
		bool SnapSuccess = FPolygonAlg::RegionSnapMoreComps(AreaPath, CombinedArr, NewRegion, NoSnapPs);
		if (SnapSuccess)
		{
			RemoveRegionNodes(RoomID, LineType);
			FSkirtingNode InSkirtingNode;
			InSkirtingNode.InitFromAreaPath(NewRegion, LineType);
			InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
			InSkirtingNode.SkirtingMeshModelID = ModelID;
			InSkirtingNode.RoomClassID = RoomClassId;
			InSkirtingNode.CraftId = CraftId;
			InSkirtingNode.SkirtingMeshID = ModelResID;
			InSkirtingNode.SKUID = SkuID;
			InSkirtingNode.MaterialID = MaterialResID;
			InSkirtingNode.MaterialModelID = MaterialModelID;
			InSkirtingNode.HasComponent = false;
			LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
			for (int _CurP = 0; _CurP < NoSnapPs.Num(); ++_CurP)
			{
				TotalNoSnapComPs.Add(NoSnapPs[_CurP]);
			}
		}
		else
		{
			RemoveRegionNodes(RoomID, LineType);
			FSkirtingNode InSkirtingNode;
			InSkirtingNode.InitFromAreaPath(AreaPath, LineType);
			InSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
			InSkirtingNode.SkirtingMeshModelID = ModelID;
			InSkirtingNode.RoomClassID = RoomClassId;
			InSkirtingNode.CraftId = CraftId;
			InSkirtingNode.SkirtingMeshID = ModelResID;
			InSkirtingNode.SKUID = SkuID;
			InSkirtingNode.MaterialID = MaterialResID;
			InSkirtingNode.MaterialModelID = MaterialModelID;
			InSkirtingNode.HasComponent = false;
			LocalCreateLinesByModelFile(InSkirtingNode, ModelResID, MaterialResID);
		}
	}
	int NumOfNoSnapComps = TotalNoSnapComPs.Num() / 4;
	for (int _CurNoSnapI = 0; _CurNoSnapI < NumOfNoSnapComps; ++_CurNoSnapI)
	{
		TArray<FVector2D> TempGroupPs;
		TempGroupPs.Add(TotalNoSnapComPs[0 + 4 * _CurNoSnapI]);
		TempGroupPs.Add(TotalNoSnapComPs[1 + 4 * _CurNoSnapI]);
		TempGroupPs.Add(TotalNoSnapComPs[2 + 4 * _CurNoSnapI]);
		TempGroupPs.Add(TotalNoSnapComPs[3 + 4 * _CurNoSnapI]);
		if (IsPolyClockWise(TempGroupPs))
		{
			ChangeClockwise(TempGroupPs);
		}
		FSkirtingNode ComSkirtingNode;
		ComSkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
		ComSkirtingNode.SkirtingMeshModelID = ModelID;
		ComSkirtingNode.RoomClassID = RoomClassId;
		ComSkirtingNode.CraftId = CraftId;
		ComSkirtingNode.SkirtingMeshID = ModelResID;
		ComSkirtingNode.SKUID = SkuID;
		ComSkirtingNode.MaterialID = MaterialResID;
		ComSkirtingNode.MaterialModelID = MaterialModelID;
		ComSkirtingNode.HasComponent = true;
		ComSkirtingNode.InitFromAreaPath(TempGroupPs, LineType);
		CreateSkirtingComponentByNodes(ComSkirtingNode, true);
	}

	////////////////////////剔除厨卫空间的脚线//////////////////////////
	TArray<AActor*> _RoomRes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomActor::StaticClass(), _RoomRes);
	const FString Str3 = TEXT("厨房");
	const FString Str4 = TEXT("客卫");
	const FString Str5 = TEXT("主卫");

	TArray<FBoundaryPsOfRegion> RegionsToRemove;
	for (int i = 0; i < _RoomRes.Num(); ++i)
	{
		ARoomActor * _Room = Cast<ARoomActor>(_RoomRes[i]);
		if (_Room)
		{
			FString TempStr = _Room->RoomUseName;
			if ((TempStr.Contains(Str3)) || (TempStr.Contains(Str4)) || (TempStr.Contains(Str5)))
			{
				FRoomPath TempRoomPs = _Room->GetRoomPath();
				FBoundaryPsOfRegion TempBoundPs;
				TempBoundPs.BoundaryPs = TempRoomPs.InnerRoomPath;
				RegionsToRemove.Add(TempBoundPs);
			}
		}
	}

	for (int _CurR = 0; _CurR < RegionsToRemove.Num(); ++_CurR)
	{
		DeleteSkirtingSegs(RegionsToRemove[_CurR].BoundaryPs, LineType);
	}
	return true;
}

void ASkirtingSystem::SyncTargetSkirtingPath(ESkirtingType TargetType)
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::SyncTargetSkirtingPath"));
	ESkirtingType SrcType = ((TargetType == ESkirtingType::SkirtingTopLine) ? ESkirtingType::SkirtingLine : ESkirtingType::SkirtingTopLine);

	auto DestroyComponents = [](TArray<USkirtingMeshComponent*> &Components)
	{
		for (auto &Comp : Components)
		{
			if (Comp != nullptr)
			{
				Comp->GetOwner()->Destroy();
			}
		}
		Components.Empty();
	};
	TArray<USkirtingMeshComponent*> TargetSkirtings;
	switch (SrcType)
	{
	case ESkirtingType::SkirtingLine:
	{
		DestroyComponents(SkirtingMeshComponents);
		DestroyComponents(SkirtingAreaMeshComps);
		TargetSkirtings.Append(TopSkirtingMeshComponents);
		TargetSkirtings.Append(TopSkirtingAreaMeshComps);
	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{
		DestroyComponents(TopSkirtingMeshComponents);
		DestroyComponents(TopSkirtingAreaMeshComps);
		TargetSkirtings.Append(SkirtingMeshComponents);
		TargetSkirtings.Append(SkirtingAreaMeshComps);
	}
	break;
	default:
		break;
	}

	for (auto &TargetSkirting : TargetSkirtings)
	{
		auto &TargetSkirtingSaveNode = TargetSkirting->GetSkirtingSaveNode();
		//lijiuyi 2018.07.13  copy Node data
		FSkirtingNode newSkirtingNode = TargetSkirtingSaveNode;
		newSkirtingNode.SkirtingType = SrcType;
		SetSkiringDefaultData(newSkirtingNode);
		CreateSkirtingComponentByNode(newSkirtingNode);
	}
}

USkirtingMeshComponent* ASkirtingSystem::CreateSkirtingActorByDefault(ESkirtingType InSkirtingType,
	ECgHeadingDirection HeadingDir)
{

	//UE_LOG(LogTemp, Warning, TEXT("USkirtingMeshComponent* ASkirtingSystem::CreateSkirtingActorByDefault"));
	FSkirtingNode SkirtingNode;
	SkirtingNode.HeadingDir = HeadingDir;
	SkirtingNode.SkirtingType = InSkirtingType;
	SkirtingNode.SrcType = ESkirtingSrcType::SrcFormDrawing;
	SetSkiringDefaultData(SkirtingNode);
	auto SkirtingMeshComp = CreateSkirtingComponentByNode(SkirtingNode);
	//在创建时存储该区域类型

	//if (SkirtingMeshComp != nullptr)
	//SkirtingMeshComp->ComArea = IsSkirtingMeshComArea(BeginPos);
	return SkirtingMeshComp;
}
//打开方案时构建踢脚线 模型
USkirtingMeshComponent* ASkirtingSystem::CreateSkirtingComponentByNode(FSkirtingNode InSkirtingNode, UModelFile *InSkirtingModel, UStaticMesh *InSkirtingMesh)
{

	FString str = InSkirtingNode.SkirtingMeshID;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
	if (InSkirtingNode.SkirtingMeshID.IsEmpty()|| InSkirtingNode.SkirtingMeshID== SkirtingLinedefaultNode.SkirtingLineResID
		|| InSkirtingNode.SkirtingMeshID== SkirtingTopLinedefaultNode.SkirtingLineResID
		|| InSkirtingNode.SkirtingMeshModelID == SkirtingLinedefaultNode.SkirtingMeshModelID
		|| InSkirtingNode.SkirtingMeshModelID == SkirtingTopLinedefaultNode.SkirtingMeshModelID)
	{
		if (!InSkirtingModel)
		{
			UE_LOG(LogTemp, Warning, TEXT("Case1"));
			InSkirtingModel = GetDefSkirtingModelByType(InSkirtingNode.SkirtingType);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Case2"));
		InSkirtingModel = ForceLoadMXByID(InSkirtingNode.SkirtingMeshID);
	}

	if (!InSkirtingModel && !InSkirtingMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Case3"));
		InSkirtingMesh = GetDefSkirtingMeshByType(InSkirtingNode.SkirtingType);
	}


	if (InSkirtingNode.Uuid.Equals(""))
	{
		int32 Seconds;
		float PartialSeconds;
		UGameplayStatics::GetAccurateRealTime(this, Seconds, PartialSeconds);
		FString uuid = FString::Printf(TEXT("%f"), Seconds + PartialSeconds);
		InSkirtingNode.Uuid = uuid;
	}
	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//AActor *SkirtingActor = GetWorld()->SpawnActor<AActor>(SpawmParams);
	ASkirtingActor *SkirtingActor = GetWorld()->SpawnActor<ASkirtingActor>(SpawmParams);
	SkirtingActor->Tags.Add(FName(TEXT("SkirtingLineActor")));
	USkirtingMeshComponent *SkirtingComponent = NewObject<USkirtingMeshComponent>(SkirtingActor);
	if (SkirtingComponent != nullptr)
	{
		SkirtingComponent->RegisterComponent();
		SkirtingComponent->ResetMeshType(InSkirtingModel);
		SkirtingComponent->ResetStaticMeshType(InSkirtingMesh);
		SkirtingComponent->GetSkirtingSaveNode() = InSkirtingNode;
		SkirtingComponent->AttachToComponent(SkirtingActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		SkirtingComponent->UpdateMeshByNode();
	}
	

	switch (InSkirtingNode.SkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{
		switch (InSkirtingNode.SrcType)
		{
		case ESkirtingSrcType::SrcFormDrawing:
		{
			SkirtingMeshComponents.Add(SkirtingComponent);
		}
		break;
		case ESkirtingSrcType::SrcFromSyncPath:
		{
			SkirtingAreaMeshComps.Add(SkirtingComponent);
		}
		break;
		default:
			break;
		}
	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{
		switch (InSkirtingNode.SrcType)
		{
		case ESkirtingSrcType::SrcFormDrawing:
		{
			TopSkirtingMeshComponents.Add(SkirtingComponent);
		}
		break;
		case ESkirtingSrcType::SrcFromSyncPath:
		{
			TopSkirtingAreaMeshComps.Add(SkirtingComponent);
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}

	return SkirtingComponent;
}
//重新生成角线
void ASkirtingSystem::RebuildSkirtingByType(ESkirtingType InSkirtingType)
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::RebuildSkirtingByType"));
	SyncAreaOutlinePathWithType(InSkirtingType);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
USkirtingMeshComponent* ASkirtingSystem::CreateSkirtingComponentByNodes(const FSkirtingNode &InSkirtingNode, bool IsComponent, UModelFile *InSkirtingModel, UStaticMesh *InSkirtingMesh)
{

	//UE_LOG(LogTemp, Warning, TEXT("USkirtingMeshComponent* ASkirtingSystem::CreateSkirtingComponentByNode"));
	FString str = InSkirtingNode.SkirtingMeshID;
	UE_LOG(LogTemp, Warning,TEXT("%s"),*str);
	if (InSkirtingNode.SkirtingMeshID.IsEmpty() || InSkirtingNode.SkirtingMeshID == SkirtingLinedefaultNode.SkirtingLineResID
		|| InSkirtingNode.SkirtingMeshID == SkirtingTopLinedefaultNode.SkirtingLineResID
		|| InSkirtingNode.SkirtingMeshModelID == SkirtingLinedefaultNode.SkirtingMeshModelID
		|| InSkirtingNode.SkirtingMeshModelID == SkirtingTopLinedefaultNode.SkirtingMeshModelID)
	{
		if (!InSkirtingModel)
		{
			UE_LOG(LogTemp, Warning, TEXT("Condition1"));
			InSkirtingModel = GetDefSkirtingModelByType(InSkirtingNode.SkirtingType);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Condition2"));
		InSkirtingModel = ForceLoadMXByID(InSkirtingNode.SkirtingMeshID);
	}

	if (!InSkirtingModel && !InSkirtingMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Condition3"));
		InSkirtingMesh = GetDefSkirtingMeshByType(InSkirtingNode.SkirtingType);
	}

	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASkirtingActor *SkirtingActor = GetWorld()->SpawnActor<ASkirtingActor>(SpawmParams);
	SkirtingActor->Tags.Add(FName(TEXT("SkirtingLineActor")));
	USkirtingMeshComponent *SkirtingComponent = NewObject<USkirtingMeshComponent>(SkirtingActor);
	if (SkirtingComponent != nullptr)
	{
		SkirtingComponent->RegisterComponent();
		SkirtingComponent->ResetMeshType(InSkirtingModel);
		SkirtingComponent->ResetStaticMeshType(InSkirtingMesh);
		SkirtingComponent->GetSkirtingSaveNode() = InSkirtingNode;
		SkirtingComponent->AttachToComponent(SkirtingActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		SkirtingComponent->UpdateMeshByNodes(IsComponent);
	}


	switch (InSkirtingNode.SkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{
		switch (InSkirtingNode.SrcType)
		{
		case ESkirtingSrcType::SrcFormDrawing:
		{
			SkirtingMeshComponents.Add(SkirtingComponent);
		}
		break;
		case ESkirtingSrcType::SrcFromSyncPath:
		{
			SkirtingAreaMeshComps.Add(SkirtingComponent);
		}
		break;
		default:
			break;
		}
	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{
		switch (InSkirtingNode.SrcType)
		{
		case ESkirtingSrcType::SrcFormDrawing:
		{
			TopSkirtingMeshComponents.Add(SkirtingComponent);
		}
		break;
		case ESkirtingSrcType::SrcFromSyncPath:
		{
			TopSkirtingAreaMeshComps.Add(SkirtingComponent);
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}

	return SkirtingComponent;
}
///////////////////////////////////////////////////////////////////////////////////////////
UModelFile* ASkirtingSystem::GetDefSkirtingModelByType(ESkirtingType InSkirtingType)
{
	//UE_LOG(LogTemp, Warning, TEXT("UModelFile* ASkirtingSystem::GetDefSkirtingModelByType"));
	UModelFile *SkirtingMesh = nullptr;
	switch (InSkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{
		auto ResMgr = UResourceMgr::Instance(GetWorld());
		if (ResMgr)
		{
			if (!SkirtingLineResID.IsEmpty())
			{
				//TEXT("C4F287B3495038D6B476139C0BFDB14C")
				//TEXT("C179D6B746DEC50B4329C4B6F114F3AC")

				SkirtingLineResID = SkirtingLinedefaultNode.SkirtingLineResID;
				SkirtingLineRes = Cast<UModelFile>(ResMgr->FindRes(SkirtingLineResID));
				if (SkirtingLineRes)
				{
					SkirtingLineRes->ForceLoad();
				}
			}
		}

		SkirtingMesh = SkirtingLineRes;
	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{
		auto ResMgr = UResourceMgr::Instance(GetWorld());
		if (!SkirtingTopLineResID.IsEmpty())
		{
			//TEXT("73AA3AD343812BC5686D4A8D165272C7")
			SkirtingTopLineResID = SkirtingTopLinedefaultNode.SkirtingLineResID;
			SkirtingTopLineRes = Cast<UModelFile>(ResMgr->FindRes(SkirtingTopLineResID));
			if (SkirtingTopLineRes)
			{
				SkirtingTopLineRes->ForceLoad();
			}
		}

		SkirtingMesh = SkirtingTopLineRes;
	}
	break;
	default:
		break;
	}

	return SkirtingMesh;
}

UStaticMesh* ASkirtingSystem::GetDefSkirtingMeshByType(ESkirtingType InSkirtingType)
{
	//UE_LOG(LogTemp, Warning, TEXT("UStaticMesh* ASkirtingSystem::GetDefSkirtingMeshByType"));
	UStaticMesh *SkirtingMesh = nullptr;
	switch (InSkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{
		SkirtingMesh = SkirtingLineMesh;
	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{
		SkirtingMesh = SkirtingTopLineMesh;
	}
	break;
	default:
		break;
	}

	return SkirtingMesh;
}

void ASkirtingSystem::CreateStaticMesh()
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::CreateStaticMesh"));
	UModelFile *TestModel = nullptr;
	auto ResMgr = UResourceMgr::Instance(GetWorld());
	if (!TestResID.IsEmpty())
	{
		TestModel = Cast<UModelFile>(ResMgr->FindRes(TestResID));
		if (TestModel)
		{
			TestModel->ForceLoad();
		}
	}

	if (!TestModel)
	{
		return;
	}

	FTransform Transform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector, FVector::ZeroVector));

	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor *TestActor = GetWorld()->SpawnActor<AActor>(SpawmParams);
	UModelFileComponent *MeshComponent = NewObject<UModelFileComponent>(TestActor);
	TestActor->SetRootComponent(MeshComponent);
	MeshComponent->UpdateModel(TestModel);
	FBox testBound = TestModel->LocalBounds.ShiftBy(-TestModel->Offset);

	FTransform LocalTransform = FTransform::Identity;
	LocalTransform = FTransform(FRotator(0, 180.0f, 0));
	LocalTransform *= FTransform(FMatrix(FVector::ForwardVector, -FVector::RightVector, FVector::UpVector, FVector::ZeroVector));
	Transform = FTransform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector,
		FVector(-testBound.Min.X, testBound.Max.Y, -testBound.Min.Z)));
	Transform = LocalTransform*Transform;
	MeshComponent->SetWorldTransform(Transform);

	FBox ActorBox = TestActor->GetComponentsBoundingBox();
	MeshComponent->RegisterComponentWithWorld(GetWorld());
}

void ASkirtingSystem::CreateProceduralMesh()
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::CreateProceduralMesh"));
	UModelFile *TestModel = nullptr;
	auto ResMgr = UResourceMgr::Instance(GetWorld());
	if (!TestResID.IsEmpty())
	{
		TestModel = Cast<UModelFile>(ResMgr->FindRes(TestResID));
		if (TestModel)
		{
			TestModel->ForceLoad();
		}
	}

	if (!TestModel)
	{
		return;
	}

	//FBox testBound = TestModel->LocalBounds.ShiftBy(-TestModel->Offset);
	FBox testBound = TestModel->LocalBounds;
	UStaticMeshInfoExtract *StaticMeshInfoExtract = NewObject<UStaticMeshInfoExtract>(this, TEXT("MeshInfoExtract"));
	StaticMeshInfoExtract->ResetMXMesh(TestModel);

	FTransform Transform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector, FVector::ZeroVector));
	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor *TestActor = GetWorld()->SpawnActor<AActor>(SpawmParams);
	UProceduralMeshComponent *ProceduralMeshComp = NewObject<UProceduralMeshComponent>(TestActor);
	if (ProceduralMeshComp != nullptr)
	{
		TestActor->SetRootComponent(ProceduralMeshComp);
		ProceduralMeshComp->CreateMeshSection_LinearColor(0, StaticMeshInfoExtract->Positions,
			StaticMeshInfoExtract->Indices, StaticMeshInfoExtract->Normals,
			StaticMeshInfoExtract->UVs, StaticMeshInfoExtract->Colors, StaticMeshInfoExtract->Tangents, true);

		FTransform LocalTransform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector, FVector::ZeroVector));
		LocalTransform *= FTransform(FMatrix(FVector::ForwardVector, -FVector::RightVector, FVector::UpVector, FVector::ZeroVector));
		Transform = FTransform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector,
			FVector(-testBound.Min.X, testBound.Max.Y, -testBound.Min.Z)));
		Transform = LocalTransform * Transform;

		ProceduralMeshComp->SetWorldTransform(Transform);
		ProceduralMeshComp->RegisterComponentWithWorld(GetWorld());
		ProceduralMeshComp->SetMaterial(0, StaticMeshInfoExtract->MxMesh->GetUE4Material(0));
		ProceduralMeshComp->SetCastShadow(false);
	}
	
}

void ASkirtingSystem::CreateAStaticMesh()
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::CreateAStaticMesh"));
	UModelFile *TestModel = nullptr;
	auto ResMgr = UResourceMgr::Instance(GetWorld());
	if (!TestResID.IsEmpty())
	{
		TestModel = Cast<UModelFile>(ResMgr->FindRes(TestResID));
		if (TestModel)
		{
			TestModel->ForceLoad();
		}
	}

	if (!TestModel)
	{
		return;
	}

	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor *TestActor = GetWorld()->SpawnActor<AActor>(SpawmParams);
	UModelFileComponent *MeshComponent = NewObject<UModelFileComponent>(TestActor);
	if (MeshComponent != nullptr)
	{
		TestActor->SetRootComponent(MeshComponent);
		MeshComponent->UpdateModel(TestModel);
		FBox testBound = TestModel->GetLocalBounds();
		testBound = testBound.ShiftBy(TestModel->Offset);

		testBound = TestModel->LocalBounds.TransformBy(FTransform(FRotator(0, 180.0f, 0),
			TestModel->Offset, TestModel->Scale3D));

		FTransform Transform =
			FTransform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector,
				FVector(-testBound.Min)));
		MeshComponent->SetWorldTransform(Transform);
		MeshComponent->RegisterComponentWithWorld(GetWorld());

		FBox ActorBox = TestActor->GetComponentsBoundingBox();
	}
	
}

void ASkirtingSystem::CreateAProceduralMesh()
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::CreateAProceduralMesh"));
	UModelFile *TestModel = nullptr;
	auto ResMgr = UResourceMgr::Instance(GetWorld());
	if (!TestResID.IsEmpty())
	{
		TestModel = Cast<UModelFile>(ResMgr->FindRes(TestResID));
		if (TestModel)
		{
			TestModel->ForceLoad();
		}
	}

	if (!TestModel)
	{
		return;
	}

	//FBox testBound = TestModel->LocalBounds.ShiftBy(-TestModel->Offset);
	UStaticMeshInfoExtract *StaticMeshInfoExtract = NewObject<UStaticMeshInfoExtract>(this, TEXT("MeshInfoExtract"));
	StaticMeshInfoExtract->ResetMXMesh(TestModel);

	FActorSpawnParameters SpawmParams;
	SpawmParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor *TestActor = GetWorld()->SpawnActor<AActor>(SpawmParams);
	UProceduralMeshComponent *ProceduralMeshComp = NewObject<UProceduralMeshComponent>(TestActor);
	if (ProceduralMeshComp != nullptr)
	{
		TestActor->SetRootComponent(ProceduralMeshComp);
		ProceduralMeshComp->CreateMeshSection_LinearColor(0, StaticMeshInfoExtract->Positions,
			StaticMeshInfoExtract->Indices, StaticMeshInfoExtract->Normals,
			StaticMeshInfoExtract->UVs, StaticMeshInfoExtract->Colors, StaticMeshInfoExtract->Tangents, true);

		FBox testBound = TestModel->LocalBounds.TransformBy(FTransform(FRotator(0, 180.0f, 0),
			TestModel->Offset, TestModel->Scale3D));
		FTransform Transform = FTransform(FMatrix(FVector::ForwardVector, FVector::RightVector, FVector::UpVector, -testBound.Min));
		Transform = FTransform(FRotator(0, 180.0f, 0),
			TestModel->Offset, TestModel->Scale3D) * Transform;

		ProceduralMeshComp->SetWorldTransform(Transform);
		ProceduralMeshComp->RegisterComponentWithWorld(GetWorld());
		ProceduralMeshComp->SetMaterial(0, StaticMeshInfoExtract->MxMesh->GetUE4Material(0));
	}
	
}

float ASkirtingSystem::GetSkirtingTopLineDefHeight() const
{
	return SkirtingTopLineDefHeight;
}

void ASkirtingSystem::CollectAllSkirtingLines(TArray<USkirtingMeshComponent*> &OutSkirtingLines)
{
	OutSkirtingLines.Append(SkirtingMeshComponents);
	OutSkirtingLines.Append(TopSkirtingMeshComponents);
	OutSkirtingLines.Append(SkirtingAreaMeshComps);
	OutSkirtingLines.Append(TopSkirtingAreaMeshComps);
}

TArray<FVector2D> ASkirtingSystem::FVector2DAreaToFVector2D(TArray<FVector2DArea> & Vec2DArea)
{
	TArray<FVector2D> Out;
	for (int i = 0; i < Vec2DArea.Num(); ++i)
	{
		Out.Add(Vec2DArea[i].Pos);
	}
	return Out;
}

bool ASkirtingSystem::IsPolyClockWise(const TArray<FVector2D> & Polygons)
{
	double d = 0.f;

	for (int i = 0; i < Polygons.Num(); i++)
	{
		d += -0.5f * ((Polygons[(i + 1) % Polygons.Num()].Y + Polygons[i].Y) * (Polygons[(i + 1) % Polygons.Num()].X - Polygons[i].X));
	}
	return d < 0.f;
}

void ASkirtingSystem::FilterVertex(TArray<FVector2D> & Polygons)
{
	TArray<FVector2D> Temp;
	for (int i = 0; i < Polygons.Num(); ++i)
	{
		int32 index = (i + 1) % Polygons.Num();
		FVector2D Length;
		if (index != 0)
			Length = Polygons[i] - Polygons[(i + 1) % Polygons.Num()];
		if(i == Polygons.Num()-1)
			Length = Polygons[i] - Polygons[(i + 1) % Polygons.Num()];

		if (index != 0 && (Length.Size() > -1 && Length.Size() <= 0) || (Length.Size() < 1 && Length.Size() >= 0))
		{
			Temp.Add((Polygons[i] + Polygons[(i + 1) % Polygons.Num()]) / 2.f);
			++i;
		}
		else
		{
			Temp.Add(Polygons[i]);
		}
	}
	Polygons.Reserve(Temp.Num());
	Polygons = Temp;
}

TArray<FVector2D> ASkirtingSystem::PolygonScale(TArray<FVector2D> & Polygons, int multiple)
{
	TArray<FVector2D> NewPolygons;
	for (int i = 0; i < Polygons.Num(); ++i)
	{
		int beginIndex = i - 1;
		FVector2D A;
		if (beginIndex < 0)
		{
			beginIndex = Polygons.Num() - 1;
			A = Polygons[(beginIndex) % Polygons.Num()];
		}
		else
		{
			A = Polygons[(i - 1) % Polygons.Num()];
		}
		FVector2D B(Polygons[i]);
		FVector2D C(Polygons[(i + 1) % Polygons.Num()]);

		FVector2D dirA = (B - A);
		dirA.Normalize();
		FVector2D dirB = (B - C);
		dirB.Normalize();
		float anglecos = FVector2D::DotProduct(-dirA, -dirB);
		if (anglecos > -0.98)
		{
			//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), FVector(Polygons[i].X, Polygons[i].Y, 0), 100.f, FLinearColor::Red, 100000.f);
			FVector2D dirc = (dirA + dirB);
			dirc.Normalize();
			if (multiple)
			{
				if (FVector2D::CrossProduct(dirA, -dirB) > 0)
					NewPolygons.Add(Polygons[i] - dirc * 3);
				else
					NewPolygons.Add(Polygons[i] + dirc * 3);
			}
			else
			{
				if (FVector2D::CrossProduct(dirA, -dirB) > 0)
					NewPolygons.Add(Polygons[i] + dirc * 3);
				else
					NewPolygons.Add(Polygons[i] - dirc * 3);
			}
		}
	/*	if(beginIndex==16)
		{
			UKismetSystemLibrary::DrawDebugPoint(GetWorld(), FVector(B.X, B.Y, 0), 100.f, FLinearColor::Red, 100000.f);
			UKismetSystemLibrary::DrawDebugPoint(GetWorld(), FVector(A.X, A.Y, 0), 100.f, FLinearColor::Blue, 100000.f);
			UKismetSystemLibrary::DrawDebugPoint(GetWorld(), FVector(C.X, C.Y, 0), 100.f, FLinearColor::Yellow, 100000.f);
		}*/
		/*FVector2D G((A.X + B.X + C.X) / 3.f, (A.Y + B.Y + C.Y) / 3.f);

		FVector2D Nor(Polygons[i] - G);
		Nor.Normalize();
		Polygons[i] = Polygons[i] + (Nor / multiple);*/
	}
	/*for (int i = 0;i < NewPolygons.Num();++i)
	{
		UKismetSystemLibrary::DrawDebugPoint(GetWorld(), FVector(NewPolygons[i].X, NewPolygons[i].Y, 0), 100.f, FLinearColor::Red, 100000.f);
	}*/
	return NewPolygons;
}

void ASkirtingSystem::ChangeClockwise(TArray<FVector2D> & Polygons)
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

//bool ASkirtingSystem::isPolygonContainsPoint(const AActor * Area,const FVector2D & CutPoint)
//{
//	if (Area)
//	{
//		int nCross = 0;
//		UProceduralMeshComponent *  PGround =nullptr;
//		TSet<UActorComponent*>  ChiCom = Area->GetComponents();
//		for (auto ACom : ChiCom)
//		{
//			if (ACom->GetName() == FString(TEXT("PGround")))
//			{
//				PGround = Cast<UProceduralMeshComponent>(ACom);
//				break;
//			}
//		}
//		if (PGround)
//		{
//
//			FProcMeshSection *  FPMS = PGround->GetProcMeshSection(PGround->GetNumSections() - 1);
//			for (int i = 0, j = FPMS->ProcVertexBuffer.Num() - 1; i < FPMS->ProcVertexBuffer.Num(); j = i++)
//			{
//
//				if ((((FPMS->ProcVertexBuffer[i].Position.Y <= CutPoint.Y) && (CutPoint.Y <FPMS->ProcVertexBuffer[j].Position.Y)) ||
//					((FPMS->ProcVertexBuffer[j].Position.Y <= CutPoint.Y) && (CutPoint.Y <FPMS->ProcVertexBuffer[i].Position.Y))) &&
//					(CutPoint.X < (FPMS->ProcVertexBuffer[j].Position.X - FPMS->ProcVertexBuffer[i].Position.X) * (CutPoint.Y - FPMS->ProcVertexBuffer[i].Position.Y)
//						/(FPMS->ProcVertexBuffer[j].Position.Y - FPMS->ProcVertexBuffer[i].Position.Y)+ FPMS->ProcVertexBuffer[i].Position.X))
//					nCross = 1;
//			}
//			if (nCross) return true;
//		}
//	}
//	return false;
//}

UModelFile* ASkirtingSystem::ForceLoadMXByID(const FString &ResID)
{
	//UE_LOG(LogTemp, Warning, TEXT("UModelFile* ASkirtingSystem::ForceLoadMXByID"));
	UModelFile *FoundMx = nullptr;
	auto ResMgr = UResourceMgr::Instance(GetWorld());
	if (ResMgr)
	{
		if (!ResID.IsEmpty())
		{
			FoundMx = Cast<UModelFile>(ResMgr->FindRes(ResID));
			if (FoundMx)
			{
				FoundMx->ForceLoad();
			}
		}
	}

	return FoundMx;
}


//void ASkirtingSystem::SetSkiringDefaultData(FSkirtingNode &InSkirtingNode)
//{
//	int32 Seconds;
//	float PartialSeconds;
//	UGameplayStatics::GetAccurateRealTime(this, Seconds, PartialSeconds);
//	FString uuid = FString::Printf(TEXT("%f"), Seconds + PartialSeconds);
//	UE_LOG(LogTemp, Warning, TEXT("SetSkiringDefaultData uuid = %s"), *InSkirtingNode.Uuid);
//	if (ESkirtingType::SkirtingLine == InSkirtingNode.SkirtingType)
//	{
//		InSkirtingNode.SkirtingMeshModelID = SkirtingLinedefaultNode.SkirtingMeshModelID;
//		InSkirtingNode.RoomClassID = SkirtingLinedefaultNode.RoomClassID;
//		InSkirtingNode.SKUID = SkirtingLinedefaultNode.SKUID;
//		InSkirtingNode.CraftId = SkirtingLinedefaultNode.CraftId;
//		InSkirtingNode.SkirtingMeshID = SkirtingLinedefaultNode.SkirtingLineResID;
//		InSkirtingNode.Uuid = uuid;
//	}
//	else
//		if (ESkirtingType::SkirtingTopLine == InSkirtingNode.SkirtingType)
//		{
//			InSkirtingNode.SkirtingMeshModelID = SkirtingTopLinedefaultNode.SkirtingMeshModelID;
//			InSkirtingNode.RoomClassID = SkirtingTopLinedefaultNode.RoomClassID;
//			InSkirtingNode.SKUID = SkirtingTopLinedefaultNode.SKUID;
//			InSkirtingNode.CraftId = SkirtingTopLinedefaultNode.CraftId;
//			InSkirtingNode.SkirtingMeshID = SkirtingTopLinedefaultNode.SkirtingLineResID;
//			InSkirtingNode.Uuid = uuid;
//		}
//}

void ASkirtingSystem::SetSkiringDefaultData(FSkirtingNode &InSkirtingNode, bool HasComponent)
{
	int32 Seconds;
	float PartialSeconds;
	UGameplayStatics::GetAccurateRealTime(this, Seconds, PartialSeconds);
	FString uuid = FString::Printf(TEXT("%f"), Seconds + PartialSeconds);
	UE_LOG(LogTemp, Warning, TEXT("SetSkiringDefaultData uuid = %s"), *InSkirtingNode.Uuid);
	if (ESkirtingType::SkirtingLine == InSkirtingNode.SkirtingType)
	{
		InSkirtingNode.SkirtingMeshModelID = SkirtingLinedefaultNode.SkirtingMeshModelID;
		InSkirtingNode.RoomClassID = SkirtingLinedefaultNode.RoomClassID;
		InSkirtingNode.SKUID = SkirtingLinedefaultNode.SKUID;
		InSkirtingNode.CraftId = SkirtingLinedefaultNode.CraftId;
		InSkirtingNode.SkirtingMeshID = SkirtingLinedefaultNode.SkirtingLineResID;
		InSkirtingNode.HasComponent = HasComponent;
		InSkirtingNode.Uuid = uuid;
	}
	else
		if (ESkirtingType::SkirtingTopLine == InSkirtingNode.SkirtingType)
		{
			InSkirtingNode.SkirtingMeshModelID = SkirtingTopLinedefaultNode.SkirtingMeshModelID;
			InSkirtingNode.RoomClassID = SkirtingTopLinedefaultNode.RoomClassID;
			InSkirtingNode.SKUID = SkirtingTopLinedefaultNode.SKUID;
			InSkirtingNode.CraftId = SkirtingTopLinedefaultNode.CraftId;
			InSkirtingNode.SkirtingMeshID = SkirtingTopLinedefaultNode.SkirtingLineResID;
			InSkirtingNode.HasComponent = HasComponent;
			InSkirtingNode.Uuid = uuid;
		}
}


void ASkirtingSystem::CalculationErrorData(TArray<FVector2D> &PolygonA, TArray<FVector2D> &PolygonB)
{
	for (int i = 0;i < PolygonA.Num();++i)
	{
		for (int j = 0;j < PolygonB.Num();++j)
		{
			FVector2D crossoverpoint;
			int state;
			float length = -100;
			FVector2D Vectordir1 = PolygonA[i] - PolygonB[j];
			FVector2D Vectordir2 = PolygonB[(j + 1) % PolygonB.Num()] - PolygonB[j];
			FVector2D Vectordir3 = PolygonA[i] - PolygonB[(j + 1) % PolygonB.Num()];
			FVector2D Vectordir4 = PolygonB[j] - PolygonB[(j + 1) % PolygonB.Num()];
			float ablength = Vectordir2.SizeSquared();
			float Dotvector = FVector2D::DotProduct(Vectordir1, Vectordir2);
			if (Dotvector <= 0)
			{
				length = Vectordir1.Size();
				state = 1;
			}
			else
				if (Dotvector >= ablength)
				{
					length = Vectordir3.Size();
					state = 2;
				}
				else
				{
					float r = Dotvector / ablength;
					crossoverpoint = FVector2D(PolygonB[j].X + (PolygonB[(j + 1) % PolygonB.Num()].X - PolygonB[j].X)*r, PolygonB[j].Y + (PolygonB[(j + 1) % PolygonB.Num()].Y - PolygonB[j].Y)*r);
					length = (crossoverpoint - PolygonA[i]).Size();
					state = 3;
				}
			if (length > 0 && length < 5)
			{
				if (state == 1)
					PolygonA[i] = PolygonB[j];
				else
					if (state == 2)
						PolygonA[i] = PolygonB[(j + 1) % PolygonB.Num()];
					else
						if (state == 3)
							PolygonA[i] = crossoverpoint;
				//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), FVector(PolygonA[i].X, PolygonA[i].Y, 0), 50.f, FLinearColor::Red, 100000.f);
			}
		}

	}
}


void ASkirtingSystem::RemoveRegionDecorateLines(TArray<FVector2D> RegionPoints)
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::RemoveRegionDecorateLines"));
	//清除已存在路径
	auto DestroyComponents = [](TArray<USkirtingMeshComponent*> &Components)
	{
		for (auto &Comp : Components)
		{
			if (Comp != nullptr)
			{
				Comp->GetOwner()->Destroy();
			}
		}
		Components.Empty();
	};



	TArray<USkirtingMeshComponent*> ToRemoveMeshCompts;
	TArray< USkirtingMeshComponent*> ToRemoveAreaCompts;
	for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
	{
		if (SkirtingMeshComponents[I] != NULL)
		{
			FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
			TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
			for (int32 J = 0; J < TempSegNodes.Num(); J++)
			{
				bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TempSegNodes[J].SkirtingStartCorner.Position);
				bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TempSegNodes[J].SkirtingEndCorner.Position);
				if ((StartInOrOnBoundary)||(EndInOrOnBoundary))
				{
					ToRemoveMeshCompts.Add(SkirtingMeshComponents[I]);
				}
				break;
			}

		}

	}
	for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
	{
		if (SkirtingAreaMeshComps[K] != NULL)
		{
			FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
			TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
			for (int32 L = 0; L < TempSegNodes.Num(); L++)
			{
				bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TempSegNodes[L].SkirtingStartCorner.Position);
				bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TempSegNodes[L].SkirtingEndCorner.Position);
				if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
				{
					ToRemoveAreaCompts.Add(SkirtingAreaMeshComps[K]);
				}
				break;
			}
		}

	}
	DestroyComponents(ToRemoveMeshCompts);
	DestroyComponents(ToRemoveAreaCompts);
	for (int32 Index = SkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
	{
		if (SkirtingMeshComponents[Index] == nullptr)
		{
			SkirtingMeshComponents.RemoveAt(Index);
		}
	}
	for (int32 Index = SkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
	{
		if (SkirtingAreaMeshComps[Index] == nullptr)
		{
			SkirtingAreaMeshComps.RemoveAt(Index);
		}
	}


	TArray<USkirtingMeshComponent*> ToRemoveTopMeshCompts;
	TArray< USkirtingMeshComponent*> ToRemoveTopAreaCompts;
	for (int32 I = 0; I < TopSkirtingMeshComponents.Num(); I++)
	{
		if (TopSkirtingMeshComponents[I] != NULL)
		{
			FSkirtingNode TempSkirtingNode = TopSkirtingMeshComponents[I]->GetSkirtingSaveNode();
			TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
			for (int32 J = 0; J < TempSegNodes.Num(); J++)
			{
				bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TempSegNodes[J].SkirtingStartCorner.Position);
				bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TempSegNodes[J].SkirtingEndCorner.Position);
				if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
				{
					ToRemoveTopMeshCompts.Add(TopSkirtingMeshComponents[I]);
				}
				break;
			}
		}

	}
	for (int32 K = 0; K < TopSkirtingAreaMeshComps.Num(); K++)
	{
		if (TopSkirtingAreaMeshComps[K] != NULL)
		{
			FSkirtingNode TempSkirtingNode = TopSkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
			TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
			for (int32 L = 0; L < TempSegNodes.Num(); L++)
			{
				bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TempSegNodes[L].SkirtingStartCorner.Position);
				bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TempSegNodes[L].SkirtingEndCorner.Position);
				if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
				{
					ToRemoveTopAreaCompts.Add(TopSkirtingAreaMeshComps[K]);
				}
				break;
			}
		}
	}
	DestroyComponents(ToRemoveTopMeshCompts);
	DestroyComponents(ToRemoveTopAreaCompts);

	for (int32 Index = TopSkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
	{
		if (TopSkirtingMeshComponents[Index] == nullptr)
		{
			TopSkirtingMeshComponents.RemoveAt(Index);
		}
	}
	for (int32 Index = TopSkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
	{
		if (TopSkirtingAreaMeshComps[Index] == nullptr)
		{
			TopSkirtingAreaMeshComps.RemoveAt(Index);
		}
	}
}



TArray<USkirtingMeshComponent*> ASkirtingSystem::GetSkirtingByRoomId(int32 RoomID)
{
	TArray<USkirtingMeshComponent*> SkirtingMeshComps;
	TArray<FVector2D> RegionPoints = RoomIDMapRegionPs(RoomID);

	for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
	{
		if (SkirtingMeshComponents[I] != NULL)
		{
			FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
			TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
			for (int32 J = 0; J < TempSegNodes.Num(); J++)
			{
				FVector2D StartPos = TempSegNodes[J].SkirtingStartCorner.Position;
				FVector2D EndPos = TempSegNodes[J].SkirtingEndCorner.Position;
				float SegDis = FVector2D::Distance(StartPos, EndPos);
				if (SegDis > 3)
				{
					FVector2D DirVec = EndPos - StartPos;
					DirVec = DirVec.GetSafeNormal();
					DirVec = DirVec.GetRotated(90);
					FVector2D SegMiddle = 0.5*(StartPos + EndPos);
					FVector2D TestP1 = SegMiddle + 8 * DirVec;
					FVector2D TestP2 = SegMiddle - 8 * DirVec;

					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TestP1);
					bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TestP2);
					if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
					{
						SkirtingMeshComps.Add(SkirtingMeshComponents[I]);

						break;
					}
				}

			}

		}

	}
	for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
	{
		if (SkirtingAreaMeshComps[K] != NULL)
		{
			FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
			TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
			for (int32 L = 0; L < TempSegNodes.Num(); L++)
			{
				FVector2D StartPos = TempSegNodes[L].SkirtingStartCorner.Position;
				FVector2D EndPos = TempSegNodes[L].SkirtingEndCorner.Position;
				float SegDis = FVector2D::Distance(StartPos, EndPos);
				if (SegDis > 3)
				{
					FVector2D DirVec = EndPos - StartPos;
					DirVec = DirVec.GetSafeNormal();
					DirVec = DirVec.GetRotated(90);
					FVector2D SegMiddle = 0.5*(StartPos + EndPos);
					FVector2D TestP1 = SegMiddle + 8 * DirVec;
					FVector2D TestP2 = SegMiddle - 8 * DirVec;

					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TestP1);
					bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TestP2);
					if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
					{
						SkirtingMeshComps.Add(SkirtingAreaMeshComps[K]);

						break;
					}

				}

			}
		}

	}
	return SkirtingMeshComps;
}


TArray<USkirtingMeshComponent*> ASkirtingSystem::FindDecorateLinesOfRoom(int32 RoomID, ESkirtingType InSkirtingType)
{

	TArray<USkirtingMeshComponent*> SkirtingMeshComps;
	TArray<FVector2D> RegionPoints = RoomIDMapRegionPs(RoomID);

	for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
	{
		if (SkirtingMeshComponents[I] != NULL)
		{
			FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
			TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
			for (int32 J = 0; J < TempSegNodes.Num(); J++)
			{
				FVector2D StartPos = TempSegNodes[J].SkirtingStartCorner.Position;
				FVector2D EndPos = TempSegNodes[J].SkirtingEndCorner.Position;
				float DisOfSeg = FVector2D::Distance(StartPos, EndPos);
				if (DisOfSeg > 3)
				{
					FVector2D DirVec = EndPos - StartPos;
					DirVec = DirVec.GetSafeNormal();
					DirVec = DirVec.GetRotated(90);
					FVector2D SegMiddle = 0.5*(StartPos + EndPos);
					FVector2D TestP1 = SegMiddle + 8 * DirVec;
					FVector2D TestP2 = SegMiddle - 8 * DirVec;

					bool P1InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TestP1);
					bool P2InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TestP2);
					if ((P1InOrOnBoundary) || (P2InOrOnBoundary))
					{
						SkirtingMeshComps.Add(SkirtingMeshComponents[I]);
					}
					break;
				}

			}

		}

	}
	for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
	{
		if (SkirtingAreaMeshComps[K] != NULL)
		{
			FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
			TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
			for (int32 L = 0; L < TempSegNodes.Num(); L++)
			{
				FVector2D StartPos = TempSegNodes[L].SkirtingStartCorner.Position;
				FVector2D EndPos = TempSegNodes[L].SkirtingEndCorner.Position;
				float DisOfSeg = FVector2D::Distance(StartPos, EndPos);
				if (DisOfSeg > 3)
				{
					FVector2D DirVec = EndPos - StartPos;
					DirVec = DirVec.GetSafeNormal();
					DirVec = DirVec.GetRotated(90);
					FVector2D SegMiddle = 0.5*(StartPos + EndPos);
					FVector2D TestP1 = SegMiddle + 8 * DirVec;
					FVector2D TestP2 = SegMiddle - 8 * DirVec;

					bool P1InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TestP1);
					bool P2InOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(RegionPoints, TestP2);
					if ((P1InOrOnBoundary) || (P2InOrOnBoundary))
					{
						SkirtingMeshComps.Add(SkirtingAreaMeshComps[K]);
					}
					break;
				}

			}
		}

	}
	return SkirtingMeshComps;
}

void ASkirtingActor::BeginPlay()
{
	Super::BeginPlay();
}

ASkirtingActor::ASkirtingActor()
{

}

void ASkirtingSystem::RemoveDecolateLinesOfSomeRegions(TArray<FBoundaryPsOfRegion> RegionsToRemove)
{
	//UE_LOG(LogTemp, Warning, TEXT("void ASkirtingSystem::RemoveRegionNodes"));
	//清除已存在路径
	auto DestroyComponents = [](TArray<USkirtingMeshComponent*> &Components)
	{
		for (auto &Comp : Components)
		{
			if (Comp != nullptr)
			{
				Comp->GetOwner()->Destroy();
			}
		}
		Components.Empty();
	};


	TArray<USkirtingMeshComponent*> ToRemoveMeshCompts;
	TArray< USkirtingMeshComponent*> ToRemoveAreaCompts;
	TArray<USkirtingMeshComponent*> ToRemoveTopMeshCompts;
	TArray< USkirtingMeshComponent*> ToRemoveTopAreaCompts;

	for (int _CurRegion = 0; _CurRegion < RegionsToRemove.Num(); ++_CurRegion)
	{

		TArray<FVector2D> AreaPath = RegionsToRemove[_CurRegion].BoundaryPs;

		//判断类型


		for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
		{
			if (SkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingEndCorner.Position);
					if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
					{
						ToRemoveMeshCompts.Add(SkirtingMeshComponents[I]);
						break;
					}

				}

			}

		}
		for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
		{
			if (SkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingEndCorner.Position);
					if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
					{
						ToRemoveAreaCompts.Add(SkirtingAreaMeshComps[K]);
						break;
					}
				}
			}

		}





		for (int32 I = 0; I < TopSkirtingMeshComponents.Num(); I++)
		{
			if (TopSkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingStartCorner.Position);
					bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[J].SkirtingEndCorner.Position);
					if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
					{
						ToRemoveTopMeshCompts.Add(TopSkirtingMeshComponents[I]);
						break;
					}

				}
			}

		}
		for (int32 K = 0; K < TopSkirtingAreaMeshComps.Num(); K++)
		{
			if (TopSkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				for (int32 L = 0; L < TempSegNodes.Num(); L++)
				{
					bool StartInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingStartCorner.Position);
					bool EndInOrOnBoundary = FPolygonAlg::JudgePointOnOrInPolygon(AreaPath, TempSegNodes[L].SkirtingEndCorner.Position);
					if ((StartInOrOnBoundary) || (EndInOrOnBoundary))
					{
						ToRemoveTopAreaCompts.Add(TopSkirtingAreaMeshComps[K]);
						break;
					}

				}
			}

		}


	}


	DestroyComponents(ToRemoveMeshCompts);
	DestroyComponents(ToRemoveAreaCompts);
	for (int32 Index = SkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
	{
		if (SkirtingMeshComponents[Index] == nullptr)
		{
			SkirtingMeshComponents.RemoveAt(Index);
		}
	}
	for (int32 Index = SkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
	{
		if (SkirtingAreaMeshComps[Index] == nullptr)
		{
			SkirtingAreaMeshComps.RemoveAt(Index);
		}
	}

	DestroyComponents(ToRemoveTopMeshCompts);
	DestroyComponents(ToRemoveTopAreaCompts);

	for (int32 Index = TopSkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
	{
		if (TopSkirtingMeshComponents[Index] == nullptr)
		{
			TopSkirtingMeshComponents.RemoveAt(Index);
		}
	}
	for (int32 Index = TopSkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
	{
		if (TopSkirtingAreaMeshComps[Index] == nullptr)
		{
			TopSkirtingAreaMeshComps.RemoveAt(Index);
		}
	}

}

FString ASkirtingActor::GetSkirtingNodeUuid()
{
	FString uuid;
	TArray<UActorComponent*> SkirtingMesmComs = this->GetComponentsByClass(USkirtingMeshComponent::StaticClass());
	if (SkirtingMesmComs.Num() > 0)
	{
		USkirtingMeshComponent *SkirtingMesmCom = Cast<USkirtingMeshComponent>(SkirtingMesmComs[0]);
		FSkirtingNode node = SkirtingMesmCom->GetSkirtingSaveNode();
		uuid = node.Uuid;
	}
	return uuid;
}

void ASkirtingSystem::GetAllPointsAroundComponents(TArray<FVector2D>&CollectComponentNodes)
{
	TArray<AActor*> Components;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AComponentManagerActor::StaticClass(), Components);
	for (int I = 0; I < Components.Num(); ++I)
	{
		AComponentManagerActor * _TempCom = Cast<AComponentManagerActor>(Components[I]);
		if (_TempCom)
		{
			TArray<FVector2D> Nodes;
			bool GetSuccess = _TempCom->GetAllHouseComponent(Nodes);
			if (GetSuccess)
			{
				for (int _CurI = 0; _CurI < Nodes.Num(); ++_CurI)
				{
					CollectComponentNodes.Add(Nodes[_CurI]);
				}
			}
		}
	}
}

void ASkirtingSystem::GetComponentPointsOfLocalRegion(const TArray<FVector2D> LocalRegion, TArray<FVector2D>&CollectComponentNodes)
{
	TArray<AActor*> Components;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AComponentManagerActor::StaticClass(), Components);
	for (int I = 0; I < Components.Num(); ++I)
	{
		AComponentManagerActor * _TempCom = Cast<AComponentManagerActor>(Components[I]);
		if (_TempCom)
		{
			TArray<FVector2D> Nodes;
			bool GetSuccess = _TempCom->GetAllHouseComponent(Nodes);
			if (GetSuccess)
			{
				for (int _CurI = 0; _CurI < Nodes.Num(); ++_CurI)
				{
					bool IsInLocalRegion = FPolygonAlg::JudgePointOnOrInPolygon(LocalRegion, Nodes[_CurI]);
					if (IsInLocalRegion)
					{
						CollectComponentNodes.Add(Nodes[_CurI]);
					}
				}
			}
		}
	}
}

void ASkirtingSystem::UpdateDrawingLinesOfPillars(ESkirtingType InSkirtingType)
{
	auto DestroyComponents = [](TArray<USkirtingMeshComponent*> &Components)
	{
		for (auto &Comp : Components)
		{
			if (Comp != nullptr)
			{
				Comp->GetOwner()->Destroy();
			}
		}
		Components.Empty();
	};

	TArray<USkirtingMeshComponent*> CollectLines;
	GetDecorateLinesAroundPillars(CollectLines, InSkirtingType);
	for (int I = 0; I < CollectLines.Num(); ++I)
	{
		TArray<FVector2D> TempContour;
		FSkirtingNode TempSkirtingNode = CollectLines[I]->GetSkirtingSaveNode();
		TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
		for (int J = 0; J < TempSegNodes.Num(); ++J)
		{
			TempContour.AddUnique(TempSegNodes[J].SkirtingEndCorner.Position);
		}

		if (IsPolyClockWise(TempContour))
		{
			ChangeClockwise(TempContour);
		}

		FSkirtingNode SkirtingNode;
		SkirtingNode.InitFromAreaPath(TempContour, InSkirtingType);
		SkirtingNode.SrcType = ESkirtingSrcType::SrcFromSyncPath;
		SetSkiringDefaultData(SkirtingNode, true);
		CreateSkirtingComponentByNodes(SkirtingNode, true);
	}

	DestroyComponents(CollectLines);
	for (int32 Index = SkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
	{
		if (SkirtingMeshComponents[Index] == nullptr)
		{
			SkirtingMeshComponents.RemoveAt(Index);
		}
	}
	for (int32 Index = SkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
	{
		if (SkirtingAreaMeshComps[Index] == nullptr)
		{
			SkirtingAreaMeshComps.RemoveAt(Index);
		}
	}

	for (int32 Index = TopSkirtingMeshComponents.Num() - 1; Index >= 0; --Index)
	{
		if (TopSkirtingMeshComponents[Index] == nullptr)
		{
			TopSkirtingMeshComponents.RemoveAt(Index);
		}
	}
	for (int32 Index = TopSkirtingAreaMeshComps.Num() - 1; Index >= 0; --Index)
	{
		if (TopSkirtingAreaMeshComps[Index] == nullptr)
		{
			TopSkirtingAreaMeshComps.RemoveAt(Index);
		}
	}


}

void ASkirtingSystem::GetDecorateLinesAroundPillars(TArray<USkirtingMeshComponent*> &CollectLines, ESkirtingType InSkirtingType)
{
	TArray<AActor*> Components;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AComponentManagerActor::StaticClass(), Components);
	TArray<TArray<FVector2D>> TotalNodes;
	for (int I = 0; I < Components.Num(); ++I)
	{
		AComponentManagerActor * _TempCom = Cast<AComponentManagerActor>(Components[I]);
		if (_TempCom)
		{
			TArray<FVector2D> Nodes;
			FComponenetInfo TempInfo = _TempCom->GetComponenetInfo();
			EComponenetType TempType = TempInfo.ComponenetType;
			if (TempType == EComponenetType::_Pillar)
			{
				bool GetSuccess = _TempCom->GetAllPillars(Nodes);
				if (GetSuccess)
				{
					TotalNodes.Add(Nodes);
				}
			}
		}
	}

	switch (InSkirtingType)
	{
	case ESkirtingType::SkirtingLine:
	{
		for (int32 I = 0; I < SkirtingMeshComponents.Num(); I++)
		{
			if (SkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				TArray<FVector2D> TempBounds;
				float TempLength = 0.0;
				float SegLength = 0.0;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					TempBounds.AddUnique(TempSegNodes[J].SkirtingStartCorner.Position);
					TempBounds.AddUnique(TempSegNodes[J].SkirtingEndCorner.Position);
					SegLength = SegLength + FVector2D::Distance(TempSegNodes[J].SkirtingStartCorner.Position, TempSegNodes[J].SkirtingEndCorner.Position);
				}
				if ((TempBounds.Num() >= 4) && (SegLength <= 400))
				{
					for (int _CurP = 0; _CurP < TotalNodes.Num(); ++_CurP)
					{
						TArray<FVector2D> TempPillar = TotalNodes[_CurP];
						FVector2D TempCenter(0.0, 0.0);
						int NextIndex = 0;
						for (int _CurIndex = 0; _CurIndex < TempPillar.Num(); ++_CurIndex)
						{
							TempCenter = TempCenter + TempPillar[_CurIndex];
							if (_CurIndex == TempPillar.Num() - 1)
							{
								NextIndex = 0;
							}
							else
							{
								NextIndex = _CurIndex + 1;
							}
							TempLength = TempLength + FVector2D::Distance(TempPillar[_CurIndex], TempPillar[NextIndex]);
						}
						TempCenter = TempCenter / TempPillar.Num();
						bool IsInRegion = FPolygonAlg::JudgePointInPolygon(TempBounds, TempCenter);
						if ((IsInRegion) && (TempLength <= 400))
						{
							CollectLines.AddUnique(SkirtingMeshComponents[I]);
							TempSkirtingNode.HasComponent = true;
						}
					}

				}
			}

		}


		for (int32 K = 0; K < SkirtingAreaMeshComps.Num(); K++)
		{
			if (SkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = SkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				TArray<FVector2D> TempBounds;
				float TempLength = 0.0;
				float SegLength = 0.0;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					TempBounds.AddUnique(TempSegNodes[J].SkirtingStartCorner.Position);
					TempBounds.AddUnique(TempSegNodes[J].SkirtingEndCorner.Position);
					SegLength = SegLength + FVector2D::Distance(TempSegNodes[J].SkirtingStartCorner.Position, TempSegNodes[J].SkirtingEndCorner.Position);
				}
				if ((TempBounds.Num() >= 4) && (SegLength <= 400))
				{
					for (int _CurP = 0; _CurP < TotalNodes.Num(); ++_CurP)
					{
						TArray<FVector2D> TempPillar = TotalNodes[_CurP];
						FVector2D TempCenter(0.0, 0.0);
						int NextIndex = 0;
						for (int _CurIndex = 0; _CurIndex < TempPillar.Num(); ++_CurIndex)
						{
							TempCenter = TempCenter + TempPillar[_CurIndex];
							if (_CurIndex == TempPillar.Num() - 1)
							{
								NextIndex = 0;
							}
							else
							{
								NextIndex = _CurIndex + 1;
							}
							TempLength = TempLength + FVector2D::Distance(TempPillar[_CurIndex], TempPillar[NextIndex]);
						}
						TempCenter = TempCenter / TempPillar.Num();
						bool IsInRegion = FPolygonAlg::JudgePointInPolygon(TempBounds, TempCenter);
						if ((IsInRegion) && (TempLength <= 400))
						{
							CollectLines.AddUnique(SkirtingAreaMeshComps[K]);
							TempSkirtingNode.HasComponent = true;
						}
					}
				}

			}
		}




	}
	break;
	case ESkirtingType::SkirtingTopLine:
	{
		for (int32 I = 0; I < TopSkirtingMeshComponents.Num(); I++)
		{
			if (TopSkirtingMeshComponents[I] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingMeshComponents[I]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				TArray<FVector2D> TempBounds;
				float TempLength = 0.0;
				float SegLength = 0.0;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					TempBounds.AddUnique(TempSegNodes[J].SkirtingStartCorner.Position);
					TempBounds.AddUnique(TempSegNodes[J].SkirtingEndCorner.Position);
					SegLength = SegLength + FVector2D::Distance(TempSegNodes[J].SkirtingStartCorner.Position, TempSegNodes[J].SkirtingEndCorner.Position);
				}

				if ((TempBounds.Num() >= 4) && (SegLength <= 400))
				{
					for (int _CurP = 0; _CurP < TotalNodes.Num(); ++_CurP)
					{
						TArray<FVector2D> TempPillar = TotalNodes[_CurP];
						FVector2D TempCenter(0.0, 0.0);
						int NextIndex = 0;
						for (int _CurIndex = 0; _CurIndex < TempPillar.Num(); ++_CurIndex)
						{
							TempCenter = TempCenter + TempPillar[_CurIndex];
							if (_CurIndex == TempPillar.Num() - 1)
							{
								NextIndex = 0;
							}
							else
							{
								NextIndex = _CurIndex + 1;
							}
							TempLength = TempLength + FVector2D::Distance(TempPillar[_CurIndex], TempPillar[NextIndex]);
						}
						TempCenter = TempCenter / TempPillar.Num();
						bool IsInRegion = FPolygonAlg::JudgePointInPolygon(TempBounds, TempCenter);
						if ((IsInRegion) && (TempLength <= 400))
						{
							CollectLines.AddUnique(TopSkirtingMeshComponents[I]);
							TempSkirtingNode.HasComponent = true;
						}
					}

				}
			}

		}

		for (int32 K = 0; K < TopSkirtingAreaMeshComps.Num(); K++)
		{
			if (TopSkirtingAreaMeshComps[K] != NULL)
			{
				FSkirtingNode TempSkirtingNode = TopSkirtingAreaMeshComps[K]->GetSkirtingSaveNode();
				TArray<FSkirtingSegNode>  TempSegNodes = TempSkirtingNode.SkirtingSegments;
				TArray<FVector2D> TempBounds;
				float TempLength = 0.0;
				float SegLength = 0.0;
				for (int32 J = 0; J < TempSegNodes.Num(); J++)
				{
					TempBounds.AddUnique(TempSegNodes[J].SkirtingStartCorner.Position);
					TempBounds.AddUnique(TempSegNodes[J].SkirtingEndCorner.Position);
					SegLength = SegLength + FVector2D::Distance(TempSegNodes[J].SkirtingStartCorner.Position, TempSegNodes[J].SkirtingEndCorner.Position);
				}
				if ((TempBounds.Num() >= 4) && (SegLength <= 400))
				{
					for (int _CurP = 0; _CurP < TotalNodes.Num(); ++_CurP)
					{
						TArray<FVector2D> TempPillar = TotalNodes[_CurP];
						FVector2D TempCenter(0.0, 0.0);
						int NextIndex = 0;
						for (int _CurIndex = 0; _CurIndex < TempPillar.Num(); ++_CurIndex)
						{
							TempCenter = TempCenter + TempPillar[_CurIndex];
							if (_CurIndex == TempPillar.Num() - 1)
							{
								NextIndex = 0;
							}
							else
							{
								NextIndex = _CurIndex + 1;
							}
							TempLength = TempLength + FVector2D::Distance(TempPillar[_CurIndex], TempPillar[NextIndex]);
						}
						TempCenter = TempCenter / TempPillar.Num();
						bool IsInRegion = FPolygonAlg::JudgePointInPolygon(TempBounds, TempCenter);
						if ((IsInRegion) && (TempLength <= 400))
						{
							CollectLines.AddUnique(TopSkirtingAreaMeshComps[K]);
							TempSkirtingNode.HasComponent = true;
						}
					}

				}
			}

		}
	}
	break;
	default:
		break;
	}
}