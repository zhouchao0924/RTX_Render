#pragma once

#include "WLink.h"
#include "IMeshObject.h"
#include "DRMaterialAdapter.generated.h"

UCLASS(BlueprintType)
class UDRMaterialAdapter : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetMaterial(const FString &ResID);

	UFUNCTION(BlueprintCallable)
	void SetModelID(const int32 &ModelID);

	UFUNCTION(BlueprintPure)
	int32 GetModelID();

	UFUNCTION(BlueprintPure)
	FString GetMaterial();

	UFUNCTION(BlueprintCallable)
	void SetRoomClassID(int32 RoomClassID);

	UFUNCTION(BlueprintPure)
	int32 GetRoomClassID();

	UFUNCTION(BlueprintCallable)
	void SetCraftID(int32 CraftID);

	UFUNCTION(BlueprintPure)
	int32 GetCraftID();

	UFUNCTION(BlueprintCallable)
	void SetUVScale(const FVector2D &UVScale);

	UFUNCTION(BlueprintPure)
	FVector2D GetUVScale();

	UFUNCTION(BlueprintCallable)
	void SetUVOffset(const FVector2D &UVOffset);

	UFUNCTION(BlueprintPure)
	FVector2D GetUVOffset();

	UFUNCTION(BlueprintCallable)
	void SetUVAngle(float UVAngle);

	UFUNCTION(BlueprintPure)
	float GetUVAngle();
public:
	IObject *GetSurfObj();
	ISurfaceObject *GetSurface();
public:
	WLink Obj;
	int   SectionIndex;
};

