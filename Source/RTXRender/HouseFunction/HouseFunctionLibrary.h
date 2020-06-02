// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Math/LayoutMath.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "../Building/FeatureWalls.h"
#include "HomeLayout/HouseArchitect/PlaneHousePicture.h"
#include "Cabinet/CabinetStruct.h"
#include "CustomDataStructure.h"
#include "HouseCustomActor.h"
#include "HouseFunctionLibrary.generated.h"

/**
 *
 */
#define ERRORRANGE 0.01f
#define ERRORBAND(num,num1)FMath::Abs((num - num1))<ERRORRANGE
#define ERRORBANDEX(num,num1,offset)FMath::Abs((num - num1))<offset
//struct  FLine
//{
//	FLine(FVector2D s = FVector2D(0, 0), FVector2D e = FVector2D(0, 0))
//	{
//		Start = s;
//		End = e;
//	}
//
//	FVector2D Start;
//	FVector2D End;
//
//	inline FVector2D GetNorm()
//	{
//		return (Start - End).GetSafeNormal();
//	}
//	inline void GetCossPos(float Len, FVector2D&CossPos, FVector2D& nor)
//	{
//		auto nor1 = GetNorm();
//		auto cnor = FVector::CrossProduct(FVector(nor1.X, nor1.Y, 0), FVector(0, 0, 1));
//		nor.Set(cnor.X, cnor.Y);
//		auto Pos = FVector2D::Distance(Start, End) / 2.f * nor1 + End;
//		CossPos = Pos + nor * Len;
//	}
//
//	inline FVector2D GetPointExLength(bool bIs, float len)
//	{
//		FVector2D nor = bIs ? GetNorm() : -GetNorm();
//		FVector2D TempPoint;
//		if (bIs)
//			TempPoint = nor * len + Start;
//		else
//			TempPoint = nor * len + End;
//		return TempPoint;
//	}
//
//	inline void GetCossPos(float Len, FVector2D& p1, FVector2D& p2, FVector2D& nor)
//	{
//		auto nor1 = GetNorm();
//		auto cnor = FVector::CrossProduct(FVector(nor1.X, nor1.Y, 0), FVector(0, 0, 1));
//		FVector2D cnor_l(cnor.X, cnor.Y);
//		nor = cnor_l;
//		auto Cpos = FVector2D::Distance(Start, End) / 2.f * nor1 + End;
//		p1 = cnor_l * Len + Cpos;
//		p2 = -cnor_l * Len + Cpos;
//	}
//
//	inline FLine LengthEX()
//	{
//		FVector2D Dir = (GetNorm() * 1000.f);
//		return FLine(Dir + Start, -Dir + End);
//	}
//	inline float GetLineLength()
//	{
//		return (Start - End).Size();
//	}
//
//	void Set(FVector2D s, FVector2D e)
//	{
//		Start = s;
//		End = e;
//	}
//	bool operator == (FLine& that)
//	{
//		return Start == that.Start && End == that.End;
//	}
//
//	FLine ShrinkLine(float Length)
//	{
//		FVector2D Dir = GetNorm();
//		FVector2D ShrinkEnd = Dir * Length + End;
//		FVector2D ShrinkStart = -Dir * Length + Start;
//		return FLine(ShrinkStart, ShrinkEnd);
//	}
//
//	FVector2D GetMiddlePoint()
//	{
//		FVector2D Dir = GetNorm();
//		float len = GetLineLength() / 2.f;
//		return  Dir * len + End;
//	}
//
//	void SetLineLen(float Length)
//	{
//		FVector2D MiddlePos = GetMiddlePoint();
//		FVector2D nor = GetNorm();
//		float midlen = Length / 2.f;
//		Start = nor * midlen + MiddlePos;
//		End = -nor * midlen + MiddlePos;
//	}
//
//};

USTRUCT(BlueprintType)
struct  FWall
{
	GENERATED_USTRUCT_BODY()

		FWall()
	{}
	FWall(FVector2D s, FVector2D e)
	{
		start = s;
		end = e;
	}

	UPROPERTY(BlueprintReadWrite)
		FVector2D start;

	UPROPERTY(BlueprintReadWrite)
		FVector2D end;

};

USTRUCT(BlueprintType)
struct  FCustomWallData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FVector2D start;

	UPROPERTY(BlueprintReadWrite)
		FVector2D end;

	UPROPERTY(BlueprintReadWrite)
		FVector2D nor;

	UPROPERTY(BlueprintReadWrite)
		bool bIs;

};

UCLASS()
class AJDR_API UHouseFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool GetChangeImageData(UObject* WorldContextObject, TArray<FWall>& OutData1, TArray<FWall>& OutData2);

	//拆改图合成
	UFUNCTION(BlueprintCallable)
		static void	ChangeIamgeCompound(FString path, int32 H, int32 W, int32 OffsetH, int32 OffsetW, UTexture2D* image);

	UFUNCTION(BlueprintCallable)
		static void GetCustomWallData(int32 Index, UFeatureWall* Feature, FCustomWallData& outdata);


	UFUNCTION(BlueprintCallable)
		static void TestCustomsavedata(FCustomAllSaveData data);

	UFUNCTION(BlueprintCallable)
		static void GetSubPointCustom(FWall line, FVector2D pos, FVector2D&OutPos);

	UFUNCTION(BlueprintCallable)
		static bool IsPointAsLineCustom(FVector2D point, FWall line);

	static FTransform StringToTransform(FString str);

	//获取区域内向法线
	UFUNCTION(BlueprintCallable)
		static void GetAreaNor(TArray<FAreaInnerWall> Data, TMap<int32, FVector2D>& outData);

	UFUNCTION(BlueprintCallable)
		static void GetAreaNorEx(TArray<FAreaInnerWall> Data, TArray<FVector2D>& outData);

	UFUNCTION(BlueprintCallable)
		static int32 GetTypeID(int32 CategoryID, int32 sleftypeid = -1);

	//判断是否时台面
	UFUNCTION(BlueprintCallable)
		static bool IsTableBoard(int32 CategoryID);

	//判断是否时角线
	UFUNCTION(BlueprintCallable)
		static bool IsAngularLine(int32 CategoryID);

	UFUNCTION(BlueprintCallable)
		static FVector GetRotationPos(FRotator rot, FVector Pos);

	UFUNCTION(BlueprintCallable)
		static FVector ABSVector(FVector Pos);

	//判断是否是顶线
	UFUNCTION(BlueprintCallable)
		static bool IsCeiline(int32 CategoryID);

	//判断是否是台面和角线1为台面2为角线3为顶线
	UFUNCTION(BlueprintCallable)
		static int32 IsTableBoardAndAngularLine(int32 CategoryID);

	UFUNCTION(BlueprintCallable)
		static bool GetLinePoint(const float& Length, const FVector2D& objStart, const FVector2D& objEnd, const FVector2D& CurrentStart, const FVector2D& CurrentEnd, FVector2D& OutStart, FVector2D& OutEnd);

	UFUNCTION(BlueprintCallable)
		static bool IsMovePoint(const FVector2D& LastMovePoint, const FVector2D& CurrentMovePoint, const FVector2D& objStart, const FVector2D& objEnd, const FVector2D& CurrentStart, const FVector2D& CurrentEnd, FVector2D& OutStart, FVector2D& OutEnd);

	//打组
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static void CustomPlayGroup(UObject* WorldContextObject, TArray<AHouseCustomActor*> HousecustomActor, EObjectTypeQuery type, EDrawDebugTrace::Type drawtype);
		static void PlayGroupTraceBox(UObject* WorldContextObject, AHouseCustomActor* HousecustomActor, TArray<AHouseCustomActor*>& Templist,FString& palyid, EObjectTypeQuery type, EDrawDebugTrace::Type drawtype);


		

	//判断点是否在区域内
	UFUNCTION(BlueprintCallable)
		static bool ChekColseInterval(const FVector2D& point, const TArray<FVector2D>& List);

	static  bool LineIntersect(const FLine& line1, const FLine& line2);
	//判断群点是否在区域内
	UFUNCTION(BlueprintCallable)
		static bool ChekColseIntervalEx(const TArray<FVector2D>& points, const TArray<FVector2D>& List);

	static  void  GetSubPoint(FVector2D Pos, FLine line2, FVector2D& OutPos);

	static bool IsPointAsLine(FVector2D& point, FLine& line);

	static float Min(float a, float b);

	static float Max(float a, float b);

	static FLine GetMoveLine(FLine MoveLine, FLine ObjLine, FVector2D nor1);

	static void GetOffsetData(FVector2D point, FLine line, float& Len, FVector2D& nor);
	//判断点是否在矩形
	static bool IsRect(FVector2D Pos1, FVector2D Size, FVector2D pos2);

	//修复偏差
	UFUNCTION(BlueprintCallable)
	static FCabinetAreaRaw RepairtheDeviation(FCabinetAreaRaw Walldata);
	static void SortLinedata(FCabinetAutoSet& SelectData);

	


private:

	static void ChangeDeviation(FCabinetAreaRaw& Walldata, float SDiff, float EDiff, bool bIs);
};
