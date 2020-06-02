
#pragma once

#include "ModelPartComponent.h"
#include "Building/BuildingData.h"
#include "Building/DRStruct.h"
#include "ModelFileComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateModeled);
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class UModelFileComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void SetData(UBuildingData * Data,FDRComplexModel Model);

	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void UpdateModel(UModelFile *Model);

	//����ģ��,���޸�����,��Ҫ��ģ���鴴��֮����ܸ��²���
	UFUNCTION(BlueprintCallable, Category = "Model")
	void ChangeModelData(const int32& ModelID, UPARAM(ref) FString& ResID);

	//����ģ�ͣ��滻attachcomponent
	UFUNCTION(BlueprintCallable, Category = "Model")
	void ChangeModel(const FString& ResID);

	//�������� , MaterialIndexΪ-1,��ʾ�滻ȫ����Ϊ>=0Ϊ�滻ָ������
	UFUNCTION(BlueprintCallable, Category = "Material")
	void ChangeModelMaterial(const FString& ResID, int32 MaterialIndex = -1);
	
	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void UpdateMaterial(int32 MaterialIndex);

	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void SetMaterialByPart(int32 iPart, UMaterialInterface *Material);

	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void SetMaterialByChannel(int32 iMtrlChannel, UMaterialInterface *Material);

	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	UModelPartComponent *GetPart(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	int32 GetNumOfParts() { return Parts.Num(); }
	 
	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	UModelFile *GetModel();

	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void UpdatePhysics();

	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void SetSimulatePhysic(bool bSimulate);

	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void SetAffectByShadow(bool bAffectByShadow);

	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void SetCastShadow(bool bCastShadow);

	FBox GetWorldBounds();
	UFUNCTION(BlueprintCallable, Category = "DR|ModelComponent")
	void SetModelAlignType(ECenterAdjustType InAdjustType);
	void DestroyComponent(bool bPromoteChildren  = false ) override;
	static FTransform GetOffset(UModelFile *Model, ECenterAdjustType AlignType);
	static FTransform GetOffset(const FBox &Bounds, const FVector &Scale3D, ECenterAdjustType AlignType);
	FTransform GetRelativeTransformModeFile(UModelFile *Model);
	void AddClipPlane(const FPlane &InPlane);
public:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "DR|ModelComponent")
	TSubclassOf<UStandardMaterialCollection>  StandardMaterials;
protected:
	UPROPERTY(Transient)
	FString				ModelResID;
	UPROPERTY(Transient)
	FBox				LocalBounds;
	UPROPERTY(Transient, BlueprintReadOnly)
	ECenterAdjustType	AdjustType;
	TArray<FPlane>		ClipPlanes;
public:
	UPROPERTY(Transient)
	TArray<UModelPartComponent*> Parts;
	UPROPERTY(BlueprintReadWrite)
		UBuildingData * BuildData;
	UPROPERTY(BlueprintReadWrite)
		FDRComplexModel ComplexModel;
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "DR|ModelComponent")
		FOnUpdateModeled	UpdateModelDone;
};

