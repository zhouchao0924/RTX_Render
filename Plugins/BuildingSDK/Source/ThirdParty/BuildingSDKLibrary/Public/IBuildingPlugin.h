
#pragma once

#include "IBuildingResourceMgr.h"

class IBuildingSDK;

class IPluginObject
{
public:
	virtual unsigned int GetVersion() { return 0; }
	virtual void Serialize(ISerialize &Ar, unsigned int Ver) = 0;
public:
	IPluginObject() { Proxy = nullptr; }
	virtual ~IPluginObject() {}
	virtual int GetType() = 0;
	virtual IValue *GetFunctionProperty(const char *name) { return nullptr; }
	virtual bool SetFunctionProperty(const char *name, const IValue *Value) { return false; }
	virtual void OnRegisterProperty(IObjectDesc *Desc) { }
	IObject *GetObject() { return Proxy; }
	void MarkFlags(unsigned int Mark) { Proxy->MarkFlags(Mark); }
	bool HasFlag(unsigned int Mark) { return Proxy->HasFlag(Mark); }
	bool MarkNeedUpdate(EChannelMask Mask = EChannelAll) { Proxy->MarkNeedUpdate(Mask); }
	void NotifyWatcher(EFuncType FuncType, IValue *pValue = nullptr) { Proxy->NotifyWatcher(FuncType, pValue); }
	friend class PluginObject;
protected:
	IObject *Proxy;
};

class IPluginObjectDesc
{
public:
	virtual bool IsResource() = 0;
	virtual const char *GetObjectClassName() = 0;
	virtual IPluginObject *CreateObject() = 0;
	virtual void DestroyObject(IPluginObject *pObj) = 0;
	virtual int  GetObjectType() = 0;
public:
	virtual void OnRegisterProperty(IObjectDesc *Desc) {}
	IObjectDesc *GetObjectDesc() { return Proxy; }
	void AddProperty(const char *name, EVarType VarType, const PropertyFunctor &functor, bool bReadOnly) { Proxy->AddProperty(name, VarType, functor, bReadOnly); }
	friend class PluginDesc;
protected:
	IObjectDesc *Proxy;
};


#define DEFIN_PLUGIN_CLASS(clasName, ID, bResource)						\
class clasName##Desc :public IPluginObjectDesc{							\
	public:																\
	void OnRegisterProperty(IObjectDesc *Desc) override{				\
			clasName inst; inst.OnRegisterProperty(Desc); }				\
	const char *GetObjectClassName() override { return "##clasName"; }	\
	IPluginObject *CreateObject() { return new clasName(); }			\
	void DestroyObject(IPluginObject *pObj) { delete pObj; }			\
	int  GetObjectType() { return ID; }									\
	bool IsResource() {return bResource; }								\
};																		\
static IPluginObjectDesc *GetPluginDesc() {								\
	static clasName##Desc s_pluginDesc;									\
	return &s_pluginDesc;												\
}																		\
static clasName *GetPluginObj(IObject *pObj) {							\
	return pObj? (clasName *)pObj->GetPlugin() : nullptr; }				\
int GetType() override { return GetPluginDesc()->GetObjectType(); }		\
void OnRegisterProperty(IObjectDesc *Desc) override;					\

#define BEGIN_PLUGIN_PROP(clsName)										\
void clsName::OnRegisterProperty(IObjectDesc *Desc){					\

#define ADD_PLUGIN_PROP(name, varType)	\
	Desc->AddProperty(#name, varType, &Property_##name, false);

#define ADD_READONLY_PLUGIN__PROP(name)	\
	Desc->AddProperty(#name, varType, &Property_##name, true);

#define ADD_PLUGIN_PROP_INT(name)					ADD_PLUGIN_PROP(name, kV_Int)
#define ADD_READONLY_PLUGIN_PROP_INT(name)			ADD_READONLY_PLUGIN__PROP(name, kV_Int)

#define ADD_PLUGIN_PROP_FLOAT(name)					ADD_PLUGIN_PROP(name, kV_Float)
#define ADD_READONLY_PLUGIN_PROP_FLOAT(name)		ADD_READONLY_PLUGIN__PROP(name, kV_Float)

#define ADD_PLUGIN_PROP_BOOL(name)					ADD_PLUGIN_PROP(name, kV_Bool)
#define ADD_READONLY_PLUGIN_PROP_BOOL(name)			ADD_READONLY_PLUGIN__PROP(name, kV_Bool)

#define ADD_PLUGIN_PROP_STRING(name)				ADD_PLUGIN_PROP(name, kV_RawString)
#define ADD_READONLY_PLUGIN_PROP_STRING(name)		ADD_READONLY_PLUGIN__PROP(name, kV_RawString)

#define ADD_PLUGIN_PROP_VECTOR2D(name)				ADD_PLUGIN_PROP(name, kV_Vec2D)
#define ADD_READONLY_PLUGIN_PROP_VECTOR2D(name)		ADD_READONLY_PLUGIN__PROP(name, kV_Vec2D)

#define ADD_PLUGIN_PROP_VECTOR2DARRAY(name)				ADD_PLUGIN_PROP(name, kV_Vec2DArray)
#define ADD_READONLY_PLUGIN_PROP_VECTOR2DARRAY(name)	ADD_READONLY_PLUGIN__PROP(name, kV_Vec2DArray)

#define ADD_PLUGIN_PROP_VECTOR3D(name)				ADD_PLUGIN_PROP(name, kV_Vec3D)
#define ADD_READONLY_PLUGIN_PROP_VECTOR3D(name)		ADD_READONLY_PLUGIN__PROP(name, kV_Vec3D)

#define END_PLUING_PROP() }


class IBuildingPlugin
{
public:
	virtual bool Init(IBuildingSDK *SDK) = 0;
	virtual bool Tick(float DeltaTime) = 0;
	virtual void UnInitialize() = 0;
	virtual void *QueryInterface(const char *Name) = 0;
};

#define PLUGIN_SECTION_0		EPluginObject + 1 
#define PLUGIN_SECTION_1		EPluginObject + 100
#define PLUGIN_SECTION_2		EPluginObject + 200
#define PLUGIN_SECTION_3		EPluginObject + 300
#define PLUGIN_SECTION_4		EPluginObject + 400
#define PLUGIN_SECTION_5		EPluginObject + 500

