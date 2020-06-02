// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AataStructure/ExpansionsDrawingStructure.h"
//#include "../HouseFunction/HouseFunctionLibrary.h"
/**
 * 
 */
#define ERRORRANGE 0.01f
#define ERRORBAND(num,num1)FMath::Abs((num - num1))<ERRORRANGE
#define ERRORBANDEX(num,num1,offset)FMath::Abs((num - num1))<offset
struct  FLine
{
	FLine(FVector2D s = FVector2D(0, 0), FVector2D e = FVector2D(0, 0))
	{
		Start = s;
		End = e;
	}
	FLine(FVector s, FVector e)
	{
		Start.Set(s.X, s.Y);
		End.Set(e.X, e.Y);
	}


	FVector2D Start;
	FVector2D End;

	inline FVector2D GetNorm()
	{
		return (Start - End).GetSafeNormal();
	}
	inline void GetCossPos(float Len, FVector2D&CossPos, FVector2D& nor)
	{
		auto nor1 = GetNorm();
		auto cnor = FVector::CrossProduct(FVector(nor1.X, nor1.Y, 0), FVector(0, 0, 1));
		nor.Set(cnor.X, cnor.Y);
		auto Pos = FVector2D::Distance(Start, End) / 2.f * nor1 + End;
		CossPos = Pos + nor * Len;
	}

	inline FVector2D GetPointExLength(bool bIs, float len)
	{
		FVector2D nor = bIs ? GetNorm() : -GetNorm();
		FVector2D TempPoint;
		if (bIs)
			TempPoint = nor * len + Start;
		else
			TempPoint = nor * len + End;
		return TempPoint;
	}

	inline void GetCossPos(float Len, FVector2D& p1, FVector2D& p2, FVector2D& nor)
	{
		auto nor1 = GetNorm();
		auto cnor = FVector::CrossProduct(FVector(nor1.X, nor1.Y, 0), FVector(0, 0, 1));
		FVector2D cnor_l(cnor.X, cnor.Y);
		nor = cnor_l;
		auto Cpos = FVector2D::Distance(Start, End) / 2.f * nor1 + End;
		p1 = cnor_l * Len + Cpos;
		p2 = -cnor_l * Len + Cpos;
	}

	inline FLine LengthEX()
	{
		FVector2D Dir = (GetNorm() * 1000.f);
		return FLine(Dir + Start, -Dir + End);
	}
	inline float GetLineLength()
	{
		return (Start - End).Size();
	}

	void Set(FVector2D s, FVector2D e)
	{
		Start = s;
		End = e;
	}
	bool operator == (FLine& that)
	{
		return Start == that.Start && End == that.End;
	}

	FLine ShrinkLine(float Length)
	{
		FVector2D Dir = GetNorm();
		FVector2D ShrinkEnd = Dir * Length + End;
		FVector2D ShrinkStart = -Dir * Length + Start;
		return FLine(ShrinkStart, ShrinkEnd);
	}

	FVector2D GetMiddlePoint()
	{
		FVector2D Dir = GetNorm();
		float len = GetLineLength() / 2.f;
		return  Dir * len + End;
	}

	void SetLineLen(float Length)
	{
		FVector2D MiddlePos = GetMiddlePoint();
		FVector2D nor = GetNorm();
		float midlen = Length / 2.f;
		Start = nor * midlen + MiddlePos;
		End = -nor * midlen + MiddlePos;
	}
};

struct Atbestline
{
	Atbestline()
	{
		left = right = slef = -1;
	}
	int left, right, slef;
	bool leftbIs, rightbIs;
	int Type;
	ExpansionAuadrangle data;
};


class CUSTOMLAYOUT_API LayoutMath
{
public:
	static LayoutMath* GetInstance();
	~LayoutMath();

private:

	LayoutMath();
	static LayoutMath* Instance;

public:
	TArray<TArray<FVector>> TopLineBoardCount(const TArray<ExpansionBasic>& TempList, const TArray <FExpansionLine>& WallList, TArray<TArray<int32>>& Actorids);
	TArray<TArray<FVector>> TopLineBoardCountEx(const TArray<ExpansionBasic>& TempList, const TArray <FExpansionLine>& WallList, TArray<TArray<int32>>& Actorids);
	void TableboardCount(const TArray<ExpansionBasic>& TempList,  TArray <FExpansionCabnetPillars>& Fluelist, TArray<TArray<FVector>>& ForwardPoints, TArray<TArray<FVector>>& LastPoints, TArray<TArray<FVector>>& ForwardDir);


public:

	//旋转
	 FVector GetPosToRotator(FRotator rot, FVector Pos);


	 FVector GetMiddPoint(FVector pos1, FVector pos2);

	 void GetABSvector(FVector& data);

	 bool IsPointAsLine(FVector2D& point, FLine& line);
	 bool IsPointToLine(FVector2D& point, FLine& line);
	 bool IsPointToLine(FVector2D& point, FLine& line, float offset, FVector& outpos);

	 float GetAbs(float b);



private:



	//分类
	 TMap<FVector, TArray<ExpansionBasic>> GetClassifyData(const TArray<ExpansionBasic>& TempList);

	 //拆分点位
	 TArray<ExpansionAuadrangle> GetAuadrangle(TMap<FVector, TArray<ExpansionBasic>>& Data);

	 //计算拆分点位
	 ExpansionAuadrangle GetAuadranglePoint(FVector size, FVector dir, FVector Pos, bool bIs);
	 //
	 TArray<TArray<FVector>> GetForwardPoint(TMap<int, Atbestline>& contentpoints, TArray<TArray<int>>&AuadrangleIndex, TArray<TArray<FVector>>&OutForwardDir);
	 TArray<FVector> GetTopPointList(TMap<int, Atbestline>& contentpoints, TArray<FVector>&forwarddir, TArray<int>& clear, Atbestline temp, int type,bool bIs);

	 void GetLastPoint(const TArray<ExpansionAuadrangle>& AuadrangleList, TArray<TArray<int>>& AuadrangleIndex, const TMap<int, Atbestline>& componet,  TArray<TArray<FVector>>&Forwaordpointlist,  TArray <FExpansionCabnetPillars>& Fluelist, TArray<TArray<FVector>>& OutLastPoint);
	 TMap<int, Atbestline>SplitAuadrangle(TArray<ExpansionAuadrangle>& data);


	 int GetOtherPointType(FVector2D pos, FLine line);

	 bool IsGuanlian(ExpansionAuadrangle temp1, ExpansionAuadrangle temp2);

	 int IsInsertPoint(FLine line1, FLine line2, ExpansionAuadrangle& data1, ExpansionAuadrangle& data2);

	 

	// void RemovedBreakwaterPoint(TArray<TArray<FVector>>& PointList, const TArray <FExpansionLine>& WallList);
public:

	TArray<TArray<FVector>> RemovedLastBreakwaterPoint(const TArray<TArray<FVector>>& ForwardList, const TArray<TArray<FVector>>& LastList, const TArray <FExpansionLine>& WallList);
	void RemovedPoint(TArray<TArray<FVector>>& PointList, const TArray <FExpansionLine>& WallList);
	void RemovedPointfloor(TArray<TArray<FVector>>& PointList, const TArray <FExpansionLine>& WallList);
	void MoveForwardPoints(TArray<TArray<FVector>>& forwardpoints, TArray<TArray<FVector>>& lastpoints, float length);

	bool PillarsCount(TArray <FExpansionCabnetPillars>& Fluelist, FLine& line, TArray<FVector>& OutPointlist, bool& bIsStartClear, bool& bIsEndClear);

	FVector GetDir(FVector other, FVector pos1, FVector pos2, TArray<FVector2D>& areas);

	void  GetSubPoint(FVector2D Pos, FLine line2, FVector2D& OutPos);

	bool ChekColseInterval(const FVector2D& point, const TArray<FVector2D>& List);

	bool LineIntersect(const FLine& line1, const FLine& line2);

	float Min(float a, float b);

	float Max(float a, float b);

	////bool IsPointAsLine(FVector2D& point, FLine& line);

	TArray<FVector2D>FVector3DtoFvector2D(const TArray<FVector>& List);
};
