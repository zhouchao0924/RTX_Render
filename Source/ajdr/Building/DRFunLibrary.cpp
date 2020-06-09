
#include "DRFunLibrary.h"
#include "EditorGameInstance.h"
#include "DRGameMode.h"
#include "DRModelFactory.h"
#include "DRComponentModel.h"
#include "ISuite.h"
#include "BuildingData.h"
#include "BuildingConfig.h"
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "IBuildingSDK.h"
#include "ResourceMgr.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Building/ModelFileActor.h"
#include "ProceduralMeshComponent.h"
#include "CGALWrapper/CgTypes.h"
#include "CGALWrapper/CgDataConvUtility.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include "HouseComponent/ComponentManagerActor.h"
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/intersections.h>
#include <CGAL/Polygon_2_algorithms.h>
#include "ISuite.h"
#include "IProperty.h"
#include "Model/ResourceMgr.h"
#include "HomeLayout/SceneEntity/Wall_Boolean_Base.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DRStruct/DRBuildingStruct.h"
#include "Data/DRProjData.h"
#include "Data/Adapter/DRHoleAdapter.h"
#include "Data/Adapter/DRModelInstanceAdapter.h"
#include "Data/Adapter/DRAreaAdapter.h"


#pragma optimize("", off)

#define ERROR_DIRECT 5.f
#define ERROR_DUBUG_ON_OFF 0
#define ERROR_DUBUG_LineTime	10.f


FVector UDRFunLibrary::MaxVertex(const TArray<FVector> & Vertex)
{
	FVector _Max;
	if (Vertex.Num())
	{
		for (int i = 0; i < Vertex.Num(); ++i)
		{
			if (Vertex[i].X > _Max.X)
				_Max.X = Vertex[i].X;
			if (Vertex[i].Y > _Max.Y)
				_Max.Y = Vertex[i].Y;
			if (Vertex[i].Z > _Max.Z)
				_Max.Z = Vertex[i].Z;
			FMath::Max<int>(1, 2);
		}
		return _Max;
	}
	return FVector::ZeroVector;
}

FVector UDRFunLibrary::MinVertex(const TArray<FVector> & Vertex)
{
	FVector _Min;
	if (Vertex.Num())
	{
		for (int i = 0; i < Vertex.Num(); ++i)
		{
			if (Vertex[i].X < _Min.X)
				_Min.X = Vertex[i].X;
			if (Vertex[i].Y < _Min.Y)
				_Min.Y = Vertex[i].Y;
			if (Vertex[i].Z < _Min.Z)
				_Min.Z = Vertex[i].Z;
		}
		return _Min;
	}
	return FVector::ZeroVector;
}

bool UDRFunLibrary::IsBuildDataModelType(UBuildingData * BuildData)
{
	if (BuildData)
	{
		if (BuildData->GetObjectType() == EObjectType::EModelInstance)
			return true;
	}
	return false;
}

bool UDRFunLibrary::IsBuildDataWindowHoleType(UBuildingData * BuildData)
{
	if (BuildData)
	{
		if (BuildData->GetObjectType() == EObjectType::EWindow)
			return true;
	}
	return false;
}

bool UDRFunLibrary::IsBuildDataDoorHoleType(UBuildingData * BuildData)
{
	if (BuildData)
	{
		if (BuildData->GetObjectType() == EObjectType::EDoorHole)
			return true;
	}
	return false;
}

bool UDRFunLibrary::IsBuildDataPointLightType(UBuildingData * BuildData)
{
	if (BuildData)
	{
		if (BuildData->GetObjectType() == EObjectType::EPointLight)
			return true;
	}
	return false;
}

bool UDRFunLibrary::IsBuildDataSpotLightType(UBuildingData * BuildData)
{
	if (BuildData)
	{
		if (BuildData->GetObjectType() == EObjectType::ESpotLight)
			return true;
	}
	return false;
}

bool UDRFunLibrary::IsBuildDataSolidWallType(UBuildingData * BuildData)
{
	if (BuildData)
	{
		if(BuildData->GetObjectType() == EObjectType::ESolidWall)
			return true;
	}
	return false;
}

void UDRFunLibrary::IsBuildDataRoomType(UBuildingData * BuildData, bool & isFloorPlane, bool & isCeilPlane)
{
	isFloorPlane = false;
	isCeilPlane = false;
	if (BuildData)
	{
		if (BuildData->GetObjectType() == EObjectType::EFloorPlane)
			isFloorPlane = true;
		if (BuildData->GetObjectType() == EObjectType::ECeilPlane)
			isCeilPlane = true;
	}
}

//liff add houseplugin type judging interface
bool UDRFunLibrary::IsBuildDataHousePluginType(UBuildingData *BuildData)
{
	if (BuildData)
	{
		int PluginType = EObjectType::EPluginObject + 1;
		int TubeType = PluginType + 1;
		int PillarType = PluginType + 2;
		int SewerType = PluginType + 3;

		if (BuildData->GetObjectType() == PluginType)
		{
			return true;
		}
		else if (BuildData->GetObjectType() == TubeType)
		{
			return true;
		}
		else if (BuildData->GetObjectType() == PillarType)
		{
			return true;
		}
		else if (BuildData->GetObjectType() == SewerType)
		{
			return true;
		}
	}
	return false;
}
//liff add end

void UDRFunLibrary::UpdateModelFileCom_Update(UModelFileComponent * ModelFile, UBuildingData * BuildData, FString ResID)
{
	if (ModelFile && BuildData)
	{
		int32  AlignType = BuildData->GetInt(TEXT("AlignType"));
		UModelFile * _ModelFile = nullptr;
		UResourceMgr * ResMgr = UResourceMgr::GetResourceMgr();
		_ModelFile = Cast<UModelFile>(ResMgr->FindRes(ResID));
		if (ResMgr && _ModelFile)
		{
			int32  AlignType = BuildData->GetInt(TEXT("AlignType"));	
			_ModelFile->ForceLoad();
			ModelFile->SetModelAlignType(ECenterAdjustType(AlignType));
			ModelFile->UpdateModel(_ModelFile);
			ModelFile->BuildData->Update();
		}
	}
}

void UDRFunLibrary::SetHoleHiddent(const UObject* WorldContextObject, bool isShow)
{
	TArray<AActor*> Hole;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AWall_Boolean_Base::StaticClass(), Hole);
	for (int i = 0; i < Hole.Num(); ++i)
	{
		AWall_Boolean_Base * _Hole = Cast<AWall_Boolean_Base>(Hole[i]);
		if (_Hole)
		{
			_Hole->LitUp->SetVisibility(true);
			_Hole->LitUp->SetHiddenInGame(isShow);
		}
	}
}

void UDRFunLibrary::UpdateModelFileChildCom(UModelFileComponent * ModelFile, UBuildingData * BuildData, FString ResID)
{
	if (ModelFile && BuildData)
	{
		UModelFile * _ModelFile = nullptr;
		UResourceMgr * ResMgr = UResourceMgr::GetResourceMgr();
		_ModelFile = Cast<UModelFile>(ResMgr->FindRes(ResID));
		if (ResMgr && _ModelFile)
		{
			//FVector Location = BuildData->GetVector(TEXT("Location"));
			//FVector Forward = BuildData->GetVector(TEXT("Forward"));
			int32  AlignType = BuildData->GetInt(TEXT("AlignType"));
			_ModelFile->ForceLoad();
			ModelFile->SetModelAlignType(ECenterAdjustType(AlignType));
			ModelFile->UpdateModel(_ModelFile);
		}
	}
}

void UDRFunLibrary::GetWallAllHoles(int32 WallID, TArray<int32> & HoleIDs)
{
	///UBuildingSystem * _BS = GetBuildingSystem();
	//if (_BS)
	//{
		//ISuite * IS = _BS->GetSuite();
		//if (IS)
		//{
			//IValue & v = IS->GetProperty(WallID, "Holes");
			//int num = v.GetNumFields();
			//for (int i = 0; i < num; ++i)
			//{
			//	IValue & Hole = v.GetField(i);
			//	IValue & HoleID = Hole.GetField("HoleID");
			//	HoleIDs.Add(HoleID.IntValue());
			//}
		//}
	//}
}

void UDRFunLibrary::SetBuildingSystemWorld(ADRGameMode * GameModel,UBuildingSystem * _Sys)
{
	if(GameModel)
		_Sys->AddToWorld(GameModel->GetWorld());
}

int32 UDRFunLibrary::GetCompatibleWallIndex(EWallSufaceInfoType NewIndexType)
{
	switch (NewIndexType)
	{
	case EWallSufaceInfoType::EBottom:
		return 5;
	case EWallSufaceInfoType::ETop:
		return 4;
	case EWallSufaceInfoType::EFront:
		return 3;
	case EWallSufaceInfoType::EBack:
		return 2;
	case EWallSufaceInfoType::ELeft:
		return 0;
	case EWallSufaceInfoType::ERight:
		return 1;
	}
	return -1;
}


int32 UDRFunLibrary::NewWallIndexToOldWallIndex(const int32& NewWallIndex)
{
	switch (NewWallIndex)
	{
	case 0:
		return 5;
	case 1:
		return 4;
	case 2:
		return 3;
	case 3:
		return 2;
	case 4:
		return 1;
	case 5:
		return 0;
	}
	return -1;
}

int32 UDRFunLibrary::OldWallIndexToNewWallIndex(const int32& OldIndex)
{
	switch (OldIndex)
	{
	case 0:
		return 5;
	case 1:
		return 4;
	case 2:
		return 3;
	case 3:
		return 2;
	case 4:
		return 1;
	case 5:
		return 0;
	}
	return -1;
}

EWallSufaceInfoType UDRFunLibrary::GetWallSurfaceTypeByIndex(const int32& Index)
{
	switch (Index)
	{
	case 5:
		return EWallSufaceInfoType::ELeft;
	case 4:
		return EWallSufaceInfoType::ERight;
	case 2:
		return EWallSufaceInfoType::EBack;
	case 3:
		return EWallSufaceInfoType::EFront;
	case 0:
		return EWallSufaceInfoType::EBottom;
	case 1:
		return EWallSufaceInfoType::ETop;
	}
	return EWallSufaceInfoType::EBottom;
}

UMaterialInterface* UDRFunLibrary::GetMaterialInterfaceByResID(FString ResID)
{
	UMaterialInterface* UE4Material = nullptr;
	UResource *Resource = UResourceMgr::GetResourceMgr()->FindRes(ResID, true);
	if (Resource)
	{
		Resource->ForceLoad();
	}
	USurfaceFile *Surface = Cast<USurfaceFile>(Resource);
	if (Surface)
	{
		UE4Material = Surface->GetUE4Material();
	}
	UModelFile *ModelFile = Cast<UModelFile>(Resource);
	if (ModelFile)
	{
		UE4Material = ModelFile->GetUE4Material(0);
	}
	if (!UE4Material)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find��mat By ResID[%s]"), *ResID);
	}
	return 	 UE4Material;
}

float UDRFunLibrary::GetWallSurfaceActeage(ALineWallActor * Wall,int32 SurfaceIndex)
{
	if (Wall)
	{
		float Acreage = 0.f;
		float _Area = 0.f;
		FProcMeshSection* _Array = Wall->WallComp3d->GetProcMeshSection(SurfaceIndex);
		if (_Array)
		{
			int _a, _b, _c;
			const int32 _NUM = _Array->ProcIndexBuffer.Num();
			for (int i = 0; i < _NUM; i += 3)
			{
				_a = _Array->ProcIndexBuffer[i];
				_b = _Array->ProcIndexBuffer[(i + 1) % _NUM];
				_c = _Array->ProcIndexBuffer[(i + 2) % _NUM];

				float _AB = (_Array->ProcVertexBuffer[_a].Position - _Array->ProcVertexBuffer[_b].Position).Size();
				float _BC = (_Array->ProcVertexBuffer[_b].Position - _Array->ProcVertexBuffer[_c].Position).Size();
				float _CA = (_Array->ProcVertexBuffer[_c].Position - _Array->ProcVertexBuffer[_a].Position).Size();
				float p = (_AB + _BC + _CA) / 2.0;
				_Area += FMath::Sqrt(p * (p - _AB) * (p - _BC) * (p - _CA));
			}
			Acreage = _Area;
			Acreage /= 10000.f;
			return Acreage;
		}
	}
	return -1.0f;
}

TArray<FWallBindInfo> UDRFunLibrary::GetWallBingInfo(const UObject* WorldContextObject)
{
	TArray<FWallBindInfo> OutRes;
	TArray<AActor*> _WallRes;
	TArray<AActor*> _RoomRes;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ALineWallActor::StaticClass(), _WallRes);
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ARoomActor::StaticClass(), _RoomRes);
	for (int i = 0; i < _WallRes.Num(); ++i)
	{
		ALineWallActor * _Wall = Cast<ALineWallActor>(_WallRes[i]);
		if (_Wall)
		{
			FWallBindInfo _WallInfo;
			_WallInfo.Wall = _Wall;
			if (_Wall->WallComp3d && _Wall->WallComp3d->GetNumSections() > 5)
			{
				for (int j = 2; j < 6; ++j)
				{
					FProcMeshSection * _WallM = _Wall->WallComp3d->GetProcMeshSection(j);
					if (_WallM && _WallM->ProcVertexBuffer.Num() != 0)
					{
						FVector _Loc = FVector::ZeroVector;
						for (int k = 0; k < _WallM->ProcVertexBuffer.Num(); ++k)
						{
							_Loc += _WallM->ProcVertexBuffer[k].Position;
						}
						FVector _SLoc = _Loc / _WallM->ProcVertexBuffer.Num();
						_Loc = _SLoc + (_WallM->ProcVertexBuffer[0].Normal * ERROR_DIRECT);
						for (int k = 0; k < _RoomRes.Num(); ++k)
						{
							ARoomActor * _Room = Cast<ARoomActor>(_RoomRes[k]);
							if (_Room)
							{
								FProcMeshSection * _RoomM = _Room->PGround->GetProcMeshSection(0);
								if (_RoomM)
								{
									TArray<FVector> _Polygons;
									for (int m = 0; m < _RoomM->ProcVertexBuffer.Num(); ++m)
									{
										_Polygons.Add(_RoomM->ProcVertexBuffer[m].Position);
									}
									if (UDRFunLibrary::IsPointInRoom(_Polygons, _Loc))
									{
#if ERROR_DUBUG_ON_OFF
										UKismetSystemLibrary::DrawDebugLine((UObject*)WorldContextObject, _SLoc, _Loc, FLinearColor(1.f, 1.f, 0.f, 1.f), ERROR_DUBUG_LineTime, 5.f);
#endif
										FWallSufaceInfo _WallFaceInfo;
										_WallFaceInfo.OwnerRoom = _Room;
										_WallFaceInfo.OwnerWall = _Wall;
										_WallFaceInfo.SurFaceIndex = j;
										_WallFaceInfo.UpdateSurfaceData();
										_WallInfo.SurFaces.Add(_WallFaceInfo);
									}
								}
							}
						}
					}
				}
			}
			OutRes.Add(_WallInfo);
		}
	}
	return OutRes;
}

FWallBindInfo UDRFunLibrary::GetWallBingInfoByWall(const ALineWallActor* Wall)
{
	FWallBindInfo Result;
	Result.Wall = nullptr;
	TArray<FWallBindInfo> OutRes = GetWallBingInfo(Wall);
	if (OutRes.Num() > 0)
	{
		for (FWallBindInfo& Item : OutRes)
		{
			if (Item.Wall == Wall)
			{
				Result.Wall = Item.Wall;
				for (FWallSufaceInfo& SurfaceItem : Item.SurFaces)
				{
					Result.SurFaces.Add(SurfaceItem);
				}
			}
		}
	}

	return Result;
}

TArray<FRoomBindInfo> UDRFunLibrary::GetRoomBindInfo(const UObject* WorldContextObject)
{
	TArray<FRoomBindInfo> OutRes;
	TArray<AActor*> Res;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ARoomActor::StaticClass(), Res);
	for (int i = 0; i < Res.Num(); ++i)
	{
		ARoomActor * _Room = Cast<ARoomActor>(Res[i]);
		if (_Room)
		{
			FRoomBindInfo _BindInfo;
			_BindInfo.Room = _Room;
			_BindInfo.UpdateSurfaceData();
			TArray<FVector> _Polygons; _Polygons.Empty();			
			/*
			
			FProcMeshSection * _RoomM = _Room->PGround->GetProcMeshSection(0);
			if (_RoomM)
			{
				for (int j = 0; j < _RoomM->ProcVertexBuffer.Num(); ++j)
				{
					_Polygons.Add(_RoomM->ProcVertexBuffer[j].Position);
				}
			}
			*/
			//解决区域关联墙体计算不全的问题
			UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WorldContextObject);
			if (projectDataManager != nullptr)
			{
				UDRAreaAdapter* AreaAdapter = Cast<UDRAreaAdapter>(projectDataManager->GetAdapter(_Room->ObjectID));
				TArray<FVector2D> AreaPointList = AreaAdapter->GetPointList();
				for (int j = 0; j < AreaPointList.Num(); ++j)
				{
					
					_Polygons.Add(FVector(AreaPointList[j], 0));
				}
			}
			
			TArray<AActor*> WallRes;
			UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ALineWallActor::StaticClass(), WallRes);
			for (int index = 0; index < WallRes.Num(); ++index)
			{
				ALineWallActor * _Wall = Cast<ALineWallActor>(WallRes[index]);
				if (_Wall)
				{
					FWallBindInfo _WallInfo;
					_WallInfo.Wall = _Wall;
					
					for (int j = 2; j < 6; ++j)
					{
						FProcMeshSection * _WallM = _Wall->WallComp3d->GetProcMeshSection(j);
						if (_WallM && _WallM->ProcVertexBuffer.Num() != 0)
						{
							FVector _Loc = FVector::ZeroVector;
							for (int k = 0 ; k < _WallM->ProcVertexBuffer.Num() ; ++k)
							{
								_Loc += _WallM->ProcVertexBuffer[k].Position;
							}
							if(_WallM->ProcVertexBuffer.Num() == 0)	continue;
							FVector _SLoc = _Loc / _WallM->ProcVertexBuffer.Num();
							_Loc = _SLoc + (_WallM->ProcVertexBuffer[0].Normal * ERROR_DIRECT);
							
							if (UDRFunLibrary::IsPointInRoom(_Polygons, _Loc))
							{
#if ERROR_DUBUG_ON_OFF
								UKismetSystemLibrary::DrawDebugLine((UObject*)WorldContextObject, _SLoc, _Loc, FLinearColor(1.f, 1.f, 0.f, 1.f), ERROR_DUBUG_LineTime, 5.f);
#endif
								FWallSufaceInfo _WallFaceInfo;
								_WallFaceInfo.OwnerRoom = _Room;
								_WallFaceInfo.OwnerWall = _Wall;
								_WallFaceInfo.SurFaceIndex = j;
								_WallFaceInfo.UpdateSurfaceData();
								_WallInfo.SurFaces.Add(_WallFaceInfo);
							}
						}
					}
					_BindInfo.Walls.Add(_WallInfo);
				}
			}
			OutRes.Add(_BindInfo);
		}
	}
	return OutRes;
}

FRoomBindInfo UDRFunLibrary::GetRoomBindInfoByRomm(const ARoomActor* Room)
{
	FRoomBindInfo Result;
	Result.Room = nullptr;
	Result.Acreage = 0.0f;

	if (Room)
	{
		TArray<FRoomBindInfo> OutRes = GetRoomBindInfo(Room);
		if (OutRes.Num() > 0)
		{
			for (FRoomBindInfo& Item : OutRes)
			{
				if (Room == Item.Room)
				{
					Result.Room = Item.Room;
					Result.Acreage = Item.Acreage;
					for (FWallBindInfo WallItem : Item.Walls)
					{
						Result.Walls.Add(WallItem);
					}
				}
			}
		}
	}
	return Result;
}

float UDRFunLibrary::GetWallAcreageByRoom(const ARoomActor* Room)
{
	FRoomBindInfo RoomInfo = GetRoomBindInfoByRomm(Room);
	float Result = 0.0f;
	if (RoomInfo.Walls.Num() > 0)
	{
		for (FWallBindInfo& Item : RoomInfo.Walls)
		{
			for (FWallSufaceInfo& Surface : Item.SurFaces)
			{
				Result += Surface.Acreage;
			}
		}
	}
	return Result;
}

FRoomBindInfo UDRFunLibrary::GetDesignationRoomBindInfo(const UObject* WorldContextObject, ARoomActor * Room)
{
	FRoomBindInfo _BindInfo;
	_BindInfo.Room = nullptr;
	if (Room)
	{
		//FRoomBindInfo _BindInfo;
		_BindInfo.Room = Room;
		_BindInfo.UpdateSurfaceData();
		TArray<FVector> _Polygons; 
		_Polygons.Empty();			
		/*FProcMeshSection * _RoomM = Room->PGround->GetProcMeshSection(0);
		if (_RoomM)
		{
			for (int j = 0; j < _RoomM->ProcVertexBuffer.Num(); ++j)
			{
				_Polygons.Add(_RoomM->ProcVertexBuffer[j].Position);
			}
		}*/

		UDRProjData* projectDataManager = UDRProjData::GetProjectDataManager(WorldContextObject);
		if (projectDataManager != nullptr)
		{
			UDRAreaAdapter* AreaAdapter = Cast<UDRAreaAdapter>(projectDataManager->GetAdapter(Room->ObjectID));
			TArray<FVector2D> AreaPointList = AreaAdapter->GetPointList();
			for (int j = 0; j < AreaPointList.Num(); ++j)
			{
				_Polygons.Add(FVector(AreaPointList[j], 0));
			}
		}

		TArray<AActor*> WallRes;
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ALineWallActor::StaticClass(), WallRes);
		for (int i = 0; i < WallRes.Num(); ++i)
		{
			ALineWallActor * _Wall = Cast<ALineWallActor>(WallRes[i]);
			if (_Wall)
			{
				FWallBindInfo _WallInfo;
				_WallInfo.Wall = _Wall;
				for (int j = 2; j < 6; ++j)
				{
					FProcMeshSection * _WallM = _Wall->WallComp3d->GetProcMeshSection(j);
					if (_WallM && _WallM->ProcVertexBuffer.Num() != 0)
					{
						FVector _Loc = FVector::ZeroVector;
						for (int k = 0; k < _WallM->ProcVertexBuffer.Num(); ++k)
						{
							_Loc += _WallM->ProcVertexBuffer[k].Position;
						}
						FVector _SLoc = _Loc / _WallM->ProcVertexBuffer.Num();
						_Loc = _SLoc + (_WallM->ProcVertexBuffer[0].Normal * ERROR_DIRECT);

						if (UDRFunLibrary::IsPointInRoom(_Polygons, _Loc))
						{
#if ERROR_DUBUG_ON_OFF
							UKismetSystemLibrary::DrawDebugLine((UObject*)WorldContextObject, _SLoc, _Loc, FLinearColor(1.f, 1.f, 0.f, 1.f), ERROR_DUBUG_LineTime, 5.f);
#endif
							FWallSufaceInfo _WallFaceInfo;
							_WallFaceInfo.OwnerRoom = Room;
							_WallFaceInfo.OwnerWall = _Wall;
							_WallFaceInfo.SurFaceIndex = j;
							_WallFaceInfo.UpdateSurfaceData();
							_WallInfo.SurFaces.Add(_WallFaceInfo);
							_BindInfo.Walls.Add(_WallInfo);
						}
					}
				}
			}

		}
	}
	return _BindInfo;
}

FWallBindInfo UDRFunLibrary::GetDesignationWallBindInfo(const UObject* WorldContextObject, ALineWallActor * Wall)
{
	FWallBindInfo OutRes;
	OutRes.Wall = nullptr;
	if (Wall)
	{
		OutRes.Wall = Wall;
		TArray<AActor*> _Rooms;
		TArray<FVector> _Polygons;
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ARoomActor::StaticClass(), _Rooms);
		for (int i = 0; i < _Rooms.Num(); ++i)
		{
			ARoomActor * _Room = Cast<ARoomActor>(_Rooms[i]);
			if (_Room)
			{
				_Polygons.Empty();
				FProcMeshSection * _RoomM = _Room->PGround->GetProcMeshSection(0);
				if (_RoomM)
				{
					for (int j = 0; j < _RoomM->ProcVertexBuffer.Num(); ++j)
					{
						_Polygons.Add(_RoomM->ProcVertexBuffer[j].Position);
					}
					for (int j = 2; j < 6; ++j)
					{
						FProcMeshSection * _WallM = Wall->WallComp3d->GetProcMeshSection(j);
						if (_WallM && _WallM->ProcVertexBuffer.Num() != 0)
						{
							FVector _Loc = FVector::ZeroVector;
							for (int k = 0; k < _WallM->ProcVertexBuffer.Num(); ++k)
							{
								_Loc += _WallM->ProcVertexBuffer[k].Position;
							}
							FVector _SLoc = _Loc / _WallM->ProcVertexBuffer.Num();
							_Loc = _SLoc + (_WallM->ProcVertexBuffer[0].Normal * ERROR_DIRECT);

							if (UDRFunLibrary::IsPointInRoom(_Polygons, _Loc))
							{
#if ERROR_DUBUG_ON_OFF
								UKismetSystemLibrary::DrawDebugLine((UObject*)WorldContextObject, _SLoc, _Loc, FLinearColor(1.f, 1.f, 0.f, 1.f), ERROR_DUBUG_LineTime, 5.f);
#endif
								FWallSufaceInfo _WallFaceInfo;
								_WallFaceInfo.OwnerRoom = _Room;
								_WallFaceInfo.OwnerWall = Wall;
								_WallFaceInfo.SurFaceIndex = j;
								_WallFaceInfo.UpdateSurfaceData();
								OutRes.SurFaces.Add(_WallFaceInfo);
							}
							else
							{
								FWallSufaceInfo _WallFaceInfo;
								_WallFaceInfo.OwnerRoom = nullptr;
								_WallFaceInfo.OwnerWall = Wall;
								_WallFaceInfo.SurFaceIndex = j;
								_WallFaceInfo.UpdateSurfaceData();
								OutRes.SurFaces.Add(_WallFaceInfo);
							}
						}
					}
				}
			}
		}
	}
	return OutRes;
}

FWallBindInfo UDRFunLibrary::GetDesignationWallSurfaceAcreage(const UObject* WorldContextObject, ALineWallActor * Wall)
{
	FWallBindInfo OutRes;
	for (int j = 0; j < 6; ++j)
	{
		FProcMeshSection * _WallM = Wall->WallComp3d->GetProcMeshSection(j);
		if (_WallM)
		{
			FWallSufaceInfo _WallFaceInfo;
			_WallFaceInfo.OwnerRoom = nullptr;
			_WallFaceInfo.OwnerWall = Wall;
			_WallFaceInfo.SurFaceIndex = j;
			_WallFaceInfo.UpdateSurfaceData();
			OutRes.SurFaces.Add(_WallFaceInfo);
		}
	}
	return OutRes;
}

FRoomBindInfo UDRFunLibrary::GetRoomByHitWallIndex(ALineWallActor* Wall, const int32& HitWallIndex)
{
	FRoomBindInfo Result;
	Result.Room = nullptr;
	FWallBindInfo WallInfo = GetDesignationWallBindInfo(Wall->GetWorld(), Wall);
	if (WallInfo.SurFaces.Num() > 0)
	{
		for (FWallSufaceInfo& Surface : WallInfo.SurFaces)
		{
			if (Surface.SurFaceIndex == HitWallIndex)
			{
				if(Surface.OwnerRoom)
				{ 
					return GetDesignationRoomBindInfo(Wall->GetWorld(), Surface.OwnerRoom);
				}
			}
		}
	}

	return Result;
}

bool UDRFunLibrary::IsPointInRoom(const  TArray<FVector> & Vertex, const FVector & Point)
{
	TArray<FVector2D> Pnts2D;
	for (int i = 0; i < Vertex.Num(); ++i)
	{
		Pnts2D.Add(FVector2D(Vertex[i]));
	}
	return FPolygonAlg::JudgePointInPolygon(Pnts2D,FVector2D(Point));
}

FWallCollectionHandle UDRFunLibrary::GetWallSectionCollection(const FWallSufaceInfo& WallSurfaceInfo, const FDRMaterial& SurfaceMatNode)
{
	FWallCollectionHandle WallCollectionHandle;
	WallCollectionHandle.WallArea = WallSurfaceInfo.Acreage;
	WallCollectionHandle.FaceType = UDRFunLibrary::WallType2SurfaceType(WallSurfaceInfo.Type);
	WallCollectionHandle.WallMaterial.ResID = TEXT("");
	WallCollectionHandle.WallMaterial.ModelID = SurfaceMatNode.ModelID;
	WallCollectionHandle.WallMaterial.Url = TEXT("");
	WallCollectionHandle.WallMaterial.RoomClassID = SurfaceMatNode.RoomClassID;
	WallCollectionHandle.WallMaterial.CraftID = SurfaceMatNode.CraftID;

	return WallCollectionHandle;
}

EWallSurfaceType UDRFunLibrary::WallType2SurfaceType(EWallSufaceInfoType SurfaceType)
{
	switch (SurfaceType)
	{
		case EWallSufaceInfoType::EBottom:	return EWallSurfaceType::Bottom;
		case EWallSufaceInfoType::ETop:		return EWallSurfaceType::Top;
		case EWallSufaceInfoType::EBack:	return EWallSurfaceType::StartSide;
		case EWallSufaceInfoType::EFront:	return EWallSurfaceType::EndSide;
		case EWallSufaceInfoType::ERight:	return EWallSurfaceType::Front;
		case EWallSufaceInfoType::ELeft:	return EWallSurfaceType::Back;
		default:	return EWallSurfaceType::StartSide;
	}
}

TArray<FVector2D> UDRFunLibrary::GetVerticesBySectionIndex(UProceduralMeshComponent* PMesh, int32 SectionIndex)
{
	TArray<FVector2D> Result;
	if (PMesh)
	{
		if (SectionIndex < (PMesh->GetNumSections()))
		{
			FProcMeshSection *MeshSect = PMesh->GetProcMeshSection(SectionIndex);
			if (MeshSect)
			{
				TArray<FProcMeshVertex> PMVertex = MeshSect->ProcVertexBuffer;
				for (FProcMeshVertex &Item : PMVertex)
				{
					Result.Add(FVector2D(Item.Position.X, Item.Position.Y));
				}
			}
		}
	}
	return Result;
}

int32 UDRFunLibrary::GetNearSection(UProceduralMeshComponent* AreaMesh, UProceduralMeshComponent* WallMesh, int32 SectionIndex)
{
	if ((!AreaMesh) || (!WallMesh))	return -1;
	TArray<FVector2D> AreaVertexs = UDRFunLibrary::GetVerticesBySectionIndex(AreaMesh, 0);
	using FExactPWithExactC = FCgDataConvUtil<CGAL::Exact_predicates_exact_constructions_kernel>;
	using FExactPolygon = FExactPWithExactC::FP2CPolygon2D;
	using FE2EDataConv = FCgDataConvUtil<CGAL::Exact_predicates_exact_constructions_kernel>;
	FExactPolygon P2d;
	for (int i = 0; i < AreaVertexs.Num(); ++i)
	{
		P2d.push_back(FE2EDataConv::Vector2D2CgPointConv(AreaVertexs[i]));
	}
	for (int i = 0; i < WallMesh->GetNumSections(); i++)
	{
		TArray<FVector2D> WallVertexs = UDRFunLibrary::GetVerticesBySectionIndex(WallMesh, i);
		FVector2D Sum = FVector2D(0, 0);
		for (FVector2D& Item : WallVertexs)
		{
			Sum += Item;
		}
		FVector2D Center = Sum / (1.0 * WallVertexs.Num());
		bool onbound = P2d.has_on_boundary(FE2EDataConv::Vector2D2CgPointConv(Center));
		bool inside = P2d.has_on_bounded_side(FE2EDataConv::Vector2D2CgPointConv(Center));
		if (onbound || inside)
		{
			return i;
		}
	}

	return -1;
}

TArray<int32> UDRFunLibrary::GetNearSectionList(UProceduralMeshComponent* AreaMesh, UProceduralMeshComponent* WallMesh)
{
	TArray<int32> Result;
	if ((!AreaMesh) || (!WallMesh))	return Result;
	if (WallMesh->GetNumSections() > 0)
	{
		for (int i = 0; i < WallMesh->GetNumSections(); i++)
		{
			int NearSection = GetNearSection(AreaMesh, WallMesh, i);
			if (NearSection >= 0)
			{
				Result.Add(NearSection);
			}
		}
	}
	return Result;
}

void UDRFunLibrary::SetObjIntValue(UBuildingSystem* BS, const int32& ObjID, FString& ValueName, const int32& IntValue)
{
	if (BS && BS->Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = BS->Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			IValue &Value = VF->Create(IntValue);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UDRFunLibrary::SetObjFloatValue(UBuildingSystem* BS, const int32& ObjID, FString& ValueName, const int32& FloatValue)
{
	if (BS && BS->Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = BS->Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			IValue &Value = VF->Create(FloatValue);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UDRFunLibrary::SetObjFStringValue(UBuildingSystem* BS, const int32& ObjID, FString& ValueName, FString& FStringValue)
{
	if (BS && BS->Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = BS->Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			const char* valuechar = FStringToConstChar(FStringValue);
			IValue &Value = VF->Create(valuechar);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UDRFunLibrary::SetObjFVector(UBuildingSystem* BS, const int32& ObjID, FString& ValueName, FVector& FVectorValue)
{
	if (BS && BS->Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = BS->Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			kVector3D KValue = ToBuildingVector(FVectorValue);
			IValue &Value = VF->Create(&KValue, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UDRFunLibrary::SetObjFVector2D(UBuildingSystem* BS, const int32& ObjID, FString& ValueName, FVector2D& FVectorValue)
{
	if (BS && BS->Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = BS->Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			kPoint KValue = ToBuildingPosition(FVectorValue);
			IValue &Value = VF->Create(&KValue, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

void UDRFunLibrary::SetObjFVector4D(UBuildingSystem* BS, const int32& ObjID, FString& ValueName, FVector4& FVectorValue)
{
	if (BS && BS->Suite)
	{
		IValueFactory* VF = UBuildingSystem::GetValueFactory();
		IObject* Obj = BS->Suite->GetObject(ObjID);
		if (VF && Obj)
		{
			kVector4D KValue = ToBuildingVector4D(FVectorValue);
			IValue &Value = VF->Create(&KValue, true);
			const char *Key = TCHAR_TO_ANSI(*ValueName);
			Obj->SetValue(Key, &Value);
		}
	}
}

UMaterialInterface* UDRFunLibrary::GetUE4MatByResID(const FString& ResID, const int32& MaterialIndex)
{
	UResourceMgr* ResMgr = UResourceMgr::GetResourceMgr();
	UResource* Res = ResMgr->FindRes(ResID, false);
	if (Res)
	{
		Res->ForceLoad();
		UModelFile* ModelFile = Cast<UModelFile>(Res);
		if (ModelFile)
		{
			return ModelFile->GetUE4Material(MaterialIndex);
		}
	}

	return nullptr;
}

void UDRFunLibrary::DrawSubline(FPaintContext& Context, const APlayerController* PC, TArray<FVector> LinePos, FLinearColor Tint, bool bAntiAlias)
{
	int index = 0;
	if(LinePos.Num() < 2)	return;
	int Len = LinePos.Num() / 2 ;
	for (int i = 0; i < Len; i++)
	{
		FVector2D PosA; 
		PC->ProjectWorldLocationToScreen(LinePos[index], PosA, false);
		FVector2D PosB;
		PC->ProjectWorldLocationToScreen(LinePos[index+1], PosB, false);
		UWidgetBlueprintLibrary::DrawLine(Context, PosA, PosB, Tint, bAntiAlias);
		index += 2;
	}
}

bool UDRFunLibrary::IsPolyClockWise(const TArray<FVector2D> & Polygons)
{
	double d = 0.f;

	for (int i = 0; i < Polygons.Num(); i++)
	{
		d += -0.5f * ((Polygons[(i + 1) % Polygons.Num()].Y + Polygons[i].Y) * (Polygons[(i + 1) % Polygons.Num()].X - Polygons[i].X));
	}
	return d < 0.f;
}

void UDRFunLibrary::ChangeClockwise(const TArray<FVector2D> & InPolygons, TArray<FVector2D> & OutPolygons)
{
	TArray<FVector2D> Temp;
	Temp.Add(InPolygons[0]);
	for (int i = InPolygons.Num() - 1; i > 0; --i)
	{
		Temp.Add(InPolygons[i]);
	}
	OutPolygons.Reserve(Temp.Num());
	OutPolygons = Temp;
}

void UDRFunLibrary::SetModelBuildingData(AFurnitureModelActor* FurnitureModelActor, UBuildingData* BuildingData)
{
	if(FurnitureModelActor)
	{
		FurnitureModelActor->BuildingData = BuildingData;
	}
}

void UDRFunLibrary::SetPointLightSturct(AFurnitureModelActor* FurnitureModelActor, const FDRPointLight& PointLightStruct)
{
	if (FurnitureModelActor)
	{
		FurnitureModelActor->PointLightStruct = PointLightStruct;
		//FMemory::Memcpy(&(FurnitureModelActor->ModelStruct), &ModelStruct, sizeof(ModelStruct));
	}
}

void UDRFunLibrary::SetSpotLightSturct(AFurnitureModelActor* FurnitureModelActor, const FDRSpotLight& SpotLightStruct)
{
	if (FurnitureModelActor)
	{
		FurnitureModelActor->SpotLightStruct = SpotLightStruct;
		//FMemory::Memcpy(&(FurnitureModelActor->ModelStruct), &ModelStruct, sizeof(ModelStruct));
	}
}

void UDRFunLibrary::CalculateBooleanMaxMinLoction(const TArray<FVector>&WallNodes, FVector& _OutStartPos, FVector& _OutEndPos)
{
	_OutStartPos = _OutEndPos = FVector::ZeroVector;
	if (WallNodes.Num() == 6)
	{
		FVector LeftLineStart = UKismetMathLibrary::FindClosestPointOnSegment(WallNodes[1], WallNodes[0], WallNodes[3]);
		FVector RightLineStart = UKismetMathLibrary::FindClosestPointOnSegment(WallNodes[5], WallNodes[0], WallNodes[3]);
		FVector LeftLineEnd = UKismetMathLibrary::FindClosestPointOnSegment(WallNodes[2], WallNodes[0], WallNodes[3]);
		FVector RightLineEnd = UKismetMathLibrary::FindClosestPointOnSegment(WallNodes[4], WallNodes[0], WallNodes[3]);
		if ((LeftLineStart - WallNodes[0]).Size() - (RightLineStart - WallNodes[0]).Size() > 0)
		{
			_OutStartPos = LeftLineStart;
		}
		else
			_OutStartPos = RightLineStart;
		if ((LeftLineEnd - WallNodes[3]).Size() - (RightLineEnd - WallNodes[3]).Size() > 0)
		{
			_OutEndPos = LeftLineEnd;
		}
		else
			_OutEndPos = RightLineEnd;
	}
}

TArray<ALineWallActor*> UDRFunLibrary::GetWallActorArray(UObject* WorldContext)
{
	TArray<ALineWallActor*> Result;
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(WorldContext, ALineWallActor::StaticClass(), Actors);
	for (AActor* &Item : Actors)
	{
		ALineWallActor* LineWallActor = Cast<ALineWallActor>(Item);
		Result.Add(LineWallActor);
	}
	return Result;
}

TArray<ACornerActor*> UDRFunLibrary::GetCornerActorArray(UObject* WorldContext)
{
	TArray<ACornerActor*> Result;
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(WorldContext, ALineWallActor::StaticClass(), Actors);
	for (AActor* &Item : Actors)
	{
		ACornerActor* CornerActor = Cast<ACornerActor>(Item);
		Result.Add(CornerActor);
	}
	return Result;
}

TArray<ARoomActor*> UDRFunLibrary::GetRoomActorArray(UObject* WorldContext)
{
	TArray<ARoomActor*> Result;
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(WorldContext, ARoomActor::StaticClass(), Actors);
	for (AActor* &Item : Actors)
	{
		ARoomActor* RoomActor = Cast<ARoomActor>(Item);
		Result.Add(RoomActor);
	}
	return Result;
}

void UDRFunLibrary::AddRoomIndex(const int32 & Index, const FString & Key, const TMap<FString, FRoomList>& Map, ARoomActor * R)
{
	if (Key.IsEmpty()) return;
	if (!R) return;
	const FRoomList * _R = Map.Find(Key);
	if (_R)
	{
		((FRoomList*)_R)->AddInitRoom(Index,*((FString *)&Key), R);
	}
	else
	{
		FString _Name = *((FString *)&Key);
		FRoomList _RoomList;
		_RoomList.AddInitRoom(Index,*((FString *)&Key), R);
		((TMap<FString, FRoomList>*)&Map)->Add(_Name, _RoomList);
	}
}

void UDRFunLibrary::AddRoom(const FString & Key, const TMap<FString, FRoomList>& Map, ARoomActor * R)
{
	if (Key.IsEmpty()) return;
	if (!R) return;
	const FRoomList * _R = Map.Find(Key);
	if (_R)
		((FRoomList*)_R)->AddRoom(*((FString *)&Key),R);
	else
	{
		FString _Name = *((FString *)&Key);
		FRoomList _RoomList;
		_RoomList.AddRoom(*((FString *)&Key),R);
		((TMap<FString, FRoomList>*)&Map)->Add(_Name, _RoomList);
	}
}

void UDRFunLibrary::DelRoom(const FString & Key, const TMap<FString, FRoomList>& Map, ARoomActor * R)
{
	if (Key.IsEmpty()) return;
	if (!R) return;
	const FRoomList * _R = Map.Find(Key);
	if (_R)
		((FRoomList*)_R)->DelRoom(R);
}

void UDRFunLibrary::ChaRoom(const FString & SrcKey, const FString & Key, const TMap<FString, FRoomList>& Map, ARoomActor * R)
{
	if (!R) return;
	
	const FRoomList * _R = Map.Find(SrcKey);
	if (!SrcKey.IsEmpty() && _R )
	{
		((FRoomList*)_R)->DelRoom(R);
	}
	if (Key.IsEmpty()) return;
	_R = Map.Find(Key);
	if (_R)
	{
		((FRoomList*)_R)->AddRoom(*((FString *)&Key),R);
	}
	else
	{
		FString _Name = *((FString *)&Key);
		FRoomList _RoomList;
		_RoomList.AddRoom(*((FString *)&Key),R);
		((TMap<FString, FRoomList>*)&Map)->Add(_Name, _RoomList);
	}
}

#pragma optimize("", on)

FString UDRFunLibrary::FloatToStringBy3Point(float num)
{
	bool IsNegativ = false;
	FString Symbol = "";
	if (num < 0.0)
	{
		num *= (-1.0);
		IsNegativ = true;
		Symbol.Append("-");
	}

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

	Symbol.Append(OutFloat);

	return Symbol;
}

TArray<AActor*> UDRFunLibrary::GetAllHousePlugins(const UObject* WorldContextObject)
{
	TArray<AActor*> OutActor;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AComponentManagerActor::StaticClass(), OutActor);
	return OutActor;
}

float UDRFunLibrary::GetWallRotate(FVector2D StartPos, FVector2D EndPos)
{
	FVector2D Temp = StartPos + FVector2D(0, 100);
	float theta = atan2(Temp.X - StartPos.X, Temp.Y - StartPos.Y) - atan2(EndPos.X - StartPos.X, EndPos.Y - StartPos.Y);
	if (theta > PI)
		theta -= 2 * PI;
	if (theta < -PI)
		theta += 2 * PI;
	return theta * 180.0 / PI;
}