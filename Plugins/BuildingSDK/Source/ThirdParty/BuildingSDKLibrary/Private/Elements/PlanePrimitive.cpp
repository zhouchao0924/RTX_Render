
#include "PlanePrimitive.h"
#include "ISuite.h"
#include "kBox.h"
#include "Room.h"
#include "poly2tri/poly2tri.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(PlanePrimitive, Primitive)
	ADD_PROP(ZPos, FloatProperty)
	ADD_PROP(Thickness, FloatProperty)
END_CLASS()

PlanePrimitive::PlanePrimitive()
	:SurfaceID(INVALID_OBJID)
	,Thickness(0.2f)
{ 
}

void PlanePrimitive::Serialize(ISerialize &Ar)
{
	Primitive::Serialize(Ar);
	Ar << SurfaceID;
}

int  PlanePrimitive::GetMaterial(int ChannelID)
{
	return SurfaceID;
}

void PlanePrimitive::SetMaterial(int InMaterialID, int ChannelID /*= 0*/)
{
	SurfaceID = InMaterialID;
}

static float zOffset_Magic = 0.00f;
static float zExpand_Magic = 0.00f;

void PlanePrimitive::Build()
{
	size_t		 nPoints = 0;
	float	*pHeights = nullptr;
	kPoint	*Polygons = nullptr;
	float    zOffset = 0;

	nPoints = GetPolygons(Polygons, pHeights, zOffset);

	Bounds.Invalid();

	std::vector<p2t::IndexPoint*> polyline;
	for (size_t i = 0; i < nPoints; ++i)
	{
		kPoint &Pre = Polygons[(i - 1 + nPoints) % nPoints];
		kPoint &Next = Polygons[(i + 1) % nPoints];
		kPoint &pt = Polygons[i];

		kPoint v1 = (pt - Pre).Normalize();
		kPoint v2 = (Next - pt).Normalize();
		kPoint halfV_offsetDir;

		if (kPoint::CrossProduct(v1, v2) < 0) //凹转折点
		{
			halfV_offsetDir = (v2 - v1);
		}
		else
		{
			halfV_offsetDir = -(v2 - v1);
		}

		halfV_offsetDir.Normalize();
		halfV_offsetDir = halfV_offsetDir*zExpand_Magic;

		kPoint pos = kPoint(pt.x + halfV_offsetDir.x, pt.y + halfV_offsetDir.y);
		Bounds.Add(pos);

		polyline.push_back(new p2t::IndexPoint(pos.x, pos.y, i));
	}

	std::vector<kPoint> lightmapUVs;
	lightmapUVs.resize(nPoints);
	kVector3D Ext = Bounds.GetExtent();
	
	for (size_t i = 0; i < nPoints; ++i)
	{
		p2t::IndexPoint *pt = polyline[i];
		Polygons[i] = kPoint(pt->x, pt->y);
		lightmapUVs[i].x = (pt->x - Bounds.MinEdge.x) / Ext.X;
		lightmapUVs[i].y = (pt->y - Bounds.MinEdge.y) / Ext.Y;
	}

	p2t::CDT CDT(polyline);
	CDT.Triangulate();
	std::vector<p2t::Triangle *> Triangles = CDT.GetTriangles();

	// 底面
	FMeshSection *bottom_mesh = AddMesh();
	for (size_t i = 0; i < Triangles.size(); ++i)
	{
		p2t::Triangle *tri = Triangles[i];
		if (tri)
		{
			int tri_index[3] = {};
			for (int c = 0; c < 3; ++c)
			{
				tri_index[c] = tri->GetPoint(c)->refIndex;
			}
			AddTri(bottom_mesh, tri_index[0], tri_index[1], tri_index[2]);
		}
	}

	kPoint Min(Bounds.MinEdge.x, Bounds.MinEdge.y);

	for (size_t i = 0; i < nPoints; ++i)
	{
		kPoint &pt = Polygons[i];
		float  Z = zOffset + (pHeights ? pHeights[i] : 0);
		AddVert(bottom_mesh, kVector3D(pt.x, pt.y, Z), kVector3D(0.7f, 0, 0.7f), kVector3D(1.0f, 0, 0), Primitive::ToUV(pt- Min), lightmapUVs[i]);
	}

	//顶部
	FMeshSection *top_mesh = AddMesh();
	for (size_t i = 0; i < Triangles.size(); ++i)
	{
		p2t::Triangle *tri = Triangles[i];
		if (tri)
		{
			int tri_index[3] = {};
			for (int c = 0; c < 3; ++c)
			{
				tri_index[c] = tri->GetPoint(c)->refIndex;
			}
			AddTri(top_mesh, tri_index[0], tri_index[2], tri_index[1]);
		}
	}

	for (size_t i = 0; i < nPoints; ++i)
	{
		kPoint &pt = Polygons[i];
		float  Z = zOffset + (pHeights ? pHeights[i] : 0) + Thickness;
		AddVert(top_mesh, kVector3D(pt.x, pt.y, Z), kVector3D(0, 0, 1.0f), kVector3D(1.0f, 0, 0), Primitive::ToUV(pt-Min), lightmapUVs[i]);
	}

	//边界
	/*
		2--3
		0--1
	*/
	FMeshSection *bound_mesh = AddMesh();
	for (size_t i = 0; i < nPoints; ++i)
	{
		kPoint s = Polygons[i];
		kPoint e = Polygons[(i + 1) % nPoints];

		float  Z0 = zOffset + (pHeights ? pHeights[i] : 0);
		float  Z1 = zOffset + (pHeights ? pHeights[(i+1)%nPoints] : 0);

		kVector3D P0 = kVector3D(s.x, s.y, Z0);
		kVector3D P1 = kVector3D(e.x, e.y, Z1);

		Z0 += Thickness;
		Z1 += Thickness;

		kVector3D P2 = kVector3D(s.x, s.y, Z0);
		kVector3D P3 = kVector3D(e.x, e.y, Z1);

		kVector3D V = (P1 - P0).Normalize();
		kVector3D Normal = V.CrossProduct(kVector3D(0, 0, 1.0f));

		int index0 = AddVert(bound_mesh, P0, Normal, V, Primitive::ToUV(kPoint(0.0f, 1.0f)), kPoint(0.0f, 1.0f));
		int index1 = AddVert(bound_mesh, P1, Normal, V, Primitive::ToUV(kPoint(1.0f, 1.0f)), kPoint(1.0f, 1.0f));
		int index2 = AddVert(bound_mesh, P2, Normal, V, Primitive::ToUV(kPoint(0.0f, 0.0f)), kPoint(0.0f, 0.0f));
		int index3 = AddVert(bound_mesh, P3, Normal, V, Primitive::ToUV(kPoint(1.0f, 0.0f)), kPoint(1.0f, 0.0f));
			
		AddTri(bound_mesh, index0, index3, index1);
		AddTri(bound_mesh, index0, index2, index3);
	}

	for (size_t i = 0; i < polyline.size(); ++i)
	{
		p2t::IndexPoint *pt = polyline[i];
		if (pt)
		{
			delete pt;
		}
	}
	polyline.clear();
}

//////////////////////////////////////////////////////////////////////////
BEGIN_DERIVED_CLASS(FloorPlane, PlanePrimitive)
	ADD_PROP(RoomID, IntProperty)
END_CLASS()

void FloorPlane::Serialize(ISerialize &Ar)
{
}

int FloorPlane::GetPolygons(kPoint *&pPolygon, float *&pHeights, float &zOffset)
{
	int Num = 0;
	pPolygon = nullptr;
	pHeights = nullptr;
		
	if (_Suite)
	{
		Room *pRoom = SUITE_GET_BUILDING_OBJ(RoomID, Room);
		if (pRoom)
		{
			Num = pRoom->GetPolygon(pPolygon, true);
		}
		zOffset = -Thickness + zOffset_Magic;
	}

	return Num;
}

BEGIN_DERIVED_CLASS(CeilPlane, PlanePrimitive)
	ADD_PROP(RoomID, IntProperty)
END_CLASS()

void CeilPlane::Serialize(ISerialize &Ar)
{
}

int CeilPlane::GetPolygons(kPoint *&pPolygon, float *&pHeights, float &zOffset)
{
	int Num = 0;
	pPolygon = nullptr;
	pHeights = nullptr;

	zOffset = -zOffset_Magic;

	Room *pRoom = SUITE_GET_BUILDING_OBJ(RoomID, Room);
	if (pRoom)
	{
		Num = pRoom->GetPolygon(pPolygon, false);
		pRoom->GetWallHeight(pHeights);
	}

	return Num;
}


