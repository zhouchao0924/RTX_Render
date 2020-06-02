
#pragma once

#include "IMeshObject.h"

struct FUCXData
{
	std::vector<kVector3D>  Vertices;
	std::vector<int>		Indices;
};

struct FConvexAggGeom
{
	struct ConvexElem
	{
		std::vector<kVector3D>  VertexData;
		kBox3D					ElemBox;
		void Serialize(ISerialize &Ar);
	};
	std::vector<ConvexElem> ConvexElems;
	void Serialize(ISerialize &Ar);
};


struct FMeshSection
{
	FMeshSection()
		:bEnableCollision(false)
		,bSectionVisible(true)
		,SurfaceIndex(-1)
	{
	}
	std::vector<kVector3D> Vertices;
	std::vector<kVector3D> Normals;
	std::vector<kVector3D> Tangents;
	std::vector<kPoint>	   UVs;
	std::vector<kPoint>	   LightmapUVs;
	std::vector<int>	   Indices;
	int					   VertCount;
	int					   PrimCount;
	bool				   bEnableCollision;
	bool				   bSectionVisible;
	int					   SurfaceIndex;
	kBox3D				   SectionBox;
};

struct FSaveMaterialInfo
{
	std::vector<int>		SectionRefs;
	std::vector<ObjectID>	Surfaces;
};

class MeshObject : public IMeshObject
{
public:
	MeshObject(IObject *Owner);
	~MeshObject();
	int  GetSectionCount() override;
	bool GetSectionMesh(int SectionIndex, float *&pVertices, float *&pNormals, float *&pTangents, float *&pUVs, float *&pLightmapUVs, int &NumVerts, int *&pIndices, int &NumIndices) override;
	int  GetSectionSurfaceID(int SubModelIndex) override;
	ISurfaceObject *GetSectionSurface(int SubModelIndex) override;
	void SetSectionSurface(int SubModelIndex, ObjectID SurfaceID) override;
	void SetMeshObjectCallback(IMeshObjectCallback *Callback) override;
	void AddQuad(FMeshSection *Mesh, const kVector3D &V0, const kVector3D &V1, const kVector3D &V2, const kVector3D &V3, const kVector3D &Normal, const kVector3D &Tan, int MaterialIndex);
	int  AddVert(FMeshSection *Mesh, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightmapUV);
	int  AddVertDefaultUV(FMeshSection *Mesh, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &LightmapUV);
	void AddTri(FMeshSection *Mesh, int V0, int V1, int V2);
	void AddSection(int NumSection);
	bool HitTest(const kVector3D &RayStart, const kVector3D &RayDir, int *OutSectionIndex = nullptr, kVector3D *OutPosition = nullptr, kVector3D *OutNormal = nullptr);
	FMeshSection *GetSection(int SectionIndex);
	int GetNumSections();
public:
	void Serialize(ISerialize &Ar, int Ver);
	void SerializeAggGeom(ISerialize &Ar, int Ver);
	void SerializeUCX(ISerialize &Ar, int Ver);
protected:
	friend class Primitive;
	FMeshSection *AddMesh();
	void SerializeMeshSection(FMeshSection &Section, ISerialize &Ar, int Ver);
	void SaveMaterials();
	void RestoreMaterials();
	void ClearMesh();
	void ClearMaterial();
	int  FindSurface(ObjectID SurfaceID);
	int  GetSurfaceUseCount(int SurfaceIndex);
	void ReIndices();
protected:
	std::string					Name;
	std::vector<ObjectID>		Surfaces;
	std::vector<FMeshSection *>	Meshes;
	kBox3D						Bounds;
	FConvexAggGeom				AggGeom;
	FUCXData					UCXData;
	FSaveMaterialInfo			*SavedMaterialInfo;
	IMeshObjectCallback			*Callback;
};

