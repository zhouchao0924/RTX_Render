// Copyright© 2017 ihomefnt All Rights Reserved.

#include "WallLinkage.h"
#include "../DataNodes/NodeTypes.h"
#include "HomeLayout/HouseArchitect/WallBuildSystem.h"
#include "EditorGameInstance.h"
#include "../HouseArchitect/AreaSystem.h"
#include "WallArrangement.h"
#include "CGALWrapper/CgDataConvUtility.h"
#include "CGALWrapper/LinearEntityAlg.h"
#include <sstream>
#include <CGAL/Boolean_set_operations_2.h>


class FAutoBuildArea
{
	class LStream : public std::stringbuf
	{
	protected:
		int sync()
		{
			UE_LOG(LogTemp, Log, TEXT("%s"), *FString(str().c_str()));
			str("");
			return std::stringbuf::sync();
		}
	};

	using FExactPWithExactC = FCgDataConvUtil<CGAL::Exact_predicates_exact_constructions_kernel>;
	using FExactArrDecl = FExactPWithExactC::FArrDeclUtil<ACornerActor*, ALineWallActor*, ARoomActor*>;
	using FExactArrangement = FExactArrDecl::FCgArrangement;
	using FExactSegment = FExactPWithExactC::FCgSegment;
	using FExactPoint2D = FExactPWithExactC::FArrPoint2D;

	struct FCorner2Pnt
	{
	public:
		FCorner2Pnt(const FCornerNode& InCornerNode)
		{
			CornerNode = InCornerNode;
			CornerPnt = FExactPWithExactC::Vector2D2ArrPointConv(CornerNode.Position);
		}

		FCornerNode CornerNode;
		FExactPoint2D CornerPnt;
	};
	TArray<FCorner2Pnt> CornerPntsInScene;
	UWallLinkageComponent *WallLinkage;
	AAreaSystem *AreaSystem;
	UWallBuildSystem *WallSystem;

	class FExactArrObserver : public FExactArrDecl::FCgArrObserver
	{
	public:
		virtual void before_create_edge(const X_monotone_curve_2&  c,
			Vertex_handle  v1,
			Vertex_handle  v2)
		{
			int i = 0;
		}

		virtual void after_create_edge(Halfedge_handle  e)
		{
			int i = 0;
		}
		virtual void before_create_vertex(const Point_2& /* p */)
		{
			int i = 0;
		}

		virtual void after_create_vertex(Vertex_handle /* v */)
		{
			int i = 0;
		}
	};

public:
	template<class Arrangement>
	void print_ccb(typename Arrangement::Ccb_halfedge_const_circulator circ)
	{
		typename Arrangement::Ccb_halfedge_const_circulator  curr = circ;
		typename Arrangement::Halfedge_const_handle          he;

		std::cout << "(" << curr->source()->point() << ")";
		do
		{
			he = curr;
			std::cout << "   [" << he->curve() << "]   "
				<< "(" << he->target()->point() << ")";

			++curr;
		} while (curr != circ);
		std::cout << std::endl;

		return;
	}

	template<class Arrangement>
	void print_face(typename Arrangement::Face_const_handle f)
	{
		// Print the outer boundary.
		if (f->is_unbounded())
		{
			std::cout << "Unbounded face. " << std::endl;
		}
		else
		{
			std::cout << "Outer boundary: ";
			print_ccb<Arrangement>(f->outer_ccb());
		}

		// Print the boundary of each of the holes.
		typename Arrangement::Hole_const_iterator  hole;
		int                                         index = 1;

		for (hole = f->holes_begin(); hole != f->holes_end(); ++hole, ++index)
		{
			std::cout << "    Hole #" << index << ": ";
			print_ccb<Arrangement>(*hole);
		}

		// Print the isolated vertices.
		typename Arrangement::Isolated_vertex_const_iterator  iv;

		for (iv = f->isolated_vertices_begin(), index = 1;
			iv != f->isolated_vertices_end(); ++iv, ++index)
		{
			std::cout << "    Isolated vertex #" << index << ": "
				<< "(" << iv->point() << ")" << std::endl;
		}

		return;
	}

	template<class Arrangement>
	void print_arrangement(const Arrangement& arr)
	{
		CGAL_precondition(arr.is_valid());

		LStream Stream;
		std::cout.rdbuf(&Stream);

		// Print the arrangement vertices.
		typename Arrangement::Vertex_const_iterator  vit;

		std::cout << arr.number_of_vertices() << " vertices:" << std::endl;
		for (vit = arr.vertices_begin(); vit != arr.vertices_end(); ++vit)
		{
			std::cout << "(" << vit->point() << ")";
			if (vit->is_isolated())
				std::cout << " - Isolated." << std::endl;
			else
				std::cout << " - degree " << vit->degree() << std::endl;
		}

		// Print the arrangement edges.
		typename Arrangement::Edge_const_iterator    eit;

		std::cout << arr.number_of_edges() << " edges:" << std::endl;
		for (eit = arr.edges_begin(); eit != arr.edges_end(); ++eit)
			std::cout << "[" << eit->curve() << "]" << std::endl;

		// Print the arrangement faces.
		typename Arrangement::Face_const_iterator    fit;

		std::cout << arr.number_of_faces() << " faces:" << std::endl;
		for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit)
			print_face<Arrangement>(fit);

		return;
	}


	void Init(UWallLinkageComponent *InWallLinkage)
	{
		WallLinkage = InWallLinkage;
		WallSystem = WallLinkage->WallSystem;
		UCEditorGameInstance *GameInst = Cast<UCEditorGameInstance>(WallLinkage->GetWorld()->GetGameInstance());
		AreaSystem = GameInst->AreaSystem;
	}

	bool AutoBuildAreaByWalls(bool bReserveCurArea)
	{
		return false;
	}

	void AutoBuildAreaByCornerPnts(bool bReserveCurArea)
	{
	}

	void RemoveSamePnt(TArray<FVector2D> &InOutPolygon)
	{
		TArray<FVector2D> OutPolygon;
		for (FVector2D &Pnt : InOutPolygon)
		{
			bool bAddPnt = true;
			if (OutPolygon.Num() != 0)
			{
				FVector2D LastPnt = OutPolygon[OutPolygon.Num() - 1];
				if (LastPnt.Equals(Pnt, 0.01f))
				{
					bAddPnt = false;
				}
			}
			
			if (bAddPnt)
			{
				OutPolygon.Add(Pnt);
			}
		}
		InOutPolygon = OutPolygon;
	}

	bool DoPolygonExist(const TArray<FVector2D>& SrcPolygonPnts, const TArray<TArray<FVector2D>> &ExistPolygonsPnts)
	{
		using FExactPolygon = FExactPWithExactC::FP2CPolygon2D;
		FExactPolygon SrcPolygon;
		for (const FVector2D& Pnt : SrcPolygonPnts)
		{
			SrcPolygon.push_back(FExactPWithExactC::Vector2D2CgPointConv(Pnt));
		}
		
		double SrcArea = CGAL::to_double(SrcPolygon.area());

		FExactPWithExactC::FP2CPolygonWithHoles2D CurPolygonOutline(SrcPolygon);
		FExactPWithExactC::FP2CPwhList2D FinalPolygons;
		FinalPolygons.push_back(CurPolygonOutline);
		for (const TArray<FVector2D> &ExistPnts : ExistPolygonsPnts)
		{
			FExactPolygon ExistPolygon;
			for (const FVector2D& Pnt : ExistPnts)
			{
				ExistPolygon.push_back(FExactPWithExactC::Vector2D2CgPointConv(Pnt));
			}
			FExactPWithExactC::FP2CPwhList2D DiffPolygons;
			for (auto FinalPolygon : FinalPolygons)
			{
				CGAL::difference(FinalPolygon, ExistPolygon, std::back_inserter(DiffPolygons));
			}
			FinalPolygons = DiffPolygons;
		}
		double FinalArea = 0.0f;
		for (FExactPWithExactC::FP2CPolygonWithHoles2D& FinalPolygon : FinalPolygons)
		{
			FinalArea += CGAL::to_double(FinalPolygon.outer_boundary().area());
		}
		FinalArea *= 2.0f;
		return (FinalArea < SrcArea);
	}

	ALineWallActor* FindWallByPoint(const FExactPoint2D& FirstPnt, const FExactPoint2D& SecondPnt, bool &bSameDir)
	{
		return nullptr;
	}

	FExactPoint2D GetPointByCorner(const FString &CornerID)
	{
		FCorner2Pnt *Corner2PntFound =
			CornerPntsInScene.FindByPredicate([CornerID](const FCorner2Pnt& Corner2Pnt)
		{
			return Corner2Pnt.CornerNode.Name == CornerID;
		});

		FExactPoint2D PointOfCorner;
		if (Corner2PntFound)
		{
			PointOfCorner = Corner2PntFound->CornerPnt;
		}

		return PointOfCorner;
	}

	FCornerNode GetCornerByPoint(const FExactPoint2D&PointOfCorner)
	{
		FCorner2Pnt *Corner2PntFound =
			CornerPntsInScene.FindByPredicate([PointOfCorner](const FCorner2Pnt& Corner2Pnt)
		{
			return Corner2Pnt.CornerPnt == PointOfCorner;
		});

		FCornerNode CornerNode;
		if (Corner2PntFound)
		{
			CornerNode = Corner2PntFound->CornerNode;
		}

		return CornerNode;
	}
};


typedef FExactDataConv::FArrDeclUtil<ACornerActor*, ALineWallActor*, ARoomActor*> FArrDeclWallData;

class FWallArrObserver : public FArrDeclWallData::FCgArrObserver
{
public:
	virtual void before_create_edge(const X_monotone_curve_2&  c,
		Vertex_handle  v1,
		Vertex_handle  v2)
	{

	}

	virtual void after_create_edge(Halfedge_handle  e)
	{

	}
};

// Sets default values for this component's properties
UWallLinkageComponent::UWallLinkageComponent()
{
	WallArrangement = new FArrDeclWallData::FCgArrangement();
	WallArrObserver = new FWallArrObserver();
}

void UWallLinkageComponent::InitLinkage()
{
}

void UWallLinkageComponent::AutoBuildAreaByWalls(bool bReserveCurArea)
{
	FAutoBuildArea AutoBuildArea;
	AutoBuildArea.Init(this);
	AutoBuildArea.AutoBuildAreaByWalls(bReserveCurArea);
}

void UWallLinkageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	delete WallArrangement;
	delete WallArrObserver;
	Super::EndPlay(EndPlayReason);
}
