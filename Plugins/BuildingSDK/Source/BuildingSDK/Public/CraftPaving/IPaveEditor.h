#pragma once

#include "IClass.h"

class IPaveEditor
{
public:
	virtual ObjectID PaveArea(ObjectID AreaID, IObject* PatternObj) = 0;
	virtual ObjectID PavePolygon(kArray<kPoint> PolygonPoints, kArray<kArray<kPoint>>HolePolyPoints, IObject* PatternObj) = 0;
	virtual void DeletePave(ObjectID ObjId) = 0;
	virtual void GetAreaPolygonInfo(ObjectID AreaID, std::vector<kPoint> FillPolygon, std::vector<kArray<kPoint>> HolePolygon) = 0;
	virtual std::vector<kVector3D> GetHolePolygon(ObjectID HoleID, kVector3D Forward, kVector3D Right) = 0;
	virtual bool UpdatePave(ObjectID ObjID, const kPoint &Loc) = 0;

};