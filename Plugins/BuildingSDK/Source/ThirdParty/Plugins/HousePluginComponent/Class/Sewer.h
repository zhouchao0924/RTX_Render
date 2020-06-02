#pragma once
#include"IBuildingPlugin.h"
#define SewerType PLUGIN_SECTION_0+3
class Sewer:public IPluginObject
{
public:
	int GetType() { return SewerType; }
	unsigned int GetVersion() { return 0; }
	IValue* GetFunctionProperty(const char* name)override;
	bool SetFunctionProperty(const char* name, const IValue* Value)override;
	void Serialize(ISerialize &Ar, unsigned int ver)override;

	void SetDiam(float dval);
	float GetDiam();

	void SetHeght(float height);
	float GetHeight();

	void SetSize(kPoint size);
	kPoint GetSize();

	void SetDefaultLoc(kVector3D defLoc);
	kVector3D GetDefaultLoc();

	void SetLocation(kPoint Loc);
	kPoint GetLocation();

private:
	float Diam;
	float Height;
	kPoint Location;
	kPoint DefaultSize;
	kVector3D DefaultLoc;
};

class SewerDesc:public IPluginObjectDesc
{
public:
	bool IsResource() { return false; }
	const char *GetObjectClassName() { return "Sewer"; }
	int GetObjectType() { return SewerType; }
	IPluginObject *CreateObject() override;
	void DestroyObject(IPluginObject *pObj) override;
};