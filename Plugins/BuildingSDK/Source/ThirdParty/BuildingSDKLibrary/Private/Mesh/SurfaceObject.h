
#pragma once

#include <set>
#include "Math/kVector2D.h"
#include "Class/Object.h"
#include "IMeshObject.h"

typedef __int64	ModelSectionID;

enum EMaterialType
{
	EMaterialRef,
	EMaterialUE4,
	EMaterialNone,
};

struct FTexSlotInfo
{
	FTexSlotInfo() { iSlot = -1; iTex = -1; }
	FTexSlotInfo(int InSlot, int InTex)
		: iSlot(InSlot)
		, iTex(InTex)
	{
	}
	int				iSlot;
	int				iTex;
	ETexSlot		slotType;
	std::string		TexParamName;
};

struct FScalarSlotInfo
{
	FScalarSlotInfo() { iSlot = -1; FloatValue = 0; }
	int				iSlot;
	float			 FloatValue;
	std::string		 ParamName;
};

struct FVectorSlotInfo
{
	FVectorSlotInfo() { iSlot = -1; LinearValue = kLinearColor(); }
	int				iSlot;
	kLinearColor			LinearValue;
	std::string		ParamName;
};

class FSurfaceParameter
{
public:
	std::string	ParamName;
	virtual IValue *GetValue() { return nullptr; }
	virtual void Serialize(ISerialize &Ar, int Ver);
};

class FSurfaceParameterFloat : public FSurfaceParameter
{
public:
	float Value;
	IValue *GetValue() override;
	void Serialize(ISerialize &Ar, int Ver) override;
};

class FSurfaceParameterVec2 : public FSurfaceParameter
{
public:
	kPoint Value;
	IValue *GetValue() override;
	void Serialize(ISerialize &Ar, int Ver) override;
};

class FSurfaceParameterVec3 : public FSurfaceParameter
{
public:
	kVector3D Value;
	IValue *GetValue() override;
	void Serialize(ISerialize &Ar, int Ver) override;
};

class FSurfaceParameterVector : public FSurfaceParameter
{
public:
	kVector4D Value;
	IValue *GetValue() override;
	void Serialize(ISerialize &Ar, int Ver) override;
};

class FSurfaceParameterTexture : public FSurfaceParameter
{
public:
	int Value;
	IValue *GetValue() override;
	void Serialize(ISerialize &Ar, int Ver) override;
};

class SurfaceObject :public ISurfaceObject
{
public:
	const char *GetUri() override;
	int GetType() override { return Type; }
	void Serialize(ISerialize &Ar, int Ver);
	void SetSource(int type, const char *Uri) override;

	void SetScalar(const char *name, float Value) override;
	void SetVector2D(const char *name, const kPoint &Value) override;
	void SetVector3D(const char *name, const kVector3D &Value) override;
	void SetVector(const char *name, const kVector4D &Value) override;

	float GetScalar(const char *name) override;
	kPoint GetVector2D(const char *name) override;
	kVector3D GetVector3D(const char *name) override;
	kVector4D GetVector(const char *name) override;

	int   GetTexture(ETexSlot slot) override;
	IValue *FindParamValue(const char *name) override;
protected:
	int										Type;
	std::string								Uri;
	std::vector<FSurfaceParameterFloat>		OverrideFloatParameters;
	std::vector<FSurfaceParameterVec2>		OverrideVec2Parameters;
	std::vector<FSurfaceParameterVec3>		OverrideVec3Parameters;
	std::vector<FSurfaceParameterVector>	OverrideVectorParameters;
	std::vector<FSurfaceParameterTexture>	OverrideTextureParameters;
};

