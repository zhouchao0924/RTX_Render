#pragma once
#include "CoreMinimal.h"

#define CHECK_ERROR( __condition__ ) { if(!(__condition__)){ return 0;} }
#define CHECK_OK( __condition__ ) { auto ret = (__condition__); if(ret){ return ret;} }
#define CHECK_RET( __condition__,Ret ) { if(!(__condition__)){ return Ret;} }
#define SINGLE(T)  static T& Instance(){ static T ms_Instance; return ms_Instance; }

#ifndef SAFE_DELETE
#define SAFE_DELETE(p){ if(p) delete p; p = NULL;}
#endif

template<typename T>
inline void TinySwap(T& A, T& B) { T C = A; A = B; B = C; }

#define TinyMax(A, B) ((A) > (B) ? (A) : (B))
#define TinyMin(A, B) ((A) < (B) ? (A) : (B))

inline int32 Float2Int(float fValue) {return INT32(fValue + (fValue > 0 ? .5f : -.5f));}

inline int32 Float2Int2(float fValue) { return Float2Int(fValue/10.0f)*10; }

int64 Pos2Id(float x, float y);
int64 Pos2Id(FVector2D Pos);
FString IdId2Str(INT64 IdStart, INT64 IdEnd);
FVector2D CeilVector(FVector2D Size);

float AreaPoly(TArray<FVector2D> &poly);
float Point2LineSeg(const FVector2D& point_A, const FVector2D& point_B, const FVector2D& point_P, float &fDot, FVector2D &point_C);
float Point2LineSegVer(const FVector2D& point_A, const FVector2D& point_B, const FVector2D& point_P, float &fDot, FVector2D &point_C);

bool IsPointOnLine(FVector2D point_A, FVector2D point_B, FVector2D point_P);

FVector2D Local2World(FVector2D& Anchor, FVector2D& Dir, FVector2D& Scale, FVector2D& LocalPos);

FVector GetResSize(FString ResID);

bool LoadStrim(FString sFileName, TArray<FVector2D>& OutLine,int IgnoreXYZ, char* sMeshName,float& Z);

inline FVector2D Rote90(FVector2D Dir) { return FVector2D(-Dir.Y, Dir.X); }
inline FVector2D Rote270(FVector2D Dir) { return FVector2D(Dir.Y, -Dir.X); }
inline float Dir2Rotate(FVector2D Dir){ return atan2(Dir.Y,Dir.X) * 180 / PI;  }
bool GetMeshSize(FString ResID, char* sMeshName, FVector& Size, FVector& Pos);
struct TPath2d
{
	TArray<FVector2D>	mOutLine;
	TArray<TPath2d>		mHoles;
};
struct TCorner2d: public FVector2D
{
	int32	Id;
	int	mState;
}; 
struct TLine2d
{
	TLine2d()
	{
		Id = -1;
		mState = 0;
	}
	int32	Id;
	int	mState;
	FVector2D mStart;
	FVector2D mEnd;
	FVector2D GetCentry() { return (mStart + mEnd)*0.5f; }
	float Len() { return FVector2D::Distance(mStart, mEnd); }
};


struct TRectBase
{
	TRectBase() :mScale(1, 1), mPos(0, 0), mSize(1, 1), mDir(0, 1),mPosZ(0), mHeight(0), mUserType(0){};
	TRectBase(FVector2D Pos, FVector2D Size, FVector2D Dir = FVector2D(1, 0), FVector2D Scale = FVector2D(1, 1), FVector2D Anchor = FVector2D(0, 0));
	FVector2D	mScale;
	FVector2D	mPos;
	FVector2D	mSize;//�ߴ��һ��
	FVector2D	mDir;
	float		mHeight;
	float		mPosZ;
	int32		mUserType;

	bool Collision(TLine2d& Line2d, float fOffset = 0.0f);
};

enum eCornerType
{
	eCornerNull = -1,
	eCorner0,
	eCorner90,
	eCorner180,
	eCorner270,
	eCorner360,
};

enum eWallSliptType
{
	eSliptWallSolid = 1,
	eSliptWall = 2,
	eSliptWallSelect = 4,
	eSliptWallDoor = 8,
	eSliptWallWindow = 16,
	eSliptWallPillar = 32,
	eSliptWallObject = 64,
	eSliptWallChimeyCabinet = 128,//��߹�
	eSliptWallChimney = 256,//�̵�
	eSliptWallCabinet = 512,
	eSliptWallNull = 1024,//�޳���ǽ
	eSliptWallSelectHang = 2048,//����ѡ������
};

enum eAutoSetType
{
	eAutoSetNull = -1,
	eAuotSetCook,		//����
	eAutoSetFloor=1,	//����ع�
	eAutoSetHang=2,		//�������
	eAutoSetToilet=3,    //��ԡ��
	eAutoSetTatami=4,    //����
	eAutoSetWardrobe=5,  //�¹�
	eAutoSetSideboardCabinet,//�ͱ߹�
	eAutoSetTelevision=7, //���ӹ�
	eAutoSetBookcase=8,//���
	eAutoSetPorchArk=9,//���ع�
	eAutoSetWashCabinet=10,//ϴ�¹�
	eAutoSetStoreCabinet=11,//�����
	eAutoSetBedCabinet=12,//��ϴ�
	eAutoSetDesk=13,			//����
	eAutoSetHangBedCabinet=14,//����
	eAutoSetCoatroom=15,//��ñ��
	eAutoSetBayWindowCabinet,//16��СƷ��Ʈ����
	eAutoSetTakeInCabinet,//17��СƷ�����ɹ�
	eAutoSetLaminate,//18��СƷ����壩
	eAutoSetBedside,//19��СƷ����ͷ��
	eAutoSetWardorbeHang,//20�¹����
	eAutoSetToiletHang,    //21��ԡ�����
	eAutoSetCoatroomHang,    //22��ñ�����
	eAutoSetNum,
};
eAutoSetType CategoryId2SetType(int32 categoryId, bool bFloor = false);
int32 SetType2CategoryId(eAutoSetType eSetType);
int GetTurnSize(int TemplateId);
bool IsTurnCabinet(int TemplateId);

enum eSplineType
{
	eSplineTypeNull = -1,
	eSplineTableBack,//��
	eSplineFloorBottom,//����
	eSplineHangTop,//����
	eSplineFront,//
	eSplineTypeNum,
};

enum eCabinetAuxObjectType
{
	eCabinetAuxObjecNull = -1,
	eCabinetAuxObjecSink = 0,
	eCabinetAuxObjecNum,
};

struct TSkuData
{
	int32	mSkuId;
	int32	mCategoryId;
	FString mName;
	FString mMxFileMD5;
	FVector	mSize;
};
//////////////////////////////////////////////////////////////////////////
struct FCabinetRes
{
	FCabinetRes()
	{
		mTeplateId = -1;
		mHeight = 0;
		mTurnBSize = 0;
		mExtern = 0;
		mSize = FVector2D(800, 600);
		mSizeMin = FVector2D(100, 600);
		mSizeMax = FVector2D(1000, 600);
		mMeshScale = FVector(1, 1, 1);
		for ( int i=0; i<eSplineTypeNum; ++i )
		{
			mbSpline[i] = false;
		}
	}
	FCabinetRes( int nLen )
	{
		mTeplateId = -1;
		mHeight = 0;
		mTurnBSize = 0;
		mExtern = 0;
		mSize = FVector2D(nLen, 600);
		mSizeMin = FVector2D(nLen, 600);
		mSizeMax = FVector2D(nLen, 600);
		for (int i = 0; i < eSplineTypeNum; ++i)
		{
			mbSpline[i] = false;
		}
	}
	int32 GetTurnFill()
	{
		return mSize.Y;
	}
	float GetMinScale() { return mSizeMin.X/mSize.X ; }
	float GetMaxScale() { return mSizeMax.X/mSize.X ; }
	/////////////////////////////////////////////
	int32		mTeplateId;	//ˮ�ۣ�¯��
	int32		mInstanceId;//
	FString		categoryName;// " : "�ع�"��//��Ŀ
	FVector2D	mSize;
	int32		mTurnBSize;	//
	int32		mCabinetHeight;	//����߶�
	FVector2D	mSizeMin;
	FVector2D	mSizeMax;
	bool		mbSpline[eSplineTypeNum];
	int32		mExtern;//1:����
	FString		mBoyMd5;
	FVector2D	mTableSize;
	FVector		mMeshScale;
	//
	int32		mHeight;//���ø߶�
	TArray<FString>		mHardSkuMd5;
	TArray<int32>		mCategoryIds;
	TArray<TSkuData>	mSkuDatas;
	bool InitSinkSku(FVector2D mScale);
};

struct FCabinetSet
{
	enum eAlignType
	{
		eAlignTail = -1,
		eAlignCentry = 0,
		eAlignHead = 1,
	};
	enum eFillType
	{
		eFillDefault = 0,
		eFillMax	= 1,
		eFillFull	= 2,
		eFillFix	= 3,//���ú���
	};
	FCabinetSet()
	{
		mId = -1;
		mFillType = eFillDefault;
		mAlign = eAlignHead;
		mScale = FVector2D(1, 1);
		mDir = FVector2D(1, 0);
		mHeight = 0;
		mScaleZ = 1.0f;
	}
	FVector2D	GetSize() { return mRes.mSize* mScale; }
	int32		mId;
	eAlignType	  mAlign;
	eFillType	  mFillType;
	eAutoSetType  mSetType;
	FCabinetRes	  mRes;
	//
	int32     mPosLocal;
	FVector2D mPos;
	FVector2D mScale;
	FVector2D mDir;
	int32	  mHeight;
	float	  mScaleZ;
};
struct TSkuInstance
{
	TSkuData	mSku;
	FVector2D	mPos;
	FVector2D	mDir;
	int32		mLeftCabinetId;
	int32		mRightCabinetId;
	int32		mHeight;
	//
	TArray<FVector2D>	mHoles[3];//��
	float				mAnchor[3];//ê��
	float	LoadSku();
};