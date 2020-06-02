
#include "assert.h"
#include "Primitive.h"
#include "ISuite.h"
#include "Elements/Surface.h"
#include "Class/Property.h"

//////////////////////////////////////////////////////////////////////////
BEGIN_DERIVED_CLASS(Primitive, BuildingObject)
	ADD_PROP(bVisible, BoolProperty)
END_CLASS()

Primitive::Primitive()
	:bHasCached(false)
	,bVisible(true)
	,Mesh(nullptr)
{
}

Primitive::~Primitive()
{
	if (Mesh)
	{
		delete Mesh;
		Mesh = nullptr;
	}
}

bool Primitive::IsVisible()
{
	return bVisible;
}

void Primitive::Serialize(ISerialize &Ar)
{
	BuildingObject::Serialize(Ar);
}

void Primitive::SetVisible(bool bInVisible)
{
	bVisible = bInVisible;
}

void Primitive::ClearCached()
{
	bHasCached = false;
	if (Mesh)
	{		
		Mesh->SaveMaterials();
		UnLinkAllSurface();
		Mesh->ClearMesh();
		Mesh->ClearMaterial();
	}
}

void Primitive::UnLinkAllSurface()
{
	if (Mesh)
	{
		int nSection = Mesh->GetNumSections();
		for (int i = 0; i < nSection; ++i)
		{
			ObjectID SurfaceID = Mesh->GetSectionSurfaceID(i);
			UnLink(SurfaceID, i);
			Mesh->ClearMaterial();
		}
	}
}

void Primitive::MarkNeedUpdate()
{
	ClearCached();
	BuildingObject::MarkNeedUpdate();
}

void Primitive::ConditonBuild()
{
	if (!bHasCached)
	{
		Build();
		if (Mesh)
		{
			Mesh->RestoreMaterials();
		}
		bHasCached = true;
	}
}

FMeshSection *Primitive::AddMesh()
{
	if (!Mesh)
	{
		Mesh = new MeshObject(this);
		Mesh->SetMeshObjectCallback(this);
	}
	return Mesh->AddMesh();
}

void Primitive::AddQuad(FMeshSection *MeshSection, const kVector3D &V0, const kVector3D &V1, const kVector3D &V2, const kVector3D &V3, const kVector3D &Normal, const kVector3D &Tan, int MaterialIndex)
{
	assert(Mesh && MeshSection);
	Mesh->AddQuad(MeshSection, V0, V1, V2, V3, Normal, Tan, MaterialIndex);
}

int  Primitive::AddVertDefaultUV(FMeshSection *MeshSection, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &LightmapUV)
{
	assert(Mesh && MeshSection);
	return Mesh->AddVertDefaultUV(MeshSection, Vert, Normal, Tan, LightmapUV);
}

int Primitive::AddVert(FMeshSection *MeshSection, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightmapUV)
{
	assert(Mesh && MeshSection);
	return Mesh->AddVert(MeshSection, Vert, Normal, Tan, UV, LightmapUV);
}

void Primitive::AddTri(FMeshSection *MeshSection, int V0, int V1, int V2)
{
	assert(Mesh && MeshSection);
	Mesh->AddTri(MeshSection, V0, V1, V2);
}

IMeshObject *Primitive::GetMeshObject(int MeshIndex /*= 0*/, bool bBuildIfNotExist /*= true*/)
{
	if (bBuildIfNotExist)
	{
		ConditonBuild();
	}
	return Mesh;
}

kBox3D Primitive::GetBounds()
{ 
	ConditonBuild();
	if (Mesh)
	{
		Mesh->Bounds;
	}
	return kBox3D();
}

void Primitive::Link(ObjectID SurfaceID, int SubModelIndex)
{
	Surface *pSurface = SUITE_GET_BUILDING_OBJ(SurfaceID, Surface);
	if (pSurface)
	{
		pSurface->Link(_ID, SubModelIndex);
	}
}

void Primitive::UnLink(ObjectID SurfaceID, int SubModelIndex)
{
	Surface *pSurface = SUITE_GET_BUILDING_OBJ(SurfaceID, Surface);
	if (pSurface)
	{
		pSurface->UnLink(_ID, SubModelIndex);
	}
}

int Primitive::GetSurfaceCount()
{
	ConditonBuild();
	if (Mesh)
	{
		return (int)Mesh->Surfaces.size();
	}
	return 0;
}

ISurfaceObject *Primitive::GetSurfaceObject(int SurfaceIndex)
{
	ISurfaceObject *pSurfaceObject = nullptr;

	if (SurfaceIndex >= 0 && SurfaceIndex < Mesh->Surfaces.size())
	{
		ObjectID SurfaceID = Mesh->Surfaces[SurfaceIndex];
		Surface *pSurface = SUITE_GET_BUILDING_OBJ(SurfaceID, Surface);
		if (pSurface)
		{
			pSurfaceObject = pSurface->GetObject();
		}
	}

	return pSurfaceObject;
}

ISurfaceObject *Primitive::GetMeshSurface(int SurfaceID)
{
	Surface *pSurface = SUITE_GET_BUILDING_OBJ(SurfaceID, Surface);
	if (pSurface)
	{
		return pSurface->GetObject();
	}
	return nullptr;
}

kPoint Primitive::ToUV(const kPoint &InUV)
{ 
	kPoint UV;
	UV.x = InUV.x < 0 ? 1.0f - InUV.x : InUV.x;
	UV.y = InUV.y < 0 ? 1.0f - InUV.y : InUV.y;
	return kPoint(UV.x *0.01f, UV.y*0.01f);
}

bool Primitive::GetTextureInfo(int iTex, int &Width, int &Height, int &MipCount, int &Pitch, ETextureFormat &Format)
{
	return false;
}

bool Primitive::GetTextureData(int iTex, int MipIndex, void *&pData, int &nBytes)
{
	return false;
}


