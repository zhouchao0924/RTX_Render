// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoreGlobals.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VaRestJsonObject.h"
#include "Misc/Guid.h"
#include "CustomProductUtils.generated.h"

class UCompundResourceMeshComponent;

USTRUCT(BlueprintType)
struct FElementInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementInfo)
		FString								Guid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementInfo)
		int32								ElementId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementInfo)
		FString								ParentGuid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementInfo)
		int32								DoorHandleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementInfo)
		float								UVAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementInfo)
		TMap<int32, FVector>				Pins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementInfo)
		TMap<int32, int32>					Connection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementInfo)
		FString								JsonInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ElementInfo)
		TArray<bool>						PinPos;

public:

	FElementInfo()
	{
		Guid = FGuid::NewGuid().ToString();
		ElementId = -1;
		DoorHandleOffset = -1;
		UVAngle = 0.00f;
	}

};

USTRUCT(BlueprintType)
struct FMeshInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
		int32 CategoryId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MeshInfo)
		TArray<FString>  MeshName;
};


/**
 * 
 */
UCLASS()
class RTXRENDER_API UCustomProductUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure, Category = Utils)
		static bool HasAnyParent(const FElementInfo& Target);
	
	UFUNCTION(BlueprintPure, meta = (DisplayName = "PinExists"), Category = Utils)
		static int32 PinExists_Vector(const FElementInfo& Target, const FVector& Pin);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "PinExists"), Category = Utils)
		static int32 PinExists_Id(const FElementInfo& Target, int32 Pin);

	UFUNCTION(BlueprintPure, Category = Utils)
		static bool CheckIsInGameThread();

	UFUNCTION(BlueprintPure, Category = Utils)
		static FBox2D ProjectActorBoundsToScreen(const APlayerController* Player, const FBox& Bounds);

	UFUNCTION(BlueprintPure, Category = Utils)
		static bool IsPointInsideBox(const FBox2D& Box, const FVector2D& Point);

	UFUNCTION(BlueprintPure, Category = Utils)
		static int32 CheckPinIndex(const FVector& Pin);

	UFUNCTION(BlueprintPure, Category = Utils)
		static FVector GetPinNormalizeLocation(int32 Pin);

	UFUNCTION(BlueprintPure, Category = Utils)
		static TArray<FVector> GetComponentAngularPoints(UPrimitiveComponent* Target);

	UFUNCTION(BlueprintPure, Category = Utils)
		static FString GetNewGuid();

	UFUNCTION(BlueprintPure, Category = Utils)
		static TArray<FVector> GetElementAngularPoints(const FVector& Original, const FVector& Size);

	UFUNCTION(BlueprintPure, Category = Utils)
		static FVector GetPinFixedWorldLocation(const FVector& Original, const FVector& Location);

	/** Converts a vector value to a string, in the form 'X= Y= Z=' */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (Vector Without Precision Control)", CompactNodeTitle = "->", BlueprintAutocast), Category = Utils)
		static FString Conv_VectorToString(FVector InVec);

	/*Is specified Category?*/
	UFUNCTION(BlueprintPure, Category = Utils)
		static  bool  HasSpecifiedMeshName(int32 InCategoryId, TArray<FMeshInfo> InMeshInfo);

	/*Get MeshName by CategoryId*/
	UFUNCTION(BlueprintPure, Category = Utils)
		static  TArray<FString> GetMeshNamesByCategoryId(int32 InCategoryId, TArray<FMeshInfo> InMeshInfo);
	
	/*Set Element Surface by CatergoryId*/
	UFUNCTION(BlueprintCallable, Category = Utils)
		static void SetElementSurfaceByCategoryId(UCompundResourceMeshComponent *InCompoundResourceMesh, int32 InElementId, int32 InCategoryId, TArray<FMeshInfo> InMeshInfo,FString ResID);

	UFUNCTION(BlueprintCallable, Category = Utils)
		static void SetElementSurface(UCompundResourceMeshComponent *InCompoundResourceMesh, int32 InElementId, int32 InCategoryId, TArray<FMeshInfo> InMeshInfo, FString ResID);
};
