// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CeilTemplate.h"
#include "CeilWrapper.generated.h"

UCLASS()
class ACeilWrapper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACeilWrapper();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	struct ElementRes
	{
		int id;
		TArray<FVector> vBoundary;
		TArray<UMaterialInterface*> vMaterial;
	};

public:
	//����ģ����Դ
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	bool SetElementRes(int uID, FString const& strResName, UMaterialInterface* UMaterialInterface = nullptr, bool bClearAllBeforeSet = false);

	//���÷���·��
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	bool SetPath(TArray<FVector2D> const& vPath, bool bClose = true, bool bUseDefaultTemplate = false, float fDefaultTemplateScale = 1.f);

	//���ɷ���ģ��
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	bool CreateMesh(bool b3DLoft = true, float fHeight = 0.f);

	//UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	//bool CreateSurface();

	//��ʾ/����ģ��Ԫ��
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	bool ShowElement(bool bShow, int uEdgeID = -1, int uID = -1);

	//����ģ��Ԫ�سߴ�
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	bool UpdateElement(int uEdgeID, int id, float XSize, float YSize);

	//��ȡ���ڱߵ�ģ�����
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	UActorComponent* GetCeilActor(int uEdgeID) const;

	//��ȡģ������ı�������Ϣ
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	int GetCeilActorID(UActorComponent* pActor) const;

	//��ȡ���ڱߵ�ģ��Ԫ��
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	UActorComponent* GetElementActor(int uEdgeID, int uID, int uType = 0) const;

	//��ȡ���ڱߵ�ģ��Ԫ�ص�ԭʼ2D�ߴ�
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	bool GetElementSize(int uEdgeID, int uID, FVector2D & Size) const;

	//��ȡͲ��λ��
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	FVector GetLampPosition(int uEdgeID) const;

private:
	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	TArray<FVector2D> GetPathFromRoomId(int32 uRoomID) const;

	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	bool GetBoundary(FString const& strMXFileName, TArray<FVector> & vBoundary) const;

	UFUNCTION(BlueprintCallable, Category = "DR|TEST")
	void Clear(bool bOnlyRedraw = true, bool bClearEleRes = false);

	bool CreateMeshEx(bool b3DLoft = true, float fHeight = 0.f);
	bool InitTemplateData(UCeilTemplate* pActor, float* fTopLayerSize = nullptr);
	//void AutoBlend(TArray<UProceduralMeshComponent*> pProcMesh, TArray<FVector2D> vPath);
	UCeilTemplate* CreatTemp(UWorld* pWorld);//(FName const& name)
	void CreateTopLayer(UWorld* pWorld, UMaterialInterface* pMaterial, float fSize);

private:
	TArray<FVector2D>		mvPath;
	TArray<UCeilTemplate*>	mvCeil;
	TArray<ElementRes>		mvEleRes;
	//
	USceneComponent*		mpTopLayer;
	float					mfTopLayerSize;
	float					mfTopLayerSizeBK;
	UMaterialInterface*		mpTopLayerMate;
	static const int		muTopLayerID = -100;
	UMaterial*				mpDefMate;
	float					mfDelta;
	bool					mbDirty;
	bool					mbUseDefaultTemplate;
	float					mfDefaultTemplateScale;

	//
	TArray<FVector>			mvPathEx;



};
