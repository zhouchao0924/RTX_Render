// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"
#include "IBuildingSDK.h"
#include "IMeshEditor.h"
#include "BuildingSDKDefs.h"
#include "ResourceMeshComponent.h"
#include "CompundResourceMeshComponent.generated.h"

USTRUCT(BlueprintType)
struct BUILDINGSDK_API FCompoundElementInterface
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	int32 ElementID;

	UPROPERTY(BlueprintReadWrite)
	TArray<int32> Pins;
};

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class BUILDINGSDK_API UCompundResourceMeshComponent : public UResourceMeshComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	int32 AddElement(const FString &ResID);

	UFUNCTION(BlueprintCallable)
	bool DeleteElement(int32 ElementID);

	UFUNCTION(BlueprintCallable)
	void SetElementModel(int32 ElementID, const FString &ResID, bool bKeepPin = false);

	UFUNCTION(BlueprintCallable)
	void SetElementSurfaceByMeshName(int32 ElementID, const FString &MeshName,  const FString &ResID);

	UFUNCTION(BlueprintCallable)
	void SetElementVisibleByMeshName(int32 ElementID, const FString &MeshName, bool IsVisible);

	UFUNCTION(BlueprintCallable)
	void SetElementSurfaceByMaterialName(int32 ElementID, const FString &MaterialName, const FString &ResID);

	UFUNCTION(BlueprintCallable)
	FVector GetElementScale(int32 ElementID);

	UFUNCTION(BlueprintCallable)
	void GetElementLocationAndRotation(int32 ElementID, FVector &Location, FRotator &Rotation);

	UFUNCTION(BlueprintCallable)
	void SetElementTransform(int32 ElementID, FRotator Rotation, FVector Location, FVector Scale = FVector(1.0f));

	UFUNCTION(BlueprintCallable)
	FVector ConvertToNormalizeLocation(int32 ElementID, FVector WorldLocation, bool bInnerCapacity);

	UFUNCTION(BlueprintCallable)
	FVector ConvertToWorldLocation(int32 ElementID, FVector NormalLocation, bool bInnerCapacity);

	UFUNCTION(BlueprintCallable)
	int32 CreateInplacePinAtParent(int32 ParentElementID, bool bInnerCapacityParent, int32 SubElementID, int32 SubPinID);

	UFUNCTION(BlueprintCallable)
	int32 GetElement(UPrimitiveComponent *Component);
	UFUNCTION(BlueprintCallable)
		void GetMeshcCenter(int32 ElementID, FVector &MeshCenter, const FString &MeshName);

	UFUNCTION(BlueprintCallable)
	bool GetElementInfo(int32 ElementID, int32 &ObjID, FString &ResID, FBox &InnerCapacityBox, FBox &OuterCapacityBox, FBox &MeshBounds);

	UFUNCTION(BlueprintCallable)
	void SetPinNormalizeLocation(int32 PinID, FVector NormalizeLocation);

	UFUNCTION(BlueprintCallable)
	int32 CreatePin(int32 ElementID, FVector UniformLocation, bool bInnerCapacity = true);

	UFUNCTION(BlueprintCallable)
	bool DeletePin(int32 PinID);

	UFUNCTION(BlueprintCallable)
	void GetPinCount(int32 ElementID, TArray<int32> &ElmemtPins);

	UFUNCTION(BlueprintCallable)
	bool GetPinInfo(int32 PinID, FVector &WorldLocation, FVector &NormalizeLocation, FVector& LocalLocation);

	UFUNCTION(BlueprintCallable)
	bool GetConnectPins(int32 ElementID0, int32 ElementID1, TArray<int32> &Pins0, TArray<int32> &Pins1);

	UFUNCTION(BlueprintCallable)
	bool IsConnect(int32 ElementID);

	UFUNCTION(BlueprintCallable)
	bool Connect(const FCompoundElementInterface &FixedInterface, const FCompoundElementInterface &SubInterface);

	UFUNCTION(BlueprintCallable)
	void UnConnect(int32 ElementID, bool bKeepTransform);

	UFUNCTION(BlueprintCallable)
	void GetElementMaterials(TArray<UMaterialInstanceDynamic *> &DynamicMaterials, int32 ElementID, int32 iSection = -1);

	UFUNCTION(BlueprintCallable)
	void GetElementMaterialBySourceMaterialName(TArray<UMaterialInstanceDynamic *> &DynamicMaterials, int32 ElementID, const FString &MaterialName);

	UFUNCTION(BlueprintCallable)
	void GetElementMaterialByMeshName(TArray<UMaterialInstanceDynamic *> &DynamicMaterials, int32 ElementID, const FString &MeshName);

	UFUNCTION(BlueprintCallable)
	void GetElementSize(int32 ElementID, int32 &Width, int32 &Height, int32 &Depth, bool bComponentInSpace = false);
	
	UFUNCTION(BlueprintCallable)
	void GetMeshSize(int32 ElementID, FVector &MeshSize, const FString &MeshName);

	UFUNCTION(BlueprintCallable)
	void GetElementMeshNames(int32 ElementID, TArray<FString> &MeshNames);

	UFUNCTION(BlueprintCallable)
	bool IsConnectValid(const TArray<FVector> &SubLoctions, const TArray<FVector> &FixedLoctions);

	UFUNCTION(BlueprintCallable)
	void SetSurfaceScalarValue(int32 ElemID, const FString &MeshName, const FString &ParamName, float ScalarVaule, int32 iSection = -1);

	UFUNCTION(BlueprintCallable)
	void SetSurfaceValue(int32 ElemID, const FString &MeshName, const FString &ParamName, const FLinearColor &Value, int32 iSection = -1);

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
protected:
	void GetSurfaces(int32 ElemID, TArray<ISurfaceObject *> &Surfaces, const FString &MeshName, int32 iSection = INDEX_NONE);
	ICompoundModel *GetCompoundEditor();
	kXform ToXform(const FTransform &Transform);
	void OnRegisterComponent(IObject *pObj) override;
public:
	UPROPERTY(Transient, EditAnywhere)
	bool	bDebug;
};



