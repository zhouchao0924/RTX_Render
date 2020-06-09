// DESCRIPTION: data conversion between ue4 and cgal

#pragma once

#include "CgTypes.h"
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arr_non_caching_segment_basic_traits_2.h>
#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Arr_observer.h>
#include <CGAL/Polygon_set_2.h>

// predicate to construct kernel
template<class FCgalP2CKernel>
class FCgDataConvUtil
{
public:
	typedef typename FCgalP2CKernel::Vector_2 FP2CVector2D;
	typedef typename FCgalP2CKernel::Vector_3 FP2CVector3D;
	typedef typename FCgalP2CKernel::Point_2 FP2CPoint2D;
	typedef typename FCgalP2CKernel::Point_3 FP2CPoint3D;
	typedef typename FCgalP2CKernel::Line_2 FP2CLine2D;
	typedef typename FCgalP2CKernel::Ray_2 FP2CRay2D;
	typedef typename FCgalP2CKernel::Segment_2 FP2CSegment2D;
	typedef CGAL::Polygon_2<typename FCgalP2CKernel> FP2CPolygon2D;
	typedef CGAL::Polygon_with_holes_2<typename FCgalP2CKernel> FP2CPolygonWithHoles2D;
	typedef CGAL::Polygon_set_2<typename FCgalP2CKernel> FP2CPolygonSet2D;
	typedef std::list<typename FP2CPolygonWithHoles2D> FP2CPwhList2D;
	typedef std::vector<typename FP2CPoint2D> FCgPnt2DArray;

	// arrangement
	typedef typename CGAL::Arr_segment_traits_2<FCgalP2CKernel> FCgArrTraits;
	typedef typename FCgArrTraits::Point_2 FArrPoint2D;
	typedef typename FCgArrTraits::X_monotone_curve_2 FCgSegment;
	template<typename VertexData_, typename HalfedgeData_, typename FaceData_>
	struct FArrDeclUtil
	{
		using FCgArrDCEL = CGAL::Arr_extended_dcel<FCgArrTraits, VertexData_, HalfedgeData_, FaceData_>;
		using FCgArrangement = CGAL::Arrangement_2<FCgArrTraits, typename FCgArrDCEL>;
		using FCgArrObserver = CGAL::Arr_observer<FCgArrangement>;
	};
	static FArrPoint2D Vector2D2ArrPointConv(const FVector2D& InVector2D)
	{
		return FArrPoint2D(InVector2D.X, InVector2D.Y);
	}
	static FVector2D ArrPoint2D2VectorConv(const FArrPoint2D& InPnt2D)
	{
		return FVector2D(CGAL::to_double(InPnt2D.x()), CGAL::to_double(InPnt2D.y()));
	}
	
	static FP2CPoint2D Vector2D2CgPointConv(const FVector2D& InVector2D)
	{
		return FP2CPoint2D(InVector2D.X, InVector2D.Y);
	}
	static FVector2D CgPoint2D2VectorConv(const FP2CPoint2D& InPnt2D)
	{
		return FVector2D(CGAL::to_double(InPnt2D.x()), CGAL::to_double(InPnt2D.y()));
	}

	static FP2CVector2D Vector2D2CgVectorConv(const FVector2D& InVector2D)
	{
		return FP2CVector2D(InVector2D.X, InVector2D.Y);
	}

	static void Vector2DArray2CgPntsConv(FCgPnt2DArray& OutPnts, const FPoint2DArray& InPnts)
	{
		for (auto &InPnt : InPnts)
		{
			OutPnts.push_back(Vector2D2CgPointConv(InPnt));
		}
	}
};
using FExactDataConv = FCgDataConvUtil<CGAL::Exact_predicates_inexact_constructions_kernel>;