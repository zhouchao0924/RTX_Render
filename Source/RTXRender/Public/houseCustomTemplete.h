// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VaRestJsonObject.h"
#include "CompundResourceMeshComponent.h"
#include "HouseFunction/CustomDataStructure.h"
#include "houseCustomTemplete.generated.h"





/**
 * 
 */
USTRUCT(BlueprintType)
struct FConnectPins
{
	GENERATED_BODY()
public:
	FConnectPins()
		:slefIndex(0)
		,parentIndex(0)
	{

	}
	UPROPERTY(BlueprintReadOnly)
		int32 slefIndex;
	UPROPERTY(BlueprintReadOnly)
		int32 parentIndex;

};
USTRUCT()
struct FSelfPins
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FVector selfPinsVec;
	UPROPERTY()
	bool bInnerCapacity;
};
USTRUCT(BlueprintType)
struct FAssociatedAnchor
{
	GENERATED_BODY()
public:
	FAssociatedAnchor()
		:doorHandleOffset(0)
		, uvAngle(0)
		, doorhandleDirection(-1)
		, InternalsType(0)
	{

	}
	UPROPERTY()
	FString parentGuid;
	UPROPERTY()
	TArray<FSelfPins> selfPinsArr;
	UPROPERTY(BlueprintReadOnly)
	TArray<FConnectPins> connectPins;
	UPROPERTY(BlueprintReadOnly)
	FTransform CustomTramsform;
	UPROPERTY(BlueprintReadOnly)
	int32 doorHandleOffset;
	UPROPERTY(BlueprintReadOnly)
	int32 doorhandleDirection;

	UPROPERTY(BlueprintReadOnly)
	int32 InternalsType;
	UPROPERTY(BlueprintReadOnly)
	int32 CabinetBodyType;

	UPROPERTY(BlueprintReadOnly)
	int32 uvAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HouseCustom Templete ")
	int32 parentElementID;
	UPROPERTY(BlueprintReadOnly, Category = "HouseCustom Templete ")
	TArray<int32> parentPins;
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FVector> MeshSize;
	UPROPERTY(BlueprintReadOnly)
	FVector componentSize;

};
UCLASS(BlueprintType)
class RTXRENDER_API UhouseCustomTemplete : public UObject
{
	GENERATED_BODY()
public:
	UhouseCustomTemplete();
	void InitDataByJson(UVaRestJsonObject* componentListObjectJson);

	UFUNCTION()
	 void SetParentHouseCustomTempleteByGuid(TArray<UhouseCustomTemplete*> subModelComponentArr);

	UFUNCTION()
		void SetElementID(int32 id);
	UVaRestJsonObject* getquantitiesjson(FVector scale, float deflength, float defwidth, float defhight);

private:
	void SetColorData(FString Key,UVaRestJsonObject* ColorJson);

public:
	UPROPERTY(BlueprintReadOnly)
	FString fileNum;

	UPROPERTY(BlueprintReadOnly)
	FString guid;

	UPROPERTY(BlueprintReadOnly)
	int32 modelId;

	UPROPERTY(BlueprintReadOnly)
	FString MaterialName;

	UPROPERTY(BlueprintReadOnly)
	int32 CategoryId;

	UPROPERTY(BlueprintReadOnly)
	int32 ElementID;

	UPROPERTY(BlueprintReadOnly)
	FAssociatedAnchor associatedAnchor;

	UPROPERTY(BlueprintReadOnly)
	FString PrmitivefileNum;


	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FMaterialColorData> CurrentColorData;

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FMaterialColorData>privateColorData;

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FString> ColorMD5;

	 

	UPROPERTY(BlueprintReadOnly)
	int32 materialId;
	UPROPERTY(BlueprintReadOnly)
	int32 ChargeUnit;

	UPROPERTY(BlueprintReadOnly)
	TArray<int32> Pins;

	UPROPERTY(BlueprintReadOnly)
		int32 ComponentID;

	UPROPERTY(BlueprintReadOnly)
		int32 price;

	UPROPERTY(BlueprintReadOnly)
		int32 num;

	UPROPERTY(BlueprintReadOnly)
		FVector Defaultsize;

	UPROPERTY(BlueprintReadOnly)
		FVector Currentsize;


	UPROPERTY(BlueprintReadOnly)
		FString CategoryName;

	UPROPERTY(BlueprintReadOnly)
		bool MeshVisieble;

	UPROPERTY(BlueprintReadOnly)
		UVaRestJsonObject* ComponentJson;

	UPROPERTY(BlueprintReadOnly)
		TMap<FString, FVector> ScaleInfo;

	UPROPERTY(BlueprintReadOnly)
		FVector MaxMeshSize;

	UPROPERTY(BlueprintReadOnly)
		FVector MinMeshSize;

	bool IsChangeColor;
public:

	UPROPERTY(BlueprintReadOnly)
	UhouseCustomTemplete *parentHouseCustomTemplete;
};


UCLASS(BlueprintType)
class RTXRENDER_API UHouseCustomTempleteFunctionLibrary :public  UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HouseCustom Templete ")
		static UhouseCustomTemplete* CreatehouseCustomTemplete(UVaRestJsonObject* componentListObjectJson);
	
	UFUNCTION(BlueprintCallable, Category = "HouseCustom Templete ")
		static void RestoreTemplateDataan(bool IsTable,TArray<UhouseCustomTemplete*>subModelComponentArr,  
										  UCompundResourceMeshComponent* compoundElementInterface,int32 rootID,TMap<FString, UhouseCustomTemplete*>&Componentlist);
};