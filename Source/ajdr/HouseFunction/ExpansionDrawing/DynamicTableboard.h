// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExpansionDrawingActor.h"
#include "IBuildingSDK.h"
#include "IMeshEditor.h"
#include "BuildingSDKDefs.h"
#include "ResourceMeshComponent.h"
#include "TriangulatedMeshComponent.h"
#include "HouseFunction/FunctionManage.h"
#include "DynamicTableboard.generated.h"

UCLASS()
class AJDR_API ADynamicTableboard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicTableboard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UTriangulatedMeshComponent *mModel;

	FString mMx;
	float Hight;
	TArray<FVector2D>PointList;

public:	
	UFUNCTION(BlueprintCallable)
	void SetMaterial(FString mx);
	void CreateMesh(TArray<FVector2D>& PathFrame, TArray<TArray<FVector2D>>& holes, float Thickness, FString Mx, float fHeight, TArray<int32>& ids, int32 actortypeid);
	
	UFUNCTION(BlueprintCallable)
	bool IsFind(int32 id);

	UFUNCTION(BlueprintCallable)
	TArray<int32> GetSelfActorids();

	UFUNCTION(BlueprintCallable)
		void FinishArticlelofting();


	UFUNCTION(BlueprintCallable)
		void RestoreArticlelofting();

	UFUNCTION(BlueprintCallable)
		TArray<AExpansionDrawingActor*> Getbreakwaters();

	UFUNCTION(BlueprintCallable)
		void Addbreakwaters(AExpansionDrawingActor* data);

	UFUNCTION(BlueprintCallable)
		void SetFunctionManage(AFunctionManage* data);

	UFUNCTION(BlueprintCallable)
		FCustomDynamicTableboarddata GetSaveData();

public:

	UPROPERTY(BlueprintReadOnly)
	AFunctionManage* manage;

	UPROPERTY(BlueprintReadOnly)
	TArray<int32> Ids;

	UPROPERTY(BlueprintReadWrite)
		bool bIsFinish;

	UPROPERTY(BlueprintReadOnly)
		int32 ActorCategoryId;

	TArray<TArray<FVector2D>> Holes;
private:
	

	FString RecolorMx;

	TArray<AExpansionDrawingActor*> breakwaters;

	
};
