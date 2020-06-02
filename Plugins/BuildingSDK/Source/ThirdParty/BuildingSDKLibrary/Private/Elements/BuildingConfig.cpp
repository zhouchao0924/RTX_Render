
#include "BuildingConfig.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(BuildingConfig, BuildingObject)
	ADD_PROP(WallMaterial, StdStringProperty)
	ADD_PROP(WallMaterialType, IntProperty)
	ADD_PROP(FloorMaterial, StdStringProperty)
	ADD_PROP(FloorMaterialType, IntProperty)
	ADD_PROP(CeilMaterial, StdStringProperty)
	ADD_PROP(CeilMaterialType, IntProperty)
	ADD_PROP(bCeilVisible, BoolProperty)
	ADD_PROP(Tolerance, FloatProperty)
	ADD_PROP(SkirtingCeil, StdStringProperty)
	ADD_PROP(SkirtingCeilExt, Vec3DProperty)
	ADD_PROP(SkirtingFloor, StdStringProperty)
	ADD_PROP(SkirtingFloorExt, Vec3DProperty)
	ADD_PROP(DefaultDoor, StdStringProperty)
	ADD_PROP(DefaultDoorFrame, StdStringProperty)
	ADD_PROP(DefaultWindow, StdStringProperty)
END_CLASS()

BuildingConfig::BuildingConfig()
	:WallMaterialType(0)
	,FloorMaterialType(0)
	,CeilMaterialType(0)
	,bCeilVisible(true)
	,Tolerance(0.05f)
{
}

void BuildingConfig::Serialize(ISerialize &Ar)
{
	BuildingObject::Serialize(Ar);
}


