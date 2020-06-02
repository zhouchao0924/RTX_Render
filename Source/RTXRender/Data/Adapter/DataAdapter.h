#pragma once

#include "DRData.h"
#include "DataType.h"
#include "DRMaterialAdapter.h"
#include "DataAdapter.generated.h"

UCLASS(BlueprintType)
class UDataAdapter : public UDRData
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure)
	int32 GetObjID() { return Obj.IsValid() ? Obj->GetID() : INVALID_OBJID; }

	UFUNCTION(BlueprintPure)
	virtual EDR_ObjectType GetObjectType() { return EDR_ObjectType::EDR_UnkownObject; }

	UFUNCTION(BlueprintCallable)
	void SetUVOffset(const FVector2D &UVOffset, int32 SectionIndex = -1, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintCallable)
	void SetUVScale(const FVector2D &UVScale, int32 SectionIndex = -1, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintCallable)
	void SetUVRot(float UVRot, int32 SectionIndex = -1, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintCallable)
	void SetCraftID(int32 CraftID, int32 SectionIndex = -1, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintCallable)
	void SetRoomClassID(int32 RoomClassID, int32 SectionIndex = -1, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintPure)
	FVector2D GetUVOffset(int32 SectionIndex = 0, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintPure)
	FVector2D GetUVScale(int32 SectionIndex = 0, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintPure)
	float GetUVRot(int32 SectionIndex = 0, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintPure)
	int32 GetCraftID(int32 SectionIndex = 0, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintPure)
	int32 GetRoomClassID(int32 SectionIndex = 0, int32 OverrideObjID = -1);

	UFUNCTION(BlueprintPure)
	virtual int32 GetNumSection();

	UFUNCTION(BlueprintPure)
	void GetValidMaterialSections(TArray<int32> &MaterialSections);
protected:
	virtual ISurfaceObject *GetSurface(int SectionIndex);
protected:
	UFUNCTION(BlueprintCallable)
	virtual UDRMaterialAdapter *GetMaterial(int32 ObjID, int32 SectionIndex);
public:
	WLink Obj;
};



