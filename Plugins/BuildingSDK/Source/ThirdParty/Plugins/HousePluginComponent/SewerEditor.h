#pragma once
#include "Public/ISewerEditor.h"
class SewerEditor:public ISewerEditor
{
public:
	ObjectID AddSewer(float Diameter, const kPoint &Loc) override;
	bool UpdateSewerAttr(ObjectID ObjID, float Diameter, const kPoint &Loc) override;
	bool SetSewerSurface(ObjectID ObjID, ObjectID SectionIndex, const char* MaterialUri, int MaterialType) override;
	void DeleteSewer(ObjectID ObjID) override;
protected:
	void SetSewerAttribute(IObject* Obj, float Diameter, float Height, const kPoint &Loc);

private:
	void SetSewerVertex(IObject* Obj, const kPoint &Location,float Diam, float Height);
	void SetCylinderData(const kPoint &Location, float Diam, float Height);
	int AddVert(int MeshSection, const kVector3D &Vert, const kVector3D &Normal, const kVector3D &Tan, const kPoint &UV, const kPoint &LightMapUV);
	void AddTri(int MeshSection, int V0, int V1, int V2);
	std::vector<kPoint> GetVertexUV(const kVector3D &AdjVert0, const kVector3D &AdjVert1, const kVector3D &AdjVert2, const kVector3D &AdjVert3);
	void SetDefUV(float wU, float lU);
private:
	IMeshBuilder* SewerMeshBuilder;
	IMeshObject* SewerMesh;
};

