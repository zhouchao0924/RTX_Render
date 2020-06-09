#pragma once
#include "CoreMinimal.h"
#include "base/common.h"
#include "CabinetSave.h"
/*����ȫ��������
*/
class FCabinetGlobal
{
public:
	SINGLE(FCabinetGlobal);
	FCabinetGlobal();
	float GetTableHeight();
	float HangTopHeight() {return mHangHeight + mHangSetHeight; }
	TSkuData* GetSku(FVector2D CabinetSize,int32 CategoryId, int32 InstanceId = -1);
	bool ClearSkuDatas();
	FString GetTableSku(int32 nType );
	TSkuData* GetSkuData(int32 InstanceId);
public:
	TMap<int32,TSkuData*>	mSkuDatas;
	FString mCabinetAuxMx[eCabinetAuxObjecNum];
	FString mTableFrontMx;
	//��λ����
	float	mFloorDepth; //�ع����
	float	mFloorHeight; //�ع�߶�
	float	mFloorSetHeight;//�ع���ø߶�

	float	mHangSetHeight;	//������ø߶�
	float	mHangHeight;	//����߶�
	float	mHangDepth;	//�������

	float	mTableThickness;//̨����
	float	mTableFrontElapse;//̨������
	float	mSkirtboardElapse;//�߽Ű�����
};
#define gCabinetGlobal FCabinetGlobal::Instance()
