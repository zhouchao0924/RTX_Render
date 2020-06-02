
#pragma once

#include "BuildingObject.h"
#include "Mesh/MeshObject.h"

class Primitive :public  BuildingObject , public IMeshObjectCallback
{
	DEFIN_CLASS()
public:
	Primitive();
	~Primitive();
	virtual bool IsVisible();
	virtual void SetVisible(bool bInVisible);
	virtual void ClearCached();
	virtual void Build() {}
	virtual void MarkNeedUpdate();
	virtual void Serialize(ISerialize &Ar);
	int  GetMeshCount() override { return Mesh? 1 : 0; }
	IMeshObject *GetMeshObject(int MeshIndex, bool bBuildIfNotExist = true) override;
	int   GetSurfaceCount();
	int   GetTextureCount() { return 0; }
	bool  GetTextureInfo(int iTex, int &Width, int &Height, int &MipCount, int &Pitch, ETextureFormat &Format);
	bool  GetTextureData(int iTex, int MipIndex, void *&pData, int &nBytes);
	ISurfaceObject *GetSurfaceObject(int SurfaceIndex);
	ISurfaceObject *GetMeshSurface(int SurfaceID) override;
	EObjectType GetType() { return EPrimitive; }
	kBox3D GetBounds();
	void ConditonBuild();
	static kPoint ToUV(const kPoint &UV);
protected:
	friend class MeshObject;
	FMeshSection *AddMesh();
	void AddQuad(FMeshSection *Mesh, const kVector3D &V0, const kVector3D &V1, const kVector3D &V2, const kVector3D &V3, const kVector3D &Normal, const kVector3D &Tan, int MaterialIndex);
	int  AddVert(FMeshSection *Mesh, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightmapUV);
	int  AddVertDefaultUV(FMeshSection *Mesh, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &LightmapUV);
	void AddTri(FMeshSection *Mesh, int V0, int V1, int V2);
	void Link(ObjectID SurfaceID, int SubSectionIndex) override;
	void UnLink(ObjectID SurfaceID, int SubSectionIndex)override;
	void UnLinkAllSurface();
protected:
	bool					bHasCached;
	bool					bVisible;
	MeshObject				*Mesh;
};


