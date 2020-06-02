// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../EditorGameInstance.h"
#include "DROperationHouseFunctionLibrary.generated.h"
/**
 * 
 */
UCLASS()
class RTXRENDER_API UDROperationHouseFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void DemolitionInitHouseData();
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void DemolitionSaveHouseData();
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void ResetHouseData();
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void InitHouseData();
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void CopyDRHouseData(TArray<FDRSolidWall>& SolidWallData, TArray<FPreAreaData>& AreData, TArray<FDRHole>& HoleData, TArray<FDRDoorAndWindow>& HoleModelData);
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void CopyDRSolidWallData(TArray<FDRSolidWall>& SolidWallData);
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void CopyDRAreaData(TArray<FPreAreaData>& AreData);
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void CopyDRHoleByID(const TArray<int32>& HoleIDs,TArray<FDRHole>& HoleData);
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void CopyDRHoleData(TArray<FDRHole>& HoleData);
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void CopyDRHoleModelData(TArray<FDRDoorAndWindow>& HoleModelData);
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void CopyDRHoleModelDataByHoleID(const TArray<int32>& HoleIDs, TArray<FDRDoorAndWindow>& HoleModelData);
	UFUNCTION(BlueprintCallable, Category = "CopyHouseData")
		static void ClearCopyData();
public:
	UFUNCTION(BlueprintCallable, Category = "BuildHouseData")
		static void LoadHouseFiles();
	UFUNCTION(BlueprintCallable, Category = "BuildHouseData")
		static void LoadWallByCopyData(const TArray<FDRSolidWall>& SolidWallData);
	UFUNCTION(BlueprintCallable, Category = "BuildHouseData")
		static void LoadBooleanByCopyData(const TArray<FDRHole>& HoleData);
	UFUNCTION(BlueprintCallable, Category = "BuildHouseData")
		static int32 SpawnHoleObject(const EModelType& Type, const FVector2D& Location, const float Width, const float Height, const float ZPos, const FString &WindowResID = "", const FString &SillsResID = "");
	UFUNCTION(BlueprintCallable, Category = "BuildHouseData")
		static void LoadDoorAndWindowByCopyData(const TArray<FDRDoorAndWindow>& HoleModelData);
	UFUNCTION(BlueprintCallable, Category = "BuildHouseData")
		static bool CreateDoorHoleReleation(const TArray<int32>&DoorHoleIDs, const FString&ModelTag, const FString&DoorResID, const FString&FrameResID, int32&DoorObjID, int32&FrameObjID);
	UFUNCTION(BlueprintCallable, Category = "BuildHouseData")
		static bool CreateWindowHoleReleation(const TArray<int32>&WindowHoleIDs, const FString&ModelTag, const FString&WindowResID, int32&WindowObjID);
public:
	UFUNCTION(BlueprintCallable, Category = "ConvertAdapterToStruct")
		static bool GetWallStruct(int32 WallID, FDRSolidWall& Local_SolidWall);
	UFUNCTION(BlueprintCallable, Category = "ConvertAdapterToStruct")
		static bool GetAreaStruct(int32 AreaID, FPreAreaData& Local_AreaWall);
	UFUNCTION(BlueprintCallable,  Category = "ConvertAdapterToStruct")
		static bool GetHoleStruct(int32 HoleID, FDRHole& Local_Hole);
	UFUNCTION(BlueprintCallable,  Category = "ConvertAdapterToStruct")
		static bool GetDoorStruct(int32 DoorFrameID, int32 DoorID,FDRDoorAndWindow& Local_Door);
	UFUNCTION(BlueprintCallable,  Category = "ConvertAdapterToStruct")
		static bool GetWindowStruct(int32 WindowID, FDRDoorAndWindow& Local_Window);

public:
	UFUNCTION(BlueprintCallable, Category = "GetHouseData")
		static void SetWallSaveDataByCopyData(int32 WallID, const FDRSolidWall& WallData);
	UFUNCTION(BlueprintCallable, Category = "GetHouseData")
		static void SetHoleSaveDataByCopyData(int32 HoleID, const FDRHole& HoleData);
	UFUNCTION(BlueprintCallable, Category = "GetHouseData")
		static void SetWindowSaveDataByCopyData(int32 WindowID, const FDRDoorAndWindow& HoleModelData);
	UFUNCTION(BlueprintCallable, Category = "GetHouseData")
		static void SetDoorSaveDataByCopyData(int32 DoorFrameID, int32 DoorID, const FDRDoorAndWindow& HoleModelData);

public:
	UFUNCTION(BlueprintCallable, Category = "ChangeHouse")
		static void  GetSubmitDataByChangeHouse(const TArray<FPreAreaData>& InOldData, const TArray<FPreAreaData>& InCurrentData, TArray<FDRChangeHouseSubmitData>& OutSumbitData);
	UFUNCTION(BlueprintCallable, Category = "ChangeHouse")
		static void  GetTagNameByChangeHouse(const TArray<FPreAreaData>& InOldData, const TArray<FPreAreaData>& InCurrentData, TArray<FString>& OutAreaTagName, TArray<FDRBasicArray>& OutAreaPoints);


public:
	static void SetGameInstance(UCEditorGameInstance*Instance)
	{
		if (Instance)
			GameInstance = Instance;
	}
	UFUNCTION(BlueprintCallable,  Category = "GetHouseData")
		static void GetAllHoleIDs(TArray<int32>& HoleIDs);


public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static	int32 GetConfigModelIDByType(const EDConfigType Type);
	UFUNCTION(BlueprintCallable)
		static	void MergeShortWallData(TArray<FDRSolidWall>& SolidWallData);

public:
	UFUNCTION(BlueprintCallable)
	static void GetMinMaxBox(const TArray<FVector2D>InPos, FVector2D& MinPos, FVector2D&MaxPos);

private:
	static UCEditorGameInstance * GameInstance;
};
