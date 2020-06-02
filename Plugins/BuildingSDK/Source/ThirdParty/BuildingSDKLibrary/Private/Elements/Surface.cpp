
#include "Surface.h"
#include "SuiteImpl.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(Surface, BuildingObject)
	ADD_PROP(Material, StdStringProperty)
	ADD_PROP(MaterialType, IntProperty)
END_CLASS()

Surface::Surface()
	:MaterialType(0)
{
}

ModelSectionID PackID(ObjectID PrimID, int SubModelIndex)
{
	ModelSectionID ModelID = PrimID;
	return (ModelID << 32) | SubModelIndex;
}

void UnpackID(ModelSectionID ModelID, ObjectID &PrimID, int &SubModelIndex)
{
	PrimID = ModelID >> 32;
	SubModelIndex = ModelID & 0x00000000ffffffff;
}

void Surface::Serialize(ISerialize &Ar)
{
	BuildingObject::Serialize(Ar);

	if (Ar.IsSaving())
	{
		int Num = (int)RefModels.size();
		Ar << Num;
		for (std::set<ModelSectionID>::iterator it = RefModels.begin(); it != RefModels.end(); ++it)
		{
			ModelSectionID Id = *it;
			Ar << Id;
		}
	}
	else if (Ar.IsLoading())
	{
		int Num = 0;
		Ar << Num;
		for (int i = 0; i < Num; ++i)
		{
			ModelSectionID Id = 0;
			Ar << Id;
			RefModels.insert(Id);
		}
	}
}

void Surface::Link(ObjectID PrimID, int ModelIndex)
{
	ModelSectionID ModelID = PackID(PrimID, ModelIndex);
	std::set<ModelSectionID>::iterator it = RefModels.find(ModelID);
	if(it == RefModels.end())
	{
		RefModels.insert(ModelID);
	}
}

void Surface::UnLink(ObjectID PrimID, int ModelIndex)
{
	ModelSectionID ModelID = PackID(PrimID, ModelIndex);
	std::set<ModelSectionID>::iterator it = RefModels.find(ModelID);
	if (it != RefModels.end())
	{
		RefModels.erase(it);
	}
}

int Surface::GetRefCount()
{
	return (int)RefModels.size();
}

bool Surface::SetFunctionProperty(const std::string &name, const IValue *Value)
{
	if (Value)
	{
		EVarType Type = Value->GetType();
		switch (Type)
		{
			case kV_Float:
			{
				Surf.SetScalar(name.c_str(), Value->FloatValue());
				NotifySurfaceValueChanged();
				return true;
			}
			case kV_Vec2D:
			{
				Surf.SetVector2D(name.c_str(), Value->Vec2Value());
				NotifySurfaceValueChanged();
				return true;
			}
			case kV_Vec3D:
			{
				Surf.SetVector3D(name.c_str(), Value->Vec3Value());
				NotifySurfaceValueChanged();
				return true;
			}
			case kV_Vec4D:
			{
				Surf.SetVector(name.c_str(), Value->Vec4Value());
				NotifySurfaceValueChanged();
				return true;
			}
			default: break;
		}
	}
	return false;
}

IValue *Surface::GetFunctionProperty(const std::string &name)
{
	return Surf.FindParamValue(name.c_str());
}

void Surface::NotifySurfaceValueChanged()
{
	SuiteImpl *Suite = (SuiteImpl *)_Suite;
	if (Suite)
	{
		ObjectID PrimID = INVALID_OBJID;
		int SubSection = -1;

		for (std::set<ModelSectionID>::iterator it = RefModels.begin(); it != RefModels.end(); ++it)
		{
			ModelSectionID ID = *it;
			UnpackID(ID, PrimID, SubSection);
			Suite->NotifySurfaceValueChanged(PrimID, SubSection);
		}
	}
}

