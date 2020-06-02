#pragma once
#include "CoreMinimal.h"
#include "./base/common.h"
#include "Struct.h"
#include "CabinetSave.h"
#include "HomeLayout/HouseArchitect/PlaneHousePicture.h"
#include "VaRestRequestJSON.h"
#include "CabinetStruct.h"
#include "CabinetSdk.generated.h"

UCLASS()
class UCabinetSdk: public  UObject
{
	GENERATED_BODY()
public:
	
	//����ǽ
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetAutoSetWalls(const TArray<FAreaInnerWall>& outWallLines, const TArray<FCustomizationBoolean>& outBooleanData,int nType, FString Tag);
	//ѡ��ǽ
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static 	bool SetAutoSlecctWalls(const TArray<int32>& WallsID, int nType);	
	//ѡ��ǽ
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static 	bool SetSlecctWalls(const TArray<FLine2dSelect>& SelectWalls, int nType);
	//�滻areatage
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool ChangAreaTag(FString newid, FString oldid);

	//ģ��json����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
	static bool SetCustomFurTypJsonData(TArray<UVaRestJsonObject*> JsonData, int nType);

	//���ƫ������
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static  bool SetAutoTypeClass(const TArray<int32>& MultiID, const TArray<int32>& RadioID, int nType);
	//�Զ�����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool AutoSetCabinets(int nType);

	//���õ�ˮ������
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetBreakWaterJson(UVaRestJsonObject* JsonList);

	//��ȡ��������
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static FString GetAreaTag();

	//sku����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetSkuData(UVaRestJsonObject* JsonList);
	//////////////////////////////////////////////////////////////////////////
	//�滻����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static TArray<float> ReplaceCabinet(int32 CabinetId, int nSetType, UVaRestJsonObject* pJsonTemplate, FString RoomTag);
	//���õ���߶�(�������� ��λ���ף�
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetHangTop(int nType, float PosZ, FString RoomTag);
	//�滻����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetSplineMesh(FString MxFileName, ESplineTypeBlue SplineType, int nType, FString RoomTag);
	//�滻Ӳװ 0:md5.mx 1:����Id�� 2:ӲװCategoryId eg 726   3:���� �¹� ����id 
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static float SetHardModle(FString MxFileName, int32 CabinetId, int SkuId, int CategoryId, int nType, int nOldSkuId,FString RoomTag);
	//�滻����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetSplineMaterial(FString MxFileName, ESplineTypeBlue SplineType, int nType, FString RoomTag);
	//��ʾ����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetSplineVisble(int SplineType, bool bVisble, int nType, FString RoomTag);
	//ɾ��
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool DeleteSplineType(FString Tag, int nType);

	//ɾ��Actor
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool DeleteActor(FString Tag, int nType, int Actorid);

	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool ExchangeActor(FString oldTag, FString newTag,int nType, int Actorid);

	//ɾ��
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool DeleteSpline(FString Tag);
	//��ʾ����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool LoadPreSaveData(int nType, FString RoomTag);
	//////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetLoftingandSKU(TMap<int32, FString>Sku, TMap<ESplineTypeBlue,FString>Lofting);

	
	//
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static int32 NewCabinetId();
	//////////////////////////////////////////////////////////////////////////
	//����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SaveCabinetMgr(FCabinetAreaMgrSave& Datas);
	//����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool LoadCabinetMgr(const FCabinetAreaMgrSave& Datas);
	//DNA����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool LoadCabinetNDAMgr(const FCabinetAreaMgrSave& Datas);
	//////////////////////////////////////////////////////////////////////////
	//web
	//////////////////////////////////////////////////////////////////////////
	//���ֻ���
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool AutoLayout( FCabinetAreaRaw AreaRaw, const FCabinetAutoSet& SelectData);

	//���ֽ��
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "CabinetSdk")
		static bool LayoutResultBack(UObject* WorldContextObject, const FCabinetLayoutResult& Data);

	//���������
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "CabinetSdk")
		static bool OverlayResultBack(UObject* WorldContextObject, const FCabinetOverlayResult& Data);

	//�޸������Ϸ��¹�ѡ��
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SelecteDwardrobeLayout(int type);
	//�¹��Ƿ�ʹ��
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool WardrobeUseHang(int nUse);

	//�޸ĳ���¯��ˮ��λ��
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool ModifySkuPos(int type, FVector2D NewPos);

	//�޸ĳ���¯��ˮ��λ�÷���
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"),Category = "CabinetSdk")
		static bool ModifySkuPosBack(UObject* WorldContextObject,const FCabinetSkuModel& Data);
	//��ɶ���
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool Complete();		
	//֪ͨʹ����Щ����
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool LayoutUseCabinet(TArray<int32>& Cabinets,TArray<int32>& Skus);
	//�������
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool DownComplete();
	//���ֵ���ģ�������滻
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static void replaceLayoutTemplate(int macterId,int mcategoryId);
};