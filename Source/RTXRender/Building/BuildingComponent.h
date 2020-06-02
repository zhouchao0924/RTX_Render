// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BuildingData.h"
#include "ProceduralMeshComponent.h"
#include "WallPlane.h"
#include "BuildingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNotifyUpdateSurface, int32, SectionIndex, int32, SurfaceID, int32, ComponetType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNotifyCreateMesh);

UCLASS(BlueprintType)
class UBuildingComponent :public UProceduralMeshComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetData(UBuildingData *Data);

	UFUNCTION(BlueprintCallable)
	void UpdateSurface(int32 SectionIndex, int32 SurfaceID);

	//��ȡ�����SurfaceID
	UFUNCTION(BlueprintCallable)
	int32 HitTestSurface(const FVector &RayStart, const FVector &RayDir) const;
	//��ȡ�����SubModelIndex
	UFUNCTION(BlueprintCallable)
	int32 HitTestSubModelIndex(const FVector &RayStart, const FVector &RayDir);
	//ͨ��SubSectionIndex��ȡSurfaceID
	UFUNCTION(BlueprintCallable)
	int32 GetSurfaceIDBySectionIndex(const int32& SectionIndex);

	///////////////////////material///////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable)
	void SetSaveUVData(const FVector2D& UV, const FVector2D& UVOffset, const float& Angle);
	
	/*UFUNCTION(BlueprintCallable)
	void SetMat2DParam(UPARAM(ref) FString& ParamName, const FVector2D& Value);
	UFUNCTION(BlueprintCallable)
	void SetMat3DParam(UPARAM(ref) FString& ParamName, const FVector& Value);
	UFUNCTION(BlueprintCallable)*/
	void SetMat4DParam(UPARAM(ref) FString& ParamName, const FVector4& Value);
	//private
	void SetSurfaceParam(const int32& SurfaceID, const FString& ParamName, IValue* Value);
	//����pSurface���ʲ���
	UFUNCTION(BlueprintCallable)
		void SetSurfaceFloatParam(const int32& SurfaceID, UPARAM(ref) FString& ParamName, const float& Value);
	//��ȡpSurface���ʲ���
	UFUNCTION(BlueprintCallable)
		float GetSurfaceFloatParam(const int32& SurfaceID, UPARAM(ref) FString& ParamName);

	//����pSurface����ֵ�����ڴ洢
	UFUNCTION(BlueprintCallable)
	void SetSurfaceParams(const int32& SurfaceID, const FVector2D& UVScale, const FVector2D& UVOffset, const float& Angle, const FVector2D& DefaultUV);
	// ��ȡpSurface����ֵ��������ʾ
	void GetSurfaceParams(const int32& SurfaceID, FVector2D& UVScale, FVector2D& UVOffset, float& Angle, FVector2D& DefaultUV);

	UFUNCTION(BlueprintCallable)
	void GetMatDefaultParamValues(const int32& SectionIndex, FVector2D& UVOffset, float& Angle, FVector2D& DefaultUV, FVector2D& ScaleUV);

	//����ĳ���ӱ�����ʣ�MaterialTypeΪ0��ʾ���ò��ʣ�Ϊ1��ʾue4���� 
	UFUNCTION(BlueprintCallable)
	void SetSectionMaterial(FVector2D& MaterialUVScale, const int32 &SectionIndex, const int32& ModelID, UPARAM(ref)FString& ResID, const int32& MaterialType = 1, const FVector2D& UVSacle = FVector2D(100.0, 100.0), const FVector2D& UVOffset = FVector2D(0.0, 0.0), const float& Angle = 0.0, const FVector2D& DefaultUV = FVector2D(1.0, 1.0));
	UFUNCTION(BlueprintCallable)
	FVector2D SetSectionMaterialUseDefaultParams(const int32 &SectionIndex, const int32& ModelID, UPARAM(ref)FString& ResID);
	//�����ӱ����һ�����ʲ���
	UFUNCTION(BlueprintCallable)
	void SetSectionMaterialParam(const int32 &SectionIndex, UPARAM(ref) FString& ParamName, const float& Value);
	UFUNCTION(BlueprintCallable)
	void SetSectionMaterialParams(const int32 &SectionIndex, const FVector2D& UV = FVector2D(1.0, 1.0), const FVector2D& UVOffset = FVector2D(0.0, 0.0), const float& Angle = 0.0);

	UMaterialInterface * GetMaterialInterfaceBySurfaceID(const int32& SurfaceID);
	UMaterialInterface * GetMaterialInterfaceByResIDAndType(const FString& ResID, const int32& MaterialType);
	void AutoSetMatParams(const int32& SectionIndex, const int32& SurfaceID, UMaterialInterface *UE4Mat);
	//��ȡ���ʵ�Ĭ�ϲ���
	UFUNCTION(BlueprintCallable)
	float GetMatDefaultFloatParam(const UMaterialInterface *UE4Mat, const FString& ParamName);
	
	TArray<FString>  GetMaterialResId();
	///////////////////////material///////////////////////////////////////////////////


public:
	//0��ʾ�ǵذ壬��PGround��1��ʾ���컨�壬��roof, 2��ʾ��LitMesh
	UPROPERTY(Transient, BlueprintReadWrite)
		int32 ComponetType;
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "DR|Resource")
		FOnNotifyUpdateSurface					OnNotifyUpdateSurface;
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "DR|Resource")
		FOnNotifyCreateMesh OnNotifyCreateMesh;

	//FVector2D UVDefault = FVector2D(1.0, 1.0);	//
	UPROPERTY(Transient, BlueprintReadOnly)
	UBuildingData *Data;
	UPROPERTY(Transient, BlueprintReadOnly)
	int32 HitSurfaceID;
	UPROPERTY(BlueprintReadOnly)
	AWallPlaneBase *WallPlane;
};


