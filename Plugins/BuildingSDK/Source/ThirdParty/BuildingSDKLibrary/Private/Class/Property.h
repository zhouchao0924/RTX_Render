
#pragma once

#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "Math/kBox.h"
#include "Math/kPlane.h"
#include "IProperty.h"
#include "ObjectDesc.h"

class Property :public IProperty
{
public:
	Property(int InOffset) { Offset = InOffset; bReadOnly = true; Desc = nullptr;}
	EVarType GetValueType() override{ return kV_Nil; }
	IValue *GetValue(void *v) override { return nullptr; }
	void SetValue(void *v, const IValue *value) override {}
	const char *GetName() override{ return PropName.c_str(); }
	void SetName(const char *InName) override;
	bool IsReadOnly() override { return bReadOnly; }
	class IObjectDesc *GetDesc() override;
	int GetOffset() override { return Offset; }
	friend class ObjectDesc;
protected:
	int			Offset;
	std::string PropName;
	bool		bReadOnly;
	ObjectDesc	*Desc;
};

class IntProperty :public Property
{
public:
	IntProperty(int InOffset);
	EVarType GetValueType() override { return kV_Int; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};

class IntArrayProperty :public Property
{
public:
	IntArrayProperty(int InOffset);
	EVarType GetValueType() override { return kV_IntArray; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};

class Vec2DArrayProperty :public Property
{
public:
	Vec2DArrayProperty(int InOffset);
	EVarType GetValueType() override { return kV_Vec2DArray; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};

class BoolProperty :public Property
{
public:
	BoolProperty(int InOffset);
	EVarType GetValueType() override { return kV_Bool; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};

class FloatProperty :public Property
{
public:
	FloatProperty(int InOffset);
	EVarType GetValueType() override { return kV_Float; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};

class RawStringProperty :public Property
{
public:
	RawStringProperty(int InOffset);
	EVarType GetValueType() override { return kV_RawString; }
	IValue * GetValue(void *v) override;
};

class StdStringProperty :public Property
{
public:
	StdStringProperty(int InOffset);
	EVarType GetValueType() override { return kV_StdString; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};

class Vec3DProperty :public Property
{
public:
	Vec3DProperty(int InOffset);
	EVarType GetValueType() override { return kV_Vec3D; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};
	
class RotatorProperty :public Property
{
public:
	RotatorProperty(int InOffset);
	EVarType GetValueType() override { return kV_Rotator; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};

class Vec2DProperty :public Property
{
public:
	Vec2DProperty(int InOffset);
	EVarType GetValueType() override { return kV_Vec2D; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};

class BoxProperty :public Property
{
public:
	BoxProperty(int InOffset);
	EVarType GetValueType() override { return kV_Bounds; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};

class PlaneProperty :public Property
{
public:
	PlaneProperty(int InOffset);
	EVarType GetValueType() override { return kV_Plane; }
	IValue * GetValue(void *v) override;
	void SetValue(void *v, const IValue *value) override;
};




