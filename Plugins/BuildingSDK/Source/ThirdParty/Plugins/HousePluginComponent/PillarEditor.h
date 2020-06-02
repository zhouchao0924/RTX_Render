#pragma once

#include "Public/IPillarEditor.h"

class PillarEditor : public IPillarEditor
{
public:
	ObjectID AddPillar(bool IsAnch, float Height, float Width, float Length, const kPoint &Loc, const kPoint &Forward) override;
	bool UpdatePillar(ObjectID ObjID, const kPoint &Loc, float RotAngle, const kXform &Transform = kXform()) override;
	bool UpdatePillarAttr(ObjectID ObjID, float Width, float Length, const kPoint &Loc, float Angle) override;
	bool SetPillarSurface(ObjectID ObjID, ObjectID SectionIndex, const char* MaterialUri, int MaterialType) override;
	void DeletePillar(ObjectID ObjID) override;
	bool GetPillarBorder(ObjectID ObjID, kVector3D *OutBorder)override;
	bool SetPillarHeightVal(ObjectID ObjID, float Height)override;
private:
	kVector3D GetVertexNormal(const kVector3D &Vert, const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2);
	void SetPillarAttr(IObject* Obj, float Height, float Width, float Length, bool IsAnchor, const kPoint &Loc, const kPoint &Forward);
	void SetMeshData(IObject* Obj, float Heigth, float Width, float Length, const kPoint &Loc, const kPoint &Forward, const kPoint &WallDir);
	void SetVertexData(const kPoint &Loc, const kPoint &Forward, const kPoint &WallDir, float Height, float Width, float Length);

	//kArray<float> GetTubeAdjWallArea(IObject* obj);
	int AddVert(int SectionIndex, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightMapUV);
	void AddTri(int MeshSection, int V0, int V1, int V2);
	bool GetWallDirect(ObjectID WallId, kPoint& Start, kPoint& End);
	std::vector<kPoint> GetVertexUV(const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2, const kVector3D &AdjVert3);
	void SetDefUV(float wU, float lU);
	void ModulateNormalVal(kXform &Transform);
private:
	IMeshBuilder *PillarMeshBuilder;
	IMeshObject *PillarMesh;
	ISurfaceObject* WallSurfObj;
	std::vector<kVector3D> BorderVect;
};

