#pragma once
#include "CoreMinimal.h"
#include "HomeLayout/HouseArchitect/PlaneHousePicture.h"
#include "CabinetStruct.generated.h"

USTRUCT(Blueprintable)
struct FCabinetPillar
{
	GENERATED_BODY()
		FCabinetPillar() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32		mType;	//0.�̵���1.���� 2.����550���̵�����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mSize;//�ߴ��һ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mDir;
};

USTRUCT(Blueprintable)
struct FCabinetAreaRaw
{
	GENERATED_BODY()
		FCabinetAreaRaw() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FAreaInnerWall> mWallLines;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCustomizationBoolean> mHoles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetPillar> mPillars;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString mName;
};

USTRUCT(Blueprintable)
struct FCabinetWallLine
{
	GENERATED_BODY()
		FCabinetWallLine() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mStart;//ɫ�鷶Χ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	mEnd;  //ɫ�鷶Χ�յ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32		Id;//web�Զ����Id���ɲ��
};

USTRUCT(Blueprintable)
struct FCabinetAutoSet
{
	GENERATED_BODY()
		FCabinetAutoSet() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32						mCategoryId;//������Ŀid
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetWallLine>	mSelectWalls;//ѡ�������
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32						mActionType; //0������ 1��ƫ�ò���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32						mDepth;	//���(ֻ�����׶��Ʋ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32						mFloor;	//�ع񲼾� 1,2 ���񲼾�
};

USTRUCT(Blueprintable)
struct FCabinetModle
{
	GENERATED_BODY()
		FCabinetModle() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 actorId;		//DR��Ӧ����ƷΨһID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 templateId;	//ģ��ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 categoryId;	//��ĿID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float turnBSize;	//ת�ǹ�̱߳���
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString categoryName;// " : "�ع�"��//��Ŀ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector	modelSize;		 // "" : {��dx��:500, "dy" : 100, "dz" : 1000},//ģ��ԭʼ�ߴ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector point;//	"point" : {"x":0, "y" : 0, "z" : 0},//λ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector	rotate;// "rotate" : {"yAxis":0, "xAxis" : 0, "zAxis" : 0},//��ת
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector	scale;// "scale" : {"x":1, "y" : 1, "z" : 1},//����
};

USTRUCT(Blueprintable)
struct FCabinetSkuModel
{
	GENERATED_BODY()
		FCabinetSkuModel() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 skuid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 type;//0.¯�� 1.ˮ�ۣ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D	point;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rotate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCabinetWallLine	wallline;
};


USTRUCT(Blueprintable)
struct FCabinetTatamiExtern
{
	GENERATED_BODY()
		FCabinetTatamiExtern() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int type;// "type":0 / 1 (0��1�ң�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int selected;//	"selected":0 / 1(0.δѡ�У�1ѡ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D point;//	"point" : {"x":0, "y" : 0},//λ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float rotate;//	"rotate" : 0//��ת
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D size;//	"size" : {��dx��:500, "dy" : 100},//�ߴ�
};

USTRUCT(Blueprintable)
struct FCabinetWalldata
{
	GENERATED_BODY()
		FCabinetWalldata() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 success;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString msg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCabinetWallLine	wallline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetModle>	mCabinets;
};
USTRUCT(Blueprintable)
struct FCabinetLayoutResult
{
	GENERATED_BODY()
		FCabinetLayoutResult() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 success;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString msg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 comtype;// ":0/1/2(0."initLayout" 1.��replaceLayoutTemplate��2.��selectedwardrobelayout�� )
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetModle>	mCabinets;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< FCabinetSkuModel> mCookExtern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< FCabinetTatamiExtern> mTatamiExtern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetWalldata> mFailDatas;
};




USTRUCT(Blueprintable)
struct FCabinetOverlayResult
{
	GENERATED_BODY()
		FCabinetOverlayResult() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FCabinetWalldata> mFailDatas;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString msg;
};