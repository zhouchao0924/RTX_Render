#pragma once

#include"IClass.h"

class ISewerEditor
{
public:
	virtual ObjectID AddSewer(float Diameter, const kPoint &Loc) = 0;
	virtual bool UpdateSewerAttr(ObjectID ObjID, float Diameter,const kPoint &Loc) = 0;
	virtual bool SetSewerSurface(ObjectID ObjID, ObjectID SectionIndex, const char* MaterialUri, int MaterialType) = 0;
	virtual void DeleteSewer(ObjectID ObjID) = 0;
	/*virtual bool GetTubeBorder(ObjectID ObjID, kVector3D *OutBorder) = 0;
	virtual bool SetTubeHeightVal(ObjectID ObjID, float Height) = 0;*/
};