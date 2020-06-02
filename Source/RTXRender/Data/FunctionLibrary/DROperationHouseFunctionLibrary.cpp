// Copyright? 2017 ihomefnt All Rights Reserved.


#include "DROperationHouseFunctionLibrary.h"
#include "Data/DRProjData.h"
#include "Adapter/DRSolidWallAdapter.h"
#include "Adapter/DRAreaAdapter.h"
#include "Adapter/DRDoorHoleAdapter.h"
#include "Adapter/DRWindowHoleAdapter.h"
#include"Adapter/DRWindowAdapter.h"
#include"Adapter/DRDoorAdapter.h"
#include"Adapter/DRCornerAdapter.h"
UCEditorGameInstance* UDROperationHouseFunctionLibrary::GameInstance = nullptr;

#define CONVERSIONUNIT 10000;

void UDROperationHouseFunctionLibrary::CopyDRSolidWallData(TArray<FDRSolidWall>& SolidWallData)
{
	SolidWallData.Empty();
	MergeShortWallData(SolidWallData);
	/*if (GameInstance&&GameInstance->GetBuildingSystemInstance())
	{
		TArray<int32>WallIDs;
		WallIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_SolidWall);
		for (int i = 0; i < WallIDs.Num(); ++i)
		{
			FDRSolidWall Local_SolidWall;
			if (GetWallStruct(WallIDs[i], Local_SolidWall))
			{
				SolidWallData.Add(Local_SolidWall);
			}
		}
	}*/
}

void UDROperationHouseFunctionLibrary::CopyDRAreaData(TArray<FPreAreaData>& AreData)
{
	AreData.Empty();
	if (GameInstance&&GameInstance->GetBuildingSystemInstance())
	{
		TArray<int32>AreaIDs;
		AreaIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_Room);
		for (int i = 0; i < AreaIDs.Num(); ++i)
		{
			FPreAreaData Local_AreaWall;
			if (GetAreaStruct(AreaIDs[i], Local_AreaWall))
			{
				AreData.Add(Local_AreaWall);
			}
		}
		TArray<int32>PolygonAreaIDs;
		PolygonAreaIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_PolygonArea);
		for (int i = 0; i < PolygonAreaIDs.Num(); ++i)
		{
			FPreAreaData Local_AreaWall;
			if (GetAreaStruct(PolygonAreaIDs[i], Local_AreaWall))
			{
				AreData.Add(Local_AreaWall);
			}
		}
	}
}

void UDROperationHouseFunctionLibrary::GetAllHoleIDs(TArray<int32>& HoleIDs)
{
	if (GameInstance&& GameInstance->GetBuildingSystemInstance())
	{
		HoleIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_DoorHole);
		TArray<int32>WindowHoleIDs;
		WindowHoleIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_Window);
		HoleIDs.Append(WindowHoleIDs);
	}
}

void UDROperationHouseFunctionLibrary::CopyDRHoleByID(const TArray<int32>& HoleIDs, TArray<FDRHole>& HoleData)
{
	for (int i = 0; i < HoleIDs.Num(); ++i)
	{
		FDRHole Local_Hole;
		if (GetHoleStruct(HoleIDs[i], Local_Hole))
		{
			HoleData.Add(Local_Hole);
		}
	}
}

void UDROperationHouseFunctionLibrary::CopyDRHoleData(TArray<FDRHole>& HoleData)
{
	HoleData.Empty();
	if (GameInstance&&GameInstance->GetBuildingSystemInstance())
	{
		TArray<int32>HoleIDs;
		GetAllHoleIDs(HoleIDs);
		for (int i = 0; i < HoleIDs.Num(); ++i)
		{
			FDRHole Local_Hole;
			if (GetHoleStruct(HoleIDs[i], Local_Hole))
			{
				HoleData.Add(Local_Hole);
			}
		}
	}
}

void UDROperationHouseFunctionLibrary::CopyDRHoleModelData(TArray<FDRDoorAndWindow>& HoleModelData)
{
	HoleModelData.Empty();
	if (GameInstance&&GameInstance->GetBuildingSystemInstance())
	{
		TArray<int32>HoleIDs;
		HoleIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_DoorHole);
		for (int i = 0; i < HoleIDs.Num(); ++i)
		{
			UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
			if (Local_DRProjData)
			{
				UDRDoorHoleAdapter* Local_Holedapter = Cast<UDRDoorHoleAdapter>(Local_DRProjData->GetAdapter(HoleIDs[i]));
				if (Local_Holedapter)
				{
					int HoleModelID = Local_Holedapter->GetDoorFrameObjId();
					int DoorID = Local_Holedapter->GetDoorObjId();
					if (HoleModelID > 0)
					{
						FDRDoorAndWindow Local_HoleModel;
						if (GetDoorStruct(HoleModelID, DoorID, Local_HoleModel))
						{
							HoleModelData.Add(Local_HoleModel);
						}
					}
				}
			}
		}
		HoleIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_Window);
		for (int i = 0; i < HoleIDs.Num(); ++i)
		{
			UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
			if (Local_DRProjData)
			{
				UDRWindowHoleAdapter* Local_Holedapter = Cast<UDRWindowHoleAdapter>(Local_DRProjData->GetAdapter(HoleIDs[i]));
				if (Local_Holedapter)
				{
					FDRDoorAndWindow Local_Window;
					if (GetWindowStruct(Local_Holedapter->GetWindowObjId(), Local_Window))
					{
						HoleModelData.Add(Local_Window);
					}
				}
			}
		}
	}
}


void UDROperationHouseFunctionLibrary::ClearCopyData()
{
	if (GameInstance)
	{
		GameInstance->TSaveDRSolidWallData.Empty();
		GameInstance->TInitDRSolidWallData.Empty();
		GameInstance->TInitDRAreaData.Empty();
		GameInstance->TSaveDRAreaData.Empty();
		GameInstance->TInitDRHoleData.Empty();
		GameInstance->TSaveDRHoleData.Empty();
		GameInstance->TInitDRRDoorAndWindowData.Empty();
		GameInstance->TSaveDRRDoorAndWindowData.Empty();
	}
}

bool UDROperationHouseFunctionLibrary::GetWallStruct(int32 WallID, FDRSolidWall& Local_SolidWall)
{
	if (GameInstance)
	{
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
		if (Local_DRProjData)
		{
			UDRSolidWallAdapter* Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(WallID));
			if (Local_SolidWallAdapter)
			{
				Local_SolidWall.TagName = Local_SolidWallAdapter->GetTagName();
				Local_SolidWall.StartPos = Local_SolidWallAdapter->GetWallPositionData().StartPos;
				Local_SolidWall.EndPos = Local_SolidWallAdapter->GetWallPositionData().EndPos;
				Local_SolidWall.LeftThick = Local_SolidWallAdapter->GetLeftThick();
				Local_SolidWall.RightThick = Local_SolidWallAdapter->GetRightThick();
				Local_SolidWall.Height = Local_SolidWallAdapter->GetHeight();
				Local_SolidWall.RoomWall = Local_SolidWallAdapter->GetRoomWall();
				Local_SolidWall.MainWall = Local_SolidWallAdapter->GetMainWall();
				Local_SolidWall.RightRuler = Local_SolidWallAdapter->GetRightRuler();
				Local_SolidWall.LeftRuler = Local_SolidWallAdapter->GetLeftRuler();
				Local_SolidWall.SolidWallType = Local_SolidWallAdapter->GetSolidWallType();
				FDRUV LocalDRUV;
				LocalDRUV.UVScale = Local_SolidWallAdapter->GetFrontUVScale();
				LocalDRUV.UVOffset = Local_SolidWallAdapter->GetFrontUVPos();
				LocalDRUV.UVAngle = Local_SolidWallAdapter->GetFrontUVAngle();
				Local_SolidWall.SolidWallUV[0]=LocalDRUV;
				LocalDRUV.UVScale = Local_SolidWallAdapter->GetBackUVScale();
				LocalDRUV.UVOffset = Local_SolidWallAdapter->GetBackUVPos();
				LocalDRUV.UVAngle = Local_SolidWallAdapter->GetBackUVAngle();
				Local_SolidWall.SolidWallUV[1]=LocalDRUV;
				LocalDRUV.UVScale = Local_SolidWallAdapter->GetSideUVScale();
				LocalDRUV.UVOffset = Local_SolidWallAdapter->GetSideUVPos();
				LocalDRUV.UVAngle = Local_SolidWallAdapter->GetSideUVAngle();
				Local_SolidWall.SolidWallUV[2]=LocalDRUV;
				Local_SolidWall.SolidWallMaterials = Local_SolidWallAdapter->GetWallMaterials();
				Local_SolidWall.NewWallType = Local_SolidWallAdapter->GetNewWallType();
				return true;
			}
		}
	}
	return false;
}

bool UDROperationHouseFunctionLibrary::GetAreaStruct(int32 AreaID, FPreAreaData& Local_AreaWall)
{
	if (GameInstance)
	{
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
		if (Local_DRProjData)
		{
			UDRAreaAdapter* Local_AreaAdapter = Cast<UDRAreaAdapter>(Local_DRProjData->GetAdapter(AreaID));
			if (Local_AreaAdapter)
			{
				if (Local_AreaAdapter->GetFloorID() == -1)
					return false;
				Local_AreaWall.VectorList = Local_AreaAdapter->GetPointList();
				Local_AreaWall.TagName = Local_AreaAdapter->GetTagName();
				Local_AreaWall.Height = Local_AreaAdapter->GetHeight();
				Local_AreaWall.RoomId = Local_AreaAdapter->GetRoomID();
				Local_AreaWall.RoomName = Local_AreaAdapter->GetRoomName();
				Local_AreaWall.RoomUsageId = Local_AreaAdapter->GetRoomUsageId();
				Local_AreaWall.RoomUsageName = Local_AreaAdapter->GetRoomUsageName();
				Local_AreaWall.RoomUseIndex = Local_AreaAdapter->GetRoomUseIndex();
				Local_AreaWall.Area = Local_AreaAdapter->GetArea();
				Local_AreaWall.TypeId = Local_AreaAdapter->GetTypeId();
				Local_AreaWall.UV = Local_AreaAdapter->GetFloorUVScale();
				Local_AreaWall.UVLocation = Local_AreaAdapter->GetFloorUVOffset();
				Local_AreaWall.UVRotation = Local_AreaAdapter->GetFloorUVRot();
				Local_AreaWall.AreaMaterials = Local_AreaAdapter->GetAreaMaterials();
				return true;
			}
		}
	}
	return false;
}

bool UDROperationHouseFunctionLibrary::GetHoleStruct(int32 HoleID, FDRHole& Local_Hole)
{
	if (GameInstance)
	{
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
		if (Local_DRProjData)
		{
			UDRHoleAdapter* Local_Holedapter = Cast<UDRHoleAdapter>(Local_DRProjData->GetAdapter(HoleID));
			if (Local_Holedapter)
			{
				Local_Hole.Transform = Local_Holedapter->GetTransform();
				Local_Hole.Width = Local_Holedapter->GetWidth();
				Local_Hole.Height = Local_Holedapter->GetHeight();
				Local_Hole.ZPos = Local_Holedapter->GetGroundHeight();
				Local_Hole.HorizontalFlip = Local_Holedapter->GetHorizontalFlip();
				Local_Hole.VerticalFlip = Local_Holedapter->GetVerticalFlip();
				Local_Hole.SelfTagName = Local_Holedapter->GetSelfTagName();
				Local_Hole.SlotActorTag = Local_Holedapter->GetSlotActorTag();
				Local_Hole.HoleType = Local_Holedapter->GetHoleType();
				Local_Hole.HoleUVs = Local_Holedapter->GetHoleUVs();
				Local_Hole.HoleMaterials = Local_Holedapter->GetHoleMaterials();
				Local_Hole.HolestatusType = Local_Holedapter->GetHoleStatusType();
				return true;
			}
		}
	}
	return false;
}

bool UDROperationHouseFunctionLibrary::GetDoorStruct(int32 DoorFrameID, int32 DoorID, FDRDoorAndWindow& Local_Door)
{
	if (DoorFrameID > 0)
	{
		if (GameInstance)
		{
			UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
			if (Local_DRProjData)
			{
				UDRModelInstanceAdapter* Local_HoledModelapter = Cast<UDRModelInstanceAdapter>(Local_DRProjData->GetAdapter(DoorFrameID));
				if (Local_HoledModelapter)
				{
					Local_Door.ModelType = EModelType::EDRDoorWay;
					Local_Door.Loction = Local_HoledModelapter->GetLocation();
					Local_Door.ModelIds[1] = Local_HoledModelapter->GetModelID();
					Local_Door.RoomClassIds[1] = Local_HoledModelapter->GetRoomClassIdByNormalModel();
					Local_Door.CraftIDs[1] = Local_HoledModelapter->GetCraftIdByNormalModel();
					Local_Door.TagName = Local_HoledModelapter->GetModelTag();
					Local_Door.DependsMats[1] = Local_HoledModelapter->GetDependsMat(0);
					if (DoorID > 0)
					{
						Local_HoledModelapter = Cast<UDRModelInstanceAdapter>(Local_DRProjData->GetAdapter(DoorID));
						if (Local_HoledModelapter)
						{
							Local_Door.ModelType = Local_HoledModelapter->GetType();
							Local_Door.ModelIds[0] = Local_HoledModelapter->GetModelID();
							Local_Door.RoomClassIds[0] = Local_HoledModelapter->GetRoomClassIdByNormalModel();
							Local_Door.CraftIDs[0] = Local_HoledModelapter->GetCraftIdByNormalModel();
							Local_Door.DependsMats[0] = Local_HoledModelapter->GetDependsMat(0);
						}
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool UDROperationHouseFunctionLibrary::GetWindowStruct(int32 WindowID, FDRDoorAndWindow& Local_Window)
{
	if (GameInstance)
	{
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
		if (Local_DRProjData)
		{
			UDRModelInstanceAdapter* Local_HoledModelapter = Cast<UDRModelInstanceAdapter>(Local_DRProjData->GetAdapter(WindowID));
			if (Local_HoledModelapter)
			{
				Local_Window.ModelType = Local_HoledModelapter->GetType();
				Local_Window.Loction = Local_HoledModelapter->GetLocation();
				Local_Window.ModelIds[0] = Local_HoledModelapter->GetModelID();
				Local_Window.RoomClassIds[0] = Local_HoledModelapter->GetRoomClassIdByNormalModel();
				Local_Window.CraftIDs[0] = Local_HoledModelapter->GetCraftIdByNormalModel();
				Local_Window.TagName = Local_HoledModelapter->GetModelTag();
				Local_Window.DependsMats[0] = Local_HoledModelapter->GetDependsMat(0);
				return true;
			}
		}
	}
	return false;
}

void UDROperationHouseFunctionLibrary::DemolitionInitHouseData()
{
	if (GameInstance)
	{
		CopyDRHouseData(GameInstance->TInitDRSolidWallData, GameInstance->TInitDRAreaData, GameInstance->TInitDRHoleData, GameInstance->TInitDRRDoorAndWindowData);
	}
}

void UDROperationHouseFunctionLibrary::DemolitionSaveHouseData()
{
	if (GameInstance)
	{
		CopyDRHouseData(GameInstance->TSaveDRSolidWallData, GameInstance->TSaveDRAreaData, GameInstance->TSaveDRHoleData, GameInstance->TSaveDRRDoorAndWindowData);
	}
}

void UDROperationHouseFunctionLibrary::ResetHouseData()
{
	if (GameInstance)
	{
		GameInstance->TSaveDRSolidWallData = GameInstance->TInitDRSolidWallData;
		GameInstance->TSaveDRAreaData = GameInstance->TInitDRAreaData;
		GameInstance->TSaveDRHoleData = GameInstance->TInitDRHoleData;
		GameInstance->TSaveDRRDoorAndWindowData = GameInstance->TInitDRRDoorAndWindowData;
	}
}

void UDROperationHouseFunctionLibrary::InitHouseData()
{
	if (GameInstance)
	{
		if (GameInstance->TInitDRSolidWallData.Num() == 0)
		{
			GameInstance->TInitDRSolidWallData = GameInstance->TSaveDRSolidWallData;
			GameInstance->TInitDRAreaData = GameInstance->TSaveDRAreaData;
			GameInstance->TInitDRHoleData = GameInstance->TSaveDRHoleData;
			GameInstance->TInitDRRDoorAndWindowData = GameInstance->TSaveDRRDoorAndWindowData;
		}
		GameInstance->TTempDRAreaData = GameInstance->TSaveDRAreaData;
	}
}

void UDROperationHouseFunctionLibrary::CopyDRHouseData(TArray<FDRSolidWall>& SolidWallData, TArray<FPreAreaData>& AreData, TArray<FDRHole>& HoleData, TArray<FDRDoorAndWindow>& HoleModelData)
{
	CopyDRSolidWallData(SolidWallData);
	CopyDRAreaData(AreData);
	CopyDRHoleData(HoleData);
	CopyDRHoleModelData(HoleModelData);
}

void UDROperationHouseFunctionLibrary::SetWallSaveDataByCopyData(int32 WallID, const FDRSolidWall& WallData)
{
	if (GameInstance)
	{
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
		if (Local_DRProjData)
		{
			UDRSolidWallAdapter* Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(WallID));
			if (Local_SolidWallAdapter)
			{
				Local_SolidWallAdapter->SetHeight(WallData.Height);
				Local_SolidWallAdapter->SetRoomWall(WallData.RoomWall);
				Local_SolidWallAdapter->SetSolidWallType(WallData.SolidWallType);
				Local_SolidWallAdapter->SetMainWall(WallData.MainWall);
				Local_SolidWallAdapter->SetWallTagName(WallData.TagName);
				Local_SolidWallAdapter->SetLeftRuler(WallData.LeftRuler);
				Local_SolidWallAdapter->SetRightRuler(WallData.RightRuler);
				Local_SolidWallAdapter->SetFrontUVAngle(WallData.SolidWallUV[0].UVAngle);
				Local_SolidWallAdapter->SetFrontUVPos(WallData.SolidWallUV[0].UVOffset);
				Local_SolidWallAdapter->SetFrontUVScale(WallData.SolidWallUV[0].UVScale);
				Local_SolidWallAdapter->SetBackUVAngle(WallData.SolidWallUV[1].UVAngle);
				Local_SolidWallAdapter->SetBackUVPos(WallData.SolidWallUV[1].UVOffset);
				Local_SolidWallAdapter->SetBackUVScale(WallData.SolidWallUV[1].UVScale);
				Local_SolidWallAdapter->SetSideUVAngle(WallData.SolidWallUV[2].UVAngle);
				Local_SolidWallAdapter->SetSideUVPos(WallData.SolidWallUV[2].UVOffset);
				Local_SolidWallAdapter->SetSideUVScale(WallData.SolidWallUV[2].UVScale);
				Local_SolidWallAdapter->SetWallMaterials(WallData.SolidWallMaterials);
				Local_SolidWallAdapter->SetNewWallType(WallData.NewWallType);
			}
		}
	}
}

void UDROperationHouseFunctionLibrary::SetHoleSaveDataByCopyData(int32 HoleID, const FDRHole& HoleData)
{
	if (GameInstance)
	{
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
		if (Local_DRProjData)
		{
			UDRHoleAdapter* Local_HoleAdapter = Cast<UDRHoleAdapter>(Local_DRProjData->GetAdapter(HoleID));
			if (Local_HoleAdapter)
			{
				Local_HoleAdapter->SetHorizontalFlip(HoleData.HorizontalFlip);
				Local_HoleAdapter->SetVerticalFlip(HoleData.VerticalFlip);
				Local_HoleAdapter->SetSelfTagName(HoleData.SelfTagName);
				Local_HoleAdapter->SetSlotActorTag(HoleData.SlotActorTag);
				Local_HoleAdapter->SetHoleType(HoleData.HoleType);
				Local_HoleAdapter->SetHoleMaterials(HoleData.HoleMaterials);
				Local_HoleAdapter->SetHoleUVs(HoleData.HoleUVs);
				Local_HoleAdapter->SetHoleStatusType(HoleData.HolestatusType);
			}
		}
	}
}

void UDROperationHouseFunctionLibrary::SetWindowSaveDataByCopyData(int32 WindowID, const FDRDoorAndWindow& HoleModelData)
{
	if (GameInstance)
	{
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
		if (Local_DRProjData)
		{
			UDRModelInstanceAdapter*Local_ModelAdapter= Cast<UDRModelInstanceAdapter>(Local_DRProjData->GetAdapter(WindowID));
			if (Local_ModelAdapter)
			{
				Local_ModelAdapter->SetType(HoleModelData.ModelType);
				UDRWindowAdapter* Local_WindowAdapter = Cast<UDRWindowAdapter>(Local_DRProjData->GetAdapter(WindowID));
				if (Local_WindowAdapter)
				{
					if (HoleModelData.ModelIds.Num() > 0)
					{
						Local_WindowAdapter->SetModelID(HoleModelData.ModelIds[0]);
					}
					if (HoleModelData.CraftIDs.Num() > 0)
					{
						Local_WindowAdapter->SetCraftIdByNormalModel(HoleModelData.CraftIDs[0]);
					}
					if (HoleModelData.RoomClassIds.Num() > 0)
					{
						Local_WindowAdapter->SetRoomClassIdByNormalModel(HoleModelData.RoomClassIds[0]);
					}
					Local_WindowAdapter->SetModelTag(HoleModelData.TagName);
					if (HoleModelData.DependsMats.Num() > 0)
					{
						Local_WindowAdapter->SetDependsMat(HoleModelData.DependsMats[0], 0);
					}
					Local_WindowAdapter->SetType(HoleModelData.ModelType);
				}
			}

		}
	}
}

void UDROperationHouseFunctionLibrary::SetDoorSaveDataByCopyData(int32 DoorFrameID, int32 DoorID, const FDRDoorAndWindow& HoleModelData)
{
	if (GameInstance)
	{
		UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
		if (Local_DRProjData)
		{
			UDRDoorAdapter* Local_DoorAdapter;
			UDRModelInstanceAdapter*Local_ModelAdapter = Cast<UDRModelInstanceAdapter>(Local_DRProjData->GetAdapter(DoorID));
			if (Local_ModelAdapter)
			{
				Local_ModelAdapter->SetType(HoleModelData.ModelType);
				Local_DoorAdapter = Cast<UDRDoorAdapter>(Local_DRProjData->GetAdapter(DoorID));
				if (Local_DoorAdapter)
				{
					if (HoleModelData.ModelIds.Num() > 0)
					{
						Local_DoorAdapter->SetModelID(HoleModelData.ModelIds[0]);
					}
					if (HoleModelData.RoomClassIds.Num() > 0)
					{
						Local_DoorAdapter->SetRoomClassIdByNormalModel(HoleModelData.RoomClassIds[0]);
					}
					if (HoleModelData.CraftIDs.Num() > 0)
					{
						Local_DoorAdapter->SetCraftIdByNormalModel(HoleModelData.CraftIDs[0]);
					}
					Local_DoorAdapter->SetModelTag(HoleModelData.TagName);
					if (HoleModelData.DependsMats.Num() > 0)
					{
						Local_DoorAdapter->SetDependsMat(HoleModelData.DependsMats[0], 0);
					}
				}
			}
			Local_ModelAdapter = Cast<UDRModelInstanceAdapter>(Local_DRProjData->GetAdapter(DoorFrameID));
			if (Local_ModelAdapter)
			{
				Local_ModelAdapter->SetType(EModelType::EDRDoorFram);
				Local_DoorAdapter = Cast<UDRDoorAdapter>(Local_DRProjData->GetAdapter(DoorFrameID));
				if (Local_DoorAdapter)
				{
					if (HoleModelData.ModelIds.Num() > 1)
					{
						Local_DoorAdapter->SetModelID(HoleModelData.ModelIds[1]);
					}
					if (HoleModelData.RoomClassIds.Num() > 1)
					{
						Local_DoorAdapter->SetRoomClassIdByNormalModel(HoleModelData.RoomClassIds[1]);
					}
					if (HoleModelData.CraftIDs.Num() > 1)
					{
						Local_DoorAdapter->SetCraftIdByNormalModel(HoleModelData.CraftIDs[1]);
					}
					Local_DoorAdapter->SetModelTag(HoleModelData.TagName);
					if (HoleModelData.DependsMats.Num() > 1)
					{
						Local_DoorAdapter->SetDependsMat(HoleModelData.DependsMats[1], 0);
					}
				}
			}
		}
	}
}

void UDROperationHouseFunctionLibrary::LoadWallByCopyData(const TArray<FDRSolidWall>& SolidWallData)
{
	if (GameInstance)
	{
		UBuildingSystem*BuildingSystem = GameInstance->GetBuildingSystemInstance();
		if (BuildingSystem)
		{
			BuildingSystem->SetConfigTolerance(2);
			for (int i = 0; i < SolidWallData.Num(); ++i)
			{
				int LocStartCornerID = BuildingSystem->AddCorner(FVector2D(SolidWallData[i].StartPos));
				int LocEndCornerID = BuildingSystem->AddCorner(FVector2D(SolidWallData[i].EndPos));
				TArray<int32> LocalWallIDs;
				LocalWallIDs = BuildingSystem->AddWall(LocStartCornerID, LocEndCornerID, SolidWallData[i].LeftThick, SolidWallData[i].RightThick, SolidWallData[i].Height);
				if (LocalWallIDs.Num() > 0)
				{
					/*if (GameInstance->WallBuildSystem)
					{
						GameInstance->WallBuildSystem->SetWallIsClosed(LocalWallIDs, SolidWallData[i].RoomWall);
					}*/
					int32 LocalObjectID = BuildingSystem->GetWallObjectIDByTwoCorner(LocStartCornerID, LocEndCornerID);
					SetWallSaveDataByCopyData(LocalObjectID, SolidWallData[i]);
				}
			}
		}
	}
}

int32 UDROperationHouseFunctionLibrary::SpawnHoleObject(const EModelType& Type, const FVector2D& Location, const float Width, const float Height, const float ZPos, const FString &WindowResID, const FString &SillsResID)
{
	int32 HoleID = INDEX_NONE;
	if (GameInstance)
	{
		UBuildingSystem*BuildingSystem = GameInstance->GetBuildingSystemInstance();
		if (BuildingSystem)
		{
			FVector2D BestLoc;
			int32 WallID = BuildingSystem->FindCloseWall(Location, Width, BestLoc, 10);
			if (Type > EModelType::EDRDoorWay)
			{
				HoleID = BuildingSystem->AddWindow(WallID, BestLoc, ZPos, Width, Height, TCHAR_TO_ANSI(*WindowResID), TCHAR_TO_ANSI(*SillsResID));
			}
			else
				HoleID = BuildingSystem->AddDoor(WallID, BestLoc, Width, Height, ZPos);
		}
	}
	return HoleID;
}

void UDROperationHouseFunctionLibrary::LoadBooleanByCopyData(const TArray<FDRHole>& HoleData)
{
	if (GameInstance)
	{
		UBuildingSystem*BuildingSystem = GameInstance->GetBuildingSystemInstance();
		if (BuildingSystem)
		{
			for (int i = 0; i < HoleData.Num(); ++i)
			{
				int32 HoleID;
				HoleID = SpawnHoleObject(HoleData[i].HoleType, FVector2D(HoleData[i].Transform.GetLocation()), HoleData[i].Width, HoleData[i].Height, HoleData[i].ZPos);
				SetHoleSaveDataByCopyData(HoleID, HoleData[i]);
			}
		}
	}
}

bool UDROperationHouseFunctionLibrary::CreateDoorHoleReleation(const TArray<int32>&DoorHoleIDs, const FString&ModelTag, const FString&DoorResID, const FString&FrameResID, int32&DoorObjID, int32&FrameObjID)
{
	if (GameInstance)
	{
		UBuildingSystem*BuildingSystem = GameInstance->GetBuildingSystemInstance();
		if (BuildingSystem)
		{
			for (int i = 0; i < DoorHoleIDs.Num(); ++i)
			{
				UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
				if (Local_DRProjData)
				{
					UDRDoorHoleAdapter*Local_DoorHoleAdapter = Cast<UDRDoorHoleAdapter>(Local_DRProjData->GetAdapter(DoorHoleIDs[i]));
					if (Local_DoorHoleAdapter)
					{
						if (Local_DoorHoleAdapter->GetSlotActorTag() == ModelTag)
						{
							if (!FrameResID.IsEmpty())
							{
								Local_DoorHoleAdapter->SetDoorFrameResId(FrameResID);
							}
							if (!DoorResID.IsEmpty())
							{
								Local_DoorHoleAdapter->SetDoorResId(DoorResID);
							}
							DoorObjID = Local_DoorHoleAdapter->GetDoorObjId();
							FrameObjID = Local_DoorHoleAdapter->GetDoorFrameObjId();
							return true;
						}
					}
				}
			}
		}
	}
	DoorObjID = INDEX_NONE;
	FrameObjID = INDEX_NONE;
	return false;
}
bool UDROperationHouseFunctionLibrary::CreateWindowHoleReleation(const TArray<int32>&WindowHoleIDs, const FString&ModelTag, const FString&WindowResID, int32&WindowObjID)
{
	if (GameInstance)
	{
		UBuildingSystem*BuildingSystem = GameInstance->GetBuildingSystemInstance();
		if (BuildingSystem)
		{
			for (int i = 0; i < WindowHoleIDs.Num(); ++i)
			{
				UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
				if (Local_DRProjData)
				{
					UDRWindowHoleAdapter*Local_WindowHoleAdapter = Cast<UDRWindowHoleAdapter>(Local_DRProjData->GetAdapter(WindowHoleIDs[i]));
					if (Local_WindowHoleAdapter)
					{
						if (Local_WindowHoleAdapter->GetSlotActorTag() == ModelTag)
						{
							if (!WindowResID.IsEmpty())
							{
								Local_WindowHoleAdapter->SetWindowResId(WindowResID);
								WindowObjID = Local_WindowHoleAdapter->GetWindowObjId();
								return true;
							}
							else
							{
								WindowObjID = INDEX_NONE;
								return false;
							}
						}
					}
				}
			}
		}
	}
	WindowObjID = INDEX_NONE;
	return false;
}


void UDROperationHouseFunctionLibrary::LoadDoorAndWindowByCopyData(const TArray<FDRDoorAndWindow>& HoleModelData)
{
	if (GameInstance)
	{
		UBuildingSystem*BuildingSystem = GameInstance->GetBuildingSystemInstance();
		if (BuildingSystem)
		{
			TArray<int32>DoorHoleIDs;
			DoorHoleIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_DoorHole);
			TArray<int32>WindowHoleIDs;
			WindowHoleIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_Window);
			for (int i = 0; i < HoleModelData.Num(); ++i)
			{
				if (HoleModelData[i].ModelType > EModelType::EDRDoorWay)
				{
					if (HoleModelData[i].ModelIds.Num() > 0)
					{
						FString WindowResId;
						WindowResId = GameInstance->FindResIDByModelID(HoleModelData[i].ModelIds[0]);
						if (WindowResId.IsEmpty())
						{
							switch (HoleModelData[i].ModelType)
							{
							case EModelType::EDRWindow:
								WindowResId = TEXT("FD3FF7E34DF874E415944AB8C55E7368");
								break;
							case EModelType::EDRGroundWindow:
								WindowResId = TEXT("D8801CC14B61F21FB5190486DF78C8DF");
								break;
							case EModelType::EDRBayWindow:
								WindowResId = TEXT("8F2EE01D45E222358E942FB895F141BB");
								break;
							}
						}
						int32 WindowObjectID = INDEX_NONE;
						if (CreateWindowHoleReleation(WindowHoleIDs, HoleModelData[i].TagName, WindowResId, WindowObjectID))
						{
							SetWindowSaveDataByCopyData(WindowObjectID, HoleModelData[i]);
						}
					}
				}
				else
				{
					if (HoleModelData[i].ModelIds.Num() > 0)
					{
						FString DoorResID;
						DoorResID = GameInstance->FindResIDByModelID(HoleModelData[i].ModelIds[0]);
						FString FrameResID;
						if (HoleModelData[i].ModelIds.Num() > 1)
						{
							FrameResID = GameInstance->FindResIDByModelID(HoleModelData[i].ModelIds[1]);
						}
						int32 DoorObjectID = INDEX_NONE;
						int32 FrameObjectID = INDEX_NONE;
						if (CreateDoorHoleReleation(DoorHoleIDs, HoleModelData[i].TagName, DoorResID, FrameResID, DoorObjectID, FrameObjectID))
						{
							SetDoorSaveDataByCopyData(FrameObjectID, DoorObjectID, HoleModelData[i]);
						}
					}
				}
			}
		}
	}
}

void UDROperationHouseFunctionLibrary::LoadHouseFiles()
{
	if (GameInstance)
	{
		LoadWallByCopyData(GameInstance->TSaveDRSolidWallData);
		LoadBooleanByCopyData(GameInstance->TSaveDRHoleData);
		LoadDoorAndWindowByCopyData(GameInstance->TSaveDRRDoorAndWindowData);
		if (GameInstance->WallBuildSystem)
		{
			if (GameInstance->TSaveDRSolidWallData.IsValidIndex(0))
			{
				float LocalHeight = GameInstance->TSaveDRSolidWallData[0].Height;
				GameInstance->GetBuildingSystemInstance()->SetWallHightConfig(LocalHeight);
				GameInstance->WallBuildSystem->SetWallHeightConfigForDR(LocalHeight);
			}
			GameInstance->WallBuildSystem->LoadAreaByDemolition();
		}
	}
}

void UDROperationHouseFunctionLibrary::GetSubmitDataByChangeHouse(const TArray<FPreAreaData>& InOldData, const TArray<FPreAreaData>& InCurrentData, TArray<FDRChangeHouseSubmitData>& OutSumbitData)
{
	FDRChangeHouseSubmitData Local_SubmitData;
	TArray<FString> Local_OldKeys;
	TArray<FString> Local_CurrentKeys;
	FPreAreaData* Local_OldAreaData = nullptr;
	FPreAreaData* Local_CurrentAreaData = nullptr;
	TMap<FString, FPreAreaData> Local_OldHouseData;
	TMap<FString, FPreAreaData> Local_CurrentHouseData;
	for (const FPreAreaData& CurrentData : InOldData)
	{
		Local_OldKeys.Add(CurrentData.TagName);
		Local_OldHouseData.Add(CurrentData.TagName, CurrentData);
	}
	for (const FPreAreaData& CurrentData : InCurrentData)
	{
		Local_CurrentKeys.Add(CurrentData.TagName);
		Local_CurrentHouseData.Add(CurrentData.TagName, CurrentData);
	}
	for (FString OldKey : Local_OldKeys)
	{
		if (Local_CurrentKeys.Contains(OldKey))
		{
			Local_OldAreaData = Local_OldHouseData.Find(OldKey);
			Local_CurrentAreaData = Local_CurrentHouseData.Find(OldKey);
			if (Local_OldAreaData && Local_CurrentAreaData)
			{
				Local_OldAreaData->Area /= CONVERSIONUNIT;
				Local_CurrentAreaData->Area /= CONVERSIONUNIT;
				if (!FMath::IsNearlyEqual(Local_OldAreaData->Area, Local_CurrentAreaData->Area, 0.1f) || !Local_CurrentAreaData->RoomUsageName.Equals(Local_OldAreaData->RoomUsageName))
				{
					Local_SubmitData.Area = Local_CurrentAreaData->Area;
					Local_SubmitData.NewRoomId = Local_CurrentAreaData->RoomId;
					Local_SubmitData.OldRoomId = Local_OldAreaData->RoomId;
					Local_SubmitData.RoomType = 3;
					Local_SubmitData.RoomUsageId = Local_CurrentAreaData->RoomUsageId;
					Local_SubmitData.RoomUsageName = Local_CurrentAreaData->RoomUsageName;
					OutSumbitData.Add(Local_SubmitData);
				}
				else
				{
					Local_SubmitData.Area = Local_CurrentAreaData->Area;
					Local_SubmitData.NewRoomId = Local_CurrentAreaData->RoomId;
					Local_SubmitData.OldRoomId = Local_OldAreaData->RoomId;
					Local_SubmitData.RoomType = 0;
					Local_SubmitData.RoomUsageId = Local_CurrentAreaData->RoomUsageId;
					Local_SubmitData.RoomUsageName = Local_CurrentAreaData->RoomUsageName;
					OutSumbitData.Add(Local_SubmitData);
				}
			}
		}
		else
		{
			Local_OldAreaData = Local_OldHouseData.Find(OldKey);
			if (Local_OldAreaData)
			{
				Local_SubmitData.Area = Local_OldAreaData->Area/ CONVERSIONUNIT;
				Local_SubmitData.NewRoomId = 0;
				Local_SubmitData.OldRoomId = Local_OldAreaData->RoomId;
				Local_SubmitData.RoomType = 1;
				Local_SubmitData.RoomUsageId = Local_OldAreaData->RoomUsageId;
				Local_SubmitData.RoomUsageName = Local_OldAreaData->RoomUsageName;
				OutSumbitData.Add(Local_SubmitData);
			}
		}
	}
	for (FString CurrentKey : Local_CurrentKeys)
	{
		if (!Local_OldKeys.Contains(CurrentKey))
		{
			Local_CurrentAreaData = Local_CurrentHouseData.Find(CurrentKey);
			if (Local_CurrentAreaData)
			{
				Local_SubmitData.Area = Local_CurrentAreaData->Area / CONVERSIONUNIT;
				Local_SubmitData.NewRoomId = Local_CurrentAreaData->RoomId;
				Local_SubmitData.OldRoomId = 0;
				Local_SubmitData.RoomType = 2;
				Local_SubmitData.RoomUsageId = Local_CurrentAreaData->RoomUsageId;
				Local_SubmitData.RoomUsageName = Local_CurrentAreaData->RoomUsageName;
				OutSumbitData.Add(Local_SubmitData);
			}
		}
	}
}

void UDROperationHouseFunctionLibrary::GetTagNameByChangeHouse(const TArray<FPreAreaData>& InOldData, const TArray<FPreAreaData>& InCurrentData, TArray<FString>& OutAreaTagName, TArray<FDRBasicArray>& OutAreaPoints)
{
	FDRBasicArray Local_AreaPoints;
	TArray<FString> Local_OldKeys;
	TArray<FString> Local_CurrentKeys;
	FPreAreaData* Local_OldAreaData;
	FPreAreaData* Local_CurrentAreaData;
	TMap<FString, FPreAreaData> Local_OldHouseData;
	TMap<FString, FPreAreaData> Local_CurrentHouseData;
	for (const FPreAreaData& CurrentData : InOldData)
	{
		Local_OldKeys.Add(CurrentData.TagName);
		Local_OldHouseData.Add(CurrentData.TagName, CurrentData);
	}
	for (const FPreAreaData& CurrentData : InCurrentData)
	{
		Local_CurrentKeys.Add(CurrentData.TagName);
		Local_CurrentHouseData.Add(CurrentData.TagName, CurrentData);
	}
	for (FString OldKey : Local_OldKeys)
	{
		if (Local_CurrentKeys.Contains(OldKey))
		{
			Local_OldAreaData = Local_OldHouseData.Find(OldKey);
			Local_CurrentAreaData = Local_CurrentHouseData.Find(OldKey);
			if (Local_OldAreaData && Local_CurrentAreaData)
			{
				float a = Local_OldAreaData->Area / CONVERSIONUNIT;
				float b = Local_CurrentAreaData->Area / CONVERSIONUNIT;
				if (!FMath::IsNearlyEqual(a, b, 0.1f))
				{
					OutAreaTagName.Add(OldKey);
					Local_AreaPoints.Vector2DArray = Local_OldAreaData->VectorList;
					OutAreaPoints.Add(Local_AreaPoints);
				}
			}
		}
		else
		{
			Local_OldAreaData = Local_OldHouseData.Find(OldKey);
			if (Local_OldAreaData)
			{
				OutAreaTagName.Add(OldKey);
				Local_AreaPoints.Vector2DArray = Local_OldAreaData->VectorList;
				OutAreaPoints.Add(Local_AreaPoints);
			}
		}
	}
}

void UDROperationHouseFunctionLibrary::CopyDRHoleModelDataByHoleID(const TArray<int32>& HoleIDs, TArray<FDRDoorAndWindow>& HoleModelData)
{
	HoleModelData.Empty();
	if (GameInstance&&GameInstance->GetBuildingSystemInstance())
	{
		for (int i = 0; i < HoleIDs.Num(); ++i)
		{
			UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
			if (Local_DRProjData)
			{
				UDRDoorHoleAdapter* Local_Holedapter = Cast<UDRDoorHoleAdapter>(Local_DRProjData->GetAdapter(HoleIDs[i]));
				if (Local_Holedapter)
				{
					int HoleModelID = Local_Holedapter->GetDoorFrameObjId();
					int DoorID = Local_Holedapter->GetDoorObjId();
					if (HoleModelID > 0)
					{
						FDRDoorAndWindow Local_HoleModel;
						if (GetDoorStruct(HoleModelID, DoorID, Local_HoleModel))
						{
							HoleModelData.Add(Local_HoleModel);
						}
					}
				}
				else
				{
					UDRWindowHoleAdapter* Local_WindowHoledapter = Cast<UDRWindowHoleAdapter>(Local_DRProjData->GetAdapter(HoleIDs[i]));
					if (Local_WindowHoledapter)
					{
						FDRDoorAndWindow Local_Window;
						if (GetWindowStruct(Local_WindowHoledapter->GetWindowObjId(), Local_Window))
						{
							HoleModelData.Add(Local_Window);
						}
					}
				}
			}
		}
	}
}

int32 UDROperationHouseFunctionLibrary::GetConfigModelIDByType(const EDConfigType Type)
{
	if (GameInstance)
	{
		return GameInstance->HouseConfigModelIDs.GetConfigModelIDByType(Type);
	}
	else
		return 0;
}

void UDROperationHouseFunctionLibrary::MergeShortWallData(TArray<FDRSolidWall>& SolidWallData)
{
	if (GameInstance&&GameInstance->GetBuildingSystemInstance())
	{
		TArray<FDRSolidWall> Local_SolidWallData;
		TArray<int32> Local_ShortWallIndex;
		TMap<int32, FDRSolidWall> Local_AllWallData;
		TMap<int32, FVector2D> Local_WallDir;
		TArray<int32>WallIDs;
		WallIDs = GameInstance->GetBuildingSystemInstance()->GetAllTypedObjects(EDR_ObjectType::EDR_SolidWall);
		for (int i = 0; i < WallIDs.Num(); ++i)
		{
			FDRSolidWall Local_SolidWall;
			if (GetWallStruct(WallIDs[i], Local_SolidWall))
			{
				float WallLength = FVector::Distance(Local_SolidWall.StartPos, Local_SolidWall.EndPos);
				Local_AllWallData.Add(WallIDs[i], Local_SolidWall);
				Local_WallDir.Add(WallIDs[i], FVector2D(Local_SolidWall.StartPos - Local_SolidWall.EndPos));
				if (WallLength <= 20.0f)
				{
					Local_ShortWallIndex.Add(WallIDs[i]);
				}
			}
		}
		for (int i = 0; i < Local_ShortWallIndex.Num(); ++i)
		{
			bool MergeSuccess = false;
			int32 Local_ShortWallID = Local_ShortWallIndex[i];
			UDRProjData* Local_DRProjData = UDRProjData::GetProjectDataManager(GameInstance);
			if (!Local_DRProjData)
			{
				return;
			}
			UDRSolidWallAdapter* Local_SolidWallAdapter = Cast<UDRSolidWallAdapter>(Local_DRProjData->GetAdapter(Local_ShortWallID));
			FVector2D Local_ShortWallDir = Local_WallDir[Local_ShortWallID];
			FDRSolidWall Local_ShortWallData = Local_AllWallData[Local_ShortWallIndex[i]];
			TArray<int32> Local_P0Walls = Local_SolidWallAdapter->GetAllWallByP0();
			if (Local_P0Walls.Num() == 2)
				for (int j = 0; j < Local_P0Walls.Num(); ++j)
				{
					if (Local_P0Walls[j] == Local_ShortWallIndex[i])
						continue;
					float CrossProductValue = FMath::Abs(FVector2D::CrossProduct(Local_WallDir[Local_P0Walls[j]].GetSafeNormal(), Local_ShortWallDir.GetSafeNormal()));
					if (CrossProductValue < 0.1f)
					{
						if (Local_AllWallData.Contains(Local_P0Walls[j]))
						{
							FDRSolidWall Local_ConnectWallData = Local_AllWallData[Local_P0Walls[j]];
							if (Local_ConnectWallData.StartPos == Local_ShortWallData.StartPos)
							{
								if (Local_WallDir[Local_P0Walls[j]].GetSafeNormal().Equals(Local_ShortWallDir.GetSafeNormal(), 0.001f) && FVector2D::Distance(Local_WallDir[Local_P0Walls[j]], FVector2D(0, 0)) > FVector2D::Distance(Local_ShortWallDir, FVector2D(0, 0)))
								{
									Local_AllWallData.Remove(Local_ShortWallIndex[i]);
									MergeSuccess = true;
									break;
								}
								if (FMath::IsNearlyEqual(Local_ConnectWallData.LeftThick, Local_ShortWallData.RightThick, 0.001f) && FMath::IsNearlyEqual(Local_ConnectWallData.RightThick, Local_ShortWallData.LeftThick, 0.001f))
								{
									Local_AllWallData[Local_P0Walls[j]].StartPos = Local_ShortWallData.EndPos;
									Local_AllWallData[Local_P0Walls[j]].EndPos = Local_ConnectWallData.EndPos;
									Local_AllWallData.Remove(Local_ShortWallIndex[i]);
									MergeSuccess = true;
									break;
								}
							}
							else
							{
								if (FMath::IsNearlyEqual(Local_ConnectWallData.LeftThick, Local_ShortWallData.LeftThick, 0.001f) && FMath::IsNearlyEqual(Local_ConnectWallData.RightThick, Local_ShortWallData.RightThick, 0.001f))
								{
									Local_AllWallData[Local_P0Walls[j]].StartPos = Local_ConnectWallData.StartPos;
									Local_AllWallData[Local_P0Walls[j]].EndPos = Local_ShortWallData.EndPos;
									Local_AllWallData.Remove(Local_ShortWallIndex[i]);
									MergeSuccess = true;
									break;
								}
							}
						}
					}
				}
			if (MergeSuccess)
				continue;
			TArray<int32> Local_P1Walls = Local_SolidWallAdapter->GetAllWallByP1();
			if (Local_P1Walls.Num() == 2)
				for (int j = 0; j < Local_P1Walls.Num(); ++j)
				{
					if (Local_P1Walls[j] == Local_ShortWallIndex[i])
						continue;
					float CrossProductValue = FMath::Abs(FVector2D::CrossProduct(Local_WallDir[Local_P1Walls[j]].GetSafeNormal(), Local_ShortWallDir.GetSafeNormal()));
					if (CrossProductValue < 0.1f)
					{
						if (Local_AllWallData.Contains(Local_P1Walls[j]))
						{
							FDRSolidWall Local_ConnectWallData = Local_AllWallData[Local_P1Walls[j]];
							if (Local_ConnectWallData.EndPos == Local_ShortWallData.EndPos)
							{
								if (Local_WallDir[Local_P1Walls[j]].GetSafeNormal().Equals(Local_ShortWallDir.GetSafeNormal(), 0.001f) && FVector2D::Distance(Local_WallDir[Local_P1Walls[j]], FVector2D(0, 0)) > FVector2D::Distance(Local_ShortWallDir, FVector2D(0, 0)))
								{
									Local_AllWallData.Remove(Local_ShortWallIndex[i]);
									MergeSuccess = true;
									break;
								}
								if (FMath::IsNearlyEqual(Local_ConnectWallData.LeftThick, Local_ShortWallData.RightThick, 0.001f) && FMath::IsNearlyEqual(Local_ConnectWallData.RightThick, Local_ShortWallData.LeftThick, 0.001f))
								{
									Local_AllWallData[Local_P1Walls[j]].StartPos = Local_ConnectWallData.StartPos;
									Local_AllWallData[Local_P1Walls[j]].EndPos = Local_ShortWallData.StartPos;
									Local_AllWallData.Remove(Local_ShortWallIndex[i]);
									MergeSuccess = true;
									break;
								}
							}
							else
							{
								if (FMath::IsNearlyEqual(Local_ConnectWallData.LeftThick, Local_ShortWallData.LeftThick, 0.001f) && FMath::IsNearlyEqual(Local_ConnectWallData.RightThick, Local_ShortWallData.RightThick, 0.001f))
								{
									Local_AllWallData[Local_P1Walls[j]].StartPos = Local_ShortWallData.StartPos;
									Local_AllWallData[Local_P1Walls[j]].EndPos = Local_ConnectWallData.EndPos;
									Local_AllWallData.Remove(Local_ShortWallIndex[i]);
									MergeSuccess = true;
									break;
								}
							}
						}
					}
				}
		}
		TArray<int32> Local_Keys;
		Local_AllWallData.GetKeys(Local_Keys);
		for (int i = 0; i < Local_Keys.Num(); ++i)
		{
			SolidWallData.Add(Local_AllWallData[Local_Keys[i]]);
		}
	}
}

void UDROperationHouseFunctionLibrary::GetMinMaxBox(const TArray<FVector2D>InPos, FVector2D& MinPos, FVector2D&MaxPos)
{
	MinPos = FVector2D::ZeroVector;
	MaxPos = FVector2D::ZeroVector;
	if (InPos.Num() > 0)
	{
		MaxPos=MinPos = InPos[0];
		for (int i = 0; i < InPos.Num(); ++i)
		{
			if (InPos[i].X < MinPos.X)
			{
				MinPos.X = InPos[i].X;
			}
			if (InPos[i].Y < MinPos.Y)
			{
				MinPos.Y = InPos[i].Y;
			}
			if (InPos[i].X > MaxPos.X)
			{
				MaxPos.X = InPos[i].X;
			}
			if (InPos[i].Y > MaxPos.Y)
			{
				MaxPos.Y = InPos[i].Y;
			}
		}
	}
}