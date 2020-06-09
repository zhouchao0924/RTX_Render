// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISuite.h"
#include "IClass.h"
#include "IBuildingSDK.h"
#include "StretchPathComponent.generated.h"

USTRUCT(BlueprintType)
struct FStretchShapeSlot
{
	GENERATED_BODY()
	FStretchShapeSlot();
	UPROPERTY(BlueprintReadOnly)
	bool	bVisible;
	UPROPERTY(BlueprintReadWrite)
	int32	TypeId;
	UPROPERTY(BlueprintReadWrite)
	FString MX;
	UPROPERTY(BlueprintReadOnly)
	int32	MeshObjId;
	UPROPERTY(BlueprintReadOnly)
	FVector2D Size;
	UPROPERTY(BlueprintReadOnly)
	UBuildingComponent *Component;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStretchPathUpdate);

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class UStretchPathComponent : public USceneComponent , public IObjectProxy
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Update(const TArray<FStretchShapeSlot> &Slots, const TArray<FVector> &Path);

	UFUNCTION(BlueprintCallable)
	void SetSize(int32 TypeID, const FVector2D &Size, int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	void SetStretchMeshVisible(int32 TypeID, bool IsVisible, int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	void SetShapeMX(int32 TypeID, const FString &ShapeMXID, int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	void SetSurface(int32 TypeID, const FString &SurfaceUri, int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	FVector2D GetSize(int32 TypeID, int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetStretchMeshVisible(int32 TypeID, int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	FString GetShapeMX(int32 TypeID, int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	int32 GetPathPointIndex(int32 StretchMeshID, int32 SubSectionIndex);

	UFUNCTION(BlueprintCallable)
	bool GetSubMesh(int32 PointIndex, int32 TypeID, int32 &OutStretchMeshID, int32 &SubSectionIndex);

	UFUNCTION(BlueprintCallable)
	bool GetPath(int32 TypeID, TArray<FVector> &TypePaths);

	UFUNCTION(BlueprintCallable)
	int32 GetTypeID(int32 StretchMeshID);

	UFUNCTION(BlueprintCallable)
	int32 GetSelectPathPointIndex(FVector Start, FVector Dir, UBuildingComponent *StretchMesh);

	UFUNCTION(BlueprintCallable)
	bool GetLightPath(TArray<int32> TypeList, int PointIndex, TArray<FVector>& LightPath, float& LightPointWidth, float& LightStartWidth);

	UFUNCTION(BlueprintCallable)
	float GetCeilThickness();

	UFUNCTION(BlueprintCallable)
	void  SetCeilThickness(float Thickness);

	UFUNCTION(BlueprintCallable)
	void  SetCeilShape(const FString &ResID);	
	
	UFUNCTION(BlueprintCallable)
	FString GetCeilShape();

	UFUNCTION(BlueprintCallable)
	void DeletePath();

	UFUNCTION(BlueprintCallable)
	UBuildingComponent *GetCeiling();

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnAddObject(IObject *RawObj);
	void OnDeleteObject(IObject *RawObj);
	void OnUpdateObject(IObject *RawObj, unsigned int ChannelMask);
	void OnUpdateSurfaceValue(IObject *RawObj, int SectionIndex, ObjectID Surface);
protected:
	class UBuildingSystem *GetBuildingSystem();
	ISuite *GetSuite();
	IStretchPath *GetStretchPathEditor();
	IObject *GetObject(int32 ObjID);
	void DestroyComponent(bool bPromoteChildren = false) override;
protected:
	UPROPERTY(BlueprintReadOnly)
	int32	PathID;
	UPROPERTY(BlueprintReadOnly)
	TMap<int32, UBuildingComponent *> Components;
public:
	/** Delegate that will be called when stretchpath component has been updated **/
	UPROPERTY(BlueprintAssignable, Category = StretchPath, meta = (DisplayName = "On StretchPath Update"))
	FOnStretchPathUpdate OnStretchPathUpdate;
};


