// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IProperty.h"
#include "Building/Building.h"

#include "BuildingData.generated.h"

USTRUCT(BlueprintType)
struct FPropertyInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString		ClassName;

	UPROPERTY(BlueprintReadOnly)
	FString		PropName;
	
	UPROPERTY(BlueprintReadOnly)
	int32		Type;
	
	UPROPERTY(BlueprintReadOnly)
	bool		bReadOnly;
};

class ISuite;
class IObject;
class IProperty;
class IObjectDesc;
class UBuildingSystem;

UCLASS(BlueprintType)
class UBuildingData :public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure)
	int32 GetNumberOfProperties();

	UFUNCTION(BlueprintPure)
	int32 GetPropertyType(int32 Index);

	UFUNCTION(BlueprintPure)
	FString GetPropertyName(int32 Index);

	UFUNCTION(BlueprintPure)
	bool IsPropertyReadOnly(int32 Index);

	UFUNCTION(BlueprintPure)
	FPropertyInfo GetPropertyInfo(int32 Index);

	UFUNCTION(BlueprintPure)
	float GetFloat(const FString &Name);

	UFUNCTION(BlueprintPure)
	bool  GetBool(const FString &Name);

	UFUNCTION(BlueprintPure)
	int32  GetInt(const FString &Name);

	UFUNCTION(BlueprintPure)
	FString  GetString(const FString &Name);

	UFUNCTION(BlueprintPure)
	FVector  GetVector(const FString &Name);

	UFUNCTION(BlueprintPure)
	FVector2D  GetVector2D(const FString &Name);

	UFUNCTION(BlueprintPure)
	FRotator  GetRotation(const FString &Name);

	UFUNCTION(BlueprintPure)
	FBox  GetBounds(const FString &Name);

	UFUNCTION(BlueprintPure)
	FPlane  GetPlane(const FString &Name);

	UFUNCTION(BlueprintPure)
	bool  IsRoot();

	UFUNCTION(BlueprintCallable)
	void SetFloat(const FString &Name, float fValue);

	UFUNCTION(BlueprintCallable)
	void SetInt(const FString &Name, int32 iValue);

	UFUNCTION(BlueprintCallable)
	void SetBool(const FString &Name, bool bValue);

	UFUNCTION(BlueprintCallable)
	void SetVector(const FString &Name, const FVector &Value);

	UFUNCTION(BlueprintCallable)
	void SetVector2D(const FString &Name, const FVector2D &Value);

	UFUNCTION(BlueprintCallable)
	void SetRotator(const FString &Name, const FRotator &Value);

	UFUNCTION(BlueprintCallable)
	void SetString(const FString &Name, const FString &Value);

	UFUNCTION(BlueprintCallable)
	void SetBounds(const FString &Name, const FBox &Box);

	UFUNCTION(BlueprintCallable)
	int32 GetObjectType();

	UFUNCTION(BlueprintCallable)
	int32 GetID() { return ObjID; }

	UFUNCTION(BlueprintCallable)
	void Update();

	struct FBuildingConfig *GetConfig();
public:
	UPROPERTY(BlueprintReadOnly)
		int32 TenmpID;

	ISuite *GetSuite();
	IObject *GetRawObj();
	IObjectDesc *GetRawDesc();
	IProperty *GetProperty(int32 Index);
	IProperty *GetProperty(const FString &Name);
	UBuildingSystem *GetBuildingSystem();
	IValueFactory *GetValueFactory();
	friend class UBuildingSystem;

protected:
	void SetRawObj(int32 InObjID);
	void GetAllDesc(TArray<IObjectDesc*> &Descs, TArray<int32> &DescCounts);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Suite")
	int32					ObjID;
	UPROPERTY(Transient)
	UBuildingSystem			*BuildingSystem;
};