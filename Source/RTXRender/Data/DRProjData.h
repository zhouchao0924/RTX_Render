#pragma once

#include "DRData.h"
#include "ISuite.h"
#include "DRStruct/DRBuildingStruct.h"
#include "Adapter/DRDoorAdapter.h"
#include "Adapter/DRWindowAdapter.h"
#include "DRProjData.generated.h"

class UDRCurtainData;
class UDRMaterialAdapter;

UCLASS(BlueprintType)
class UDRProjData :public UDRData
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = ProjectDataManager)
	static UDRProjData* GetProjectDataManager(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = ProjectDataManager)
	static void ClearProjectDataManager();

public:
	UFUNCTION(BlueprintCallable)
	UDataAdapter *GetAdapter(int32 ObjID);
	
	UFUNCTION(BlueprintCallable)
	void AddProjData(UDRData *Data);

	UFUNCTION(BlueprintCallable)
	void RemoveProjData(UDRData* Data);

	UFUNCTION(BlueprintCallable)
	void AddObjectContext(int32 ObjectId, UObject* UEObject, EDR_ObjectType ObjectType);

	UFUNCTION(BlueprintCallable)
	void AddDoorAndWindowContext(int32 ObjectId, UObject* UEObject, bool bIsDoor);

	UFUNCTION(BlueprintCallable)
	void RemoveObjectContext(int32 ObjectId, EDR_ObjectType ObjectType);

	UFUNCTION(BlueprintCallable)
	void RemoveDoorAndWindowContext(int32 ObjectId);

	void SetSuite(ISuite *InSuite) { Suite = InSuite; }
	ISuite* GetSuite() { return Suite; }
	int32 GetChunkID() override { return CHUNK_DR_PROJ; }
	void  SerializeDataByVersion(ISerialize &Ar, int32 Ver) override;

	UFUNCTION(BlueprintCallable)
	UDRMaterialAdapter *GetMaterialAdapter(int32 ObjID, int32 SectionIndex);
	UDRMaterialAdapter *GetMaterial(IObject *Obj, int32 SectionIndex);

public:
	UPROPERTY(Transient, BlueprintReadWrite)
	TMap<int32, FString>  Model2ResIDMapping;

	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, UObject*> AreaMap;

	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, UObject*> SolidWallMap;

	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, UObject*> DoorHoleMap;

	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, UObject*> WindowHoleMap;

	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, UObject*> NormalModelMap;

	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, UObject*> DoorMap;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, UObject*> WindowMap;

	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, UObject*> PointLightMap;

	UPROPERTY(Transient, BlueprintReadOnly)
	TMap<int32, UObject*> SpotLightMap;

	UPROPERTY(Transient)
	TArray<UDRCurtainData *>	CurtainData;

protected:

	UPROPERTY(Transient)
	TMap<int32, UDataAdapter*> Adapters;

	UPROPERTY(Transient)
	UDRDoorAdapter *DoorAdapter;

	UPROPERTY(Transient)
	UDRWindowAdapter *WindowAdapter;

	UPROPERTY(Transient)
	UDRMaterialAdapter	 *MaterialAdapter;

	ISuite	*Suite;

private:
	static UDRProjData* _projectDataManager;
};



