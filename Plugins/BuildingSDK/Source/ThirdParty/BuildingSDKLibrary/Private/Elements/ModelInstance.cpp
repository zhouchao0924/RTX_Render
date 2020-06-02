
#include "ISuite.h"
#include "Anchor.h"
#include "Class/Property.h"
#include "ModelInstance.h"

BEGIN_DERIVED_CLASS(ModelInstance, BuildingObject)
	ADD_PROP(ResID, StdStringProperty)
	ADD_PROP(Type, IntProperty)
	ADD_PROP(AnchorID, IntProperty)
	ADD_PROP(Location, Vec3DProperty)
	ADD_PROP(Scale, Vec3DProperty)
	ADD_PROP(Forward, Vec3DProperty)
	ADD_PROP(AlignType, IntProperty)
	ADD_PROP(ClipBase0, Vec3DProperty)
	ADD_PROP(ClipBase1, Vec3DProperty)
	ADD_PROP(ClipNormal0, Vec3DProperty)
	ADD_PROP(ClipNormal1, Vec3DProperty)
END_CLASS()

ModelInstance::ModelInstance()
	: AnchorID(INVALID_OBJID)
	, Type(-1)
{
}

void ModelInstance::Serialize(ISerialize &Ar)
{
	BuildingObject::Serialize(Ar);

	Ar << Bounds;
}

void ModelInstance::OnDestroy()
{
	Anchor *pAnchor = SUITE_GET_BUILDING_OBJ(AnchorID, Anchor);
	if (pAnchor)
	{
		pAnchor->UnLink(this);
	}
	
	BuildingObject::OnDestroy();
}

