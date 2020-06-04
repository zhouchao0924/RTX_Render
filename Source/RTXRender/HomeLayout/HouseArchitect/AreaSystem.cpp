// Copyright© 2017 ihomefnt All Rights Reserved.


#include "AreaSystem.h"
#include "WallBuildSystem.h"
#include "ModelSystem.h"
#include "EditorGameInstance.h"
#include "CGALWrapper/CgDataConvUtility.h"
#include "CGALWrapper/PolygonAlg.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include "HomeLayout/SceneEntity/RoomActor.h"
#include "Algo/Reverse.h"
#include "HomeLayout/HouseArchitect/PlaneHousePicture.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRSolidWallAdapter.h"
#include "Data/Adapter/DRHoleAdapter.h"
#include "Data/Adapter/DRAreaAdapter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
AAreaSystem::AAreaSystem()
{
	
}

void AAreaSystem::BeginPlay()
{
	Super::BeginPlay();
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(MyWorld->GetGameInstance());
		if (GameInst)
		{
			WallSystem = GameInst->WallBuildSystem;
		}
	}
}

void AAreaSystem::ReArrangeRegion()
{
	RegionsWithRCInWorld.Empty();
	for (auto RegionInW : RegionsInWorld)
	{
		URegionObject* RegionObj = NewObject<URegionObject>(RegionInW);
		RegionObj->RegionActor = RegionInW;
		RegionsWithRCInWorld.Add(RegionObj);
		ArrangeWalls2Region(RegionObj);
		ArrangeSkirtingLines2Region(RegionObj);
	}
}

FRegionCollection AAreaSystem::CalculateRegionCollection(ARoomActor *RegionInW, bool bFindByCache)
{

	auto RegionWithRObj = CacheRoomRelationObj(RegionInW, bFindByCache);

	return RegionWithRObj->RegionCollection;

}

TArray<FSkirtingCollectionHandle> AAreaSystem::GetSkirtingLineCollection(const FRegionCollection &InRegionC)
{
	TArray<FSkirtingCollectionHandle> SkirtingLineC = InRegionC.SkirtingCollections.FilterByPredicate(
		[](const FSkirtingCollectionHandle &SkirtingCollectionHandle)
	{
		return SkirtingCollectionHandle.SkirtingType == ESkirtingType::SkirtingLine;
	});

	return SkirtingLineC;
}

TArray<FSkirtingCollectionHandle> AAreaSystem::GetSkirtingTopLineCollection(const FRegionCollection &InRegionC)
{
	TArray<FSkirtingCollectionHandle> SkirtingTopLineC = InRegionC.SkirtingCollections.FilterByPredicate(
	[](const FSkirtingCollectionHandle &SkirtingCollectionHandle)
	{
		return SkirtingCollectionHandle.SkirtingType == ESkirtingType::SkirtingTopLine;
	});

	return SkirtingTopLineC;
}

TArray<AWall_Boolean_Base*> AAreaSystem::GetRelatedOpeningsOfRoom(ARoomActor *RegionInW, bool bFindByCache)
{

	auto RegionWithRObj = CacheRoomRelationObj(RegionInW, bFindByCache);

	TArray<AWall_Boolean_Base*> OpeningsOfRoom;
	TArray<UWallSurfaceObject*> WallsRelated2ThisRegion;
	WallsRelated2ThisRegion.Append(RegionWithRObj->WallsCcb);
	WallsRelated2ThisRegion.Append(RegionWithRObj->WallsInThisRegion);
	for (auto &WallRelate2Region : WallsRelated2ThisRegion)
	{
		for (const auto &WallSeg : WallRelate2Region->WallsLayOnThisSeg)
		{
			OpeningsOfRoom.Append(WallSeg->OpeningsOnThisWallSurface);
		}
	}

	for (int i = 0;i < OpeningsOfRoom.Num();)
	{
		if (nullptr == OpeningsOfRoom[i])
		{
			OpeningsOfRoom.RemoveAt(i);
			UE_LOG(LogTemp, Warning, TEXT(" OpeningsOfRoom Null"));
		}
		else
		{
			++i;
		}

	}

	return OpeningsOfRoom;
}



//auto lab = [=](AWall_Boolean_Base *OpeningActor)
//{
//	if (OpeningActor)
//	{
//		FOpeningSimpleAttr SimpleAttr = OpeningActor->GetOpeningSimpleAttr();
//
//		return SimpleAttr.Type == EOpeningType::BayWindow;
//	}
//	UE_LOG(LogTemp, Warning, TEXT(" OpeningActor->GetOpeningSimpleAttr()[106] is Null"));
//	return false;
//};

TArray<AFurnitureModelActor*> AAreaSystem::CollectRelatedModelsOfRoom(ARoomActor *RegionInW, bool bFindByCache /*= false*/)
{

	TArray<AFurnitureModelActor*> ModelsInRoom;
	if (RegionInW != nullptr)
	{
		FRoomPath RoomPath = RegionInW->GetRoomPath();
		TArray<FVector2D> RoomPathPnts = MoveTemp(RoomPath.InnerRoomPath);
		TArray<AWall_Boolean_Base*> WindowsInRoom;

		TArray<AWall_Boolean_Base*> UpdateWindowsInRoom;
		UPlaneHousePicture::GetAllWindowsOnWalls(GetWorld(), WindowsInRoom);
		for (int I = 0; I < WindowsInRoom.Num(); ++I)
		{
			FOpeningSimpleAttr TempOpen = WindowsInRoom[I]->GetOpeningSimpleAttr();
			FVector2D TempLoc = TempOpen.Location;
			bool FindBayWindow = FPolygonAlg::GetDisOfPointToPolygon(TempLoc, RoomPathPnts);
			if (FindBayWindow)
			{
				UpdateWindowsInRoom.Add(WindowsInRoom[I]);
				FindBayWindow = false;
			}
		}


		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
		const auto& ModelsInWorld = GameInst->ModelSystem->GetModelsInWorld();

		for (const auto ModelInW : ModelsInWorld)
		{
			if (IsTheModelInTheRoomImpl(ModelInW, RegionInW, UpdateWindowsInRoom))
			{
				ModelsInRoom.Add(ModelInW);
			}
		}
	}
	return ModelsInRoom;
}


bool  AAreaSystem::SeperateModelsIntoRooms(TArray<ARoomActor*>RegionsInW, TArray<FModelsInRegion>&ModelsOfHouse,bool NoStored)
{

	if (NoStored)
	{
		if (RegionsInW.Num() == 0)
		{
			return false;
		}
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
		auto& ModelsInWorld = GameInst->ModelSystem->GetModelsInWorld();
		auto &Local_Models = ModelsInWorld;

		TArray<AFurnitureModelActor*> HasCollectedModels;
		TArray<ARoomActor*> LivingRooms;
		TArray<ARoomActor*> DiningRooms;
		const FString Str1 = TEXT("客厅");
		const FString Str2 = TEXT("餐厅");
		for (int _CurRoom = 0; _CurRoom < RegionsInW.Num(); ++_CurRoom)
		{
			FModelsInRegion TempFModelsInRegion;
			ARoomActor* TempRoomActor = RegionsInW[_CurRoom];
			TempFModelsInRegion.Region = TempRoomActor;
			FString TempUseName = TempRoomActor->RoomUseName;

			TArray<AFurnitureModelActor*> TempModelsInRoom;
			if ((!TempUseName.Contains(Str1)) && (!TempUseName.Contains(Str2)))
			{
				if (RegionsInW[_CurRoom] != nullptr)
				{
					FRoomPath RoomPath = RegionsInW[_CurRoom]->GetRoomPath();
					TArray<FVector2D> RoomPathPnts = MoveTemp(RoomPath.InnerRoomPath);
					TArray<AWall_Boolean_Base*> BayWindowsInRoom;

					TArray<AWall_Boolean_Base*> UpdateBayWindowsInRoom;
					UPlaneHousePicture::GetBayYeWindowsOnWalls(GetWorld(), BayWindowsInRoom);
					for (int I = 0; I < BayWindowsInRoom.Num(); ++I)
					{
						FOpeningSimpleAttr TempOpen = BayWindowsInRoom[I]->GetOpeningSimpleAttr();
						FVector2D TempLoc = TempOpen.Location;
						bool FindBayWindow = FPolygonAlg::GetDisOfPointToPolygon(TempLoc, RoomPathPnts);
						if (FindBayWindow)
						{
							UpdateBayWindowsInRoom.Add(BayWindowsInRoom[I]);
							FindBayWindow = false;
						}
					}
					for (const auto ModelInW : ModelsInWorld)
					{
						if (!HasCollectedModels.Contains(ModelInW))
						{
							if (IsTheModelInTheRoomImpl(ModelInW, TempRoomActor, UpdateBayWindowsInRoom))
							{
								TempModelsInRoom.Add(ModelInW);
								HasCollectedModels.Add(ModelInW);
							}
						}
					}
					TempFModelsInRegion.ModelsInRegion = TempModelsInRoom;
					ModelsOfHouse.Add(TempFModelsInRegion);
				}
			}
			else if (TempUseName.Contains(Str1))
			{
				LivingRooms.Add(TempRoomActor);

			}
			else if (TempUseName.Contains(Str2))
			{
				DiningRooms.Add(TempRoomActor);
			}
		}
		int NumOfLivingRooms = LivingRooms.Num();
		int NumOfDiningRooms = DiningRooms.Num();
		if ((NumOfLivingRooms == 1) && (NumOfDiningRooms == 1))
		{
			if ((LivingRooms[0] != nullptr) && (DiningRooms[0] != nullptr))
			{
				TArray<FVector2D> UpdateLivingRegionPs, UpdateDiningRegionPs;
				FRoomPath LivingRoomPath = LivingRooms[0]->GetRoomPath();
				TArray<FVector2D> LivingRegionPs = MoveTemp(LivingRoomPath.InnerRoomPath);
				FRoomPath DiningRoomPath = DiningRooms[0]->GetRoomPath();
				TArray<FVector2D> DiningRegionPs = MoveTemp(DiningRoomPath.InnerRoomPath);

				bool MergeLivingPs = FPolygonAlg::MergeCollinearPoints(LivingRegionPs, UpdateLivingRegionPs);
				bool MergeDiningPs = FPolygonAlg::MergeCollinearPoints(DiningRegionPs, UpdateDiningRegionPs);

				bool IfHasCommonPoints = false;
				bool JudgeIfInterSect = false;
				JudgeIfInterSect = FPolygonAlg::JudgePolygonInterSect(UpdateLivingRegionPs, UpdateDiningRegionPs);
				IfHasCommonPoints = FPolygonAlg::JudgeHasClosePoint(UpdateLivingRegionPs, UpdateDiningRegionPs, 5.0);

				const float PointToSegDisLimit = 8.0;
				if ((!IfHasCommonPoints) && (!JudgeIfInterSect))
				{
					for (int I = 0; I <= 1; ++I)
					{
						FModelsInRegion TempFModelsInRegion;
						TArray<AFurnitureModelActor*> TempModelsInRoom;

						TArray<AWall_Boolean_Base*> BayWindowsInRoom;
						TArray<AWall_Boolean_Base*> UpdateBayWindowsInRoom;
						UPlaneHousePicture::GetBayYeWindowsOnWalls(GetWorld(), BayWindowsInRoom);
						if (I == 0)
						{
							TempFModelsInRegion.Region = LivingRooms[0];
							for (int J = 0; J < BayWindowsInRoom.Num(); ++J)
							{
								FOpeningSimpleAttr TempOpen = BayWindowsInRoom[J]->GetOpeningSimpleAttr();
								FVector2D TempLoc = TempOpen.Location;
								bool FindBayWindow = FPolygonAlg::GetDisOfPointToPolygon(TempLoc, UpdateLivingRegionPs);
								if (FindBayWindow)
								{
									UpdateBayWindowsInRoom.Add(BayWindowsInRoom[I]);
									FindBayWindow = false;
								}
							}
							for (const auto ModelInW : ModelsInWorld)
							{
								if (!HasCollectedModels.Contains(ModelInW))
								{

									if (IsTheModelInTheRoomImpl(ModelInW, LivingRooms[0], UpdateBayWindowsInRoom))
									{
										TempModelsInRoom.Add(ModelInW);
										HasCollectedModels.Add(ModelInW);
									}
								}
							}
							TempFModelsInRegion.ModelsInRegion = TempModelsInRoom;
							ModelsOfHouse.Add(TempFModelsInRegion);

						}
						else
						{
							TempFModelsInRegion.Region = DiningRooms[0];
							for (int J = 0; J < BayWindowsInRoom.Num(); ++J)
							{
								FOpeningSimpleAttr TempOpen = BayWindowsInRoom[J]->GetOpeningSimpleAttr();
								FVector2D TempLoc = TempOpen.Location;
								bool FindBayWindow = FPolygonAlg::GetDisOfPointToPolygon(TempLoc, UpdateDiningRegionPs);
								if (FindBayWindow)
								{
									UpdateBayWindowsInRoom.Add(BayWindowsInRoom[J]);
									FindBayWindow = false;
								}
							}
							for (const auto ModelInW : ModelsInWorld)
							{

								if (!HasCollectedModels.Contains(ModelInW))
								{
									if (IsTheModelInTheRoomImpl(ModelInW, DiningRooms[0], UpdateBayWindowsInRoom))
									{
										TempModelsInRoom.Add(ModelInW);
										HasCollectedModels.Add(ModelInW);
									}
								}
							}
							TempFModelsInRegion.ModelsInRegion = TempModelsInRoom;
							ModelsOfHouse.Add(TempFModelsInRegion);
						}
					}
				}
				else
				{
					FModelsInRegion TempFModelsInLivingRoom, TempFModelsInDiningRoom;
					TArray<AFurnitureModelActor*> TempModelsInLiving, TempModelsInDining;

					for (const auto ModelInW : ModelsInWorld)
					{
						bool TestPointInLiving = false;
						bool TestPointInDining = false;
						if (!HasCollectedModels.Contains(ModelInW))
						{
							if (ModelInW != nullptr)
							{

								const FBox ModelBoundingBox = ModelInW->GetComponentsBoundingBox(true);
								FVector2D ModelCenter = FVector2D(ModelBoundingBox.GetCenter());

								TestPointInLiving = FPolygonAlg::JudgePointOnOrInPolygon(UpdateLivingRegionPs, ModelCenter);
								TestPointInDining = FPolygonAlg::JudgePointOnOrInPolygon(UpdateDiningRegionPs, ModelCenter);

								if ((TestPointInLiving) && (!TestPointInDining))
								{
									TempModelsInLiving.Add(ModelInW);
									HasCollectedModels.Add(ModelInW);
								}
								else if ((TestPointInDining) && (!TestPointInLiving))
								{
									TempModelsInDining.Add(ModelInW);
									HasCollectedModels.Add(ModelInW);
								}
								else if ((TestPointInLiving) && (TestPointInDining))
								{
									TempModelsInLiving.Add(ModelInW);
									HasCollectedModels.Add(ModelInW);
								}

								if ((!TestPointInLiving) && (!TestPointInDining))
								{
									TArray<int> ToDelLivingEdgeIndexs;
									int32 NextIndex = 0;
									for (int32 Index = 0; Index < UpdateLivingRegionPs.Num(); ++Index)
									{
										if (Index == UpdateLivingRegionPs.Num() - 1)
										{
											NextIndex = 0;
										}
										else
										{
											NextIndex = Index + 1;
										}
										FVector2D EdgeMiddlePoint = 0.5*(UpdateLivingRegionPs[Index] + UpdateLivingRegionPs[NextIndex]);
										FVector2D EdgeVec = UpdateLivingRegionPs[NextIndex] - UpdateLivingRegionPs[Index];
										EdgeVec = EdgeVec.GetSafeNormal();
										FVector2D EdgeRotate = EdgeVec.GetRotated(90);
										FVector2D TryPoint1 = EdgeMiddlePoint + 10 * EdgeRotate;
										FVector2D TryPoint2 = EdgeMiddlePoint - 10 * EdgeRotate;
										bool JudgeP1 = FPolygonAlg::JudgePointOnOrInPolygon(UpdateDiningRegionPs, TryPoint1);
										bool JudgeP2 = FPolygonAlg::JudgePointOnOrInPolygon(UpdateDiningRegionPs, TryPoint2);
										if (JudgeP1 || JudgeP2)
										{
											ToDelLivingEdgeIndexs.AddUnique(Index);
										}
									}

									int32 NextLivingEIndex = 0;
									for (int32 Index = 0; Index < UpdateLivingRegionPs.Num(); Index++)
									{
										if (!ToDelLivingEdgeIndexs.Contains(Index))
										{
											if (Index == UpdateLivingRegionPs.Num() - 1)
											{
												NextLivingEIndex = 0;
											}
											else
											{
												NextLivingEIndex = Index + 1;
											}
											float PointToSeg = FMath::PointDistToSegment(FVector(ModelCenter, 0), FVector(UpdateLivingRegionPs[Index], 0), FVector(UpdateLivingRegionPs[NextLivingEIndex], 0));
											if (PointToSeg <= PointToSegDisLimit)
											{
												TestPointInLiving = true;
												TempModelsInLiving.Add(ModelInW);
												HasCollectedModels.Add(ModelInW);
												break;
											}
										}
									}


									if (!TestPointInLiving)
									{
										TArray<int> ToDelDiningEdgeIndexs;
										int32 NextDiningIndex = 0;
										for (int32 Index = 0; Index < UpdateDiningRegionPs.Num(); ++Index)
										{
											if (Index == UpdateDiningRegionPs.Num() - 1)
											{
												NextDiningIndex = 0;
											}
											else
											{
												NextDiningIndex = Index + 1;
											}
											FVector2D EdgeDinMiddlePoint = 0.5*(UpdateDiningRegionPs[Index] + UpdateDiningRegionPs[NextDiningIndex]);
											FVector2D EdgeDinVec = UpdateDiningRegionPs[NextDiningIndex] - UpdateDiningRegionPs[Index];
											EdgeDinVec = EdgeDinVec.GetSafeNormal();
											FVector2D EdgeDinRotate = EdgeDinVec.GetRotated(90);
											FVector2D TryDinPoint1 = EdgeDinMiddlePoint + 10 * EdgeDinRotate;
											FVector2D TryDinPoint2 = EdgeDinMiddlePoint - 10 * EdgeDinRotate;
											if ((FPolygonAlg::JudgePointOnOrInPolygon(UpdateLivingRegionPs, TryDinPoint1)) || (FPolygonAlg::JudgePointOnOrInPolygon(UpdateLivingRegionPs, TryDinPoint2)))
											{
												ToDelDiningEdgeIndexs.AddUnique(Index);
											}
										}
										int32 NextEIndex = 0;
										for (int32 Index = 0; Index < UpdateDiningRegionPs.Num(); Index++)
										{
											if (!ToDelDiningEdgeIndexs.Contains(Index))
											{
												if (Index == UpdateDiningRegionPs.Num() - 1)
												{
													NextEIndex = 0;
												}
												else
												{
													NextEIndex = Index + 1;
												}
												float PointToSeg = FMath::PointDistToSegment(FVector(ModelCenter, 0), FVector(UpdateDiningRegionPs[Index], 0), FVector(UpdateDiningRegionPs[NextEIndex], 0));
												if (PointToSeg <= PointToSegDisLimit)
												{
													TestPointInDining = true;
													TempModelsInDining.Add(ModelInW);
													HasCollectedModels.Add(ModelInW);
													break;
												}
											}
										}
									}

								}
							}
						}
					}

					TempFModelsInLivingRoom.Region = LivingRooms[0];
					TempFModelsInLivingRoom.ModelsInRegion = TempModelsInLiving;
					ModelsOfHouse.Add(TempFModelsInLivingRoom);
					TempFModelsInDiningRoom.Region = DiningRooms[0];
					TempFModelsInDiningRoom.ModelsInRegion = TempModelsInDining;
					ModelsOfHouse.Add(TempFModelsInDiningRoom);
				}
			}
		}
		else
		{
			for (int _CurI = 0; _CurI < NumOfLivingRooms; ++_CurI)
			{
				ARoomActor* TempRoomActor = LivingRooms[_CurI];
				if (TempRoomActor != nullptr)
				{
					FModelsInRegion TempFModelsInRegion;
					TempFModelsInRegion.Region = TempRoomActor;
					TArray<AFurnitureModelActor*> TempModelsInRoom;
					FRoomPath RoomPath = TempRoomActor->GetRoomPath();
					TArray<FVector2D> RoomPathPnts = MoveTemp(RoomPath.InnerRoomPath);

					TArray<AWall_Boolean_Base*> BayWindowsInRoom;

					TArray<AWall_Boolean_Base*> UpdateBayWindowsInRoom;
					UPlaneHousePicture::GetBayYeWindowsOnWalls(GetWorld(), BayWindowsInRoom);
					for (int I = 0; I < BayWindowsInRoom.Num(); ++I)
					{
						FOpeningSimpleAttr TempOpen = BayWindowsInRoom[I]->GetOpeningSimpleAttr();
						FVector2D TempLoc = TempOpen.Location;
						bool FindBayWindow = FPolygonAlg::GetDisOfPointToPolygon(TempLoc, RoomPathPnts);
						if (FindBayWindow)
						{
							UpdateBayWindowsInRoom.Add(BayWindowsInRoom[I]);
							FindBayWindow = false;
						}
					}
					for (const auto ModelInW : ModelsInWorld)
					{
						if (!HasCollectedModels.Contains(ModelInW))
						{
							if (IsTheModelInTheRoomImpl(ModelInW, TempRoomActor, UpdateBayWindowsInRoom))
							{
								TempModelsInRoom.Add(ModelInW);
								HasCollectedModels.Add(ModelInW);
							}
						}
					}
					TempFModelsInRegion.ModelsInRegion = TempModelsInRoom;
					ModelsOfHouse.Add(TempFModelsInRegion);

				}
			}

			for (int _CurJ = 0; _CurJ < NumOfDiningRooms; ++_CurJ)
			{
				ARoomActor* TempRoomActor = DiningRooms[_CurJ];
				if (TempRoomActor != nullptr)
				{
					FModelsInRegion TempFModelsInRegion;
					TempFModelsInRegion.Region = TempRoomActor;
					TArray<AFurnitureModelActor*> TempModelsInRoom;
					FRoomPath RoomPath = TempRoomActor->GetRoomPath();
					TArray<FVector2D> RoomPathPnts = MoveTemp(RoomPath.InnerRoomPath);

					TArray<AWall_Boolean_Base*> BayWindowsInRoom;

					TArray<AWall_Boolean_Base*> UpdateBayWindowsInRoom;
					UPlaneHousePicture::GetBayYeWindowsOnWalls(GetWorld(), BayWindowsInRoom);
					for (int I = 0; I < BayWindowsInRoom.Num(); ++I)
					{
						FOpeningSimpleAttr TempOpen = BayWindowsInRoom[I]->GetOpeningSimpleAttr();
						FVector2D TempLoc = TempOpen.Location;
						bool FindBayWindow = FPolygonAlg::GetDisOfPointToPolygon(TempLoc, RoomPathPnts);
						if (FindBayWindow)
						{
							UpdateBayWindowsInRoom.Add(BayWindowsInRoom[I]);
							FindBayWindow = false;
						}
					}
					for (const auto ModelInW : ModelsInWorld)
					{
						if (!HasCollectedModels.Contains(ModelInW))
						{
							if (IsTheModelInTheRoomImpl(ModelInW, TempRoomActor, UpdateBayWindowsInRoom))
							{
								TempModelsInRoom.Add(ModelInW);
								HasCollectedModels.Add(ModelInW);
							}
						}
					}
					TempFModelsInRegion.ModelsInRegion = TempModelsInRoom;
					ModelsOfHouse.Add(TempFModelsInRegion);

				}
			}

		}
		TotalModelsOfHouse = ModelsOfHouse;
	}
	else
	{
	  ModelsOfHouse= TotalModelsOfHouse;
    }
	
	
	return true;
}





bool AAreaSystem::IsTheModelInTheRoom(AFurnitureModelActor *FuritureModel, ARoomActor *RegionInW, bool bFindByCache)
{
	if ((FuritureModel != nullptr) && (RegionInW != nullptr))
	{

		FRoomPath RoomPath = RegionInW->GetRoomPath();
		TArray<FVector2D> RoomPathPnts = MoveTemp(RoomPath.InnerRoomPath);
		TArray<AWall_Boolean_Base*> BayWindowsInRoom;

		TArray<AWall_Boolean_Base*> UpdateBayWindowsInRoom;
		UPlaneHousePicture::GetBayYeWindowsOnWalls(GetWorld(), BayWindowsInRoom);
		for (int I = 0; I < BayWindowsInRoom.Num(); ++I)
		{
			FOpeningSimpleAttr TempOpen = BayWindowsInRoom[I]->GetOpeningSimpleAttr();
			FVector2D TempLoc = TempOpen.Location;
			bool FindBayWindow = FPolygonAlg::GetDisOfPointToPolygon(TempLoc, RoomPathPnts);
			if (FindBayWindow)
			{
				UpdateBayWindowsInRoom.Add(BayWindowsInRoom[I]);
				FindBayWindow = false;
			}
		}

		return IsTheModelInTheRoomImpl(FuritureModel, RegionInW, UpdateBayWindowsInRoom);
	}
	else
	{
		return false;
	}
}

bool AAreaSystem::IsTheModelInTheRoomImpl(AFurnitureModelActor *FuritureModel, ARoomActor *RegionInW, const TArray<AWall_Boolean_Base*> &OpeningsInTheRoom)
{
	if ((FuritureModel != nullptr) && (RegionInW != nullptr))
	{
		int32 Local_ModelCategory(0);
		FuritureModel->GetModelCategory(Local_ModelCategory);
		if (752 == Local_ModelCategory || 751 == Local_ModelCategory)
		{
			UDRProjData* Local_ProjDataManager = UDRProjData::GetProjectDataManager(this);
			if (!Local_ProjDataManager)
				return false;

			for (const auto WallBooleanBase : OpeningsInTheRoom)
			{
				FOpeningSimpleAttr Local_OpenAttr = WallBooleanBase->GetOpeningSimpleAttr();
				if ((Local_OpenAttr.Type == EOpeningType::Window || Local_OpenAttr.Type == EOpeningType::BayWindow))
				{
					FVector2D Local_SillLocation = FVector2D(FuritureModel->GetActorLocation());
					FVector2D Local_WindowLocation = Local_OpenAttr.Location;
					float Local_Distance = FVector2D::Distance(Local_SillLocation, Local_WindowLocation);
					UE_LOG(LogTemp,Log,TEXT("Distance is %f"),Local_Distance);
					if (Local_Distance <= 50)
					{
						return true;
					}
				}
			}
		}
		
		const FBox ModelBoundingBox = FuritureModel->GetComponentsBoundingBox(true);
		FVector2D ModelCenter = FVector2D(ModelBoundingBox.GetCenter());

		return IsLocationInTheRoomImpl(ModelCenter, RegionInW, OpeningsInTheRoom);
		
	}
	else
	{
		return false;
	}
	
}

bool AAreaSystem::IsLocationInTheRoom(FVector2D Location, ARoomActor *RegionInW, bool bFindByCache)
{
	if (RegionInW != nullptr)
	{
		FRoomPath RoomPath = RegionInW->GetRoomPath();
		TArray<FVector2D> RoomPathPnts = MoveTemp(RoomPath.InnerRoomPath);
		TArray<AWall_Boolean_Base*> BayWindowsInRoom;

		TArray<AWall_Boolean_Base*> UpdateBayWindowsInRoom;
		UPlaneHousePicture::GetBayYeWindowsOnWalls(GetWorld(), BayWindowsInRoom);
		for (int I = 0; I < BayWindowsInRoom.Num(); ++I)
		{
			FOpeningSimpleAttr TempOpen = BayWindowsInRoom[I]->GetOpeningSimpleAttr();
			FVector2D TempLoc = TempOpen.Location;
			bool FindBayWindow = FPolygonAlg::GetDisOfPointToPolygon(TempLoc, RoomPathPnts);
			if (FindBayWindow)
			{
				UpdateBayWindowsInRoom.Add(BayWindowsInRoom[I]);
				FindBayWindow = false;
			}
		}
		return IsLocationInTheRoomImpl(Location, RegionInW, UpdateBayWindowsInRoom);
    }
	else
	{
		return false;
	}
	
}

bool AAreaSystem::IsLocationInTheRoomImpl(const FVector2D &ModelCenter, ARoomActor *RegionInW, const TArray<AWall_Boolean_Base*> &OpeningsInTheRoom)
{
	// we needn't check the intersection of model bounding box and area
	if (RegionInW != nullptr)
	{

		FRoomPath RoomPath = RegionInW->GetRoomPath();
		TArray<FVector2D> RoomPathPnts = MoveTemp(RoomPath.InnerRoomPath);

		bool bBelong2ThisRoom = false;
		if (ULinearEntityLibrary::IsPointInPolygon(ModelCenter, RoomPathPnts) != ECgPolyBoundedSide::EON_UNBOUNDED)
		{
			bBelong2ThisRoom = true;
		}
		else
		{
			// check if it belongs to baywindow
			for (auto Hole : OpeningsInTheRoom)
			{
				FOpeningSimpleAttr FOpenAttr = Hole->GetOpeningSimpleAttr();
				if (FOpenAttr.Type == EOpeningType::BayWindow)
				{
					TArray<FVector2D> RegionPs;
					bool GetSuccess = UPlaneHousePicture::GetBayYeWindowRegion(GetWorld(), Hole, RegionPs, RoomPathPnts);
					if (GetSuccess)
					{
						bBelong2ThisRoom = FPolygonAlg::JudgePointOnOrInPolygon(RegionPs, ModelCenter);
						if (bBelong2ThisRoom)
						{
							break;
						}
					}
				}
			}
			if (!bBelong2ThisRoom)
			{
				const float PointToSegDisLimit = 10.0;
				int32 NextIndex = 0;
				for (int32 Index = 0; Index < RoomPathPnts.Num(); Index++)
				{
					if (Index == RoomPathPnts.Num() - 1)
					{
						NextIndex = 0;
					}
					else
					{
						NextIndex = Index + 1;
					}
					float PointToSeg = FMath::PointDistToSegment(FVector(ModelCenter, 0), FVector(RoomPathPnts[Index], 0), FVector(RoomPathPnts[NextIndex], 0));
					if (PointToSeg <= PointToSegDisLimit)
					{
						bBelong2ThisRoom = true;
						break;
					}
				}
			}
		}
		return bBelong2ThisRoom;
	}
	else
	{
		return false;
	}
}

float AAreaSystem::GetWallCollectionArea(const FWallCollectionHandle &WallCollection) const
{
	return WallCollection.GetAreaOfWall();
}

FString AAreaSystem::GetWallAreaByM2(const FWallCollectionHandle &WallCollection) const
{
	double WallAreaByM2 = WallCollection.WallArea * 0.0001f;
	
	const int32 Precision = 3;

	int32 Result = (int32)ceil(WallAreaByM2 * ULinearEntityLibrary::PowerInt(10, Precision));
	FString WallAreaInt = FString::FromInt(Result);
	int32 NumberLen = WallAreaInt.Len();
	FString WallAreaOutput;
	if (NumberLen > Precision)
	{
		WallAreaOutput = WallAreaInt.Left(NumberLen - Precision);
		WallAreaOutput.AppendChar(_T('.'));
		WallAreaOutput.Append(WallAreaInt.Right(Precision));
	}
	else
	{
		WallAreaOutput = _T("0.");
		int32 ZeroCnt = Precision - NumberLen;
		for (int32 Index = 0; Index < ZeroCnt; ++Index)
		{
			WallAreaOutput.AppendChar(_T('0'));
		}
		WallAreaOutput.Append(WallAreaInt);
	}
	
	return WallAreaOutput;
}

float AAreaSystem::GetSkirtingCollectionLength(const FSkirtingCollectionHandle &SkirtingCollection) const
{
	return SkirtingCollection.GetSkirtingLength();
}

float AAreaSystem::GetAreaPerimeter(ARoomActor *Area) const
{
	double Perimeter = 0.0f;
	FRoomPath AreaPath = Area->GetRoomPath();
	int32 RoomPathCnt = AreaPath.InnerRoomPath.Num();
	for (int32 i = 0; i <  RoomPathCnt; ++i)
	{
		Perimeter += std::sqrt(CGAL::squared_distance(FExactDataConv::Vector2D2CgPointConv(AreaPath.InnerRoomPath[i]),
			FExactDataConv::Vector2D2CgPointConv(AreaPath.InnerRoomPath[(i+1)% RoomPathCnt])));
	}
	
	return (float)Perimeter;
}

TArray<ARoomActor*> AAreaSystem::SearchRelatedAreaByOpening(AWall_Boolean_Base *InOpeningActor, const UObject* WorldContextObject)
{
	TArray<ARoomActor*> OutAreas;
	if (WallSystem&&WallSystem->BuildingSystem)
	{
		if (InOpeningActor)
		{
			UDRProjData *ProjectData = UDRProjData::GetProjectDataManager(InOpeningActor);
			if (ProjectData != nullptr)
			{
				UDRHoleAdapter *HoleAdapter = Cast<UDRHoleAdapter>(ProjectData->GetAdapter(InOpeningActor->ObjectID));
				if (HoleAdapter != nullptr)
				{
					int WallID = HoleAdapter->GetDependWallId();
					FVector2D Loc = FVector2D(HoleAdapter->GetTransform().GetLocation());
					UBuildingData *WallData = WallSystem->BuildingSystem->GetData(WallID);
					if (WallData)
					{
						FVector2D Forward = FVector2D(WallData->GetVector("Forward"));
						FVector2D Dir;
						if (HoleAdapter->GetVerticalFlip())
							Dir = Forward.GetRotated(90);
						else
							Dir = Forward.GetRotated(-90);
						TArray<AActor*> _RoomRes;
						UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomActor::StaticClass(), _RoomRes);
						for (int i = 0; i < _RoomRes.Num(); ++i)
						{
							ARoomActor * _Room = Cast<ARoomActor>(_RoomRes[i]);
							if (_Room)
							{
								//解决区域关联墙体计算不全的问题
								UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WorldContextObject);
								if (projectDataManager != nullptr)
								{
									UDRAreaAdapter* AreaAdapter = Cast<UDRAreaAdapter>(projectDataManager->GetAdapter(_Room->ObjectID));
									TArray<FVector2D> AreaPointList = AreaAdapter->GetPointList();
									TArray<FVector2D> TPolygons;
									for (int m = 0; m < AreaPointList.Num(); ++m)
									{
										TPolygons.Add(AreaPointList[m]);
									}
									if (TPolygons.Num() > 2)
									{
										if (HoleAdapter->GetHoleType() == EModelType::EDRDoor || HoleAdapter->GetHoleType() == EModelType::EDRDoubleDoor)
										{
											if (FPolygonAlg::JudgePointInPolygon(TPolygons, Loc + Dir * 30))
											{
												OutAreas.Add(_Room);
												break;
											}
										}
										else
											if (HoleAdapter->GetHoleType() == EModelType::EDRBayWindow || HoleAdapter->GetHoleType() == EModelType::EDRGroundWindow
												|| HoleAdapter->GetHoleType() == EModelType::EDRWindow)
											{
												if (FPolygonAlg::JudgePointInPolygon(TPolygons, Loc + Dir * 30))
												{
													OutAreas.Add(_Room);
													break;
												}
												else
													if (FPolygonAlg::JudgePointInPolygon(TPolygons, Loc - Dir * 30))
													{
														OutAreas.Add(_Room);
														break;
													}
											}
											else
												if (HoleAdapter->GetHoleType() == EModelType::EDRSliderDoor || HoleAdapter->GetHoleType() == EModelType::EDRDoorWay)
												{
													if (FPolygonAlg::JudgePointInPolygon(TPolygons, Loc + Dir * 30))
													{
														OutAreas.Add(_Room);
													}

													if (FPolygonAlg::JudgePointInPolygon(TPolygons, Loc - Dir * 30))
													{
														OutAreas.Add(_Room);
													}
												}
									}
								}
							}
						}

						if (HoleAdapter->GetHoleType() == EModelType::EDRDoor || HoleAdapter->GetHoleType() == EModelType::EDRDoubleDoor)
						{
							if (0 == OutAreas.Num())
							{
								for (int i = 0; i < _RoomRes.Num(); ++i)
								{
									ARoomActor * _Room = Cast<ARoomActor>(_RoomRes[i]);
									if (_Room)
									{
										//解决区域关联墙体计算不全的问题
										UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WorldContextObject);
										if (projectDataManager != nullptr)
										{
											UDRAreaAdapter* AreaAdapter = Cast<UDRAreaAdapter>(projectDataManager->GetAdapter(_Room->ObjectID));
											TArray<FVector2D> AreaPointList = AreaAdapter->GetPointList();
											TArray<FVector2D> TPolygons;
											for (int m = 0; m < AreaPointList.Num(); ++m)
											{
												TPolygons.Add(AreaPointList[m]);
											}
											if (TPolygons.Num() > 2)
											{
												if (FPolygonAlg::JudgePointInPolygon(TPolygons, Loc - Dir * 30))
												{
													OutAreas.Add(_Room);
													break;
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
		}
	}
	return OutAreas;
}

TArray<ARoomActor*> AAreaSystem::SearchRelatedAreasByWallSurface(ALineWallActor *InWallActor, EWallSurfaceType WallFace)
{
	TArray<ARoomActor*> OutAreas;
	for (auto &RegionWithR : RegionsWithRCInWorld)
	{
		bool bOwnerRegion = false;
		TArray<UWallSurfaceObject*> WallSurfaceInThisRegion;
		WallSurfaceInThisRegion.Append(RegionWithR->WallsCcb);
		WallSurfaceInThisRegion.Append(RegionWithR->WallsInThisRegion);
		for (const auto &WallRelate2ThisRegion : WallSurfaceInThisRegion)
		{
			for (const auto &WallSeg : WallRelate2ThisRegion->WallsLayOnThisSeg)
			{
				if (WallSeg->WallLayOnSeg == InWallActor && WallSeg->FaceType == WallFace)
				{
					bOwnerRegion = true;
				}
			}
		}
		if (bOwnerRegion)
		{
			OutAreas.Add(RegionWithR->RegionActor);
		}
	}

	ARoomActor *FoundArea = nullptr;

	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(InWallActor);
	if (projectDataManager == nullptr) {
		return TArray<ARoomActor*>();
	}

	UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(InWallActor->ObjectId));
	if (solidWallAdapter == nullptr) {
		return TArray<ARoomActor*>();
	}

	float TotalDist = -1.0f;
	for (ARoomActor* RegionActor : OutAreas)
	{
		TArray<FVector2D> RoomPnts = RegionActor->GetRoomPath().InnerRoomPath;
		float CurDist = std::sqrt(ULinearEntityLibrary::GetSquaredPointDist2Polygon(FVector2D(solidWallAdapter->GetWallPositionData().StartPos.X, solidWallAdapter->GetWallPositionData().StartPos.Y), RoomPnts))
			+ std::sqrt(ULinearEntityLibrary::GetSquaredPointDist2Polygon(FVector2D(solidWallAdapter->GetWallPositionData().EndPos.X, solidWallAdapter->GetWallPositionData().EndPos.Y), RoomPnts));
		if (TotalDist < 0 || CurDist < TotalDist)
		{
			TotalDist = CurDist;
			FoundArea = RegionActor;
		}
	}
	OutAreas.Empty();
	OutAreas.Add(FoundArea);

	return OutAreas;
}

TArray<ALineWallActor*> AAreaSystem::CollectCcbWallsOfRoom(ARoomActor *RegionInW, bool bFindByCache)
{
	TArray<ALineWallActor*> WallsRelated;

	auto RegionWithRObj = CacheRoomRelationObj(RegionInW, bFindByCache);

	TArray<AWall_Boolean_Base*> OpeningsOfRoom;
	TArray<ARoomActor*> OutAreas;
	for (auto &WallCcb : RegionWithRObj->WallsCcb)
	{
		for (const auto &WallSeg : WallCcb->WallsLayOnThisSeg)
		{
			WallsRelated.Add(WallSeg->WallLayOnSeg);
		}
	}

	return WallsRelated;
}

URegionObject* AAreaSystem::CacheRoomRelationObj(ARoomActor *RegionInW, bool bFindByCache)
{
	auto RegionWithRFound = RegionsWithRCInWorld.FindByPredicate([RegionInW](const URegionObject *RegionObj)
	{
		return RegionObj->RegionActor == RegionInW;
	});

	URegionObject *RegionWithRObj = nullptr;
	if (RegionWithRFound)
	{
		RegionWithRObj = *RegionWithRFound;
	}
	else
	{
		RegionWithRObj = NewObject<URegionObject>(RegionInW);
		RegionWithRObj->RegionActor = RegionInW;
		RegionsWithRCInWorld.Add(RegionWithRObj);
	}

	if (!RegionWithRFound || !bFindByCache)
	{
		ArrangeWalls2Region(RegionWithRObj);
	}

	return RegionWithRObj;
}

void AAreaSystem::ArrangeWalls2Region(URegionObject *RegionObj)
{
	RegionObj->WallsInThisRegion.Empty();
	RegionObj->WallsCcb.Empty();
	RegionObj->SkirtingLines.Empty();
	auto RegionInW = RegionObj->RegionActor;
	auto RoomPath = RegionInW->GetRoomPath();
	auto Orientation = ULinearEntityLibrary::GetPolygonOrientation(RoomPath.InnerRoomPath);
	if (Orientation != ECgPolyOrientation::ECLOCKWISE)
	{
		Algo::Reverse(RoomPath.InnerRoomPath);
	}
	int32 SurroundingCornerCnt = RoomPath.InnerRoomPath.Num();
	for (int32 CornerIdx = 0; CornerIdx < SurroundingCornerCnt; ++CornerIdx)
	{
		auto StartPnt = RoomPath.InnerRoomPath[CornerIdx];
		auto EndPnt = RoomPath.InnerRoomPath[(CornerIdx + 1) % SurroundingCornerCnt];
		UWallSurfaceObject *WallSurObj = NewObject<UWallSurfaceObject>(RegionObj);
		RegionObj->WallsCcb.Add(WallSurObj);
		WallSurObj->StartPosOfRegion = StartPnt;
		WallSurObj->EndPosOfRegion = EndPnt;
		RebuildWallSurRelationCache(WallSurObj);
	}

	ArrangeInnerWalls2Room(RegionObj);

	RegionObj->CalculateRegionCollection();
}

void AAreaSystem::ArrangeInnerWalls2Room(URegionObject *RegionObj)
{
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	UWallBuildSystem *WallSystem = GameInst->WallBuildSystem;
	TArray<ALineWallActor*> WallsInScene = WallSystem->GetWallsInScene();

	// exclude walss that lies along with the edge of this region
	TArray<ALineWallActor*> WallsCcbOfRoom =  CollectCcbWallsOfRoom(RegionObj->RegionActor, true);
	WallsInScene.RemoveAll([&WallsCcbOfRoom](ALineWallActor *WallInScene)
	{
		return (WallsCcbOfRoom.Find(WallInScene) >= 0);
	});

	using FPolygon2D = FExactDataConv::FP2CPolygon2D;
	using FSegment2D = FExactDataConv::FP2CSegment2D;
	using FPoint2D = FExactDataConv::FP2CPoint2D;

	FRoomPath RoomPath = RegionObj->RegionActor->GetRoomPath();
	FPolygon2D AreaPolygon;
	for (auto RoomPnt : RoomPath.InnerRoomPath)
	{
		AreaPolygon.push_back(FExactDataConv::Vector2D2CgPointConv(RoomPnt));
	}

	// cut walls by this room
	for (const auto WallInScene : WallsInScene)
	{
		UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WallInScene);
		if (projectDataManager == nullptr) {
			continue;
		}

		UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallInScene->ObjectId));
		if (solidWallAdapter == nullptr) {
			continue;
		}

		// front wall and back wall segment surface
		FSegment2D WallSegment(FExactDataConv::Vector2D2CgPointConv(FVector2D(solidWallAdapter->GetWallPositionData().StartPos.X, solidWallAdapter->GetWallPositionData().EndPos.Y)),
			FExactDataConv::Vector2D2CgPointConv(FVector2D(solidWallAdapter->GetWallPositionData().EndPos.X, solidWallAdapter->GetWallPositionData().EndPos.Y)));
		
		TArray<FSegment2D> WallPolygonSegments;
		ULinearEntityLibrary::IntersectionOfPolygonAndSegment<CGAL::Exact_predicates_inexact_constructions_kernel>
			(WallPolygonSegments, AreaPolygon, WallSegment);

		for (auto &WallCuttingSegment : WallPolygonSegments)
		{
			FVector2D WallSegStart = FExactDataConv::CgPoint2D2VectorConv(WallCuttingSegment[0]);
			FVector2D WallSegEnd = FExactDataConv::CgPoint2D2VectorConv(WallCuttingSegment[1]);

			UWallSurfaceObject *WallCuttingSegFrontFace = NewObject<UWallSurfaceObject>(RegionObj);
			WallCuttingSegFrontFace->bWallSurfaceInRoom = true;
			RegionObj->WallsInThisRegion.Add(WallCuttingSegFrontFace);

			UWallSegObject *WallFrontSeg = NewObject<UWallSegObject>(WallCuttingSegFrontFace);
			WallCuttingSegFrontFace->WallsLayOnThisSeg.Add(WallFrontSeg);
			WallFrontSeg->FaceType = EWallSurfaceType::Front;
			WallFrontSeg->WallLayOnSeg = WallInScene;
			WallFrontSeg->bWallInRegion = true;
			WallFrontSeg->WallSegStart = WallSegStart;
			WallFrontSeg->WallSegEnd = WallSegEnd;

			UWallSurfaceObject *WallCuttingSegBackFace = NewObject<UWallSurfaceObject>(RegionObj);
			WallCuttingSegBackFace->bWallSurfaceInRoom = true;
			RegionObj->WallsInThisRegion.Add(WallCuttingSegBackFace);

			UWallSegObject *WallBackSeg = NewObject<UWallSegObject>(WallCuttingSegBackFace);
			WallCuttingSegBackFace->WallsLayOnThisSeg.Add(WallBackSeg);
			WallBackSeg->FaceType = EWallSurfaceType::Back;
			WallBackSeg->WallLayOnSeg = WallInScene;
			WallBackSeg->bWallInRegion = true;
			WallBackSeg->WallSegStart = WallSegStart;
			WallBackSeg->WallSegEnd = WallSegEnd;
		}

		// start wall and end wall surface
		FVector2D WallStartLeft(solidWallAdapter->GetWallPositionData().LeftStartPos.X, solidWallAdapter->GetWallPositionData().LeftStartPos.Y);
		FVector2D WallStartRight(solidWallAdapter->GetWallPositionData().RightStartPos.X, solidWallAdapter->GetWallPositionData().RightStartPos.Y);
		// judge if the start surface lies in this area
		ObjectID* connectedWall(nullptr);
		if ((AreaPolygon.bounded_side(FExactDataConv::Vector2D2CgPointConv(WallStartLeft)) == CGAL::ON_BOUNDED_SIDE)
			&& AreaPolygon.bounded_side(FExactDataConv::Vector2D2CgPointConv(WallStartRight)) == CGAL::ON_BOUNDED_SIDE)
		{
			if (projectDataManager->GetSuite()->GetConnectWalls(solidWallAdapter->GetP0(), connectedWall) == 1)
			{
				UWallSurfaceObject *WallStartSurface = NewObject<UWallSurfaceObject>(RegionObj);
				RegionObj->WallsInThisRegion.Add(WallStartSurface);
				WallStartSurface->bWallSurfaceInRoom = true;
				UWallSegObject *WallStartSeg = NewObject<UWallSegObject>(WallStartSurface);
				WallStartSurface->WallsLayOnThisSeg.Add(WallStartSeg);
				WallStartSeg->FaceType = EWallSurfaceType::StartSide;
				WallStartSeg->WallLayOnSeg = WallInScene;
				WallStartSeg->bWallInRegion = true;
				WallStartSeg->WallSegStart = WallStartLeft;
				WallStartSeg->WallSegEnd = WallStartRight;
			}
		}

		FVector2D WallEndLeft(solidWallAdapter->GetWallPositionData().LeftEndPos.X, solidWallAdapter->GetWallPositionData().LeftEndPos.Y);
		FVector2D WallEndRight(solidWallAdapter->GetWallPositionData().RightEndPos.X, solidWallAdapter->GetWallPositionData().RightEndPos.Y);
		// judge if the end surface lies in this area
		connectedWall = nullptr;
		if ((AreaPolygon.bounded_side(FExactDataConv::Vector2D2CgPointConv(WallEndLeft)) == CGAL::ON_BOUNDED_SIDE)
			&& AreaPolygon.bounded_side(FExactDataConv::Vector2D2CgPointConv(WallEndRight)) == CGAL::ON_BOUNDED_SIDE)
		{
			if (projectDataManager->GetSuite()->GetConnectWalls(solidWallAdapter->GetP1(), connectedWall) == 1)
			{
				UWallSurfaceObject *WallEndSurface = NewObject<UWallSurfaceObject>(RegionObj);
				RegionObj->WallsInThisRegion.Add(WallEndSurface);
				WallEndSurface->bWallSurfaceInRoom = true;
				UWallSegObject *WallEndSeg = NewObject<UWallSegObject>(WallEndSurface);
				WallEndSurface->WallsLayOnThisSeg.Add(WallEndSeg);
				WallEndSeg->FaceType = EWallSurfaceType::EndSide;
				WallEndSeg->WallLayOnSeg = WallInScene;
				WallEndSeg->bWallInRegion = true;
				WallEndSeg->WallSegStart = WallStartLeft;
				WallEndSeg->WallSegEnd = WallStartRight;
			}
		}
	}
}

void AAreaSystem::ArrangeSkirtingLines2Region(URegionObject *RegionObj)
{
	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	TArray<USkirtingMeshComponent*> SkirtingLinesInScene;
	GameInst->SkirtingSystem->CollectAllSkirtingLines(SkirtingLinesInScene);

	using FSegment2D = FExactDataConv::FP2CSegment2D;
	using FPoint2D = FExactDataConv::FP2CPoint2D;
	using FPolygon2D = FExactDataConv::FP2CPolygon2D;
	using FCGVector2D = FExactDataConv::FP2CVector2D;

	auto RegionInW = RegionObj->RegionActor;
	auto RoomPath = RegionInW->GetRoomPath();
	RegionObj->RegionActor->SktringInArea();
	FPolygon2D RegionPolygon;
	for (auto RoomPnt : RoomPath.InnerRoomPath)
	{
		RegionPolygon.push_back(FExactDataConv::Vector2D2CgPointConv(RoomPnt));
	}
	//for (USkirtingMeshComponent * SkirtingLineInScene : SkirtingLinesInScene)
	//{
	
		for (int i = 0 ; i< RegionObj->RegionActor->AreaSkirting.Num();i++)
		{
			USkirtingSegComponent * SkirtingSeg = RegionObj->RegionActor->AreaSkirting[i];
			auto &SegNode = SkirtingSeg->GetRelatedNode();
			for (const auto &SkirtingInterval : SegNode.GetCachedIntervals())
			{
				FPoint2D SkirtingStartPnt(FExactDataConv::Vector2D2CgPointConv(SkirtingInterval.GetStart()));
				FPoint2D SkirtingEndPnt(FExactDataConv::Vector2D2CgPointConv(SkirtingInterval.GetEnd()));
				FVector2D FSkirtingStartPnt = SkirtingInterval.GetStart();
				FVector2D FSkirtingEndPnt = SkirtingInterval.GetEnd();
				auto StartPntPolygonSide = RegionPolygon.bounded_side(SkirtingStartPnt);
				auto EndPntPolygonSide = RegionPolygon.bounded_side(SkirtingEndPnt);
				USkirtingSegObject *SkirtingSegObj = NewObject<USkirtingSegObject>(RegionObj);
				RegionObj->SkirtingLines.Add(SkirtingSegObj);
				SkirtingSegObj->RelateSkirtingSeg = SkirtingSeg;
				SkirtingSegObj->SetStart(0.0f);
				float num = FVector2D(FSkirtingEndPnt - FSkirtingStartPnt).Size();
				SkirtingSegObj->SetEnd(num);
				//if ((StartPntPolygonSide != CGAL::ON_UNBOUNDED_SIDE) && (EndPntPolygonSide != CGAL::ON_UNBOUNDED_SIDE))
				//{
				//	//SkirtingSegObj->SetEnd(std::sqrt(CGAL::squared_distance(SkirtingStartPnt, SkirtingEndPnt)));
				//	float num = FVector2D(FSkirtingEndPnt - FSkirtingStartPnt).Size();
				//	SkirtingSegObj->SetEnd(num);
				//}
 			//	else
				//{
				//	TArray<FSegment2D> Segments;
				//	FSegment2D SkirtingSegmentG(SkirtingStartPnt, SkirtingEndPnt);
				//	ULinearEntityLibrary::IntersectionOfPolygonAndSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(Segments, RegionPolygon, SkirtingSegmentG);
				//	float TotalLength = 0.0f;
				//	if(Segments.Num())
				//	{
				//		TotalLength = FVector2D(FSkirtingEndPnt - FSkirtingStartPnt).Size();
				//	}
				//	SkirtingSegObj->SetEnd(TotalLength);
				//	
				//}

			}
		}
	//}
}

struct FWallSegmentAlongWithHalfEdge
{
	FWallSegmentAlongWithHalfEdge()
		:Owner(nullptr), bSameDirWithHalfEdge(false)
	{
	}

	ALineWallActor *Owner;
	bool bSameDirWithHalfEdge;
	FDClosedInterval SegmentInterval;
	FExactDataConv::FP2CPoint2D WallSegStartPnt;
	FExactDataConv::FP2CPoint2D WallSegEndPnt;
};

void AAreaSystem::RebuildWallSurRelationCache(UWallSurfaceObject *Region2Build)
{
	auto RegionSegDirection = Region2Build->EndPosOfRegion - Region2Build->StartPosOfRegion;
	using FSegment2D = FExactDataConv::FP2CSegment2D;
	using FPoint2D = FExactDataConv::FP2CPoint2D;
	FPoint2D RegionSegStart(FExactDataConv::Vector2D2CgPointConv(Region2Build->StartPosOfRegion));
	FPoint2D RegionSegEnd(FExactDataConv::Vector2D2CgPointConv(Region2Build->EndPosOfRegion));
	FSegment2D RegionSeg(RegionSegStart, RegionSegEnd);
	auto RegionLine = RegionSeg.supporting_line();

	UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(GetWorld()->GetGameInstance());
	UWallBuildSystem *WallSystem = GameInst->WallBuildSystem;
	TArray<ALineWallActor*> WallsLayOnSeg;
	UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(GameInst);
	if (projectDataManager == nullptr) {
		return;
	}
	WallSystem->SearchWallsByEndPnt(WallsLayOnSeg,
		Region2Build->StartPosOfRegion, Region2Build->EndPosOfRegion);
	if (WallsLayOnSeg.Num() != 0)
	{
		// determine how they split this seg
		TArray<FWallSegmentAlongWithHalfEdge> WallIntervals;
		for (auto WallSeg : WallsLayOnSeg)
		{
			UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallSeg->ObjectId));
			if (solidWallAdapter == nullptr) {
				continue;
			}

			FVector2D wallEndPos(solidWallAdapter->GetWallPositionData().EndPos.X, solidWallAdapter->GetWallPositionData().EndPos.Y);
			FVector2D wallStartPos(solidWallAdapter->GetWallPositionData().StartPos.X, solidWallAdapter->GetWallPositionData().StartPos.Y);

			auto WallDirection = wallEndPos - wallStartPos;
			bool bSameDirWithRegion = ((WallDirection | RegionSegDirection) > 0.0f);
			double StartDist = 0.0f;
			double EndDist = 0.0f;
			FPoint2D WallStartSegPnt;
			FPoint2D WallEndSegPnt;
			if (bSameDirWithRegion)
			{
				FVector2D wallRightStart(solidWallAdapter->GetWallPositionData().RightStartPos.X, solidWallAdapter->GetWallPositionData().RightStartPos.Y);
				FVector2D wallRightEnd(solidWallAdapter->GetWallPositionData().RightEndPos.X, solidWallAdapter->GetWallPositionData().RightEndPos.Y);
				FPoint2D WallStartRightProj, WallEndRightProj;
				FPoint2D WallStartRight = FExactDataConv::Vector2D2CgPointConv(wallRightStart);
				FPoint2D WallEndRight = FExactDataConv::Vector2D2CgPointConv(wallRightEnd);

				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallStartRightProj,
					WallStartRight,
					RegionSeg, &RegionLine);
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallEndRightProj,
					WallEndRight,
					RegionSeg, &RegionLine);
				StartDist = std::sqrt(CGAL::squared_distance(WallStartRightProj, RegionSegStart));
				EndDist = std::sqrt(CGAL::squared_distance(WallEndRightProj, RegionSegStart));

				FSegment2D WallRightSegment(WallStartRight, WallEndRight);
				auto WallRightLine = WallRightSegment.supporting_line();
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallStartSegPnt,
					RegionSegStart,
					WallRightSegment, &WallRightLine);
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallEndSegPnt,
					RegionSegEnd,
					WallRightSegment, &WallRightLine);
			}
			else
			{
				FVector2D wallLeftStart(solidWallAdapter->GetWallPositionData().LeftStartPos.X, solidWallAdapter->GetWallPositionData().LeftStartPos.Y);
				FVector2D wallLeftEnd(solidWallAdapter->GetWallPositionData().LeftEndPos.X, solidWallAdapter->GetWallPositionData().LeftEndPos.Y);
				FPoint2D WallStartLeftProj, WallEndLeftProj;
				FPoint2D WallStartLeft = FExactDataConv::Vector2D2CgPointConv(wallLeftStart);
				FPoint2D WallEndLeft = FExactDataConv::Vector2D2CgPointConv(wallLeftEnd);

				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallStartLeftProj,
					WallStartLeft,
					RegionSeg, &RegionLine);
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallEndLeftProj,
					WallEndLeft,
					RegionSeg, &RegionLine);
				StartDist = std::sqrt(CGAL::squared_distance(WallStartLeftProj, RegionSegStart));
				EndDist = std::sqrt(CGAL::squared_distance(WallEndLeftProj, RegionSegStart));

				FSegment2D WallLeftSegment(WallStartLeft, WallEndLeft);
				auto WallLeftLine = WallLeftSegment.supporting_line();
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallStartSegPnt,
					RegionSegStart,
					WallLeftSegment, &WallLeftLine);
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallEndSegPnt,
					RegionSegEnd,
					WallLeftSegment, &WallLeftLine);
			}

			double LengthCheck = FMath::Abs(StartDist - EndDist);
			if (LengthCheck < SmallPositiveNumber)
			{
				continue;
			}

			FWallSegmentAlongWithHalfEdge WallSegWithHEdge;
			WallSegWithHEdge.bSameDirWithHalfEdge = bSameDirWithRegion;
			WallSegWithHEdge.SegmentInterval = FDClosedInterval(StartDist, EndDist);
			WallSegWithHEdge.Owner = WallSeg;
			WallSegWithHEdge.WallSegStartPnt = WallStartSegPnt;
			WallSegWithHEdge.WallSegEndPnt = WallEndSegPnt;
			WallIntervals.Add(WallSegWithHEdge);
		}
		
		WallIntervals.Sort([](const FWallSegmentAlongWithHalfEdge &F, const FWallSegmentAlongWithHalfEdge &S)
		{
			return F.SegmentInterval.StartPnt < S.SegmentInterval.StartPnt;
		});

		// split to segments
		double LastPosition = 0.0f;
		for (auto &WallInterval : WallIntervals)
		{
			if (WallInterval.SegmentInterval.EndPnt <= LastPosition)
			{
				continue;
			}

			double WallSegStartPnt = WallInterval.SegmentInterval.StartPnt;
			if (WallSegStartPnt < LastPosition)
			{
				WallSegStartPnt = LastPosition;
			}

			UWallSegObject *WallSegObj = NewObject<UWallSegObject>(Region2Build);
			Region2Build->WallsLayOnThisSeg.Add(WallSegObj);
			WallSegObj->WallLayOnSeg = WallInterval.Owner;
			WallSegObj->SetStart(WallSegStartPnt);
			WallSegObj->SetEnd(WallInterval.SegmentInterval.EndPnt);
			WallSegObj->SetWallSegStart(FExactDataConv::CgPoint2D2VectorConv(WallInterval.WallSegStartPnt));
			WallSegObj->SetWallSegEnd(FExactDataConv::CgPoint2D2VectorConv(WallInterval.WallSegEndPnt));
			WallSegObj->SetRegionStart(Region2Build->StartPosOfRegion);
			WallSegObj->SetRegionEnd(Region2Build->EndPosOfRegion);
			WallSegObj->bSameDirWithRegion = WallInterval.bSameDirWithHalfEdge;
			LastPosition = WallInterval.SegmentInterval.EndPnt;
			if (WallInterval.bSameDirWithHalfEdge)
			{
				WallSegObj->FaceType = EWallSurfaceType::Front;
			}
			else
			{
				WallSegObj->FaceType = EWallSurfaceType::Back;
			}
		}
	}
	
	TArray<ALineWallActor*> SideWallsLayOnSeg;
	WallSystem->SearchSideWallsByEndPnt(SideWallsLayOnSeg,
		Region2Build->StartPosOfRegion, Region2Build->EndPosOfRegion);
	if (SideWallsLayOnSeg.Num() != 0)
	{
		// determine how they split this seg
		TArray<FWallSegmentAlongWithHalfEdge> WallIntervals;
		for (auto WallSeg : SideWallsLayOnSeg)
		{
			UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallSeg->ObjectId));
			if (solidWallAdapter == nullptr) {
				continue;
			}

			FPoint2D WallStartLeft = FExactDataConv::Vector2D2CgPointConv(FVector2D(solidWallAdapter->GetWallPositionData().LeftStartPos.X, solidWallAdapter->GetWallPositionData().LeftStartPos.Y));
			FPoint2D WallStartRight = FExactDataConv::Vector2D2CgPointConv(FVector2D(solidWallAdapter->GetWallPositionData().RightStartPos.X, solidWallAdapter->GetWallPositionData().RightStartPos.Y));
			FPoint2D WallEndLeft = FExactDataConv::Vector2D2CgPointConv(FVector2D(solidWallAdapter->GetWallPositionData().LeftEndPos.X, solidWallAdapter->GetWallPositionData().LeftEndPos.Y));
			FPoint2D WallEndRight = FExactDataConv::Vector2D2CgPointConv(FVector2D(solidWallAdapter->GetWallPositionData().RightEndPos.X, solidWallAdapter->GetWallPositionData().RightEndPos.Y));
			FPoint2D WallStartLeftProj, WallStartRightProj, WallEndLeftProj, WallEndRightProj;
			ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallStartLeftProj,
				WallStartLeft,
				RegionSeg, &RegionLine);
			ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallStartRightProj,
				WallStartRight,
				RegionSeg, &RegionLine);
			ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallEndLeftProj,
				WallEndLeft,
				RegionSeg, &RegionLine);
			ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallEndRightProj,
				WallEndRight,
				RegionSeg, &RegionLine);

			double StartFaceDist = CGAL::squared_distance(WallStartLeft, WallStartLeftProj) + CGAL::squared_distance(WallStartRight, WallStartRightProj);
			double EndFaceDist = CGAL::squared_distance(WallEndLeft, WallEndLeftProj) + CGAL::squared_distance(WallEndRight, WallEndRightProj);
			bool bFromStart = (StartFaceDist > EndFaceDist);

			FPoint2D WallSegStartPnt, WallSegEndPnt;
			double StartDist = 0.0f, EndDist = 0.0f;
			if (bFromStart)
			{
				StartDist = std::sqrt(CGAL::squared_distance(WallStartLeftProj, RegionSegStart));
				EndDist = std::sqrt(CGAL::squared_distance(WallStartRightProj, RegionSegStart));

				FSegment2D WallStartSegment(WallStartLeft, WallStartRight);
				auto WallStartLine = WallStartSegment.supporting_line();
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallSegStartPnt,
					RegionSegStart,
					WallStartSegment, &WallStartLine);
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallSegEndPnt,
					RegionSegEnd,
					WallStartSegment, &WallStartLine);
			}
			else
			{
				StartDist = std::sqrt(CGAL::squared_distance(WallEndLeftProj, RegionSegStart));
				EndDist = std::sqrt(CGAL::squared_distance(WallEndRightProj, RegionSegStart));

				FSegment2D WallEndSegment(WallEndLeft, WallEndRight);
				auto WallEndLine = WallEndSegment.supporting_line();
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallSegStartPnt,
					RegionSegStart,
					WallEndSegment, &WallEndLine);
				ULinearEntityLibrary::ClosestPntOnSegment<CGAL::Exact_predicates_inexact_constructions_kernel>(WallSegEndPnt,
					RegionSegEnd,
					WallEndSegment, &WallEndLine);
			}

			FWallSegmentAlongWithHalfEdge WallSegWithHEdge;
			WallSegWithHEdge.bSameDirWithHalfEdge = bFromStart;
			WallSegWithHEdge.SegmentInterval = FDClosedInterval(StartDist, EndDist);
			WallSegWithHEdge.Owner = WallSeg;
			WallSegWithHEdge.WallSegStartPnt = WallSegStartPnt;
			WallSegWithHEdge.WallSegEndPnt = WallSegEndPnt;
			WallIntervals.Add(WallSegWithHEdge);
		}

		WallIntervals.Sort([](const FWallSegmentAlongWithHalfEdge &F, const FWallSegmentAlongWithHalfEdge &S)
		{
			return F.SegmentInterval.StartPnt < S.SegmentInterval.StartPnt;
		});

		// split to segments
		double LastPosition = 0.0f;
		for (auto &WallInterval : WallIntervals)
		{
			UDRSolidWallAdapter* solidWallAdapter = Cast<UDRSolidWallAdapter>(projectDataManager->GetAdapter(WallInterval.Owner->ObjectId));
			if (solidWallAdapter == nullptr) {
				continue;
			}

			if (WallInterval.SegmentInterval.EndPnt <= LastPosition)
			{
				continue;
			}

			double WallSegStartPnt = WallInterval.SegmentInterval.StartPnt;
			if (WallSegStartPnt < LastPosition)
			{
				WallSegStartPnt = LastPosition;
			}

			UWallSegObject *WallSegObj = NewObject<UWallSegObject>(Region2Build);
			WallSegObj->WallLayOnSeg = WallInterval.Owner;
			WallSegObj->SetStart(WallSegStartPnt);
			WallSegObj->SetEnd(WallInterval.SegmentInterval.EndPnt);
			WallSegObj->SetRegionStart(Region2Build->StartPosOfRegion);
			WallSegObj->SetRegionEnd(Region2Build->EndPosOfRegion);
			WallSegObj->bSameDirWithRegion = WallInterval.bSameDirWithHalfEdge;
			LastPosition = WallInterval.SegmentInterval.EndPnt;

			ObjectID* connectedWalls(nullptr);
			if (WallInterval.bSameDirWithHalfEdge)
			{
				if (1 == projectDataManager->GetSuite()->GetConnectWalls(solidWallAdapter->GetP0(), connectedWalls))
				{
					Region2Build->WallsLayOnThisSeg.Add(WallSegObj);
				}
				
				WallSegObj->FaceType = EWallSurfaceType::StartSide;
				WallSegObj->SetWallSegStart(FVector2D(solidWallAdapter->GetWallPositionData().LeftStartPos.X, solidWallAdapter->GetWallPositionData().LeftStartPos.Y));
				WallSegObj->SetWallSegEnd(FVector2D(solidWallAdapter->GetWallPositionData().RightStartPos.X, solidWallAdapter->GetWallPositionData().RightStartPos.Y));
			}
			else
			{
				if (1 == projectDataManager->GetSuite()->GetConnectWalls(solidWallAdapter->GetP1(), connectedWalls))
				{
					Region2Build->WallsLayOnThisSeg.Add(WallSegObj);
				}

				WallSegObj->FaceType = EWallSurfaceType::EndSide;
				WallSegObj->SetWallSegStart(FVector2D(solidWallAdapter->GetWallPositionData().LeftEndPos.X, solidWallAdapter->GetWallPositionData().LeftEndPos.Y));
				WallSegObj->SetWallSegEnd(FVector2D(solidWallAdapter->GetWallPositionData().RightEndPos.X, solidWallAdapter->GetWallPositionData().RightEndPos.Y));
			}
		}
	}
}

void AAreaSystem::OnRegionActorSpawned(ARoomActor* Actor)
{
	RegionsInWorld.Add(Actor);
}

void AAreaSystem::OnRegionActorDestroyed(ARoomActor* Actor)
{
	RegionsInWorld.Remove(Actor);

	int32 RegionWithRCnt = RegionsWithRCInWorld.Num();
	for (int32 RegionIndex = RegionWithRCnt - 1; RegionIndex >= 0; --RegionIndex)
	{
		if (RegionsWithRCInWorld[RegionIndex]->RegionActor == Actor)
		{
			RegionsWithRCInWorld.RemoveAt(RegionIndex);
			break;
		}
	}
}

float AAreaSystem::GetRoomArea(ARoomActor *InRoomActor) const
{
	using FPolygon2D = FExactDataConv::FP2CPolygon2D;
	using FPoint2D = FExactDataConv::FP2CPoint2D;

	auto RoomPath = InRoomActor->GetRoomPath();
	FPolygon2D AreaPolygon;
	for (const auto& RoomPnt : RoomPath.InnerRoomPath)
	{
		AreaPolygon.push_back(FExactDataConv::Vector2D2CgPointConv(RoomPnt));
	}
	
	return (float)(std::abs(AreaPolygon.area()));
}

const TArray<URegionObject*>& AAreaSystem::GetRegionsWithRCInWorld() const
{
	return RegionsWithRCInWorld;
}

const TArray<ARoomActor*>& AAreaSystem::GetRegionsInWorld() const
{
	return RegionsInWorld;
}
