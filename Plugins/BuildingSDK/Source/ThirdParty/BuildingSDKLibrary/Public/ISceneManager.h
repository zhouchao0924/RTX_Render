
#pragma once

#include "Math/kVector3D.h"
#include "Math/kPlane.h"
#include "Math/kArray.h"

class IObject;
typedef unsigned int SceneID;
typedef unsigned int PlaneID;
#define INVALID_SCENEID		-1

class ISceneManager
{
public:
	virtual SceneID AddObject(IObject *pObj) = 0;
	virtual SceneID UpdateObject(IObject *Obj) = 0;
	virtual void RemoveObject(SceneID ID) = 0;
	
	virtual void SetEye(const kVector3D &InEyePos, const kVector3D &InEyeDir, float InW, float InH, float InFOV) = 0;

	virtual PlaneID AddClipPlane(const kPlane3D &ClipPlane) = 0;
	virtual void RemoveClipPlane(PlaneID ID) = 0;
	virtual void ClearClipPlane() = 0;

	virtual kArray<IObject *> GetVisibleObjects() = 0;
	virtual void DrawSceneNode(bool bTransient) = 0;
	virtual IObject *HitTest(const kVector3D &Start, const kVector3D &Direction, float RayLen, kVector3D *OutNormal, int *OutSectionIndex) = 0;
};


