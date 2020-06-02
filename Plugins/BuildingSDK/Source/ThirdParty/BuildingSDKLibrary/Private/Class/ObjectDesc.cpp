
#include "Property.h"
#include "ObjectDesc.h"

ObjectDesc::ObjectDesc()
	: SuperClassDesc(nullptr)
	, bConstructProperty(false)
{
}

void ObjectDesc::AddProperty(const char *InName, IProperty *Prop, bool bReadOnly)
{
	std::string name = InName;

	if (MapProperties.find(name) == MapProperties.end())
	{
		Property *lProp = (Property *)Prop;
		lProp->SetName(InName);
		lProp->bReadOnly = bReadOnly;
		lProp->Desc = this;
		int index = (int)Properties.size();
		Properties.push_back(lProp);
		MapProperties[name] = index;
	}
	else
	{
		delete Prop;
	}
}

IProperty *ObjectDesc::GetProperty(const char *Propname)
{
	std::string name = Propname;
	std::unordered_map<std::string, int>::iterator it = MapProperties.find(name);
	if (it != MapProperties.end())
	{
		return Properties[it->second];
	}
	return nullptr;
}

int ObjectDesc::GetNumberOfProperty()
{
	return (int)Properties.size();
}

IProperty *ObjectDesc::GetProperty(int index)
{
	if (index >= 0 && index < (int)Properties.size())
	{
		return Properties[index];
	}
	return nullptr;
}

IObjectDesc *ObjectDesc::GetSuperDesc()
{
	return SuperClassDesc;
}




