

#include "PolygonAlg.h"
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

#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <vector>
#include <CGAL/intersections.h>
#include "VaRestJsonObject.h"

#include<CGAL/intersections.h>
#include<CGAL/Ray_2.h>
#include<CGAL/Vector_2.h>

namespace Triangulation
{
	struct FaceInfo2
	{
		FaceInfo2()
		{
		}
		int NestingLevel;
		bool IsInDomain()
		{
			return NestingLevel % 2 == 1;
		}
	};

	using K = CGAL::Exact_predicates_inexact_constructions_kernel;
	using Vb = CGAL::Triangulation_vertex_base_2<K>;
	using Fbb = CGAL::Triangulation_face_base_with_info_2<FaceInfo2, K>;
	using Fb = CGAL::Constrained_triangulation_face_base_2<K, Fbb>;
	using TDS = CGAL::Triangulation_data_structure_2<Vb, Fb>;
	using Itag = CGAL::Exact_predicates_tag;
	using CDT = CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>;
	using Point = CDT::Point;
	using Polygon_2 = CGAL::Polygon_2<K>;
	using Point_2 = K::Point_2;
	using Circle_2 = K::Circle_2;
	using Polygon_with_holes_2 = CGAL::Polygon_with_holes_2<K>;
	using Pwh_list_2 = std::list<Polygon_with_holes_2>;


	using DFb = CGAL::Delaunay_mesh_face_base_2<K>;
	using DTDS = CGAL::Triangulation_data_structure_2<Vb, DFb>;
	using CDTDS = CGAL::Constrained_Delaunay_triangulation_2<K, DTDS>;
	using Criteria = CGAL::Delaunay_mesh_size_criteria_2<CDTDS>;
	using Vertex_handle = CDTDS::Vertex_handle;
	using Mesher = CGAL::Delaunay_mesher_2<CDTDS, Criteria>;
	using Triangle_2 = K::Triangle_2;
	using Segment_2 = K::Segment_2;
	using Line_2 = K::Line_2;
	using Intersect_2 = K::Intersect_2;
	using Ray_2 = K::Ray_2;
	using Vector_2 = K::Vector_2;

	void MarkDomains(CDT& ct, CDT::Face_handle Start, int Index, std::list<CDT::Edge>& Border)
	{
		using CDT = CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>;

		if (Start->info().NestingLevel != -1)
		{
			return;
		}
		std::list<CDT::Face_handle> Queue;
		Queue.push_back(Start);
		while (!Queue.empty())
		{
			CDT::Face_handle FH = Queue.front();
			Queue.pop_front();
			if (FH->info().NestingLevel == -1)
			{
				FH->info().NestingLevel = Index;
				for (int i = 0; i < 3; i++)
				{
					CDT::Edge E(FH, i);
					CDT::Face_handle n = FH->neighbor(i);
					if (n != nullptr && n->info().NestingLevel == -1)
					{
						if (ct.is_constrained(E))
							Border.push_back(E);
						else
							Queue.push_back(n);
					}
				}
			}
		}
	}

	void MarkDomains(CDT& MyCdt)
	{
		for (CDT::All_faces_iterator Iter = MyCdt.all_faces_begin(); Iter != MyCdt.all_faces_end(); ++Iter)
		{
			Iter->info().NestingLevel = -1;
		}
		std::list<CDT::Edge> Border;
		MarkDomains(MyCdt, MyCdt.infinite_face(), 0, Border);
		while (!Border.empty())
		{
			CDT::Edge E = Border.front();
			Border.pop_front();
			CDT::Face_handle n = E.first->neighbor(E.second);
			if (n != nullptr && n->info().NestingLevel == -1)
			{
				MarkDomains(MyCdt, n, E.first->info().NestingLevel + 1, Border);
			}
		}
	}

	bool GetPointSetCenter(const FPoint2DArray InPnts, FVector2D& CenterPoint)
	{
		if (InPnts.Num() == 0)
		{
			return false;
		}
		double Xcoor = 0, Ycoor = 0.0;
		for (int32 Index = 0; Index < InPnts.Num(); Index++)
		{
			Xcoor = Xcoor + InPnts[Index].X;
			Ycoor = Ycoor + InPnts[Index].Y;
		}
		Xcoor = Xcoor / InPnts.Num();
		Ycoor = Ycoor / InPnts.Num();
		CenterPoint.X = Xcoor;
		CenterPoint.Y = Ycoor;
		return true;
	}

	bool FindBancePoint(const FPoint2DArray InPnts, Point_2 SeedPoint, FVector2D& LastPoint)
	{
		if (InPnts.Num() <= 2)
		{
			return false;
		}

		TArray<Segment_2> SegmentSet;
		int32 PNextIndex = 0;
		for (int32 PIndex = 0; PIndex < InPnts.Num(); PIndex++)
		{
			if (PIndex == InPnts.Num() - 1)
			{
				PNextIndex = 0;
			}
			else
			{
				PNextIndex = PIndex + 1;
			}
			Point_2 Pa(InPnts[PIndex].X, InPnts[PIndex].Y);
			Point_2 Pb(InPnts[PNextIndex].X, InPnts[PNextIndex].Y);
			SegmentSet.Add(Segment_2(Pa, Pb));
		}
		TArray<float> DisSet;
		int32 TargetMinIndex = 0;
		float MinDis = CGAL::squared_distance(SegmentSet[0], SeedPoint);
		for (int32 SegIndex = 1; SegIndex < SegmentSet.Num(); SegIndex++)
		{
			float DisNum = CGAL::squared_distance(SegmentSet[SegIndex], SeedPoint);
			if (DisNum < MinDis)
			{
				MinDis = DisNum;
				TargetMinIndex = SegIndex;
			}
		}
		float SegVecX = SegmentSet[TargetMinIndex].direction().dx();
		float SegVecY = SegmentSet[TargetMinIndex].direction().dy();
		float SegVecL = sqrt(SegmentSet[TargetMinIndex].squared_length());
		FVector2D SegVec(SegVecX / SegVecL, SegVecY / SegVecL);
		FVector2D VerticalVec(-SegVec.Y, SegVec.X);
		Point_2 Pbottom(SeedPoint.x() + sqrt(MinDis)*VerticalVec.X, SeedPoint.y() + sqrt(MinDis)*VerticalVec.Y);
		Line_2  Line(Pbottom, SeedPoint);

		TArray<Point_2> InterSectPs;
		for (int32 SegIndex = 0; SegIndex < SegmentSet.Num(); SegIndex++)
		{
			CGAL::cpp11::result_of<Intersect_2(Segment_2, Line_2)>::type
				result = intersection(SegmentSet[SegIndex], Line);
			if (result)
			{
				if (const Point_2* Pintersect = boost::get<Point_2>(&*result))
				{
					InterSectPs.Add(*Pintersect);
				}
				else
				{
					continue;
				}
			}
		}

		FVector2D PointTop;
		int32 TargetIndexToSeed = 0;
		if (InterSectPs.Num() < 2)
		{
			return false;
		}
		else if (InterSectPs.Num() == 2)
		{
			LastPoint = FVector2D(0.5*(InterSectPs[0].x() + InterSectPs[1].x()), 0.5*(InterSectPs[0].y() + InterSectPs[1].y()));
			return true;
		}
		else
		{
			TArray<float> AllDisToSeed;
			float MinDisToSeed = CGAL::squared_distance(InterSectPs[0], SeedPoint);
			int32 MinIndex = 0;
			for (int32 InterIndex = 1; InterIndex < InterSectPs.Num(); InterIndex++)
			{
				float DisToSeed = CGAL::squared_distance(InterSectPs[InterIndex], SeedPoint);
				if (MinDisToSeed > DisToSeed)
				{
					MinDisToSeed = DisToSeed;
					MinIndex = InterIndex;
				}
			}
			FVector2D NewPointBottom(InterSectPs[MinIndex].x(), InterSectPs[MinIndex].y());
			FVector2D BaseVec(InterSectPs[MinIndex].x() - SeedPoint.x(), InterSectPs[MinIndex].y() - SeedPoint.y());
			TArray<float> ReDis;
			TMap<int32, float> IndexMapDis;
			int32 OpIndex = 0;
			for (int32 InSecPIndex = 0; InSecPIndex < InterSectPs.Num(); InSecPIndex++)
			{
				if (InSecPIndex != MinIndex)
				{
					FVector2D TempVec(InterSectPs[InSecPIndex].x() - SeedPoint.x(), InterSectPs[InSecPIndex].y() - SeedPoint.y());
					if (BaseVec.X*TempVec.X + BaseVec.Y*TempVec.Y < 0)
					{
						float NewDis = CGAL::squared_distance(InterSectPs[InSecPIndex], SeedPoint);
						ReDis.Add(NewDis);
						IndexMapDis.Add(InSecPIndex, NewDis);
					}
				}
			}
			if (ReDis.Num() == 1)
			{
				OpIndex = IndexMapDis.Contains(ReDis[0]);
			}
			else
			{
				float MinReverseDis = ReDis[0];
				for (int32 ReverseIndex = 1; ReverseIndex < ReDis.Num(); ReverseIndex++)
				{

					if (ReDis[ReverseIndex] < MinReverseDis)
					{
						MinReverseDis = ReDis[ReverseIndex];

					}
				}
				OpIndex = IndexMapDis.Contains(MinReverseDis);
			}

			PointTop = FVector2D(InterSectPs[OpIndex].x(), InterSectPs[OpIndex].y());
			float FinalX = 0.5*(NewPointBottom.X + PointTop.X);
			float FinalY = 0.5*(NewPointBottom.Y + PointTop.Y);
			LastPoint = FVector2D(FinalX, FinalY);
		}

		return true;
	}


	bool GetClosestCenter(CDTDS& Mycdt, FVector2D InitialPoint, TArray<FVector2D>& TrianglesCenters, FVector2D& ClosestCenter2D)
	{
		if (Mycdt.number_of_faces() == 0)
		{
			return false;
		}
		TArray<Triangle_2> AllTriangles;
		for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
		{
			if ((*Fbegin).is_in_domain())
			{
				FVector2D  v0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
				FVector2D  v1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
				FVector2D  v2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
				Triangle_2 Tri(Mycdt.triangle(Fbegin));
				AllTriangles.Add(Tri);
				TrianglesCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
			}
		}

		float MinDis = CGAL::squared_distance(Point_2(InitialPoint.X, InitialPoint.Y), Point_2(TrianglesCenters[0].X, TrianglesCenters[0].Y));
		float DisLength = 0.0;
		int32 TargetIndex = 0;
		for (int32 Index = 1; Index < TrianglesCenters.Num(); Index++)
		{
			DisLength = CGAL::squared_distance(Point_2(InitialPoint.X, InitialPoint.Y), Point_2(TrianglesCenters[0].X, TrianglesCenters[0].Y));
			if (MinDis > DisLength)
			{
				MinDis = DisLength;
				TargetIndex = Index;
			}
		}
		ClosestCenter2D.X = TrianglesCenters[TargetIndex].X;
		ClosestCenter2D.Y = TrianglesCenters[TargetIndex].Y;
		return true;
	}




}




void FPolygonAlg::PolyTriagulation(FPoint2DArray& OutPnts,
	const FPoint2DArray& InputPoly, const TArray<FPoint2DArray>& InputHoles,
	bool bDoubleSide)
{
	using namespace Triangulation;
	using Polygon_2 = CGAL::Polygon_2<CGAL::Exact_predicates_inexact_constructions_kernel>;
	std::vector<Polygon_2> TriPolygons;
	std::vector<Polygon_2> TriHoles;

	{
		Polygon_2 CGALPoly;
		for (FVector2D MyPoint : InputPoly)
		{
			CGALPoly.push_back(Point_2(MyPoint.X, MyPoint.Y));
		}
		if (!CGALPoly.is_counterclockwise_oriented())
		{
			CGALPoly.reverse_orientation();
		}

		std::vector<Polygon_2> CGALHoles;
		for (auto Hole : InputHoles)
		{
			Polygon_2 CGALHole;
			for (FVector2D MyPoint : Hole)
			{
				CGALHole.push_back(Point_2(MyPoint.X, MyPoint.Y));
			}
			if (!CGALHole.is_counterclockwise_oriented())
			{
				CGALHole.reverse_orientation();
			}
			CGALHoles.push_back(CGALHole);
		}

		Polygon_with_holes_2 CurPolygonOutline(CGALPoly);

		Pwh_list_2 FinalPolygons;
		FinalPolygons.push_back(CurPolygonOutline);
		for (auto HolePolygon : CGALHoles)
		{
			Pwh_list_2 DiffPolygons;
			for (auto FinalPolygon : FinalPolygons)
			{
				CGAL::difference(FinalPolygon, HolePolygon, std::back_inserter(DiffPolygons));
			}
			FinalPolygons = DiffPolygons;
		}

		for (const auto& Shape : FinalPolygons)
		{
			if (!Shape.is_unbounded())
			{
				auto Boundary = Shape.outer_boundary();
				Polygon_2 TriBoundary;
				for (auto VerticeIter = Boundary.vertices_begin(); VerticeIter != Boundary.vertices_end(); ++VerticeIter)
				{
					TriBoundary.push_back(Point(CGAL::to_double(VerticeIter->x()), CGAL::to_double(VerticeIter->y())));
				}
				TriPolygons.push_back(TriBoundary);
			}
			for (auto hit = Shape.holes_begin(); hit != Shape.holes_end(); ++hit)
			{
				Polygon_2 TriHole;
				for (auto VerticeIter = hit->vertices_begin(); VerticeIter != hit->vertices_end(); ++VerticeIter)
				{
					TriHole.push_back(Point(CGAL::to_double(VerticeIter->x()), CGAL::to_double(VerticeIter->y())));
				}
				TriHoles.push_back(TriHole);
			}
		}
	}

	for (auto& Polygon : TriPolygons)
	{
		using namespace Triangulation;

		// Insert the polygons into a constrained triangulation
		CDT MyCDT;

		// construct two non-intersecting nested polygons
		{
			MyCDT.insert_constraint(Polygon.vertices_begin(), Polygon.vertices_end(), true);
		}

		for (auto Hole : TriHoles)
		{
			MyCDT.insert_constraint(Hole.vertices_begin(), Hole.vertices_end(), true);
		}

		// Mark facets that are inside the domain bounded by the polygon
		MarkDomains(MyCDT);

		for (auto Face = MyCDT.finite_faces_begin(); Face != MyCDT.finite_faces_end(); ++Face)
		{
			if (Face->info().IsInDomain())
			{
				FVector2D V0 = FVector2D(Face->vertex(0)->point().hx(), Face->vertex(0)->point().hy());
				FVector2D V1 = FVector2D(Face->vertex(1)->point().hx(), Face->vertex(1)->point().hy());
				FVector2D V2 = FVector2D(Face->vertex(2)->point().hx(), Face->vertex(2)->point().hy());

				OutPnts.Add(V0);
				OutPnts.Add(V2);
				OutPnts.Add(V1);

				if (bDoubleSide)
				{
					OutPnts.Add(V0);
					OutPnts.Add(V1);
					OutPnts.Add(V2);
				}
			}
		}
	}
}

bool FPolygonAlg::CalculateShortestL(const FPoint2DArray InPnts, float& ShortestL)
{
	if (InPnts.Num() == 0)
	{
		return false;
	}
	int32 PNextIndex = 0;
	TArray<float> AllEdgeLs;
	for (int32 PIndex = 0; PIndex < InPnts.Num(); PIndex++)
	{
		if (PIndex == InPnts.Num() - 1)
		{
			PNextIndex = 0;
		}
		else
		{
			PNextIndex = PIndex + 1;
		}
		float EdgeXDiffer = InPnts[PIndex].X - InPnts[PNextIndex].X;
		float EdgeYDiffer = InPnts[PIndex].Y - InPnts[PNextIndex].Y;
		float EachEdgeL = sqrt(EdgeXDiffer*EdgeXDiffer + EdgeYDiffer * EdgeYDiffer);
		AllEdgeLs.Add(EachEdgeL);
	}
	float MinL = AllEdgeLs[0];
	for (int32 Index = 0; Index < AllEdgeLs.Num(); Index++)
	{
		if (MinL > AllEdgeLs[Index])
		{
			MinL = AllEdgeLs[Index];
		}
	}
	ShortestL = MinL;
	return true;
}


bool FPolygonAlg::LocateBetterCenter(const FPoint2DArray InPnts, FVector2D& BetterCenter)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;
	if (InPnts.Num() <= 2)
	{
		return false;
	}
	else if (InPnts.Num() == 3)
	{
		BetterCenter = (InPnts[0] + InPnts[1] + InPnts[2]) / 3;
		return true;
	}
	else if (InPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < InPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + InPnts[PIndex].X;
			PCenterY = PCenterY + InPnts[PIndex].Y;
			Region2D.push_back(Point_2(InPnts[PIndex].X, InPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / InPnts.Num(), PCenterY / InPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			BetterCenter = IBetterCenter;
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < InPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(InPnts[Index].X, InPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			BetterCenter = TriangleCenters[TargetIndex];
			return true;
		}
	}
	else
	{
		float MaxPx = InPnts[0].X, MinPx = InPnts[0].X, MaxPy = InPnts[0].Y, MinPy = InPnts[0].Y;
		for (int32 Index = 0; Index < InPnts.Num(); Index++)
		{
			if (MaxPx < InPnts[Index].X)
			{
				MaxPx = InPnts[Index].X;
			}
		}
		for (int32 Index = 0; Index < InPnts.Num(); Index++)
		{
			if (MinPx > InPnts[Index].X)
			{
				MinPx = InPnts[Index].X;
			}
		}
		for (int32 Index = 0; Index < InPnts.Num(); Index++)
		{
			if (MaxPy < InPnts[Index].Y)
			{
				MaxPy = InPnts[Index].Y;
			}
		}
		for (int32 Index = 0; Index < InPnts.Num(); Index++)
		{
			if (MinPy > InPnts[Index].Y)
			{
				MinPy = InPnts[Index].Y;
			}
		}
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < InPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(InPnts[PIndex].X, InPnts[PIndex].Y));
		}
		FVector2D TempBetterCenter = FVector2D(0.5*(MinPx + MaxPx), 0.5*(MinPy + MaxPy));
		if (Region2D.bounded_side(Point_2(TempBetterCenter.X, TempBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			BetterCenter = TempBetterCenter;
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < InPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(InPnts[Index].X, InPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			BetterCenter = TriangleCenters[TargetIndex];
		}
	}
	return true;
}



bool FPolygonAlg::LocateSuitableCenter(const FPoint2DArray InPnts, FVector2D& SuitableCenter)
{

	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;

	TArray<FVector2D>  PostPoints,TempP, TempPoints, OutPnts;
	for (int _CurI = 0; _CurI < InPnts.Num(); ++_CurI)
	{
		PostPoints.AddUnique(InPnts[_CurI]);
	}

	bool PreProcess = MergeCollinearPoints(PostPoints, TempP);
	if (!PreProcess)
	{
		return false;
	}

	bool MergeShortSuccess = MergeShrotEdgeCmSize(TempP, TempPoints);
	if (!MergeShortSuccess)
	{
		return false;
	}

	bool MergeCollinearSuccess = MergeCollinearPoints(TempPoints, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}
	if (OutPnts.Num() <= 2)
	{
		return false;
	}
	else if (OutPnts.Num() == 3)
	{
		SuitableCenter = (OutPnts[0] + OutPnts[1] + OutPnts[2]) / 3;
		return true;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (FPolygonAlg::JudgePointInPolygon(PostPoints, IBetterCenter))
		{
			SuitableCenter = IBetterCenter;
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < PostPoints.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(PostPoints[Index].X, PostPoints[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			if (AreaSet.Num() > 0)
			{
				float MaxArea = AreaSet[0];
				int32 TargetIndex = 0;
				for (int32 Index = 1; Index < AreaSet.Num(); Index++)
				{
					if (MaxArea < AreaSet[Index])
					{
						MaxArea = AreaSet[Index];
						TargetIndex = Index;
					}
				}
				SuitableCenter = TriangleCenters[TargetIndex];
				return true;
			}
			SuitableCenter = FVector2D::ZeroVector;
			return false;
		}
	}
	else
	{
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				if (DisPFrontToPStart < DisPNextToPNextNext)
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
				}
				else if (DisPFrontToPStart >= DisPNextToPNextNext)
				{

					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;

				}

				if (FPolygonAlg::JudgePointInPolygon(PostPoints, FVector2D(TempCenterX, TempCenterY)))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}
		if (AreaSet.Num() > 0)
		{
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 CenterIndex = 1; CenterIndex < AreaCenters.Num(); CenterIndex++)
			{
				if (AreaSet[CenterIndex] > MaxArea)
				{
					MaxArea = AreaSet[CenterIndex];
					TargetIndex = CenterIndex;
				}
			}

			SuitableCenter = AreaCenters[TargetIndex];
		}

		if(!FPolygonAlg::JudgePointInPolygon(PostPoints,SuitableCenter))
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < PostPoints.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(PostPoints[Index].X, PostPoints[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> TAreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					TAreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			if (TAreaSet.Num() > 0)
			{
				float MaxArea = TAreaSet[0];
				int32 TargetIndex = 0;
				for (int32 Index = 1; Index < TAreaSet.Num(); Index++)
				{
					if (MaxArea < TAreaSet[Index])
					{
						MaxArea = TAreaSet[Index];
						TargetIndex = Index;
					}
				}
				SuitableCenter = TriangleCenters[TargetIndex];
			}
			else
			{
				SuitableCenter = FVector2D::ZeroVector;
				return false;
			}
		}
	}
	return true;
}


bool FPolygonAlg::CalculateInterSection(const FPoint2DArray InPnts, FVector2D PSource, FVector2D PTarget, FVector2D& InterSectP)
{
	if (InPnts.Num() == 0)
	{
		return false;
	}
	using namespace Triangulation;
	TArray<Segment_2> AllSegments;
	int32 PNextIndex = 0;
	for (int32 PIndex = 0; PIndex < InPnts.Num(); PIndex++)
	{
		if (PIndex == InPnts.Num() - 1)
		{
			PNextIndex = 0;
		}
		else
		{
			PNextIndex = PIndex + 1;
		}
		Segment_2 Seg(Point_2(InPnts[PIndex].X, InPnts[PIndex].Y), Point_2(InPnts[PNextIndex].X, InPnts[PNextIndex].Y));
		AllSegments.Add(Seg);
	}
	Ray_2 BaseRay_2(Point_2(PSource.X, PSource.Y), Point_2(PTarget.X, PTarget.Y));
	bool FindIntersect = false;
	for (int32 Index = 0; Index < AllSegments.Num(); Index++)
	{
		CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay_2);
		if (Result)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
			{
				InterSectP.X = (*TempIntersectP).x();
				InterSectP.Y = (*TempIntersectP).y();
				FindIntersect = true;
				break;
			}

		}
	}
	if (!FindIntersect)
	{
		return false;
	}
	return true;
}


bool  FPolygonAlg::MergeShortEdges(const FPoint2DArray InPnts, FPoint2DArray& OutPnts)
{
	const int32 ShortEdgeLengthLimit = 20;
	if (InPnts.Num() == 0)
	{
		return false;
	}
	FPoint2DArray TempOutPnts;
	TArray<int32> RemoveIndexs;
	if (InPnts.Num() > 4)
	{
		int32 PNextIndex = 0;
		for (int32 Index = 0; Index < InPnts.Num(); Index++)
		{
			if (Index == InPnts.Num() - 1)
			{
				PNextIndex = 0;
			}
			else
			{
				PNextIndex = Index + 1;
			}
			float XDiffer = InPnts[Index].X - InPnts[PNextIndex].X;
			float YDiffer = InPnts[Index].Y - InPnts[PNextIndex].Y;
			float EdgeL = sqrt(XDiffer*XDiffer + YDiffer * YDiffer);
			if (EdgeL <= ShortEdgeLengthLimit)
			{
				RemoveIndexs.AddUnique(Index);
				RemoveIndexs.AddUnique(PNextIndex);
			}
		}
	}
	for (int J = 0; J <= InPnts.Num() - 1; ++J)
	{
		if (!RemoveIndexs.Contains(J))
		{
			TempOutPnts.Add(InPnts[J]);
		}
	}
	if (TempOutPnts.Num() <= 3)
	{
		OutPnts = InPnts;
	}
	else
	{
		bool IsSuccess = FPolygonAlg::MergeCollinearPoints(TempOutPnts, OutPnts);
		return IsSuccess;
	}
	return true;
}

bool  FPolygonAlg::MergeCollinearPoints(const FPoint2DArray InPnts, FPoint2DArray& OutPnts)
{
	TArray<FVector2D> PostPoints;
	for (int _CurI = 0; _CurI < InPnts.Num(); ++_CurI)
	{
		PostPoints.AddUnique(InPnts[_CurI]);
	}
	if (PostPoints.Num() <= 2)
	{
		return false;
	}
	if (PostPoints.Num() > 4)
	{
		int32  NextIndex = 0, BeforeIndex = 0;
		for (int32 Index = 0; Index < PostPoints.Num(); Index++)
		{
			if (Index == PostPoints.Num() - 1)
			{
				NextIndex = 0;
				BeforeIndex = Index - 1;
			}
			else if (Index == 0)
			{
				NextIndex = Index + 1;
				BeforeIndex = PostPoints.Num() - 1;
			}
			else
			{
				NextIndex = Index + 1;
				BeforeIndex = Index - 1;
			}
			FVector2D CurrentVec = FVector2D(PostPoints[NextIndex].X - PostPoints[Index].X,PostPoints[NextIndex].Y - PostPoints[Index].Y);
			FVector2D NextVec = FVector2D(PostPoints[Index].X - PostPoints[BeforeIndex].X, PostPoints[Index].Y - PostPoints[BeforeIndex].Y);
			CurrentVec = CurrentVec.GetSafeNormal();
			NextVec = NextVec.GetSafeNormal();
			float VecInnerProduct = CurrentVec.X*NextVec.Y - CurrentVec.Y*NextVec.X;
			if (abs(VecInnerProduct) <= 1.0E-2)
			{
				continue;
			}
			else
			{
				OutPnts.Add(PostPoints[Index]);
			}
		}
		return true;
	}
	else
	{
		OutPnts = PostPoints;
	}
	return true;
}

bool  FPolygonAlg::LocateSuitableCenters(const FPoint2DArray InPnts, FVector2D& MaxRegionCenter, TArray<FVector2D>& SmallRegionCenters)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;

	TArray<FVector2D> TempOutPnts, OutPnts;
	bool MergeSuccess = MergeShortEdges(InPnts, TempOutPnts);
	if (!MergeSuccess)
	{
		return false;
	}

	bool MergeCollinearSuccess = MergeCollinearPoints(TempOutPnts, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}

	FVector2D SuitableCenter;

	if (OutPnts.Num() <= 2)
	{
		return false;
	}
	else if (OutPnts.Num() == 3)
	{
		MaxRegionCenter = (OutPnts[0] + OutPnts[1] + OutPnts[2]) / 3;
		return true;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			MaxRegionCenter = IBetterCenter;
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			MaxRegionCenter = TriangleCenters[TargetIndex];
			return true;
		}
	}
	else
	{
		TMap<FVector2D, TArray<FVector2D>> CenterMapLocalRegions;
		TMap<FVector2D, float> CenterMapRate;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float MaxL = DisPFrontToPStart, MinL = DisPFrontToPStart;
				if (DisPNextToPNextNext >= MaxL)
				{
					MaxL = DisPNextToPNextNext;
					if (DisPStartToPNext >= MaxL)
					{
						MaxL = DisPStartToPNext;
					}
				}
				else
				{
					if (DisPStartToPNext >= MaxL)
					{
						MaxL = DisPStartToPNext;
					}
				}

				if (DisPNextToPNextNext <= MinL)
				{
					MinL = DisPNextToPNextNext;
					if (DisPStartToPNext <= MinL)
					{
						MinL = DisPStartToPNext;
					}
				}
				else
				{
					if (DisPStartToPNext <= MinL)
					{
						MinL = DisPStartToPNext;
					}
				}

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				float VerticalDis1 = DisPFrontToPStart * sqrt(1 - Product1 * Product1);
				float VerticalDis2 = DisPNextToPNextNext * sqrt(1 - Product2 * Product2);


				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 3.5) && (VerticalDis1 >= 400) && (VerticalDis2 >= 400))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;

				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 3.5) && (VerticalDis1 >= 400) && (VerticalDis2 >= 400))
				{

					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;


				}



				if (Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE)
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
					TArray<FVector2D> TempAttr;
					TempAttr.Add(OutPnts[PStart]);
					TempAttr.Add(OutPnts[PNext]);
					TempAttr.Add(OutPnts[PNextNext]);
					TempAttr.Add(OutPnts[PFront]);
					CenterMapLocalRegions.Add(TempCenter, TempAttr);
					CenterMapRate.Add(TempCenter, MaxL / MinL);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}




		float MaxArea = 0.0;
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if ((AreaSet[CenterIndex] != 0.0) && (CenterMapRate[AreaCenters[CenterIndex]] <= 3.5))
			{
				if (AreaSet[CenterIndex] > MaxArea)
				{
					MaxArea = AreaSet[CenterIndex];
					TargetIndex = CenterIndex;
				}
			}

		}

		MaxRegionCenter = AreaCenters[TargetIndex];
		TArray<FVector2D> TempSmallRegionCenters;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if ((AreaSet[CenterIndex] != MaxArea) && (AreaSet[CenterIndex] != 0))
			{
				TempSmallRegionCenters.Add(AreaCenters[CenterIndex]);
			}
		}
		TArray<FVector2D> UpdateSmallRegionCenters;
		for (int32 Num = 0; Num < TempSmallRegionCenters.Num(); Num++)
		{
			float XDis = TempSmallRegionCenters[Num].X - MaxRegionCenter.X;
			float YDis = TempSmallRegionCenters[Num].Y - MaxRegionCenter.Y;
			float DisTemp = sqrt(XDis*XDis + YDis * YDis);
			if (DisTemp > 100)
			{
				UpdateSmallRegionCenters.Add(TempSmallRegionCenters[Num]);
			}
		}
		bool MergeP = MergePoints(UpdateSmallRegionCenters, 200, SmallRegionCenters);
		if (!MergeP)
		{
			return false;
		}


	}
	return true;
}

bool  FPolygonAlg::LocateCentersOfDiningOrLivingRegion(const FPoint2DArray InPnts, FVector2D& MaxRegionCenter,
	TArray<FVector2D>& UpdateSmallRegionCenters, TMap<FVector2D, TArray<FVector2D>>&CenterMapLocalRegions)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;

	TArray<FVector2D> OutPnts;
	bool MergeCollinearSuccess = MergeCollinearPoints(InPnts,OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}


	FVector2D SuitableCenter;

	if (OutPnts.Num() <= 2)
	{
		return false;
	}
	else if (OutPnts.Num() == 3)
	{
		MaxRegionCenter = (OutPnts[0] + OutPnts[1] + OutPnts[2]) / 3;
		CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
		return true;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			MaxRegionCenter = IBetterCenter;
			CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			MaxRegionCenter = TriangleCenters[TargetIndex];
			CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
			return true;
		}
	}
	else
	{
		TMap<FVector2D, float> CenterMapRate;
		TMap<FVector2D, float> CenterMapMiddleEdgeL;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				float VerticalDis1 = DisPFrontToPStart * sqrt(1 - Product1 * Product1);
				float VerticalDis2 = DisPNextToPNextNext * sqrt(1 - Product2 * Product2);

				float  TempRatio = 0.0;
				bool FindLocalCenter = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 4.0) && (VerticalDis1 >= 40) && (VerticalDis2 >= 40) && (DisPFrontToPStart >= 80))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					float TempValue1 = DisPStartToPNext / DisPFrontToPStart;
					float TempValue2 = DisPFrontToPStart / DisPStartToPNext;

					if (TempValue1 > TempValue2)
					{
						TempRatio = TempValue1;
					}
					else
					{
						TempRatio = TempValue2;
					}
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PStart]);
					TempPointArray.Add(OutPnts[PNext]);
					float JudgeDis = FVector2D::Distance(OutPnts[PFront], OutPnts[PStart]);
					float JudgeArea = JudgeDis * (FVector2D::Distance(OutPnts[PStart], OutPnts[PNext]));
					if ((JudgeDis >= 80) && (JudgeArea > 100 * 100))
					{
						CenterMapLocalRegions.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					}
					else
					{
						FVector2D ExtendMiddleP = 0.5*(OutPnts[PFront] + ExtendPoint) + 20 * FVector2D(Vec3_x, Vec3_y);
						CenterMapLocalRegions.Add(ExtendMiddleP, TempPointArray);
						TempCenterX = ExtendMiddleP.X;
						TempCenterY = ExtendMiddleP.Y;
					}
					CenterMapMiddleEdgeL.Add(FVector2D(TempCenterX, TempCenterY), DisPStartToPNext);
					FindLocalCenter = true;
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 4.0) && (VerticalDis1 >= 40) && (VerticalDis2 >= 40) && (DisPNextToPNextNext >= 80))
				{

					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					float TempValue1 = DisPStartToPNext / DisPNextToPNextNext;
					float TempValue2 = DisPNextToPNextNext / DisPStartToPNext;
					if (TempValue1 > TempValue2)
					{
						TempRatio = TempValue1;
					}
					else
					{
						TempRatio = TempValue2;
					}
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempPointArray.Add(OutPnts[PNext]);
					TempPointArray.Add(OutPnts[PStart]);
					float JudgeDis = FVector2D::Distance(OutPnts[PNext], OutPnts[PNextNext]);
					float JudgeArea = JudgeDis * (FVector2D::Distance(OutPnts[PStart], OutPnts[PNext]));
					if ((JudgeDis >= 80) && (JudgeArea > 100 * 100))
					{
						CenterMapLocalRegions.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					}
					else
					{
						FVector2D ExtendMiddleP = 0.5*(OutPnts[PNextNext] + ExtendPoint) + 20 * FVector2D(Vec2_x, Vec2_y);
						CenterMapLocalRegions.Add(ExtendMiddleP, TempPointArray);
						TempCenterX = ExtendMiddleP.X;
						TempCenterY = ExtendMiddleP.Y;
					}
					CenterMapMiddleEdgeL.Add(FVector2D(TempCenterX, TempCenterY), DisPStartToPNext);
					FindLocalCenter = true;
				}



				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (FindLocalCenter))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
					CenterMapRate.Add(TempCenter, TempRatio);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}




		float MaxArea = 0.0;
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if ((AreaSet[CenterIndex] != 0.0) && (CenterMapRate[AreaCenters[CenterIndex]] <= 4.0) && (CenterMapMiddleEdgeL[AreaCenters[CenterIndex]] >= 120))
			{
				if (AreaSet[CenterIndex] > MaxArea)
				{
					MaxArea = AreaSet[CenterIndex];
					TargetIndex = CenterIndex;
				}
			}

		}

		MaxRegionCenter = AreaCenters[TargetIndex];
		TArray<FVector2D> TempSmallRegionCenters;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if ((AreaSet[CenterIndex] != MaxArea) && (AreaSet[CenterIndex] != 0))
			{
				TempSmallRegionCenters.Add(AreaCenters[CenterIndex]);
			}
		}

		for (int32 Num = 0; Num < TempSmallRegionCenters.Num(); Num++)
		{
			float XDis = TempSmallRegionCenters[Num].X - MaxRegionCenter.X;
			float YDis = TempSmallRegionCenters[Num].Y - MaxRegionCenter.Y;
			float DisTemp = sqrt(XDis*XDis + YDis * YDis);
			if (DisTemp > 100)
			{
				UpdateSmallRegionCenters.Add(TempSmallRegionCenters[Num]);
			}
		}
	}
	return true;
}


bool FPolygonAlg::LocateCentersOfRoom(const FPoint2DArray InPnts, FVector2D& MaxRegionCenter,
	TArray<FVector2D>& UpdateSmallRegionCenters, TMap<FVector2D, TArray<FVector2D>>&CenterMapLocalRegions)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;
	const float DisBoundTol = 25.0f;
	TArray<FVector2D>   OutPnts;

	bool IsSuccess = FPolygonAlg::MergeCollinearPoints(InPnts, OutPnts);
	if (!IsSuccess)
	{
		return false;
	}

	if (OutPnts.Num() <= 2)
	{
		return false;
	}
	else if (OutPnts.Num() == 3)
	{
		MaxRegionCenter = (OutPnts[0] + OutPnts[1] + OutPnts[2]) / 3;
		CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
		return true;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			MaxRegionCenter = IBetterCenter;
			CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			MaxRegionCenter = TriangleCenters[TargetIndex];
			CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
			return true;
		}
	}
	else
	{
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				bool RegionUseness = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 3))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PStart]);
					TempPointArray.Add(OutPnts[PNext]);
					float JudgeDis = FVector2D::Distance(OutPnts[PFront], OutPnts[PStart]);
					if (JudgeDis > DisBoundTol)
					{
						CenterMapLocalRegions.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
						RegionUseness = true;
					}

				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 3))
				{
					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempPointArray.Add(OutPnts[PNext]);
					TempPointArray.Add(OutPnts[PStart]);
					float JudgeDis = FVector2D::Distance(OutPnts[PNext], OutPnts[PNextNext]);
					if (JudgeDis > DisBoundTol)
					{
						CenterMapLocalRegions.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
						RegionUseness = true;
					}

				}



				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (RegionUseness))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);

				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = AreaSet[0];
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 1; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] > MaxArea)
			{
				MaxArea = AreaSet[CenterIndex];
				TargetIndex = CenterIndex;
			}
		}
		if (AreaSet[TargetIndex] >= 150 * 100)
		{
			MaxRegionCenter = AreaCenters[TargetIndex];
		}
		else
		{
			float MinX = OutPnts[0].X;
			float MaxX = OutPnts[0].X;
			float MinY = OutPnts[0].Y;
			float MaxY = OutPnts[0].Y;
			for (int _CurP = 0; _CurP < OutPnts.Num(); ++_CurP)
			{
				if (MinX > OutPnts[_CurP].X)
				{
					MinX = OutPnts[_CurP].X;
				}
				if (MaxX < OutPnts[_CurP].X)
				{
					MaxX = OutPnts[_CurP].X;
				}
				if (MinY > OutPnts[_CurP].Y)
				{
					MinY = OutPnts[_CurP].Y;
				}
				if (MaxY < OutPnts[_CurP].Y)
				{
					MaxY = OutPnts[_CurP].Y;
				}
			}
			FVector2D PLeftUp = FVector2D(MinX, MaxY);
			FVector2D PLeftDown = FVector2D(MinX, MinY);
			FVector2D PRightDown = FVector2D(MaxX, MinY);
			FVector2D PRightUp = FVector2D(MaxX, MaxY);
			FVector2D TempCenter = 0.25*(PLeftUp + PLeftDown + PRightDown + PRightUp);
			if (FPolygonAlg::JudgePointInPolygon(OutPnts, TempCenter))
			{
				MaxRegionCenter = TempCenter;
				MaxArea = abs(FPolygonAlg::GetAreaOfRegion(OutPnts));
				CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
			}

		}


		if (!Region2D.bounded_side(Point_2(MaxRegionCenter.X, MaxRegionCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			MaxRegionCenter = TriangleCenters[TargetIndex];
		}

		const float DisBoundOfRoom = 70;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if ((AreaSet[CenterIndex] != MaxArea) && (AreaSet[CenterIndex] != 0))
			{
				float DisToMaxRegionCenter = FVector2D::Distance(AreaCenters[CenterIndex], MaxRegionCenter);
				if (DisToMaxRegionCenter > DisBoundOfRoom)
				{
					UpdateSmallRegionCenters.Add(AreaCenters[CenterIndex]);
				}

			}
		}
	}
	return true;
}



bool FPolygonAlg::LocateCenterOfKitchen(const TArray<FVector2D> InPnts, FVector2D& MaxRegionCenter)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;
	TArray<FVector2D>  TempPoints, OutPnts;

	bool IsSuccess = FPolygonAlg::MergeCollinearPoints(InPnts, TempPoints);
	if (!IsSuccess)
	{
		return false;
	}
	bool MergeShortSuccess = FPolygonAlg::MergeShortEdges(TempPoints, OutPnts);
	if (!MergeShortSuccess)
	{
		return false;
	}


	if (OutPnts.Num() <= 2)
	{
		return false;
	}
	else if (OutPnts.Num() == 3)
	{
		MaxRegionCenter = (OutPnts[0] + OutPnts[1] + OutPnts[2]) / 3;
		return true;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			MaxRegionCenter = IBetterCenter;
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			MaxRegionCenter = TriangleCenters[TargetIndex];
			return true;
		}
	}
	else
	{
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				bool RegionUseness = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 3))
				{
					float JudgeDis = FVector2D::Distance(OutPnts[PFront], OutPnts[PStart]);
					if (JudgeDis >= 145)
					{
						TempCenterX = 0.5*(P4.x() + P2.x());
						TempCenterY = 0.5*(P4.y() + P2.y());

					}
					else
					{
						TempCenterX = 0.5*(P1.x() + P2.x());
						TempCenterY = 0.5*(P1.y() + P2.y());
						TempCenterX = TempCenterX + 100 * Vec3_x;
						TempCenterY = TempCenterY + 100 * Vec3_y;
					}
					TempArea = DisPFrontToPStart * DisPStartToPNext;

				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 3))
				{
					float JudgeDis = FVector2D::Distance(OutPnts[PNext], OutPnts[PNextNext]);
					if (JudgeDis >= 145)
					{
						TempCenterX = 0.5*(P3.x() + P1.x());
						TempCenterY = 0.5*(P3.y() + P1.y());
					}
					else
					{
						TempCenterX = 0.5*(P1.x() + P2.x());
						TempCenterY = 0.5*(P1.y() + P2.y());
						TempCenterX = TempCenterX + 100 * Vec2_x;
						TempCenterY = TempCenterY + 100 * Vec2_y;
					}
					TempArea = DisPNextToPNextNext * DisPStartToPNext;

				}



				if (Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE)
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);

				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = AreaSet[0];
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 1; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] > MaxArea)
			{
				MaxArea = AreaSet[CenterIndex];
				TargetIndex = CenterIndex;
			}
		}

		MaxRegionCenter = AreaCenters[TargetIndex];

		if (!Region2D.bounded_side(Point_2(MaxRegionCenter.X, MaxRegionCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			MaxRegionCenter = TriangleCenters[TargetIndex];
		}

	}
	return true;
}

bool FPolygonAlg::MergePoints(const FPoint2DArray InPnts, const float DisBound, TArray<FVector2D>& RegionCenters)
{
	using namespace Triangulation;


	TArray<FVector2D>Centers;
	for (int32 Index = 0; Index < InPnts.Num(); Index++)
	{
		Centers.Add(InPnts[Index]);
	}

	while (Centers.Num() > 0)
	{


		for (int32 Index = 0; Index < Centers.Num(); Index++)
		{
			TArray<FVector2D> TempCenters;
			TempCenters.Add(Centers[Index]);
			FVector2D TempP = Centers[Index];
			Centers.Remove(TempP);
			for (int32 NextIndex = 0; NextIndex < Centers.Num(); NextIndex++)
			{
				float XDiffer = TempP.X - Centers[NextIndex].X;
				float YDiffer = TempP.Y - Centers[NextIndex].Y;
				float Dis = sqrt(XDiffer*XDiffer + YDiffer * YDiffer);
				if (Dis < DisBound)
				{
					FVector2D Temp = Centers[NextIndex];
					Centers.Remove(Temp);
					TempCenters.Add(Temp);
				}
			}
			if (TempCenters.Num() == 1)
			{
				RegionCenters.Add(TempCenters[0]);
			}
			else if (TempCenters.Num() == 2)
			{
				float AveX = 0.0, AveY = 0.0;
				AveX = TempCenters[0].X + TempCenters[1].X;
				AveY = TempCenters[0].Y + TempCenters[1].Y;
				AveX = AveX / 2;
				AveY = AveY / 2;
				AveX = AveX + 0.5*(abs(TempCenters[1].X - TempCenters[0].X));
				AveY = AveY + 0.5*(abs(TempCenters[1].Y - TempCenters[0].Y));
				RegionCenters.Add(FVector2D(AveX, AveY));
			}
			else
			{

				float AveX = 0.0, AveY = 0.0;
				for (int32 PIndex = 0; PIndex < TempCenters.Num(); PIndex++)
				{
					AveX = AveX + TempCenters[PIndex].X;
					AveY = AveY + TempCenters[PIndex].Y;
				}

				RegionCenters.Add(FVector2D(AveX / TempCenters.Num(), AveY / TempCenters.Num()));
			}
		}
	}

	return true;
}

bool FPolygonAlg::MergeShrotEdgeCmSize(const FPoint2DArray InPnts, FPoint2DArray& OutPnts)
{
	const int32 ShortEdgeLengthLimit = 20;
	if (InPnts.Num() <= 2)
	{
		return false;
	}

	FPoint2DArray TempOutPnts;
	TArray<int32> RemoveIndexs;
	if (InPnts.Num() > 4)
	{
		int32 PNextIndex = 0;
		for (int32 Index = 0; Index < InPnts.Num(); Index++)
		{
			if (Index == InPnts.Num() - 1)
			{
				PNextIndex = 0;
			}
			else
			{
				PNextIndex = Index + 1;
			}
			float XDiffer = InPnts[Index].X - InPnts[PNextIndex].X;
			float YDiffer = InPnts[Index].Y - InPnts[PNextIndex].Y;
			float EdgeL = sqrt(XDiffer*XDiffer + YDiffer * YDiffer);
			if (EdgeL <= ShortEdgeLengthLimit)
			{
				RemoveIndexs.AddUnique(Index);
				RemoveIndexs.AddUnique(PNextIndex);
			}
		}
	}
	for (int J = 0; J <= InPnts.Num() - 1; ++J)
	{
		if (!RemoveIndexs.Contains(J))
		{
			TempOutPnts.Add(InPnts[J]);
		}
	}
	if (TempOutPnts.Num() < 4)
	{
		OutPnts = InPnts;
	}
	else
	{
		OutPnts = TempOutPnts;
	}
	return true;
}


void FPolygonAlg::PolygonTriangulate(const FPoint2DArray InPnts, TArray<int32>&TriIndexs)
{
	using namespace Triangulation;
	CDTDS Mycdt;
	Polygon_2 Region2D;
	for (int32 Index = 0; Index < InPnts.Num(); Index++)
	{
		Region2D.push_back(Point_2(InPnts[Index].X, InPnts[Index].Y));
	}
	float Area = Region2D.area();
	if (Area < 0)
	{
		Region2D.reverse_orientation();
	}
	TArray<Vertex_handle> AllHandles;
	for (int32 Index = 0; Index < InPnts.Num(); Index++)
	{
		Vertex_handle TempV = Mycdt.insert(Point_2(InPnts[Index].X, InPnts[Index].Y));
		AllHandles.Add(TempV);
	}
	for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
	{
		if (JIndex == AllHandles.Num() - 1)
		{
			Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
		}
		else
		{
			Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
		}
	}
	Mesher mesher(Mycdt);

	TArray<Triangle_2> TriangleSet;
	for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
	{

		FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
		FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
		FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
		Triangle_2 Tri(Mycdt.triangle(Fbegin));
		Point_2 Center(centroid(Tri));
		if (Region2D.has_on_bounded_side(Center))
		{
			TriangleSet.Add(Tri);
		}
	}
	for (int32 Index = 0; Index < TriangleSet.Num(); Index++)
	{
		for (int32 J = 0; J < 3; J++)
		{
			for (int32 Pindex = 0; Pindex < InPnts.Num(); Pindex++)
			{
				FVector2D TempPoint(TriangleSet[Index].vertex(J).x(), TriangleSet[Index].vertex(J).y());
				if (TempPoint.Equals(InPnts[Pindex]))
				{
					TriIndexs.Add(Pindex);
					break;
				}
			}
		}
	}
}


bool FPolygonAlg::MesherPolygonToGetCenter(const FPoint2DArray InPnts, FVector2D& Center)
{
	if (InPnts.Num() == 0)
	{
		return false;
	}
	using namespace Triangulation;
	CDTDS Mycdt;
	Polygon_2 Region2D;
	for (int32 Index = 0; Index < InPnts.Num(); Index++)
	{
		Region2D.push_back(Point_2(InPnts[Index].X, InPnts[Index].Y));
	}
	if (Region2D.area() < 0)
	{
		Region2D.reverse_orientation();
	}
	TArray<Vertex_handle> AllHandles;
	//for (int32 Index = 0; Index < InPnts.Num(); Index++)
	//{
	//	Vertex_handle TempV = Mycdt.insert(Point_2(InPnts[Index].X, InPnts[Index].Y));
	//	AllHandles.Add(TempV);
	//}

	//TArray<FVector2D> NewPs;
	for (auto Vbegin = Region2D.vertices_begin(), Vend = Region2D.vertices_end(); Vbegin != Vend; ++Vbegin)
	{
		//NewPs.Add(FVector2D((*Vbegin).x(), (*Vbegin).y()));
		Vertex_handle TempV = Mycdt.insert(*Vbegin);
		AllHandles.Add(TempV);
	}


	for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
	{
		if (JIndex == AllHandles.Num() - 1)
		{
			Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
		}
		else
		{
			Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
		}
	}
	Mesher mesher(Mycdt);
	TArray<Triangle_2> TriangleSet;
	TArray<float> AreaSet;
	TArray<FVector2D> TriangleCenters;
	for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
	{
		FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
		FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
		FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
		Triangle_2 Tri(Mycdt.triangle(Fbegin));
		Point_2 Center(centroid(Tri));
		AreaSet.Add(abs(Tri.area()));
		TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
	}

	float MaxArea = AreaSet[0];
	int32 TargetIndex = 0;
	for (int32 Index = 1; Index < AreaSet.Num(); Index++)
	{
		if (MaxArea < AreaSet[Index])
		{
			MaxArea = AreaSet[Index];
			TargetIndex = Index;
		}
	}
	Center = TriangleCenters[TargetIndex];

	return true;
}


bool FPolygonAlg::GetAllInterSectPs(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>&UpdatePolygonA, \
	TArray<FVector2D>& UpdatePolygonB, TArray<FVector2D>& InterSectPs)
{
	bool MergePolygonA = FPolygonAlg::StricMergeCollinear(PolygonFirst, UpdatePolygonA);
	if (!MergePolygonA)
	{
		return false;
	}
	bool MergePolygonB = FPolygonAlg::StricMergeCollinear(PolygonSecond, UpdatePolygonB);
	if (!MergePolygonB)
	{
		return false;
	}
	/////计算UpdatePolygonA上每一点到UpdatePolygonB线段的距离
	TArray<FVector2D> TempInterSectPs, AInterSegPs;
	int32 NextJ = 0;
	for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
	{
		for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
		{
			if (J == UpdatePolygonB.Num() - 1)
			{
				NextJ = 0;
			}
			else
			{
				NextJ = J + 1;
			}
			//}
			float DisToSeg = FMath::PointDistToSegment(FVector(UpdatePolygonA[I], 0), FVector(UpdatePolygonB[J], 0), FVector(UpdatePolygonB[NextJ], 0));
			if (5 >= DisToSeg)
			{
				TempInterSectPs.AddUnique(UpdatePolygonA[I]);
				AInterSegPs.AddUnique(UpdatePolygonA[I]);
			}
		}
	}

	////////计算UpdatePolygonB上每一点到UpdatePolygonA线段的距离
	for (int32 I = 0; I < UpdatePolygonB.Num(); I++)
	{
		for (int32 J = 0; J < UpdatePolygonA.Num(); J++)
		{
			if (J == UpdatePolygonA.Num() - 1)
			{
				NextJ = 0;
			}
			else
			{
				NextJ = J + 1;
			}

			float DisToSeg = FMath::PointDistToSegment(FVector(UpdatePolygonB[I], 0), FVector(UpdatePolygonA[J], 0), FVector(UpdatePolygonA[NextJ], 0));
			if (5 >= DisToSeg)
			{
				int32 TempCount = 0;
				for (int32 K = 0; K < AInterSegPs.Num(); K++)
				{
					float TempDis = FVector2D::Distance(AInterSegPs[K], UpdatePolygonB[I]);
					if (TempDis > 3.6)
					{
						TempCount++;
					}
				}
				if (TempCount == AInterSegPs.Num())
				{
					TempInterSectPs.AddUnique(UpdatePolygonB[I]);
				}
			}
		}
	}
	if (TempInterSectPs.Num() > 0)
	{
		InterSectPs.AddUnique(TempInterSectPs[0]);
		TempInterSectPs.RemoveAt(0);
	}


	int32 TargetIndexToRemove = 0;
	TArray<int32> ToClearIndexs;
	const int ControlSteps = 1000;
	int CountStep = 0;
	while (ToClearIndexs.Num() != TempInterSectPs.Num())
	{
		CountStep++;
		for (int32 I = 0; I < TempInterSectPs.Num(); I++)
		{
			int32 TempCount = 0;
			for (int32 J = 0; J < InterSectPs.Num(); J++)
			{
				if (!InterSectPs[J].Equals(TempInterSectPs[I]))
				{
					TempCount++;
				}
				else
				{
					ToClearIndexs.AddUnique(I);
				}
			}
			if (TempCount == InterSectPs.Num())
			{
				InterSectPs.AddUnique(TempInterSectPs[I]);
				TargetIndexToRemove = I;
				ToClearIndexs.AddUnique(I);
				break;
			}
		}
		if (CountStep >= ControlSteps)
		{
			return false;
		}

	}
	if (InterSectPs.Num() == 0)
	{
		return false;
	}
	return true;
}




bool FPolygonAlg::CreateVirtualWallPoints(const TArray<FVector2D> InterSectPs, TArray<FVector2D>& VirtualWallPs)
{
	if (InterSectPs.Num() == 2)
	{
		VirtualWallPs = InterSectPs;
	}
	else if (InterSectPs.Num() == 3)
	{
		FVector2D Vec0 = InterSectPs[0] - InterSectPs[1];
		FVector2D Vec1 = InterSectPs[2] - InterSectPs[1];
		FVector2D Vec2 = InterSectPs[1] - InterSectPs[0];
		FVector2D Vec3 = InterSectPs[2] - InterSectPs[0];

		FVector2D NewVec0 = Vec0.GetSafeNormal();
		FVector2D NewVec1 = Vec1.GetSafeNormal();
		FVector2D NewVec2 = Vec2.GetSafeNormal();
		FVector2D NewVec3 = Vec3.GetSafeNormal();

		////case 1:P1在中间
		float FValue1 = FMath::Abs(FVector2D::DotProduct(NewVec0, NewVec1));
		float FValue2 = FMath::Abs(FVector2D::DotProduct(NewVec2, NewVec3));
		float FValue3 = FMath::Abs(FVector2D::DotProduct(NewVec1, NewVec3));
		if (FValue1<= 0.01)
		{
			VirtualWallPs.Add(InterSectPs[0]);
			VirtualWallPs.Add(InterSectPs[1]);
			VirtualWallPs.Add(InterSectPs[2]);
		}
		////case 2:P0在中间
		else if (FValue2 <= 0.01)
		{
			VirtualWallPs.Add(InterSectPs[1]);
			VirtualWallPs.Add(InterSectPs[0]);
			VirtualWallPs.Add(InterSectPs[2]);

		}
		////case3:P2在中间
		else if (FValue3 <= 0.01)
		{
			VirtualWallPs.Add(InterSectPs[0]);
			VirtualWallPs.Add(InterSectPs[2]);
			VirtualWallPs.Add(InterSectPs[1]);
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool FPolygonAlg::CreateActualVirtualWallPoints(const TArray<FVector2D> FirstPolygon, const TArray<FVector2D> SecondPolygon, const TArray<FVector2D> InterSectPs, TArray<FVector2D>& VirtualWallPs)
{
	if (InterSectPs.Num() == 2)
	{
		VirtualWallPs = InterSectPs;
	}
	else if (InterSectPs.Num() == 3)
	{
		const float TestExtendL = 1.5f;
		FVector2D Vec0 = InterSectPs[0] - InterSectPs[1];
		FVector2D Middle0 = 0.5*(InterSectPs[0] + InterSectPs[1]);

		FVector2D Vec1 = InterSectPs[2] - InterSectPs[1];
		FVector2D Middle1 = 0.5*(InterSectPs[1] + InterSectPs[2]);

		FVector2D Vec2 = InterSectPs[2] - InterSectPs[0];
		FVector2D Middle2 = 0.5*(InterSectPs[0] + InterSectPs[2]);
		/////////////判定三点之间的真实边界//////////////////
		FVector2D NewVec0 = Vec0.GetSafeNormal();
		FVector2D NewVec1 = Vec1.GetSafeNormal();
		FVector2D NewVec2 = Vec2.GetSafeNormal();

		FVector2D RotatedDir0 = NewVec0.GetRotated(90);
		FVector2D RotatedDir1 = NewVec1.GetRotated(90);
		FVector2D RotatedDir2 = NewVec2.GetRotated(90);

		FVector2D TestNode0 = Middle0 + TestExtendL * RotatedDir0;
		FVector2D OppoNodeOfTestNode0 = Middle0 - TestExtendL * RotatedDir0;

		FVector2D TestNode1 = Middle1 + TestExtendL * RotatedDir1;
		FVector2D OppoNodeOfTestNode1 = Middle1 - TestExtendL * RotatedDir1;

		FVector2D TestNode2 = Middle2 + TestExtendL * RotatedDir2;
		FVector2D OppoNodeOfTestNode2 = Middle2 - TestExtendL * RotatedDir2;

		TArray<FVector2D> CollectSegNodes;

		bool IsNode0InPoly1 = FPolygonAlg::JudgePointInPolygon(FirstPolygon, TestNode0);
		bool IsOppoNode0InPoly2 = FPolygonAlg::JudgePointInPolygon(SecondPolygon, OppoNodeOfTestNode0);

		bool IsNode0InPoly2 = FPolygonAlg::JudgePointInPolygon(SecondPolygon, TestNode0);
		bool IsOppoNode0InPoly1 = FPolygonAlg::JudgePointInPolygon(FirstPolygon, OppoNodeOfTestNode0);

		if (((IsNode0InPoly1) && (IsOppoNode0InPoly2)) || ((IsNode0InPoly2) && (IsOppoNode0InPoly1)))
		{
			CollectSegNodes.Add(InterSectPs[0]);
			CollectSegNodes.Add(InterSectPs[1]);
		}

		bool IsNode1InPoly1 = FPolygonAlg::JudgePointInPolygon(FirstPolygon, TestNode1);
		bool IsOppoNode1InPoly2 = FPolygonAlg::JudgePointInPolygon(SecondPolygon, OppoNodeOfTestNode1);

		bool IsNode1InPoly2 = FPolygonAlg::JudgePointInPolygon(SecondPolygon, TestNode1);
		bool IsOppoNode1InPoly1 = FPolygonAlg::JudgePointInPolygon(FirstPolygon, OppoNodeOfTestNode1);

		if (((IsNode1InPoly1) && (IsOppoNode1InPoly2)) || ((IsNode1InPoly2) && (IsOppoNode1InPoly1)))
		{
			CollectSegNodes.Add(InterSectPs[1]);
			CollectSegNodes.Add(InterSectPs[2]);
		}

		bool IsNode2InPoly1 = FPolygonAlg::JudgePointInPolygon(FirstPolygon, TestNode2);
		bool IsOppoNode2InPoly2 = FPolygonAlg::JudgePointInPolygon(SecondPolygon, OppoNodeOfTestNode2);

		bool IsNode2InPoly2 = FPolygonAlg::JudgePointInPolygon(SecondPolygon, TestNode2);
		bool IsOppoNode2InPoly1 = FPolygonAlg::JudgePointInPolygon(FirstPolygon, OppoNodeOfTestNode2);

		if (((IsNode2InPoly1) && (IsOppoNode2InPoly2)) || ((IsNode2InPoly2) && (IsOppoNode2InPoly1)))
		{
			CollectSegNodes.Add(InterSectPs[0]);
			CollectSegNodes.Add(InterSectPs[2]);
		}

		int TargetMiddleIndex = 0;
		for (int _CurP = 0; _CurP < InterSectPs.Num(); ++_CurP)
		{
			int CountNum = 0;
			for (int _CurJ = 0; _CurJ < CollectSegNodes.Num(); ++_CurJ)
			{
				if (CollectSegNodes[_CurJ].Equals(InterSectPs[_CurP]))
				{
					CountNum++;
				}
			}
			if (CountNum == 2)
			{
				TargetMiddleIndex = _CurP;
			}
		}

		if (TargetMiddleIndex == 0)
		{
			VirtualWallPs.Add(InterSectPs[1]);
			VirtualWallPs.Add(InterSectPs[0]);
			VirtualWallPs.Add(InterSectPs[2]);
		}
		else if (TargetMiddleIndex == 1)
		{
			VirtualWallPs = InterSectPs;
		}
		else if (TargetMiddleIndex == 2)
		{
			VirtualWallPs.Add(InterSectPs[0]);
			VirtualWallPs.Add(InterSectPs[2]);
			VirtualWallPs.Add(InterSectPs[1]);
		}
	}
	else
	{
		return false;
	}
	return true;
}
bool FPolygonAlg::CreatePolygonUnion(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>&NewPolygonFirst, 
	TArray<FVector2D>&NewPolygonSecond, TArray<FVector2D>&PolygonUnion)
{
	using namespace Triangulation;
	const int MaxNumToL = 1000;
	const float PointToL = 0.001;
	int StepCount = 0;
	TArray<FVector2D> UpdatePolygonA, UpdatePolygonB, InterSectPs;
	bool InterSectSuccess = GetAllInterSectPs(PolygonFirst, PolygonSecond, UpdatePolygonA, UpdatePolygonB, InterSectPs);
	if (!InterSectSuccess)
	{
		NewPolygonFirst = UpdatePolygonA;
		NewPolygonSecond = UpdatePolygonB;
		return false;
	}

	TArray<FVector2D> VirtualWallPs;
	//bool CreateVirtual = CreateVirtualWallPoints(InterSectPs, VirtualWallPs);
	bool CreateVirtual=CreateActualVirtualWallPoints(UpdatePolygonA,UpdatePolygonB,InterSectPs,VirtualWallPs);
	if (!CreateVirtual)
	{
		return false;
	}
	for (int32 I = 0; I < VirtualWallPs.Num(); I++)
	{
		for (int32 J = 0; J < UpdatePolygonA.Num(); J++)
		{
			if (UpdatePolygonA[J].Equals(VirtualWallPs[I], PointToL))
			{
				UpdatePolygonA[J] = VirtualWallPs[I];
				break;
			}
		}
	}

	for (int32 I = 0; I < VirtualWallPs.Num(); I++)
	{
		for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
		{
			if (UpdatePolygonB[J].Equals(VirtualWallPs[I], PointToL))
			{
				UpdatePolygonB[J] = VirtualWallPs[I];
				break;
			}
		}
	}

	FPolygonAlg::CGPolygonCombine(UpdatePolygonA, UpdatePolygonB, PolygonUnion);
	float S1 = FPolygonAlg::GetAreaOfRegion(UpdatePolygonA);
	float S2 = FPolygonAlg::GetAreaOfRegion(UpdatePolygonB);
	float S3 = FPolygonAlg::GetAreaOfRegion(PolygonUnion);
	float AreaDiffer = abs(S1 + S2 - S3);
	float DifferRatio = AreaDiffer / (S1 + S2);
	bool IsCombinedUseful = true;
	if (DifferRatio >= 0.1)
	{
		IsCombinedUseful = false;
	}

	if (!IsCombinedUseful)
	{
		PolygonUnion.Empty();
		const float ExtendL = 1.5;
		if (VirtualWallPs.Num() == 2)
		{

			int32 LyingACount = 0, LyingBCount = 0;
			for (int32 VWallPIndex = 0; VWallPIndex < VirtualWallPs.Num(); VWallPIndex++)
			{
				for (int32 APIndex = 0; APIndex < UpdatePolygonA.Num(); APIndex++)
				{
					if (VirtualWallPs[VWallPIndex].Equals(UpdatePolygonA[APIndex]))
					{
						UpdatePolygonA[APIndex] = VirtualWallPs[VWallPIndex];
						LyingACount++;
						break;
					}
				}
			}

			int32 FinialBTargetIndex = 0, FinialTargetVWallIndex = 0;
			for (int32 VWallPIndex = 0; VWallPIndex < VirtualWallPs.Num(); VWallPIndex++)
			{
				for (int32 BPIndex = 0; BPIndex < UpdatePolygonB.Num(); BPIndex++)
				{
					if (VirtualWallPs[VWallPIndex].Equals(UpdatePolygonB[BPIndex], 3.6))
					{
						LyingBCount++;
						FinialBTargetIndex = BPIndex;
						FinialTargetVWallIndex = VWallPIndex;
						if (VirtualWallPs[VWallPIndex].Equals(UpdatePolygonB[BPIndex]))
						{
							UpdatePolygonB[BPIndex] = VirtualWallPs[VWallPIndex];
						}
					}

				}
			}

			if (LyingBCount == 1)
			{
				if (!VirtualWallPs.Contains(UpdatePolygonB[FinialBTargetIndex]))
				{
					UpdatePolygonB[FinialBTargetIndex] = VirtualWallPs[FinialTargetVWallIndex];
				}
			}

			//////两交点都在A上
			if ((LyingACount == 2) && (LyingBCount == 0))
			{
				Segment_2 InterSegment(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				int32 NextI = 0;
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}
				/////B边界包含A边界
				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}

				int32 RemoveBSegIndex = 0;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment.target().x() - InterSegment.source().x(), InterSegment.target().y() - InterSegment.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						float Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							break;
						}

					}
				}
				/////两交点在B边界线段内部时
				Point_2 BRemoveSegStart = BSegs[RemoveBSegIndex].source();
				Point_2 BRemoveSegEnd = BSegs[RemoveBSegIndex].target();

				Point_2 InterSectPStart = InterSegment.source();
				Point_2 InterSectPEnd = InterSegment.target();
				FVector2D TempVec1 = FVector2D(BRemoveSegStart.x() - VirtualWallPs[0].X, BRemoveSegStart.y() - VirtualWallPs[0].Y);
				FVector2D TempVec2 = FVector2D(BRemoveSegEnd.x() - VirtualWallPs[0].X, BRemoveSegEnd.y() - VirtualWallPs[0].Y);

				FVector2D TempVec3 = FVector2D(BRemoveSegStart.x() - VirtualWallPs[1].X, BRemoveSegStart.y() - VirtualWallPs[1].Y);
				FVector2D TempVec4 = FVector2D(BRemoveSegEnd.x() - VirtualWallPs[1].X, BRemoveSegEnd.y() - VirtualWallPs[1].Y);

				float Cross1 = FVector2D::DotProduct(TempVec1, TempVec2);
				float Cross2 = FVector2D::DotProduct(TempVec3, TempVec4);

				if ((Cross1 < 0) && (Cross2 < 0))
				{
					float Dis1 = CGAL::squared_distance(BRemoveSegStart, InterSectPStart);
					float Dis2 = CGAL::squared_distance(BRemoveSegStart, InterSectPEnd);
					if (Dis1 < Dis2)
					{
						Segment_2 AddSeg1(BRemoveSegStart, InterSectPStart);
						Segment_2 AddSeg2(BRemoveSegEnd, InterSectPEnd);
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg1);
						BSegs.Add(AddSeg2);
					}
					else
					{
						Segment_2 AddSeg1(BRemoveSegStart, InterSectPEnd);
						Segment_2 AddSeg2(BRemoveSegEnd, InterSectPStart);
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg1);
						BSegs.Add(AddSeg2);
					}
				}
				//////update ASegs
				int32 ARemoveIndex = 0;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					if (((ASegs[ASegIndex].source() == InterSectPStart) && (ASegs[ASegIndex].target() == InterSectPEnd)) ||
						((ASegs[ASegIndex].source() == InterSectPEnd) && (ASegs[ASegIndex].target() == InterSectPStart)))
					{
						ARemoveIndex = ASegIndex;
						break;
					}

				}
				ASegs.RemoveAt(ARemoveIndex);
				//////Reorder ASegs
				TArray<Point_2> AReorderSegPoints;
				TMap<int32, Point_2> InterSectPsMapTailPs;
				AReorderSegPoints.Add(InterSectPStart);

				TArray<int32> SegAddIndexs;
				while (SegAddIndexs.Num() != ASegs.Num())
				{
					StepCount++;
					for (int32 I = 0; I < ASegs.Num(); I++)
					{
						if (!SegAddIndexs.Contains(I))
						{
							float APointDis1 = sqrt(CGAL::squared_distance(ASegs[I].source(), AReorderSegPoints.Last()));
							float APointDis2 = sqrt(CGAL::squared_distance(ASegs[I].target(), AReorderSegPoints.Last()));
							if (APointDis1 <= 0.1)
							{
								AReorderSegPoints.Add(ASegs[I].target());
								SegAddIndexs.AddUnique(I);
							}
							else if (APointDis2 <= 0.1)
							{
								AReorderSegPoints.Add(ASegs[I].source());
								SegAddIndexs.AddUnique(I);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
				TailFirstVec.Normalize();
				Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(0, TailFirstP);

				FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
				TailSecondVec.Normalize();
				Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(1, TailSecondP);

				////Reorder BSegs
				TArray<Point_2> BReorderSegPoints;
				BReorderSegPoints.Add(InterSectPEnd);
				TArray<int32> BSegAddIndexs;
				while (BSegAddIndexs.Num() != BSegs.Num())
				{
					StepCount++;
					for (int32 J = 0; J < BSegs.Num(); J++)
					{
						if (!BSegAddIndexs.Contains(J))
						{
							float BPointDis1 = sqrt(CGAL::squared_distance(BSegs[J].source(), BReorderSegPoints.Last()));
							float BPointDis2 = sqrt(CGAL::squared_distance(BSegs[J].target(), BReorderSegPoints.Last()));
							if (BPointDis1 <= 0.1)
							{
								BReorderSegPoints.Add(BSegs[J].target());
								BSegAddIndexs.AddUnique(J);
							}
							else if (BPointDis2 <= 0.1)
							{
								BReorderSegPoints.Add(BSegs[J].source());
								BSegAddIndexs.AddUnique(J);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
				{
					NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
				}

				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
				for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
				{
					NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
				}
				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));
			}
			//////A边上1个交点，B边上1个交点
			else if ((LyingACount == 1) && (LyingBCount == 1))
			{
				Segment_2 InterSegment(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				int32 NextI = 0;
				////构建A和B的所有初始边的集合
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}

				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}

				/////找出A上的交点在B的哪条线段上
				int32 RemoveBSegIndex = 0;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment.target().x() - InterSegment.source().x(), InterSegment.target().y() - InterSegment.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.l)
						{
							RemoveBSegIndex = BSegIndex;
							break;
						}

					}
				}
				////找出B上的交点在A的哪条线段上
				int32 RemoveASegIndex = 0;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment.target().x() - InterSegment.source().x(), InterSegment.target().y() - InterSegment.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.1)
						{
							RemoveASegIndex = ASegIndex;
							break;
						}

					}
				}

				/////重新构建新的线段
				Point_2 BRemoveSegStart = BSegs[RemoveBSegIndex].source();
				Point_2 BRemoveSegEnd = BSegs[RemoveBSegIndex].target();

				Point_2 InterSectPStart = InterSegment.source();
				Point_2 InterSectPEnd = InterSegment.target();

				////构建A新的线段
				float DisInterSeg = CGAL::squared_distance(InterSectPStart, InterSectPEnd);
				float DisARemoveSeg = CGAL::squared_distance(ASegs[RemoveASegIndex].source(), ASegs[RemoveASegIndex].target());
				float DisBRemoveSeg = CGAL::squared_distance(BSegs[RemoveBSegIndex].source(), BSegs[RemoveBSegIndex].target());

				if ((DisInterSeg <= DisARemoveSeg) && (DisInterSeg <= DisBRemoveSeg) && (DisARemoveSeg != DisBRemoveSeg))
				{
					if (ASegs[RemoveASegIndex].source() == InterSectPStart)
					{
						Segment_2 AddSeg1(InterSectPEnd, ASegs[RemoveASegIndex].target());
						ASegs.RemoveAt(RemoveASegIndex);
						ASegs.Add(AddSeg1);
					}
					else if (ASegs[RemoveASegIndex].source() == InterSectPEnd)
					{
						Segment_2 AddSeg1(InterSectPStart, ASegs[RemoveASegIndex].target());
						ASegs.RemoveAt(RemoveASegIndex);
						ASegs.Add(AddSeg1);
					}
					else if (ASegs[RemoveASegIndex].target() == InterSectPStart)
					{
						Segment_2 AddSeg1(InterSectPEnd, ASegs[RemoveASegIndex].source());
						ASegs.RemoveAt(RemoveASegIndex);
						ASegs.Add(AddSeg1);
					}
					else if (ASegs[RemoveASegIndex].target() == InterSectPEnd)
					{
						Segment_2 AddSeg1(InterSectPStart, ASegs[RemoveASegIndex].source());
						ASegs.RemoveAt(RemoveASegIndex);
						ASegs.Add(AddSeg1);
					}

					if (BSegs[RemoveBSegIndex].source() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, BSegs[RemoveBSegIndex].target());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].source() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, BSegs[RemoveBSegIndex].target());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].target() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, BSegs[RemoveBSegIndex].source());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].target() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, BSegs[RemoveBSegIndex].source());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
				}

				//////Reorder ASegs
				TArray<Point_2> AReorderSegPoints;
				TMap<int32, Point_2> InterSectPsMapTailPs;
				AReorderSegPoints.Add(InterSectPStart);

				TArray<int32> SegAddIndexs;
				while (SegAddIndexs.Num() != ASegs.Num())
				{
					StepCount++;
					for (int32 I = 0; I < ASegs.Num(); I++)
					{
						if (!SegAddIndexs.Contains(I))
						{
							float APointDis1 = sqrt(CGAL::squared_distance(ASegs[I].source(), AReorderSegPoints.Last()));
							float APointDis2 = sqrt(CGAL::squared_distance(ASegs[I].target(), AReorderSegPoints.Last()));
							if (APointDis1 <= 0.1)
							{
								AReorderSegPoints.Add(ASegs[I].target());
								SegAddIndexs.AddUnique(I);
							}
							else if (APointDis2 <= 0.1)
							{
								AReorderSegPoints.Add(ASegs[I].source());
								SegAddIndexs.AddUnique(I);
							}
						}
					}

					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
				TailFirstVec.Normalize();
				Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(0, TailFirstP);

				FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
				TailSecondVec.Normalize();
				Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(1, TailSecondP);

				////Reorder BSegs
				TArray<Point_2> BReorderSegPoints;
				BReorderSegPoints.Add(InterSectPEnd);
				TArray<int32> BSegAddIndexs;
				while (BSegAddIndexs.Num() != BSegs.Num())
				{
					StepCount++;
					for (int32 J = 0; J < BSegs.Num(); J++)
					{
						if (!BSegAddIndexs.Contains(J))
						{
							float BPointDis1 = sqrt(CGAL::squared_distance(BSegs[J].source(), BReorderSegPoints.Last()));
							float BPointDis2 = sqrt(CGAL::squared_distance(BSegs[J].target(), BReorderSegPoints.Last()));
							if (BPointDis1 <= 0.1)
							{
								BReorderSegPoints.Add(BSegs[J].target());
								BSegAddIndexs.AddUnique(J);
							}
							else if (BPointDis2 <= 0.1)
							{
								BReorderSegPoints.Add(BSegs[J].source());
								BSegAddIndexs.AddUnique(J);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
				{
					NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
				}

				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
				for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
				{
					NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
				}
				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));

			}
			else if ((LyingACount == 2) && (LyingBCount == 1))
			{
				Segment_2 InterSegment(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				int32 NextI = 0;
				////构建A和B的所有初始边的集合
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}

				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}
				/////找出A上的交点在B的哪条线段上
				int32 RemoveBSegIndex = 0;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment.target().x() - InterSegment.source().x(), InterSegment.target().y() - InterSegment.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							/*if ((BSegs[BSegIndex].source() == InterSegment.source()) || (BSegs[BSegIndex].source() == InterSegment.target())
								|| (BSegs[BSegIndex].target() == InterSegment.source())||(BSegs[BSegIndex].target() == InterSegment.target()))*/
							float Dis1 = CGAL::squared_distance(TempSeg.source(), TempSeg.target());
							float Dis2 = CGAL::squared_distance(InterSegment.source(), InterSegment.target());
							if (Dis1 >= Dis2)
							{
								RemoveBSegIndex = BSegIndex;
								break;
							}

						}

					}
				}
				////找出B上的交点在A的哪条线段上
				int32 RemoveASegIndex = 0;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment.target().x() - InterSegment.source().x(), InterSegment.target().y() - InterSegment.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							break;
						}

					}
				}
				/////重新构建新的线段
				Point_2 BRemoveSegStart = BSegs[RemoveBSegIndex].source();
				Point_2 BRemoveSegEnd = BSegs[RemoveBSegIndex].target();

				Point_2 InterSectPStart = InterSegment.source();
				Point_2 InterSectPEnd = InterSegment.target();

				////构建A新的线段
				float DisInterSeg = CGAL::squared_distance(InterSectPStart, InterSectPEnd);
				float DisARemoveSeg = CGAL::squared_distance(ASegs[RemoveASegIndex].source(), ASegs[RemoveASegIndex].target());
				float DisBRemoveSeg = CGAL::squared_distance(BSegs[RemoveBSegIndex].source(), BSegs[RemoveBSegIndex].target());

				if ((DisInterSeg == DisARemoveSeg) && (DisInterSeg < DisBRemoveSeg))
				{
					ASegs.RemoveAt(RemoveASegIndex);

					if (BSegs[RemoveBSegIndex].source() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, BSegs[RemoveBSegIndex].target());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].source() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, BSegs[RemoveBSegIndex].target());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].target() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, BSegs[RemoveBSegIndex].source());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].target() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, BSegs[RemoveBSegIndex].source());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
				}
				//////Reorder ASegs
				TArray<Point_2> AReorderSegPoints;
				TMap<int32, Point_2> InterSectPsMapTailPs;
				AReorderSegPoints.Add(InterSectPStart);

				TArray<int32> SegAddIndexs;
				while (SegAddIndexs.Num() != ASegs.Num())
				{
					StepCount++;
					for (int32 I = 0; I < ASegs.Num(); I++)
					{
						if (!SegAddIndexs.Contains(I))
						{
							float APointDis1 = sqrt(CGAL::squared_distance(ASegs[I].source(), AReorderSegPoints.Last()));
							float APointDis2 = sqrt(CGAL::squared_distance(ASegs[I].target(), AReorderSegPoints.Last()));
							if (APointDis1 <= 0.1)
							{
								AReorderSegPoints.Add(ASegs[I].target());
								SegAddIndexs.AddUnique(I);
							}
							else if (APointDis2 <= 0.1)
							{
								AReorderSegPoints.Add(ASegs[I].source());
								SegAddIndexs.AddUnique(I);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;

				FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
				TailFirstVec.Normalize();
				Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(0, TailFirstP);

				FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
				TailSecondVec.Normalize();
				Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(1, TailSecondP);

				////Reorder BSegs
				TArray<Point_2> BReorderSegPoints;
				BReorderSegPoints.Add(InterSectPEnd);
				TArray<int32> BSegAddIndexs;
				while (BSegAddIndexs.Num() != BSegs.Num())
				{
					StepCount++;
					for (int32 J = 0; J < BSegs.Num(); J++)
					{
						if (!BSegAddIndexs.Contains(J))
						{
							float BPointDis1 = sqrt(CGAL::squared_distance(BSegs[J].source(), BReorderSegPoints.Last()));
							float BPointDis2 = sqrt(CGAL::squared_distance(BSegs[J].target(), BReorderSegPoints.Last()));
							if (BPointDis1 <= 0.1)
							{
								BReorderSegPoints.Add(BSegs[J].target());
								BSegAddIndexs.AddUnique(J);
							}
							else if (BPointDis2 <= 0.1)
							{
								BReorderSegPoints.Add(BSegs[J].source());
								BSegAddIndexs.AddUnique(J);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
				{
					NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
				}

				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
				for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
				{
					NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
				}
				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));

			}
			else if ((LyingACount == 2) && (LyingBCount == 2))
			{
				Segment_2 InterSegment(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				float InterSegDis = CGAL::squared_distance(InterSegment.source(), InterSegment.target());
				int32 NextI = 0;
				////构建A和B的所有初始边的集合
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}

				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}
				/////找出A上的交点在B的哪条线段上
				int32 RemoveBSegIndex = 0;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment);
					if (sqrt(Dis) <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment.target().x() - InterSegment.source().x(), InterSegment.target().y() - InterSegment.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							float SegDis = CGAL::squared_distance(BSegs[BSegIndex].source(), BSegs[BSegIndex].target());
							if ((abs(sqrt(SegDis) - sqrt(InterSegDis)) <= 5) || ((sqrt(SegDis) - sqrt(InterSegDis)) > 15))
							{
								RemoveBSegIndex = BSegIndex;
								break;
							}
						}

					}
				}
				////找出B上的交点在A的哪条线段上
				int32 RemoveASegIndex = 0;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment.target().x() - InterSegment.source().x(), InterSegment.target().y() - InterSegment.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							break;
						}

					}
				}
				/////重新构建新的线段
				Point_2 BRemoveSegStart = BSegs[RemoveBSegIndex].source();
				Point_2 BRemoveSegEnd = BSegs[RemoveBSegIndex].target();

				Point_2 InterSectPStart = InterSegment.source();
				Point_2 InterSectPEnd = InterSegment.target();

				////构建A新的线段
				float DisInterSeg = CGAL::squared_distance(InterSectPStart, InterSectPEnd);
				float DisARemoveSeg = CGAL::squared_distance(ASegs[RemoveASegIndex].source(), ASegs[RemoveASegIndex].target());
				float DisBRemoveSeg = CGAL::squared_distance(BSegs[RemoveBSegIndex].source(), BSegs[RemoveBSegIndex].target());

				if ((DisInterSeg == DisARemoveSeg) && (DisInterSeg == DisBRemoveSeg))
				{
					ASegs.RemoveAt(RemoveASegIndex);
					BSegs.RemoveAt(RemoveBSegIndex);

					//////Reorder ASegs
					TArray<Point_2> AReorderSegPoints;
					TMap<int32, Point_2> InterSectPsMapTailPs;
					AReorderSegPoints.Add(InterSectPStart);

					TArray<int32> SegAddIndexs;
					while (SegAddIndexs.Num() != ASegs.Num())
					{
						StepCount++;
						for (int32 I = 0; I < ASegs.Num(); I++)
						{
							if (!SegAddIndexs.Contains(I))
							{
								float APointDis1 = sqrt(CGAL::squared_distance(ASegs[I].source(), AReorderSegPoints.Last()));
								float APointDis2 = sqrt(CGAL::squared_distance(ASegs[I].target(), AReorderSegPoints.Last()));
								if (APointDis1 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].target());
									SegAddIndexs.AddUnique(I);
								}
								else if (APointDis2 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].source());
									SegAddIndexs.AddUnique(I);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;

					FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
					TailFirstVec.Normalize();
					Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(0, TailFirstP);

					FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
					TailSecondVec.Normalize();
					Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(1, TailSecondP);

					////Reorder BSegs
					TArray<Point_2> BReorderSegPoints;
					BReorderSegPoints.Add(InterSectPEnd);
					TArray<int32> BSegAddIndexs;
					while (BSegAddIndexs.Num() != BSegs.Num())
					{
						StepCount++;
						for (int32 J = 0; J < BSegs.Num(); J++)
						{
							if (!BSegAddIndexs.Contains(J))
							{
								float BPointDis1 = sqrt(CGAL::squared_distance(BSegs[J].source(), BReorderSegPoints.Last()));
								float BPointDis2 = sqrt(CGAL::squared_distance(BSegs[J].target(), BReorderSegPoints.Last()));
								if (BPointDis1 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].target());
									BSegAddIndexs.AddUnique(J);
								}
								else if (BPointDis2 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].source());
									BSegAddIndexs.AddUnique(J);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;
					for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
					{
						NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
						PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					}

					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
					for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
					{
						NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
						PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					}
					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));

				}
				else if ((DisInterSeg == DisARemoveSeg) && (DisBRemoveSeg - DisInterSeg >= 1))
				{
					ASegs.RemoveAt(RemoveASegIndex);
					if (BSegs[RemoveBSegIndex].source() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, BSegs[RemoveBSegIndex].target());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].source() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, BSegs[RemoveBSegIndex].target());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].target() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, BSegs[RemoveBSegIndex].source());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].target() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, BSegs[RemoveBSegIndex].source());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					//////Reorder ASegs
					TArray<Point_2> AReorderSegPoints;
					TMap<int32, Point_2> InterSectPsMapTailPs;
					AReorderSegPoints.Add(InterSectPStart);

					TArray<int32> SegAddIndexs;
					while (SegAddIndexs.Num() != ASegs.Num())
					{
						StepCount++;
						for (int32 I = 0; I < ASegs.Num(); I++)
						{
							if (!SegAddIndexs.Contains(I))
							{
								float APointDis1 = sqrt(CGAL::squared_distance(ASegs[I].source(), AReorderSegPoints.Last()));
								float APointDis2 = sqrt(CGAL::squared_distance(ASegs[I].target(), AReorderSegPoints.Last()));
								if (APointDis1 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].target());
									SegAddIndexs.AddUnique(I);
								}
								else if (APointDis2 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].source());
									SegAddIndexs.AddUnique(I);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;

					FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
					TailFirstVec.Normalize();
					Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(0, TailFirstP);

					FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
					TailSecondVec.Normalize();
					Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(1, TailSecondP);

					////Reorder BSegs
					TArray<Point_2> BReorderSegPoints;
					BReorderSegPoints.Add(InterSectPEnd);
					TArray<int32> BSegAddIndexs;
					while (BSegAddIndexs.Num() != BSegs.Num())
					{
						StepCount++;
						for (int32 J = 0; J < BSegs.Num(); J++)
						{
							if (!BSegAddIndexs.Contains(J))
							{
								float BPointDis1 = sqrt(CGAL::squared_distance(BSegs[J].source(), BReorderSegPoints.Last()));
								float BPointDis2 = sqrt(CGAL::squared_distance(BSegs[J].target(), BReorderSegPoints.Last()));
								if (BPointDis1 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].target());
									BSegAddIndexs.AddUnique(J);
								}
								else if (BPointDis2 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].source());
									BSegAddIndexs.AddUnique(J);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;
					for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
					{
						NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
						PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					}

					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
					for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
					{
						NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
						PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					}
					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));

				}
				else if ((DisInterSeg == DisARemoveSeg) && (DisBRemoveSeg - DisInterSeg < 1))
				{
					ASegs.RemoveAt(RemoveASegIndex);
					BSegs.RemoveAt(RemoveBSegIndex);

					//////Reorder ASegs
					TArray<Point_2> AReorderSegPoints;
					TMap<int32, Point_2> InterSectPsMapTailPs;
					AReorderSegPoints.Add(InterSectPStart);

					TArray<int32> SegAddIndexs;
					while (SegAddIndexs.Num() != ASegs.Num())
					{
						StepCount++;
						for (int32 I = 0; I < ASegs.Num(); I++)
						{
							if (!SegAddIndexs.Contains(I))
							{
								float APointDis1 = sqrt(CGAL::squared_distance(ASegs[I].source(), AReorderSegPoints.Last()));
								float APointDis2 = sqrt(CGAL::squared_distance(ASegs[I].target(), AReorderSegPoints.Last()));
								if (APointDis1 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].target());
									SegAddIndexs.AddUnique(I);
								}
								else if (APointDis2 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].source());
									SegAddIndexs.AddUnique(I);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;
					FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
					TailFirstVec.Normalize();
					Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(0, TailFirstP);

					FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
					TailSecondVec.Normalize();
					Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(1, TailSecondP);

					////Reorder BSegs
					TArray<Point_2> BReorderSegPoints;
					BReorderSegPoints.Add(InterSectPEnd);
					TArray<int32> BSegAddIndexs;
					while (BSegAddIndexs.Num() != BSegs.Num())
					{
						StepCount++;
						for (int32 J = 0; J < BSegs.Num(); J++)
						{
							if (!BSegAddIndexs.Contains(J))
							{
								float BPointDis1 = sqrt(CGAL::squared_distance(BSegs[J].source(), BReorderSegPoints.Last()));
								float BPointDis2 = sqrt(CGAL::squared_distance(BSegs[J].target(), BReorderSegPoints.Last()));
								if (BPointDis1 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].target());
									BSegAddIndexs.AddUnique(J);
								}
								else if (BPointDis2 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].source());
									BSegAddIndexs.AddUnique(J);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;
					for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
					{
						NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
						PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					}

					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
					for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
					{
						NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
						PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					}
					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));

				}
				else if ((DisInterSeg == DisARemoveSeg) && (DisInterSeg - DisBRemoveSeg) >= 1)
				{
					ASegs.RemoveAt(RemoveASegIndex);
					if (BSegs[RemoveBSegIndex].source() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, BSegs[RemoveBSegIndex].target());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].source() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, BSegs[RemoveBSegIndex].target());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].target() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, BSegs[RemoveBSegIndex].source());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					else if (BSegs[RemoveBSegIndex].target() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, BSegs[RemoveBSegIndex].source());
						BSegs.RemoveAt(RemoveBSegIndex);
						BSegs.Add(AddSeg2);
					}
					//////Reorder ASegs
					TArray<Point_2> AReorderSegPoints;
					TMap<int32, Point_2> InterSectPsMapTailPs;
					AReorderSegPoints.Add(InterSectPStart);

					TArray<int32> SegAddIndexs;
					while (SegAddIndexs.Num() != ASegs.Num())
					{
						StepCount++;
						for (int32 I = 0; I < ASegs.Num(); I++)
						{
							if (!SegAddIndexs.Contains(I))
							{
								float APointDis1 = sqrt(CGAL::squared_distance(ASegs[I].source(), AReorderSegPoints.Last()));
								float APointDis2 = sqrt(CGAL::squared_distance(ASegs[I].target(), AReorderSegPoints.Last()));
								if (APointDis1 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].target());
									SegAddIndexs.AddUnique(I);
								}
								else if (APointDis2 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].source());
									SegAddIndexs.AddUnique(I);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;
					FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
					TailFirstVec.Normalize();
					Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(0, TailFirstP);

					FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
					TailSecondVec.Normalize();
					Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(1, TailSecondP);

					////Reorder BSegs
					TArray<Point_2> BReorderSegPoints;
					BReorderSegPoints.Add(InterSectPEnd);
					TArray<int32> BSegAddIndexs;
					while (BSegAddIndexs.Num() != BSegs.Num())
					{
						StepCount++;
						for (int32 J = 0; J < BSegs.Num(); J++)
						{
							if (!BSegAddIndexs.Contains(J))
							{
								float BPointDis1 = sqrt(CGAL::squared_distance(BSegs[J].source(), BReorderSegPoints.Last()));
								float BPointDis2 = sqrt(CGAL::squared_distance(BSegs[J].target(), BReorderSegPoints.Last()));
								if (BPointDis1 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].target());
									BSegAddIndexs.AddUnique(J);
								}
								else if (BPointDis2 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].source());
									BSegAddIndexs.AddUnique(J);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;
					for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
					{
						NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
						PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					}

					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
					for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
					{
						NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
						PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					}
					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));



				}
				else if ((DisInterSeg == DisARemoveSeg) && (DisInterSeg - DisBRemoveSeg) < 1)
				{
					ASegs.RemoveAt(RemoveASegIndex);
					BSegs.RemoveAt(RemoveBSegIndex);
					//////Reorder ASegs
					TArray<Point_2> AReorderSegPoints;
					TMap<int32, Point_2> InterSectPsMapTailPs;
					AReorderSegPoints.Add(InterSectPStart);

					TArray<int32> SegAddIndexs;
					while (SegAddIndexs.Num() != ASegs.Num())
					{
						StepCount++;
						for (int32 I = 0; I < ASegs.Num(); I++)
						{
							if (!SegAddIndexs.Contains(I))
							{
								float APointDis1 = sqrt(CGAL::squared_distance(ASegs[I].source(), AReorderSegPoints.Last()));
								float APointDis2 = sqrt(CGAL::squared_distance(ASegs[I].target(), AReorderSegPoints.Last()));
								if (APointDis1 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].target());
									SegAddIndexs.AddUnique(I);
								}
								else if (APointDis2 <= 0.1)
								{
									AReorderSegPoints.Add(ASegs[I].source());
									SegAddIndexs.AddUnique(I);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;
					FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
					TailFirstVec.Normalize();
					Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(0, TailFirstP);

					FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
					TailSecondVec.Normalize();
					Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
					InterSectPsMapTailPs.Add(1, TailSecondP);

					////Reorder BSegs
					TArray<Point_2> BReorderSegPoints;
					BReorderSegPoints.Add(InterSectPEnd);
					TArray<int32> BSegAddIndexs;
					while (BSegAddIndexs.Num() != BSegs.Num())
					{
						StepCount++;
						for (int32 J = 0; J < BSegs.Num(); J++)
						{
							if (!BSegAddIndexs.Contains(J))
							{
								float BPointDis1 = sqrt(CGAL::squared_distance(BSegs[J].source(), BReorderSegPoints.Last()));
								float BPointDis2 = sqrt(CGAL::squared_distance(BSegs[J].target(), BReorderSegPoints.Last()));
								if (BPointDis1 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].target());
									BSegAddIndexs.AddUnique(J);
								}
								else if (BPointDis2 <= 0.1)
								{
									BReorderSegPoints.Add(BSegs[J].source());
									BSegAddIndexs.AddUnique(J);
								}
							}
						}
						if (StepCount >= MaxNumToL)
						{
							return false;
						}
					}
					StepCount = 0;

					for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
					{
						NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
						PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					}

					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
					for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
					{
						NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
						PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					}
					NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));



				}
			}
			else if ((LyingACount == 1) && (LyingBCount == 2))
			{
				Segment_2 InterSegment(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				int32 NextI = 0;
				////构建A和B的所有初始边的集合
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}

				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}
				/////找出A上的交点在B的哪条线段上
				int32 RemoveBSegIndex = 0;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment.target().x() - InterSegment.source().x(), InterSegment.target().y() - InterSegment.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							break;
						}

					}
				}
				////找出B上的交点在A的哪条线段上
				int32 RemoveASegIndex = 0;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment.target().x() - InterSegment.source().x(), InterSegment.target().y() - InterSegment.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							break;
						}

					}
				}
				/////重新构建新的线段
				Point_2 BRemoveSegStart = BSegs[RemoveBSegIndex].source();
				Point_2 BRemoveSegEnd = BSegs[RemoveBSegIndex].target();

				Point_2 InterSectPStart = InterSegment.source();
				Point_2 InterSectPEnd = InterSegment.target();

				////构建A新的线段
				float DisInterSeg = CGAL::squared_distance(InterSectPStart, InterSectPEnd);
				float DisARemoveSeg = CGAL::squared_distance(ASegs[RemoveASegIndex].source(), ASegs[RemoveASegIndex].target());
				float DisBRemoveSeg = CGAL::squared_distance(BSegs[RemoveBSegIndex].source(), BSegs[RemoveBSegIndex].target());

				if ((DisInterSeg == DisBRemoveSeg) && (DisInterSeg < DisARemoveSeg))
				{
					BSegs.RemoveAt(RemoveBSegIndex);

					if (ASegs[RemoveASegIndex].source() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, ASegs[RemoveASegIndex].target());
						ASegs.RemoveAt(RemoveASegIndex);
						ASegs.Add(AddSeg2);
					}
					else if (ASegs[RemoveASegIndex].source() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, ASegs[RemoveASegIndex].target());
						ASegs.RemoveAt(RemoveASegIndex);
						ASegs.Add(AddSeg2);
					}
					else if (ASegs[RemoveASegIndex].target() == InterSectPStart)
					{
						Segment_2 AddSeg2(InterSectPEnd, ASegs[RemoveASegIndex].source());
						ASegs.RemoveAt(RemoveASegIndex);
						ASegs.Add(AddSeg2);
					}
					else if (ASegs[RemoveASegIndex].target() == InterSectPEnd)
					{
						Segment_2 AddSeg2(InterSectPStart, ASegs[RemoveASegIndex].source());
						ASegs.RemoveAt(RemoveASegIndex);
						ASegs.Add(AddSeg2);
					}
				}
				//////Reorder ASegs
				TArray<Point_2> AReorderSegPoints;
				TMap<int32, Point_2> InterSectPsMapTailPs;
				AReorderSegPoints.Add(InterSectPStart);

				TArray<int32> SegAddIndexs;
				while (SegAddIndexs.Num() != ASegs.Num())
				{
					StepCount++;
					for (int32 I = 0; I < ASegs.Num(); I++)
					{
						if (!SegAddIndexs.Contains(I))
						{
							float APointDis1 = sqrt(CGAL::squared_distance(ASegs[I].source(), AReorderSegPoints.Last()));
							float APointDis2 = sqrt(CGAL::squared_distance(ASegs[I].target(), AReorderSegPoints.Last()));
							if (APointDis1 <= 0.1)
							{
								AReorderSegPoints.Add(ASegs[I].target());
								SegAddIndexs.AddUnique(I);
							}
							else if (APointDis2 <= 0.1)
							{
								AReorderSegPoints.Add(ASegs[I].source());
								SegAddIndexs.AddUnique(I);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
				TailFirstVec.Normalize();
				Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(0, TailFirstP);

				FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
				TailSecondVec.Normalize();
				Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(1, TailSecondP);

				////Reorder BSegs
				TArray<Point_2> BReorderSegPoints;
				BReorderSegPoints.Add(InterSectPEnd);
				TArray<int32> BSegAddIndexs;
				while (BSegAddIndexs.Num() != BSegs.Num())
				{
					StepCount++;
					for (int32 J = 0; J < BSegs.Num(); J++)
					{
						if (!BSegAddIndexs.Contains(J))
						{
							float BPointDis1 = sqrt(CGAL::squared_distance(BSegs[J].source(), BReorderSegPoints.Last()));
							float BPointDis2 = sqrt(CGAL::squared_distance(BSegs[J].target(), BReorderSegPoints.Last()));
							if (BPointDis1 <= 0.1)
							{
								BReorderSegPoints.Add(BSegs[J].target());
								BSegAddIndexs.AddUnique(J);
							}
							else if (BPointDis2 <= 0.1)
							{
								BReorderSegPoints.Add(BSegs[J].source());
								BSegAddIndexs.AddUnique(J);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}

				StepCount = 0;
				for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
				{
					NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
				}

				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
				for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
				{
					NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
				}
				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));




			}


		}
		else if (VirtualWallPs.Num() == 3)
		{
			int32 LyingACount = 0, LyingBCount = 0;
			for (int32 VWallPIndex = 0; VWallPIndex < VirtualWallPs.Num(); VWallPIndex++)
			{
				for (int32 APIndex = 0; APIndex < UpdatePolygonA.Num(); APIndex++)
				{
					if (VirtualWallPs[VWallPIndex].Equals(UpdatePolygonA[APIndex], 0.001))
					{
						UpdatePolygonA[APIndex] = VirtualWallPs[VWallPIndex];
						LyingACount++;
						break;
					}
				}
			}

			for (int32 VWallPIndex = 0; VWallPIndex < VirtualWallPs.Num(); VWallPIndex++)
			{
				for (int32 BPIndex = 0; BPIndex < UpdatePolygonB.Num(); BPIndex++)
				{
					if (VirtualWallPs[VWallPIndex].Equals(UpdatePolygonB[BPIndex], 0.001))
					{
						UpdatePolygonB[BPIndex] = VirtualWallPs[VWallPIndex];
						LyingBCount++;
						break;
					}
				}
			}

			if ((LyingACount == 2) && (LyingBCount == 2))
			{
				Segment_2 InterSegment1(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				Segment_2 InterSegment2(Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y), Point_2(VirtualWallPs[2].X, VirtualWallPs[2].Y));
				float InterSeg1Dis = CGAL::squared_distance(InterSegment1.source(), InterSegment1.target());
				float InterSeg2Dis = CGAL::squared_distance(InterSegment2.source(), InterSegment2.target());
				int32 NextI = 0;
				////构建A和B的所有初始边的集合
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}

				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}

				/////找出A上的交点在B的哪条线段上
				TArray<int32> RemoveBSegIndexs;
				int32 RemoveBSegIndex = 0;
				TMap<int32, Segment_2> RemoveBIndexMapSegs;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment2);
							break;
						}

					}
				}
				////找出B上的交点在A的哪条线段上
				TArray<int32> RemoveASegIndexs;
				int32 RemoveASegIndex = 0;
				TMap<int32, Segment_2> RemoveAIndexMapSegs;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment2);
							break;
						}

					}
				}

				////找到A中需要添加新线段的边
				Point_2 InterSeg1PStart = InterSegment1.source();
				Point_2 InterSeg1PEnd = InterSegment1.target();
				Point_2 InterSeg2PStart = InterSegment2.source();
				Point_2 InterSeg2PEnd = InterSegment2.target();

				float InterSegDis1 = CGAL::squared_distance(InterSeg1PStart, InterSeg1PEnd);
				float InterSegDis2 = CGAL::squared_distance(InterSeg2PStart, InterSeg2PEnd);

				for (int32 I = 0; I < RemoveASegIndexs.Num(); I++)
				{
					Segment_2 MapInterSectSeg = RemoveAIndexMapSegs[RemoveASegIndexs[I]];
					float TempSectDis = CGAL::squared_distance(MapInterSectSeg.source(), MapInterSectSeg.target());
					float TempEdgeDis = CGAL::squared_distance(ASegs[RemoveASegIndexs[I]].source(), ASegs[RemoveASegIndexs[I]].target());
					if (TempEdgeDis > TempSectDis)
					{
						if (MapInterSectSeg.source() == ASegs[RemoveASegIndexs[I]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), ASegs[RemoveASegIndexs[I]].target());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.source() == ASegs[RemoveASegIndexs[I]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), ASegs[RemoveASegIndexs[I]].source());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == ASegs[RemoveASegIndexs[I]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), ASegs[RemoveASegIndexs[I]].target());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == ASegs[RemoveASegIndexs[I]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), ASegs[RemoveASegIndexs[I]].source());
							ASegs.Add(AddSeg1);
						}
					}
				}

				////更新A的新的所有边
				TArray<Segment_2> UpdateASegs;

				for (int32 I = 0; I < ASegs.Num(); I++)
				{
					if (!RemoveASegIndexs.Contains(I))
					{
						UpdateASegs.Add(ASegs[I]);
					}

				}


				for (int32 J = 0; J < RemoveBSegIndexs.Num(); J++)
				{
					Segment_2 MapInterSectSeg = RemoveBIndexMapSegs[RemoveBSegIndexs[J]];
					float TempSectDis = CGAL::squared_distance(MapInterSectSeg.source(), MapInterSectSeg.target());
					float TempEdgeDis = CGAL::squared_distance(BSegs[RemoveBSegIndexs[J]].source(), BSegs[RemoveBSegIndexs[J]].target());
					if (TempEdgeDis > TempSectDis)
					{
						if (MapInterSectSeg.source() == BSegs[RemoveBSegIndexs[J]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), BSegs[RemoveBSegIndexs[J]].target());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.source() == BSegs[RemoveBSegIndexs[J]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), BSegs[RemoveBSegIndexs[J]].source());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == BSegs[RemoveBSegIndexs[J]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), BSegs[RemoveBSegIndexs[J]].target());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == BSegs[RemoveBSegIndexs[J]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), BSegs[RemoveBSegIndexs[J]].source());
							BSegs.Add(AddSeg1);
						}
					}
				}

				////更新B的所有的新边
				TArray<Segment_2> UpdateBSegs;
				for (int32 I = 0; I < BSegs.Num(); I++)
				{
					if (!RemoveBSegIndexs.Contains(I))
					{
						UpdateBSegs.Add(BSegs[I]);
					}

				}


				////////////////
				//////Reorder ASegs
				Point_2 InterSectPStart(VirtualWallPs[0].X, VirtualWallPs[0].Y);
				Point_2 InterSectPEnd(VirtualWallPs[2].X, VirtualWallPs[2].Y);

				TArray<Point_2> AReorderSegPoints;
				TMap<int32, Point_2> InterSectPsMapTailPs;
				AReorderSegPoints.Add(InterSectPStart);

				TArray<int32> SegAddIndexs;
				while (SegAddIndexs.Num() != UpdateASegs.Num())
				{
					StepCount++;
					for (int32 I = 0; I < UpdateASegs.Num(); I++)
					{
						if (!SegAddIndexs.Contains(I))
						{
							float APointDis1 = sqrt(CGAL::squared_distance(UpdateASegs[I].source(), AReorderSegPoints.Last()));
							float APointDis2 = sqrt(CGAL::squared_distance(UpdateASegs[I].target(), AReorderSegPoints.Last()));
							if (APointDis1 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].target());
								SegAddIndexs.AddUnique(I);
							}
							else if (APointDis2 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].source());
								SegAddIndexs.AddUnique(I);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
				TailFirstVec.Normalize();
				Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(0, TailFirstP);

				FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
				TailSecondVec.Normalize();
				Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(1, TailSecondP);

				////Reorder BSegs
				TArray<Point_2> BReorderSegPoints;
				BReorderSegPoints.Add(InterSectPEnd);
				TArray<int32> BSegAddIndexs;
				while (BSegAddIndexs.Num() != UpdateBSegs.Num())
				{
					StepCount++;
					for (int32 J = 0; J < UpdateBSegs.Num(); J++)
					{
						if (!BSegAddIndexs.Contains(J))
						{
							float BPointDis1 = sqrt(CGAL::squared_distance(UpdateBSegs[J].source(), BReorderSegPoints.Last()));
							float BPointDis2 = sqrt(CGAL::squared_distance(UpdateBSegs[J].target(), BReorderSegPoints.Last()));
							if (BPointDis1 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].target());
								BSegAddIndexs.AddUnique(J);
							}
							else if (BPointDis2 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].source());
								BSegAddIndexs.AddUnique(J);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}

				StepCount = 0;
				for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
				{
					NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
				}

				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
				for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
				{
					NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
				}
				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));

			}
			else if ((LyingACount == 3) && (LyingBCount == 1))
			{
				Segment_2 InterSegment1(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				Segment_2 InterSegment2(Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y), Point_2(VirtualWallPs[2].X, VirtualWallPs[2].Y));
				float InterSeg1Dis = CGAL::squared_distance(InterSegment1.source(), InterSegment1.target());
				float InterSeg2Dis = CGAL::squared_distance(InterSegment2.source(), InterSegment2.target());
				int32 NextI = 0;
				////构建A和B的所有初始边的集合
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}

				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}

				/////找出A上的交点在B的哪条线段上
				TArray<int32> RemoveBSegIndexs;
				int32 RemoveBSegIndex = 0;
				TMap<int32, Segment_2> RemoveBIndexMapSegs;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment2);
							break;
						}

					}
				}
				////找出B上的交点在A的哪条线段上
				TArray<int32> RemoveASegIndexs;
				int32 RemoveASegIndex = 0;
				TMap<int32, Segment_2> RemoveAIndexMapSegs;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment2);
							break;
						}

					}
				}

				////找到A中需要添加新线段的边
				Point_2 InterSeg1PStart = InterSegment1.source();
				Point_2 InterSeg1PEnd = InterSegment1.target();
				Point_2 InterSeg2PStart = InterSegment2.source();
				Point_2 InterSeg2PEnd = InterSegment2.target();

				float InterSegDis1 = CGAL::squared_distance(InterSeg1PStart, InterSeg1PEnd);
				float InterSegDis2 = CGAL::squared_distance(InterSeg2PStart, InterSeg2PEnd);

				for (int32 I = 0; I < RemoveASegIndexs.Num(); I++)
				{
					Segment_2 MapInterSectSeg = RemoveAIndexMapSegs[RemoveASegIndexs[I]];
					float TempSectDis = CGAL::squared_distance(MapInterSectSeg.source(), MapInterSectSeg.target());
					float TempEdgeDis = CGAL::squared_distance(ASegs[RemoveASegIndexs[I]].source(), ASegs[RemoveASegIndexs[I]].target());
					if (TempEdgeDis > TempSectDis)
					{
						if (MapInterSectSeg.source() == ASegs[RemoveASegIndexs[I]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), ASegs[RemoveASegIndexs[I]].target());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.source() == ASegs[RemoveASegIndexs[I]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), ASegs[RemoveASegIndexs[I]].source());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == ASegs[RemoveASegIndexs[I]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), ASegs[RemoveASegIndexs[I]].target());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == ASegs[RemoveASegIndexs[I]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), ASegs[RemoveASegIndexs[I]].source());
							ASegs.Add(AddSeg1);
						}
					}

				}

				////更新A的新的所有边
				TArray<Segment_2> UpdateASegs;

				for (int32 I = 0; I < ASegs.Num(); I++)
				{
					if (!RemoveASegIndexs.Contains(I))
					{
						UpdateASegs.Add(ASegs[I]);
					}

				}


				for (int32 J = 0; J < RemoveBSegIndexs.Num(); J++)
				{
					Segment_2 MapInterSectSeg = RemoveBIndexMapSegs[RemoveBSegIndexs[J]];
					float TempSectDis = CGAL::squared_distance(MapInterSectSeg.source(), MapInterSectSeg.target());
					float TempEdgeDis = CGAL::squared_distance(BSegs[RemoveBSegIndexs[J]].source(), BSegs[RemoveBSegIndexs[J]].target());
					if (TempEdgeDis > TempSectDis)
					{
						if (MapInterSectSeg.source() == BSegs[RemoveBSegIndexs[J]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), BSegs[RemoveBSegIndexs[J]].target());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.source() == BSegs[RemoveBSegIndexs[J]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), BSegs[RemoveBSegIndexs[J]].source());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == BSegs[RemoveBSegIndexs[J]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), BSegs[RemoveBSegIndexs[J]].target());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == BSegs[RemoveBSegIndexs[J]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), BSegs[RemoveBSegIndexs[J]].source());
							BSegs.Add(AddSeg1);
						}
					}
				}

				////更新B的所有的新边
				TArray<Segment_2> UpdateBSegs;
				for (int32 I = 0; I < BSegs.Num(); I++)
				{
					if (!RemoveBSegIndexs.Contains(I))
					{
						UpdateBSegs.Add(BSegs[I]);
					}

				}


				////////////////
				//////Reorder ASegs
				Point_2 InterSectPStart(VirtualWallPs[0].X, VirtualWallPs[0].Y);
				Point_2 InterSectPEnd(VirtualWallPs[2].X, VirtualWallPs[2].Y);

				TArray<Point_2> AReorderSegPoints;
				TMap<int32, Point_2> InterSectPsMapTailPs;
				AReorderSegPoints.Add(InterSectPStart);

				TArray<int32> SegAddIndexs;
				while (SegAddIndexs.Num() != UpdateASegs.Num())
				{
					StepCount++;
					for (int32 I = 0; I < UpdateASegs.Num(); I++)
					{
						if (!SegAddIndexs.Contains(I))
						{
							float APointDis1 = sqrt(CGAL::squared_distance(UpdateASegs[I].source(), AReorderSegPoints.Last()));
							float APointDis2 = sqrt(CGAL::squared_distance(UpdateASegs[I].target(), AReorderSegPoints.Last()));
							if (APointDis1 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].target());
								SegAddIndexs.AddUnique(I);
							}
							else if (APointDis2 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].source());
								SegAddIndexs.AddUnique(I);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
				TailFirstVec.Normalize();
				Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(0, TailFirstP);

				FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
				TailSecondVec.Normalize();
				Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(1, TailSecondP);

				////Reorder BSegs
				TArray<Point_2> BReorderSegPoints;
				BReorderSegPoints.Add(InterSectPEnd);
				TArray<int32> BSegAddIndexs;
				while (BSegAddIndexs.Num() != UpdateBSegs.Num())
				{
					StepCount++;
					for (int32 J = 0; J < UpdateBSegs.Num(); J++)
					{
						if (!BSegAddIndexs.Contains(J))
						{
							float BPointDis1 = sqrt(CGAL::squared_distance(UpdateBSegs[J].source(), BReorderSegPoints.Last()));
							float BPointDis2 = sqrt(CGAL::squared_distance(UpdateBSegs[J].target(), BReorderSegPoints.Last()));
							if (BPointDis1 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].target());
								BSegAddIndexs.AddUnique(J);
							}
							else if (BPointDis2 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].source());
								BSegAddIndexs.AddUnique(J);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;

				for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
				{
					NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
				}

				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
				for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
				{
					NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
				}
				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));

			}
			else if ((LyingACount == 2) && (LyingBCount == 3))
			{

				Segment_2 InterSegment1(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				Segment_2 InterSegment2(Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y), Point_2(VirtualWallPs[2].X, VirtualWallPs[2].Y));
				float InterSeg1Dis = CGAL::squared_distance(InterSegment1.source(), InterSegment1.target());
				float InterSeg2Dis = CGAL::squared_distance(InterSegment2.source(), InterSegment2.target());
				int32 NextI = 0;
				////构建A和B的所有初始边的集合
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}

				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}

				/////找出A上的交点在B的哪条线段上
				TArray<int32> RemoveBSegIndexs;
				int32 RemoveBSegIndex = 0;
				TMap<int32, Segment_2> RemoveBIndexMapSegs;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment2);
							break;
						}

					}
				}
				////找出B上的交点在A的哪条线段上
				TArray<int32> RemoveASegIndexs;
				int32 RemoveASegIndex = 0;
				TMap<int32, Segment_2> RemoveAIndexMapSegs;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment2);
							break;
						}

					}
				}

				////找到A中需要添加新线段的边
				Point_2 InterSeg1PStart = InterSegment1.source();
				Point_2 InterSeg1PEnd = InterSegment1.target();
				Point_2 InterSeg2PStart = InterSegment2.source();
				Point_2 InterSeg2PEnd = InterSegment2.target();

				float InterSegDis1 = CGAL::squared_distance(InterSeg1PStart, InterSeg1PEnd);
				float InterSegDis2 = CGAL::squared_distance(InterSeg2PStart, InterSeg2PEnd);

				for (int32 I = 0; I < RemoveASegIndexs.Num(); I++)
				{
					Segment_2 MapInterSectSeg = RemoveAIndexMapSegs[RemoveASegIndexs[I]];
					float TempSectDis = CGAL::squared_distance(MapInterSectSeg.source(), MapInterSectSeg.target());
					float TempEdgeDis = CGAL::squared_distance(ASegs[RemoveASegIndexs[I]].source(), ASegs[RemoveASegIndexs[I]].target());
					if (TempEdgeDis > TempSectDis)
					{
						if (MapInterSectSeg.source() == ASegs[RemoveASegIndexs[I]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), ASegs[RemoveASegIndexs[I]].target());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.source() == ASegs[RemoveASegIndexs[I]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), ASegs[RemoveASegIndexs[I]].source());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == ASegs[RemoveASegIndexs[I]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), ASegs[RemoveASegIndexs[I]].target());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == ASegs[RemoveASegIndexs[I]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), ASegs[RemoveASegIndexs[I]].source());
							ASegs.Add(AddSeg1);
						}
					}

				}

				////更新A的新的所有边
				TArray<Segment_2> UpdateASegs;

				for (int32 I = 0; I < ASegs.Num(); I++)
				{
					if (!RemoveASegIndexs.Contains(I))
					{
						UpdateASegs.Add(ASegs[I]);
					}

				}


				for (int32 J = 0; J < RemoveBSegIndexs.Num(); J++)
				{
					Segment_2 MapInterSectSeg = RemoveBIndexMapSegs[RemoveBSegIndexs[J]];
					float TempSectDis = CGAL::squared_distance(MapInterSectSeg.source(), MapInterSectSeg.target());
					float TempEdgeDis = CGAL::squared_distance(BSegs[RemoveBSegIndexs[J]].source(), BSegs[RemoveBSegIndexs[J]].target());
					if (TempEdgeDis > TempSectDis)
					{
						if (MapInterSectSeg.source() == BSegs[RemoveBSegIndexs[J]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), BSegs[RemoveBSegIndexs[J]].target());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.source() == BSegs[RemoveBSegIndexs[J]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), BSegs[RemoveBSegIndexs[J]].source());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == BSegs[RemoveBSegIndexs[J]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), BSegs[RemoveBSegIndexs[J]].target());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == BSegs[RemoveBSegIndexs[J]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), BSegs[RemoveBSegIndexs[J]].source());
							BSegs.Add(AddSeg1);
						}
					}
				}

				////更新B的所有的新边
				TArray<Segment_2> UpdateBSegs;
				for (int32 I = 0; I < BSegs.Num(); I++)
				{
					if (!RemoveBSegIndexs.Contains(I))
					{
						UpdateBSegs.Add(BSegs[I]);
					}

				}


				////////////////
				//////Reorder ASegs
				Point_2 InterSectPStart(VirtualWallPs[0].X, VirtualWallPs[0].Y);
				Point_2 InterSectPEnd(VirtualWallPs[2].X, VirtualWallPs[2].Y);

				TArray<Point_2> AReorderSegPoints;
				TMap<int32, Point_2> InterSectPsMapTailPs;
				AReorderSegPoints.Add(InterSectPStart);

				TArray<int32> SegAddIndexs;
				while (SegAddIndexs.Num() != UpdateASegs.Num())
				{
					StepCount++;
					for (int32 I = 0; I < UpdateASegs.Num(); I++)
					{
						if (!SegAddIndexs.Contains(I))
						{
							float APointDis1 = sqrt(CGAL::squared_distance(UpdateASegs[I].source(), AReorderSegPoints.Last()));
							float APointDis2 = sqrt(CGAL::squared_distance(UpdateASegs[I].target(), AReorderSegPoints.Last()));
							if (APointDis1 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].target());
								SegAddIndexs.AddUnique(I);
							}
							else if (APointDis2 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].source());
								SegAddIndexs.AddUnique(I);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;

				FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
				TailFirstVec.Normalize();
				Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(0, TailFirstP);

				FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
				TailSecondVec.Normalize();
				Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(1, TailSecondP);

				////Reorder BSegs
				TArray<Point_2> BReorderSegPoints;
				BReorderSegPoints.Add(InterSectPEnd);
				TArray<int32> BSegAddIndexs;
				while (BSegAddIndexs.Num() != UpdateBSegs.Num())
				{
					StepCount++;
					for (int32 J = 0; J < UpdateBSegs.Num(); J++)
					{
						if (!BSegAddIndexs.Contains(J))
						{
							float BPointDis1 = sqrt(CGAL::squared_distance(UpdateBSegs[J].source(), BReorderSegPoints.Last()));
							float BPointDis2 = sqrt(CGAL::squared_distance(UpdateBSegs[J].target(), BReorderSegPoints.Last()));
							if (BPointDis1 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].target());
								BSegAddIndexs.AddUnique(J);
							}
							else if (BPointDis2 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].source());
								BSegAddIndexs.AddUnique(J);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
				{
					NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
				}

				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
				for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
				{
					NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
				}
				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));

			}
			else if ((LyingACount == 3) && (LyingBCount == 3))
			{
				Segment_2 InterSegment1(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				Segment_2 InterSegment2(Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y), Point_2(VirtualWallPs[2].X, VirtualWallPs[2].Y));
				float InterSeg1Dis = CGAL::squared_distance(InterSegment1.source(), InterSegment1.target());
				float InterSeg2Dis = CGAL::squared_distance(InterSegment2.source(), InterSegment2.target());
				int32 NextI = 0;
				////构建A和B的所有初始边的集合
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}

				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}

				/////找出A上的交点在B的哪条线段上
				TArray<int32> RemoveBSegIndexs;
				int32 RemoveBSegIndex = 0;
				TMap<int32, Segment_2> RemoveBIndexMapSegs;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment2);
							break;
						}

					}
				}
				////找出B上的交点在A的哪条线段上
				TArray<int32> RemoveASegIndexs;
				int32 RemoveASegIndex = 0;
				TMap<int32, Segment_2> RemoveAIndexMapSegs;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment2);
							break;
						}

					}
				}

				////找到A中需要添加新线段的边
				Point_2 InterSeg1PStart = InterSegment1.source();
				Point_2 InterSeg1PEnd = InterSegment1.target();
				Point_2 InterSeg2PStart = InterSegment2.source();
				Point_2 InterSeg2PEnd = InterSegment2.target();

				float InterSegDis1 = CGAL::squared_distance(InterSeg1PStart, InterSeg1PEnd);
				float InterSegDis2 = CGAL::squared_distance(InterSeg2PStart, InterSeg2PEnd);



				////更新A的新的所有边
				TArray<Segment_2> UpdateASegs;

				for (int32 I = 0; I < ASegs.Num(); I++)
				{
					if (!RemoveASegIndexs.Contains(I))
					{
						UpdateASegs.Add(ASegs[I]);
					}

				}




				////更新B的所有的新边
				TArray<Segment_2> UpdateBSegs;
				for (int32 I = 0; I < BSegs.Num(); I++)
				{
					if (!RemoveBSegIndexs.Contains(I))
					{
						UpdateBSegs.Add(BSegs[I]);
					}

				}


				////////////////
				//////Reorder ASegs
				Point_2 InterSectPStart(VirtualWallPs[0].X, VirtualWallPs[0].Y);
				Point_2 InterSectPEnd(VirtualWallPs[2].X, VirtualWallPs[2].Y);

				TArray<Point_2> AReorderSegPoints;
				TMap<int32, Point_2> InterSectPsMapTailPs;
				AReorderSegPoints.Add(InterSectPStart);

				TArray<int32> SegAddIndexs;
				while (SegAddIndexs.Num() != UpdateASegs.Num())
				{
					StepCount++;
					for (int32 I = 0; I < UpdateASegs.Num(); I++)
					{
						if (!SegAddIndexs.Contains(I))
						{
							float APointDis1 = sqrt(CGAL::squared_distance(UpdateASegs[I].source(), AReorderSegPoints.Last()));
							float APointDis2 = sqrt(CGAL::squared_distance(UpdateASegs[I].target(), AReorderSegPoints.Last()));
							if (APointDis1 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].target());
								SegAddIndexs.AddUnique(I);
							}
							else if (APointDis2 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].source());
								SegAddIndexs.AddUnique(I);
							}
						}
					}

					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
				TailFirstVec.Normalize();
				Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(0, TailFirstP);

				FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
				TailSecondVec.Normalize();
				Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(1, TailSecondP);

				////Reorder BSegs
				TArray<Point_2> BReorderSegPoints;
				BReorderSegPoints.Add(InterSectPEnd);
				TArray<int32> BSegAddIndexs;
				while (BSegAddIndexs.Num() != UpdateBSegs.Num())
				{
					StepCount++;
					for (int32 J = 0; J < UpdateBSegs.Num(); J++)
					{
						if (!BSegAddIndexs.Contains(J))
						{
							float BPointDis1 = sqrt(CGAL::squared_distance(UpdateBSegs[J].source(), BReorderSegPoints.Last()));
							float BPointDis2 = sqrt(CGAL::squared_distance(UpdateBSegs[J].target(), BReorderSegPoints.Last()));
							if (BPointDis1 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].target());
								BSegAddIndexs.AddUnique(J);
							}
							else if (BPointDis2 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].source());
								BSegAddIndexs.AddUnique(J);
							}
						}
					}

					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
				{
					NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
				}

				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
				for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
				{
					NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
				}
				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));

			}
			else if ((LyingACount == 1) && (LyingBCount == 3))
			{
				Segment_2 InterSegment1(Point_2(VirtualWallPs[0].X, VirtualWallPs[0].Y), Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y));
				Segment_2 InterSegment2(Point_2(VirtualWallPs[1].X, VirtualWallPs[1].Y), Point_2(VirtualWallPs[2].X, VirtualWallPs[2].Y));
				float InterSeg1Dis = CGAL::squared_distance(InterSegment1.source(), InterSegment1.target());
				float InterSeg2Dis = CGAL::squared_distance(InterSegment2.source(), InterSegment2.target());
				int32 NextI = 0;
				////构建A和B的所有初始边的集合
				TArray<Segment_2> ASegs, BSegs;
				for (int32 I = 0; I < UpdatePolygonA.Num(); I++)
				{
					if (I == UpdatePolygonA.Num() - 1)
					{
						NextI = 0;
					}
					else
					{
						NextI = I + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonA[I].X, UpdatePolygonA[I].Y));
					Point_2 End(Point_2(UpdatePolygonA[NextI].X, UpdatePolygonA[NextI].Y));
					Segment_2 TempSeg(Start, End);
					ASegs.Add(TempSeg);
				}

				int32 NextJ = 0;
				for (int32 J = 0; J < UpdatePolygonB.Num(); J++)
				{
					if (J == UpdatePolygonB.Num() - 1)
					{
						NextJ = 0;
					}
					else
					{
						NextJ = J + 1;
					}
					Point_2 Start(Point_2(UpdatePolygonB[J].X, UpdatePolygonB[J].Y));
					Point_2 End(Point_2(UpdatePolygonB[NextJ].X, UpdatePolygonB[NextJ].Y));
					Segment_2 TempSeg(Start, End);
					BSegs.Add(TempSeg);
				}

				/////找出A上的交点在B的哪条线段上
				TArray<int32> RemoveBSegIndexs;
				int32 RemoveBSegIndex = 0;
				TMap<int32, Segment_2> RemoveBIndexMapSegs;
				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); BSegIndex++)
				{
					float Dis = CGAL::squared_distance(BSegs[BSegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = BSegs[BSegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double Value = abs(Vec1.X*Vec2.X + Vec2.Y*Vec1.Y);
						if (abs(Value - 1) <= 0.15)
						{
							RemoveBSegIndex = BSegIndex;
							RemoveBSegIndexs.AddUnique(RemoveBSegIndex);
							RemoveBIndexMapSegs.Add(RemoveBSegIndex, InterSegment2);
							break;
						}

					}
				}
				////找出B上的交点在A的哪条线段上
				TArray<int32> RemoveASegIndexs;
				int32 RemoveASegIndex = 0;
				TMap<int32, Segment_2> RemoveAIndexMapSegs;
				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment1);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment1.target().x() - InterSegment1.source().x(), InterSegment1.target().y() - InterSegment1.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment1);
							break;
						}

					}
				}

				for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ASegIndex++)
				{
					float Dis = CGAL::squared_distance(ASegs[ASegIndex], InterSegment2);
					if (Dis <= 5)
					{
						Segment_2 TempSeg = ASegs[ASegIndex];
						FVector2D Vec1 = FVector2D(TempSeg.target().x() - TempSeg.source().x(), TempSeg.target().y() - TempSeg.source().y());
						FVector2D Vec2 = FVector2D(InterSegment2.target().x() - InterSegment2.source().x(), InterSegment2.target().y() - InterSegment2.source().y());
						Vec1 = Vec1.GetSafeNormal();
						Vec2 = Vec2.GetSafeNormal();
						double TempValue = abs(Vec1.X*Vec2.X + Vec1.Y*Vec2.Y);
						if (abs(TempValue - 1) <= 0.15)
						{
							RemoveASegIndex = ASegIndex;
							RemoveASegIndexs.AddUnique(RemoveASegIndex);
							RemoveAIndexMapSegs.Add(RemoveASegIndex, InterSegment2);
							break;
						}

					}
				}

				////找到A中需要添加新线段的边
				Point_2 InterSeg1PStart = InterSegment1.source();
				Point_2 InterSeg1PEnd = InterSegment1.target();
				Point_2 InterSeg2PStart = InterSegment2.source();
				Point_2 InterSeg2PEnd = InterSegment2.target();

				float InterSegDis1 = CGAL::squared_distance(InterSeg1PStart, InterSeg1PEnd);
				float InterSegDis2 = CGAL::squared_distance(InterSeg2PStart, InterSeg2PEnd);

				for (int32 I = 0; I < RemoveASegIndexs.Num(); I++)
				{
					Segment_2 MapInterSectSeg = RemoveAIndexMapSegs[RemoveASegIndexs[I]];
					float TempSectDis = CGAL::squared_distance(MapInterSectSeg.source(), MapInterSectSeg.target());
					float TempEdgeDis = CGAL::squared_distance(ASegs[RemoveASegIndexs[I]].source(), ASegs[RemoveASegIndexs[I]].target());
					if (TempEdgeDis > TempSectDis)
					{
						if (MapInterSectSeg.source() == ASegs[RemoveASegIndexs[I]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), ASegs[RemoveASegIndexs[I]].target());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.source() == ASegs[RemoveASegIndexs[I]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), ASegs[RemoveASegIndexs[I]].source());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == ASegs[RemoveASegIndexs[I]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), ASegs[RemoveASegIndexs[I]].target());
							ASegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == ASegs[RemoveASegIndexs[I]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), ASegs[RemoveASegIndexs[I]].source());
							ASegs.Add(AddSeg1);
						}
					}

				}

				////更新A的新的所有边
				TArray<Segment_2> UpdateASegs;

				for (int32 I = 0; I < ASegs.Num(); I++)
				{
					if (!RemoveASegIndexs.Contains(I))
					{
						UpdateASegs.Add(ASegs[I]);
					}

				}


				for (int32 J = 0; J < RemoveBSegIndexs.Num(); J++)
				{
					Segment_2 MapInterSectSeg = RemoveBIndexMapSegs[RemoveBSegIndexs[J]];
					float TempSectDis = CGAL::squared_distance(MapInterSectSeg.source(), MapInterSectSeg.target());
					float TempEdgeDis = CGAL::squared_distance(BSegs[RemoveBSegIndexs[J]].source(), BSegs[RemoveBSegIndexs[J]].target());
					if (TempEdgeDis > TempSectDis)
					{
						if (MapInterSectSeg.source() == BSegs[RemoveBSegIndexs[J]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), BSegs[RemoveBSegIndexs[J]].target());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.source() == BSegs[RemoveBSegIndexs[J]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.target(), BSegs[RemoveBSegIndexs[J]].source());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == BSegs[RemoveBSegIndexs[J]].source())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), BSegs[RemoveBSegIndexs[J]].target());
							BSegs.Add(AddSeg1);
						}
						else if (MapInterSectSeg.target() == BSegs[RemoveBSegIndexs[J]].target())
						{
							Segment_2 AddSeg1(MapInterSectSeg.source(), BSegs[RemoveBSegIndexs[J]].source());
							BSegs.Add(AddSeg1);
						}
					}
				}

				////更新B的所有的新边
				TArray<Segment_2> UpdateBSegs;
				for (int32 I = 0; I < BSegs.Num(); I++)
				{
					if (!RemoveBSegIndexs.Contains(I))
					{
						UpdateBSegs.Add(BSegs[I]);
					}

				}


				////////////////
				//////Reorder ASegs
				Point_2 InterSectPStart(VirtualWallPs[0].X, VirtualWallPs[0].Y);
				Point_2 InterSectPEnd(VirtualWallPs[2].X, VirtualWallPs[2].Y);

				TArray<Point_2> AReorderSegPoints;
				TMap<int32, Point_2> InterSectPsMapTailPs;
				AReorderSegPoints.Add(InterSectPStart);

				TArray<int32> SegAddIndexs;
				while (SegAddIndexs.Num() != UpdateASegs.Num())
				{
					StepCount++;
					for (int32 I = 0; I < UpdateASegs.Num(); I++)
					{
						if (!SegAddIndexs.Contains(I))
						{
							float APointDis1 = sqrt(CGAL::squared_distance(UpdateASegs[I].source(), AReorderSegPoints.Last()));
							float APointDis2 = sqrt(CGAL::squared_distance(UpdateASegs[I].target(), AReorderSegPoints.Last()));
							if (APointDis1 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].target());
								SegAddIndexs.AddUnique(I);
							}
							else if (APointDis2 <= 0.1)
							{
								AReorderSegPoints.Add(UpdateASegs[I].source());
								SegAddIndexs.AddUnique(I);
							}
						}
					}

					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				FVector2D TailFirstVec = FVector2D(AReorderSegPoints[1].x() - AReorderSegPoints[0].x(), AReorderSegPoints[1].y() - AReorderSegPoints[0].y());
				TailFirstVec.Normalize();
				Point_2 TailFirstP(InterSectPStart.x() + TailFirstVec.X * ExtendL, InterSectPStart.y() + TailFirstVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(0, TailFirstP);

				FVector2D TailSecondVec = FVector2D(AReorderSegPoints.Last(1).x() - AReorderSegPoints.Last().x(), AReorderSegPoints.Last(1).y() - AReorderSegPoints.Last().y());
				TailSecondVec.Normalize();
				Point_2 TailSecondP(InterSectPEnd.x() + TailSecondVec.X * ExtendL, InterSectPEnd.y() + TailSecondVec.Y*ExtendL);
				InterSectPsMapTailPs.Add(1, TailSecondP);

				////Reorder BSegs
				TArray<Point_2> BReorderSegPoints;
				BReorderSegPoints.Add(InterSectPEnd);
				TArray<int32> BSegAddIndexs;
				while (BSegAddIndexs.Num() != UpdateBSegs.Num())
				{
					StepCount++;
					for (int32 J = 0; J < UpdateBSegs.Num(); J++)
					{
						if (!BSegAddIndexs.Contains(J))
						{
							float BPointDis1 = sqrt(CGAL::squared_distance(UpdateBSegs[J].source(), BReorderSegPoints.Last()));
							float BPointDis2 = sqrt(CGAL::squared_distance(UpdateBSegs[J].target(), BReorderSegPoints.Last()));
							if (BPointDis1 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].target());
								BSegAddIndexs.AddUnique(J);
							}
							else if (BPointDis2 <= 0.1)
							{
								BReorderSegPoints.Add(UpdateBSegs[J].source());
								BSegAddIndexs.AddUnique(J);
							}
						}
					}
					if (StepCount >= MaxNumToL)
					{
						return false;
					}
				}
				StepCount = 0;
				for (int32 I = 0; I < AReorderSegPoints.Num(); I++)
				{
					NewPolygonFirst.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
					PolygonUnion.AddUnique(FVector2D(AReorderSegPoints[I].x(), AReorderSegPoints[I].y()));
				}

				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[1].x(), InterSectPsMapTailPs[1].y()));
				for (int32 J = 0; J < BReorderSegPoints.Num(); J++)
				{
					NewPolygonSecond.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
					PolygonUnion.AddUnique(FVector2D(BReorderSegPoints[J].x(), BReorderSegPoints[J].y()));
				}
				NewPolygonSecond.AddUnique(FVector2D(InterSectPsMapTailPs[0].x(), InterSectPsMapTailPs[0].y()));
			}
		}
		else
		{
			return false;
		}
		if ((NewPolygonFirst.Num() == 0) || (NewPolygonSecond.Num() == 0) || (PolygonUnion.Num() == 0))
		{
			return false;
		}
	}
	return true;
}

bool  FPolygonAlg::CreateVirtualWall(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>& VirtualWallPs)
{
	TArray<FVector2D> NewPolygonFirst, NewPolygonSecond;
	if (PolygonFirst.Num() < PolygonSecond.Num())
	{
		NewPolygonFirst = PolygonFirst;
		NewPolygonSecond = PolygonSecond;
	}
	else if (PolygonFirst.Num() > PolygonSecond.Num())
	{
		NewPolygonFirst = PolygonSecond;
		NewPolygonSecond = PolygonFirst;
	}
	else
	{
		float L1 = 0.0, L2 = 0.0;
		int32 NextI = 0;
		for (int32 I = 0; I < PolygonFirst.Num(); I++)
		{
			if (I == PolygonFirst.Num() - 1)
			{
				NextI = 0;
			}
			else
			{
				NextI = I + 1;
			}
			L1 = L1 + FVector2D::Distance(PolygonFirst[I], PolygonFirst[NextI]);
		}
		int32 NextJ = 0;
		for (int32 J = 0; J < PolygonSecond.Num(); J++)
		{
			if (J == PolygonSecond.Num() - 1)
			{
				NextJ = 0;
			}
			else
			{
				NextJ = J + 1;
			}
			L2 = L2 + FVector2D::Distance(PolygonSecond[J], PolygonSecond[NextJ]);
		}
		if (L1 < L2)
		{
			NewPolygonFirst = PolygonFirst;
			NewPolygonSecond = PolygonSecond;
		}
		else
		{
			NewPolygonFirst = PolygonSecond;
			NewPolygonSecond = PolygonFirst;
		}
	}

	TArray<FVector2D> UpdatePolygonA, UpdatePolygonB, InterSectPs;
	bool InterSectSuccess = GetAllInterSectPs(NewPolygonFirst, NewPolygonSecond, UpdatePolygonA, UpdatePolygonB, InterSectPs);
	if (!InterSectSuccess)
	{
		return false;
	}
	bool CreateSuccess = CreateVirtualWallPoints(InterSectPs, VirtualWallPs);
	if (!CreateSuccess)
	{
		return false;
	}
	return true;
}

bool FPolygonAlg::JudgePointInPolygon(const TArray<FVector2D> InPnts, FVector2D ToJudgePoint)
{
	using namespace Triangulation;
	Polygon_2 CreatePolygon;
	for (int32 I = 0; I < InPnts.Num(); I++)
	{
		CreatePolygon.push_back(Point_2(InPnts[I].X, InPnts[I].Y));
	}
	if (!CreatePolygon.has_on_bounded_side(Point_2(ToJudgePoint.X, ToJudgePoint.Y)))
	{
		return false;
	}
	return true;
}

bool FPolygonAlg::JudgePointOnOrInPolygon(const TArray<FVector2D> InPnts, FVector2D ToJudgePoint)
{
	using namespace Triangulation;
	Polygon_2 CreatePolygon;
	for (int32 I = 0; I < InPnts.Num(); I++)
	{
		CreatePolygon.push_back(Point_2(InPnts[I].X, InPnts[I].Y));
	}
	if ((CreatePolygon.has_on_bounded_side(Point_2(ToJudgePoint.X, ToJudgePoint.Y))) || (CreatePolygon.has_on_boundary(Point_2(ToJudgePoint.X, ToJudgePoint.Y))))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FPolygonAlg::CreateVirtualWallWithDir(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>& VirtualWallPs,
	TMap<FVector2D, FVector2D>&TailPsMapDir)
{
	using namespace Triangulation;
	TArray<FVector2D> NewPolygonFirst, NewPolygonSecond;
	if (PolygonFirst.Num() < PolygonSecond.Num())
	{
		NewPolygonFirst = PolygonFirst;
		NewPolygonSecond = PolygonSecond;
	}
	else if (PolygonFirst.Num() > PolygonSecond.Num())
	{
		NewPolygonFirst = PolygonSecond;
		NewPolygonSecond = PolygonFirst;
	}
	else
	{
		float L1 = 0.0, L2 = 0.0;
		int32 NextI = 0;
		for (int32 I = 0; I < PolygonFirst.Num(); I++)
		{
			if (I == PolygonFirst.Num() - 1)
			{
				NextI = 0;
			}
			else
			{
				NextI = I + 1;
			}
			L1 = L1 + FVector2D::Distance(PolygonFirst[I], PolygonFirst[NextI]);
		}
		int32 NextJ = 0;
		for (int32 J = 0; J < PolygonSecond.Num(); J++)
		{
			if (J == PolygonSecond.Num() - 1)
			{
				NextJ = 0;
			}
			else
			{
				NextJ = J + 1;
			}
			L2 = L2 + FVector2D::Distance(PolygonSecond[J], PolygonSecond[NextJ]);
		}
		if (L1 < L2)
		{
			NewPolygonFirst = PolygonFirst;
			NewPolygonSecond = PolygonSecond;
		}
		else
		{
			NewPolygonFirst = PolygonSecond;
			NewPolygonSecond = PolygonFirst;
		}
	}

	TArray<FVector2D> UpdatePolygonA, UpdatePolygonB, InterSectPs;
	bool InterSectSuccess = GetAllInterSectPs(NewPolygonFirst, NewPolygonSecond, UpdatePolygonA, UpdatePolygonB, InterSectPs);
	if (!InterSectSuccess)
	{
		return false;
	}
	bool CreateSuccess = CreateVirtualWallPoints(InterSectPs, VirtualWallPs);
	if (!CreateSuccess)
	{
		return false;
	}

	if (VirtualWallPs.Num() == 2)
	{
		FVector2D InterSectDir = VirtualWallPs[1] - VirtualWallPs[0];
		InterSectDir = InterSectDir.GetSafeNormal();
		FVector2D VerticalDir = InterSectDir.GetRotated(90);
		FVector2D InterSectMiddleP = 0.5*(VirtualWallPs[0] + VirtualWallPs[1]);
		FVector2D ExtendP = InterSectMiddleP + 10 * VerticalDir;
		if (JudgePointInPolygon(UpdatePolygonA, ExtendP))
		{
			TailPsMapDir.Add(VirtualWallPs[0], VerticalDir);
			TailPsMapDir.Add(VirtualWallPs[1], VerticalDir);
		}
		else
		{
			TailPsMapDir.Add(VirtualWallPs[0], -VerticalDir);
			TailPsMapDir.Add(VirtualWallPs[1], -VerticalDir);
		}
	}
	if (VirtualWallPs.Num() == 3)
	{
		FVector2D InterSectDir1 = VirtualWallPs[1] - VirtualWallPs[0];
		InterSectDir1 = InterSectDir1.GetSafeNormal();
		FVector2D VerticalDir1 = InterSectDir1.GetRotated(90);
		FVector2D MiddleP1 = 0.5*(VirtualWallPs[0] + VirtualWallPs[1]);
		FVector2D Extend1 = MiddleP1 + 10 * VerticalDir1;
		FVector2D InterSectDir2 = VirtualWallPs[2] - VirtualWallPs[1];
		InterSectDir2 = InterSectDir2.GetSafeNormal();
		FVector2D VerticalDir2 = InterSectDir2.GetRotated(90);
		FVector2D MiddleP2 = 0.5*(VirtualWallPs[1] + VirtualWallPs[2]);
		FVector2D Extend2 = MiddleP2 + 10 * VerticalDir2;
		if (JudgePointInPolygon(UpdatePolygonA, Extend1))
		{
			TailPsMapDir.Add(VirtualWallPs[0], VerticalDir1);
		}
		else
		{
			TailPsMapDir.Add(VirtualWallPs[0], -VerticalDir1);
		}

		if (JudgePointInPolygon(UpdatePolygonA, Extend2))
		{
			TailPsMapDir.Add(VirtualWallPs[2], VerticalDir2);
		}
		else
		{
			TailPsMapDir.Add(VirtualWallPs[2], -VerticalDir2);
		}
	}
	return true;
}

bool FPolygonAlg::CalculateTwoPolygonInterSect(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>&UpdatePolygonFirst,
	TArray<FVector2D>& UpdatePolygonSecond, TArray<FVector2D>&InterSectPs, TArray<FVector2D>& CombinedPolygon)
{
	using namespace Triangulation;
	const float PointDisTol = 0.1f;
	int CountSteps = 0;
	const int MaxNumTol = 1000;
	TArray<FVector2D> AllPoints;
	TArray<Segment_2> ASegs, BSegs;
	int32 NextI = 0;
	for (int32 I = 0; I < PolygonFirst.Num(); ++I)
	{
		if (I == PolygonFirst.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = I + 1;
		}
		Segment_2 TempSeg(Point_2(PolygonFirst[I].X, PolygonFirst[I].Y), Point_2(PolygonFirst[NextI].X, PolygonFirst[NextI].Y));
		ASegs.Add(TempSeg);
		AllPoints.AddUnique(PolygonFirst[I]);
	}

	int32 NextJ = 0;
	for (int32 J = 0; J < PolygonSecond.Num(); ++J)
	{
		if (J == PolygonSecond.Num() - 1)
		{
			NextJ = 0;
		}
		else
		{
			NextJ = J + 1;
		}
		Segment_2 TempSeg(Point_2(PolygonSecond[J].X, PolygonSecond[J].Y), Point_2(PolygonSecond[NextJ].X, PolygonSecond[NextJ].Y));
		BSegs.Add(TempSeg);
		AllPoints.AddUnique(PolygonSecond[J]);
	}

	TArray<Segment_2> InterSectSegs;
	TArray<Point_2> TempInterSectPs;
	TArray<int32> RemoveASegIndexs, RemoveBSegIndexs;
	for (int32 K = 0; K < ASegs.Num(); K++)
	{
		for (int32 L = 0; L < BSegs.Num(); ++L)
		{
			float Dis1 = sqrt(CGAL::squared_distance(ASegs[K].source(), BSegs[L].source()));
			float Dis2 = sqrt(CGAL::squared_distance(ASegs[K].target(), BSegs[L].target()));
			float Dis3 = sqrt(CGAL::squared_distance(ASegs[K].target(), BSegs[L].source()));
			float Dis4 = sqrt(CGAL::squared_distance(ASegs[K].source(), BSegs[L].target()));
			if ((Dis1 <= PointDisTol) && (Dis2 <= PointDisTol))
			{
				InterSectSegs.Add(ASegs[K]);
				RemoveASegIndexs.AddUnique(K);
				RemoveBSegIndexs.AddUnique(L);
				TempInterSectPs.AddUnique(ASegs[K].source());
				TempInterSectPs.AddUnique(ASegs[K].target());
				break;
			}
			else if ((Dis3 <= PointDisTol) && (Dis4 <= PointDisTol))
			{
				InterSectSegs.Add(ASegs[K]);
				RemoveASegIndexs.AddUnique(K);
				RemoveBSegIndexs.AddUnique(L);
				TempInterSectPs.AddUnique(ASegs[K].target());
				TempInterSectPs.AddUnique(ASegs[K].source());
				break;
			}
		}
	}

	TArray<Point_2> TurnPoints;
	for (int32 I = 0; I < TempInterSectPs.Num(); ++I)
	{
		int32 TempCount = 0;
		for (int32 J = 0; J < RemoveASegIndexs.Num(); ++J)
		{
			float TempDis1 = sqrt(CGAL::squared_distance(TempInterSectPs[I], ASegs[RemoveASegIndexs[J]].source()));
			float TempDis2 = sqrt(CGAL::squared_distance(TempInterSectPs[I], ASegs[RemoveASegIndexs[J]].target()));
			if ((TempDis1 <= PointDisTol) || (TempDis2 <= PointDisTol))
			{
				TempCount++;
			}
		}

		if (TempCount == 1)
		{
			TurnPoints.Add(TempInterSectPs[I]);
		}
	}

	if (TurnPoints.Num() <= 1)
	{
		return false;
	}

	//reorder interSect Points
	TArray<Point_2> TempCGInterPs;
	TempCGInterPs.Add(TurnPoints[0]);
	InterSectPs.Add(FVector2D(TurnPoints[0].x(), TurnPoints[0].y()));

	TArray<int32> HasInterSectIndexs;
	while (TempCGInterPs.Num() != InterSectSegs.Num() + 1)
	{
		CountSteps++;
		for (int32 InterIndex = 0; InterIndex < InterSectSegs.Num(); ++InterIndex)
		{
			if (!HasInterSectIndexs.Contains(InterIndex))
			{
				float TempDis1 = sqrt(CGAL::squared_distance(TempCGInterPs.Last(), InterSectSegs[InterIndex].source()));
				float TempDis2 = sqrt(CGAL::squared_distance(TempCGInterPs.Last(), InterSectSegs[InterIndex].target()));
				if (TempDis1 <= PointDisTol)
				{
					TempCGInterPs.Add(InterSectSegs[InterIndex].target());
					InterSectPs.Add(FVector2D(InterSectSegs[InterIndex].target().x(), InterSectSegs[InterIndex].target().y()));
					HasInterSectIndexs.AddUnique(InterIndex);
					break;
				}
				else if (TempDis2 <= PointDisTol)
				{
					TempCGInterPs.Add(InterSectSegs[InterIndex].source());
					InterSectPs.Add(FVector2D(InterSectSegs[InterIndex].source().x(), InterSectSegs[InterIndex].source().y()));
					HasInterSectIndexs.AddUnique(InterIndex);
					break;
				}
			}
		}

		if (CountSteps >= MaxNumTol)
		{
			return false;
		}
	}
	CountSteps = 0;
	if (InterSectPs.Num() <= 1)
	{
		return false;
	}

	//Calculate TailPoint Vector
	TMap<FVector2D, FVector2D> ATailPointsMapDirs, BTailPointsMapDirs;
	TArray<int32> FindASegIndexs, FindBSegIndexs;
	for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ++ASegIndex)
	{
		if ((!RemoveASegIndexs.Contains(ASegIndex)) && (!FindASegIndexs.Contains(ASegIndex)))
		{
			float TempDisL1 = sqrt(CGAL::squared_distance(ASegs[ASegIndex].source(), TempCGInterPs[0]));
			float TempDisL2 = sqrt(CGAL::squared_distance(ASegs[ASegIndex].target(), TempCGInterPs[0]));
			float TempDisL3 = sqrt(CGAL::squared_distance(ASegs[ASegIndex].source(), TempCGInterPs.Last()));
			float TempDisL4 = sqrt(CGAL::squared_distance(ASegs[ASegIndex].target(), TempCGInterPs.Last()));
			if (TempDisL1 <= PointDisTol)
			{
				float TempXDir = ASegs[ASegIndex].target().x() - ASegs[ASegIndex].source().x();
				float TempYDir = ASegs[ASegIndex].target().y() - ASegs[ASegIndex].source().y();
				FVector2D TempDir0 = FVector2D(TempXDir, TempYDir);
				TempDir0 = TempDir0.GetSafeNormal();
				//ATailPointsMapDirs.Add(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()), -TempDir0);
				BTailPointsMapDirs.Add(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()), TempDir0);
				FindASegIndexs.AddUnique(ASegIndex);
			}
			else if (TempDisL2 <= PointDisTol)
			{
				float TempXDir = ASegs[ASegIndex].source().x() - ASegs[ASegIndex].target().x();
				float TempYDir = ASegs[ASegIndex].source().y() - ASegs[ASegIndex].target().y();
				FVector2D TempDir0 = FVector2D(TempXDir, TempYDir);
				TempDir0 = TempDir0.GetSafeNormal();
				//ATailPointsMapDirs.Add(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()), -TempDir0);
				BTailPointsMapDirs.Add(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()), TempDir0);
				FindASegIndexs.AddUnique(ASegIndex);
			}
			else if (TempDisL3<= PointDisTol)
			{
				float TempXDir = ASegs[ASegIndex].target().x() - ASegs[ASegIndex].source().x();
				float TempYDir = ASegs[ASegIndex].target().y() - ASegs[ASegIndex].source().y();
				FVector2D TempDir0 = FVector2D(TempXDir, TempYDir);
				TempDir0 = TempDir0.GetSafeNormal();
				//ATailPointsMapDirs.Add(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()), -TempDir0);
				BTailPointsMapDirs.Add(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()), TempDir0);
				FindASegIndexs.AddUnique(ASegIndex);
			}
			else if (TempDisL4 <= PointDisTol)
			{
				float TempXDir = ASegs[ASegIndex].source().x() - ASegs[ASegIndex].target().x();
				float TempYDir = ASegs[ASegIndex].source().y() - ASegs[ASegIndex].target().y();
				FVector2D TempDir0 = FVector2D(TempXDir, TempYDir);
				TempDir0 = TempDir0.GetSafeNormal();
				//ATailPointsMapDirs.Add(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()), -TempDir0);
				BTailPointsMapDirs.Add(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()), TempDir0);
				FindASegIndexs.AddUnique(ASegIndex);
			}
		}
	}

	if (BTailPointsMapDirs.Num() <= 1)
	{
		return false;
	}

	for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); ++BSegIndex)
	{
		if ((!RemoveBSegIndexs.Contains(BSegIndex)) && (!FindBSegIndexs.Contains(BSegIndex)))
		{
			float TempDisL1 = sqrt(CGAL::squared_distance(BSegs[BSegIndex].source(), TempCGInterPs[0]));
			float TempDisL2 = sqrt(CGAL::squared_distance(BSegs[BSegIndex].target(), TempCGInterPs[0]));
			float TempDisL3 = sqrt(CGAL::squared_distance(BSegs[BSegIndex].source(), TempCGInterPs.Last()));
			float TempDisL4 = sqrt(CGAL::squared_distance(BSegs[BSegIndex].target(), TempCGInterPs.Last()));
			if (TempDisL1<= PointDisTol)
			{
				float TempXDir = BSegs[BSegIndex].target().x() - BSegs[BSegIndex].source().x();
				float TempYDir = BSegs[BSegIndex].target().y() - BSegs[BSegIndex].source().y();
				FVector2D TempDir0 = FVector2D(TempXDir, TempYDir);
				TempDir0 = TempDir0.GetSafeNormal();
				//BTailPointsMapDirs.Add(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()), -TempDir0);
				ATailPointsMapDirs.Add(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()), TempDir0);
				FindBSegIndexs.AddUnique(BSegIndex);
			}
			else if (TempDisL2 <= PointDisTol)
			{
				float TempXDir = BSegs[BSegIndex].source().x() - BSegs[BSegIndex].target().x();
				float TempYDir = BSegs[BSegIndex].source().y() - BSegs[BSegIndex].target().y();
				FVector2D TempDir0 = FVector2D(TempXDir, TempYDir);
				TempDir0 = TempDir0.GetSafeNormal();
				//BTailPointsMapDirs.Add(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()), -TempDir0);
				ATailPointsMapDirs.Add(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()), TempDir0);
				FindBSegIndexs.AddUnique(BSegIndex);
			}
			else if (TempDisL3 <= PointDisTol)
			{
				float TempXDir = BSegs[BSegIndex].target().x() - BSegs[BSegIndex].source().x();
				float TempYDir = BSegs[BSegIndex].target().y() - BSegs[BSegIndex].source().y();
				FVector2D TempDir0 = FVector2D(TempXDir, TempYDir);
				TempDir0 = TempDir0.GetSafeNormal();
				//BTailPointsMapDirs.Add(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()), -TempDir0);
				ATailPointsMapDirs.Add(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()), TempDir0);
				FindBSegIndexs.AddUnique(BSegIndex);
			}
			else if (TempDisL4 <= PointDisTol)
			{
				float TempXDir = BSegs[BSegIndex].source().x() - BSegs[BSegIndex].target().x();
				float TempYDir = BSegs[BSegIndex].source().y() - BSegs[BSegIndex].target().y();
				FVector2D TempDir0 = FVector2D(TempXDir, TempYDir);
				TempDir0 = TempDir0.GetSafeNormal();
				//BTailPointsMapDirs.Add(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()), -TempDir0);
				ATailPointsMapDirs.Add(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()), TempDir0);
				FindBSegIndexs.AddUnique(BSegIndex);
			}
		}
	}
	if (ATailPointsMapDirs.Num() <= 1)
	{
		return false;
	}
	//UpdatePolygoA
	FVector2D UpdatePolygonAStart = FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()) + 1.5*ATailPointsMapDirs[FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y())];
	UpdatePolygonFirst.AddUnique(UpdatePolygonAStart);
	UpdatePolygonFirst.AddUnique(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()));
	CombinedPolygon.AddUnique(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()));

	TArray<int32> AddedASegIndexs;
	while (UpdatePolygonFirst.Num() != ASegs.Num() - RemoveASegIndexs.Num() + 1)
	{
		CountSteps++;
		for (int32 ASegIndex = 0; ASegIndex < ASegs.Num(); ++ASegIndex)
		{
			if ((!RemoveASegIndexs.Contains(ASegIndex)) && (!AddedASegIndexs.Contains(ASegIndex)))
			{
				Point_2 TempPoint(UpdatePolygonFirst.Last().X, UpdatePolygonFirst.Last().Y);
				float TempDis1 = sqrt(CGAL::squared_distance(TempPoint, ASegs[ASegIndex].source()));
				float TempDis2 = sqrt(CGAL::squared_distance(TempPoint, ASegs[ASegIndex].target()));
				if (TempDis1 <= PointDisTol)
				{
					UpdatePolygonFirst.AddUnique(FVector2D(ASegs[ASegIndex].target().x(), ASegs[ASegIndex].target().y()));
					CombinedPolygon.AddUnique(FVector2D(ASegs[ASegIndex].target().x(), ASegs[ASegIndex].target().y()));
					AddedASegIndexs.AddUnique(ASegIndex);
					break;
				}
				else if (TempDis2 <= PointDisTol)
				{
					UpdatePolygonFirst.AddUnique(FVector2D(ASegs[ASegIndex].source().x(), ASegs[ASegIndex].source().y()));
					CombinedPolygon.AddUnique(FVector2D(ASegs[ASegIndex].source().x(), ASegs[ASegIndex].source().y()));
					AddedASegIndexs.AddUnique(ASegIndex);
					break;
				}
			}
		}
		if (CountSteps >= MaxNumTol)
		{
			return false;
		}
	}
	CountSteps = 0;
	UpdatePolygonFirst.AddUnique(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()));
	CombinedPolygon.AddUnique(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()));
	FVector2D UpdatePolygonAEnd = FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()) + 1.5*ATailPointsMapDirs[FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y())];
	UpdatePolygonFirst.AddUnique(UpdatePolygonAEnd);

	//UpdatePolygoB
	FVector2D UpdatePolygonBStart = FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()) + 1.5*BTailPointsMapDirs[FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y())];
	UpdatePolygonSecond.AddUnique(UpdatePolygonBStart);
	UpdatePolygonSecond.AddUnique(FVector2D(TempCGInterPs.Last().x(), TempCGInterPs.Last().y()));

	TArray<int32> AddedBSegIndexs;
	while (UpdatePolygonSecond.Num() != BSegs.Num() - RemoveBSegIndexs.Num() + 1)
	{
		CountSteps++;
		for (int32 BSegIndex = 0; BSegIndex < BSegs.Num(); ++BSegIndex)
		{
			if ((!RemoveBSegIndexs.Contains(BSegIndex)) && (!AddedBSegIndexs.Contains(BSegIndex)))
			{
				Point_2 TempPoint(UpdatePolygonSecond.Last().X, UpdatePolygonSecond.Last().Y);
				float TempDis1 = sqrt(CGAL::squared_distance(TempPoint, BSegs[BSegIndex].source()));
				float TempDis2 = sqrt(CGAL::squared_distance(TempPoint, BSegs[BSegIndex].target()));
				if (TempDis1 <= PointDisTol)
				{
					UpdatePolygonSecond.AddUnique(FVector2D(BSegs[BSegIndex].target().x(), BSegs[BSegIndex].target().y()));
					CombinedPolygon.AddUnique(FVector2D(BSegs[BSegIndex].target().x(), BSegs[BSegIndex].target().y()));
					AddedBSegIndexs.AddUnique(BSegIndex);
					break;
				}
				else if (TempDis2 <= PointDisTol)
				{
					UpdatePolygonSecond.AddUnique(FVector2D(BSegs[BSegIndex].source().x(), BSegs[BSegIndex].source().y()));
					CombinedPolygon.AddUnique(FVector2D(BSegs[BSegIndex].source().x(), BSegs[BSegIndex].source().y()));
					AddedBSegIndexs.AddUnique(BSegIndex);
					break;
				}
			}
		}
		if (CountSteps >= MaxNumTol)
		{
			return false;
		}
	}

	UpdatePolygonSecond.AddUnique(FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()));
	FVector2D UpdatePolygonBEnd = FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y()) + 1.5*BTailPointsMapDirs[FVector2D(TempCGInterPs[0].x(), TempCGInterPs[0].y())];
	UpdatePolygonSecond.AddUnique(UpdatePolygonBEnd);
	if (CombinedPolygon.Num() == 0)
	{
		return false;
	}
	return true;
}

bool FPolygonAlg::CGPolygonCombine(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, TArray<FVector2D>& CombinedPolygon)
{
	using namespace Triangulation;
	Polygon_2 Polygon1, Polygon2;
	for (int32 I = 0; I < PolygonFirst.Num(); ++I)
	{
		Polygon1.push_back(Point_2(PolygonFirst[I].X, PolygonFirst[I].Y));
	}
	for (int32 J = 0; J < PolygonSecond.Num(); ++J)
	{
		Polygon2.push_back(Point_2(PolygonSecond[J].X, PolygonSecond[J].Y));
	}
	Polygon_with_holes_2 unionR;
	if (CGAL::join(Polygon1, Polygon2, unionR))
	{
		Polygon_2 TempP = unionR.outer_boundary();
		for (auto vBegin = TempP.vertices_begin(), vEnd = TempP.vertices_end(); vBegin != vEnd; ++vBegin)
		{
			CombinedPolygon.AddUnique(FVector2D((*vBegin).x(), (*vBegin).y()));
		}
	}
	else
	{
		return false;
	}
	return true;
}


bool FPolygonAlg::JudgePolygonInterSect(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond)
{
	using namespace Triangulation;
	Polygon_2 Polygon1, Polygon2;


	for (int32 I = 0; I < PolygonFirst.Num(); ++I)
	{
		Polygon1.push_back(Point_2(PolygonFirst[I].X, PolygonFirst[I].Y));
	}

	for (int32 J = 0; J < PolygonSecond.Num(); ++J)
	{
		Polygon2.push_back(Point_2(PolygonSecond[J].X, PolygonSecond[J].Y));
	}
	if (Polygon1.area() < 0)
	{
		Polygon1.reverse_orientation();
	}
	if (Polygon2.area() < 0)
	{
		Polygon2.reverse_orientation();
	}
	if (!CGAL::do_intersect(Polygon1, Polygon2))
	{
		return false;
	}
	return true;
}

bool FPolygonAlg::FindParallelEdge(const TArray<FVector2D>InPolygon, FVector2D PStart, FVector2D PEnd, FVector2D& ParallelPStart,
	FVector2D& ParallelPEnd, int32& ParallelEdgeIndex, const float& DisValue)
{
	using namespace Triangulation;
	bool FindRes = false;
	if (InPolygon.Num() == 0)
	{
		return false;
	}
	FVector2D BaseVec = PEnd - PStart;
	BaseVec = BaseVec.GetSafeNormal();
	Segment_2 BaseSeg(Point_2(PStart.X, PStart.Y), Point_2(PEnd.X, PEnd.Y));
	int32 NextI = 0;
	for (int32 I = 0; I < InPolygon.Num(); ++I)
	{
		if (I == InPolygon.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = I + 1;
		}
		FVector2D TempVec = InPolygon[NextI] - InPolygon[I];
		TempVec = TempVec.GetSafeNormal();
		float FValue = abs(BaseVec.X*TempVec.Y - BaseVec.Y*TempVec.X);
		if (FValue <= 0.1)
		{
			Segment_2 TempSeg(Point_2(InPolygon[I].X, InPolygon[I].Y), Point_2(InPolygon[NextI].X, InPolygon[NextI].Y));
			float DisOfSegToSeg = sqrt(CGAL::squared_distance(BaseSeg, TempSeg));
			if (DisOfSegToSeg <= DisValue)
			{
				ParallelPStart = InPolygon[I];
				ParallelPEnd = InPolygon[NextI];
				ParallelEdgeIndex = I;
				FindRes = true;
				break;
			}
		}
		if (FindRes)
		{
			break;
		}
	}

	return FindRes;
}

bool FPolygonAlg::JudgeHasClosePoint(const TArray<FVector2D> PolygonFirst, const TArray<FVector2D> PolygonSecond, const float DisBound)
{
	using namespace Triangulation;
	bool FindRes = false;
	for (int32 I = 0; I < PolygonFirst.Num(); ++I)
	{
		for (int32 J = 0; J < PolygonSecond.Num(); ++J)
		{
			float TempDis = FVector2D::Distance(PolygonFirst[I], PolygonSecond[J]);
			if (TempDis <= DisBound)
			{
				FindRes = true;
				return true;
			}
		}

	}
	if (!FindRes)
	{
		int32 NextIndex1 = 0;
		int32 NextIndex2 = 0;
		for (int32 Index1 = 0; Index1 < PolygonFirst.Num(); ++Index1)
		{
			if (Index1 == PolygonFirst.Num() - 1)
			{
				NextIndex1 = 0;
			}
			else
			{
				NextIndex1 = Index1 + 1;
			}
			Segment_2 Seg1(Point_2(PolygonFirst[Index1].X, PolygonFirst[Index1].Y), Point_2(PolygonFirst[NextIndex1].X, PolygonFirst[NextIndex1].Y));

			for (int32 Index2 = 0; Index2 < PolygonSecond.Num(); ++Index2)
			{
				if (Index2 == PolygonSecond.Num() - 1)
				{
					NextIndex2 = 0;
				}
				else
				{
					NextIndex2 = Index2 + 1;
				}
				Segment_2 Seg2(Point_2(PolygonSecond[Index2].X, PolygonSecond[Index2].Y), Point_2(PolygonSecond[NextIndex2].X, PolygonSecond[NextIndex2].Y));
				float TempDis = sqrt(CGAL::squared_distance(Seg1, Seg2));
				if (TempDis <= DisBound)
				{
					FindRes = true;
					return true;
				}
			}
		}
	}
	return FindRes;
}


bool FPolygonAlg::MergeShortEdgesPreProcess(const TArray<FVector2D> InPnts, TArray<FVector2D>& OutPnts, const float DisBound)
{

	if (InPnts.Num() == 0)
	{
		return false;
	}
	TArray<FVector2D> TempPoints;
	bool RemoveCollinear = FPolygonAlg::MergeCollinearPoints(InPnts, TempPoints);
	int32 PNextIndex = 0;
	for (int32 Index = 0; Index < TempPoints.Num(); Index++)
	{
		if (Index == TempPoints.Num() - 1)
		{
			PNextIndex = 0;
		}
		else
		{
			PNextIndex = Index + 1;
		}
		float XDiffer = TempPoints[Index].X - TempPoints[PNextIndex].X;
		float YDiffer = TempPoints[Index].Y - TempPoints[PNextIndex].Y;
		float EdgeL = sqrt(XDiffer*XDiffer + YDiffer * YDiffer);
		if (EdgeL > DisBound)
		{
			OutPnts.Add(TempPoints[Index]);
		}
		else
		{
			continue;
		}
	}
	if (OutPnts.Num() <= 3)
	{
		OutPnts = TempPoints;
	}
	return true;
}

bool FPolygonAlg::GetMaxAreaEndPoints(const TArray<FVector2D> InPutPolygon, TArray<FVector2D>&EndPoints, TArray<FVector2D>&MapPoints, TArray<FVector2D>& MaxRegion)
{
	using namespace Triangulation;
	TArray<FVector2D> PreProcessPolygon;
	TArray<FVector2D>  OutPnts;
	bool MergeCollinearSuccess = MergeCollinearPoints(InPutPolygon, PreProcessPolygon);
	if (!MergeCollinearSuccess)
	{
		return false;
	}

	bool MergeSuccess = MergeShortEdges(PreProcessPolygon, OutPnts);
	if (!MergeSuccess)
	{
		return false;
	}


	if (OutPnts.Num() <= 4)
	{
		return false;
	}
	else
	{
		TMap<int32, TArray<FVector2D>> IndexMapEndPoints;
		TMap<int32, TArray<FVector2D>> IndexMapDirPoints;
		TArray<float> UsefulAreas;
		TArray<TArray<FVector2D>> UsefulRegions;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		int32 UsefulAreaNum = 0;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{
				TArray<Point_2> TempRegions;

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				float VerticalDis1 = DisPFrontToPStart * sqrt(1 - Product1 * Product1);
				float VerticalDis2 = DisPNextToPNextNext * sqrt(1 - Product2 * Product2);

				TArray<FVector2D> TempPointArray, TempDirPoints;
				TArray<FVector2D> TempUsefulRegion;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 2.0) && (DisPFrontToPStart / DisPStartToPNext <= 2))
				{

					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempDirPoints.Add(OutPnts[PStart]);
					TempDirPoints.Add(OutPnts[PNext]);

				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 2.0) && (DisPNextToPNextNext / DisPStartToPNext <= 2.0))
				{
					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempDirPoints.Add(OutPnts[PStart]);
					TempDirPoints.Add(OutPnts[PNext]);
				}

				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (TempArea != 0.0))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
					UsefulAreas.Add(TempArea);

					IndexMapEndPoints.Add(UsefulAreaNum, TempPointArray);
					IndexMapDirPoints.Add(UsefulAreaNum, TempDirPoints);
					UsefulAreaNum++;

					TArray<FVector2D> TempUsefulRegion;
					TempUsefulRegion.Add(OutPnts[PStart]);
					TempUsefulRegion.Add(OutPnts[PNext]);
					TempUsefulRegion.Add(OutPnts[PNextNext]);
					TempUsefulRegion.Add(OutPnts[PFront]);
					UsefulRegions.Add(TempUsefulRegion);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = 0.0;
		int32 TargetUsefulIndex = 0;
		for (int32 Index = 0; Index < UsefulAreas.Num(); ++Index)
		{
			if ((MaxArea == 0.0) || (UsefulAreas[Index] > MaxArea))
			{
				MaxArea = UsefulAreas[Index];
				TargetUsefulIndex = Index;
			}
		}
		if (UsefulAreas.Num() > 0)
		{
			EndPoints = IndexMapEndPoints[TargetUsefulIndex];
			MapPoints = IndexMapDirPoints[TargetUsefulIndex];
			MaxRegion = UsefulRegions[TargetUsefulIndex];
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool FPolygonAlg::GetWayInterSectPs(const TArray<FVector2D> InPutPolygon, const TArray<FVector2D>EndPoints, const TArray<FVector2D>MapPoints,
	TArray<FVector2D>&InterSectPs)
{
	using namespace Triangulation;
	TArray<Segment_2> AllSegments;
	TArray<FVector2D> InPnts;
	FPolygonAlg::MergeShortEdgesPreProcess(InPutPolygon, InPnts, 0.1);
	int32 PNextIndex = 0;
	for (int32 PIndex = 0; PIndex < InPnts.Num(); PIndex++)
	{
		if (PIndex == InPnts.Num() - 1)
		{
			PNextIndex = 0;
		}
		else
		{
			PNextIndex = PIndex + 1;
		}
		Segment_2 Seg(Point_2(InPnts[PIndex].X, InPnts[PIndex].Y), Point_2(InPnts[PNextIndex].X, InPnts[PNextIndex].Y));
		AllSegments.Add(Seg);
	}
	TArray<FVector2D>CollectInterSectPs;
	for (int32 I = 0; I < EndPoints.Num(); ++I)
	{
		Vector_2 TempVec(EndPoints[I].X - MapPoints[I].X, EndPoints[I].Y - MapPoints[I].Y);
		Ray_2 BaseRay_2(Point_2(EndPoints[I].X, EndPoints[I].Y), TempVec);
		bool FindIntersect = false;
		for (int32 Index = 0; Index < AllSegments.Num(); Index++)
		{
			CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay_2);
			if (Result)
			{
				if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
				{
					FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
					float TempDis = FVector2D::Distance(EndPoints[I], TempPoint);
					if (TempDis > 1)
					{
						CollectInterSectPs.Add(FVector2D((*TempIntersectP).x(), (*TempIntersectP).y()));
						break;
					}
				}
				else
				{
					return false;
				}
			}
		}
	}

	if (CollectInterSectPs.Num() != EndPoints.Num())
	{
		return false;
	}
	InterSectPs = CollectInterSectPs;
	return true;
}


bool FPolygonAlg::GetWayInterSectPsWithSegInfo(const TArray<FVector2D> InPutPolygon, const TArray<FVector2D>EndPoints, const TArray<FVector2D>MapPoints,
	TArray<FVector2D>&InterSectPs, bool &IfSegClose)
{
	using namespace Triangulation;
	TArray<Segment_2> AllSegments;
	TArray<FVector2D> InPnts;
	TArray<int>InterSectSegIndexs;
	FPolygonAlg::MergeCollinearPoints(InPutPolygon, InPnts);
	int32 PNextIndex = 0;
	for (int32 PIndex = 0; PIndex < InPnts.Num(); PIndex++)
	{
		if (PIndex == InPnts.Num() - 1)
		{
			PNextIndex = 0;
		}
		else
		{
			PNextIndex = PIndex + 1;
		}
		Segment_2 Seg(Point_2(InPnts[PIndex].X, InPnts[PIndex].Y), Point_2(InPnts[PNextIndex].X, InPnts[PNextIndex].Y));
		AllSegments.Add(Seg);
	}
	TArray<FVector2D>CollectInterSectPs;
	for (int32 I = 0; I < EndPoints.Num(); ++I)
	{
		Vector_2 TempVec(EndPoints[I].X - MapPoints[I].X, EndPoints[I].Y - MapPoints[I].Y);
		Ray_2 BaseRay_2(Point_2(EndPoints[I].X, EndPoints[I].Y), TempVec);
		bool FindIntersect = false;
		for (int32 Index = 0; Index < AllSegments.Num(); Index++)
		{
			CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay_2);
			if (Result)
			{
				if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
				{
					FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
					float TempDis = FVector2D::Distance(EndPoints[I], TempPoint);
					if (TempDis > 1)
					{
						CollectInterSectPs.Add(FVector2D((*TempIntersectP).x(), (*TempIntersectP).y()));
						InterSectSegIndexs.Add(Index);
						break;
					}
				}
			}
		}
	}

	if (CollectInterSectPs.Num() != EndPoints.Num())
	{
		return false;
	}
	IfSegClose = false;
	if (InterSectSegIndexs.Num() == 2)
	{
		Line_2 StraightLine1(AllSegments[InterSectSegIndexs[0]].source(), AllSegments[InterSectSegIndexs[0]].target());
		Line_2 StraightLine2(AllSegments[InterSectSegIndexs[1]].source(), AllSegments[InterSectSegIndexs[1]].target());
		float DisOfSegToSeg = sqrt(CGAL::squared_distance(StraightLine1, StraightLine2));
		if ((InterSectSegIndexs[0] == InterSectSegIndexs[1]) || (DisOfSegToSeg <= 60))
		{
			IfSegClose = true;
		}
	}
	InterSectPs = CollectInterSectPs;
	return true;
}


bool FPolygonAlg::DivideDoorPointsToGroups(const TArray<FVector> DoorPoints, TArray<FVector2D> EndPoints, TArray<FVector2D> MapEndPoints,
	TMap<int32, int32>&DoorIndexMapSegIndex)
{
	if ((EndPoints.Num() == 0) || (MapEndPoints.Num() == 0))
	{
		return false;
	}
	if (EndPoints.Num() != MapEndPoints.Num())
	{
		return false;
	}
	if (EndPoints.Num() == 2)
	{
		FVector2D TempSeg1Center = FVector2D(0.5*(EndPoints[0].X + MapEndPoints[0].X), 0.5*(EndPoints[0].Y + MapEndPoints[0].Y));
		FVector2D TempSeg2Center = FVector2D(0.5*(EndPoints[1].X + MapEndPoints[1].X), 0.5*(EndPoints[1].Y + MapEndPoints[1].Y));
		FVector2D DirVec1 = TempSeg1Center - TempSeg2Center;
		DirVec1 = DirVec1.GetSafeNormal();
		if (DoorPoints.Num() >= 8)
		{
			for (int32 DoorIndex = 0; DoorIndex < DoorPoints.Num() / 8; ++DoorIndex)
			{
				FVector TempDoorCenter3D = 0.25*(DoorPoints[0 + 8 * DoorIndex] + DoorPoints[1 + 8 * DoorIndex] +
					DoorPoints[2 + 8 * DoorIndex] + DoorPoints[3 + 8 * DoorIndex]);
				FVector2D TempDoorCenter2D = FVector2D(TempDoorCenter3D.X, TempDoorCenter3D.Y);
				FVector2D DoorCenterToSeg1Center = TempDoorCenter2D - TempSeg1Center;
				FVector2D DoorCenterToSeg2Center = TempDoorCenter2D - TempSeg2Center;
				float Res1 = FVector2D::DotProduct(DirVec1, DoorCenterToSeg1Center);
				float Res2 = FVector2D::DotProduct(-DirVec1, DoorCenterToSeg2Center);
				if (Res1 >= 0)
				{
					DoorIndexMapSegIndex.Add(DoorIndex, 0);
				}

				if (Res2 >= 0)
				{
					DoorIndexMapSegIndex.Add(DoorIndex, 1);
				}

			}
		}
	}
	if (DoorIndexMapSegIndex.Num() == 0)
	{
		return false;
	}
	return true;
}


bool FPolygonAlg::GetUsefulRegionOfMinArea(const TArray<FVector2D> InPutPolygon, TArray<FVector2D>& MinAreaRegion, FVector2D& SuitPoint)
{
	using namespace Triangulation;
	TArray<FVector2D> PreProcessPolygon;
	FPolygonAlg::MergeShortEdgesPreProcess(InPutPolygon, PreProcessPolygon, 0.1);
	TArray<FVector2D> TempOutPnts, OutPnts;
	bool MergeSuccess = MergeShrotEdgeCmSize(PreProcessPolygon, TempOutPnts);
	if (!MergeSuccess)
	{
		return false;
	}

	bool MergeCollinearSuccess = MergeCollinearPoints(TempOutPnts, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}
	if (OutPnts.Num() <= 4)
	{
		return false;
	}
	else
	{
		TMap<int32, TArray<FVector2D>> IndexMapEndPoints;
		TMap<int32, TArray<FVector2D>> IndexMapDirPoints;
		TArray<float> UsefulAreas;
		TArray<TArray<FVector2D>> UsefulRegions;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		int32 UsefulAreaNum = 0;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{
				TArray<Point_2> TempRegions;

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				float VerticalDis1 = DisPFrontToPStart * sqrt(1 - Product1 * Product1);
				float VerticalDis2 = DisPNextToPNextNext * sqrt(1 - Product2 * Product2);

				TArray<FVector2D> TempPointArray, TempDirPoints;
				TArray<FVector2D> TempUsefulRegion;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPFrontToPStart / DisPStartToPNext <= 2.0))
				{

					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempDirPoints.Add(OutPnts[PStart]);
					TempDirPoints.Add(OutPnts[PNext]);

				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPNextToPNextNext / DisPStartToPNext <= 2.0))
				{
					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempDirPoints.Add(OutPnts[PStart]);
					TempDirPoints.Add(OutPnts[PNext]);
				}

				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (TempArea != 0.0))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
					UsefulAreas.Add(TempArea);
					IndexMapEndPoints.Add(UsefulAreaNum, TempPointArray);
					IndexMapDirPoints.Add(UsefulAreaNum, TempDirPoints);
					UsefulAreaNum++;

					TArray<FVector2D> TempUsefulRegion;
					TempUsefulRegion.Add(OutPnts[PStart]);
					TempUsefulRegion.Add(OutPnts[PNext]);
					TempUsefulRegion.Add(OutPnts[PNextNext]);
					TempUsefulRegion.Add(OutPnts[PFront]);
					UsefulRegions.Add(TempUsefulRegion);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MinArea = 0.0;
		int32 TargetUsefulIndex = 0;
		for (int32 Index = 0; Index < UsefulAreas.Num(); ++Index)
		{
			if ((MinArea == 0.0) || (UsefulAreas[Index] < MinArea))
			{
				MinArea = UsefulAreas[Index];
				TargetUsefulIndex = Index;
			}
		}
		if (UsefulAreas.Num() > 0)
		{
			TArray<FVector2D> EndPoints = IndexMapEndPoints[TargetUsefulIndex];
			TArray<FVector2D> MapPoints = IndexMapDirPoints[TargetUsefulIndex];
			MinAreaRegion = UsefulRegions[TargetUsefulIndex];

			FVector2D MapMiddleP = 0.5*(MapPoints[0] + MapPoints[1]);
			FVector2D SegMiddleP = 0.5*(EndPoints[0] + EndPoints[1]);
			FVector2D TempDir = SegMiddleP - MapMiddleP;
			TempDir = TempDir.GetSafeNormal();
			SuitPoint = SegMiddleP + 100 * TempDir;
		}
		else
		{
			return false;
		}
	}
	return true;
}

float FPolygonAlg::DistanceOfPointToSeg(const FVector2D InPoint, const FVector2D StartOfSeg, const FVector2D EndOfSeg)
{
	using namespace Triangulation;
	Segment_2 Seg(Point_2(StartOfSeg.X, StartOfSeg.Y), Point_2(EndOfSeg.X, EndOfSeg.Y));
	return sqrt(CGAL::squared_distance(Point_2(InPoint.X, InPoint.Y), Seg));
}


bool FPolygonAlg::GetUsefulRegionOfMaxArea(const TArray<FVector2D> InPutPolygon, TArray<FVector2D>& MaxAreaRegion, FVector2D& SuitPoint)
{
	using namespace Triangulation;
	TArray<FVector2D> PreProcessPolygon;
	FPolygonAlg::MergeShortEdgesPreProcess(InPutPolygon, PreProcessPolygon, 0.1);
	TArray<FVector2D> OutPnts;

	bool MergeCollinearSuccess = MergeCollinearPoints(PreProcessPolygon, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}

	if (OutPnts.Num() <= 4)
	{
		return false;
	}
	else
	{
		TMap<int32, TArray<FVector2D>> IndexMapEndPoints;
		TMap<int32, TArray<FVector2D>> IndexMapDirPoints;
		TArray<float> UsefulAreas;
		TArray<TArray<FVector2D>> UsefulRegions;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		int32 UsefulAreaNum = 0;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{
				TArray<Point_2> TempRegions;

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				float VerticalDis1 = DisPFrontToPStart * sqrt(1 - Product1 * Product1);
				float VerticalDis2 = DisPNextToPNextNext * sqrt(1 - Product2 * Product2);

				TArray<FVector2D> TempPointArray, TempDirPoints;
				TArray<FVector2D> TempUsefulRegion;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPFrontToPStart / DisPStartToPNext <= 2.0))
				{

					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempDirPoints.Add(OutPnts[PStart]);
					TempDirPoints.Add(OutPnts[PNext]);

				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPNextToPNextNext / DisPStartToPNext <= 2.0))
				{
					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempDirPoints.Add(OutPnts[PStart]);
					TempDirPoints.Add(OutPnts[PNext]);
				}

				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (TempArea != 0.0))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
					UsefulAreas.Add(TempArea);
					IndexMapEndPoints.Add(UsefulAreaNum, TempPointArray);
					IndexMapDirPoints.Add(UsefulAreaNum, TempDirPoints);
					UsefulAreaNum++;

					TArray<FVector2D> TempUsefulRegion;
					TempUsefulRegion.Add(OutPnts[PStart]);
					TempUsefulRegion.Add(OutPnts[PNext]);
					TempUsefulRegion.Add(OutPnts[PNextNext]);
					TempUsefulRegion.Add(OutPnts[PFront]);
					UsefulRegions.Add(TempUsefulRegion);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = 0.0;
		int32 TargetUsefulIndex = 0;
		for (int32 Index = 0; Index < UsefulAreas.Num(); ++Index)
		{
			if ((MaxArea == 0.0) || (UsefulAreas[Index] > MaxArea))
			{
				MaxArea = UsefulAreas[Index];
				TargetUsefulIndex = Index;
			}
		}
		if (UsefulAreas.Num() > 0)
		{
			TArray<FVector2D> EndPoints = IndexMapEndPoints[TargetUsefulIndex];
			TArray<FVector2D> MapPoints = IndexMapDirPoints[TargetUsefulIndex];
			FVector2D MapMiddleP = 0.5*(MapPoints[0] + MapPoints[1]);
			FVector2D SegMiddleP = 0.5*(EndPoints[0] + EndPoints[1]);
			FVector2D TempDir = SegMiddleP - MapMiddleP;
			TempDir = TempDir.GetSafeNormal();

			MaxAreaRegion = UsefulRegions[TargetUsefulIndex];
			FVector2D ResStart, ResEnd;
			int32 ParrelEdgeIndex = 0;
			bool FindParrell = FPolygonAlg::FindParallelEdge(OutPnts, EndPoints[0], EndPoints[1], ResStart, ResEnd, ParrelEdgeIndex, 0.5);
			if (FindParrell)
			{
				float Rate = 0.5*(1 - FVector2D::Distance(ResStart, ResEnd) / FVector2D::Distance(EndPoints[0], EndPoints[1]));
				FVector2D TempNode = Rate * EndPoints[1] + (1 - Rate)*EndPoints[0];
				SuitPoint = TempNode - 100 * TempDir;
			}
			else
			{
				SuitPoint = SegMiddleP - 80 * TempDir;
			}

		}
		else
		{
			return false;
		}
	}
	return true;




}


bool FPolygonAlg::FindParallelEdges(const TArray<FVector2D>InPolygon, FVector2D PStart, FVector2D PEnd, TMap<int32, TArray<FVector2D>>&EdgeMapPoints,
	TArray<int32>& EdgeIndexs, const float& DisValue)
{
	using namespace Triangulation;
	bool FindRes = false;
	if (InPolygon.Num() == 0)
	{
		return false;
	}
	FVector2D BaseVec = PEnd - PStart;
	BaseVec = BaseVec.GetSafeNormal();
	Segment_2 BaseSeg(Point_2(PStart.X, PStart.Y), Point_2(PEnd.X, PEnd.Y));
	int32 NextI = 0;
	for (int32 I = 0; I < InPolygon.Num(); ++I)
	{
		if (I == InPolygon.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = I + 1;
		}
		FVector2D TempVec = InPolygon[NextI] - InPolygon[I];
		TempVec = TempVec.GetSafeNormal();
		float FValue = abs(BaseVec.X*TempVec.Y - BaseVec.Y*TempVec.X);
		if (FValue <= DisValue)
		{
			Segment_2 TempSeg(Point_2(InPolygon[I].X, InPolygon[I].Y), Point_2(InPolygon[NextI].X, InPolygon[NextI].Y));
			float DisOfSegToSeg = sqrt(CGAL::squared_distance(BaseSeg, TempSeg));
			if (DisOfSegToSeg <= DisValue)
			{
				TArray<FVector2D> TempPoints;
				TempPoints.Add(InPolygon[I]);
				TempPoints.Add(InPolygon[NextI]);
				EdgeIndexs.Add(I);
				EdgeMapPoints.Add(I, TempPoints);
			}
		}
	}
	if (EdgeMapPoints.Num() == 0)
	{
		return false;
	}
	return true;
}



bool FPolygonAlg::GetMaxRegionOfSpecialRoom(const TArray<FVector2D> InPutPolygon, TArray<FVector2D>& MaxAreaRegion, FVector2D& SuitPoint,
	TArray<FVector>DoorPoints)
{
	using namespace Triangulation;
	TArray<FVector2D> PreProcessPolygon;
	FPolygonAlg::MergeShortEdgesPreProcess(InPutPolygon, PreProcessPolygon, 0.1);
	TArray<FVector2D> OutPnts;

	bool MergeCollinearSuccess = FPolygonAlg::MergeCollinearPoints(PreProcessPolygon, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}

	if (OutPnts.Num() < 4)
	{
		return false;
	}
	else
	{
		TMap<int32, TArray<FVector2D>> IndexMapEndPoints;
		TMap<int32, TArray<FVector2D>> IndexMapDirPoints;
		TArray<float> UsefulAreas;
		TArray<TArray<FVector2D>> UsefulRegions;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		int32 UsefulAreaNum = 0;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{
				TArray<Point_2> TempRegions;

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				float VerticalDis1 = DisPFrontToPStart * sqrt(1 - Product1 * Product1);
				float VerticalDis2 = DisPNextToPNextNext * sqrt(1 - Product2 * Product2);

				TArray<FVector2D> TempPointArray, TempDirPoints;
				TArray<FVector2D> TempUsefulRegion;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 3.0))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempDirPoints.Add(OutPnts[PNext]);
					TempDirPoints.Add(OutPnts[PStart]);
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 3.0))
				{
					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempDirPoints.Add(OutPnts[PStart]);
					TempDirPoints.Add(OutPnts[PNext]);
				}

				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (TempArea >= 150 * 100))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
					UsefulAreas.Add(TempArea);
					IndexMapEndPoints.Add(UsefulAreaNum, TempPointArray);
					IndexMapDirPoints.Add(UsefulAreaNum, TempDirPoints);
					UsefulAreaNum++;

					TArray<FVector2D> TempUsefulRegion;
					TempUsefulRegion.Add(OutPnts[PStart]);
					TempUsefulRegion.Add(OutPnts[PNext]);
					TempUsefulRegion.Add(OutPnts[PNextNext]);
					TempUsefulRegion.Add(OutPnts[PFront]);
					UsefulRegions.Add(TempUsefulRegion);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = 0.0;
		int32 TargetUsefulIndex = 0;
		for (int32 Index = 0; Index < UsefulAreas.Num(); ++Index)
		{
			if ((MaxArea == 0.0) || (UsefulAreas[Index] > MaxArea))
			{
				MaxArea = UsefulAreas[Index]; 
				TargetUsefulIndex = Index;
			}
		}
		if (UsefulAreas.Num() > 0)
		{
			TArray<FVector2D> EndPoints = IndexMapEndPoints[TargetUsefulIndex];
			TArray<FVector2D> MapPoints = IndexMapDirPoints[TargetUsefulIndex];
			FVector2D MapMiddleP = 0.5*(MapPoints[0] + MapPoints[1]);
			FVector2D SegMiddleP = 0.5*(EndPoints[0] + EndPoints[1]);
			FVector2D TempDir = SegMiddleP - MapMiddleP;
			TempDir = TempDir.GetSafeNormal();

			MaxAreaRegion = UsefulRegions[TargetUsefulIndex];
			bool IsLocated = false;
			if (!IsLocated)
			{
				FVector2D FindTempDoorCenter;
				if ((EndPoints.Num() == 2) && (MapPoints.Num() == 2))
				{
					bool IsFindDoor = JudgeDoorOnLineAndGetDoor(EndPoints[0], MapPoints[0], DoorPoints, FindTempDoorCenter, 0.01);
					if (!IsFindDoor)
					{
						IsFindDoor = JudgeDoorOnLineAndGetDoor(EndPoints[1], MapPoints[1], DoorPoints, FindTempDoorCenter, 0.01);
					}
					if (IsFindDoor)
					{
						float LeftDis = FVector2D::Distance(EndPoints[0], MapPoints[0]);
						float RightDis = FVector2D::Distance(EndPoints[1], MapPoints[1]);
						FVector2D LocalCenterP;
						float DoorWidth = FVector::Distance(DoorPoints[0], DoorPoints[1]);
						if (DoorWidth <= 150)
						{
							if (LeftDis <= RightDis)
							{
								LocalCenterP = 0.5*(EndPoints[0] + MapPoints[1]);
							}
							else
							{
								LocalCenterP = 0.5*(EndPoints[1] + MapPoints[0]);
							}
							SuitPoint = 0.5*(LocalCenterP + FindTempDoorCenter);
							IsLocated = true;
						}
					}
				}
			}

			if (!IsLocated)
			{
				TMap<int32, TArray<FVector2D>>EdgeMapPoints;
				TArray<int32> EdgeIndexs;
				bool FindParrell = FPolygonAlg::FindParallelEdges(OutPnts, EndPoints[0], EndPoints[1], EdgeMapPoints, EdgeIndexs, 0.01);
				if (FindParrell)
				{
					if (EdgeIndexs.Num() == 1)
					{
						////判断平行线上是否有门
						FVector2D FindDoorCenter;
						bool FindDoor = JudgeDoorOnLineAndGetDoor(EndPoints[0], EndPoints[1], DoorPoints, FindDoorCenter, 0.01);
						if (!FindDoor)
						{
							TArray<FVector2D> TempAttr = EdgeMapPoints[EdgeIndexs[0]];
							FVector2D TempStraightVec = EndPoints[1] - EndPoints[0];
							TempStraightVec = TempStraightVec.GetSafeNormal();
							float TempDis = FVector2D::Distance(TempAttr[0], TempAttr[1]);
							float EndPDis = FVector2D::Distance(EndPoints[0], EndPoints[1]);
							float Rate = 1 - TempDis / EndPDis;
							if ((abs(Rate) > 0.7) && (abs(Rate) <= 0.85))
							{
								FVector2D TestMiddleP = 0.5*(TempAttr[0] + TempAttr[1]);
								FVector2D TestLocNode = TestMiddleP + 0.5*EndPDis*TempStraightVec;
								bool TestNodeInPolygon = FPolygonAlg::JudgePointInPolygon(OutPnts, TestLocNode);
								if (!TestNodeInPolygon)
								{
									FVector2D TestNode = TestMiddleP - 0.5*(EndPDis - TempDis)*TempStraightVec;
									float DisOfSegMiddleToMapMiddle = FVector2D::Distance(SegMiddleP, MapMiddleP);
									SuitPoint = TestNode - 3.0 / 4 * DisOfSegMiddleToMapMiddle * TempDir;
								}
								else
								{
									FVector2D TestNode = TestMiddleP + 0.5*(EndPDis - TempDis)*TempStraightVec;
									float DisOfSegMiddleToMapMiddle = FVector2D::Distance(SegMiddleP, MapMiddleP);
									SuitPoint = TestNode - 3.0 / 4 * DisOfSegMiddleToMapMiddle * TempDir;
								}
							}
							else if (abs(Rate) <= 0.7)
							{
								FVector2D TestMiddleP = 0.5*(TempAttr[0] + TempAttr[1]);
								FVector2D TestLocNode = TestMiddleP + 0.5*EndPDis*TempStraightVec;
								FVector2D MapMiddlePToSegMiddleP = MapMiddleP - SegMiddleP;
								float DisOfTwoSegs = abs(FVector2D::DotProduct(MapMiddlePToSegMiddleP, TempDir));
								bool TestNodeInPolygon = FPolygonAlg::JudgePointInPolygon(OutPnts, TestLocNode);
								if (!TestNodeInPolygon)
								{
									TestLocNode = TestMiddleP - 0.5*EndPDis*TempStraightVec;
									SuitPoint = TestLocNode - 0.5*DisOfTwoSegs * TempDir;
								}
								else
								{
									SuitPoint = TestLocNode - 0.5*DisOfTwoSegs * TempDir;
								}
							}
							else if (abs(Rate) > 0.85)
							{
								SuitPoint = 0.5*(EndPoints[0] + EndPoints[1]);
							}
						}
						else
						{
							bool HasChanged = false;
							int NumOfDoors = DoorPoints.Num() / 8;
							for (int I = 0; I < NumOfDoors; ++I)
							{
								FVector2D TempCenter = FVector2D(0.25*(DoorPoints[8 * I] + DoorPoints[8 * I + 1] + DoorPoints[8 * I + 2] + DoorPoints[8 * I + 3]));
								float TempDis = FVector2D::Distance(TempCenter, FindDoorCenter);
								if (TempDis >= 0.01)
								{
									FVector2D TempVec = FVector2D(DoorPoints[8 * I + 1] - DoorPoints[8 * I]);
									TempVec = TempVec.GetSafeNormal();
									int NextJ = 0;
									for (int J = 0; J < OutPnts.Num(); ++J)
									{
										if (J == OutPnts.Num() - 1)
										{
											NextJ = 0;
										}
										else
										{
											NextJ = J + 1;
										}
										FVector2D TempBaseVec = OutPnts[NextJ] - OutPnts[J];
										TempBaseVec = TempBaseVec.GetSafeNormal();
										Segment_2 RegionSeg(Point_2(OutPnts[J].X, OutPnts[J].Y), Point_2(OutPnts[NextJ].X, OutPnts[NextJ].Y));
										float FValue = abs(TempBaseVec.X*TempVec.Y - TempBaseVec.Y*TempVec.X);
										if (FValue <= 0.01)
										{
											Segment_2 TempSeg(Point_2(DoorPoints[8 * I + 1].X, DoorPoints[8 * I + 1].Y), Point_2(DoorPoints[8 * I].X, DoorPoints[8 * I].Y));
											float DisOfSegToSeg = sqrt(CGAL::squared_distance(RegionSeg, TempSeg));
											float RegionSegDis = sqrt(RegionSeg.squared_length());
											TArray<FVector2D> TempLastSegPs = EdgeMapPoints[EdgeIndexs[0]];
											float TempLastSegDis = FVector2D::Distance(TempLastSegPs[0], TempLastSegPs[1]);
											if ((DisOfSegToSeg <= 25) && (RegionSegDis <= TempLastSegDis))
											{
												FVector2D TempMP = 0.5*(MapMiddleP + SegMiddleP);
												FVector2D DoorCenterToMP = TempCenter - TempMP;
												float TransLength = FVector2D::DotProduct(DoorCenterToMP, TempDir);
												SuitPoint = TempCenter - TransLength * TempDir;
												HasChanged = true;
												break;
											}
										}
									}

								}
							}
							if (!HasChanged)
							{
								SuitPoint = FindDoorCenter - 90 * TempDir;
							}
						}
					}
					else
					{
						int NumOfDoors = DoorPoints.Num() / 8;
						bool LocalFindRes = false;
						const float TempDisVound = 180;
						if (NumOfDoors == 1)
						{
							FVector2D TempDoorCenter = 0.25*FVector2D(DoorPoints[0] + DoorPoints[1] + DoorPoints[2] + DoorPoints[3]);
							FVector2D TempVec = FVector2D(DoorPoints[1] - DoorPoints[0]);
							TempVec = TempVec.GetSafeNormal();
							FVector2D RotateOfVec = TempVec.GetRotated(90);
							FVector2D TryExtendP = TempDoorCenter + 40 * TempVec;
							bool IfInRegion = FPolygonAlg::JudgePointInPolygon(OutPnts, TryExtendP);
							if (IfInRegion)
							{
								FVector2D InterSectP;
								bool IsLocated = FPolygonAlg::CalculateRayInterSectPoly(OutPnts, TempDoorCenter, RotateOfVec, InterSectP, TempDisVound);
								if (IsLocated)
								{
									SuitPoint = 0.5*(TempDoorCenter + InterSectP);
									LocalFindRes = true;
								}
							}
							else
							{
								FVector2D InterSectP;
								bool IsLocated = FPolygonAlg::CalculateRayInterSectPoly(OutPnts, TempDoorCenter, -RotateOfVec, InterSectP, TempDisVound);
								if (IsLocated)
								{
									SuitPoint = 0.5*(TempDoorCenter + InterSectP);
									LocalFindRes = true;
								}
							}
						}

						if (!LocalFindRes)
						{
							float MinDis = 0.0;
							int32 TargetIndexE = 0;
							for (int32 I = 0; I < EdgeIndexs.Num(); ++I)
							{
								TArray<FVector2D> TempAttrPs = EdgeMapPoints[EdgeIndexs[I]];
								float TempDis = FVector2D::Distance(TempAttrPs[0], TempAttrPs[1]);
								if ((MinDis == 0.0) || (TempDis < MinDis))
								{
									MinDis = TempDis;
									TargetIndexE = I;
								}
							}
							SuitPoint = 0.5*(EdgeMapPoints[EdgeIndexs[TargetIndexE]][0] + EdgeMapPoints[EdgeIndexs[TargetIndexE]][1]) - 100 * TempDir;
						}

					}
				}
				else
				{
					SuitPoint = SegMiddleP - 100 * TempDir;
				}
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}


bool FPolygonAlg::GetRegionOfFourCornerSpecialRoom(const TArray<FVector2D> InPolygon, FVector2D& SuitPoint, TArray<FVector2D> WallPs,
	TArray<FVector> DoorPoints, const float DisValue)
{
	using namespace Triangulation;
	const float WallTol = 60;
	bool SomeEdgeHasNoWall = false;

	int32 TargetIndex = 0;
	if (InPolygon.Num() != 4)
	{
		return false;
	}
	FVector2D RegionCenter = 0.25*(InPolygon[0] + InPolygon[1] + InPolygon[2] + InPolygon[3]);
	int32 NumOfDoors = DoorPoints.Num() / 8;
	int32 NextI = 0;
	for (int32 I = 0; I < InPolygon.Num(); ++I)
	{
		if (I == InPolygon.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = I + 1;
		}
		FVector2D TempVec = InPolygon[NextI] - InPolygon[I];
		int32 CountNum = 0;
		TempVec = TempVec.GetSafeNormal();
		int32 NumOfWalls = WallPs.Num() / 4;
		for (int32 WallIndex = 0; WallIndex < NumOfWalls; ++WallIndex)
		{
			FVector2D TempWallVec = WallPs[4 * WallIndex + 1] - WallPs[4 * WallIndex];
			FVector2D TempWallVec2D = FVector2D(TempWallVec.X, TempWallVec.Y);
			TempWallVec2D = TempWallVec2D.GetSafeNormal();
			Segment_2 BaseSeg(Point_2(WallPs[4 * WallIndex + 1].X, WallPs[4 * WallIndex + 1].Y), Point_2(WallPs[4 * WallIndex].X, WallPs[4 * WallIndex].Y));
			float FValue = abs(TempWallVec2D.X*TempVec.Y - TempWallVec2D.Y*TempVec.X);
			if (FValue <= DisValue)
			{
				Segment_2 TempSeg(Point_2(InPolygon[I].X, InPolygon[I].Y), Point_2(InPolygon[NextI].X, InPolygon[NextI].Y));
				float DisOfSegToSeg = sqrt(CGAL::squared_distance(BaseSeg, TempSeg));
				if (DisOfSegToSeg <= WallTol)
				{
					CountNum++;
					break;
				}
			}
		}
		if (CountNum == 0)
		{
			SomeEdgeHasNoWall = true;
			TargetIndex = I;
			break;
		}
	}

	if (SomeEdgeHasNoWall)
	{
		int32 NextEdgeIndex = 0;
		int32 TargetEdgeIndex = 0;
		int32 TargetDoorIndex = 0;
		FVector2D TempNode;
		bool FindEdge = false;
		for (int32 J = 0; J < NumOfDoors; ++J)
		{
			FVector TempDoorVec = DoorPoints[8 * J + 1] - DoorPoints[8 * J];
			FVector2D TempDoorVec2D = FVector2D(TempDoorVec.X, TempDoorVec.Y);
			TempDoorVec2D = TempDoorVec2D.GetSafeNormal();
			Segment_2 TempDoorSeg(Point_2(DoorPoints[8 * J + 1].X, DoorPoints[8 * J + 1].Y), Point_2(DoorPoints[8 * J].X, DoorPoints[8 * J].Y));
			for (int32 EdgeIndex = 0; EdgeIndex < InPolygon.Num(); ++EdgeIndex)
			{
				if (EdgeIndex == InPolygon.Num() - 1)
				{
					NextEdgeIndex = 0;
				}
				else
				{
					NextEdgeIndex = EdgeIndex + 1;
				}
				FVector2D TempEdgeVec = InPolygon[NextEdgeIndex] - InPolygon[EdgeIndex];
				FVector2D TempEdgeVec2D = FVector2D(TempEdgeVec.X, TempEdgeVec.Y);
				TempEdgeVec2D = TempEdgeVec2D.GetSafeNormal();
				Segment_2 BaseEdgeSeg(Point_2(InPolygon[EdgeIndex].X, InPolygon[EdgeIndex].Y), Point_2(InPolygon[NextEdgeIndex].X, InPolygon[NextEdgeIndex].Y));
				float FValue = abs(TempEdgeVec2D.X*TempDoorVec2D.Y - TempEdgeVec2D.Y*TempDoorVec2D.X);
				float DisOfDoorToEdge = sqrt(CGAL::squared_distance(BaseEdgeSeg, TempDoorSeg));
				if ((FValue <= DisValue) && (DisOfDoorToEdge <= WallTol))
				{
					TargetEdgeIndex = EdgeIndex;
					FindEdge = true;
					TempNode = 0.5*(InPolygon[EdgeIndex] + InPolygon[NextEdgeIndex]);
					break;
				}
			}
			if (FindEdge)
			{
				TargetDoorIndex = J;
				SuitPoint = 2.0 / 3 * TempNode + 1.0 / 3 * RegionCenter;
				break;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		int32 NextEdgeIndex = 0;
		int32 TargetEdgeIndex = 0;
		int32 TargetDoorIndex = 0;
		bool FindEdge = false;
		for (int32 J = 0; J < NumOfDoors; ++J)
		{
			FVector TempDoorVec = DoorPoints[8 * J + 1] - DoorPoints[8 * J];
			float DoorDis = TempDoorVec.Size();
			Segment_2 DoorSeg(Point_2(DoorPoints[8 * J + 1].X, DoorPoints[8 * J + 1].Y), Point_2(DoorPoints[8 * J].X, DoorPoints[8 * J].Y));
			if ((DoorDis <= 150) || (NumOfDoors == 1))
			{
				FVector2D TempDoorVec2D = FVector2D(TempDoorVec.X, TempDoorVec.Y);
				TempDoorVec2D = TempDoorVec2D.GetSafeNormal();

				for (int32 EdgeIndex = 0; EdgeIndex < InPolygon.Num(); ++EdgeIndex)
				{
					if (EdgeIndex == InPolygon.Num() - 1)
					{
						NextEdgeIndex = 0;
					}
					else
					{
						NextEdgeIndex = EdgeIndex + 1;
					}
					FVector2D TempEdgeVec = InPolygon[NextEdgeIndex] - InPolygon[EdgeIndex];
					FVector2D TempEdgeVec2D = FVector2D(TempEdgeVec.X, TempEdgeVec.Y);
					TempEdgeVec2D = TempEdgeVec2D.GetSafeNormal();
					Segment_2 BaseEdgeSeg(Point_2(InPolygon[EdgeIndex].X, InPolygon[EdgeIndex].Y), Point_2(InPolygon[NextEdgeIndex].X, InPolygon[NextEdgeIndex].Y));
					float FValue = abs(TempEdgeVec2D.X*TempDoorVec2D.Y - TempEdgeVec2D.Y*TempDoorVec2D.X);
					float DisOfSegToSeg = sqrt(CGAL::squared_distance(BaseEdgeSeg, DoorSeg));
					if ((FValue <= DisValue) && (DisOfSegToSeg <= WallTol))
					{
						TargetEdgeIndex = EdgeIndex;
						FindEdge = true;
						break;
					}
				}
				if (FindEdge)
				{
					TargetDoorIndex = J;
					break;
				}
				else
				{
					return false;
				}
			}
		}

		if (FindEdge)
		{
			FVector2D DoorStartLeft = FVector2D(DoorPoints[0 + 8 * TargetDoorIndex].X, DoorPoints[0 + 8 * TargetDoorIndex].Y);
			FVector2D DoorEndLeft = FVector2D(DoorPoints[1 + 8 * TargetDoorIndex].X, DoorPoints[1 + 8 * TargetDoorIndex].Y);
			FVector2D DoorStartRight = FVector2D(DoorPoints[2 + 8 * TargetDoorIndex].X, DoorPoints[2 + 8 * TargetDoorIndex].Y);
			FVector2D DoorEndRight = FVector2D(DoorPoints[3 + 8 * TargetDoorIndex].X, DoorPoints[3 + 8 * TargetDoorIndex].Y);
			FVector2D DoorCenter = 0.25*(DoorStartLeft + DoorEndLeft + DoorEndRight + DoorStartRight);
			SuitPoint = 0.5*(RegionCenter + DoorCenter);
		}
		else
		{
			SuitPoint = RegionCenter;
		}
	}
	return true;
}



bool FPolygonAlg::GetMaxLocalRegionCenter(const FPoint2DArray InPnts, FVector2D& SuitableCenter)
{

	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;

	TArray<FVector2D>  TempPoints, OutPnts;

	bool MergeShortSuccess = MergeShrotEdgeCmSize(InPnts, TempPoints);
	if (!MergeShortSuccess)
	{
		return false;
	}

	bool MergeCollinearSuccess = MergeCollinearPoints(TempPoints, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}
	if (OutPnts.Num() <= 2)
	{
		return false;
	}
	else if (OutPnts.Num() == 3)
	{
		SuitableCenter = (OutPnts[0] + OutPnts[1] + OutPnts[2]) / 3;
		return true;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			SuitableCenter = IBetterCenter;
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			SuitableCenter = TriangleCenters[TargetIndex];
			return true;
		}
	}
	else
	{
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 2))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 2))
				{

					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;

				}



				if (Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE)
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = AreaSet[0];
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 1; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] > MaxArea)
			{
				MaxArea = AreaSet[CenterIndex];
				TargetIndex = CenterIndex;
			}
		}

		SuitableCenter = AreaCenters[TargetIndex];

		if (!Region2D.bounded_side(Point_2(SuitableCenter.X, SuitableCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			SuitableCenter = TriangleCenters[TargetIndex];
		}
	}
	return true;
}


bool FPolygonAlg::JudgeDoorOnLine(const FVector2D StartP, const FVector2D EndP, TArray<FVector> DoorPoints, const float DisValue)
{
	using namespace Triangulation;
	bool FindDoor = false;
	FVector2D BaseVec = EndP - StartP;
	BaseVec = BaseVec.GetSafeNormal();
	Segment_2 BaseSeg(Point_2(StartP.X, StartP.Y), Point_2(EndP.X, EndP.Y));
	int32 NumOfDoors = DoorPoints.Num() / 8;
	const float WallToL = 40;
	int32 NextI = 0;
	for (int32 I = 0; I < NumOfDoors; ++I)
	{
		FVector2D TempVec = FVector2D(DoorPoints[8 * I + 1] - DoorPoints[8 * I]);
		TempVec = TempVec.GetSafeNormal();
		float FValue = abs(BaseVec.X*TempVec.Y - BaseVec.Y*TempVec.X);
		if (FValue <= DisValue)
		{
			Segment_2 TempSeg(Point_2(DoorPoints[8 * I + 1].X, DoorPoints[8 * I + 1].Y), Point_2(DoorPoints[8 * I].X, DoorPoints[8 * I].Y));
			float DisOfSegToSeg = sqrt(CGAL::squared_distance(BaseSeg, TempSeg));
			if (DisOfSegToSeg <= WallToL)
			{
				FindDoor = true;
				break;
			}
		}
	}

	return FindDoor;
}


bool FPolygonAlg::JudgeDoorOnLineAndGetDoor(const FVector2D StartP, const FVector2D EndP, TArray<FVector> DoorPoints, FVector2D& DoorCenter, const float DisValue)
{
	using namespace Triangulation;
	bool FindDoor = false;
	FVector2D BaseVec = EndP - StartP;
	BaseVec = BaseVec.GetSafeNormal();
	Segment_2 BaseSeg(Point_2(StartP.X, StartP.Y), Point_2(EndP.X, EndP.Y));
	int32 NumOfDoors = DoorPoints.Num() / 8;
	const float WallToL = 40;
	int32 NextI = 0;
	for (int32 I = 0; I < NumOfDoors; ++I)
	{
		FVector2D TempVec = FVector2D(DoorPoints[8 * I + 1] - DoorPoints[8 * I]);
		TempVec = TempVec.GetSafeNormal();
		float FValue = abs(BaseVec.X*TempVec.Y - BaseVec.Y*TempVec.X);
		if (FValue <= DisValue)
		{
			Segment_2 TempSeg(Point_2(DoorPoints[8 * I + 1].X, DoorPoints[8 * I + 1].Y), Point_2(DoorPoints[8 * I].X, DoorPoints[8 * I].Y));
			float DisOfSegToSeg = sqrt(CGAL::squared_distance(BaseSeg, TempSeg));
			if (DisOfSegToSeg <= WallToL)
			{
				FindDoor = true;
				DoorCenter = FVector2D(0.25*(DoorPoints[8 * I] + DoorPoints[8 * I + 1] + DoorPoints[8 * I + 2] + DoorPoints[8 * I + 3]));
				break;
			}
		}
	}

	return FindDoor;
}


bool FPolygonAlg::GetDisOfPointToPolygon(const FVector2D TestPoint, TArray<FVector2D> RegionPs)
{
	using namespace Triangulation;
	TArray<FVector2D> InPnts, UpdatePs;;
	FPolygonAlg::MergeShortEdgesPreProcess(RegionPs, InPnts, 0.1);
	FPolygonAlg::MergeCollinearPoints(InPnts, UpdatePs);
	int32 PNextIndex = 0;
	bool FindRes = false;
	for (int32 PIndex = 0; PIndex < UpdatePs.Num(); PIndex++)
	{
		if (PIndex == UpdatePs.Num() - 1)
		{
			PNextIndex = 0;
		}
		else
		{
			PNextIndex = PIndex + 1;
		}
		float TempDis = FPolygonAlg::DistanceOfPointToSeg(TestPoint, UpdatePs[PIndex], UpdatePs[PNextIndex]);
		if (TempDis <= 50)
		{
			FindRes = true;
			break;
		}
	}
	return FindRes;
}



bool FPolygonAlg::LocateCentersOfBalcony(const TArray<FVector2D> InPnts, FVector2D& MaxRegionCenter,
	TArray<FVector2D>& UpdateSmallRegionCenters, TMap<FVector2D, TArray<FVector2D>>&CenterMapLocalRegions)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;

	TArray<FVector2D> OutPnts;

	bool IsSuccess = FPolygonAlg::MergeCollinearPoints(InPnts, OutPnts);
	if (!IsSuccess)
	{
		return false;
	}


	if (OutPnts.Num() <= 2)
	{
		return false;
	}
	else if (OutPnts.Num() == 3)
	{
		MaxRegionCenter = (OutPnts[0] + OutPnts[1] + OutPnts[2]) / 3;
		CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
		return true;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			MaxRegionCenter = IBetterCenter;
			CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			mesher.refine_mesh();
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			MaxRegionCenter = TriangleCenters[TargetIndex];
			CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
			return true;
		}
	}
	else
	{
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;
				bool FindLocalCenter = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPFrontToPStart > 80))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);

					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PStart]);
					TempPointArray.Add(OutPnts[PNext]);
					CenterMapLocalRegions.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					FindLocalCenter = true;
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPNextToPNextNext > 80))
				{
					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);

					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempPointArray.Add(OutPnts[PNext]);
					TempPointArray.Add(OutPnts[PStart]);
					CenterMapLocalRegions.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					FindLocalCenter = true;
				}



				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (FindLocalCenter))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);

				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = AreaSet[0];
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 1; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] > MaxArea)
			{
				MaxArea = AreaSet[CenterIndex];
				TargetIndex = CenterIndex;
			}
		}

		MaxRegionCenter = AreaCenters[TargetIndex];

		if (!FPolygonAlg::JudgePointInPolygon(OutPnts,MaxRegionCenter))
		{
			FPolygonAlg::LocateSuitableCenter(OutPnts, MaxRegionCenter);
			CenterMapLocalRegions.Add(MaxRegionCenter, OutPnts);
		}

		const float DisBoundOfRoom = 150;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if ((AreaSet[CenterIndex] != MaxArea) && (AreaSet[CenterIndex] != 0))
			{
				float DisToMaxRegionCenter = FVector2D::Distance(AreaCenters[CenterIndex], MaxRegionCenter);
				if (DisToMaxRegionCenter > DisBoundOfRoom)
				{
					UpdateSmallRegionCenters.Add(AreaCenters[CenterIndex]);
				}

			}
		}
	}
	return true;
}


bool FPolygonAlg::JudgePointInOrOnDingingOrLivingRegion(const FVector2D ModelCenter, const TArray<FVector2D>LivingRegionPs, const TArray<FVector2D>DiningRegionPs, int&Result)
{
	if ((LivingRegionPs.Num() == 0) || (DiningRegionPs.Num() == 0))
	{
		return false;
	}
	TArray<FVector2D> UpdateLivingRegionPs, UpdateDiningRegionPs;
	bool MergeLivingPs = MergeCollinearPoints(LivingRegionPs, UpdateLivingRegionPs);
	bool MergeDiningPs = MergeCollinearPoints(DiningRegionPs, UpdateDiningRegionPs);
	bool PointInLivingRegionPs = false;
	bool PointInDiningRegionPs = false;
	bool IfHasCommonPoints = false;
	bool JudgeIfInterSect = false;
	JudgeIfInterSect = JudgePolygonInterSect(UpdateLivingRegionPs, UpdateDiningRegionPs);
	IfHasCommonPoints = JudgeHasClosePoint(UpdateLivingRegionPs, UpdateDiningRegionPs, 5.0);

	const float PointToSegDisLimit = 8.0;
	if ((!IfHasCommonPoints) && (!JudgeIfInterSect))
	{
		PointInLivingRegionPs = JudgePointOnOrInPolygon(UpdateLivingRegionPs, ModelCenter);
		PointInDiningRegionPs = JudgePointOnOrInPolygon(UpdateDiningRegionPs, ModelCenter);
		if ((PointInLivingRegionPs) && (!PointInDiningRegionPs))
		{
			Result = 1;
			return PointInLivingRegionPs;
		}
		if ((PointInDiningRegionPs) && (!PointInLivingRegionPs))
		{
			Result = 2;
			return PointInDiningRegionPs;
		}
		if ((PointInDiningRegionPs) && (PointInLivingRegionPs))
		{
			Result = 1;
			return PointInLivingRegionPs;
		}

		if ((!PointInLivingRegionPs) && (!PointInDiningRegionPs))
		{
			int32 NextIndex = 0;
			for (int32 Index = 0; Index < UpdateLivingRegionPs.Num(); ++Index)
			{
				if (Index == UpdateLivingRegionPs.Num() - 1)
				{
					NextIndex = 0;
				}
				else
				{
					NextIndex = Index + 1;
				}
				float PointToSeg = FMath::PointDistToSegment(FVector(ModelCenter, 0), FVector(UpdateLivingRegionPs[Index], 0), FVector(UpdateLivingRegionPs[NextIndex], 0));
				if (PointToSeg <= PointToSegDisLimit)
				{
					PointInLivingRegionPs = true;
					Result = 1;
					break;
				}
			}
			if (PointInLivingRegionPs)
			{
				return PointInLivingRegionPs;
			}
			if (!PointInLivingRegionPs)
			{
				int32 NextDinIndex = 0;
				for (int32 Index = 0; Index < UpdateDiningRegionPs.Num(); ++Index)
				{
					if (Index == UpdateDiningRegionPs.Num() - 1)
					{
						NextDinIndex = 0;
					}
					else
					{
						NextDinIndex = Index + 1;
					}
					float PointToSeg = FMath::PointDistToSegment(FVector(ModelCenter, 0), FVector(UpdateDiningRegionPs[Index], 0), FVector(UpdateDiningRegionPs[NextDinIndex], 0));
					if (PointToSeg <= PointToSegDisLimit)
					{
						PointInDiningRegionPs = true;
						Result = 2;
						break;
					}
				}
			}
			if (PointInDiningRegionPs)
			{
				return PointInDiningRegionPs;
			}
		}
		if ((!PointInLivingRegionPs) && (!PointInDiningRegionPs))
		{
			Result = 0;
			return false;
		}
	}
	else
	{
		//judge if ModelCenter lies in the region of LivingRoom
		PointInLivingRegionPs = JudgePointOnOrInPolygon(UpdateLivingRegionPs, ModelCenter);
		//judge if ModelCenter lies in the region of DiningRoom
		PointInDiningRegionPs = JudgePointOnOrInPolygon(UpdateDiningRegionPs, ModelCenter);


		if ((PointInLivingRegionPs) && (!PointInDiningRegionPs))
		{
			Result = 1;
			return PointInLivingRegionPs;
		}
		if ((PointInDiningRegionPs) && (!PointInLivingRegionPs))
		{
			Result = 2;
			return PointInDiningRegionPs;
		}
		if ((PointInDiningRegionPs) && (PointInLivingRegionPs))
		{
			Result = 1;
			return PointInLivingRegionPs;
		}

		TArray<int> ToDelLivingEdgeIndexs;
		int32 NextIndex = 0;
		for (int32 Index = 0; Index < UpdateLivingRegionPs.Num(); ++Index)
		{
			if (Index == UpdateLivingRegionPs.Num() - 1)
			{
				NextIndex = 0;
			}
			else
			{
				NextIndex = Index + 1;
			}
			FVector2D EdgeMiddlePoint = 0.5*(UpdateLivingRegionPs[Index] + UpdateLivingRegionPs[NextIndex]);
			FVector2D EdgeVec = UpdateLivingRegionPs[NextIndex] - UpdateLivingRegionPs[Index];
			EdgeVec = EdgeVec.GetSafeNormal();
			FVector2D EdgeRotate = EdgeVec.GetRotated(90);
			FVector2D TryPoint1 = EdgeMiddlePoint + 10 * EdgeRotate;
			FVector2D TryPoint2 = EdgeMiddlePoint - 10 * EdgeRotate;
			if ((JudgePointOnOrInPolygon(UpdateDiningRegionPs, TryPoint1)) || (JudgePointOnOrInPolygon(UpdateDiningRegionPs, TryPoint2)))
			{
				ToDelLivingEdgeIndexs.AddUnique(Index);
			}
		}


		int32 NextLivingEIndex = 0;
		for (int32 Index = 0; Index < UpdateLivingRegionPs.Num(); Index++)
		{
			if (!ToDelLivingEdgeIndexs.Contains(Index))
			{
				if (Index == UpdateLivingRegionPs.Num() - 1)
				{
					NextLivingEIndex = 0;
				}
				else
				{
					NextLivingEIndex = Index + 1;
				}
				float PointToSeg = FMath::PointDistToSegment(FVector(ModelCenter, 0), FVector(UpdateLivingRegionPs[Index], 0), FVector(UpdateLivingRegionPs[NextLivingEIndex], 0));
				if (PointToSeg <= PointToSegDisLimit)
				{
					PointInLivingRegionPs = true;
					Result = 1;
					break;
				}
			}
		}
		if (PointInLivingRegionPs)
		{
			return PointInLivingRegionPs;
		}

		TArray<int> ToDelDiningEdgeIndexs;
		int32 NextDiningIndex = 0;
		for (int32 Index = 0; Index < UpdateDiningRegionPs.Num(); ++Index)
		{
			if (Index == UpdateDiningRegionPs.Num() - 1)
			{
				NextDiningIndex = 0;
			}
			else
			{
				NextDiningIndex = Index + 1;
			}
			FVector2D EdgeDinMiddlePoint = 0.5*(UpdateDiningRegionPs[Index] + UpdateDiningRegionPs[NextDiningIndex]);
			FVector2D EdgeDinVec = UpdateDiningRegionPs[NextDiningIndex] - UpdateDiningRegionPs[Index];
			EdgeDinVec = EdgeDinVec.GetSafeNormal();
			FVector2D EdgeDinRotate = EdgeDinVec.GetRotated(90);
			FVector2D TryDinPoint1 = EdgeDinMiddlePoint + 10 * EdgeDinRotate;
			FVector2D TryDinPoint2 = EdgeDinMiddlePoint - 10 * EdgeDinRotate;
			if ((JudgePointOnOrInPolygon(UpdateLivingRegionPs, TryDinPoint1)) || (JudgePointOnOrInPolygon(UpdateLivingRegionPs, TryDinPoint2)))
			{
				ToDelDiningEdgeIndexs.AddUnique(Index);
			}
		}


		int32 NextEIndex = 0;
		for (int32 Index = 0; Index < UpdateDiningRegionPs.Num(); Index++)
		{
			if (!ToDelDiningEdgeIndexs.Contains(Index))
			{
				if (Index == UpdateDiningRegionPs.Num() - 1)
				{
					NextEIndex = 0;
				}
				else
				{
					NextEIndex = Index + 1;
				}
				float PointToSeg = FMath::PointDistToSegment(FVector(ModelCenter, 0), FVector(UpdateDiningRegionPs[Index], 0), FVector(UpdateDiningRegionPs[NextEIndex], 0));
				if (PointToSeg <= PointToSegDisLimit)
				{
					PointInDiningRegionPs = true;
					Result = 2;
					break;
				}
			}
		}
		if (PointInDiningRegionPs)
		{
			return PointInDiningRegionPs;
		}

		if ((!PointInLivingRegionPs) && (!PointInDiningRegionPs))
		{
			Result = 0;
			return false;
		}
	}
	return true;
}


bool FPolygonAlg::ReOrderModelCenters(TArray<FVector2D> RegionPs, TArray<FVector> ModelCenters, TArray<FVector>& OrderedModelCenters, const float DisOfClip)
{
	using namespace Triangulation;
	if ((RegionPs.Num() <= 2) || (ModelCenters.Num() == 0))
	{
		return false;
	}
	TArray<FVector2D> UpdateRegionPs;
	bool MergeSuccess = FPolygonAlg::MergeCollinearPoints(RegionPs, UpdateRegionPs);
	if (!MergeSuccess)
	{
		return false;
	}

	Polygon_2 RegionPoly;
	for (int _CurI = 0; _CurI < UpdateRegionPs.Num(); ++_CurI)
	{
		RegionPoly.push_back(Point_2(UpdateRegionPs[_CurI].X, UpdateRegionPs[_CurI].Y));
	}
	TMap<int, TArray<FVector>> EdgeIndexMapModelCenters;
	int NextP = 0, NextNextP = 0, FrontP = 0;
	for (int CurP = 0; CurP < UpdateRegionPs.Num(); ++CurP)
	{
		if (CurP == UpdateRegionPs.Num() - 1)
		{
			NextP = 0;
			NextNextP = NextP + 1;
			FrontP = CurP - 1;
		}
		else if (CurP == 0)
		{
			NextP = CurP + 1;
			NextNextP = NextP + 1;
			FrontP = UpdateRegionPs.Num() - 1;
		}
		else if (CurP == UpdateRegionPs.Num() - 2)
		{
			NextP = CurP + 1;
			NextNextP = 0;
			FrontP = CurP - 1;
		}
		else
		{
			NextP = CurP + 1;
			NextNextP = NextP + 1;
			FrontP = CurP - 1;
		}
		FVector2D Dir1 = UpdateRegionPs[NextNextP] - UpdateRegionPs[NextP];
		FVector2D Dir2 = UpdateRegionPs[FrontP] - UpdateRegionPs[CurP];
		Dir1 = Dir1.GetSafeNormal();
		Dir2 = Dir2.GetSafeNormal();

		FVector2D EdgeGoVec = UpdateRegionPs[NextP] - UpdateRegionPs[CurP];
		FVector2D NextEdgeGoVec = UpdateRegionPs[NextNextP] - UpdateRegionPs[NextP];
		FVector2D FrontToCurVec = UpdateRegionPs[FrontP] - UpdateRegionPs[CurP];

		EdgeGoVec = EdgeGoVec.GetSafeNormal();
		NextEdgeGoVec = NextEdgeGoVec.GetSafeNormal();
		FrontToCurVec = FrontToCurVec.GetSafeNormal();

		FVector2D POnCurToNext = UpdateRegionPs[CurP] + 2 * EdgeGoVec;
		FVector2D POnCurToFront = UpdateRegionPs[CurP] + 2 * FrontToCurVec;
		FVector2D TryP1 = 0.5*(POnCurToNext + POnCurToFront);
		FVector2D Ver1 = TryP1 - UpdateRegionPs[CurP];
		Ver1 = Ver1.GetSafeNormal();

		FVector2D POnNextToCur = UpdateRegionPs[NextP] - 2 * EdgeGoVec;
		FVector2D POnNextToNextNext = UpdateRegionPs[NextP] + 2 * NextEdgeGoVec;
		FVector2D TryP2 = 0.5*(POnNextToCur + POnNextToNextNext);
		FVector2D Ver2 = TryP2 - UpdateRegionPs[NextP];
		Ver2 = Ver2.GetSafeNormal();


		FVector2D TestP1 = UpdateRegionPs[CurP] + DisOfClip * Ver1;
		FVector2D TestP2 = UpdateRegionPs[NextP] + DisOfClip * Ver2;
		if (!FPolygonAlg::JudgePointOnOrInPolygon(UpdateRegionPs, TestP1))
		{
			TestP1 = UpdateRegionPs[CurP] - DisOfClip * Ver1;
		}
		if (!FPolygonAlg::JudgePointOnOrInPolygon(UpdateRegionPs, TestP2))
		{
			TestP2 = UpdateRegionPs[NextP] - DisOfClip * Ver2;
		}

		Polygon_2 TempPoly;
		TempPoly.push_back(Point_2(UpdateRegionPs[CurP].X, UpdateRegionPs[CurP].Y));
		TempPoly.push_back(Point_2(UpdateRegionPs[NextP].X, UpdateRegionPs[NextP].Y));
		TempPoly.push_back(Point_2(TestP2.X, TestP2.Y));
		TempPoly.push_back(Point_2(TestP1.X, TestP1.Y));

		TArray<FVector> TempSet;

		for (int ModelIndex = 0; ModelIndex < ModelCenters.Num(); ++ModelIndex)
		{
			if (TempPoly.has_on_bounded_side(Point_2(ModelCenters[ModelIndex].X, ModelCenters[ModelIndex].Y)))
			{
				TempSet.AddUnique(ModelCenters[ModelIndex]);
			}
		}
		TArray<float> TempDisSet;
		TMap<int, float>IndexMapDis;
		for (int K = 0; K < TempSet.Num(); ++K)
		{
			FVector2D CenterToEdgeStart(TempSet[K].X - UpdateRegionPs[CurP].X, TempSet[K].Y - UpdateRegionPs[CurP].Y);
			float TempValue = FVector2D::DotProduct(CenterToEdgeStart, EdgeGoVec);
			TempDisSet.Add(TempValue);
			IndexMapDis.Add(K, TempValue);
		}
		TempDisSet.Sort();
		TArray<FVector> ReorderTempSet;

		TArray<int> CollectedIndexs;
		for (int Index = 0; Index < TempDisSet.Num(); ++Index)
		{

			for (int J = 0; J < IndexMapDis.Num(); ++J)
			{
				if (!CollectedIndexs.Contains(J))
				{
					if (IndexMapDis[J] == TempDisSet[Index])
					{
						CollectedIndexs.Add(J);
						ReorderTempSet.Add(TempSet[J]);
						break;
					}
				}

			}

		}
		if (ReorderTempSet.Num() != 0)
		{
			EdgeIndexMapModelCenters.Add(CurP, ReorderTempSet);
		}
		TempSet.Empty();
	}
	float PolyArea = RegionPoly.area();
	TArray<FVector> TempCollectPs;
	if (PolyArea > 0)
	{
		TArray<FVector> TempArray;
		for (int _CurP = 0; _CurP < UpdateRegionPs.Num(); ++_CurP)
		{
			if (EdgeIndexMapModelCenters.Contains(_CurP))
			{
				TArray<FVector> TempGroupP = EdgeIndexMapModelCenters[_CurP];
				for (int PIndex = 0; PIndex <= TempGroupP.Num() - 1; ++PIndex)
				{
					if (!TempCollectPs.Contains(TempGroupP[PIndex]))
					{
						TempCollectPs.AddUnique(TempGroupP[PIndex]);
						TempArray.Add(TempGroupP[PIndex]);
					}

				}
			}
		}
		for (int I = 0; I < TempArray.Num(); ++I)
		{
			int J = TempArray.Num() - 1 - I;
			OrderedModelCenters.Add(TempArray[J]);
		}
	}
	else
	{
		for (int _CurP = 0; _CurP < UpdateRegionPs.Num(); ++_CurP)
		{
			if (EdgeIndexMapModelCenters.Contains(_CurP))
			{
				TArray<FVector> TempGroupP = EdgeIndexMapModelCenters[_CurP];
				for (int PIndex = 0; PIndex <= TempGroupP.Num() - 1; ++PIndex)
				{
					if (!TempCollectPs.Contains(TempGroupP[PIndex]))
					{
						TempCollectPs.AddUnique(TempGroupP[PIndex]);
						OrderedModelCenters.Add(TempGroupP[PIndex]);
					}
				}
			}
		}
	}
	return true;
}


bool FPolygonAlg::CheckRegionUseness(TArray<FVector2D> RegionPs)
{
	using namespace Triangulation;
	const float PointEqualTolerance = 1.0E-3;
	int NextI = 0;
	TArray<Segment_2> AllSegs;
	for (int _CurI = 0; _CurI < RegionPs.Num(); ++_CurI)
	{
		if (_CurI == RegionPs.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = _CurI + 1;
		}
		Point_2 PStart(RegionPs[_CurI].X, RegionPs[_CurI].Y);
		Point_2 PEnd(RegionPs[NextI].X, RegionPs[NextI].Y);
		Segment_2 SegEdge(PStart, PEnd);
		AllSegs.Add(SegEdge);
	}
	for (int SegIndex = 0; SegIndex < AllSegs.Num(); ++SegIndex)
	{
		for (int _CurJ = 0; _CurJ < AllSegs.Num(); ++_CurJ)
		{
			if (_CurJ != SegIndex)
			{
				CGAL::cpp11::result_of<Intersect_2(Segment_2, Segment_2)>::type Result = intersection(AllSegs[SegIndex], AllSegs[_CurJ]);
				if (Result)
				{
					if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
					{
						float DisToSegStart = sqrt(CGAL::squared_distance(AllSegs[SegIndex].source(), *TempIntersectP));
						float DisToSegEnd = sqrt(CGAL::squared_distance(AllSegs[SegIndex].target(), *TempIntersectP));
						if ((DisToSegStart < PointEqualTolerance) || (DisToSegEnd < PointEqualTolerance))
						{
							continue;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}


bool FPolygonAlg::StrictPolygonUnion(TArray<FVector2D> PolygonOne, TArray<FVector2D> PolygonTwo, TArray<FVector2D>& PolygonUnion)
{
	using namespace Triangulation;
	Polygon_2 Poly1, Poly2;
	Polygon_with_holes_2 UnionR;
	for (int _CurI = 0; _CurI < PolygonOne.Num(); ++_CurI)
	{
		Poly1.push_back(Point_2(PolygonOne[_CurI].X, PolygonOne[_CurI].Y));
	}
	for (int _CurJ = 0; _CurJ < PolygonTwo.Num(); ++_CurJ)
	{
		Poly2.push_back(Point_2(PolygonTwo[_CurJ].X, PolygonTwo[_CurJ].Y));
	}
	if (CGAL::join(Poly1, Poly2, UnionR))
	{
		Polygon_2 TempP = UnionR.outer_boundary();
		for (auto vBegin = TempP.vertices_begin(), vEnd = TempP.vertices_end(); vBegin != vEnd; ++vBegin)
		{
			PolygonUnion.Add(FVector2D((*vBegin).x(), (*vBegin).y()));
		}
	}
	else
	{
		return false;
	}
	return true;
}

//bool FPolygonAlg::JudgePolygonsSnap(TArray<FVector2D> PolygonOne, TArray<FVector2D>PolygonTwo, TArray<FVector2D>&RegionPs, TArray<FVector2D>&ComponentPs)
//{
//	using namespace Triangulation;
//	Polygon_2 Poly1, Poly2;
//	TArray<FVector2D> UpdatePolygonOne, UpdatePolygonTwo;
//	FPolygonAlg::MergeCollinearPoints(PolygonOne, UpdatePolygonOne);
//	FPolygonAlg::MergeCollinearPoints(PolygonTwo, UpdatePolygonTwo);
//	bool IfSnap = false;
//	for (int _CurI = 0; _CurI < UpdatePolygonOne.Num(); ++_CurI)
//	{
//		Poly1.push_back(Point_2(UpdatePolygonOne[_CurI].X, UpdatePolygonOne[_CurI].Y));
//	}
//	for (int _CurJ = 0; _CurJ < UpdatePolygonTwo.Num(); ++_CurJ)
//	{
//		Poly2.push_back(Point_2(UpdatePolygonTwo[_CurJ].X, UpdatePolygonTwo[_CurJ].Y));
//	}
//	float Area1 = abs(Poly1.area());
//	float Area2 = abs(Poly2.area());
//	Polygon_2 RegionPoly, ComponentPoly;
//	TArray<Segment_2>ComponentSegs, RegionSegs;
//	if (Area1 > Area2)
//	{
//		RegionPoly = Poly1;
//		ComponentPoly = Poly2;
//
//	}
//	else
//	{
//		RegionPoly = Poly2;
//		ComponentPoly = Poly1;
//		TArray<FVector2D> TempArray;
//		TempArray = UpdatePolygonOne;
//		UpdatePolygonTwo = UpdatePolygonOne;
//		UpdatePolygonOne = TempArray;
//	}
//	int NextAIndex = 0;
//	for (int SegAIndex = 0; SegAIndex < UpdatePolygonOne.Num(); ++SegAIndex)
//	{
//		if (SegAIndex == UpdatePolygonOne.Num() - 1)
//		{
//			NextAIndex = 0;
//		}
//		else
//		{
//			NextAIndex = SegAIndex + 1;
//		}
//		Point_2 PA(UpdatePolygonOne[SegAIndex].X, UpdatePolygonOne[SegAIndex].Y);
//		Point_2 PB(UpdatePolygonOne[NextAIndex].X, UpdatePolygonOne[NextAIndex].Y);
//		Segment_2 TempSeg(PA, PB);
//		RegionSegs.Add(TempSeg);
//	}
//	int NextBIndex = 0;
//	for (int SegBIndex = 0; SegBIndex < UpdatePolygonTwo.Num(); ++SegBIndex)
//	{
//		if (SegBIndex == UpdatePolygonTwo.Num() - 1)
//		{
//			NextBIndex = 0;
//		}
//		else
//		{
//			NextBIndex = SegBIndex + 1;
//		}
//		Point_2 PA(UpdatePolygonTwo[SegBIndex].X, UpdatePolygonOne[SegBIndex].Y);
//		Point_2 PB(UpdatePolygonTwo[NextBIndex].X, UpdatePolygonOne[NextBIndex].Y);
//		Segment_2 TempSeg(PA, PB);
//		ComponentSegs.Add(TempSeg);
//	}
//
//	TArray<int> RegionSegsToRemove, ComponentSegsToRemove;
//	TMap<int, int> ComponentSegMapRegionSeg;
//	for (int _CurSeg = 0; _CurSeg < ComponentSegs.Num(); ++_CurSeg)
//	{
//		for (int _CurRegionSeg = 0; _CurRegionSeg < RegionSegs.Num(); ++_CurRegionSeg)
//		{
//			float SegStartToRegion = sqrt(CGAL::squared_distance(ComponentSegs[_CurSeg].source(), RegionSegs[_CurRegionSeg]));
//			float SegEndToRegion = sqrt(CGAL::squared_distance(ComponentSegs[_CurSeg].target(), RegionSegs[_CurRegionSeg]));
//			if ((SegStartToRegion < 0.1) && (SegEndToRegion < 0.1))
//			{
//				RegionSegsToRemove.AddUnique(_CurRegionSeg);
//				ComponentSegsToRemove.AddUnique(_CurSeg);
//				ComponentSegMapRegionSeg.Add(_CurSeg, _CurRegionSeg);
//				break;
//			}
//		}
//	}
//	TArray<Segment_2> AddRegionSegs, AddComponentSegs;
//	if (ComponentSegsToRemove.Num() == 1)
//	{
//		Point_2 RegionInterSegStart = RegionSegs[RegionSegsToRemove[0]].source();
//		Point_2 RegionInterSegEnd = RegionSegs[RegionSegsToRemove[0]].target();
//		Point_2 ComponentInterSegStart = ComponentSegs[ComponentSegsToRemove[0]].source();
//		Point_2 ComponentInterSegEnd = ComponentSegs[ComponentSegsToRemove[0]].target();
//		float DisToP1 = CGAL::squared_distance(RegionInterSegStart, ComponentInterSegStart);
//		float DisToP2 = CGAL::squared_distance(RegionInterSegStart, ComponentInterSegEnd);
//		if (DisToP1 < DisToP2)
//		{
//			Segment_2 TempRegionSeg1(RegionInterSegStart, ComponentInterSegStart);
//			Segment_2 TempRegionSeg2(RegionInterSegEnd, ComponentInterSegEnd);
//			AddRegionSegs.Add(TempRegionSeg1);
//			AddRegionSegs.Add(TempRegionSeg2);
//		}
//		else
//		{
//			Segment_2 TempRegionSeg1(RegionInterSegStart, ComponentInterSegEnd);
//			Segment_2 TempRegionSeg2(RegionInterSegEnd, ComponentInterSegStart);
//			AddRegionSegs.Add(TempRegionSeg1);
//			AddRegionSegs.Add(TempRegionSeg2);
//		}
//		IfSnap = true;
//	}
//	else if (ComponentSegsToRemove.Num() == 2)
//	{
//		for (int I = 0; I < ComponentSegsToRemove.Num(); ++I)
//		{
//			int RegionMapSegIndex = ComponentSegMapRegionSeg[ComponentSegsToRemove[I]];
//			Point_2 RegionInterSegStart = RegionSegs[RegionSegsToRemove[RegionMapSegIndex]].source();
//			Point_2 RegionInterSegEnd = RegionSegs[RegionSegsToRemove[RegionMapSegIndex]].target();
//			Point_2 ComponentInterSegStart = ComponentSegs[ComponentSegsToRemove[I]].source();
//			Point_2 ComponentInterSegEnd = ComponentSegs[ComponentSegsToRemove[I]].target();
//
//			float DisToP1 = CGAL::squared_distance(RegionInterSegStart, ComponentInterSegStart);
//			float DisToP2 = CGAL::squared_distance(RegionInterSegStart, ComponentInterSegEnd);
//			float DisToP3 = CGAL::squared_distance(RegionInterSegEnd, ComponentInterSegStart);
//			float DisToP4 = CGAL::squared_distance(RegionInterSegEnd, ComponentInterSegEnd);
//			if (DisToP1 < 0.1)
//			{
//				Segment_2 TempRegionSeg1(RegionInterSegEnd, ComponentInterSegEnd);
//				AddRegionSegs.Add(TempRegionSeg1);
//			}
//			else if (DisToP2 < 0.1)
//			{
//				Segment_2 TempRegionSeg1(RegionInterSegEnd, ComponentInterSegStart);
//				AddRegionSegs.Add(TempRegionSeg1);
//			}
//			else if (DisToP3 < 0.1)
//			{
//				Segment_2 TempRegionSeg1(RegionInterSegStart, ComponentInterSegEnd);
//				AddRegionSegs.Add(TempRegionSeg1);
//			}
//			else if (DisToP4 < 0.1)
//			{
//				Segment_2 TempRegionSeg1(RegionInterSegStart, ComponentInterSegStart);
//				AddRegionSegs.Add(TempRegionSeg1);
//			}
//		}
//		IfSnap = true;
//	}
//	else
//	{
//		IfSnap = false;
//	}
//
//	if (IfSnap)
//	{
//		for (int I = RegionSegsToRemove.Num() - 1; I >= 0; --I)
//		{
//			RegionSegs.RemoveAt(I);
//		}
//		for (int Index = 0; Index < AddRegionSegs.Num(); ++Index)
//		{
//			RegionSegs.Add(AddRegionSegs[Index]);
//		}
//		for (int J = ComponentSegsToRemove.Num() - 1; J >= 0; --J)
//		{
//			ComponentSegs.RemoveAt(J);
//		}
//		for (int K = 0; K < AddComponentSegs.Num(); ++K)
//		{
//			ComponentSegs.Add(AddComponentSegs[K]);
//		}
//
//		TArray<int> CollectNewRegionSegIndex, CollectNewComponentSegIndex;
//		CollectNewRegionSegIndex.AddUnique(0);
//		CollectNewComponentSegIndex.AddUnique(0);
//		RegionPs.Add(FVector2D(RegionSegs[0].source().x(), RegionSegs[0].source().y()));
//		RegionPs.Add(FVector2D(RegionSegs[0].target().x(), RegionSegs[0].target().y()));
//		ComponentPs.Add(FVector2D(ComponentSegs[0].source().x(), ComponentSegs[0].source().y()));
//		ComponentPs.Add(FVector2D(ComponentSegs[0].target().x(), ComponentSegs[0].target().y()));
//		while (CollectNewRegionSegIndex.Num() != RegionSegs.Num())
//		{
//			for (int _CurI = 0; _CurI < RegionSegs.Num(); ++_CurI)
//			{
//				if (!CollectNewRegionSegIndex.Contains(_CurI))
//				{
//					FVector2D TempPoint = RegionPs.Last();
//
//					float TempDisToP1 = sqrt(CGAL::squared_distance(Point_2(TempPoint.X, TempPoint.Y), RegionSegs[_CurI].source()));
//					float TempDisToP2 = sqrt(CGAL::squared_distance(Point_2(TempPoint.X, TempPoint.Y), RegionSegs[_CurI].target()));
//
//					if (TempDisToP1 < 0.1)
//					{
//						CollectNewRegionSegIndex.AddUnique(_CurI);
//						RegionPs.Add(FVector2D(RegionSegs[_CurI].target().x(), RegionSegs[_CurI].target().y()));
//						break;
//					}
//					else if (TempDisToP2 < 0.1)
//					{
//						CollectNewRegionSegIndex.AddUnique(_CurI);
//						RegionPs.Add(FVector2D(RegionSegs[_CurI].source().x(), RegionSegs[_CurI].source().y()));
//						break;
//					}
//				}
//			}
//		}
//
//		while (CollectNewComponentSegIndex.Num() != ComponentSegs.Num())
//		{
//			for (int _CurI = 0; _CurI < ComponentSegs.Num(); ++_CurI)
//			{
//				if (!CollectNewComponentSegIndex.Contains(_CurI))
//				{
//					FVector2D TempComPoint = ComponentPs.Last();
//
//					float TempDisToP1 = sqrt(CGAL::squared_distance(Point_2(TempComPoint.X, TempComPoint.Y), ComponentSegs[_CurI].source()));
//					float TempDisToP2 = sqrt(CGAL::squared_distance(Point_2(TempComPoint.X, TempComPoint.Y), ComponentSegs[_CurI].target()));
//
//					if (TempDisToP1 < 0.1)
//					{
//						CollectNewComponentSegIndex.AddUnique(_CurI);
//						ComponentPs.Add(FVector2D(ComponentSegs[_CurI].target().x(), ComponentSegs[_CurI].target().y()));
//						break;
//					}
//					else if (TempDisToP2 < 0.1)
//					{
//						CollectNewComponentSegIndex.AddUnique(_CurI);
//						ComponentPs.Add(FVector2D(ComponentSegs[_CurI].source().x(), ComponentSegs[_CurI].source().y()));
//						break;
//					}
//				}
//			}
//		}
//
//
//
//	}
//
//	return IfSnap;
//}



bool FPolygonAlg::LocateLightsOfDiningOrLivingRegion(const TArray<FVector2D> InPnts, TArray<FVector>&LightsLoc, const float WallHeight, const float ReduceHeight)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;
	const float ShrinkLength = 20 * sqrt(2);
	TArray<FVector2D> OutPnts;
	bool MergeCollinearSuccess = MergeCollinearPoints(InPnts, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}

	//bool MergeSuccess = MergeShortEdges(TempOutPnts, OutPnts);
	//if (!MergeSuccess)
	//{
	//	return false;
	//}
	FVector2D SuitableCenter;
	if (OutPnts.Num() <= 3)
	{
		return false;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			SuitableCenter = IBetterCenter;
			float Distance1 = FVector2D::Distance(OutPnts[0], SuitableCenter);
			float Distance2 = FVector2D::Distance(OutPnts[1], SuitableCenter);
			float Distance3 = FVector2D::Distance(OutPnts[2], SuitableCenter);
			float Distance4 = FVector2D::Distance(OutPnts[3], SuitableCenter);
			float ShrinkAge1 = (Distance1 - ShrinkLength) / Distance1;
			float ShrinkAge2 = (Distance2 - ShrinkLength) / Distance2;
			float ShrinkAge3 = (Distance3 - ShrinkLength) / Distance3;
			float ShrinkAge4 = (Distance4 - ShrinkLength) / Distance4;
			FVector2D UpdateOutPnt1 = (1 - ShrinkAge1)*SuitableCenter + ShrinkAge1 * OutPnts[0];
			FVector2D UpdateOutPnt2 = (1 - ShrinkAge2)*SuitableCenter + ShrinkAge2 * OutPnts[1];
			FVector2D UpdateOutPnt3 = (1 - ShrinkAge3)*SuitableCenter + ShrinkAge3 * OutPnts[2];
			FVector2D UpdateOutPnt4 = (1 - ShrinkAge4)*SuitableCenter + ShrinkAge4 * OutPnts[3];

			FVector LocOfLight1 = FVector(0.5*(UpdateOutPnt1 + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight2 = FVector(0.5*(UpdateOutPnt2 + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight3 = FVector(0.5*(UpdateOutPnt3 + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight4 = FVector(0.5*(UpdateOutPnt4 + SuitableCenter), WallHeight - ReduceHeight);
			LightsLoc.Add(LocOfLight1);
			LightsLoc.Add(LocOfLight2);
			LightsLoc.Add(LocOfLight3);
			LightsLoc.Add(LocOfLight4);
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			SuitableCenter = TriangleCenters[TargetIndex];

			FVector2D MiddleP1 = FMath::ClosestPointOnSegment2D(SuitableCenter, OutPnts[0], OutPnts[1]);
			FVector2D MiddleP2 = FMath::ClosestPointOnSegment2D(SuitableCenter, OutPnts[1], OutPnts[2]);
			FVector2D MiddleP3 = FMath::ClosestPointOnSegment2D(SuitableCenter, OutPnts[2], OutPnts[3]);
			FVector2D MiddleP4 = FMath::ClosestPointOnSegment2D(SuitableCenter, OutPnts[3], OutPnts[0]);
			FVector LocOfLight1 = FVector(0.5*(MiddleP1 + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight2 = FVector(0.5*(MiddleP2 + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight3 = FVector(0.5*(MiddleP3 + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight4 = FVector(0.5*(MiddleP4 + SuitableCenter), WallHeight - ReduceHeight);
			LightsLoc.Add(LocOfLight1);
			LightsLoc.Add(LocOfLight2);
			LightsLoc.Add(LocOfLight3);
			LightsLoc.Add(LocOfLight4);

			return true;
		}
	}
	else
	{

		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		TMap<FVector2D, TArray<FVector2D>> MaxCenterMapBounds;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				bool FindLocalCenter = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 4.0))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;

					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PStart]);
					TempPointArray.Add(OutPnts[PNext]);
					MaxCenterMapBounds.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					FindLocalCenter = true;
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 4.0))
				{

					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;

					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempPointArray.Add(OutPnts[PNext]);
					TempPointArray.Add(OutPnts[PStart]);
					MaxCenterMapBounds.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					FindLocalCenter = true;
				}



				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (FindLocalCenter))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}




		float MaxArea = 0.0;
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] != 0.0)
			{
				if (AreaSet[CenterIndex] > MaxArea)
				{
					MaxArea = AreaSet[CenterIndex];
					TargetIndex = CenterIndex;
				}
			}

		}

		if (MaxArea >= 150 * 100)
		{
			SuitableCenter = AreaCenters[TargetIndex];
			TArray<FVector2D> BoundPs = MaxCenterMapBounds[SuitableCenter];
			for (int _CurBoundP = 0; _CurBoundP < BoundPs.Num(); ++_CurBoundP)
			{
				float TempDistance = FVector2D::Distance(BoundPs[_CurBoundP], SuitableCenter);
				float TempShrinkAge = (TempDistance - ShrinkLength) / TempDistance;
				FVector2D UpdateOutPnt = (1 - TempShrinkAge)*SuitableCenter + TempShrinkAge * BoundPs[_CurBoundP];

				FVector TempLoc = FVector(0.5*(UpdateOutPnt + SuitableCenter), WallHeight - ReduceHeight);
				LightsLoc.Add(TempLoc);
			}
		}
		else
		{
			float MinX = OutPnts[0].X;
			float MaxX = OutPnts[0].X;
			float MinY = OutPnts[0].Y;
			float MaxY = OutPnts[0].Y;
			for (int _CurP = 0; _CurP < OutPnts.Num(); ++_CurP)
			{
				if (MinX > OutPnts[_CurP].X)
				{
					MinX = OutPnts[_CurP].X;
				}
				if (MaxX < OutPnts[_CurP].X)
				{
					MaxX = OutPnts[_CurP].X;
				}
				if (MinY > OutPnts[_CurP].Y)
				{
					MinY = OutPnts[_CurP].Y;
				}
				if (MaxY < OutPnts[_CurP].Y)
				{
					MaxY = OutPnts[_CurP].Y;
				}
			}
			FVector2D PLeftUp = FVector2D(MinX, MaxY);
			FVector2D PLeftDown = FVector2D(MinX, MinY);
			FVector2D PRightDown = FVector2D(MaxX, MinY);
			FVector2D PRightUp = FVector2D(MaxX, MaxY);
			FVector2D TempCenter = 0.25*(PLeftUp + PLeftDown + PRightDown + PRightUp);
			if (FPolygonAlg::JudgePointInPolygon(OutPnts, TempCenter))
			{
				SuitableCenter = TempCenter;
				float Distance1 = FVector2D::Distance(PLeftUp, SuitableCenter);
				float Distance2 = FVector2D::Distance(PLeftDown, SuitableCenter);
				float Distance3 = FVector2D::Distance(PRightDown, SuitableCenter);
				float Distance4 = FVector2D::Distance(PRightUp, SuitableCenter);
				float ShrinkAge1 = (Distance1 - ShrinkLength) / Distance1;
				float ShrinkAge2 = (Distance2 - ShrinkLength) / Distance2;
				float ShrinkAge3 = (Distance3 - ShrinkLength) / Distance3;
				float ShrinkAge4 = (Distance4 - ShrinkLength) / Distance4;
				FVector2D UpdateOutPnt1 = (1 - ShrinkAge1)*SuitableCenter + ShrinkAge1 * PLeftUp;
				FVector2D UpdateOutPnt2 = (1 - ShrinkAge2)*SuitableCenter + ShrinkAge2 * PLeftDown;
				FVector2D UpdateOutPnt3 = (1 - ShrinkAge3)*SuitableCenter + ShrinkAge3 * PRightDown;
				FVector2D UpdateOutPnt4 = (1 - ShrinkAge4)*SuitableCenter + ShrinkAge4 * PRightUp;

				FVector LocOfLight1 = FVector(0.5*(UpdateOutPnt1 + SuitableCenter), WallHeight - ReduceHeight);
				FVector LocOfLight2 = FVector(0.5*(UpdateOutPnt2 + SuitableCenter), WallHeight - ReduceHeight);
				FVector LocOfLight3 = FVector(0.5*(UpdateOutPnt3 + SuitableCenter), WallHeight - ReduceHeight);
				FVector LocOfLight4 = FVector(0.5*(UpdateOutPnt4 + SuitableCenter), WallHeight - ReduceHeight);
				LightsLoc.Add(LocOfLight1);
				LightsLoc.Add(LocOfLight2);
				LightsLoc.Add(LocOfLight3);
				LightsLoc.Add(LocOfLight4);

			}
		}

	}
	return true;
}

bool FPolygonAlg::LocateLightsOfRoom(const TArray<FVector2D> InPnts, TArray<FVector>&LightsLoc, const float WallHeight, const float ReduceHeight)
{
	using namespace Triangulation;
	FVector2D SuitableCenter;
	CDTDS Mycdt;

	TArray<FVector2D>  TempPoints, OutPnts;
	const float ShrinkLength = 20 * sqrt(2);
	bool MergeShortSuccess = MergeShrotEdgeCmSize(InPnts, TempPoints);
	if (!MergeShortSuccess)
	{
		return false;
	}

	bool MergeCollinearSuccess = MergeCollinearPoints(TempPoints, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}
	if (OutPnts.Num() <= 3)
	{
		return false;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			SuitableCenter = IBetterCenter;
			float Distance1 = FVector2D::Distance(OutPnts[0], SuitableCenter);
			float Distance2 = FVector2D::Distance(OutPnts[1], SuitableCenter);
			float Distance3 = FVector2D::Distance(OutPnts[2], SuitableCenter);
			float Distance4 = FVector2D::Distance(OutPnts[3], SuitableCenter);
			float ShrinkAge1 = (Distance1 - ShrinkLength) / Distance1;
			float ShrinkAge2 = (Distance2 - ShrinkLength) / Distance2;
			float ShrinkAge3 = (Distance3 - ShrinkLength) / Distance3;
			float ShrinkAge4 = (Distance4 - ShrinkLength) / Distance4;
			FVector2D UpdateOutPnt1 = (1 - ShrinkAge1)*SuitableCenter + ShrinkAge1 * OutPnts[0];
			FVector2D UpdateOutPnt2 = (1 - ShrinkAge2)*SuitableCenter + ShrinkAge2 * OutPnts[1];
			FVector2D UpdateOutPnt3 = (1 - ShrinkAge3)*SuitableCenter + ShrinkAge3 * OutPnts[2];
			FVector2D UpdateOutPnt4 = (1 - ShrinkAge4)*SuitableCenter + ShrinkAge4 * OutPnts[3];

			FVector LocOfLight1 = FVector(0.5*(UpdateOutPnt1 + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight2 = FVector(0.5*(UpdateOutPnt2 + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight3 = FVector(0.5*(UpdateOutPnt3 + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight4 = FVector(0.5*(UpdateOutPnt4 + SuitableCenter), WallHeight - ReduceHeight);
			LightsLoc.Add(LocOfLight1);
			LightsLoc.Add(LocOfLight2);
			LightsLoc.Add(LocOfLight3);
			LightsLoc.Add(LocOfLight4);
			return true;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			SuitableCenter = TriangleCenters[TargetIndex];
			FVector LocOfLight1 = FVector(0.5*(OutPnts[0] + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight2 = FVector(0.5*(OutPnts[1] + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight3 = FVector(0.5*(OutPnts[2] + SuitableCenter), WallHeight - ReduceHeight);
			FVector LocOfLight4 = FVector(0.5*(OutPnts[3] + SuitableCenter), WallHeight - ReduceHeight);
			LightsLoc.Add(LocOfLight1);
			LightsLoc.Add(LocOfLight2);
			LightsLoc.Add(LocOfLight3);
			LightsLoc.Add(LocOfLight4);
			return true;
		}
	}
	else
	{
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		TMap<FVector2D, TArray<FVector2D>> MaxCenterMapBounds;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				bool RegionUseness = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 3))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PStart]);
					TempPointArray.Add(OutPnts[PNext]);
					MaxCenterMapBounds.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					RegionUseness = true;
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 3))
				{
					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempPointArray.Add(OutPnts[PNext]);
					TempPointArray.Add(OutPnts[PStart]);
					MaxCenterMapBounds.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					RegionUseness = true;
				}



				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (RegionUseness))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);

				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = AreaSet[0];
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 1; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] > MaxArea)
			{
				MaxArea = AreaSet[CenterIndex];
				TargetIndex = CenterIndex;
			}
		}

		if (MaxArea >= 150 * 100)
		{
			SuitableCenter = AreaCenters[TargetIndex];
			TArray<FVector2D> BoundPs = MaxCenterMapBounds[SuitableCenter];
			for (int _CurBoundP = 0; _CurBoundP < BoundPs.Num(); ++_CurBoundP)
			{
				float TempDistance = FVector2D::Distance(BoundPs[_CurBoundP], SuitableCenter);
				float TempShrinkAge = (TempDistance - ShrinkLength) / TempDistance;
				FVector2D UpdateOutPnt = (1 - TempShrinkAge)*SuitableCenter + TempShrinkAge * BoundPs[_CurBoundP];
				FVector TempLoc = FVector(0.5*(UpdateOutPnt + SuitableCenter), WallHeight - ReduceHeight);
				LightsLoc.Add(TempLoc);
			}
		}
		else
		{
			float MinX = OutPnts[0].X;
			float MaxX = OutPnts[0].X;
			float MinY = OutPnts[0].Y;
			float MaxY = OutPnts[0].Y;
			for (int _CurP = 0; _CurP < OutPnts.Num(); ++_CurP)
			{
				if (MinX > OutPnts[_CurP].X)
				{
					MinX = OutPnts[_CurP].X;
				}
				if (MaxX < OutPnts[_CurP].X)
				{
					MaxX = OutPnts[_CurP].X;
				}
				if (MinY > OutPnts[_CurP].Y)
				{
					MinY = OutPnts[_CurP].Y;
				}
				if (MaxY < OutPnts[_CurP].Y)
				{
					MaxY = OutPnts[_CurP].Y;
				}
			}
			FVector2D PLeftUp = FVector2D(MinX, MaxY);
			FVector2D PLeftDown = FVector2D(MinX, MinY);
			FVector2D PRightDown = FVector2D(MaxX, MinY);
			FVector2D PRightUp = FVector2D(MaxX, MaxY);
			FVector2D TempCenter = 0.25*(PLeftUp + PLeftDown + PRightDown + PRightUp);
			if (FPolygonAlg::JudgePointInPolygon(OutPnts, TempCenter))
			{
				SuitableCenter = TempCenter;
				float Distance1 = FVector2D::Distance(PLeftUp, SuitableCenter);
				float Distance2 = FVector2D::Distance(PLeftDown, SuitableCenter);
				float Distance3 = FVector2D::Distance(PRightDown, SuitableCenter);
				float Distance4 = FVector2D::Distance(PRightUp, SuitableCenter);
				float ShrinkAge1 = (Distance1 - ShrinkLength) / Distance1;
				float ShrinkAge2 = (Distance2 - ShrinkLength) / Distance2;
				float ShrinkAge3 = (Distance3 - ShrinkLength) / Distance3;
				float ShrinkAge4 = (Distance4 - ShrinkLength) / Distance4;
				FVector2D UpdateOutPnt1 = (1 - ShrinkAge1)*SuitableCenter + ShrinkAge1 * PLeftUp;
				FVector2D UpdateOutPnt2 = (1 - ShrinkAge2)*SuitableCenter + ShrinkAge2 * PLeftDown;
				FVector2D UpdateOutPnt3 = (1 - ShrinkAge3)*SuitableCenter + ShrinkAge3 * PRightDown;
				FVector2D UpdateOutPnt4 = (1 - ShrinkAge4)*SuitableCenter + ShrinkAge4 * PRightUp;

				FVector LocOfLight1 = FVector(0.5*(UpdateOutPnt1 + SuitableCenter), WallHeight - ReduceHeight);
				FVector LocOfLight2 = FVector(0.5*(UpdateOutPnt2 + SuitableCenter), WallHeight - ReduceHeight);
				FVector LocOfLight3 = FVector(0.5*(UpdateOutPnt3 + SuitableCenter), WallHeight - ReduceHeight);
				FVector LocOfLight4 = FVector(0.5*(UpdateOutPnt4 + SuitableCenter), WallHeight - ReduceHeight);
				LightsLoc.Add(LocOfLight1);
				LightsLoc.Add(LocOfLight2);
				LightsLoc.Add(LocOfLight3);
				LightsLoc.Add(LocOfLight4);

			}
		}

	}
	return true;
}


bool FPolygonAlg::LocateLightOfKitchen(const TArray<FVector2D> InPnts, FVector&LightLoc, const float WallHeight, const float ReduceHeight)
{
	using namespace Triangulation;
	FVector2D SuitableCenter;
	CDTDS Mycdt;

	TArray<FVector2D>  TempPoints, OutPnts;
	bool MergeShortSuccess = MergeShrotEdgeCmSize(InPnts, TempPoints);
	if (!MergeShortSuccess)
	{
		return false;
	}

	bool MergeCollinearSuccess = MergeCollinearPoints(TempPoints, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}
	if (OutPnts.Num() <= 3)
	{
		return false;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			SuitableCenter = IBetterCenter;

		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			SuitableCenter = TriangleCenters[TargetIndex];

		}
	}
	else
	{
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		TMap<FVector2D, TArray<FVector2D>> MaxCenterMapBounds;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				bool RegionUseness = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 3))
				{
					float JudgeDis = FVector2D::Distance(OutPnts[PFront], OutPnts[PStart]);
					if (JudgeDis >= 145)
					{
						TempCenterX = 0.5*(P4.x() + P2.x());
						TempCenterY = 0.5*(P4.y() + P2.y());

						RegionUseness = true;
					}
					else
					{
						TempCenterX = 0.5*(P1.x() + P2.x());
						TempCenterY = 0.5*(P1.y() + P2.y());
						TempCenterX = TempCenterX + 100 * Vec3_x;
						TempCenterY = TempCenterY + 100 * Vec3_y;

						RegionUseness = true;
					}
					TempArea = DisPFrontToPStart * DisPStartToPNext;

				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 3))
				{
					float JudgeDis = FVector2D::Distance(OutPnts[PNext], OutPnts[PNextNext]);
					if (JudgeDis >= 145)
					{
						TempCenterX = 0.5*(P3.x() + P1.x());
						TempCenterY = 0.5*(P3.y() + P1.y());


						RegionUseness = true;
					}
					else
					{
						TempCenterX = 0.5*(P1.x() + P2.x());
						TempCenterY = 0.5*(P1.y() + P2.y());
						TempCenterX = TempCenterX + 100 * Vec2_x;
						TempCenterY = TempCenterY + 100 * Vec2_y;


						RegionUseness = true;
					}
					TempArea = DisPNextToPNextNext * DisPStartToPNext;

				}



				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (RegionUseness))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);

				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = AreaSet[0];
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 1; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] > MaxArea)
			{
				MaxArea = AreaSet[CenterIndex];
				TargetIndex = CenterIndex;
			}
		}

		if (MaxArea != 0.0)
		{
			SuitableCenter = AreaCenters[TargetIndex];
		}
		else
		{
			float MinX = OutPnts[0].X;
			float MaxX = OutPnts[0].X;
			float MinY = OutPnts[0].Y;
			float MaxY = OutPnts[0].Y;
			for (int _CurP = 0; _CurP < OutPnts.Num(); ++_CurP)
			{
				if (MinX > OutPnts[_CurP].X)
				{
					MinX = OutPnts[_CurP].X;
				}
				if (MaxX < OutPnts[_CurP].X)
				{
					MaxX = OutPnts[_CurP].X;
				}
				if (MinY > OutPnts[_CurP].Y)
				{
					MinY = OutPnts[_CurP].Y;
				}
				if (MaxY < OutPnts[_CurP].Y)
				{
					MaxY = OutPnts[_CurP].Y;
				}
			}
			FVector2D PLeftUp = FVector2D(MinX, MaxY);
			FVector2D PLeftDown = FVector2D(MinX, MinY);
			FVector2D PRightDown = FVector2D(MaxX, MinY);
			FVector2D PRightUp = FVector2D(MaxX, MaxY);
			FVector2D TempCenter = 0.25*(PLeftUp + PLeftDown + PRightDown + PRightUp);
			if (FPolygonAlg::JudgePointInPolygon(OutPnts, TempCenter))
			{
				SuitableCenter = TempCenter;
			}
		}


	}
	LightLoc = FVector(SuitableCenter, WallHeight - ReduceHeight);
	return true;
}



bool FPolygonAlg::LocateLightOfOneToilet(const TArray<FVector2D> InPnts, FVector&LightLoc, const float WallHeight, const float ReduceHeight)
{
	using namespace Triangulation;
	FVector2D SuitableCenter;
	CDTDS Mycdt;
	TArray<FVector2D>  TempPoints, OutPnts;
	bool MergeShortSuccess = MergeShrotEdgeCmSize(InPnts, TempPoints);
	if (!MergeShortSuccess)
	{
		return false;
	}

	bool MergeCollinearSuccess = MergeCollinearPoints(TempPoints, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}
	if (OutPnts.Num() <= 3)
	{
		return false;
	}
	else if (OutPnts.Num() == 4)
	{
		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			SuitableCenter = IBetterCenter;
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			SuitableCenter = TriangleCenters[TargetIndex];
		}
	}
	else
	{
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		TMap<FVector2D, TArray<FVector2D>> MaxCenterMapBounds;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				bool RegionUseness = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 3))
				{
					float JudgeDis = FVector2D::Distance(OutPnts[PFront], OutPnts[PStart]);
					if (JudgeDis >= 145)
					{
						TempCenterX = 0.5*(P4.x() + P2.x());
						TempCenterY = 0.5*(P4.y() + P2.y());

						RegionUseness = true;
					}
					else
					{
						TempCenterX = 0.5*(P1.x() + P2.x());
						TempCenterY = 0.5*(P1.y() + P2.y());
						TempCenterX = TempCenterX + 100 * Vec3_x;
						TempCenterY = TempCenterY + 100 * Vec3_y;

						RegionUseness = true;
					}
					TempArea = DisPFrontToPStart * DisPStartToPNext;

				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 3))
				{
					float JudgeDis = FVector2D::Distance(OutPnts[PNext], OutPnts[PNextNext]);
					if (JudgeDis >= 145)
					{
						TempCenterX = 0.5*(P3.x() + P1.x());
						TempCenterY = 0.5*(P3.y() + P1.y());
						RegionUseness = true;
					}
					else
					{
						TempCenterX = 0.5*(P1.x() + P2.x());
						TempCenterY = 0.5*(P1.y() + P2.y());
						TempCenterX = TempCenterX + 100 * Vec2_x;
						TempCenterY = TempCenterY + 100 * Vec2_y;

						RegionUseness = true;
					}
					TempArea = DisPNextToPNextNext * DisPStartToPNext;

				}

				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (RegionUseness))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);

				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = AreaSet[0];
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 1; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] > MaxArea)
			{
				MaxArea = AreaSet[CenterIndex];
				TargetIndex = CenterIndex;
			}
		}

		SuitableCenter = AreaCenters[TargetIndex];
	}
	LightLoc = FVector(SuitableCenter, WallHeight - ReduceHeight);
	return true;
}


bool FPolygonAlg::LocateTwoLightsOfOneToilet(const TArray<FVector2D> InPnts, TArray<FVector>&LightLocs, const float WallHeight, const float ReduceHeight)
{
	using namespace Triangulation;
	FVector2D SuitableCenter;
	CDTDS Mycdt;
	TArray<FVector2D> OutPnts;
	bool MergeCollinearSuccess = MergeCollinearPoints(InPnts, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}
	if (OutPnts.Num() <= 3)
	{
		return false;
	}
	else if (OutPnts.Num() == 4)
	{
		float LengthOfSide1 = FVector2D::Distance(OutPnts[0], OutPnts[1]);
		float LengthOfSide2 = FVector2D::Distance(OutPnts[1], OutPnts[2]);
		FVector2D BaseExtendDir;
		float BaseExtendL = LengthOfSide1;
		if (LengthOfSide1 > LengthOfSide2)
		{
			BaseExtendDir = OutPnts[0] - OutPnts[1];
		}
		else
		{
			BaseExtendDir = OutPnts[2] - OutPnts[1];
			BaseExtendL = LengthOfSide2;
		}
		BaseExtendDir.Normalize();

		float PCenterX = 0.0, PCenterY = 0.0;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			PCenterX = PCenterX + OutPnts[PIndex].X;
			PCenterY = PCenterY + OutPnts[PIndex].Y;
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}

		FVector2D IBetterCenter = FVector2D(PCenterX / OutPnts.Num(), PCenterY / OutPnts.Num());
		if (Region2D.bounded_side(Point_2(IBetterCenter.X, IBetterCenter.Y)) == CGAL::ON_BOUNDED_SIDE)
		{
			SuitableCenter = IBetterCenter;
			FVector2D TempPoint1 = SuitableCenter + 1.0 / 6.0*BaseExtendL*BaseExtendDir;
			FVector2D TempPoint2 = SuitableCenter - 1.0 / 6.0*BaseExtendL*BaseExtendDir;
			if (FPolygonAlg::JudgePointInPolygon(OutPnts, TempPoint1))
			{
				LightLocs.AddUnique(FVector(TempPoint1, WallHeight - ReduceHeight));
			}
			if (FPolygonAlg::JudgePointInPolygon(OutPnts, TempPoint2))
			{
				LightLocs.AddUnique(FVector(TempPoint2, WallHeight - ReduceHeight));
			}
		}
		else
		{
			TArray<Vertex_handle> AllHandles;
			for (int32 Index = 0; Index < OutPnts.Num(); Index++)
			{
				Vertex_handle TempV = Mycdt.insert(Point_2(OutPnts[Index].X, OutPnts[Index].Y));
				AllHandles.Add(TempV);
			}
			for (int32 JIndex = 0; JIndex < AllHandles.Num(); JIndex++)
			{
				if (JIndex == AllHandles.Num() - 1)
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[0]);
				}
				else
				{
					Mycdt.insert_constraint(AllHandles[JIndex], AllHandles[JIndex + 1]);
				}
			}
			Mesher mesher(Mycdt);
			TArray<float> AreaSet;
			TArray<FVector2D> TriangleCenters;
			for (auto Fbegin = Mycdt.finite_faces_begin(), Fend = Mycdt.finite_faces_end(); Fbegin != Fend; ++Fbegin)
			{
				if ((*Fbegin).is_in_domain())
				{
					FVector2D V0(Mycdt.triangle(Fbegin).vertex(0)[0], Mycdt.triangle(Fbegin).vertex(0)[1]);
					FVector2D V1(Mycdt.triangle(Fbegin).vertex(1)[0], Mycdt.triangle(Fbegin).vertex(1)[1]);
					FVector2D V2(Mycdt.triangle(Fbegin).vertex(2)[0], Mycdt.triangle(Fbegin).vertex(2)[1]);
					Triangle_2 Tri(Mycdt.triangle(Fbegin));
					AreaSet.Add(Tri.area());
					TriangleCenters.Add(FVector2D(centroid(Tri).x(), centroid(Tri).y()));
				}
			}
			float MaxArea = AreaSet[0];
			int32 TargetIndex = 0;
			for (int32 Index = 1; Index < AreaSet.Num(); Index++)
			{
				if (MaxArea < AreaSet[Index])
				{
					MaxArea = AreaSet[Index];
					TargetIndex = Index;
				}
			}
			SuitableCenter = TriangleCenters[TargetIndex];
			LightLocs.AddUnique(FVector(SuitableCenter, WallHeight - ReduceHeight));
		}
	}
	else
	{
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		TMap<FVector2D, TArray<FVector2D>> CenterMapLights;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				FVector2D BaseDir = OutPnts[PNext] - OutPnts[PStart];
				BaseDir.Normalize();
				FVector2D TempCenter;
				bool RegionUseness = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 3))
				{
					float JudgeDis = FVector2D::Distance(OutPnts[PFront], OutPnts[PStart]);
					if (JudgeDis >= 145)
					{
						TempCenterX = 0.5*(P4.x() + P2.x());
						TempCenterY = 0.5*(P4.y() + P2.y());

						RegionUseness = true;
					}
					else
					{
						TempCenterX = 0.5*(P1.x() + P2.x());
						TempCenterY = 0.5*(P1.y() + P2.y());
						TempCenterX = TempCenterX + 100 * Vec3_x;
						TempCenterY = TempCenterY + 100 * Vec3_y;

						RegionUseness = true;
					}
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					TempCenter = FVector2D(TempCenterX, TempCenterY);
					FVector2D TempLight1 = TempCenter + 1.0 / 6.0*DisPStartToPNext*BaseDir;
					FVector2D TempLight2 = TempCenter - 1.0 / 6.0*DisPStartToPNext*BaseDir;
					TArray<FVector2D> TempLightArr;
					TempLightArr.AddUnique(TempLight1);
					TempLightArr.AddUnique(TempLight2);
					CenterMapLights.Add(TempCenter, TempLightArr);
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 3))
				{
					float JudgeDis = FVector2D::Distance(OutPnts[PNext], OutPnts[PNextNext]);
					if (JudgeDis >= 145)
					{
						TempCenterX = 0.5*(P3.x() + P1.x());
						TempCenterY = 0.5*(P3.y() + P1.y());
						RegionUseness = true;
					}
					else
					{
						TempCenterX = 0.5*(P1.x() + P2.x());
						TempCenterY = 0.5*(P1.y() + P2.y());
						TempCenterX = TempCenterX + 100 * Vec2_x;
						TempCenterY = TempCenterY + 100 * Vec2_y;

						RegionUseness = true;
					}
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					TempCenter = FVector2D(TempCenterX, TempCenterY);
					FVector2D TempLight1 = TempCenter + 1.0 / 6.0*DisPStartToPNext*BaseDir;
					FVector2D TempLight2 = TempCenter - 1.0 / 6.0*DisPStartToPNext*BaseDir;
					TArray<FVector2D> TempLightArr;
					TempLightArr.AddUnique(TempLight1);
					TempLightArr.AddUnique(TempLight2);
					CenterMapLights.Add(TempCenter, TempLightArr);

				}

				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (RegionUseness))
				{
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = AreaSet[0];
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 1; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] > MaxArea)
			{
				MaxArea = AreaSet[CenterIndex];
				TargetIndex = CenterIndex;
			}
		}
		if (MaxArea > 0.1)
		{
			SuitableCenter = AreaCenters[TargetIndex];
			if (CenterMapLights.Contains(SuitableCenter))
			{
				TArray<FVector2D> MatchPoints = CenterMapLights[SuitableCenter];
				for (int _CurI = 0; _CurI < MatchPoints.Num(); ++_CurI)
				{
					if (FPolygonAlg::JudgePointInPolygon(OutPnts, MatchPoints[_CurI]))
					{
						LightLocs.AddUnique(FVector(MatchPoints[_CurI], WallHeight - ReduceHeight));
					}
				}
			}
			else
			{
				LightLocs.AddUnique(FVector(SuitableCenter, WallHeight - ReduceHeight));
			}
		}
	}
	return true;
}


bool FPolygonAlg::LocateLightsOfSecondToliet(const TArray<FVector2D> InPnts, const TArray<FVector2D> WallPoints, TArray<FVector>& LightsLoc, const float WallHeight, const float ReduceHeight)
{
	using namespace Triangulation;

	if (WallPoints.Num() < 4)
	{
		return false;
	}
	FVector2D Center;
	Center.X = 0.0;
	Center.Y = 0.0;
	for (int I = 0; I < WallPoints.Num(); ++I)
	{
		Center.X = Center.X + WallPoints[I].X;
		Center.Y = Center.Y + WallPoints[I].Y;
	}
	Center.X = Center.X / WallPoints.Num();
	Center.Y = Center.Y / WallPoints.Num();
	FVector2D Dir1 = WallPoints[1] - WallPoints[0];
	FVector2D Dir2 = WallPoints[2] - WallPoints[1];
	Dir1 = Dir1.GetSafeNormal();
	Dir2 = Dir2.GetSafeNormal();
	float LengthOfDir1 = FVector2D::Distance(WallPoints[0], WallPoints[1]);
	float LengthOfDir2 = FVector2D::Distance(WallPoints[1], WallPoints[2]);
	TMap<int, float> IndexMapInterSectL;
	TArray<FVector2D> FirstPointArr, SecondPointArr;
	Vector_2 BaseDir;
	if (LengthOfDir1 < LengthOfDir2)
	{
		FVector2D TempRotatedDir = Dir2.GetRotated(90);
		BaseDir = Vector_2(TempRotatedDir.X, TempRotatedDir.Y);
	}
	else
	{
		FVector2D TempRotatedDir = Dir1.GetRotated(90);
		BaseDir = Vector_2(TempRotatedDir.X, TempRotatedDir.Y);
	}
	TArray<FVector2D> UpdateRegionPs;
	MergeCollinearPoints(InPnts, UpdateRegionPs);
	TArray<Segment_2> AllSegments;
	int NextI = 0;
	for (int _CurI = 0; _CurI < UpdateRegionPs.Num(); ++_CurI)
	{
		if (_CurI == UpdateRegionPs.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = _CurI + 1;
		}
		Point_2  P1(UpdateRegionPs[_CurI].X, UpdateRegionPs[_CurI].Y);
		Point_2  P2(UpdateRegionPs[NextI].X, UpdateRegionPs[NextI].Y);
		Segment_2 TempSeg(P1, P2);
		AllSegments.Add(TempSeg);
	}

	Point_2 OriginP(Center.X, Center.Y);
	Ray_2 BaseRay_2(OriginP, BaseDir);
	bool FindIntersect = false;
	for (int32 Index = 0; Index < AllSegments.Num(); Index++)
	{
		CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay_2);
		if (Result)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
			{
				FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
				float TempLen = FVector2D::Distance(TempPoint, Center);
				IndexMapInterSectL.Add(0, TempLen);
				FirstPointArr.Add(Center);
				FirstPointArr.Add(TempPoint);
				break;
			}
		}
	}

	Ray_2 ReverBaseRay_2(OriginP, -BaseDir);
	for (int32 Index = 0; Index < AllSegments.Num(); Index++)
	{
		CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], ReverBaseRay_2);
		if (Result)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
			{
				FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
				float TempLen = FVector2D::Distance(TempPoint, Center);
				IndexMapInterSectL.Add(1, TempLen);
				SecondPointArr.Add(Center);
				SecondPointArr.Add(TempPoint);
				break;
			}
		}
	}
	if (IndexMapInterSectL.Num() == 2)
	{
		float Length1 = IndexMapInterSectL[0];
		float Length2 = IndexMapInterSectL[1];
		if (Length1 > Length2)
		{
			FVector2D TempLight1 = 1.0 / 3.0*FirstPointArr[1] + 2.0 / 3.0*FirstPointArr[0];
			FVector2D TempLight2 = 2.0 / 3.0*FirstPointArr[1] + 1.0 / 3.0*FirstPointArr[0];
			FVector2D TempLight3 = 0.5*(SecondPointArr[0] + SecondPointArr[1]);
			LightsLoc.Add(FVector(TempLight1, WallHeight - ReduceHeight));
			LightsLoc.Add(FVector(TempLight2, WallHeight - ReduceHeight));
			LightsLoc.Add(FVector(TempLight3, WallHeight - ReduceHeight));
		}
		else
		{
			FVector2D TempLight1 = 1.0 / 3.0*SecondPointArr[1] + 2.0 / 3.0*SecondPointArr[0];
			FVector2D TempLight2 = 2.0 / 3.0*SecondPointArr[1] + 1.0 / 3.0*SecondPointArr[0];
			FVector2D TempLight3 = 0.5*(FirstPointArr[0] + FirstPointArr[1]);
			LightsLoc.Add(FVector(TempLight1, WallHeight - ReduceHeight));
			LightsLoc.Add(FVector(TempLight2, WallHeight - ReduceHeight));
			LightsLoc.Add(FVector(TempLight3, WallHeight - ReduceHeight));
		}
	}
	else
	{
		if (IndexMapInterSectL.Contains(0))
		{
			FVector2D TempLight = 0.5*(FirstPointArr[0] + FirstPointArr[1]);
			LightsLoc.Add(FVector(TempLight, WallHeight - ReduceHeight));
		}
		else if (IndexMapInterSectL.Contains(1))
		{
			FVector2D TempLight = 0.5*(SecondPointArr[0] + SecondPointArr[1]);
			LightsLoc.Add(FVector(TempLight, WallHeight - ReduceHeight));
		}
	}
	return true;
}



bool FPolygonAlg::LocateLightsOfPassageAndVestibule(const TArray<FVector2D> InPutPolygon, TArray<FVector>&LocationOfLights, const float WallHeight, const float ReduceHeight)
{
	using namespace Triangulation;
	TArray<FVector> CollectLights;
	TArray<FVector2D> EndPoints, MapPoints, MaxRegion;
	TArray<FVector2D> InterSectPs;
	TArray<Segment_2> AllSegments;
	int NextI = 0;
	for (int I = 0; I < InPutPolygon.Num(); ++I)
	{
		if (I == InPutPolygon.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = I + 1;
		}
		AllSegments.Add(Segment_2(Point_2(InPutPolygon[I].X, InPutPolygon[I].Y), Point_2(InPutPolygon[NextI].X, InPutPolygon[NextI].Y)));
	}
	TArray<FVector2D> PointsOfPart1, PointsOfPart2, PointsOfPart3;
	Polygon_2 TempPolygon;
	bool IsSuccess = FPolygonAlg::GetMaxAreaEndPoints(InPutPolygon, EndPoints, MapPoints, MaxRegion);
	if (IsSuccess)
	{
		bool IsInterSected = FPolygonAlg::GetWayInterSectPs(InPutPolygon, EndPoints, MapPoints, InterSectPs);
		if (IsInterSected)
		{
			TMap<int, FVector2D> SegMiddlePointMapDirs;
			TempPolygon.push_back(Point_2(InterSectPs[0].X, InterSectPs[0].Y));
			TempPolygon.push_back(Point_2(EndPoints[0].X, EndPoints[0].Y));
			TempPolygon.push_back(Point_2(EndPoints[1].X, EndPoints[1].Y));
			TempPolygon.push_back(Point_2(InterSectPs[1].X, InterSectPs[1].Y));
			FVector2D TempMiddle1 = 0.5*(InterSectPs[0] + EndPoints[0]);
			FVector2D TempMiddle2 = 0.5*(InterSectPs[1] + EndPoints[1]);
			FVector2D Dir1 = InterSectPs[0] - EndPoints[0];
			Dir1 = Dir1.GetSafeNormal();
			FVector2D Dir2 = InterSectPs[1] - EndPoints[1];
			Dir2 = Dir2.GetSafeNormal();
			FVector2D RotateOfDir1 = Dir1.GetRotated(90);
			FVector2D RotateOfDir2 = Dir2.GetRotated(90);
			FVector2D ExtendP1 = TempMiddle1 + 10 * RotateOfDir1;
			FVector2D ExtendP2 = TempMiddle2 + 10 * RotateOfDir2;
			FVector2D ReverseOfExtendP1 = TempMiddle1 - 10 * RotateOfDir1;
			FVector2D ReverseOfExtendP2 = TempMiddle2 - 10 * RotateOfDir2;
			bool JudgeP1InRegion = TempPolygon.has_on_bounded_side(Point_2(ExtendP1.X, ExtendP1.Y));
			bool JudgeP2InRegion = TempPolygon.has_on_bounded_side(Point_2(ExtendP2.X, ExtendP2.Y));
			bool JudgeReverseOfP1InRegion = TempPolygon.has_on_bounded_side(Point_2(ReverseOfExtendP1.X, ReverseOfExtendP1.Y));
			bool JudgeReverseOfP2InRegion = TempPolygon.has_on_bounded_side(Point_2(ReverseOfExtendP2.X, ReverseOfExtendP2.Y));
			if (JudgeP1InRegion)
			{
				Point_2 OriginP(TempMiddle1.X, TempMiddle1.Y);
				Vector_2 Vec1(-RotateOfDir1.X, -RotateOfDir1.Y);
				Ray_2 BaseRay1(OriginP, Vec1);
				for (int32 Index = 0; Index < AllSegments.Num(); Index++)
				{
					CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay1);
					if (Result)
					{
						if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
						{
							FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
							PointsOfPart1.Add(TempPoint);
							PointsOfPart1.Add(TempMiddle1);
							SegMiddlePointMapDirs.Add(0, -RotateOfDir1);
							break;
						}
						else
						{
							return false;
						}
					}
				}
			}
			if (JudgeReverseOfP1InRegion)
			{
				Point_2 OriginP(TempMiddle1.X, TempMiddle1.Y);
				Vector_2 Vec1(RotateOfDir1.X, RotateOfDir1.Y);
				Ray_2 BaseRay1(OriginP, Vec1);
				for (int32 Index = 0; Index < AllSegments.Num(); Index++)
				{
					CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay1);
					if (Result)
					{
						if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
						{
							FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
							PointsOfPart1.Add(TempPoint);
							PointsOfPart1.Add(TempMiddle1);
							SegMiddlePointMapDirs.Add(0, RotateOfDir1);
							break;
						}
						else
						{
							return false;
						}
					}
				}
			}

			if (!SegMiddlePointMapDirs.Contains(0))
			{
				return false;
			}

			if (JudgeP2InRegion)
			{
				Point_2 OriginP(TempMiddle2.X, TempMiddle2.Y);
				Vector_2 Vec2(-RotateOfDir2.X, -RotateOfDir2.Y);
				Ray_2 BaseRay2(OriginP, Vec2);
				for (int32 Index = 0; Index < AllSegments.Num(); Index++)
				{
					CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay2);
					if (Result)
					{
						if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
						{
							FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
							PointsOfPart3.Add(TempMiddle2);
							PointsOfPart3.Add(TempPoint);
							SegMiddlePointMapDirs.Add(1, -RotateOfDir2);
							break;
						}
						else
						{
							return false;
						}
					}
				}
			}
			if (JudgeReverseOfP2InRegion)
			{
				Point_2 OriginP(TempMiddle2.X, TempMiddle2.Y);
				Vector_2 Vec2(RotateOfDir2.X, RotateOfDir2.Y);
				Ray_2 BaseRay2(OriginP, Vec2);
				for (int32 Index = 0; Index < AllSegments.Num(); Index++)
				{
					CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay2);
					if (Result)
					{
						if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
						{
							FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
							PointsOfPart3.Add(TempMiddle2);
							PointsOfPart3.Add(TempPoint);
							SegMiddlePointMapDirs.Add(1, RotateOfDir2);
							break;
						}
						else
						{
							return false;
						}
					}
				}
			}

			if (!SegMiddlePointMapDirs.Contains(1))
			{
				return false;
			}

			if (PointsOfPart1.Num() == 2)
			{
				float DisOfPart1 = FVector2D::Distance(PointsOfPart1[0], PointsOfPart1[1]);
				FVector2D TempExtendVec = PointsOfPart1[1] - PointsOfPart1[0];
				TempExtendVec = TempExtendVec.GetSafeNormal();
				int NumToAdd = (int)((DisOfPart1 - 130) / 120) + 1;
				if ((DisOfPart1 <= 150) || (NumToAdd <= 2))
				{
					CollectLights.AddUnique(FVector(PointsOfPart1[1], WallHeight - ReduceHeight));
					CollectLights.AddUnique(FVector(0.5*(PointsOfPart1[1]+PointsOfPart1[0]), WallHeight - ReduceHeight));
				}
				else
				{

					if (NumToAdd >= 3)
					{
						float EachSegDis = (DisOfPart1 - 130) / (NumToAdd - 1);
						for (int _CurI = 0; _CurI < NumToAdd; ++_CurI)
						{
							if (_CurI == 0)
							{
								FVector2D LightAtEnd1 = PointsOfPart1[0] + 65 * TempExtendVec;
								CollectLights.Add(FVector(LightAtEnd1, WallHeight - ReduceHeight));
							}
							else if (_CurI == NumToAdd - 1)
							{
								FVector2D LightAtEnd2 = PointsOfPart1[1] - 65 * TempExtendVec;
								CollectLights.Add(FVector(LightAtEnd2, WallHeight - ReduceHeight));
							}
							else
							{
								FVector2D TempLight = PointsOfPart1[0] + 65 * TempExtendVec + _CurI * EachSegDis * TempExtendVec;
								CollectLights.Add(FVector(TempLight, WallHeight - ReduceHeight));
							}
						}
					}
				}
			}

			if (PointsOfPart3.Num() == 2)
			{
				float DisOfPart1 = FVector2D::Distance(PointsOfPart3[0], PointsOfPart3[1]);
				FVector2D TempExtendVec = PointsOfPart3[1] - PointsOfPart3[0];
				TempExtendVec = TempExtendVec.GetSafeNormal();
				int NumToAdd = (int)((DisOfPart1 - 130) / 120) + 1;
				if ((DisOfPart1 <= 150) || (NumToAdd <= 2))
				{
					CollectLights.AddUnique(FVector(PointsOfPart3[0], WallHeight - ReduceHeight));
					CollectLights.AddUnique(FVector(0.5*(PointsOfPart3[0]+PointsOfPart3[1]), WallHeight - ReduceHeight));
				}
				else
				{
					if (NumToAdd >= 3)
					{

						float EachSegDis = (DisOfPart1 - 130) / (NumToAdd - 1);
						for (int _CurI = 0; _CurI < NumToAdd; ++_CurI)
						{
							if (_CurI == 0)
							{
								FVector2D LightAtEnd1 = PointsOfPart3[0] + 65 * TempExtendVec;
								CollectLights.Add(FVector(LightAtEnd1, WallHeight - ReduceHeight));
							}
							else if (_CurI == NumToAdd - 1)
							{
								FVector2D LightAtEnd2 = PointsOfPart3[1] - 65 * TempExtendVec;
								CollectLights.Add(FVector(LightAtEnd2, WallHeight - ReduceHeight));
							}
							else
							{
								FVector2D TempLight = PointsOfPart3[0] + 65 * TempExtendVec + _CurI * EachSegDis * TempExtendVec;
								CollectLights.Add(FVector(TempLight, WallHeight - ReduceHeight));
							}
						}
					}
				}
			}


			FVector2D VecOfMiddleLine = TempMiddle2 - TempMiddle1;
			VecOfMiddleLine = VecOfMiddleLine.GetSafeNormal();
			float JudgeParrallelValue = VecOfMiddleLine.X*RotateOfDir1.Y - VecOfMiddleLine.Y*RotateOfDir1.X;
			if (abs(JudgeParrallelValue) <= 0.10)
			{
				PointsOfPart2.Add(TempMiddle1);
				PointsOfPart2.Add(TempMiddle2);
				float DisOfTwoEnds = FVector2D::Distance(PointsOfPart2[0], PointsOfPart2[1]);
				float DisOfAfterCutting = DisOfTwoEnds - 170;
				int NumOfLights = (int)(DisOfAfterCutting / 120) + 1;
				FVector2D VecOfPart3 = PointsOfPart2[1] - PointsOfPart2[0];
				VecOfPart3 = VecOfPart3.GetSafeNormal();

				if (NumOfLights >= 3)
				{
					float EachSegDis = DisOfAfterCutting / (NumOfLights - 1);
					for (int _CurI = 0; _CurI < NumOfLights; ++_CurI)
					{
						if (_CurI == 0)
						{
							FVector2D LightAtEnd1 = PointsOfPart2[0] + 85 * VecOfPart3;
							CollectLights.Add(FVector(LightAtEnd1, WallHeight - ReduceHeight));
						}
						else if (_CurI == NumOfLights - 1)
						{
							FVector2D LightAtEnd2 = PointsOfPart2[1] - 85 * VecOfPart3;
							CollectLights.Add(FVector(LightAtEnd2, WallHeight - ReduceHeight));
						}
						else
						{
							FVector2D TempLight = PointsOfPart2[0] + 85 * VecOfPart3 + _CurI * EachSegDis * VecOfPart3;
							CollectLights.Add(FVector(TempLight, WallHeight - ReduceHeight));
						}
					}
				}
				else
				{
					FVector2D TempLight = 0.5*(PointsOfPart2[0] + PointsOfPart2[1]);
					CollectLights.Add(FVector(TempLight, WallHeight - ReduceHeight));
				}
			}
			else
			{
				TArray<FVector2D> Group1, Group2;
				Point_2 OriginP(TempMiddle1.X, TempMiddle1.Y);
				if (SegMiddlePointMapDirs.Contains(0))
				{
					FVector2D OppoSiteVec = -SegMiddlePointMapDirs[0];
					Vector_2 TempVec(OppoSiteVec.X, OppoSiteVec.Y);
					Ray_2 TempBay(OriginP, TempVec);
					for (int32 Index = 0; Index < AllSegments.Num(); Index++)
					{
						CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], TempBay);
						if (Result)
						{
							if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
							{
								FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
								Group1.Add(TempMiddle1);
								Group1.Add(TempPoint);
								break;
							}
						}
					}
				}


				Point_2 TempOri(TempMiddle2.X, TempMiddle2.Y);
				if (SegMiddlePointMapDirs.Contains(1))
				{
					FVector2D TempOppoVec = -SegMiddlePointMapDirs[1];
					Vector_2 TempVec2(TempOppoVec.X, TempOppoVec.Y);
					Ray_2 TempBay2(TempOri, TempVec2);
					for (int32 Index = 0; Index < AllSegments.Num(); Index++)
					{
						CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], TempBay2);
						if (Result)
						{
							if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
							{
								FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
								Group2.Add(TempMiddle2);
								Group2.Add(TempPoint);
								break;
							}
						}
					}
				}

				if (Group1.Num() == 2)
				{
					float TempDisOfEnds = FVector2D::Distance(Group1[0], Group1[1]);
					float TempDisAfterCutting = TempDisOfEnds - 170;
					int TempNumOfLights = (int)(TempDisAfterCutting / 120) + 1;
					FVector2D TempVec = Group1[1] - Group1[0];
					TempVec = TempVec.GetSafeNormal();

					if (TempNumOfLights >= 3)
					{

						float EachSegDis = TempDisAfterCutting / (TempNumOfLights - 1);
						for (int _CurI = 0; _CurI < TempNumOfLights; ++_CurI)
						{
							if (_CurI == 0)
							{
								FVector2D LightAtEnd1 = Group1[0] + 85 * TempVec;
								CollectLights.Add(FVector(LightAtEnd1, WallHeight - ReduceHeight));
							}
							else if (_CurI == TempNumOfLights - 1)
							{
								FVector2D LightAtEnd2 = Group1[1] - 85 * TempVec;
								CollectLights.Add(FVector(LightAtEnd2, WallHeight - ReduceHeight));
							}
							else
							{
								FVector2D TempLight = Group1[0] + 85 * TempVec + _CurI * EachSegDis * TempVec;
								CollectLights.Add(FVector(TempLight, WallHeight - ReduceHeight));
							}
						}
					}
					else
					{
						FVector2D TempLight = 0.5*(Group1[0] + Group1[1]);
						CollectLights.Add(FVector(TempLight, WallHeight - ReduceHeight));
					}
				}
				else
				{
					return false;
				}

				if (Group2.Num() == 2)
				{
					float TempDisOfEnds = FVector2D::Distance(Group2[0], Group2[1]);
					float TempDisAfterCutting = TempDisOfEnds - 170;
					int TempNumOfLights = (int)(TempDisAfterCutting / 120) + 1;
					FVector2D TempVec = Group2[1] - Group2[0];
					TempVec = TempVec.GetSafeNormal();

					if (TempNumOfLights >= 3)
					{

						for (int _CurI = 0; _CurI < TempNumOfLights; ++_CurI)
						{
							if (_CurI == 0)
							{
								FVector2D LightAtEnd1 = Group2[0] + 85 * TempVec;
								CollectLights.Add(FVector(LightAtEnd1, WallHeight - ReduceHeight));
							}
							else if (_CurI == TempNumOfLights - 1)
							{
								FVector2D LightAtEnd2 = Group2[1] - 85 * TempVec;
								CollectLights.Add(FVector(LightAtEnd2, WallHeight - ReduceHeight));
							}
							else
							{
								FVector2D TempLight = Group1[0] + 85 * TempVec + _CurI * 120 * TempVec;
								CollectLights.Add(FVector(TempLight, WallHeight - ReduceHeight));
							}
						}

					}
					else
					{
						FVector2D TempLight = 0.5*(Group2[0] + Group2[1]);
						CollectLights.Add(FVector(TempLight, WallHeight - ReduceHeight));
					}
				}
				else
				{
					return false;
				}

			}

			for (int _CurL = 0; _CurL < CollectLights.Num(); ++_CurL)
			{
				bool JudgeLightInRegion = FPolygonAlg::JudgePointInPolygon(InPutPolygon, FVector2D(CollectLights[_CurL]));
				if (JudgeLightInRegion)
				{
					LocationOfLights.AddUnique(CollectLights[_CurL]);
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}



float FPolygonAlg::GetAreaOfRegion(const TArray<FVector2D> InPnts)
{
	using namespace Triangulation;
	if (InPnts.Num() == 0)
	{
		return 0.0f;
	}
	Polygon_2 TempPolygon;
	for (int I = 0; I < InPnts.Num(); ++I)
	{
		TempPolygon.push_back(Point_2(InPnts[I].X, InPnts[I].Y));
	}
	return abs(TempPolygon.area());
}



bool FPolygonAlg::LocateLightsOfSmallRegion(const TArray<FVector2D> InPnts, TArray<FVector>&LightsLoc, const float WallHeight, const float ReduceHeight)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;
	//const float ShrinkLength = 150 * sqrt(2);
	TArray<FVector2D> TempOutPnts, OutPnts;
	bool MergeCollinearSuccess = MergeCollinearPoints(InPnts, TempOutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}

	bool MergeSuccess = MergeShortEdges(TempOutPnts, OutPnts);
	if (!MergeSuccess)
	{
		return false;
	}
	FVector2D SuitableCenter;
	if (OutPnts.Num() <= 4)
	{
		return false;
	}
	else
	{
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		TMap<FVector2D, TArray<FVector2D>> MaxCenterMapBounds;
		TMap<FVector2D, float> CenterMapRatio;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				bool FindLocalCenter = false;
				float TempRatio = 0.0;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 4.0))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;

					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PStart]);
					TempPointArray.Add(OutPnts[PNext]);
					MaxCenterMapBounds.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					FindLocalCenter = true;
					float R1 = DisPStartToPNext / DisPFrontToPStart;
					if (R1 > 1.0 / R1)
					{
						CenterMapRatio.Add(FVector2D(TempCenterX, TempCenterY), R1);
					}
					else
					{
						CenterMapRatio.Add(FVector2D(TempCenterX, TempCenterY), 1.0 / R1);
					}
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 4.0))
				{

					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;

					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempPointArray.Add(OutPnts[PNext]);
					TempPointArray.Add(OutPnts[PStart]);
					MaxCenterMapBounds.Add(FVector2D(TempCenterX, TempCenterY), TempPointArray);
					FindLocalCenter = true;
					float R2 = DisPStartToPNext / DisPNextToPNextNext;
					if (R2 > 1.0 / R2)
					{
						CenterMapRatio.Add(FVector2D(TempCenterX, TempCenterY), R2);
					}
					else
					{
						CenterMapRatio.Add(FVector2D(TempCenterX, TempCenterY), 1.0 / R2);
					}
				}



				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (FindLocalCenter))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}




		float MaxArea = 0.0;
		int32 TargetIndex = 0;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] != 0.0)
			{
				if ((AreaSet[CenterIndex] > MaxArea) && (CenterMapRatio[AreaCenters[CenterIndex]] <= 2.0))
				{
					MaxArea = AreaSet[CenterIndex];
					TargetIndex = CenterIndex;
				}
			}
		}
		if (MaxCenterMapBounds.Contains(AreaCenters[TargetIndex]))
		{
			TArray<FVector2D> MaxLocalRegion = MaxCenterMapBounds[AreaCenters[TargetIndex]];

			if (MaxLocalRegion.Num() >= 4)
			{

				for (int _CurIndex = 0; _CurIndex < AreaCenters.Num(); ++_CurIndex)
				{
					if ((AreaSet[_CurIndex] != 0.0) && (_CurIndex != TargetIndex))
					{
						if (MaxCenterMapBounds.Contains(AreaCenters[_CurIndex]))
						{
							bool IsInMaxLocalRegion = FPolygonAlg::JudgePointInPolygon(MaxLocalRegion, AreaCenters[_CurIndex]);
							if (!IsInMaxLocalRegion)
							{
								TArray<FVector2D> BoundPs = MaxCenterMapBounds[AreaCenters[_CurIndex]];
								if (BoundPs.Num() == 4)
								{
									FVector2D TempMiddle1 = 0.5*(BoundPs[0] + BoundPs[1]);
									FVector2D TempMiddle2 = 0.5*(BoundPs[2] + BoundPs[3]);
									FVector2D TempCenter = 0.5*(TempMiddle1 + TempMiddle2);
									FVector2D VecToStart = TempCenter - TempMiddle1;
									VecToStart = VecToStart.GetSafeNormal();
									float TempEndDis = FVector2D::Distance(TempMiddle1, TempMiddle2);
									float DisOfAfterCutting = TempEndDis - 150;
									int NumOfLights = (int)(DisOfAfterCutting / 75) + 1;

									if (NumOfLights >= 3)
									{
										float EachSegDis = DisOfAfterCutting / (NumOfLights - 1);
										for (int _CurI = 0; _CurI < NumOfLights; ++_CurI)
										{
											if (_CurI == 0)
											{
												FVector2D LightAtEnd1 = TempMiddle1 + 75 * VecToStart;
												LightsLoc.Add(FVector(LightAtEnd1, WallHeight - ReduceHeight));
											}
											else if (_CurI == NumOfLights - 1)
											{
												FVector2D LightAtEnd2 = TempMiddle2 - 75 * VecToStart;
												LightsLoc.Add(FVector(LightAtEnd2, WallHeight - ReduceHeight));
											}
											else
											{
												FVector2D TempLight = TempMiddle1 + 75 * VecToStart + _CurI * EachSegDis * VecToStart;
												LightsLoc.Add(FVector(TempLight, WallHeight - ReduceHeight));
											}
										}
									}
									else
									{
										FVector2D TempLight = 0.5*(TempMiddle1 + TempMiddle2);
										LightsLoc.Add(FVector(TempLight, WallHeight - ReduceHeight));
									}
								}
							}
						}

					}
				}

			}
		}

	}
	return true;

}


bool FPolygonAlg::CalculateRayInterSectPoly(const TArray<FVector2D> InPnts, const FVector2D StartPoint, const FVector2D PointVec, FVector2D& InterSectP, const float DisBound)
{
	using namespace Triangulation;
	Vector_2 TempVec(PointVec.X, PointVec.Y);
	Ray_2 TempRay(Point_2(StartPoint.X, StartPoint.Y), TempVec);
	TArray<Segment_2> AllSegs;
	bool FindInterSect = false;
	int NextI = 0;
	for (int _CurI = 0; _CurI < InPnts.Num(); ++_CurI)
	{
		if (_CurI == InPnts.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = _CurI + 1;
		}
		Segment_2 TempSeg(Point_2(InPnts[_CurI].X, InPnts[_CurI].Y), Point_2(InPnts[NextI].X, InPnts[NextI].Y));
		CGAL::cpp11::result_of<Intersect_2(Ray_2, Segment_2)>::type Result = intersection(TempRay, TempSeg);
		if (Result)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
			{
				FVector2D TempP((*TempIntersectP).x(), (*TempIntersectP).y());
				float TempDis = FVector2D::Distance(TempP, StartPoint);
				if (TempDis > DisBound)
				{
					InterSectP = TempP;
					FindInterSect = true;
					break;
				}
			}
		}
	}

	if (!FindInterSect)
	{
		return false;
	}

	return true;
}


bool FPolygonAlg::LocateCenterOfOneToilet(const TArray<FVector2D>InPnts, FVector2D& Center)
{
	bool IsSuccess = FPolygonAlg::LocateSuitableCenter(InPnts, Center);
	return IsSuccess;

}

bool FPolygonAlg::LocateCentersOfSecondToilet(const TArray<FVector2D>InPnts, const TArray<FVector2D> WallPointsOfRegion, TArray<FVector2D>& Centers)
{
	using namespace Triangulation;
	TMap<FVector2D, float> CenterMapLength;
	TArray<FVector2D> TempCenters;
	if (WallPointsOfRegion.Num() < 4)
	{
		return false;
	}
	FVector2D Center;
	Center.X = 0.0;
	Center.Y = 0.0;
	for (int I = 0; I < WallPointsOfRegion.Num(); ++I)
	{
		Center.X = Center.X + WallPointsOfRegion[I].X;
		Center.Y = Center.Y + WallPointsOfRegion[I].Y;
	}
	Center.X = Center.X / WallPointsOfRegion.Num();
	Center.Y = Center.Y / WallPointsOfRegion.Num();
	FVector2D Dir1 = WallPointsOfRegion[1] - WallPointsOfRegion[0];
	FVector2D Dir2 = WallPointsOfRegion[2] - WallPointsOfRegion[1];
	Dir1 = Dir1.GetSafeNormal();
	Dir2 = Dir2.GetSafeNormal();
	float LengthOfDir1 = FVector2D::Distance(WallPointsOfRegion[0], WallPointsOfRegion[1]);
	float LengthOfDir2 = FVector2D::Distance(WallPointsOfRegion[1], WallPointsOfRegion[2]);
	Vector_2 BaseDir;
	if (LengthOfDir1 < LengthOfDir2)
	{
		FVector2D TempBaseDir = Dir2;
		FVector2D TempRotatedDir = TempBaseDir.GetRotated(90);
		BaseDir = Vector_2(TempRotatedDir.X, TempRotatedDir.Y);
	}
	else
	{
		FVector2D TempBaseDir = Dir1;
		FVector2D TempRotatedDir = TempBaseDir.GetRotated(90);
		BaseDir = Vector_2(TempRotatedDir.X, TempRotatedDir.Y);
	}
	TArray<FVector2D> UpdateRegionPs;
	MergeCollinearPoints(InPnts, UpdateRegionPs);
	TArray<Segment_2> AllSegments;
	int NextI = 0;
	for (int _CurI = 0; _CurI < UpdateRegionPs.Num(); ++_CurI)
	{
		if (_CurI == UpdateRegionPs.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = _CurI + 1;
		}
		Point_2  P1(UpdateRegionPs[_CurI].X, UpdateRegionPs[_CurI].Y);
		Point_2  P2(UpdateRegionPs[NextI].X, UpdateRegionPs[NextI].Y);
		Segment_2 TempSeg(P1, P2);
		AllSegments.Add(TempSeg);
	}

	Point_2 OriginP(Center.X, Center.Y);
	Ray_2 BaseRay_2(OriginP, BaseDir);
	bool FindIntersect = false;
	for (int32 Index = 0; Index < AllSegments.Num(); Index++)
	{
		CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay_2);
		if (Result)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
			{
				FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
				FVector2D TempLoc = 1.0 / 3 * (TempPoint)+2.0 / 3 * Center;
				float TempLen = FVector2D::Distance(TempPoint, Center);
				CenterMapLength.Add(TempLoc, TempLen);
				TempCenters.Add(TempLoc);
				break;
			}
		}
	}

	Ray_2 ReverBaseRay_2(OriginP, -BaseDir);
	for (int32 Index = 0; Index < AllSegments.Num(); Index++)
	{
		CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], ReverBaseRay_2);
		if (Result)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
			{
				FVector2D TempPoint((*TempIntersectP).x(), (*TempIntersectP).y());
				FVector2D TempLoc = 1.0 / 3 * (TempPoint)+2.0 / 3 * Center;
				float TempLen = FVector2D::Distance(TempPoint, Center);
				CenterMapLength.Add(TempLoc, TempLen);
				TempCenters.Add(TempLoc);
				break;
			}
		}
	}
	if (CenterMapLength.Num() == 2)
	{
		if (CenterMapLength[TempCenters[0]] > CenterMapLength[TempCenters[1]])
		{
			Centers.Add(TempCenters[0]);
			Centers.Add(TempCenters[1]);
		}
		else
		{
			Centers.Add(TempCenters[1]);
			Centers.Add(TempCenters[0]);
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool FPolygonAlg::JudgePolygonsSnap(TArray<FVector2D> PolygonOne, TArray<FVector2D>PolygonTwo, TArray<FVector2D>&RegionPs, TArray<FVector2D>&ComponentPs)
{
	using namespace Triangulation;
	Polygon_2 Poly1, Poly2;
	TArray<FVector2D> UpdatePolygonOne, UpdatePolygonTwo;
	FPolygonAlg::StricMergeCollinear(PolygonOne, UpdatePolygonOne);
	FPolygonAlg::StricMergeCollinear(PolygonTwo, UpdatePolygonTwo);
	bool IfSnap = false;
	for (int _CurI = 0; _CurI < UpdatePolygonOne.Num(); ++_CurI)
	{
		Poly1.push_back(Point_2(UpdatePolygonOne[_CurI].X, UpdatePolygonOne[_CurI].Y));
	}
	for (int _CurJ = 0; _CurJ < UpdatePolygonTwo.Num(); ++_CurJ)
	{
		Poly2.push_back(Point_2(UpdatePolygonTwo[_CurJ].X, UpdatePolygonTwo[_CurJ].Y));
	}
	float Area1 = abs(Poly1.area());
	float Area2 = abs(Poly2.area());
	Polygon_2 RegionPoly, ComponentPoly;
	TArray<Segment_2>ComponentSegs, RegionSegs, UpdateRegionSegs, UpdateComSegs;
	if (Area1 > Area2)
	{
		RegionPoly = Poly1;
		ComponentPoly = Poly2;

	}
	else
	{
		RegionPoly = Poly2;
		ComponentPoly = Poly1;
		TArray<FVector2D> TempArray;
		TempArray = UpdatePolygonOne;
		UpdatePolygonTwo = UpdatePolygonOne;
		UpdatePolygonOne = TempArray;
	}
	int NextAIndex = 0;
	for (int SegAIndex = 0; SegAIndex < UpdatePolygonOne.Num(); ++SegAIndex)
	{
		if (SegAIndex == UpdatePolygonOne.Num() - 1)
		{
			NextAIndex = 0;
		}
		else
		{
			NextAIndex = SegAIndex + 1;
		}
		Point_2 PA(UpdatePolygonOne[SegAIndex].X, UpdatePolygonOne[SegAIndex].Y);
		Point_2 PB(UpdatePolygonOne[NextAIndex].X, UpdatePolygonOne[NextAIndex].Y);
		Segment_2 TempSeg(PA, PB);
		RegionSegs.Add(TempSeg);
	}
	int NextBIndex = 0;
	for (int SegBIndex = 0; SegBIndex < UpdatePolygonTwo.Num(); ++SegBIndex)
	{
		if (SegBIndex == UpdatePolygonTwo.Num() - 1)
		{
			NextBIndex = 0;
		}
		else
		{
			NextBIndex = SegBIndex + 1;
		}
		Point_2 PA(UpdatePolygonTwo[SegBIndex].X, UpdatePolygonTwo[SegBIndex].Y);
		Point_2 PB(UpdatePolygonTwo[NextBIndex].X, UpdatePolygonTwo[NextBIndex].Y);
		Segment_2 TempSeg(PA, PB);
		ComponentSegs.Add(TempSeg);
	}

	const float CloseDisTol = 0.5;
	TArray<int> RegionSegsToRemove, ComponentSegsToRemove;
	TMap<int, int> ComponentSegMapRegionSeg;
	for (int _CurSeg = 0; _CurSeg < ComponentSegs.Num(); ++_CurSeg)
	{
		for (int _CurRegionSeg = 0; _CurRegionSeg < RegionSegs.Num(); ++_CurRegionSeg)
		{
			float SegStartToRegion = sqrt(CGAL::squared_distance(ComponentSegs[_CurSeg].source(), RegionSegs[_CurRegionSeg]));
			float SegEndToRegion = sqrt(CGAL::squared_distance(ComponentSegs[_CurSeg].target(), RegionSegs[_CurRegionSeg]));
			if ((SegStartToRegion < CloseDisTol) && (SegEndToRegion < CloseDisTol))
			{
				RegionSegsToRemove.AddUnique(_CurRegionSeg);
				ComponentSegsToRemove.AddUnique(_CurSeg);
				ComponentSegMapRegionSeg.Add(_CurSeg, _CurRegionSeg);
				break;
			}
		}
	}
	TArray<Segment_2> AddRegionSegs, AddComponentSegs;
	for (int I = 0; I < ComponentSegs.Num(); ++I)
	{
		if (!ComponentSegsToRemove.Contains(I))
		{
			AddComponentSegs.Add(ComponentSegs[I]);
		}
	}
	if (ComponentSegsToRemove.Num() == 1)
	{
		Point_2 RegionInterSegStart = RegionSegs[RegionSegsToRemove[0]].source();
		Point_2 RegionInterSegEnd = RegionSegs[RegionSegsToRemove[0]].target();
		Point_2 ComponentInterSegStart = ComponentSegs[ComponentSegsToRemove[0]].source();
		Point_2 ComponentInterSegEnd = ComponentSegs[ComponentSegsToRemove[0]].target();
		float DisToP1 = CGAL::squared_distance(RegionInterSegStart, ComponentInterSegStart);
		float DisToP2 = CGAL::squared_distance(RegionInterSegStart, ComponentInterSegEnd);
		if (DisToP1 < DisToP2)
		{
			Segment_2 TempRegionSeg1(RegionInterSegStart, ComponentInterSegStart);
			Segment_2 TempRegionSeg2(RegionInterSegEnd, ComponentInterSegEnd);
			AddRegionSegs.Add(TempRegionSeg1);
			AddRegionSegs.Add(TempRegionSeg2);
			for (int I = 0; I < AddComponentSegs.Num(); ++I)
			{
				AddRegionSegs.Add(AddComponentSegs[I]);
			}
		}
		else
		{
			Segment_2 TempRegionSeg1(RegionInterSegStart, ComponentInterSegEnd);
			Segment_2 TempRegionSeg2(RegionInterSegEnd, ComponentInterSegStart);
			AddRegionSegs.Add(TempRegionSeg1);
			AddRegionSegs.Add(TempRegionSeg2);
			for (int I = 0; I < AddComponentSegs.Num(); ++I)
			{
				AddRegionSegs.Add(AddComponentSegs[I]);
			}
		}
		IfSnap = true;
	}
	else if (ComponentSegsToRemove.Num() == 2)
	{
		for (int I = 0; I < ComponentSegsToRemove.Num(); ++I)
		{
			int RegionMapSegIndex = ComponentSegMapRegionSeg[ComponentSegsToRemove[I]];
			Point_2 RegionInterSegStart = RegionSegs[RegionMapSegIndex].source();
			Point_2 RegionInterSegEnd = RegionSegs[RegionMapSegIndex].target();
			Point_2 ComponentInterSegStart = ComponentSegs[ComponentSegsToRemove[I]].source();
			Point_2 ComponentInterSegEnd = ComponentSegs[ComponentSegsToRemove[I]].target();

			float DisToP1 = CGAL::squared_distance(RegionInterSegStart, ComponentInterSegStart);
			float DisToP2 = CGAL::squared_distance(RegionInterSegStart, ComponentInterSegEnd);
			float DisToP3 = CGAL::squared_distance(RegionInterSegEnd, ComponentInterSegStart);
			float DisToP4 = CGAL::squared_distance(RegionInterSegEnd, ComponentInterSegEnd);
			if (DisToP1 < CloseDisTol)
			{
				Segment_2 TempRegionSeg1(RegionInterSegEnd, ComponentInterSegEnd);
				AddRegionSegs.Add(TempRegionSeg1);
			}
			else if (DisToP2 < CloseDisTol)
			{
				Segment_2 TempRegionSeg1(RegionInterSegEnd, ComponentInterSegStart);
				AddRegionSegs.Add(TempRegionSeg1);
			}
			else if (DisToP3 < CloseDisTol)
			{
				Segment_2 TempRegionSeg1(RegionInterSegStart, ComponentInterSegEnd);
				AddRegionSegs.Add(TempRegionSeg1);
			}
			else if (DisToP4 < CloseDisTol)
			{
				Segment_2 TempRegionSeg1(RegionInterSegStart, ComponentInterSegStart);
				AddRegionSegs.Add(TempRegionSeg1);
			}


		}

		for (int J = 0; J < AddComponentSegs.Num(); ++J)
		{
			AddRegionSegs.Add(AddComponentSegs[J]);
		}
		IfSnap = true;
	}
	else
	{
		IfSnap = false;
	}

	if (IfSnap)
	{
		if (ComponentSegsToRemove.Num() == 1)
		{
			RegionPs.Add(FVector2D(RegionSegs[ComponentSegsToRemove[0]].source().x(), RegionSegs[ComponentSegsToRemove[0]].source().y()));
			ComponentPs.Add(FVector2D(ComponentSegs[ComponentSegsToRemove[0]].source().x(), ComponentSegs[ComponentSegsToRemove[0]].source().y()));

		}
		else if (ComponentSegsToRemove.Num() == 2)
		{
			TArray<Point_2> CollectPoints;
			for (int I = 0; I < ComponentSegsToRemove.Num(); ++I)
			{
				CollectPoints.Add(ComponentSegs[ComponentSegsToRemove[I]].source());
				CollectPoints.Add(ComponentSegs[ComponentSegsToRemove[I]].target());
			}
			int TargetIndex = 0;
			for (int J = 0; J < CollectPoints.Num(); ++J)
			{
				int NumOfCount = 0;
				for (int K = 0; K < CollectPoints.Num(); ++K)
				{
					if (CollectPoints[J] == CollectPoints[K])
					{
						NumOfCount++;
						TargetIndex = J;
					}
				}
				if (NumOfCount == 1)
				{
					RegionPs.Add(FVector2D(CollectPoints[TargetIndex].x(), CollectPoints[TargetIndex].y()));
					ComponentPs.Add(FVector2D(CollectPoints[TargetIndex].x(), CollectPoints[TargetIndex].y()));
					break;
				}
			}

		}

		for (int I = 0; I < RegionSegs.Num(); ++I)
		{
			if (!RegionSegsToRemove.Contains(I))
			{
				UpdateRegionSegs.Add(RegionSegs[I]);
			}
		}

		for (int Index = 0; Index < AddRegionSegs.Num(); ++Index)
		{
			UpdateRegionSegs.Add(AddRegionSegs[Index]);
		}


		for (int J = 0; J < ComponentSegs.Num(); ++J)
		{
			if (!ComponentSegsToRemove.Contains(J))
			{
				UpdateComSegs.Add(ComponentSegs[J]);
			}
		}
		while (UpdateRegionSegs.Num() != 1)
		{
			for (int _CurI = 0; _CurI < UpdateRegionSegs.Num(); ++_CurI)
			{

				FVector2D TempPoint = RegionPs.Last();

				float TempDisToP1 = sqrt(CGAL::squared_distance(Point_2(TempPoint.X, TempPoint.Y), UpdateRegionSegs[_CurI].source()));
				float TempDisToP2 = sqrt(CGAL::squared_distance(Point_2(TempPoint.X, TempPoint.Y), UpdateRegionSegs[_CurI].target()));

				if (TempDisToP1 <= CloseDisTol)
				{

					RegionPs.Add(FVector2D(UpdateRegionSegs[_CurI].target().x(), UpdateRegionSegs[_CurI].target().y()));
					UpdateRegionSegs.RemoveAt(_CurI);
					break;
				}
				else if (TempDisToP2 <= CloseDisTol)
				{

					RegionPs.Add(FVector2D(UpdateRegionSegs[_CurI].source().x(), UpdateRegionSegs[_CurI].source().y()));
					UpdateRegionSegs.RemoveAt(_CurI);
					break;
				}

			}
		}

		while (UpdateComSegs.Num() != 0)
		{
			for (int _CurI = 0; _CurI < UpdateComSegs.Num(); ++_CurI)
			{

				FVector2D TempComPoint = ComponentPs.Last();

				float TempDisToP1 = sqrt(CGAL::squared_distance(Point_2(TempComPoint.X, TempComPoint.Y), UpdateComSegs[_CurI].source()));
				float TempDisToP2 = sqrt(CGAL::squared_distance(Point_2(TempComPoint.X, TempComPoint.Y), UpdateComSegs[_CurI].target()));

				if (TempDisToP1 < CloseDisTol)
				{

					ComponentPs.Add(FVector2D(UpdateComSegs[_CurI].target().x(), UpdateComSegs[_CurI].target().y()));
					UpdateComSegs.RemoveAt(_CurI);
					break;
				}
				else if (TempDisToP2 < CloseDisTol)
				{
					ComponentPs.Add(FVector2D(UpdateComSegs[_CurI].source().x(), UpdateComSegs[_CurI].source().y()));
					UpdateComSegs.RemoveAt(_CurI);
					break;
				}
			}
		}
	}

	return IfSnap;
}


bool FPolygonAlg::RegionSnapMoreComps(TArray<FVector2D> RegionPs, TArray<FVector2D>Comps, TArray<FVector2D>&UpdateRegion, TArray<FVector2D>&NoSnapComps)
{
	if ((RegionPs.Num() == 0) || (Comps.Num() == 0))
	{
		return false;
	}
	int NumOfComps = Comps.Num() / 4;
	for (int _CurI = 0; _CurI < NumOfComps; ++_CurI)
	{
		TArray<FVector2D> TempComp;
		TempComp.Add(Comps[4 * _CurI]);
		TempComp.Add(Comps[4 * _CurI + 1]);
		TempComp.Add(Comps[4 * _CurI + 2]);
		TempComp.Add(Comps[4 * _CurI + 3]);
		TArray<FVector2D> NewRegionPs, NewComponentPs;
		bool IsSuccess = FPolygonAlg::JudgePolygonsSnap(RegionPs, TempComp, NewRegionPs, NewComponentPs);
		if (IsSuccess)
		{
			RegionPs = NewRegionPs;
			UpdateRegion = NewRegionPs;
		}
		else
		{
			UpdateRegion = RegionPs;
			for (int _CurJ = 0; _CurJ < TempComp.Num(); ++_CurJ)
			{
				NoSnapComps.Add(TempComp[_CurJ]);
			}
		}

	}
	return true;
}

bool FPolygonAlg::GetMinDisOfPointToPolygon(const FVector2D InPnt, const TArray<FVector2D>InPolygon, float& MinDis)
{
	using namespace Triangulation;
	if (InPolygon.Num() == 0)
	{
		return false;
	}
	int NextPoint = 0;
	float TempMinDis = 0.0;
	for (int _CurP = 0; _CurP < InPolygon.Num(); ++_CurP)
	{
		if (_CurP == InPolygon.Num() - 1)
		{
			NextPoint = 0;
		}
		else
		{
			NextPoint = _CurP + 1;
		}
		float TempDis = DistanceOfPointToSeg(InPnt, InPolygon[_CurP], InPolygon[NextPoint]);
		if ((TempMinDis == 0.0) || (TempDis <= TempMinDis))
		{
			TempMinDis = TempDis;
		}
	}
	MinDis = TempMinDis;
	return true;
}

bool FPolygonAlg::FilterLights(const TArray<FVector>LightLocs, const TArray<FVector2D> InPolygon, TArray<FVector>&UpdateLights, const float DisBound)
{
	if ((LightLocs.Num() == 0) || (InPolygon.Num() == 0))
	{
		return false;
	}
	for (int _CurLight = 0; _CurLight < LightLocs.Num(); ++_CurLight)
	{
		float TempMinDis = 0.0;
		bool IsSuccess = GetMinDisOfPointToPolygon(FVector2D(LightLocs[_CurLight]), InPolygon, TempMinDis);
		if (IsSuccess)
		{
			if (TempMinDis > DisBound)
			{
				UpdateLights.AddUnique(LightLocs[_CurLight]);
			}
		}
	}
	return true;
}



void FPolygonAlg::CalculateRayToInterSectP(const TArray<FVector2D> InPolygon, const FVector2D StartPoint, FVector2D PointVec, FVector2D& InterSectP)
{
	using namespace Triangulation;
	TArray<Segment_2> AllSegments;
	int NextI = 0;
	for (int _CurI = 0; _CurI < InPolygon.Num(); ++_CurI)
	{
		if (_CurI == InPolygon.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = _CurI + 1;
		}
		Point_2  P1(InPolygon[_CurI].X, InPolygon[_CurI].Y);
		Point_2  P2(InPolygon[NextI].X, InPolygon[NextI].Y);
		Segment_2 TempSeg(P1, P2);
		AllSegments.Add(TempSeg);
	}
	Point_2 OriginP(StartPoint.X, StartPoint.Y);
	FVector2D RayVec = PointVec;
	RayVec = RayVec.GetSafeNormal();
	Vector_2 StandVec(RayVec.X, RayVec.Y);
	Ray_2 BaseRay_2(OriginP, StandVec);

	for (int32 Index = 0; Index < AllSegments.Num(); Index++)
	{
		CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay_2);
		if (Result)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
			{
				InterSectP = FVector2D((*TempIntersectP).x(), (*TempIntersectP).y());
				float TempDis = FVector2D::Distance(StartPoint, InterSectP);
				if (TempDis > 1.0)
				{
					break;
				}
			}
			else if(const Segment_2* TempSegment=boost::get<Segment_2>(&*Result))
			{
				Point_2 SegStart = TempSegment->source();
				Point_2 SegEnd = TempSegment->target();
				float TempDis1 = FVector2D::Distance(FVector2D(SegStart.x(), SegStart.y()), StartPoint);
				float TempDis2 = FVector2D::Distance(FVector2D(SegEnd.x(), SegEnd.y()), StartPoint);
				if (TempDis1 > TempDis2)
				{
					InterSectP = FVector2D(SegEnd.x(), SegEnd.y());
				}
				else
				{
					InterSectP = FVector2D(SegStart.x(), SegStart.y());
				}
			}
		}
	}
}

bool FPolygonAlg::GetRayToInterSectP(const TArray<FVector2D> InPolygon, const FVector2D StartPoint, FVector2D PointVec, FVector2D& InterSectP)
{
	using namespace Triangulation;
	TArray<Segment_2> AllSegments;
	int NextI = 0;
	for (int _CurI = 0; _CurI < InPolygon.Num(); ++_CurI)
	{
		if (_CurI == InPolygon.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = _CurI + 1;
		}
		Point_2  P1(InPolygon[_CurI].X, InPolygon[_CurI].Y);
		Point_2  P2(InPolygon[NextI].X, InPolygon[NextI].Y);
		Segment_2 TempSeg(P1, P2);
		AllSegments.Add(TempSeg);
	}
	Point_2 OriginP(StartPoint.X, StartPoint.Y);
	FVector2D RayVec = PointVec;
	RayVec = RayVec.GetSafeNormal();
	Vector_2 StandVec(RayVec.X, RayVec.Y);
	Ray_2 BaseRay_2(OriginP, StandVec);
	bool IsGetted = false;
	for (int32 Index = 0; Index < AllSegments.Num(); Index++)
	{
		CGAL::cpp11::result_of<Intersect_2(Segment_2, Ray_2)>::type Result = intersection(AllSegments[Index], BaseRay_2);
		if (Result)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
			{
				InterSectP = FVector2D((*TempIntersectP).x(), (*TempIntersectP).y());
				float TempDis = FVector2D::Distance(StartPoint, InterSectP);
				if (TempDis > 1.0)
				{
					IsGetted = true;
					break;
				}

			}
		}
	}
	return IsGetted;
}

bool FPolygonAlg::AdjustWatchAndPickPoints(const FVector2D FirstPickP, const FVector2D SecondPickP, const TArray<FVector2D> FirstPoly,
	const TArray<FVector2D> SecondPoly, FVector2D& UpdateFirstPoint, FVector2D& UpdateSecondPoint, FVector2D& UpdateWatchPoint)
{
	using namespace Triangulation;
	TArray<FVector2D> UpdateFirstPoly, UpdateSecondPoly;
	bool MergeFirstPoly = FPolygonAlg::MergeCollinearPoints(FirstPoly, UpdateFirstPoly);
	bool MergeSecondPoly = FPolygonAlg::MergeCollinearPoints(SecondPoly, UpdateSecondPoly);
	bool JudgePoly1 = false;
	bool JudgePoly2 = false;
	TArray<Segment_2> NearSegs;
	if ((MergeFirstPoly) && (MergeSecondPoly))
	{
		int NextI = 0;
		for (int _CurI = 0; _CurI < UpdateFirstPoly.Num(); ++_CurI)
		{
			if (_CurI == UpdateFirstPoly.Num() - 1)
			{
				NextI = 0;
			}
			else
			{
				NextI = _CurI + 1;
			}
			FVector2D CurrentVec = UpdateFirstPoly[NextI] - UpdateFirstPoly[_CurI];
			CurrentVec = CurrentVec.GetSafeNormal();
			FVector2D VecOfRotated = CurrentVec.GetRotated(90);
			FVector2D CurMiddleP = 0.5*(UpdateFirstPoly[_CurI] + UpdateFirstPoly[NextI]);
			FVector2D ExtendP1 = CurMiddleP + 30 * VecOfRotated;
			FVector2D ExtendP2 = CurMiddleP - 30 * VecOfRotated;
			bool P1IsInFirstPoly = JudgePointInPolygon(FirstPoly, ExtendP1);
			bool P2IsInFirstPoly = JudgePointInPolygon(FirstPoly, ExtendP2);
			if ((P1IsInFirstPoly) || (P2IsInFirstPoly))
			{
				JudgePoly1 = true;
			}
			bool P1IsInSecondPoly = JudgePointInPolygon(SecondPoly, ExtendP1);
			bool P2IsInSecondPoly = JudgePointInPolygon(SecondPoly, ExtendP2);

			if ((P1IsInSecondPoly) || (P2IsInSecondPoly))
			{
				JudgePoly2 = true;
			}

			if ((JudgePoly1) && (JudgePoly2))
			{
				Point_2 SegStart(UpdateFirstPoly[_CurI].X, UpdateFirstPoly[_CurI].Y);
				Point_2 SegEnd(UpdateFirstPoly[NextI].X, UpdateFirstPoly[NextI].Y);
				Segment_2 Seg1(SegStart, SegEnd);
				NearSegs.Add(Seg1);
				JudgePoly1 = false;
				JudgePoly2 = false;
				break;
			}
		}

		int NextJ = 0;
		for (int _CurJ = 0; _CurJ < UpdateSecondPoly.Num(); ++_CurJ)
		{
			if (_CurJ == UpdateSecondPoly.Num() - 1)
			{
				NextJ = 0;
			}
			else
			{
				NextJ = _CurJ + 1;
			}
			FVector2D ThisVec = UpdateSecondPoly[NextJ] - UpdateSecondPoly[_CurJ];
			ThisVec = ThisVec.GetSafeNormal();
			FVector2D ThisVecOfRotated = ThisVec.GetRotated(90);
			FVector2D CurM = 0.5*(UpdateSecondPoly[_CurJ] + UpdateSecondPoly[NextJ]);
			FVector2D TempP1 = CurM + 30 * ThisVecOfRotated;
			FVector2D TempP2 = CurM - 30 * ThisVecOfRotated;
			bool P1IsInFirstPoly = JudgePointInPolygon(FirstPoly, TempP1);
			bool P2IsInFirstPoly = JudgePointInPolygon(FirstPoly, TempP2);
			if ((P1IsInFirstPoly) || (P2IsInFirstPoly))
			{
				JudgePoly1 = true;
			}
			bool P1IsInSecondPoly = JudgePointInPolygon(SecondPoly, TempP1);
			bool P2IsInSecondPoly = JudgePointInPolygon(SecondPoly, TempP2);

			if ((P1IsInSecondPoly) || (P2IsInSecondPoly))
			{
				JudgePoly2 = true;
			}

			if ((JudgePoly1) && (JudgePoly2))
			{
				Point_2 SegStart(UpdateSecondPoly[_CurJ].X, UpdateSecondPoly[_CurJ].Y);
				Point_2 SegEnd(UpdateSecondPoly[NextJ].X, UpdateSecondPoly[NextJ].Y);
				Segment_2 Seg2(SegStart, SegEnd);
				NearSegs.Add(Seg2);
				break;
			}
		}
	}
	if (NearSegs.Num() == 2)
	{
		float PEndX = NearSegs[0].target().x();
		float PEndY = NearSegs[0].target().y();
		float PStartX = NearSegs[0].source().x();
		float PStartY = NearSegs[0].source().y();
		FVector2D CommonSegVec = FVector2D(PEndX - PStartX, PEndY - PStartY);
		CommonSegVec = CommonSegVec.GetSafeNormal();
		FVector2D WatchPointVec = SecondPickP - FirstPickP;
		WatchPointVec = WatchPointVec.GetSafeNormal();
		float VecDotValue = abs(FVector2D::DotProduct(CommonSegVec, WatchPointVec));
		if (VecDotValue >= 0.1)
		{
			float LengthOfSeg1 = sqrt(CGAL::squared_distance(NearSegs[0].source(), NearSegs[0].target()));
			float LengthOfSeg2 = sqrt(CGAL::squared_distance(NearSegs[1].source(), NearSegs[1].target()));
			FVector2D VerticalVec = CommonSegVec.GetRotated(90);

			if (LengthOfSeg1 < LengthOfSeg2)
			{
				FVector2D VerAlongLine = FirstPickP + 100 * VerticalVec;
				FVector CloestP = FMath::ClosestPointOnInfiniteLine(FVector(FirstPickP, 0), FVector(VerAlongLine, 0), FVector(SecondPickP, 0));
				bool JudgePointInSecondPoly = FPolygonAlg::JudgePointInPolygon(SecondPoly, FVector2D(CloestP));
				if (JudgePointInSecondPoly)
				{
					UpdateSecondPoint = FVector2D(CloestP);
					UpdateFirstPoint = FirstPickP;
				}
				else
				{
					UpdateSecondPoint = SecondPickP;
					UpdateFirstPoint = FirstPickP;
				}
			}
			else if (LengthOfSeg1 > LengthOfSeg2)
			{
				FVector2D VerAlongLine = SecondPickP + 100 * VerticalVec;
				FVector CloestP = FMath::ClosestPointOnInfiniteLine(FVector(SecondPickP, 0), FVector(VerAlongLine, 0), FVector(FirstPickP, 0));
				bool JudgePointInFirstPoly = FPolygonAlg::JudgePointInPolygon(FirstPoly, FVector2D(CloestP));
				if (JudgePointInFirstPoly)
				{
					UpdateFirstPoint = FVector2D(CloestP);
					UpdateSecondPoint = SecondPickP;
				}
				else
				{
					UpdateSecondPoint = SecondPickP;
					UpdateFirstPoint = FirstPickP;
				}
			}
			else
			{
				FVector2D MiddleP1 = FVector2D(0.5*(NearSegs[0].source().x() + NearSegs[0].target().x()), 0.5*(NearSegs[0].source().y() + NearSegs[0].target().y()));
				FVector2D MiddleP2 = FVector2D(0.5*(NearSegs[1].source().x() + NearSegs[1].target().x()), 0.5*(NearSegs[1].source().y() + NearSegs[1].target().y()));
				FVector2D MiddleP1ToSeg1 = FirstPickP - MiddleP1;
				FVector2D MiddleP2ToSeg2 = SecondPickP - MiddleP2;
				MiddleP1ToSeg1 = MiddleP1ToSeg1.GetSafeNormal();
				MiddleP2ToSeg2 = MiddleP2ToSeg2.GetSafeNormal();
				float DotValue1 = abs(FVector2D::DotProduct(CommonSegVec, MiddleP1ToSeg1));
				float DotValue2 = abs(FVector2D::DotProduct(CommonSegVec, MiddleP2ToSeg2));
				if (DotValue1 >= 0.1)
				{
					FVector2D VerAlongLine = SecondPickP + 100 * VerticalVec;
					FVector CloestP = FMath::ClosestPointOnInfiniteLine(FVector(SecondPickP, 0), FVector(VerAlongLine, 0), FVector(FirstPickP, 0));
					bool JudgePointInFirstPoly = FPolygonAlg::JudgePointInPolygon(FirstPoly, FVector2D(CloestP));
					if (JudgePointInFirstPoly)
					{
						UpdateFirstPoint = FVector2D(CloestP);
						UpdateSecondPoint = SecondPickP;
					}
					else
					{
						UpdateSecondPoint = SecondPickP;
						UpdateFirstPoint = FirstPickP;
					}
				}
				else
				{
					UpdateSecondPoint = SecondPickP;
					UpdateFirstPoint = FirstPickP;
				}

				if (DotValue2 >= 0.1)
				{
					FVector2D VerAlongLine = FirstPickP + 100 * VerticalVec;
					FVector CloestP = FMath::ClosestPointOnInfiniteLine(FVector(FirstPickP, 0), FVector(VerAlongLine, 0), FVector(SecondPickP, 0));
					bool JudgePointInSecondPoly = FPolygonAlg::JudgePointInPolygon(SecondPoly, FVector2D(CloestP));
					if (JudgePointInSecondPoly)
					{
						UpdateSecondPoint = FVector2D(CloestP);
						UpdateFirstPoint = FirstPickP;
					}
					else
					{
						UpdateSecondPoint = SecondPickP;
						UpdateFirstPoint = FirstPickP;
					}
				}
				else
				{
					UpdateSecondPoint = SecondPickP;
					UpdateFirstPoint = FirstPickP;
				}
			}
		}
		else
		{
			UpdateSecondPoint = SecondPickP;
			UpdateFirstPoint = FirstPickP;
		}
		UpdateWatchPoint = 0.5*(UpdateFirstPoint + UpdateSecondPoint);
	}
	else
	{
		return false;
	}
	return true;
}


bool FPolygonAlg::JudgeModelBoxUseness(const FVector FMin, const FVector FMax, const TArray<FVector2D> RegionPs)
{
	FVector2D P1(FMin.X, FMin.Y);
	FVector2D P2(FMax.X, FMin.Y);
	FVector2D P3(FMax.X, FMax.Y);
	FVector2D P4(FMin.X, FMax.Y);
	FVector2D TempCenter = 0.25*(P1 + P2 + P3 + P4);
	const float Shrinkage = 0.7;
	FVector2D UpdateP1 = Shrinkage * P1 + (1-Shrinkage)*TempCenter;
	FVector2D UpdateP2 = Shrinkage * P2 + (1-Shrinkage)*TempCenter;
	FVector2D UpdateP3 = Shrinkage * P3 + (1-Shrinkage)*TempCenter;
	FVector2D UpdateP4 = Shrinkage * P4 + (1-Shrinkage)*TempCenter;

	bool P1IsInRegion = FPolygonAlg::JudgePointInPolygon(RegionPs, UpdateP1);
	bool P2IsInRegion = FPolygonAlg::JudgePointInPolygon(RegionPs, UpdateP2);
	bool P3IsInRegion = FPolygonAlg::JudgePointInPolygon(RegionPs, UpdateP3);
	bool P4IsInRegion = FPolygonAlg::JudgePointInPolygon(RegionPs, UpdateP4);
	if ((P1IsInRegion) && (P2IsInRegion) && (P3IsInRegion) && (P4IsInRegion))
	{
		return true;
	}
	return false;
}

bool FPolygonAlg::DownLightsLocation(const TArray<FVector2D> RegionPs, const float LightEqualSpace, const int NumOfLights, TArray<FVector2D>&LightsLocation)
{
	//////区域数据预处理////////////////
	using namespace Triangulation;
	TArray<FVector2D> PostRegionPs;
	TArray<FVector2D> Part1Lights, Part2Lights;

	bool MergeSuccess = FPolygonAlg::StricMergeCollinear(RegionPs, PostRegionPs);
	if (MergeSuccess)
	{
		TArray<FVector2D> TempLightsLoc;
		if (PostRegionPs.Num() <= 3)
		{
			return false;
		}
		else if (PostRegionPs.Num() == 4)
		{
			float TempDis1 = FVector2D::Distance(PostRegionPs[0], PostRegionPs[1]);
			float TempDis2 = FVector2D::Distance(PostRegionPs[1], PostRegionPs[2]);
			FVector2D RegionCenter = 0.25*(PostRegionPs[0] + PostRegionPs[1] + PostRegionPs[2] + PostRegionPs[3]);
			FVector2D DirOfLongEdge;
			if (TempDis1 > TempDis2)
			{
				FVector2D TempDir1 = PostRegionPs[1] - PostRegionPs[0];
				DirOfLongEdge = TempDir1.GetSafeNormal();
			}
			else
			{
				FVector2D TempDir2 = PostRegionPs[2] - PostRegionPs[1];
				DirOfLongEdge = TempDir2.GetSafeNormal();
			}
			if (NumOfLights == 1)
			{
				TempLightsLoc.Add(RegionCenter);
			}
			else if (NumOfLights >= 2)
			{
				int NumOfSide1 = NumOfLights / 2;
				int NumOfSide2 = NumOfLights - NumOfSide1;


				////////计算Side1的灯的位置/////////////////////
				FVector2D FirstLightOfSide1 = RegionCenter + 0.5*LightEqualSpace*DirOfLongEdge;
				Part1Lights.AddUnique(FirstLightOfSide1);
				for (int _CurI = 0; _CurI < NumOfSide1 - 1; ++_CurI)
				{
					FVector2D TempLightOfSide1 = FirstLightOfSide1 + (_CurI + 1)*LightEqualSpace * DirOfLongEdge;
					Part1Lights.AddUnique(TempLightOfSide1);
				}
				/////////计算Side2的灯的位置//////////////////
				FVector2D FirstLightOfSide2 = RegionCenter - 0.5*LightEqualSpace*DirOfLongEdge;
				Part2Lights.AddUnique(FirstLightOfSide2);
				for (int _CurJ = 0; _CurJ < NumOfSide2 - 1; ++_CurJ)
				{
					FVector2D TempLightOfSide2 = FirstLightOfSide2 - (_CurJ + 1)*LightEqualSpace*DirOfLongEdge;
					Part2Lights.AddUnique(TempLightOfSide2);
				}
			}
			else if (NumOfLights == 0)
			{
				FVector2D TempInterSectP1, TempInterSectP2;
				CalculateRayToInterSectP(PostRegionPs, RegionCenter, DirOfLongEdge, TempInterSectP1);
				CalculateRayToInterSectP(PostRegionPs, RegionCenter, -DirOfLongEdge, TempInterSectP2);
				float TempDis1 = FVector2D::Distance(RegionCenter, TempInterSectP1);
				float TempDis2 = FVector2D::Distance(RegionCenter, TempInterSectP2);
				if (TempDis1 > 0.5*LightEqualSpace)
				{
					////////计算Side1的灯的位置/////////////////////
					FVector2D FirstLightOfSide1 = RegionCenter + 0.5*LightEqualSpace*DirOfLongEdge;
					Part1Lights.AddUnique(FirstLightOfSide1);
					float DisR = TempDis1 - 0.5*LightEqualSpace;
					int NumOfR = DisR / LightEqualSpace;
					if (NumOfR >= 1)
					{
						for (int _CurL = 0; _CurL < NumOfR; ++_CurL)
						{
							FVector2D TempAddNode = FirstLightOfSide1 + (_CurL + 1)*LightEqualSpace*DirOfLongEdge;
							Part1Lights.AddUnique(TempAddNode);
						}
					}
				}

				if (TempDis2 > 0.5*LightEqualSpace)
				{
					////////计算Side2的灯的位置/////////////////////
					FVector2D FirstLightOfSide2 = RegionCenter - 0.5*LightEqualSpace*DirOfLongEdge;
					Part2Lights.AddUnique(FirstLightOfSide2);
					float DisL = TempDis2 - 0.5*LightEqualSpace;
					int NumOfL = DisL / LightEqualSpace;
					if (NumOfL >= 1)
					{
						for (int _CurL = 0; _CurL < NumOfL; ++_CurL)
						{
							FVector2D TempAddNode = FirstLightOfSide2 - (_CurL + 1)*LightEqualSpace*DirOfLongEdge;
							Part2Lights.AddUnique(TempAddNode);
						}
					}
				}
				////////////////////////////////////////////////////////////////////
				if ((TempDis1 <= 0.5*LightEqualSpace) && (TempDis2 <= 0.5*LightEqualSpace))
				{
					TempLightsLoc.AddUnique(RegionCenter);
				}
			}
		}
		else
		{
			TMap<int, TArray<FVector2D>> IndexMapEndPoints;
			int CountIndex = 0;
			int TargetIndex = 0;
			Polygon_2 Region2D;
			for (int32 PIndex = 0; PIndex < PostRegionPs.Num(); PIndex++)
			{
				Region2D.push_back(Point_2(PostRegionPs[PIndex].X, PostRegionPs[PIndex].Y));
			}
			int32 PNext = 0, PFront = 0, PNextNext = 0;
			TArray<float> AreaSet;
			TArray<FVector2D> AreaCenters;
			for (int32 PStart = 0; PStart < PostRegionPs.Num(); PStart++)
			{
				if (PStart == 0)
				{
					PNext = PStart + 1;
					PFront = PostRegionPs.Num() - 1;
					PNextNext = PNext + 1;
				}
				else if (PStart == PostRegionPs.Num() - 1)
				{
					PNext = 0;
					PNextNext = PNext + 1;
					PFront = PStart - 1;
				}
				else if (PStart == PostRegionPs.Num() - 2)
				{
					PNext = PStart + 1;
					PNextNext = 0;
					PFront = PStart - 1;
				}
				else if (PStart == 1)
				{
					PNext = PStart + 1;
					PNextNext = PNext + 1;
					PFront = 0;
				}
				else
				{
					PNext = PStart + 1;
					PNextNext = PNext + 1;
					PFront = PStart - 1;
				}
				FVector2D Vec1 = FVector2D(PostRegionPs[PNext].X - PostRegionPs[PStart].X, PostRegionPs[PNext].Y - PostRegionPs[PStart].Y);
				FVector2D Vec2 = FVector2D(PostRegionPs[PNextNext].X - PostRegionPs[PNext].X, PostRegionPs[PNextNext].Y - PostRegionPs[PNext].Y);
				FVector2D Vec3 = FVector2D(PostRegionPs[PFront].X - PostRegionPs[PStart].X, PostRegionPs[PFront].Y - PostRegionPs[PStart].Y);
				float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
				float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

				float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
				float Vec1_x = Vec1.X / Vec1L;
				float Vec1_y = Vec1.Y / Vec1L;

				float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
				float Vec2_x = Vec2.X / Vec2L;
				float Vec2_y = Vec2.Y / Vec2L;

				float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
				float Vec3_x = Vec3.X / Vec3L;
				float Vec3_y = Vec3.Y / Vec3L;
				if (OutCrossZ1*OutCrossZ2 > 0)
				{
					FVector2D PointDirectVec;
					FVector2D TempMiddle = 0.5*(PostRegionPs[PStart] + PostRegionPs[PNext]);
					FVector2D TempDir = PostRegionPs[PNext] - PostRegionPs[PStart];
					TempDir.Normalize();
					FVector2D TempRotatedDir = TempDir.GetRotated(90);
					FVector2D TempTestPoint = TempMiddle + 5 * TempRotatedDir;
					if (FPolygonAlg::JudgePointInPolygon(PostRegionPs, TempTestPoint))
					{
						PointDirectVec = TempRotatedDir;
					}
					else
					{
						PointDirectVec = -TempRotatedDir;
					}
					FVector2D TempInterSectP;
					bool IsFound = FPolygonAlg::GetRayToInterSectP(PostRegionPs, TempMiddle, PointDirectVec, TempInterSectP);
					if (IsFound)
					{
						TArray<FVector2D> TempEndArr;
						TempEndArr.Add(TempMiddle);
						TempEndArr.Add(TempInterSectP);
						IndexMapEndPoints.Add(CountIndex, TempEndArr);
						CountIndex++;
					}
				}
			}
			float MaxLength = 0.0f;
			for (int I = 0; I < IndexMapEndPoints.Num(); ++I)
			{
				if (IndexMapEndPoints.Contains(I))
				{
					TArray<FVector2D> MapEndPoints = IndexMapEndPoints[I];
					float EndLength = FVector2D::Distance(MapEndPoints[0], MapEndPoints[1]);
					if ((MaxLength <= 0.0001) || (MaxLength < EndLength))
					{
						MaxLength = EndLength;
						TargetIndex = I;
					}
				}
			}
			/////////////////////依据输入筒灯个数计算位置/////////////////////////////
			TArray<FVector2D> FinalEndPoints = IndexMapEndPoints[TargetIndex];
			FVector2D SegCenter = 0.5*(FinalEndPoints[0] + FinalEndPoints[1]);
			FVector2D TempDirOfSeg = FinalEndPoints[1] - FinalEndPoints[0];
			TempDirOfSeg.Normalize();
			if (NumOfLights == 1)
			{
				TempLightsLoc.Add(SegCenter);
			}
			else if (NumOfLights >= 2)
			{
				int NumOfSide1 = NumOfLights / 2;
				int NumOfSide2 = NumOfLights - NumOfSide1;
				////////计算Side1的灯的位置///////////////////////
				FVector2D FirstLightOfSide1 = SegCenter + 0.5*LightEqualSpace*TempDirOfSeg;
				Part1Lights.AddUnique(FirstLightOfSide1);
				for (int _CurI = 0; _CurI < NumOfSide1 - 1; ++_CurI)
				{
					FVector2D TempLightOfSide1 = FirstLightOfSide1 + (_CurI + 1)*LightEqualSpace * TempDirOfSeg;
					Part1Lights.AddUnique(TempLightOfSide1);
				}
				/////////计算Side2的灯的位置//////////////////////
				FVector2D FirstLightOfSide2 = SegCenter - 0.5*LightEqualSpace*TempDirOfSeg;
				Part2Lights.AddUnique(FirstLightOfSide2);
				for (int _CurJ = 0; _CurJ < NumOfSide2 - 1; ++_CurJ)
				{
					FVector2D TempLightOfSide2 = FirstLightOfSide2 - (_CurJ + 1)*LightEqualSpace*TempDirOfSeg;
					Part2Lights.AddUnique(TempLightOfSide2);
				}
			}
			else if (NumOfLights == 0)
			{
				float TempDis = 0.5*FVector2D::Distance(FinalEndPoints[0], FinalEndPoints[1]);

				if (TempDis > 0.5*LightEqualSpace)
				{
					////////计算Side1的灯的位置/////////////////////
					FVector2D FirstLightOfSide1 = SegCenter + 0.5*LightEqualSpace*TempDirOfSeg;
					Part1Lights.AddUnique(FirstLightOfSide1);
					float DisR = TempDis - 0.5*LightEqualSpace;
					int NumOfR = DisR / LightEqualSpace;
					if (NumOfR >= 1)
					{
						for (int _CurL = 0; _CurL < NumOfR; ++_CurL)
						{
							FVector2D TempAddNode = FirstLightOfSide1 + (_CurL + 1)*LightEqualSpace*TempDirOfSeg;
							Part1Lights.AddUnique(TempAddNode);
						}
					}


					////////计算Side2的灯的位置/////////////////////
					FVector2D FirstLightOfSide2 = SegCenter - 0.5*LightEqualSpace*TempDirOfSeg;
					Part2Lights.AddUnique(FirstLightOfSide2);
					float DisL = TempDis - 0.5*LightEqualSpace;
					int NumOfL = DisL / LightEqualSpace;
					if (NumOfL >= 1)
					{
						for (int _CurL = 0; _CurL < NumOfL; ++_CurL)
						{
							FVector2D TempAddNode = FirstLightOfSide2 - (_CurL + 1)*LightEqualSpace*TempDirOfSeg;
							Part2Lights.AddUnique(TempAddNode);
						}
					}
				}

				if (TempDis <= 0.5*LightEqualSpace)
				{
					TempLightsLoc.AddUnique(SegCenter);
				}
			}
		}

		////////////////////////////重新整体排序/////////////////////////
		for (int _CurFirst = Part1Lights.Num() - 1; _CurFirst >= 0; --_CurFirst)
		{
			TempLightsLoc.AddUnique(Part1Lights[_CurFirst]);
		}
		for (int _CurSecond = 0; _CurSecond < Part2Lights.Num(); ++_CurSecond)
		{
			TempLightsLoc.AddUnique(Part2Lights[_CurSecond]);
		}
		/////////////过滤不在区域内部的点位//////////////////////////////////
		for (int _CurLight = 0; _CurLight < TempLightsLoc.Num(); ++_CurLight)
		{
			bool IsInRegion = JudgePointInPolygon(PostRegionPs, TempLightsLoc[_CurLight]);
			if (IsInRegion)
			{
				LightsLocation.AddUnique(TempLightsLoc[_CurLight]);
			}
		}
	}
	return true;
}

float FPolygonAlg::GetShortestdistanceBySegments(const FVector2D Seg1Start, const FVector2D Seg1End, const FVector2D Seg2Start, const FVector2D Seg2End)
{
	using namespace Triangulation;
	Point_2 Pa(Seg1Start.X, Seg1Start.Y);
	Point_2 Pb(Seg1End.X, Seg1End.Y);
	Segment_2 Segment1 = Segment_2(Pa, Pb);
	Pa = Point_2(Seg2Start.X, Seg2Start.Y);
	Pb = Point_2(Seg2End.X, Seg2End.Y);
	Segment_2 Segment2 = Segment_2(Pa, Pb);
	float distance = CGAL::squared_distance(Segment1, Segment2);
	return distance;
}

bool FPolygonAlg::IntersectionByRayAndSegment(const FVector2D StartRay, const FVector2D Dir, const FVector2D SegStart, const FVector2D SegEnd, FVector2D& OutPos)
{
	using namespace Triangulation;
	Vector_2 TempVec(Dir.X, Dir.Y);
	Ray_2 TempRay(Point_2(StartRay.X, StartRay.Y), TempVec);
	Segment_2 TempSeg(Point_2(SegStart.X, SegStart.Y), Point_2(SegEnd.X, SegEnd.Y));
	CGAL::cpp11::result_of<Intersect_2(Ray_2, Segment_2)>::type Result = intersection(TempRay, TempSeg);
	if (Result)
	{
		if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
		{
			OutPos= FVector2D((*TempIntersectP).x(), (*TempIntersectP).y());
			return true;
		}
	}
	return false;
}

bool FPolygonAlg::LocateMaxLocalRegionOfLivingOrDiningRoom(const TArray<FVector2D> RoomPs,
	const int RoomID,
	const TArray<FVector2D> OppoSiteRoomPs,
	TArray<FVector2D>& MaxLocalRegion)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;

	if ((OppoSiteRoomPs.Num() == 0) || (RoomPs.Num() == 0))
	{
		return false;
	}

	TArray<FVector2D>  OutPnts;
	bool MergeCollinearSuccess = MergeCollinearPoints(RoomPs, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}

	FVector2D SuitableCenter;

	if (OutPnts.Num() <= 3)
	{
		return false;
	}
	else if (OutPnts.Num() == 4)
	{
		int NextP = 0;
		int TargetStartIndex = 0;
		bool FindTarget = false;
		for (int _CurP = 0; _CurP < OutPnts.Num(); ++_CurP)
		{
			if (_CurP == OutPnts.Num() - 1)
			{
				NextP = 0;
			}
			else
			{
				NextP = _CurP + 1;
			}
			FVector2D TempSegMiddle = 0.5*(OutPnts[_CurP] + OutPnts[NextP]);
			FVector2D TempDir = OutPnts[NextP] - OutPnts[_CurP];
			TempDir.Normalize();
			FVector2D TempRotatedDir = TempDir.GetRotated(90);
			FVector2D TempExtendP1 = TempSegMiddle + 30 * TempRotatedDir;
			FVector2D TempExtendP2 = TempSegMiddle - 30 * TempRotatedDir;
			if (FPolygonAlg::JudgePointInPolygon(OppoSiteRoomPs, TempExtendP1))
			{
				TargetStartIndex = _CurP;
				FindTarget = true;
				break;
			}
			if (FPolygonAlg::JudgePointInPolygon(OppoSiteRoomPs, TempExtendP2))
			{
				TargetStartIndex = _CurP;
				FindTarget = true;
				break;
			}
		}
		if (FindTarget)
		{
			if (TargetStartIndex == 0)
			{
				MaxLocalRegion.Add(OutPnts[0]);
				MaxLocalRegion.Add(OutPnts[1]);
				MaxLocalRegion.Add(OutPnts[2]);
				MaxLocalRegion.Add(OutPnts[3]);
			}
			else if (TargetStartIndex == 1)
			{
				MaxLocalRegion.Add(OutPnts[1]);
				MaxLocalRegion.Add(OutPnts[2]);
				MaxLocalRegion.Add(OutPnts[3]);
				MaxLocalRegion.Add(OutPnts[0]);
			}
			else if (TargetStartIndex == 2)
			{
				MaxLocalRegion.Add(OutPnts[2]);
				MaxLocalRegion.Add(OutPnts[3]);
				MaxLocalRegion.Add(OutPnts[0]);
				MaxLocalRegion.Add(OutPnts[1]);
			}
			else if (TargetStartIndex == 3)
			{
				MaxLocalRegion.Add(OutPnts[3]);
				MaxLocalRegion.Add(OutPnts[0]);
				MaxLocalRegion.Add(OutPnts[1]);
				MaxLocalRegion.Add(OutPnts[2]);
			}
		}
	}
	else
	{
		TMap<FVector2D, TArray<FVector2D>> CenterMapRegion;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				float VerticalDis1 = DisPFrontToPStart * sqrt(1 - Product1 * Product1);
				float VerticalDis2 = DisPNextToPNextNext * sqrt(1 - Product2 * Product2);

				float  TempRatio = 0.0;
				bool FindLocalCenter = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 4.0) && (DisPStartToPNext >= 150))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					FVector2D TempCenter(TempCenterX, TempCenterY);
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PStart]);
					TempPointArray.Add(OutPnts[PNext]);
					CenterMapRegion.Add(TempCenter, TempPointArray);
					FindLocalCenter = true;
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 4.0) && (DisPStartToPNext >= 150))
				{

					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					FVector2D TempCenter(TempCenterX, TempCenterY);
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempPointArray.Add(OutPnts[PNext]);
					TempPointArray.Add(OutPnts[PStart]);
					CenterMapRegion.Add(TempCenter, TempPointArray);
					FindLocalCenter = true;
				}



				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (FindLocalCenter))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = 0.0;
		int32 TargetIndex = 0;
		FVector2D MaxRegionCenter;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] != 0.0)
			{
				if (AreaSet[CenterIndex] > MaxArea)
				{
					MaxArea = AreaSet[CenterIndex];
					TargetIndex = CenterIndex;
				}
			}

		}

		if (MaxArea > 1.0)
		{
			MaxRegionCenter = AreaCenters[TargetIndex];
			MaxLocalRegion = CenterMapRegion[MaxRegionCenter];
		}
	}
	if (MaxLocalRegion.Num() != 4)
	{
		return false;
	}
	return true;
}



bool FPolygonAlg::LocateMaxLocalRegionOfRoom(const TArray<FVector2D> RoomPs, TArray<FVector2D>&MaxLocalRegion)
{
	using namespace Triangulation;
	FVector2D CenterPoint;
	CDTDS Mycdt;

	if (RoomPs.Num() == 0)
	{
		return false;
	}

	TArray<FVector2D>  OutPnts;
	bool MergeCollinearSuccess = MergeCollinearPoints(RoomPs, OutPnts);
	if (!MergeCollinearSuccess)
	{
		return false;
	}

	FVector2D SuitableCenter;
	if (OutPnts.Num() <= 3)
	{
		return false;
	}
	else if (OutPnts.Num() == 4)
	{
		MaxLocalRegion = RoomPs;
	}
	else
	{
		TMap<FVector2D, TArray<FVector2D>> CenterMapRegion;
		Polygon_2 Region2D;
		for (int32 PIndex = 0; PIndex < OutPnts.Num(); PIndex++)
		{
			Region2D.push_back(Point_2(OutPnts[PIndex].X, OutPnts[PIndex].Y));
		}
		int32 PNext = 0, PFront = 0, PNextNext = 0;
		TArray<float> AreaSet;
		TArray<FVector2D> AreaCenters;
		for (int32 PStart = 0; PStart < OutPnts.Num(); PStart++)
		{
			if (PStart == 0)
			{
				PNext = PStart + 1;
				PFront = OutPnts.Num() - 1;
				PNextNext = PNext + 1;
			}
			else if (PStart == OutPnts.Num() - 1)
			{
				PNext = 0;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			else if (PStart == OutPnts.Num() - 2)
			{
				PNext = PStart + 1;
				PNextNext = 0;
				PFront = PStart - 1;
			}
			else if (PStart == 1)
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = 0;
			}
			else
			{
				PNext = PStart + 1;
				PNextNext = PNext + 1;
				PFront = PStart - 1;
			}
			FVector2D Vec1 = FVector2D(OutPnts[PNext].X - OutPnts[PStart].X, OutPnts[PNext].Y - OutPnts[PStart].Y);
			FVector2D Vec2 = FVector2D(OutPnts[PNextNext].X - OutPnts[PNext].X, OutPnts[PNextNext].Y - OutPnts[PNext].Y);
			FVector2D Vec3 = FVector2D(OutPnts[PFront].X - OutPnts[PStart].X, OutPnts[PFront].Y - OutPnts[PStart].Y);
			float OutCrossZ1 = Vec1.X*Vec2.Y - Vec1.Y*Vec2.X;
			float OutCrossZ2 = Vec1.X*Vec3.Y - Vec1.Y*Vec3.X;

			float Vec1L = sqrt(Vec1.X*Vec1.X + Vec1.Y*Vec1.Y);
			float Vec1_x = Vec1.X / Vec1L;
			float Vec1_y = Vec1.Y / Vec1L;

			float Vec2L = sqrt(Vec2.X*Vec2.X + Vec2.Y*Vec2.Y);
			float Vec2_x = Vec2.X / Vec2L;
			float Vec2_y = Vec2.Y / Vec2L;

			float Vec3L = sqrt(Vec3.X*Vec3.X + Vec3.Y*Vec3.Y);
			float Vec3_x = Vec3.X / Vec3L;
			float Vec3_y = Vec3.Y / Vec3L;
			if (OutCrossZ1*OutCrossZ2 > 0)
			{

				Point_2 P1(OutPnts[PStart].X, OutPnts[PStart].Y);
				Point_2 P2(OutPnts[PNext].X, OutPnts[PNext].Y);
				Point_2 P3(OutPnts[PNextNext].X, OutPnts[PNextNext].Y);
				Point_2 P4(OutPnts[PFront].X, OutPnts[PFront].Y);
				float TempCenterX = 0.0, TempCenterY = 0.0, TempArea = 0.0;
				float DisPFrontToPStart = sqrt(CGAL::squared_distance(P1, P4));
				float DisPNextToPNextNext = sqrt(CGAL::squared_distance(P2, P3));
				float DisPStartToPNext = sqrt(CGAL::squared_distance(P1, P2));

				float Product1 = Vec3_x * Vec1_x + Vec3_y * Vec1_y;
				float Product2 = Vec2_x * Vec1_x + Vec2_y * Vec1_y;

				float VerticalDis1 = DisPFrontToPStart * sqrt(1 - Product1 * Product1);
				float VerticalDis2 = DisPNextToPNextNext * sqrt(1 - Product2 * Product2);

				float  TempRatio = 0.0;
				bool FindLocalCenter = false;
				if ((DisPFrontToPStart < DisPNextToPNextNext) && (DisPStartToPNext / DisPFrontToPStart <= 4.0) && (DisPStartToPNext >= 150))
				{
					TempCenterX = 0.5*(P4.x() + P2.x());
					TempCenterY = 0.5*(P4.y() + P2.y());
					TempArea = DisPFrontToPStart * DisPStartToPNext;
					FVector2D TempCenter(TempCenterX, TempCenterY);
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PNext].X + Vec2_x * DisPFrontToPStart, OutPnts[PNext].Y + Vec2_y * DisPFrontToPStart);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PFront]);
					TempPointArray.Add(OutPnts[PStart]);
					TempPointArray.Add(OutPnts[PNext]);
					CenterMapRegion.Add(TempCenter, TempPointArray);
					FindLocalCenter = true;
				}
				else if ((DisPFrontToPStart >= DisPNextToPNextNext) && (DisPStartToPNext / DisPNextToPNextNext <= 4.0) && (DisPStartToPNext >= 150))
				{

					TempCenterX = 0.5*(P3.x() + P1.x());
					TempCenterY = 0.5*(P3.y() + P1.y());
					TempArea = DisPNextToPNextNext * DisPStartToPNext;
					FVector2D TempCenter(TempCenterX, TempCenterY);
					TArray<FVector2D> TempPointArray;
					FVector2D ExtendPoint = FVector2D(OutPnts[PStart].X + Vec3_x * DisPNextToPNextNext, OutPnts[PStart].Y + Vec3_y * DisPNextToPNextNext);
					TempPointArray.Add(ExtendPoint);
					TempPointArray.Add(OutPnts[PNextNext]);
					TempPointArray.Add(OutPnts[PNext]);
					TempPointArray.Add(OutPnts[PStart]);
					CenterMapRegion.Add(TempCenter, TempPointArray);
					FindLocalCenter = true;
				}



				if ((Region2D.bounded_side(Point_2(TempCenterX, TempCenterY)) == CGAL::ON_BOUNDED_SIDE) && (FindLocalCenter))
				{
					FVector2D TempCenter = FVector2D(TempCenterX, TempCenterY);
					AreaCenters.Add(TempCenter);
					AreaSet.Add(TempArea);
				}
				else
				{
					AreaCenters.Add(FVector2D(0.0, 0.0));
					AreaSet.Add(0.0);
				}
			}
			else
			{
				AreaCenters.Add(FVector2D(0.0, 0.0));
				AreaSet.Add(0.0);
			}
		}

		float MaxArea = 0.0;
		int32 TargetIndex = 0;
		FVector2D MaxRegionCenter;
		for (int32 CenterIndex = 0; CenterIndex < AreaCenters.Num(); CenterIndex++)
		{
			if (AreaSet[CenterIndex] != 0.0)
			{
				if (AreaSet[CenterIndex] > MaxArea)
				{
					MaxArea = AreaSet[CenterIndex];
					TargetIndex = CenterIndex;
				}
			}

		}

		if (MaxArea > 1.0)
		{
			MaxRegionCenter = AreaCenters[TargetIndex];
			MaxLocalRegion = CenterMapRegion[MaxRegionCenter];
		}
	}
	if (MaxLocalRegion.Num() != 4)
	{
		return false;
	}
	return true;
}


bool FPolygonAlg::CalculateRayInterSectWithPoly(const FVector2D StartPos, const FVector2D RayVec, const TArray<FVector2D> Poly, TArray<FVector2D>& InterSectPs)
{
	using namespace Triangulation;
	Vector_2 TempVec(RayVec.X, RayVec.Y);
	Ray_2 TempRay(Point_2(StartPos.X, StartPos.Y), TempVec);
	TArray<Segment_2> AllSegs;
	const float DisBound = 0.1f;
	int NextI = 0;
	for (int _CurI = 0; _CurI < Poly.Num(); ++_CurI)
	{
		if (_CurI == Poly.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = _CurI + 1;
		}
		Segment_2 TempSeg(Point_2(Poly[_CurI].X, Poly[_CurI].Y), Point_2(Poly[NextI].X, Poly[NextI].Y));
		CGAL::cpp11::result_of<Intersect_2(Ray_2, Segment_2)>::type Result = intersection(TempRay, TempSeg);
		if (Result)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result))
			{
				FVector2D TempP((*TempIntersectP).x(), (*TempIntersectP).y());
				float TempDis = FVector2D::Distance(TempP, StartPos);
				if (TempDis > DisBound)
				{
					InterSectPs.Add(TempP);
				}
			}
		}
	}

	if (InterSectPs.Num() == 0)
	{
		return false;
	}

	return true;
}


int FPolygonAlg::SplitRegionAndLocateMaxModel(const TArray<FVector2D> RegionPs, const FVector MaxModelCenter, TArray<FVector2D>& LeftSidePs, TArray<FVector2D>& RightSidePs)
{
	////////////检测数据//////////////////
	if (RegionPs.Num() != 4)
	{
		return 0;
	}
	///////////分组数据//////////////////////////
	FVector2D TempPoint1 = 0.5*(RegionPs[0] + RegionPs[1]);
	FVector2D TempPoint2 = 0.5*(RegionPs[2] + RegionPs[3]);
	LeftSidePs.Add(TempPoint1);
	LeftSidePs.Add(RegionPs[1]);
	LeftSidePs.Add(RegionPs[2]);
	LeftSidePs.Add(TempPoint2);
	RightSidePs.Add(RegionPs[0]);
	RightSidePs.Add(TempPoint1);
	RightSidePs.Add(TempPoint2);
	RightSidePs.Add(RegionPs[3]);
	if (FPolygonAlg::JudgePointInPolygon(LeftSidePs, FVector2D(MaxModelCenter)))
	{
		return 1;
	}

	if (FPolygonAlg::JudgePointInPolygon(RightSidePs, FVector2D(MaxModelCenter)))
	{
		return 2;
	}
	return 0;
}


bool FPolygonAlg::StricMergeCollinear(const FPoint2DArray InPnts, FPoint2DArray& OutPnts)
{
	TArray<FVector2D> PostPoints;
	for (int _CurP = 0; _CurP < InPnts.Num(); ++_CurP)
	{
		PostPoints.AddUnique(InPnts[_CurP]);
	}
	if (PostPoints.Num() <= 2)
	{
		return false;
	}
	if (PostPoints.Num() > 4)
	{
		int32  NextIndex = 0, BeforeIndex = 0;
		for (int32 Index = 0; Index < PostPoints.Num(); Index++)
		{
			if (Index == PostPoints.Num() - 1)
			{
				NextIndex = 0;
				BeforeIndex = Index - 1;
			}
			else if (Index == 0)
			{
				NextIndex = Index + 1;
				BeforeIndex = PostPoints.Num() - 1;
			}
			else
			{
				NextIndex = Index + 1;
				BeforeIndex = Index - 1;
			}
			FVector2D CurrentVec = FVector2D(PostPoints[NextIndex].X - PostPoints[Index].X, PostPoints[NextIndex].Y - PostPoints[Index].Y);
			FVector2D NextVec = FVector2D(PostPoints[Index].X - PostPoints[BeforeIndex].X, PostPoints[Index].Y - PostPoints[BeforeIndex].Y);
			CurrentVec = CurrentVec.GetSafeNormal();
			NextVec = NextVec.GetSafeNormal();
			float VecInnerProduct = CurrentVec.X*NextVec.Y - CurrentVec.Y*NextVec.X;
			if (abs(VecInnerProduct) <= 1.0E-4)
			{
				continue;
			}
			else
			{
				OutPnts.Add(PostPoints[Index]);
			}
		}

		return true;
	}
	else
	{
		OutPnts = PostPoints;
	}
	return true;
}


bool FPolygonAlg::SplitPolygonByRay(const TArray<FVector2D> Polygon, const FVector2D SegStart, const FVector2D SegVec, TArray<FVector2D>& Part1Polygon,TArray<FVector2D>& Part2Polygon)
{
	using namespace Triangulation;
	const float DisBound = 0.1f;
	const float PointToL = 0.0001f;
	if (Polygon.Num() == 0)
	{
		return false;
	}
	if (!FPolygonAlg::JudgePointInPolygon(Polygon,SegStart))
	{
		return false;
	}
	TArray<FVector2D> UpdatePolygon,InterSectPs;
	bool FilterSuccess = FPolygonAlg::StricMergeCollinear(Polygon, UpdatePolygon);
	if (!FilterSuccess)
	{
		return false;
	}
	FVector2D RayVec = SegVec;
	RayVec.Normalize();

	Ray_2 SegRay1(Point_2(SegStart.X, SegStart.Y),Vector_2(RayVec.X,RayVec.Y));
	Ray_2 SegRay2(Point_2(SegStart.X, SegStart.Y),Vector_2(-RayVec.X,-RayVec.Y));
	TMap<int, FVector2D> IndexMapInterSectP;
	TArray<Segment_2> AllSegs;
	TArray<int> InterSectEdgeIndexs;
	int NextI = 0;
	for (int _CurI = 0; _CurI < UpdatePolygon.Num(); ++_CurI)
	{
		if (_CurI == UpdatePolygon.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = _CurI + 1;
		}
		Segment_2 TempSeg(Point_2(UpdatePolygon[_CurI].X,UpdatePolygon[_CurI].Y), Point_2(UpdatePolygon[NextI].X,UpdatePolygon[NextI].Y));
		AllSegs.Add(TempSeg);

		CGAL::cpp11::result_of<Intersect_2(Ray_2, Segment_2)>::type Result1 = intersection(SegRay1, TempSeg);
		if (Result1)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result1))
			{
				FVector2D TempP((*TempIntersectP).x(), (*TempIntersectP).y());
				float TempDis = FVector2D::Distance(TempP, SegStart);
				if (TempDis > DisBound)
				{
					InterSectPs.Add(TempP);
					IndexMapInterSectP.Add(_CurI, TempP);
					InterSectEdgeIndexs.Add(_CurI);
					break;
				}
			}
		}
	}

	NextI = 0;
	for (int _CurI = 0; _CurI < UpdatePolygon.Num(); ++_CurI)
	{
		if (_CurI == UpdatePolygon.Num() - 1)
		{
			NextI = 0;
		}
		else
		{
			NextI = _CurI + 1;
		}
		Segment_2 TempSeg(Point_2(UpdatePolygon[_CurI].X, UpdatePolygon[_CurI].Y), Point_2(UpdatePolygon[NextI].X, UpdatePolygon[NextI].Y));
		CGAL::cpp11::result_of<Intersect_2(Ray_2, Segment_2)>::type Result2 = intersection(SegRay2, TempSeg);
		if (Result2)
		{
			if (const Point_2* TempIntersectP = boost::get<Point_2>(&*Result2))
			{
				FVector2D TempP((*TempIntersectP).x(), (*TempIntersectP).y());
				float TempDis = FVector2D::Distance(TempP, SegStart);
				if (TempDis > DisBound)
				{
					InterSectPs.Add(TempP);
					IndexMapInterSectP.Add(_CurI, TempP);
					InterSectEdgeIndexs.Add(_CurI);
					break;
				}
			}
		}
	}
	if (InterSectPs.Num() != 2)
	{
		return false;
	}
	Segment_2 InterSeg(Point_2(InterSectPs[0].X, InterSectPs[0].Y), Point_2(InterSectPs[1].X, InterSectPs[1].Y));
	AllSegs.Add(InterSeg);
	int EdgeIndex1 = InterSectEdgeIndexs[0];
	if (EdgeIndex1 == UpdatePolygon.Num() - 1)
	{
		Segment_2 TempAddSeg1(Point_2(IndexMapInterSectP[EdgeIndex1].X, IndexMapInterSectP[EdgeIndex1].Y),Point_2(UpdatePolygon.Last().X,UpdatePolygon.Last().Y));
		Segment_2 TempAddSeg2(Point_2(IndexMapInterSectP[EdgeIndex1].X, IndexMapInterSectP[EdgeIndex1].Y), Point_2(UpdatePolygon[0].X, UpdatePolygon[0].Y));
		AllSegs.Add(TempAddSeg1);
		AllSegs.Add(TempAddSeg2);
	}
	else
	{
		Segment_2 TempAddSeg1(Point_2(IndexMapInterSectP[EdgeIndex1].X, IndexMapInterSectP[EdgeIndex1].Y), Point_2(UpdatePolygon[EdgeIndex1].X, UpdatePolygon[EdgeIndex1].Y));
		Segment_2 TempAddSeg2(Point_2(IndexMapInterSectP[EdgeIndex1].X, IndexMapInterSectP[EdgeIndex1].Y), Point_2(UpdatePolygon[EdgeIndex1+1].X, UpdatePolygon[EdgeIndex1+1].Y));
		AllSegs.Add(TempAddSeg1);
		AllSegs.Add(TempAddSeg2);
	}

	TArray<Segment_2> Part1Segs,Part2Segs;
	FVector2D TempRotatedVec = RayVec.GetRotated(90);

	for (int _CurSeg = 1; _CurSeg < AllSegs.Num(); ++_CurSeg)
	{
		FVector2D TempSegStart(AllSegs[_CurSeg].source().x(), AllSegs[_CurSeg].source().y());
		FVector2D TemSegEnd(AllSegs[_CurSeg].end().x(), AllSegs[_CurSeg].end().y());
		FVector2D TempSegMiddle=0.5*(TempSegStart + TemSegEnd);
		FVector2D TempDir = TempSegMiddle - SegStart;
		TempDir.Normalize();
		float TempDotValue = FVector2D::DotProduct(TempDir, TempRotatedVec);
		if (TempDotValue > 0.f)
		{
			Part1Segs.Add(AllSegs[_CurSeg]);
		}
		else
		{
			Part2Segs.Add(AllSegs[_CurSeg]);
		}
	}
	Part1Polygon.Add(FVector2D(InterSeg.source().x(), InterSeg.source().y()));
	Part1Polygon.Add(FVector2D(InterSeg.target().x(), InterSeg.target().y()));

	int NumCount1 = 0;
	while (NumCount1!=Part1Segs.Num())
	{
		for (int _CurSeg = 0; _CurSeg < Part1Segs.Num(); ++_CurSeg)
		{
			float TempDis1ToPoint = sqrt(CGAL::squared_distance(Part1Segs[_CurSeg].source(), Point_2(Part1Polygon.Last().X, Part1Polygon.Last().Y)));
			if (TempDis1ToPoint <= PointToL)
			{
				Part1Polygon.Add(FVector2D(Part1Segs[_CurSeg].target().x(), Part1Segs[_CurSeg].target().y()));
				NumCount1++;
				break;
			}
			else
			{
				float TempDis2ToPoint = sqrt(CGAL::squared_distance(Part1Segs[_CurSeg].target(), Point_2(Part1Polygon.Last().X, Part1Polygon.Last().Y)));
				if (TempDis2ToPoint <= PointToL)
				{
					Part1Polygon.Add(FVector2D(Part1Segs[_CurSeg].source().x(), Part1Segs[_CurSeg].source().y()));
					NumCount1++;
					break;
				}
			}
		}
	}

	Part2Polygon.Add(FVector2D(InterSeg.source().x(), InterSeg.source().y()));
	Part2Polygon.Add(FVector2D(InterSeg.target().x(), InterSeg.target().y()));

	int NumCount2 = 0;
	while (NumCount2 != Part2Segs.Num())
	{
		for (int _CurSeg = 0; _CurSeg < Part2Segs.Num(); ++_CurSeg)
		{
			float TempDis1ToPoint = sqrt(CGAL::squared_distance(Part2Segs[_CurSeg].source(), Point_2(Part2Polygon.Last().X, Part2Polygon.Last().Y)));
			if (TempDis1ToPoint <= PointToL)
			{
				Part1Polygon.Add(FVector2D(Part2Segs[_CurSeg].target().x(), Part2Segs[_CurSeg].target().y()));
				NumCount2++;
				break;
			}
			else
			{
				float TempDis2ToPoint = sqrt(CGAL::squared_distance(Part2Segs[_CurSeg].target(), Point_2(Part2Polygon.Last().X, Part2Polygon.Last().Y)));
				if (TempDis2ToPoint <= PointToL)
				{
					Part1Polygon.Add(FVector2D(Part2Segs[_CurSeg].source().x(), Part2Segs[_CurSeg].source().y()));
					NumCount2++;
					break;
				}
			}
		}
	}

	return true;
}