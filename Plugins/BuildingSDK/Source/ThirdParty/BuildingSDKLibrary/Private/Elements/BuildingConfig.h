
#pragma once

#include "BuildingObject.h"

class BuildingConfig :public BuildingObject
{
	DEFIN_CLASS()
public:
	BuildingConfig();
	EObjectType GetType() override { return EBuildingConfig; }
	void Serialize(ISerialize &Ar);
public:
	std::string		WallMaterial;
	int				WallMaterialType;
	std::string		FloorMaterial;
	int				FloorMaterialType;
	std::string		CeilMaterial;
	int				CeilMaterialType;
	bool			bCeilVisible;
	float			Tolerance;
	std::string		SkirtingCeil;
	kVector3D		SkirtingCeilExt;
	std::string		SkirtingFloor;
	kVector3D		SkirtingFloorExt;
	std::string		DefaultDoor;
	std::string		DefaultDoorFrame;
	std::string		DefaultWindow;
};


