// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "../DataNodes/SavedDataNode.h"
#include "../StandardData/CornerData.h"
#include "../StandardData/OpeningData.h"
#include "../StandardData/WallData.h"
#include "../StandardData/RoomData.h"
#include "../StandardData/TextLabelData.h"
#include "BasicLibrary/DataReusedPool.h"
#include "VaRestJsonObject.h"

#include "ProjectPaper.generated.h"


UCLASS(BlueprintType)
class AJDR_API UProjectPaper : public UObject
{
	GENERATED_BODY()
	
private:
friend class UProjectManager;
	UProjectPaper();

public:
	void InitGameInst(class UCEditorGameInstance *InGameInst);

	UFUNCTION(BlueprintCallable)
	void LoadFromFile();

	UFUNCTION(BlueprintCallable)
	void LoadFromHouseJson(UVaRestJsonObject *HouseJson);

	UFUNCTION(BlueprintCallable)
	void PaperDoOffset(const FVector2D &OldOrigin, const FVector2D &NewOrigin);

	UFUNCTION(BlueprintCallable)
	void InitHousePictureInfo();

	UFUNCTION(BlueprintPure)
	const TArray<FCornerData>& GetCorners();

	UFUNCTION(BlueprintPure)
	const TArray<FWallData>& GetWalls();

	UFUNCTION(BlueprintPure)
	const TArray<FOpeningData>& GetOpenings();

	UFUNCTION(BlueprintPure)
	static FString CreateNewGuid();

	UFUNCTION(BlueprintPure)
	FString GetOpeningNameByType(EOpeningType InType);

	UFUNCTION(BlueprintPure)
	FTextLabelData GetRulerData();

	UFUNCTION(BlueprintCallable)
	void ConvertWallData2SaveGame(UPARAM(ref)FSavedDataNode &OutWallData, UCEditorGameInstance *InGameInst, const FWallData &InData);

	/*UFUNCTION(BlueprintCallable)
	void ConvertOpeningData2SaveGame(UPARAM(ref)FSavedDataNode &OutWallData, const FWallData &InData);*/

protected:
	void LoadPaperFromHouseJson(const TSharedPtr<FJsonObject> &HouseJsonObject);

	void LoadCornersFromJson(const TSharedPtr<FJsonObject> &HouseJsonObject);

	void LoadWallsFromJson(const TSharedPtr<FJsonObject> &HouseJsonObject);

	void LoadOpeningsFromJson(const TSharedPtr<FJsonObject> &HouseJsonObject);

	void LoadTextLabelsFromJson(const TSharedPtr<FJsonObject> &HouseJsonObject);

	void MakeRelation();

	static double GetDefOpeningZPosByType(EOpeningType InType);

	static double GetDefOpeningHeightByType(EOpeningType InType);

public:
	TDataReusedPool<FCornerData> CornerNodes;
	TDataReusedPool<FWallData> WallNodes;
	TDataReusedPool<FOpeningData> OpeningNodes;
	TDataReusedPool<FRoomData> RoomNodes;
	FTextLabelData RulerData;
	FString VersionNo; // 1.0.0

	UPROPERTY()
	class UCEditorGameInstance *GameInst;
};
