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
	FVector2D	mSize;//³ß´çµÄÒ»°ë
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
	eSliptWallChimeyCabinet = 128,//Ôî¾ß¹ñ
	eSliptWallChimney = 256,//ÑÌµÀ
	eSliptWallCabinet = 512,
	eSliptWallNull = 1024,//ÌÞ³ýµÄÇ½
	eSliptWallSelectHang = 2048,//µõ¹ñÑ¡ÔñÇøÓò
};

enum eAutoSetType
{
	eAutoSetNull = -1,
	eAuotSetCook,		//³÷¹ñ
	eAutoSetFloor=1,	//³÷¹ñµØ¹ñ
	eAutoSetHang=2,		//³÷¹ñµõ¹ñ
	eAutoSetToilet=3,    //ÎÀÔ¡¹ñ
	eAutoSetTatami=4,    //é½é½Ã×
	eAutoSetWardrobe=5,  //ÒÂ¹ñ
	eAutoSetSideboardCabinet,//²Í±ß¹ñ
	eAutoSetTelevision=7, //µçÊÓ¹ñ
	eAutoSetBookcase=8,//Êé¹ñ
	eAutoSetPorchArk=9,//Ðþ¹Ø¹ñ
	eAutoSetWashCabinet=10,//Ï´ÒÂ¹ñ
	eAutoSetStoreCabinet=11,//´¢Îï¹ñ
	eAutoSetBedCabinet=12,//×éºÏ´²
	eAutoSetDesk=13,			//Êé×À
	eAutoSetHangBedCabinet=14,//µõ¹ñ
	eAutoSetCoatroom=15,//ÒÂÃ±¼ä
	eAutoSetBayWindowCabinet,//16£¬Ð¡Æ·£¨Æ®´°¹ñ£©
	eAutoSetTakeInCabinet,//17£¬Ð¡Æ·£¨ÊÕÄÉ¹ñ£©
	eAutoSetLaminate,//18£¬Ð¡Æ·£¨²ã°å£©
	eAutoSetBedside,//19£¬Ð¡Æ·£¨´²Í·¹ñ£©
	eAutoSetWardorbeHang,//20ÒÂ¹ñµõ¹ñ
	eAutoSetToiletHang,    //21ÎÀÔ¡¹ñµõ¹ñ
	eAutoSetCoatroomHang,    //22ÒÂÃ±¼äµõ¹ñ
	eAutoSetNum,
};
eAutoSetType CategoryId2SetType(int32 categoryId, bool bFloor = false);
int32 SetType2CategoryId(eAutoSetType eSetType);
int GetTurnSize(int TemplateId);
bool IsTurnCabinet(int TemplateId);

enum eSplineType
{
	eSplineTypeNull = -1,
	eSplineTableBack,//ºóµµ
	eSplineFloorBottom,//½ÅÏß
	eSplineHangTop,//¶¥Ïß
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
	int32		mTeplateId;	//Ë®²Û£¬Â¯Ôî
	int32		mInstanceId;//
	FString		categoryName;// " : "µØ¹ñ"£¬//ÀàÄ¿
	FVector2D	mSize;
	int32		mTurnBSize;	//
	int32		mCabinetHeight;	//¹ñÌå¸ß¶È
	FVector2D	mSizeMin;
	FVector2D	mSizeMax;
	bool		mbSpline[eSplineTypeNum];
	int32		mExtern;//1:À­Àº
	FString		mBoyMd5;
	FVector2D	mTableSize;
	FVector		mMeshScale;
	//
	int32		mHeight;//·ÅÖÃ¸ß¶È
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
		eFillFix	= 3,//ÉèÖÃºÃÁË
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
	TArray<FVector2D>	mHoles[3];//¶´
	float				mAnchor[3];//Ãªµã
	float	LoadSku();
};