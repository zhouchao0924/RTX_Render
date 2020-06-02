
#pragma once

#include "IObjectFactory.h"

class IStretchPath
{
public:
	struct FSlot
	{
		int TypeID;
		const char *MX;
		kPoint Size;
	};
	virtual ObjectID AddPath(IObjectFactory *Factory, int PatternID, const kArray<FSlot> &Slots , kArray<kVector3D> &Path) = 0;
	virtual void DeletePath(IObjectFactory *Factory, ObjectID PathID) = 0;
	virtual void SetSize(IObjectFactory *Factory, ObjectID PathID, int TypeID, const kPoint &Size, int PointIndex = -1) = 0;
	virtual void SetShape(IObjectFactory *Factory, ObjectID PathID, int TypeID, const char *MX, int PointIndex = -1) = 0;
	virtual void SetSurface(IObjectFactory *Factory, ObjectID PathID, int TypeID, const char *SurfaceUri,int PointIndex = -1) = 0;
	virtual void SetVisible(IObjectFactory *Factory, ObjectID PathID, int TypeID, bool bVisible, int PointIndex = -1) = 0;
	virtual kPoint GetSize(IObjectFactory *Factory, ObjectID PathID, int TypeID, int PointIndex) = 0;
	virtual bool GetVisible(IObjectFactory *Factory, ObjectID PathID, int TypeID, int PointIndex) = 0;
	virtual const char *GetShape(IObjectFactory *Factory, ObjectID PathID, int TypeID, int PointIndex) = 0;
	virtual int  GetPointIndex(IObjectFactory *Factory, ObjectID PathID, ObjectID StretchMeshObjID, int SubSectionIndex) = 0;
	virtual bool GetMeshSectionByPointIndex(IObjectFactory *Factory, ObjectID PathID, int PointIndex, int TypeID, ObjectID &OutStretchMeshID, int &OutSubSectionIndex) = 0;
	virtual bool GetPaths(IObjectFactory *Factory, ObjectID PathID, int TypeID, kArray<kVector3D> &OutPath) = 0;
	virtual float GetCeilThickness(IObjectFactory *Factory, ObjectID PathID) = 0;
	virtual void  SetCeilThickness(IObjectFactory *Factory, ObjectID PathID, float Thickness) = 0;
	virtual void  SetCeilShape(IObjectFactory *Factory, ObjectID PathID, const char *ResID) = 0;
	virtual const char *GetCeilShape(IObjectFactory *Factory, ObjectID PathID) = 0;
};


