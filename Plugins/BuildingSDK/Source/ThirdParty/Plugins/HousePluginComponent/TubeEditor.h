#pragma once

#include "Public/ITubeEditor.h"

class TubeEditor :public ITubeEditor
{
public:
	ObjectID AddTube(float Height, float Width, float Length, const kPoint &Loc, const kPoint &Forward) override;
	bool UpdateTube(ObjectID ObjID, const kPoint &Loc, float RotAngle, const kXform &Transform = kXform()) override;
	bool UpdateTubeAttr(ObjectID ObjID, float Width, float Length, const kPoint &Loc, float Angle) override;
	bool SetTubeSurface(ObjectID ObjID, ObjectID SectionIndex, const char* MaterialUri, int MaterialType) override;
	void DeleteTube(ObjectID ObjID) override;
	bool GetTubeBorder(ObjectID ObjID, kVector3D* OutBorder)override;
	bool SetTubeHeightVal(ObjectID ObjID, float Height)override;
private:
	kVector3D GetVertexNormal(const kVector3D &Vert, const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2);
	void SetTubeAttr(IObject* Obj, float Height, float Width, float Length, const kPoint &Loc, const kPoint &Forward);
	void SetMeshData(IObject* Obj, float Heigth, float Width, float Length, const kPoint &Loc, const kPoint &Forward, const kPoint &WallDir);
	void SetVertexData(const kPoint &Loc, const kPoint &Forward, const kPoint &WallDirVect, float Height, float Width, float Length);

	int AddVert(int SectionIndex, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightMapUV);
	void AddTri(int MeshSection, int V0, int V1, int V2);
	bool GetWallDirect(ObjectID WallId, kPoint& Start, kPoint& End);
	std::vector<kPoint> GetVertexUV(const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2, const kVector3D &AdjVert3);
	void SetDefUV(float wU, float lU);
	void ModulateNormalVal(kXform &Transform);
private:
	IMeshBuilder* TubeMeshBuilder;
	IMeshObject* TubeMesh;
	std::vector<kVector3D> BorderVect;
	ISurfaceObject* WallSurfObj;
};