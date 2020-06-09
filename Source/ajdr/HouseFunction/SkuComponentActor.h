// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomDataStructure.h"
#include "SkuComponentActor.generated.h"

class AHouseCustomActor;
class UModelFile;
class UModelFileComponent;
class ADynamicTableboard;
UCLASS()
class AJDR_API ASkuComponentActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkuComponentActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
		bool GetMeshLocation(const FString& MeshName,FVector& Loaction);

	UFUNCTION(BlueprintCallable)
		bool GetMeshLocationEx(const FString& MD5,const FString& MeshName, FVector& Loaction);

	UFUNCTION(BlueprintCallable)
		bool GetIsMove();

	void SetDynamicTable(ADynamicTableboard* tab);

	UFUNCTION(BlueprintCallable)
		void InitSkuData(const FSkuData &data,bool bclone=false);
	 void SetMove(bool bIs);

	 UFUNCTION(BlueprintCallable)
	 ASkuComponentActor* Clone();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
		UModelFileComponent* Sku;

	UPROPERTY(BlueprintReadOnly)
		AHouseCustomActor* ParentActor;

	UPROPERTY(BlueprintReadOnly)
		int32 CategoryId;

	UPROPERTY(BlueprintReadOnly)
		int32 ActorCategoryId;

	UPROPERTY(BlueprintReadOnly)
	UModelFile *modelFile;
	UPROPERTY(BlueprintReadOnly)
		int32 modelid;
	UPROPERTY(BlueprintReadOnly)
		int32 skuid;
	UPROPERTY(BlueprintReadOnly)
		FString Mx;
	UPROPERTY(BlueprintReadOnly)
		bool isclone;
	UPROPERTY(BlueprintReadOnly)
		FString myUUID;
	UPROPERTY(BlueprintReadOnly)
		bool TransformChange;

	float GetWashbowlZ();
	float GetWashbowlZ(const FString& MD5,bool& bIs);

	void CreateSku(FString Md5, int32 id, int32 actortypeid);

	ADynamicTableboard*  ChangeSku(FSkuData data);

	bool GetHole(TArray<FVector2D>& Holelist);

	void SetUUID();
private:

	bool IsMove;
	ADynamicTableboard* Table;
};
