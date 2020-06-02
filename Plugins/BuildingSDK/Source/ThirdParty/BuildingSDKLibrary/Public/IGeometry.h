
#pragma once

#include "IProperty.h"
#include "IMeshObject.h"
#include "Math/kVector2D.h"

class IGeometry
{
public:
	virtual ~IGeometry() {}
	virtual void BeginTriangulation(kPoint *Polygon, int nPoints) = 0;
	virtual void AddHole(kPoint *Hole, int nPoints) = 0;
	virtual bool EndTriangulation(kArray<kPoint> &outVertices, kArray<int> &outIndices) = 0;
	virtual bool SimpleTriangulate(kPoint *pPolygon, int nPoints, kArray<kPoint> &outVertices, kArray<int> &outIndices) = 0;

	virtual bool GetClipPath(bool bPathClosed, const kArray<kVector3D> &Path, const kPoint &Offset, const kVector3D &StartPreForward, const kVector3D &EndNextForward, kArray<kVector3D> &OutPaths) = 0;
	virtual bool Stretch(IMeshObject *MeshObj, const kPoint &Offset, bool bShapeClosed, const kPoint &ShapeScale, const kArray<kPoint> &Shape, bool bPathClosed, const kArray<kVector3D> &Path, const kVector3D &StartPreForward = kVector3D(), const kVector3D &EndNextForward = kVector3D()) = 0;
	virtual bool GetMesh2DBoundary(IMeshObject *MeshObj, kArray<kPoint> *&OutBoundarys, unsigned char *&OutCloseFlags, int &OutBoundaryCount, int IgnoreXYZ = 1) = 0;

	//begin arrangement
	virtual void BeginArrangement() = 0;
	virtual kPoint GetPoint(int index) = 0;
	virtual bool AddLine(const kPoint &P0, const kPoint &P1, int *Index0 = nullptr, int *Index1 = nullptr) = 0;
	virtual void EndArrangement(kArray<int> *&out_boundaries, int &out_boundaryCount) = 0;
	virtual void SimpleArrangement(kPoint *pPolygon, int nPoints, kArray<kPoint> &out_boundary) = 0;

	virtual void DecompressImageBC5(unsigned long width, unsigned long height, const unsigned char *compressedBC5, unsigned long *imagePixel, bool bRGBA8 = true) = 0;
};



