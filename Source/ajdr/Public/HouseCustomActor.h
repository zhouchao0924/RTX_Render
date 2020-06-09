// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "ResourceActor.h"
#include "VaRestSubsystem.h"
#include "VaRestJsonObject.h"
#include "VaRestLibrary.h"
#include "houseCustomTemplete.h"
//#include "ModelFileComponent.h "
#include "HouseFunction/SkuComponentActor.h"
#include "HouseFunction/CustomDataStructure.h"
#include "HouseCustomActor.generated.h"


class AResourceActor;
class ASkuComponentActor;
class ADynamicTableboard;
#define TAMHIGHT 2.f
UCLASS(BlueprintType)
class AJDR_API AHouseCustomActor : public AResourceActor
{
	//GENERATED_UCLASS_BODY()
	GENERATED_BODY()


public:	

	AHouseCustomActor();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	bool InitCompoundElementRootByResID(const FString& ResID);
	UFUNCTION(BlueprintCallable)
	void RestoreTemplateObject(UVaRestJsonObject* JsonObj);
	UFUNCTION(BlueprintCallable)
	void SetTempleteJson(UVaRestJsonObject* JsonObj);



	//显示/隐藏
	UFUNCTION(BlueprintCallable)
		void SetMaterialVisible(UhouseCustomTemplete* component, bool bIs);
	//选配
	UFUNCTION(BlueprintCallable)
		void SetMatching(UhouseCustomTemplete* component, bool bIs);
	//
	UFUNCTION(BlueprintCallable)
		void SetMaterialScale(TMap<FString, FVector> info,int32 Ver);

	//设置柜身两边模型
	UFUNCTION(BlueprintCallable)
		void SetCabinetBodyVisible(int32 Meshtype);
	//设置柜身两边模型
	void SetCabinetBodyVisible(UhouseCustomTemplete* Compoennt, int32 CaType);
	//还原
	UFUNCTION(BlueprintCallable)
		void RevetAllMesh();
	//设置索引
	UFUNCTION(BlueprintCallable)
		void SetIndex(int32 id);
	//创建sku
	UFUNCTION(BlueprintCallable)
		void CreateSkuMesh(FString MD5, FVector Pos, int32 id, float z, FTransform transform, bool bIs = true);
	//替换sku
	UFUNCTION(BlueprintCallable)
		bool ChangSku(float z, int32 Key, FSkuData data, ADynamicTableboard*& table);

	UFUNCTION(BlueprintCallable)
		void SetPrimitiveJson(int32 Categoryid, FString Md5);

	UFUNCTION(BlueprintCallable)
		void SetType(int32 Aid, int32 TempId, ECustomFurType ClassTypeID, int32 Categoryid, int32 Ver);

	UFUNCTION(BlueprintCallable)
		bool ReplaceSurfaceByCategoryName(FString CategoryName, const TArray<FString>&MeshName, const TArray<FString>& ResID, const TArray<int32>& ColorID, const TArray<int32>&ColorCode, const TArray<int32>&Propertyid);

	UFUNCTION(BlueprintCallable)
		void ReplaceModelByCategoryId(int32 CategoryId,const FString& ResID, int32 MaterialID, int32 chargeUnit, int32 price, FVector size,int32 modelid);
	UFUNCTION(BlueprintCallable)
		void ReplaceSurfaceByCategoryId(int32 CategoryId,const FString&MeshName, const FString& ResID,int32 ColorID, int32 ColorCode,int32 Propertyid);

	//全局替换
	UFUNCTION(BlueprintCallable)
		bool  GlobalReplacement(const FString& CategoryName, const FString& ResID, int32 MaterialID, int32 chargeUnit, int32 price, FVector size, int32 modelid,int32 HandleOffset, int32 handleDirection,bool bIsGlobal,int32 InterType, FVector maxsize, FVector minsize);

	//特殊替换
	UFUNCTION(BlueprintCallable)
		void SpecialMaterialData(const FString& CategoryName,  const FString& ResID, const FString& MaterialResID, int32 MaterialID, int32 chargeUnit, int32 price, FVector size, int32 modelid, int32 ColorID, int32 ColorCode, int32 Propertyid);

	UFUNCTION(BlueprintCallable)
		void SetSkudata(TMap<int32, FSkuData> skudata);

	//设置数据
	UFUNCTION(BlueprintCallable)
		void SetInitMeshMaterialData(int32 Ver, int32 index, FString guid, bool IsVisible, int32 InType, int32 CaType, int32 handlepos, int32 handlero);

	UFUNCTION(BlueprintCallable)
	UVaRestJsonObject* getCalculatejson();



	UFUNCTION(BlueprintCallable)
		void GetMaterialScale();

	//完成定制进行数据存储
	UFUNCTION(BlueprintCallable)
		void FinishTemplate();



	UFUNCTION(BlueprintCallable)
		void GetFinishColorJson(UhouseCustomTemplete* component, UVaRestJsonObject* json);
	//模板物料和颜色还原
	UFUNCTION(BlueprintCallable)
		void RestoreTemplate();

	//更新拉手偏移和内构精确位置
	UFUNCTION(BlueprintCallable)
		void UpdataHandleOffset();

	//设置拉手偏移
	UFUNCTION(BlueprintCallable)
		void SetHandleOffset(int index);
	//设置拉手偏移
	UFUNCTION(BlueprintCallable)
		void SetHandleOffsetex(UhouseCustomTemplete* component);

	//设置拉手偏移
	UFUNCTION(BlueprintCallable)
		void SetHandleOffsetIndex(UhouseCustomTemplete* component,int index);


	//设置门拉手隐藏/显示
	UFUNCTION(BlueprintCallable)
		void SetDoorHandleVisible(bool bIs);

	//设置数据
	UFUNCTION(BlueprintCallable)		
		void SetClassSaveData(int32 Index,int32 doorHandleOffset, int32 doorhandleDirection);

	//风格替换
	UFUNCTION(BlueprintCallable)
		void SetStyleTemplate(UVaRestJsonObject* StyleJson);

	//内构替换特殊处理
	UFUNCTION(BlueprintCallable)
		bool  DisposeInternals(int type, FString Res, UhouseCustomTemplete*Internal, FVector maxsize, FVector minsize);

	//拉手缩放锁定
	UFUNCTION(BlueprintCallable)
		void RevertLaShou();

	//更新内构
	UFUNCTION(BlueprintCallable)
		void UpdataInternals(UhouseCustomTemplete*Internal,bool bIs = true);

	UFUNCTION(BlueprintImplementableEvent)
		void GetBox(UhouseCustomTemplete* CreateData, FBox& InnerCapacityBox, FBox& OuterCapacityBox);

	UFUNCTION(BlueprintImplementableEvent)
		void GetSize(UhouseCustomTemplete* CreateData ,FVector& Size);

	//查找物料
	UFUNCTION(BlueprintCallable)
		bool FindComponent(const FString& MaterialName  ,TArray<UhouseCustomTemplete*>& Componts);


	//检测更新龙头位置
	UFUNCTION(BlueprintCallable)
		void ChekUpdateSpigot(const FString& Meshname,bool bWt, bool bIs = true);

	void ChekUpdateSpigottl(const FString& Meshname, bool bWt, bool bIs = true);
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "Create Custom Template"))
		void CreateHouseCustomActorByTemplateId(UObject* WorldContextObject,int32 ID,const FString& URL);

	//获取模型位置
	UFUNCTION(BlueprintCallable)
		FBoxSphereBounds GetMeshContent(const FString& CategoryName, const FString& MeshName);

	UFUNCTION(BlueprintCallable)
		UhouseCustomTemplete* FindCutomTemplate(const FString& meshName);

	UFUNCTION(BlueprintCallable)
		void RemoveSkudata(int32 id);

	//1台面 2踢脚板 3顶角线
	UFUNCTION(BlueprintCallable)
		void SetLoft(bool bIs, int type);

	UFUNCTION(BlueprintCallable)
		void SetLoftex(FIsLoft date);

	void  CreateHouseCustomActorCallback(class UVaRestRequestJSON* Request);


	FVector GetSize(int32 ID);

	UVaRestJsonObject * CalculateMaterialByArray(TArray<UhouseCustomTemplete*> componts, FVector scale, FVector size);
public:
	UPROPERTY(BlueprintReadOnly)
	UCompundResourceMeshComponent* compoundElementInterface;
	UPROPERTY(BlueprintReadOnly)
	UVaRestJsonObject* TempleteJson;
	UPROPERTY(BlueprintReadOnly)
	TArray<UhouseCustomTemplete*> subModelComponentArr;
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, UhouseCustomTemplete*> ComponentList;
	UPROPERTY(BlueprintReadWrite)
	TMap<int32, ASkuComponentActor*> SkuComponent;

	TMap<FString, TArray<UhouseCustomTemplete*>>ComponentType;

	//版本控制
	UPROPERTY(BlueprintReadOnly)
		int32 Versions;

	UPROPERTY(BlueprintReadOnly)
		int32 ActorId;

	UPROPERTY(BlueprintReadOnly)
		int32 TemplateId;

	UPROPERTY(BlueprintReadOnly)
		int32 TemplateCategoryId;

	UPROPERTY(BlueprintReadOnly)
		ECustomFurType ClassType;

	UPROPERTY(BlueprintReadOnly)
		FString Url;

	UPROPERTY(BlueprintReadOnly)
		int32 CounterclockwiseIndex;

	UPROPERTY(BlueprintReadOnly)
		TMap<int32, FSkuData> Skuinfo;

	UPROPERTY(BlueprintReadOnly)
		FVector TemplateSize;

	UPROPERTY(BlueprintReadOnly)
		FVector TemplateMaxScale;

	UPROPERTY(BlueprintReadOnly)
		FVector TemplateMinScale;

	UPROPERTY(BlueprintReadWrite)
		int32 LCategoryid;

	UPROPERTY(BlueprintReadWrite)
		int32 Roomid;

	UPROPERTY(BlueprintReadWrite)
		bool IsTable;

	UPROPERTY(BlueprintReadOnly)
		FIsLoft loft;

	UPROPERTY(BlueprintReadOnly)
		FString MD5;

	UPROPERTY(BlueprintReadOnly)
		FString PlaygroupID;

	UPROPERTY()
	int32 RootElementID;
	UPROPERTY()
	FVaRestCallDelegate CallBack;


	UPROPERTY(BlueprintReadWrite)
		UVaRestJsonObject* PrimitiveTempleteJson;



private:
	FVector2D GetHandleOffsetPos(UhouseCustomTemplete* Mesh);

	FVector GetMeshSize(const FString& Name);

	FVector GetMeshSize(const FString& meshname, const FString& compoentname,FVector& Pos);

	const FString CabinetBodyName0;
	const FString CabinetBodyName1;

};
