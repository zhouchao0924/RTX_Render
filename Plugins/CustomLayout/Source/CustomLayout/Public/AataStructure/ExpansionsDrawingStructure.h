// Copyright? 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "Math/LayoutMath.h"
#include "ExpansionsDrawingStructure.generated.h"


struct  ExpansionBasic
{


	ExpansionBasic(FVector loc, FVector extent, FVector dir,int32 id)
	{
		Pos = loc;
		Size = extent;
		Dir = dir;
		ID = id;
	}
	ExpansionBasic()
	{}

	FVector Pos;
	FVector Size;
	FVector Dir;
	int32 ID;
};





struct ExpansionAuadrangle
{
	FVector LastPos1, LastPos2;
	FVector ForwordPos1, ForwordPos2;
};


struct CabinetsDate
{
	FVector size;
	float Min, Max;
	int templateId,Categoryid;
	FVector Pos;
};





enum class EDoorWindowType : uint8
{
	None = 0,
	DoorWay ,
	SingleDoor,
	DoubleDoor,
	SlidingDoor,
	Window,//��ͨ��
	BayWindow,//Ʈ��
	FrenchWindow,//��ش�
};

struct FExpansionLine;
struct FExpansionCabnetPillars;



struct DoorWindow
{
	int id;
	FVector2D start, end;
	EDoorWindowType Type;
};

struct Layoutpoint
{
	FVector2D Point;
	TArray<Layoutpoint>RelevancePoint;
};

struct Layoutwall
{
	Layoutwall() {}
	void Set(FVector2D st, FVector2D en)
	{
		Start.Point = st;
		End.Point = en;
		Start.RelevancePoint.Add(End);
		End.RelevancePoint.Add(Start);
	}
	Layoutpoint Start;
	Layoutpoint End;
};

struct FSelectWalldata
{
	Layoutwall Wall;
	FVector2D Dir;
	int Id;
	TArray<DoorWindow> DoorWindowlist;
	TArray<FExpansionCabnetPillars> PillarsList;
};

struct  Selectdata
{
	TArray<FExpansionLine> wallList;
	int32	mCategoryId;//������Ŀid
	int32	mActionType; //0������ 1��ƫ�ò���
	int32	mDepth;	//���(ֻ�����׶��Ʋ���)
    int32	mFloor;	//�ع񲼾� 1,2 ���񲼾�
};

struct Areadata
{
	TArray<FExpansionLine> wallList;
	TArray<DoorWindow> doorwindowList;
	TArray<FExpansionCabnetPillars> PillarsList;
};

enum class CustomCabinetsType:uint8
{
	None = 0,
	//����
	Cabinet,
	//�¹�
	Wardrobe,
};

USTRUCT(BlueprintType)
struct  FExpansionTest
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector	Pos;
};

USTRUCT(BlueprintType)
struct FExpansionLine
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D End;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D Dir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Id;
};

USTRUCT(BlueprintType)
struct FExpansionCabnetPillars
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D Pos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Angle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D Dir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Type;
};

