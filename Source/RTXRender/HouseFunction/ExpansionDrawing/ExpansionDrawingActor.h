// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building/StretchMesh/StretchPathComponent.h"
#include "HouseFunction/CustomDataStructure.h"
#include "ExpansionDrawingActor.generated.h"

class ADynamicTableboard;
class AFunctionManage;
UCLASS()
class RTXRENDER_API AExpansionDrawingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExpansionDrawingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UStretchPathComponent *mModel;

	void CreateTopMesh(TArray<FVector>&PointList, FString& Mx,float z, TArray<int32>& ids, EArticlelofting ty,int32 actortypeid);
	UFUNCTION(BlueprintCallable)
	void SetColor(FString MX);
	UFUNCTION(BlueprintCallable)
	void SetMesh(FString Mx);

	UFUNCTION(BlueprintCallable)
	bool IsFind(int32 id);

	UFUNCTION(BlueprintCallable)
		TArray<int32> GetSelfActorids();

	UFUNCTION(BlueprintCallable)
		EArticlelofting GetType();

	UFUNCTION(BlueprintCallable)
		void FinishArticlelofting();


	UFUNCTION(BlueprintCallable)
		void RestoreArticlelofting();

	UFUNCTION(BlueprintCallable)
		ADynamicTableboard* GetTable();

	UFUNCTION(BlueprintCallable)
		void SetTable(ADynamicTableboard* data);

	UFUNCTION(BlueprintCallable)
		void SetFunctionManage(AFunctionManage* data);

	float TopZ;

	FString MeshMx;
	FString ColorMx;

	
	UFUNCTION(BlueprintCallable)
		FCustomExpansionDrawingdata GetSaveData();


public:

	UPROPERTY(BlueprintReadOnly)
		AFunctionManage* manage;

	UPROPERTY(BlueprintReadOnly)
		int32 ActorCategoryId;

	UPROPERTY(BlueprintReadWrite)
		bool bIsFinish;

private:
	TArray<FVector> ToPointList;
	TArray<int32> Ids;

	EArticlelofting type;

	FString RecolorMx, RemeshMx;

	ADynamicTableboard* table;
};
