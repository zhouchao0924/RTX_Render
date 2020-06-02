
#pragma once

#include "kLine.h"
#include "Corner.h"
#include "Primitive.h"

class PlanePrimitive :public  Primitive
{
	DEFIN_CLASS()
public:
	PlanePrimitive();
	void Serialize(ISerialize &Ar);
	int  GetMaterial(int ChannelID);
	void SetMaterial(int InMaterialID, int ChannelID /*= 0*/);
	void Build() override;
	EObjectType GetType() { return EPlanePrimitive; }
	virtual int GetPolygons(kPoint *&pPolygon, float *&pHeights, float &zOffset) { return 0; }
public:
	ObjectID			SurfaceID;
	float				ZPos;
	float				Thickness;
	kBox3D				Bounds;
};

class FloorPlane :public  PlanePrimitive
{
	DEFIN_CLASS()
public:
	void Serialize(ISerialize &Ar);
	EObjectType GetType() { return EFloorPlane; }
	int  GetPolygons(kPoint *&pPolygon, float *&pHeights, float &zOffset) override;
public:
	ObjectID RoomID;
};

class CeilPlane :public PlanePrimitive
{
	DEFIN_CLASS()
public:
	void Serialize(ISerialize &Ar);
	EObjectType GetType() { return ECeilPlane; }
	int  GetPolygons(kPoint *&pPolygon, float *&pHeights, float &zOffset) override;
public:
	ObjectID RoomID;
};


