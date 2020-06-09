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
	
	//设置墙
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetAutoSetWalls(const TArray<FAreaInnerWall>& outWallLines, const TArray<FCustomizationBoolean>& outBooleanData,int nType, FString Tag);
	//选中墙
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static 	bool SetAutoSlecctWalls(const TArray<int32>& WallsID, int nType);	
	//选中墙
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static 	bool SetSlecctWalls(const TArray<FLine2dSelect>& SelectWalls, int nType);
	//替换areatage
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool ChangAreaTag(FString newid, FString oldid);

	//模板json数据
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
	static bool SetCustomFurTypJsonData(TArray<UVaRestJsonObject*> JsonData, int nType);

	//风格偏好数据
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static  bool SetAutoTypeClass(const TArray<int32>& MultiID, const TArray<int32>& RadioID, int nType);
	//自动布局
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool AutoSetCabinets(int nType);

	//设置挡水板数据
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetBreakWaterJson(UVaRestJsonObject* JsonList);

	//获取区域数据
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static FString GetAreaTag();

	//sku数据
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetSkuData(UVaRestJsonObject* JsonList);
	//////////////////////////////////////////////////////////////////////////
	//替换柜子
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static TArray<float> ReplaceCabinet(int32 CabinetId, int nSetType, UVaRestJsonObject* pJsonTemplate, FString RoomTag);
	//设置吊柜高度(世界坐标 单位毫米）
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetHangTop(int nType, float PosZ, FString RoomTag);
	//替换样条
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetSplineMesh(FString MxFileName, ESplineTypeBlue SplineType, int nType, FString RoomTag);
	//替换硬装 0:md5.mx 1:柜子Id， 2:硬装CategoryId eg 726   3:橱柜 衣柜 设置id 
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static float SetHardModle(FString MxFileName, int32 CabinetId, int SkuId, int CategoryId, int nType, int nOldSkuId,FString RoomTag);
	//替换材质
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetSplineMaterial(FString MxFileName, ESplineTypeBlue SplineType, int nType, FString RoomTag);
	//显示隐藏
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetSplineVisble(int SplineType, bool bVisble, int nType, FString RoomTag);
	//删除
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool DeleteSplineType(FString Tag, int nType);

	//删除Actor
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool DeleteActor(FString Tag, int nType, int Actorid);

	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool ExchangeActor(FString oldTag, FString newTag,int nType, int Actorid);

	//删除
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool DeleteSpline(FString Tag);
	//显示隐藏
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool LoadPreSaveData(int nType, FString RoomTag);
	//////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SetLoftingandSKU(TMap<int32, FString>Sku, TMap<ESplineTypeBlue,FString>Lofting);

	
	//
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static int32 NewCabinetId();
	//////////////////////////////////////////////////////////////////////////
	//保存
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SaveCabinetMgr(FCabinetAreaMgrSave& Datas);
	//加载
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool LoadCabinetMgr(const FCabinetAreaMgrSave& Datas);
	//DNA加载
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool LoadCabinetNDAMgr(const FCabinetAreaMgrSave& Datas);
	//////////////////////////////////////////////////////////////////////////
	//web
	//////////////////////////////////////////////////////////////////////////
	//布局或检测
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool AutoLayout( FCabinetAreaRaw AreaRaw, const FCabinetAutoSet& SelectData);

	//布局结果
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "CabinetSdk")
		static bool LayoutResultBack(UObject* WorldContextObject, const FCabinetLayoutResult& Data);

	//检测结果返回
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "CabinetSdk")
		static bool OverlayResultBack(UObject* WorldContextObject, const FCabinetOverlayResult& Data);

	//修改榻榻米上方衣柜被选中
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool SelecteDwardrobeLayout(int type);
	//衣柜是否使用
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool WardrobeUseHang(int nUse);

	//修改橱柜炉灶水槽位置
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool ModifySkuPos(int type, FVector2D NewPos);

	//修改橱柜炉灶水槽位置返回
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"),Category = "CabinetSdk")
		static bool ModifySkuPosBack(UObject* WorldContextObject,const FCabinetSkuModel& Data);
	//完成定制
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool Complete();		
	//通知使用哪些柜子
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool LayoutUseCabinet(TArray<int32>& Cabinets,TArray<int32>& Skus);
	//下载完成
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static bool DownComplete();
	//布局单个模板类型替换
	UFUNCTION(BlueprintCallable, Category = "CabinetSdk")
		static void replaceLayoutTemplate(int macterId,int mcategoryId);
};