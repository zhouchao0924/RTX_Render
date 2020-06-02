#pragma once

#include "IClass.h"

class ITubeEditor
{
public:
	virtual ObjectID AddTube(float Height, float Width, float Length,/* bool IsWidth, */const kPoint &Loc, const kPoint &Forward) = 0;
	virtual bool UpdateTube(ObjectID ObjID, const kPoint &Loc, float RotAngle, const kXform &Transform = kXform()) = 0;
	virtual bool UpdateTubeAttr(ObjectID ObjID, float Width, float Length, const kPoint &Loc, float Angle)=0;
	virtual bool SetTubeSurface(ObjectID ObjID, ObjectID SectionIndex, const char* MaterialUri, int MaterialType) = 0;
	virtual void DeleteTube(ObjectID ObjID) = 0;
	virtual bool GetTubeBorder(ObjectID ObjID, kVector3D *OutBorder) = 0;
	virtual bool SetTubeHeightVal(ObjectID ObjID, float Height) = 0;
};
