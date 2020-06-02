
#include "MeshObject.h"
#include "kTriangle.h"
#include "MXFile/MXFile.h"

extern bool LoadBool(ISerialize &Ar);
extern kBox3D LoadBox(ISerialize &Ar);
extern std::string LoadStr(ISerialize &Ar);
extern void SaveStr(ISerialize &Ar, std::string &str);
extern void SaveBox(ISerialize &Ar, kBox3D box);

void FConvexAggGeom::ConvexElem::Serialize(ISerialize &Ar)
{
	SERIALIZE_VEC(VertexData);

	if (Ar.IsLoading())
	{
		ElemBox = LoadBox(Ar);
	}
	else if (Ar.IsSaving())
	{
		Ar << ElemBox;
	}
}

void FConvexAggGeom::Serialize(ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		int Num = (int)ConvexElems.size();
		Ar << Num;
		for (size_t i = 0; i < ConvexElems.size(); ++i)
		{
			ConvexElem &Elem = ConvexElems[i];
			Elem.Serialize(Ar);
		}
	}
	else if (Ar.IsLoading())
	{
		int Num = 0;
		Ar << Num;
		ConvexElems.resize(Num);
		for (size_t i = 0; i < ConvexElems.size(); ++i)
		{
			ConvexElem &Elem = ConvexElems[i];
			Elem.Serialize(Ar);
		}
	}
}


void MeshObject::Serialize(ISerialize &Ar, int Ver)
{
	if (Ar.IsSaving())
	{
		int NumMtrl = Surfaces.size();
		Ar << NumMtrl;
		Ar.Serialize(&Surfaces[0], NumMtrl * sizeof(int));

		int NumSections = Meshes.size();
		Ar << NumSections;
		for (int i = 0; i < NumSections; ++i)
		{
			FMeshSection *Section = Meshes[i];
			SerializeMeshSection(*Section, Ar, Ver);
		}

		SaveStr(Ar, Name);
		SaveBox(Ar, Bounds);
	}
	else if (Ar.IsLoading())
	{
		int NumMtrl = 0;
		Ar << NumMtrl;
		Surfaces.resize(NumMtrl);
		Ar.Serialize(&Surfaces[0], NumMtrl * sizeof(int));

		int NumSection = 0;
		Ar << NumSection;
		if (NumSection > 0)
		{
			AddSection(NumSection);
			for (int i = 0; i < NumSection; ++i)
			{
				FMeshSection *Section = Meshes[i];
				SerializeMeshSection(*Section, Ar, Ver);
				Section->SurfaceIndex = Surfaces[i];
			}
		}

		if (Ver > MODELFILE_BODY_VER_6)
		{
			Name = LoadStr(Ar);
			Bounds = LoadBox(Ar);
		}
	}

	SerializeAggGeom(Ar, Ver);
	SerializeUCX(Ar, Ver);
}

void MeshObject::SerializeMeshSection(FMeshSection &Section, ISerialize &Ar, int Ver)
{
	if (Ar.IsSaving())
	{
		int NumVerts = Section.VertCount;
		Ar << NumVerts;

		for (int i = 0; i < NumVerts; ++i)
		{
			bool bFlipTangentY = false;
			kColor Color;
			Ar << Section.Vertices[i];
			Ar << Section.Normals[i];
			Ar << bFlipTangentY;
			Ar << Section.Tangents[i];
			Ar << Color;
			Ar << Section.UVs[i];
		}

		int NumIndices = (int)Section.Indices.size();
		Ar << NumIndices;

		Ar.Serialize(&Section.Indices[0], sizeof(int)*NumIndices);

		Ar << Section.SectionBox;
		Ar << Section.bEnableCollision;
		Ar << Section.bSectionVisible;
	}
	else if (Ar.IsLoading())
	{
		kColor Color;
		bool bFlipTangentY = false;
		int NumVerts = 0;
		Ar << NumVerts;

		Section.Vertices.resize(NumVerts);
		Section.Normals.resize(NumVerts);
		Section.Tangents.resize(NumVerts);
		Section.UVs.resize(NumVerts);

		for (int i = 0; i < NumVerts; ++i)
		{
			Ar << Section.Vertices[i];
			Ar << Section.Normals[i];
			bFlipTangentY = LoadBool(Ar);
			Ar << Section.Tangents[i];
			Ar << Color;
			Ar << Section.UVs[i];
		}

		int NumIndices = 0;
		Ar << NumIndices;
		Section.Indices.resize(NumIndices);

		Ar.Serialize(&Section.Indices[0], sizeof(int)*NumIndices);

		Section.SectionBox = LoadBox(Ar);
		Section.bEnableCollision = LoadBool(Ar);
		Section.bSectionVisible = LoadBool(Ar);
	}
}

void MeshObject::SerializeAggGeom(ISerialize &Ar, int Ver)
{
	if (Ar.IsLoading())
	{
		if (Ver > MODELFILE_BODY_VER_1)
		{
			AggGeom.Serialize(Ar);
		}
	}
	else if (Ar.IsSaving())
	{
		AggGeom.Serialize(Ar);
	}
}

void MeshObject::SerializeUCX(ISerialize &Ar, int Ver)
{
	if (Ar.IsLoading())
	{
		if (Ver > MODELFILE_BODY_VER_2)
		{
			int Num = 0;
			Ar << Num;
			UCXData.Vertices.resize(Num);
			if (Num > 0)
			{
				Ar.Serialize(&UCXData.Vertices[0], sizeof(kVector3D)*Num);
			}

			Num = 0;
			Ar << Num;
			UCXData.Indices.resize(Num);
			if (Num > 0)
			{
				Ar.Serialize(&UCXData.Indices[0], sizeof(int)*Num);
			}
		}
	}
	else if (Ar.IsSaving())
	{
		int Num = 0;

		Num = UCXData.Vertices.size();
		Ar << Num;
		if (Num > 0)
		{
			Ar.Serialize(&UCXData.Vertices[0], sizeof(kVector3D)*Num);
		}

		Num = UCXData.Indices.size();
		Ar << Num;
		if (Num > 0)
		{
			Ar.Serialize(&UCXData.Indices[0], sizeof(int)*Num);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
MeshObject::MeshObject(IObject *InOwner)
	: Callback(nullptr)
{
}

MeshObject::~MeshObject()
{
	if (SavedMaterialInfo)
	{
		delete SavedMaterialInfo;
	}
	ClearMesh();
}

void MeshObject::ClearMesh()
{
	for (int i = 0; i < (int)Meshes.size(); ++i)
	{
		FMeshSection *mesh = Meshes[i];
		delete mesh;
	}
	Meshes.clear();
}

void MeshObject::ClearMaterial()
{
	for (int i = 0; i < (int)Meshes.size(); ++i)
	{
		FMeshSection *mesh = Meshes[i];
		if (mesh)
		{
			mesh->SurfaceIndex = -1;
		}
	}
	Surfaces.clear();
}

void MeshObject::SaveMaterials()
{
	size_t nMesh = Meshes.size();

	if (nMesh > 0)
	{
		if (!SavedMaterialInfo)
		{
			SavedMaterialInfo = new FSaveMaterialInfo();
		}

		SavedMaterialInfo->Surfaces = Surfaces;
		SavedMaterialInfo->SectionRefs.resize(nMesh);

		for (size_t i = 0; i < nMesh; ++i)
		{
			FMeshSection *mesh = Meshes[i];
			SavedMaterialInfo->SectionRefs[i] = mesh->SurfaceIndex;
		}
	}
}

void MeshObject::RestoreMaterials()
{
	if (SavedMaterialInfo)
	{
		size_t nNew = Meshes.size();
		size_t nOld = SavedMaterialInfo->SectionRefs.size();

		std::vector<ObjectID> newSurfaces;
		std::vector<int> oldIdxNew;

		if (nNew < nOld)
		{
			for (size_t i = 0; i < nNew; ++i)
			{
				ObjectID SurfaceID = SavedMaterialInfo->Surfaces[SavedMaterialInfo->SectionRefs[i]];
				
				size_t k = 0;
				for (; k < newSurfaces.size(); ++k);
				
				if (k >= newSurfaces.size())
				{
					newSurfaces.push_back(SurfaceID);
				}

				oldIdxNew.push_back(k);
			}

			for (size_t i = 0; i < Meshes.size(); ++i)
			{
				FMeshSection *mesh = Meshes[i];
				if (mesh)
				{
					int oldSurfaceIndex = SavedMaterialInfo->SectionRefs[i];
					int newSurfaceIndex = oldIdxNew[oldSurfaceIndex];
					mesh->SurfaceIndex = newSurfaceIndex;
				}
			}

			Surfaces.swap(newSurfaces);
		}
		else
		{
			for (size_t i = 0; i < nOld; ++i)
			{
				FMeshSection *mesh = Meshes[i];
				if (mesh)
				{
					mesh->SurfaceIndex = SavedMaterialInfo->SectionRefs[i];;
				}
			}

			Surfaces.swap(SavedMaterialInfo->Surfaces);
		}
	}
}

int  MeshObject::GetSectionCount()
{
	return (int)Meshes.size();
}

ObjectID  MeshObject::GetSectionSurfaceID(int SubModelIndex)
{
	if (SubModelIndex >= 0 && SubModelIndex < Meshes.size())
	{
		FMeshSection * MeshSection = Meshes[SubModelIndex];
		if (MeshSection->SurfaceIndex != -1)
		{
			return Surfaces[MeshSection->SurfaceIndex];
		}
	}
	return INVALID_OBJID;
}

ISurfaceObject *MeshObject::GetSectionSurface(int SubModelIndex)
{
	ObjectID ID = GetSectionSurfaceID(SubModelIndex);
	if (Callback)
	{
		return Callback->GetMeshSurface(ID);
	}
	return nullptr; 
}

bool MeshObject::GetSectionMesh(int SectionIndex, float *&pVertices, float *&pNormals, float *&pTangents, float *&pUVs, float *&pLightmapUVs, int &NumVerts, int *&pIndices, int &NumIndices)
{
	if (SectionIndex >= 0 && SectionIndex < Meshes.size())
	{
		FMeshSection *pSection = Meshes[SectionIndex];
		pVertices = (float *)&(pSection->Vertices[0]);
		pNormals = (float *)&(pSection->Normals[0]);
		pTangents = (float *)&(pSection->Tangents[0]);
		pUVs = (float *)&(pSection->UVs[0]);
		pLightmapUVs = pSection->LightmapUVs.empty() ? nullptr : (float *)&(pSection->LightmapUVs[0]);
		pIndices = &(pSection->Indices[0]);
		NumIndices = (int)pSection->Indices.size();
		NumVerts = (int)pSection->Vertices.size();
	}
	else
	{
		pIndices = nullptr;
		pVertices = pNormals = pUVs = pLightmapUVs = nullptr;
		NumVerts = NumIndices = 0;
	}
	return NumVerts > 0;
}

int MeshObject::FindSurface(ObjectID SurfaceID)
{
	size_t i = 0;
	for (; i < Surfaces.size(); ++i)
	{
		if (Surfaces[i] == SurfaceID)
		{
			return (int)i;
		}
	}
	return  -1;
}

int MeshObject::GetSurfaceUseCount(int SurfaceIndex)
{
	int useCount = 0;
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		if (Meshes[i]->SurfaceIndex == SurfaceIndex)
		{
			useCount++;
		}
	}
	return useCount;
}

void MeshObject::ReIndices()
{
	size_t nMesh = Meshes.size();
	std::vector<ObjectID> NewSurfaces;

	for (size_t i = 0; i < nMesh; ++i)
	{
		FMeshSection *MeshSection = Meshes[i];
		if (MeshSection->SurfaceIndex >= 0)
		{
			ObjectID SurfaceID = Surfaces[MeshSection->SurfaceIndex];

			size_t k = 0;
			for (; k < NewSurfaces.size(); ++k)
			{
				if (NewSurfaces[k] == SurfaceID)
				{
					break;
				}
			}

			if (k >= NewSurfaces.size())
			{
				NewSurfaces.push_back(SurfaceID);
			}

			MeshSection->SurfaceIndex = (int)k;
		}
		else
		{
			MeshSection->SurfaceIndex = -1;
		}
	}

	Surfaces.swap(NewSurfaces);
}

void MeshObject::SetSectionSurface(int SubModelIndex, ObjectID SurfaceID)
{
	if (SubModelIndex >= 0 && SubModelIndex < Meshes.size())
	{
		FMeshSection *MeshSection = Meshes[SubModelIndex];
		if (MeshSection && MeshSection->SurfaceIndex >= 0)
		{
			int OldIndex = MeshSection->SurfaceIndex;
			ObjectID OldSurfaceID = Surfaces[OldIndex];
			if (Callback)
			{
				Callback->UnLink(OldSurfaceID, SubModelIndex);
			}

			MeshSection->SurfaceIndex = -1;

			int useCount = GetSurfaceUseCount(MeshSection->SurfaceIndex);
			if (useCount <= 0)
			{
				Surfaces[OldIndex] = INVALID_OBJID;
				ReIndices();
			}
		}

		if (SurfaceID != INVALID_OBJID)
		{
			int NewIndex = FindSurface(SurfaceID);
			if (NewIndex >= 0)
			{
				MeshSection->SurfaceIndex = NewIndex;
			}
			else
			{
				MeshSection->SurfaceIndex = (int)Surfaces.size();
				Surfaces.push_back(SurfaceID);
			}

			if (Callback)
			{
				Callback->Link(SurfaceID, SubModelIndex);
			}
		}
	}
}

FMeshSection *MeshObject::AddMesh()
{
	FMeshSection *mesh = new FMeshSection();
	Meshes.push_back(mesh);
	return mesh;
}

void MeshObject::AddQuad(FMeshSection *Mesh, const kVector3D &V0, const kVector3D &V1, const kVector3D &V2, const kVector3D &V3, const kVector3D &Normal, const kVector3D &Tan, int MaterialIndex)
{
	/*
	2-------3
	|		|
	1-------0
	*/
	kPoint UVScale = kPoint((V1 - V0).Size(), (V2 - V1).Size());
	const kPoint UVs[4] = { kPoint(1.0f, 1.0f), kPoint(0,1.0f), kPoint(0,0), kPoint(1.0f, 0) };

	int Index0 = AddVert(Mesh, V0, Normal, Tan, UVs[0] * UVScale, UVs[0]);
	int Index1 = AddVert(Mesh, V1, Normal, Tan, UVs[1] * UVScale, UVs[1]);
	int Index2 = AddVert(Mesh, V2, Normal, Tan, UVs[2] * UVScale, UVs[2]);
	int Index3 = AddVert(Mesh, V3, Normal, Tan, UVs[3] * UVScale, UVs[3]);

	AddTri(Mesh, Index0, Index3, Index1);
	AddTri(Mesh, Index1, Index3, Index2);
}

int  MeshObject::AddVertDefaultUV(FMeshSection *Mesh, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &LightmapUV)
{
	int Index = (int)Mesh->Vertices.size();
	Mesh->Vertices.push_back(Vert);
	Mesh->Normals.push_back(Normal);
	Mesh->Tangents.push_back(Tan);
	Mesh->UVs.push_back(kPoint(Vert.x, Vert.y));
	Mesh->LightmapUVs.push_back(LightmapUV);
	return Index;
}

int MeshObject::AddVert(FMeshSection *MeshSection, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightmapUV)
{
	int Index = (int)MeshSection->Vertices.size();
	MeshSection->Vertices.push_back(Vert);
	MeshSection->Normals.push_back(Normal);
	MeshSection->Tangents.push_back(Tan);
	MeshSection->UVs.push_back(UV);
	MeshSection->LightmapUVs.push_back(LightmapUV);
	Bounds.Add(Vert);
	return Index;
}

void MeshObject::AddTri(FMeshSection *Mesh, int V0, int V1, int V2)
{
	Mesh->Indices.push_back(V0);
	Mesh->Indices.push_back(V1);
	Mesh->Indices.push_back(V2);
}

void MeshObject::SetMeshObjectCallback(IMeshObjectCallback *InCallback)
{
	Callback = InCallback;
}

void MeshObject::AddSection(int NumSection)
{
	for (int i = 0; i < NumSection; ++i)
	{
		AddMesh();
	}
}

FMeshSection *MeshObject::GetSection(int SectionIndex)
{
	if (SectionIndex >= 0 && SectionIndex < Meshes.size())
	{
		return Meshes[SectionIndex];
	}
	return nullptr;
}

int MeshObject::GetNumSections()
{
	return (int)Meshes.size();
}

struct FHitVert
{
	kVector3D	Position;
	kVector3D	Normal;
	float		Dist;
	int			iSection;
};

bool MeshObject::HitTest(const kVector3D &RayStart, const kVector3D &RayDir, int *OutSectionIndex /*= nullptr*/, kVector3D *OutPosition /*= nullptr*/, kVector3D *OutNormal /*= nullptr*/)
{
	std::vector<FHitVert> Hits;

	FHitVert HitInfo;

	for (size_t sectionIndex = 0; sectionIndex < Meshes.size(); ++sectionIndex)
	{
		FMeshSection *pSection = Meshes[sectionIndex];
		
		for (size_t tri = 0; tri < pSection->Indices.size(); tri+=3)
		{
			int v0 = pSection->Indices[tri];
			int v1 = pSection->Indices[tri+1];
			int v2 = pSection->Indices[tri+2];

			kTriangle Tri;
			Tri.pointA = pSection->Vertices[v0];
			Tri.pointB = pSection->Vertices[v1];
			Tri.pointC = pSection->Vertices[v2];

			if (Tri.GetIntersectionWithLimitedLine(kLine(RayStart, RayDir*10000.0f), HitInfo.Position))
			{
				HitInfo.iSection = sectionIndex;
				HitInfo.Normal = pSection->Normals[v0];
				HitInfo.Dist = (HitInfo.Position - RayStart).Size();

				if (Hits.empty())
				{
					Hits.push_back(HitInfo);
				}
				else
				{
					size_t k = Hits.size();
					Hits.resize(k + 1);
					
					size_t i = k;
					for (; i > 0; --i)
					{
						if (HitInfo.Dist < Hits[i - 1].Dist)
						{
							Hits[i] = Hits[i - 1];
							break;
						}
					}

					Hits[i] = HitInfo;
				}
			}
		}
	}

	if (!Hits.empty())
	{
		if (OutSectionIndex)
		{
			*OutSectionIndex = Hits[0].iSection;
		}

		if (OutNormal)
		{
			*OutNormal = Hits[0].Normal;
		}

		if (OutPosition)
		{
			*OutPosition = Hits[0].Position;
		}
	}

	return !Hits.empty();
}


