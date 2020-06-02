// Fill out your copyright notice in the Description page of Project Settings.


#include "LinearEntityAlg.h"

#include <CGAL/Exact_circular_kernel_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Simple_cartesian.h>

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/intersections.h>
#include <CGAL/Polygon_2_algorithms.h>
#include "CgDataConvUtility.h"
#include "Kismet/KismetMathLibrary.h"


float ULinearEntityLibrary::SquaredDistanceFromPointToLine(const FVector2D& Point, const FVector2D& LineStart, const FVector2D& LineStop)
{
	using Kernel = CGAL::Simple_cartesian<double>;
	using Point_2 = Kernel::Point_2;
	using Line_2 = Kernel::Line_2;

	Point_2 CheckPoint(Point.X, Point.Y);
	Line_2 CheckLine(Point_2(LineStart.X, LineStart.Y), Point_2(LineStop.X, LineStop.Y));

	return CGAL::squared_distance(CheckPoint, CheckLine);
}

float ULinearEntityLibrary::SquaredDistanceFromPntToSegment(const FVector2D& Point, const FVector2D& SegStart, const FVector2D& SegEnd)
{
	using Kernel = CGAL::Simple_cartesian<double>;
	using Point_2 = Kernel::Point_2;
	using Segment_2 = Kernel::Segment_2;

	Point_2 CheckPoint(Point.X, Point.Y);
	Segment_2 CheckLine(Point_2(SegStart.X, SegStart.Y), Point_2(SegEnd.X, SegEnd.Y));

	return CGAL::squared_distance(CheckPoint, CheckLine);
}

float ULinearEntityLibrary::GetSquaredPointDist2Polygon(const FVector2D &InPnt, const TArray<FVector2D>& InPolygon)
{
	float PntDist = 0.0f;
	if (IsPointInPolygon(InPnt, InPolygon) == ECgPolyBoundedSide::EON_UNBOUNDED)
	{
		bool bInit = false;
		int32 PolygonPntCnt = InPolygon.Num();
		for (int32 i = 0; i < PolygonPntCnt; ++i)
		{
			FVector2D CurPnt = InPolygon[i];
			FVector2D NextPnt = InPolygon[(i + 1)% PolygonPntCnt];
			float CurSegDist = SquaredDistanceFromPntToSegment(InPnt, CurPnt, NextPnt);
			if (!bInit || CurSegDist < PntDist)
			{
				PntDist = CurSegDist;
			}
		}
	}

	return PntDist;
}

ECgPolyBoundedSide ULinearEntityLibrary::IsPointInPolygon(const FVector2D& InJudgePnt, const TArray<FVector2D>& InPolygon)
{
	using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
	using FDataConv = FCgDataConvUtil<Kernel>;

	FDataConv::FCgPnt2DArray PolyPntsInCg;
	FDataConv::Vector2DArray2CgPntsConv(PolyPntsInCg, InPolygon);

	return (ECgPolyBoundedSide)(CGAL::bounded_side_2(PolyPntsInCg.begin(),
		PolyPntsInCg.end(),
		FDataConv::Vector2D2CgPointConv(InJudgePnt), Kernel()) + 1);
}

ECgPolyOrientation ULinearEntityLibrary::GetPolygonOrientation(const TArray<FVector2D>& InPolygon)
{
	ECgPolyOrientation PolyOrientation = ECgPolyOrientation::EUNDEFINED;
	if (InPolygon.Num() > 2)
	{
		using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
		using FDataConv = FCgDataConvUtil<Kernel>;

		FDataConv::FCgPnt2DArray PointsOfCgal;
		FDataConv::Vector2DArray2CgPntsConv(PointsOfCgal, InPolygon);
		FDataConv::FP2CPolygon2D Polygon(PointsOfCgal.begin(), PointsOfCgal.end());

		switch (Polygon.orientation())
		{
		case CGAL::CLOCKWISE:
			PolyOrientation = ECgPolyOrientation::ECLOCKWISE;
			break;
		case CGAL::COUNTERCLOCKWISE:
			PolyOrientation = ECgPolyOrientation::ECOUNTERCLOCKWISE;
			break;
		default:
			break;
		}
	}
	
	return PolyOrientation;
}

bool ULinearEntityLibrary::IsSimplePolygon(const TArray<FVector2D>& InPolygon)
{
	using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
	using FDataConv = FCgDataConvUtil<Kernel>;

	FDataConv::FCgPnt2DArray PointsOfCgal;
	FDataConv::Vector2DArray2CgPntsConv(PointsOfCgal, InPolygon);
	FDataConv::FP2CPolygon2D Polygon(PointsOfCgal.begin(), PointsOfCgal.end());

	return Polygon.is_simple();
}

float ULinearEntityLibrary::GetRotatedOfVector2D(const FVector2D& InVector)
{
	float HeadingAngle = FMath::Atan2(InVector.Y, InVector.X);
	HeadingAngle = Ue4Radian2Normal(HeadingAngle);

	return HeadingAngle;
}

float ULinearEntityLibrary::UnwindRadian2Normal(float InRadian)
{
	return Ue4Radian2Normal(FMath::UnwindRadians(InRadian));
}

float ULinearEntityLibrary::Ue4Radian2Normal(float InUe4Radian)
{
	if (InUe4Radian < 0)
	{
		InUe4Radian += UKismetMathLibrary::GetTAU();
	}

	return InUe4Radian;
}

bool ULinearEntityLibrary::CheckLineSegmentsIntersect(const FVector2D& Segment1Start, const FVector2D& Segment1End, const FVector2D& Segment2Start, const FVector2D& Segment2End)
{
	const FVector2D Segment1Dir = Segment1End - Segment1Start;
	const FVector2D Segment2Dir = Segment2End - Segment2Start;

	const float Determinant = FVector2D::CrossProduct(Segment1Dir, Segment2Dir);
	if (!FMath::IsNearlyZero(Determinant))
	{
		const FVector2D SegmentStartDelta = Segment2Start - Segment1Start;
		const float OneOverDet = 1.0f / Determinant;
		const float Seg1Intersection = FVector2D::CrossProduct(SegmentStartDelta, Segment2Dir) * OneOverDet;
		const float Seg2Intersection = FVector2D::CrossProduct(SegmentStartDelta, Segment1Dir) * OneOverDet;

		const float Epsilon = 1 / 128.0f;
		return (Seg1Intersection > Epsilon && Seg1Intersection < 1.0f - Epsilon && Seg2Intersection > Epsilon && Seg2Intersection < 1.0f - Epsilon);
	}

	return false;
}

TArray<FVector2D> ULinearEntityLibrary::Convert3dTo2dPnts(const TArray<FVector> &In3dPath)
{
	TArray<FVector2D> Out2dPnts;
	for (auto &In3dPnt : In3dPath)
	{
		Out2dPnts.Add(FVector2D(In3dPnt));
	}

	return Out2dPnts;
}

int32 ULinearEntityLibrary::PowerInt(int32 X, int32 P)
{
	if (P == 0)
		return 1;
	if (P == 1)
		return X;

	return X * PowerInt(X, P - 1);
}

TArray<FVector2D> ULinearEntityLibrary::TwoPolygonWithHolesArea(TArray<FVector2D> & PolygonA, TArray<FVector2D> & PolygonB)
{
	using FE2EDataConv = FCgDataConvUtil<CGAL::Exact_predicates_exact_constructions_kernel>;

	TArray<FVector2D> Out;
	FE2EDataConv::FP2CPolygon2D P;
	FE2EDataConv::FP2CPolygon2D Q;
	FE2EDataConv::FP2CPolygon2D S;

	for (int i = 0; i < PolygonA.Num(); ++i)
	{
		P.push_back(FE2EDataConv::Vector2D2CgPointConv(PolygonA[i]));
	}
	for (int i = 0; i < PolygonB.Num(); ++i)
	{
		Q.push_back(FE2EDataConv::Vector2D2CgPointConv(PolygonB[i]));
	}

	FE2EDataConv::FP2CPolygonWithHoles2D unionR;
	if (CGAL::join(P, Q, unionR))
	{
		FE2EDataConv::FP2CPolygon2D &OutPolygon = unionR.outer_boundary();
		for (auto iterPnt = OutPolygon.vertices_begin();
			iterPnt != OutPolygon.vertices_end(); ++iterPnt)
		{
			Out.Add(FE2EDataConv::CgPoint2D2VectorConv(*iterPnt));
		}
	}
	return Out;
}
