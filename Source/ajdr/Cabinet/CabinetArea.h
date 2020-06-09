#pragma once
#include "CoreMinimal.h"
#include "base/common.h"
#include "CabinetShell.h"
#include "Actor/TableActor.h"
#include "Actor/SplineActor.h"
#include "CabinetSave.h"
#include "../HomeLayout/DataNodes/NodeTypes.h"
#include "CabinetGroup.h"

class FCabinetArea
{
public:
	FCabinetArea();
	~FCabinetArea();
	bool AddRawWall(int32 wallId, FVector2D Start, FVector2D End, FVector2D VerticalDir);
	bool AddHole(int Id, FVector2D PosStart, FVector2D PosEnd, EOpeningType Type);
	bool GenOutPath();
	bool AdjustSelect(TLine2d& li);

	bool AutoSetCabinets(int nType=0);
	bool ClearCabinets(TArray<int32>& Actorid, bool bClearCook);//重叠区域删除

	TArray<TLine2d>&	GetWalls() { return mOutPaths; }
	TArray< TRectBase>& GetPillar() { return mPillars; }
	TRectBase*			GetChimery();

	bool NewCabinet(FCabinetSet& CabinetSet);

	bool GetFurnitureModel(int nType, TArray<TRectBase>& Rects);

	bool GetSelectWalls(TArray<int32>& RawSelectWalls, TArray<TLine2d>& SelectPaths);

	int GetCabinets(TArray<FCabinetShell*>& Cabinets, eAutoSetType eSetType, eSplineType bFloor= eSplineTypeNull);

	FCabinetShell* GetCabinets(eAutoSetType eSetType, int eType);
	//
	bool SetSplineVisble(int SplineType, bool bVisble, eAutoSetType eType);
	bool SetSplineMaterial(FString MxFileName, eSplineType eType, eAutoSetType eSetType);
	bool SetSplineMesh(FString MxFileName, eSplineType eType, eAutoSetType eSetType);

	bool GetSaveData(FCabinetAreaSave& data);
	bool LoadData(FCabinetAreaSave& data,bool bDNA=false);
	//
	bool CompleteLayout(int nType);
	bool LoadPreSaveSku();
	bool GetCabinet(eAutoSetType eType,TMap<int32, FCabinetShell*>& mCabinets);
public:
	FString	  mName;
	TMap<int, TLine2d> mRawWalls;
	TMap<int, TLine2d> mHoles;
	//
	TMap<int, TLine2d> mRawAdjustWalls;//墙去除墙角的的线段
	TArray<TLine2d>    mOutPaths;//墙的合并线段
	TArray< TRectBase> mPillars;//

	FCabinetSetGroup	mCabinetGroup[eAutoSetNum];
	TSkuInstance		mSkus[2];//0水槽1灶具

	int32 mType;
protected:
	TLine2d* GetWall(int id);
	bool ClearCabinet(eAutoSetType eType);

	FSkuInstanceSave mSkuSink;
	FSkuInstanceSave mSkuChimery;
};

class FCabinetAreaMgr
{
public:
	SINGLE(FCabinetAreaMgr);
	FCabinetAreaMgr();
	FCabinetArea* GetArea(FString Id);
	FCabinetArea* NewArea(FString Id);
	FCabinetArea* GetCurArea() { return mCurArea; }
	bool Clear();

	bool DeleteArea(FString sName);
	bool SaveCabinetMgr(FCabinetAreaMgrSave& Datas);
	bool LoadCabinetMgr(const FCabinetAreaMgrSave& Datas,bool bDNA = false);

	bool ChangAreaTag(FString newid, FString oldid);
protected:
	FCabinetArea* mCurArea;
	TMap<FString, FCabinetArea*> mAreas;
};
#define gCabinetAreaMgr FCabinetAreaMgr::Instance()
