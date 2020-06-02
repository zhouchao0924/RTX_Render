#pragma once
#include "CoreMinimal.h"
#include "base/common.h"
#include "CabinetShell.h"
#include "Actor/TableActor.h"
#include "Actor/SplineActor.h"
#include "CabinetSave.h"

struct TLine2d;

struct FCabinetSetGroup
{
	FCabinetSetGroup() {}
public:
	FString		mSplinesMesh[eSplineTypeNum];
	FString		mSplinesMaterial[eSplineTypeNum];
	FString		mTableMeterial;

	TArray<TLine2d>	  mSelectWalls;
	//////////////////////////////////////////////////////////////////////////
	TMap<int32, FCabinetShell*> mCabinets;
	//////////////////////////////////////////////////////////////////////////
	UPROPERTY(Transient)
		TArray<ASplineActor*> mSplines[eSplineTypeNum];
	UPROPERTY(Transient)
		TArray<ATableActor*>  mTables;
	//////////////////////////////////////////////////////////////////////////
	eAutoSetType	mSetType;
	class FCabinetArea*	mArea;
	//////////////////////////////////////////////////////////////////////////
	bool Clear();
	bool ClearExtern();
	bool ClearTable();
	bool ClearSpline(eSplineType eType);
	//////////////////////////////////////////////////////////////////////////
	bool GenExtern();
	bool GenTable(eSplineType eType);
	bool AdjustTablePath(TArray<TPath2d>& PolyPaths,float fHegiht, eSplineType eType);
	//////////////////////////////////////////////////////////////////////////
	int GetCabinets(TArray<FCabinetShell*>& Cabinets,eSplineType bFloor = eSplineTypeNull);
	float GetCabinetTopZ(TArray<FCabinetShell*>& Cabinets);

	bool SetSplineVisble(eSplineType SplineType, bool bVisble);
	bool SetSplineMaterial(FString MxFileName, eSplineType eType);
	bool SetSplineMesh(FString MxFileName, eSplineType eType);
	bool SetHangTop(float PosZ);
	float SetHardModle(FString MxFileName, int32 CabinetId, int CategoryId, int SkuId);
	FCabinetShell* GetCabinet(int32 CabinetId);
	bool DeleteCabinet(int32 CabinetId);

	bool GetSaveData(FCabinetSetGroupBlue& data);
	bool LoadSaveData(FCabinetSetGroupBlue& data);
	bool LoadPreSaveData();
	FCabinetSetGroupBlue  mGroupSave;
		
	void FindSelectWall(const TArray<TLine2d> &SelectWalls);
private:
	void AdjustSplineHangTop(TArray<FVector2D> &PathFloorBottom);
	void AdjustSplineHangTopLine(FVector2D &StartPoint, FVector2D &EndPoint, float value);
	bool IfPointOnWall(const FVector2D &Point, const TLine2d &Wall);
};