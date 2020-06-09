
#pragma once

#include "IClass.h"

class IPoleEditor
{
public:
	virtual ObjectID AddPole(float WallHeight, float Width, float Length, /*bool IsWidth,*/ const kPoint &Loc, const kPoint &Forward) = 0;
	virtual bool UpdatePole(ObjectID ObjID, float Width, float Length, const kPoint &Loc, float Angle) = 0;
	virtual bool UpdatePole(ObjectID ObjID, const kPoint &Loc, float RotAngle,/* ObjectID WallId,*/ const kXform &Transform = kXform())=0;	
	virtual void DeletePole(ObjectID ObjID)=0;
	virtual bool SetPoleSurface(ObjectID ObjID, ObjectID SectionIndex, const char* MaterialUri,int MaterialType)=0;
	virtual bool GetPoleBorder(ObjectID ObjID, kVector3D *OutBorder) = 0;
	virtual bool SetPoleHeightVal(ObjectID ObjID, float Height) = 0;
};

