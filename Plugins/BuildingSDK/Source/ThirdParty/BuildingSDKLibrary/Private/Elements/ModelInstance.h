
#pragma once

#include "kBox.h"
#include "kPlane.h"
#include "BuildingObject.h"

enum EModelAlignType
{
	EAlignOriginal,

	EAlignBottomCenter,
	EAlignTopCenter,
	EAlignLeftCenter,
	EAlignRightCenter,
	EAlignFrontCenter,
	EAlignBackCenter,

	EAlignTopFrontEdgeCenter,
	EAlignTopBackEdgeCenter,
	EAlignTopLeftEdgeCenter,
	EAlignTopRightEdgeCenter,

	EAlignBottomFrontEdgeCenter,
	EAlignBottomBackEdgeCenter,
	EAlignBottomLeftEdgeCenter,
	EAlignBottomRightEdgeCenter,

	EAlignLeftFrontEdgeCenter,
	EAlignRightFrontEdgeCenter,
	EAlignLeftBackEdgeCenter,
	EAlignRightBackEdgeCenter
};

class ModelInstance :public BuildingObject
{
	DEFIN_CLASS()
public:
	ModelInstance();
	EObjectType GetType() { return EModelInstance; }
	void OnDestroy() override;
	void Serialize(ISerialize &Ar);
public:
	ObjectID			AnchorID;
	int					Type;
	std::string			ResID;
	kVector3D			Location;
	kVector3D			Scale;
	kVector3D			Forward;
	kBox3D				Bounds;
	EModelAlignType		AlignType;
	kVector3D			ClipBase0;
	kVector3D			ClipBase1;
	kVector3D			ClipNormal0;
	kVector3D			ClipNormal1;
};



