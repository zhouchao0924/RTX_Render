
#pragma once

#include "Public/IPoleEditor.h"

struct FluePrimitve
{
	kPoint UpLeft;
	kPoint DownLeft;
	kPoint DownRight;
	kPoint UpRight;
};


class PoleEditor : public IPoleEditor
{
public:
	ObjectID AddPole(float WallHeight, float Width, float Length, /*bool IsWidth,*/ const kPoint &Loc, const kPoint &Forward) override;
	bool UpdatePole(ObjectID ObjID, float Width, float Lenght, const kPoint &Loc, float Angle) override;
	void DeletePole(ObjectID ObjID) override;
	bool UpdatePole(ObjectID ObjID, const kPoint &Loc, float RotAngle, /*ObjectID WallId,*/ const kXform &Transform = kXform()) override;
	bool SetPoleSurface(ObjectID ObjID, ObjectID SectionIndex, const char *MaterialUri, int MaterialType) override;
	bool GetPoleBorder(ObjectID ObjID, kVector3D *OutBorder)override;
	bool SetPoleHeightVal(ObjectID ObjID, float Height) override;
protected:
	void SetPoleAttribute(ObjectID ObjID, float Width, float Length, float WallHeight, const kPoint &Loc, const kPoint &Forward);
	void GetNearestWall(const FluePrimitve &FluePrim, const kPoint &SceneLoc, ObjectID &WallId, kPoint &NearLoc);
	
private:
	kVector3D GetVertexNormal(const kVector3D &Vert, const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2);
	void SetFlueVertex(const kPoint &Location, const kPoint &Forward, const kPoint &WallDir, float Width, float Length, float Height);

	//bool IsRotated(IObject* PoleObj, ObjectID WallId[], const kPoint &Location, kRotation &RotateVal);
	int CreateMeshSection();
	int AddVert(int MeshSection, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightMapUV);
	void AddTri(int MeshSection, int V0, int V1, int V2);
	bool GetWallDirect(ObjectID WallId, kPoint& Start, kPoint& End);
	std::vector<kPoint> GetVertexUV(const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2, const kVector3D &AdjVert3);
	void SetDefUV(float wU, float lU);
	void ModulateNormalVal(kXform &Transform);
private:
	IMeshBuilder* PoleMeshBuilder;
	IMeshObject* PoleMesh;
	std::vector<kVector3D> BorderVect;
	ISurfaceObject* WallSurfObj;
};