
#pragma once

#include <vector>
#include "IClass.h"
#include <unordered_map>

class ObjectDesc :public IObjectDesc
{
public:
	ObjectDesc();
	const char *GetObjectClassName() { return ClassName.c_str(); }
	void AddProperty(const char *InName, IProperty *Prop, bool bReadOnly);
	IProperty *GetProperty(const char *Propname);
	int GetNumberOfProperty();
	IProperty *GetProperty(int index);
	IObjectDesc *GetSuperDesc();
public:
	int						ObjectType;
	bool					bConstructProperty;
	ObjectDesc				*SuperClassDesc;
	std::string				ClassName;
	std::vector<class Property *> Properties;
	std::unordered_map<std::string, int> MapProperties;
};


#define  DEFIN_CLASS()																		\
	public:																					\
		static ObjectDesc *GetObjectDesc() { static ObjectDesc ClsDesc; return &ClsDesc; }	\
		static void BuildProperties();

#define ADD_PROP(propname, propcls)				\
	ClsDesc->AddProperty(#propname, new propcls(int((__int64)&inst.##propname - (__int64)&inst)),false);

#define ADD_PROP_READONLY(propname, propcls)				\
	ClsDesc->AddProperty(#propname, new propcls(int((__int64)&inst.##propname - (__int64)&inst)),true);


#define BEGIN_DERIVED_CLASS(clsname,superclsname)					\
void clsname::BuildProperties(){									\
	ObjectDesc *ClsDesc = clsname::GetObjectDesc() ;				\
	if(!ClsDesc->bConstructProperty){								\
		ClsDesc->SuperClassDesc = superclsname::GetObjectDesc();	\
		superclsname::BuildProperties();							\
		ClsDesc->ClassName = #clsname;								\
		clsname		inst;											\
		ClsDesc->ObjectType = inst.GetType();

#define BEGIN_CLASS(clsname)										\
void clsname::BuildProperties(){									\
	ObjectDesc *ClsDesc = clsname::GetObjectDesc() ;				\
	if(!ClsDesc->bConstructProperty){								\
		ClsDesc->ClassName = #clsname;								\
		clsname		inst;											\
		ClsDesc->ObjectType = inst.GetType();

#define END_CLASS()								\
		ClsDesc->bConstructProperty = true;		\
	}											\
}

