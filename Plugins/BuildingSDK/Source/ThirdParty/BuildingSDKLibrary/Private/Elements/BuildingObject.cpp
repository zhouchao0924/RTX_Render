
#include "IProperty.h"
#include "BuildingObject.h"
#include "Class/Property.h"
#include "Class/PValue.h"

BuildingObject::BuildingObject()
	: bNeedUpdate(false)
	, _Suite(nullptr)
{
}

BEGIN_DERIVED_CLASS(BuildingObject, Object)
END_CLASS()

void BuildingObject::Serialize(ISerialize &Ar)
{
	Object::Serialize(Ar);
	SERIALIZE_VEC(Anchors);
}


