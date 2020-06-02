#pragma once

#include "IClass.h"

class IPillarEditor
{
public:
	virtual ObjectID AddPillar(bool IsAnch, float Height, float Width, float Length, const kPoint &Loc, const kPoint &Forward) = 0;
	virtual bool UpdatePillar(ObjectID ObjID, const kPoint &Loc, float RotAngle, const kXform &Transform = kXform()) = 0;
	virtual bool UpdatePillarAttr(ObjectID ObjID, float Width, float Length, const kPoint &Loc, float Angle) = 0;
	virtual bool SetPillarSurface(ObjectID ObjID, ObjectID SectionIndex, const char* MaterialUri, int MaterialType) = 0;
	virtual void DeletePillar(ObjectID ObjID) = 0;
	virtual bool GetPillarBorder(ObjectID ObjID, kVector3D *OutBorder) = 0;
	virtual bool SetPillarHeightVal(ObjectID ObjID, float Height) = 0;
};