#pragma once

#include "HomeLayout/HouseArchitect/WallBuildSystem.h"
#include "HomeLayout/HouseArchitect/SkirtingSystem.h"
#include "HomeLayout/HouseArchitect/AreaSystem.h"
#include "HomeLayout/HouseArchitect/ModelSystem.h"
#include "Engine/GameInstance.h"
#include "Networking.h"
#include "Struct/Struct.h"
#include "Building/DRStruct.h"
#include "Building/DRModelFactory.h"
#include "Building/BuildingSystem.h"
#include "../Data/DRStruct/DRBuildingStruct.h"
#include "EditorGameInstance.generated.h"

class UResourceMgr;
class UProjectManager;

UENUM(BlueprintType)
enum class EDrawHouseType : uint8
{
	NormalDrawType,
	SmallHouseDrawType,
	OrdersDrawType,
	ChaigaiDrawType,
	NewHouseType,
	UserHouseType,
	NationalHouseType
};

UCLASS()
class RTXRENDER_API UCEditorGameInstance : public UGameInstance
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "PlatformMgr")
	ASkirtingSystem* GetSkirtingSystem();

	UFUNCTION(BlueprintImplementableEvent)
	FString FindResIDByModelID(int ModelID);

	UFUNCTION(BlueprintCallable, Category = "SubSystem", meta = (DisplayName = "GetBuildingSystem"))
	UBuildingSystem *GetBuildingSystemInstance();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
	UDRModelFactory * GetCreateFactory();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
	void GetAnchorAllObjID(const int32 & AnchorID, TArray<int> & Obj_IDs);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
	bool IsAnchorWindowType(UBuildingData * BuildData, int32 Anchor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
	bool IsAnchorDoorType(UBuildingData * BuildData, int32 Anchor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Suite")
	bool IsAnchorModelType(UBuildingData * BuildData, int32 Anchor);
	
	UFUNCTION(BlueprintCallable, Category = "Suite")
	ADRComponentModel * GetComponentActor(int32 HoleID);

	UFUNCTION(BlueprintCallable, Category = "Suite")
	int32 AddModelToObject(int32 BaseObjID, FDRModelInstance Data);

	UFUNCTION(BlueprintImplementableEvent, Category = "MySocket")
	void LoginDRSolution();
	UFUNCTION(BlueprintNativeEvent, Category = "SolutionID")
	int GetCurrentSolutionID() const;

	FString StringFromBinaryArray(TArray<uint8> BinaryArray);

public:

	void Init() override;
	void Shutdown() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	TSubclassOf<AActor>	LightClassType;
public:
	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "SubSystem"))
	UWallBuildSystem *WallBuildSystem;

	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "SubSystem"))
	ASkirtingSystem *SkirtingSystem;

	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "SubSystem"))
	AAreaSystem *AreaSystem;

	UPROPERTY(BlueprintReadWrite, NonTransactional, meta = (Category = "SubSystem"))
	AModelSystem *ModelSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDRVirtualWallStruct> VirtualWallData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FComponenetInfo> TInitHouseComponentData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FComponenetInfo> HouseComponentData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FComponenetInfo> TSaveHouseComponentData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDRSolidWall> TInitDRSolidWallData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDRSolidWall> TSaveDRSolidWallData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDeleteHoleInfo> DRDemolishHole;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDeleteHoleInfo> TSaveDRDemolishHole;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDeleteHoleInfo> TInitDRDemolishHole;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FPreAreaData> TInitDRAreaData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FPreAreaData> TTempDRAreaData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FPreAreaData> TSaveDRAreaData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDRHole> TInitDRHoleData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDRHole> TSaveDRHoleData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDRDoorAndWindow> TInitDRRDoorAndWindowData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDRDoorAndWindow> TSaveDRRDoorAndWindowData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		TArray<FDeleteWall> DRDeleteWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "House")
		bool bIsFirstOpenFile;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "ConfigHouse")
		FDHouseConfigModelID HouseConfigModelIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Authority)
	FString				Token;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FHouseInfoValue         HouseInfoData;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool				IsCanAutoSaveByDemolition;

	UPROPERTY(Transient)
	UDRModelFactory * TempDRMoldeFactory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDrawHouseType DrawHouse_Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CurrentBaseHouseId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CurrentChildHouseId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CurrentOriginalHouseId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CurrentParentHouseId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CurrentTempParentHouseId;

	/*rtx*/
	UPROPERTY()
	UVaRestJsonObject*  RTXDATA;
		
private:
	UPROPERTY(Transient)
	UBuildingSystem * BuildingSystem;
};


