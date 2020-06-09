// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CgTypes.h"
#include "CgDataConvUtility.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Templates/UnrealTemplate.h"
#include "LinearEntityAlg.generated.h"

template<class DataType>
struct FClosedIntervalD1
{
public:
	DataType StartPnt;
	DataType EndPnt;
	bool bValid;

	FClosedIntervalD1()
	{
		bValid = false;
	}

	FClosedIntervalD1(DataType InStart, DataType InEnd)
		:StartPnt(InStart), EndPnt(InEnd)
	{
		if (StartPnt > EndPnt)
		{
			Swap(StartPnt, EndPnt);
		}
		bValid = true;
	}

	static bool DoIntervalIntersect(const FClosedIntervalD1 &First, const FClosedIntervalD1 &Second)
	{
		return FGenericPlatformMath::Max(First.StartPnt, Second.StartPnt) <= FGenericPlatformMath::Min(First.EndPnt, Second.EndPnt);
	}

	FClosedIntervalD1& operator+=(const FClosedIntervalD1 &AnOtherInterval)
	{
		if (DoIntervalIntersect(*this, AnOtherInterval))
		{
			StartPnt = FGenericPlatformMath::Min(StartPnt, AnOtherInterval.StartPnt);
			EndPnt = FGenericPlatformMath::Max(EndPnt, AnOtherInterval.EndPnt);
		}

		return *this;
	}

	bool DoSubtract(const FClosedIntervalD1 &SubtractedInterval, FClosedIntervalD1 &OutLeftInterval,
		FClosedIntervalD1 &OutRightInterval)
	{
		OutLeftInterval.bValid = false;
		OutRightInterval.bValid = false;
		bool bResultValid = false;
		if (StartPnt < SubtractedInterval.StartPnt)
		{
			OutLeftInterval.StartPnt = StartPnt;
			OutLeftInterval.EndPnt = FGenericPlatformMath::Min(SubtractedInterval.StartPnt, EndPnt);
			OutLeftInterval.bValid = true;
			bResultValid = true;
		}

		if (SubtractedInterval.EndPnt < EndPnt)
		{
			OutRightInterval.StartPnt = FGenericPlatformMath::Max(StartPnt, SubtractedInterval.EndPnt);
			OutRightInterval.EndPnt = EndPnt;
			OutRightInterval.bValid = true;
			bResultValid = true;
		}

		return bResultValid;
	}
};
typedef FClosedIntervalD1<float> FFClosedInterval;
typedef FClosedIntervalD1<double> FDClosedInterval;

// library for linear entity like point, segment, line, polygon
UCLASS()
class AJDR_API ULinearEntityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static float SquaredDistanceFromPointToLine(const FVector2D& Point, const FVector2D& LineStart, const FVector2D& LineStop);

	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static float SquaredDistanceFromPntToSegment(const FVector2D& Point, const FVector2D& SegStart, const FVector2D& SegEnd);

	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static float GetSquaredPointDist2Polygon(const FVector2D &InPnt, const TArray<FVector2D>& InPolygon);

	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static ECgPolyBoundedSide IsPointInPolygon(const FVector2D& InJudgePnt, const TArray<FVector2D>& InPolygon);

	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static ECgPolyOrientation GetPolygonOrientation(const TArray<FVector2D>& InPolygon);

	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static bool IsSimplePolygon(const TArray<FVector2D>& InPolygon);

	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static float GetRotatedOfVector2D(const FVector2D& InVector);

	// convert to radian range from 0 to 2*PI
	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static float UnwindRadian2Normal(float InRadian);

	// convert to radian range from 0 to 2*PI
	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static float Ue4Radian2Normal(float InUe4Radian);

	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static bool CheckLineSegmentsIntersect(const FVector2D& Segment1Start, const FVector2D& Segment1End, const FVector2D& Segment2Start, const FVector2D& Segment2End);

	UFUNCTION(BlueprintCallable, Category = "Linear Algorithm")
		static TArray<FVector2D> Convert3dTo2dPnts(const TArray<FVector> &In3dPath);

	static int32 PowerInt(int32 X, int32 P);

	static TArray<FVector2D> TwoPolygonWithHolesArea(TArray<FVector2D> & PolygonA, TArray<FVector2D> & PolygonB);

public:
	template<typename DataType>
	static void ClosedIntervalSetCombine(typename TArray<FClosedIntervalD1<DataType>> &OutCombinedIntervals,
		typename const TArray<FClosedIntervalD1<DataType>> &InAnyIntervals)
	{
		OutCombinedIntervals = InAnyIntervals;
		OutCombinedIntervals.Sort([](const FClosedIntervalD1<DataType> &First, const FClosedIntervalD1<DataType> &Second)
		{
			return First.StartPnt < Second.StartPnt;
		});
		int32 IntervalNum = OutCombinedIntervals.Num();
		if (IntervalNum < 1)
		{
			return;
		}

		auto *LastInterval = &(OutCombinedIntervals[0]);
		for (int32 i = 1; i < OutCombinedIntervals.Num();)
		{
			auto &CurInterval = OutCombinedIntervals[i];
			if (FClosedIntervalD1<DataType>::DoIntervalIntersect(*LastInterval, CurInterval))
			{
				*LastInterval += CurInterval;
				OutCombinedIntervals.RemoveAt(i);
			}
			else
			{
				LastInterval = &(CurInterval);
				++i;
			}
		}
	}

	template<typename DataType>
	static void IntervalSubtract(typename TArray<FClosedIntervalD1<DataType>> &OutResult,
		typename FClosedIntervalD1<DataType> TotalInterval, typename const TArray<FClosedIntervalD1<DataType>> &InSortedSubIntervals)
	{

		bool MakeMarker = false;
		for (auto &SubInterval : InSortedSubIntervals)
		{

			if (SubInterval.EndPnt <= TotalInterval.StartPnt ||
				SubInterval.StartPnt >= TotalInterval.EndPnt)
			{
				continue;
			}
			FClosedIntervalD1<DataType> LeftInterval, RightInterval;
			TotalInterval.DoSubtract(SubInterval, LeftInterval, RightInterval);
			if (LeftInterval.bValid)
			{
				OutResult.Add(LeftInterval);
			}
			if (RightInterval.bValid)
			{
				TotalInterval = RightInterval;
			}
			else
			{
				MakeMarker = true;
				break;
			}
		}
		if (!MakeMarker)
		{
			OutResult.Add(TotalInterval);
		}

	}

	template<typename FCgalP2CKernel>
	static void IntersectionOfPolygonAndSegment(typename TArray<typename FCgDataConvUtil<FCgalP2CKernel>::FP2CSegment2D> &OutSegments,
		typename const FCgDataConvUtil<FCgalP2CKernel>::FP2CPolygon2D &InPolygon,
		typename const FCgDataConvUtil<FCgalP2CKernel>::FP2CSegment2D &InSegment)
	{
		using FPoint2D = FCgDataConvUtil<FCgalP2CKernel>::FP2CPoint2D;
		using FCGVector2D = FCgDataConvUtil<FCgalP2CKernel>::FP2CVector2D;
		using FSegment2D = FCgDataConvUtil<FCgalP2CKernel>::FP2CSegment2D;
		TArray<FPoint2D> PntsIntersection;
		for (auto EdgeIter = InPolygon.edges_begin(); EdgeIter != InPolygon.edges_end(); ++EdgeIter)
		{
			auto Result = CGAL::intersection(InSegment, *EdgeIter);
			if (Result)
			{
				if (FPoint2D* Intersect = boost::get<FPoint2D>(&*Result))
				{
					PntsIntersection.Add(*Intersect);
				}
				// i think we needn't check segment intersection for calculating length
				/*else if (FSegment2D *Intersect = boost::get<FPoint2D>(&*Result))
				{
				PntsIntersection.Add(Intersect->source());
				PntsIntersection.Add(Intersect->target());
				}*/
			}
		}
		FPoint2D SegmentStart = InSegment[0];
		FPoint2D SegmentEnd = InSegment[1];
		FCGVector2D SegmentDir = InSegment.to_vector();
		PntsIntersection.Sort([SegmentDir, &SegmentStart](const FPoint2D &First, const FPoint2D &Second)
		{
			FCGVector2D SegmentStartPnt(SegmentStart[0], SegmentStart[1]);
			FCGVector2D FirstPnt(First[0], First[1]);
			FCGVector2D SecondPnt(Second[0], Second[1]);
			return (SegmentDir * (FirstPnt - SegmentStartPnt)) > (SegmentDir * (SecondPnt - SegmentStartPnt));
		});
		auto StartPntRelation = InPolygon.bounded_side(SegmentStart);
		auto EndPntRelation = InPolygon.bounded_side(SegmentEnd);
		auto LastPntRelation = StartPntRelation;
		auto LastPnt = SegmentStart;

		const float DisBoundValue = 0.1;
		for (auto &PntIntersection : PntsIntersection)
		{
			if (LastPntRelation == CGAL::ON_BOUNDED_SIDE)
			{
				float DisDiffer = sqrt(CGAL::squared_distance(LastPnt, PntIntersection));
				//if (LastPnt != PntIntersection)
				if (DisDiffer > DisBoundValue)
				{
					FSegment2D NewSegment(LastPnt, PntIntersection);
					OutSegments.Add(NewSegment);
				}

				LastPntRelation = CGAL::ON_UNBOUNDED_SIDE;
			}
			else
			{
				LastPnt = PntIntersection;
				LastPntRelation = CGAL::ON_BOUNDED_SIDE;
			}
		}

		if ((LastPntRelation == CGAL::ON_BOUNDED_SIDE) && (EndPntRelation == CGAL::ON_BOUNDED_SIDE))
		{
			FSegment2D NewSegment(LastPnt, SegmentEnd);
			OutSegments.Add(NewSegment);
		}
	}


	template<typename FCgalP2CKernel>
	static bool ClosestPntOnSegment(typename FCgDataConvUtil<FCgalP2CKernel>::FP2CPoint2D &OutClosestPnt,
		typename const FCgDataConvUtil<FCgalP2CKernel>::FP2CPoint2D &PointGiven,
		typename const FCgDataConvUtil<FCgalP2CKernel>::FP2CSegment2D &Segment,
		typename FCgDataConvUtil<FCgalP2CKernel>::FP2CLine2D *LineOnSegPtr = nullptr)
	{
		auto SupportingLine = Segment.supporting_line();
		if (!LineOnSegPtr)
		{
			LineOnSegPtr = &SupportingLine;
		}
		LineOnSegPtr = &SupportingLine;

		OutClosestPnt = LineOnSegPtr->projection(PointGiven);
		FVector2D testPntGiven = FCgDataConvUtil<FCgalP2CKernel>::CgPoint2D2VectorConv(PointGiven);
		FVector2D testSeg1 = FCgDataConvUtil<FCgalP2CKernel>::CgPoint2D2VectorConv(Segment[0]);
		FVector2D testSeg2 = FCgDataConvUtil<FCgalP2CKernel>::CgPoint2D2VectorConv(Segment[1]);
		FVector2D test = FCgDataConvUtil<FCgalP2CKernel>::CgPoint2D2VectorConv(OutClosestPnt);
		double PntDist2Segment = CGAL::squared_distance(OutClosestPnt, Segment);
		//!Segment.has_on(OutClosestPnt)
		bool bPntOnSeg = true;
		if (PntDist2Segment > SmallPositiveNumber)
		{
			OutClosestPnt = CGAL::compare_distance_to_point(OutClosestPnt, Segment[0], Segment[1]) == CGAL::SMALLER ? Segment[0] : Segment[1];
			bPntOnSeg = false;
		}

		return bPntOnSeg;
	}
};
