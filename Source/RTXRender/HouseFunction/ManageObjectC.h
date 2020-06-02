// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FunctionManage.h"
#include "VaRestJsonObject.h"
#include "ManageObjectC.generated.h"

struct FCustomFurTree
{

	int32 Categoryid;
	int32 ParentCategoryid;
	int32 TwoCategoryid;
	FString TwoName;
	FString name;
	int32 Level;
	TArray <FCustomFurTree*> child;
};

USTRUCT(BlueprintType)
struct FCurreentLevelData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Categoryid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ParentCategoryid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 TwoCategoryid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString TwoName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Level;

};

USTRUCT(BlueprintType)
struct FCurrentCustomFurDate
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Actorid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Pos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Categortid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Z;
};

class ASkuComponentActor;
UCLASS()
class RTXRENDER_API AManageObjectC : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AManageObjectC();
	~AManageObjectC();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void SetFunctionManage(AFunctionManage* ref);

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AFunctionManage* FunctionManageInstance;

	static AManageObjectC* GetInstance();

public:
	UFUNCTION(BlueprintCallable)
		void CreateCutomFurTree(UVaRestJsonObject* Json);


	UFUNCTION(BlueprintCallable)
		bool GetCurrentData(FCurreentLevelData& data, TArray<FCurreentLevelData>& ChildData, int32 Categoryid);

	UFUNCTION(BlueprintCallable)
		void ClearTree();


public:
	UFUNCTION(BlueprintImplementableEvent)
		AFunctionManage* GetManageFunction(const TArray<FCustomData>& CreateData);

	
		

	UFUNCTION(BlueprintImplementableEvent)
		void ExceptionHandling(const FString& warning, const int32& type);

	UFUNCTION(BlueprintImplementableEvent)
		bool TemplatesDownload(const TArray<int32>& templates, const TArray<int32>& skuids);

	UFUNCTION(BlueprintImplementableEvent)
		void GetClearData(const FString& AreaTag, const TArray<int32>& Actorid, const int32& Categoryid);

	UFUNCTION(BlueprintImplementableEvent)
		TArray<FCurrentCustomFurDate> GetFurDate(const FString& AreaTag);

	UFUNCTION(BlueprintImplementableEvent)
		TArray<UVaRestJsonObject*> GetBakewaterList();


	UPROPERTY(BlueprintReadWrite)
		float TampZ;

	UFUNCTION(BlueprintCallable)
		void SetJsonData(TArray<UVaRestJsonObject*> Sku,  TArray<UVaRestJsonObject*> Template);

public:

	TArray<UVaRestJsonObject*> GetTemplateList();
	TArray<UVaRestJsonObject*> GetSkuList();


private:

	void GetChildData(TArray<FCurreentLevelData>& Datalist, FCustomFurTree*Furdata);

	void GetJsonData(UVaRestJsonObject* Json, FCustomFurTree* child);

	bool GetCrreent(int32 Categoryid, FCustomFurTree* Furdata);

	void RemoveAll(FCustomFurTree* TreeData);


	TArray<UVaRestJsonObject*> Skulist;
	TArray<UVaRestJsonObject*> TempList;

private:

	FCurreentLevelData temp;
	TArray<FCurreentLevelData> ChildTemp;

	int32 TempCategoryid = 0;
	FString TName;

	static AManageObjectC* Myinstance;

	FCustomFurTree* FurTreeData;



};
