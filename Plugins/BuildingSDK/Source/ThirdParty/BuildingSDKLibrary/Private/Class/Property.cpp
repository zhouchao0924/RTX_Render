
#include "Property.h"
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"

IObjectDesc *Property::GetDesc()
{ 
	return Desc; 
}

void Property::SetName(const char *InName)
{
	PropName = InName;
}

IntProperty::IntProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * IntProperty::GetValue(void *v)
{
	if (v)
	{
		int *intV = (int *)(((char *)v + Offset));
		return &GValueFactory->Create(*intV);
	}
	return nullptr;
}

void IntProperty::SetValue(void *v, const IValue *value)
{
	if (value->GetType() == kV_Int)
	{
		int *intV = (int *)(((char *)v + Offset));
		*intV = value->IntValue();
	}
}

IntArrayProperty::IntArrayProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * IntArrayProperty::GetValue(void *v)
{
	if (v)
	{
		std::vector<int> *intArray = (std::vector<int> *)(((char *)v + Offset));
		return &GValueFactory->Create(intArray);
	}
	return nullptr;
}

void IntArrayProperty::SetValue(void *v, const IValue *value)
{
	if (value->GetType() == kV_IntArray)
	{
		std::vector<int> *intArray = (std::vector<int> *)(((char *)v + Offset));
		*intArray = value->IntArrayValue();
	}
}

Vec2DArrayProperty::Vec2DArrayProperty(int InOffset)
	:Property(InOffset)
{
}

IValue *Vec2DArrayProperty::GetValue(void *v)
{
	if (v)
	{
		std::vector<kPoint> *vec2Array = (std::vector<kPoint> *)(((char *)v + Offset));
		return &GValueFactory->Create(vec2Array);
	}
	return nullptr;
}

void Vec2DArrayProperty::SetValue(void *v, const IValue *value)
{
	if (value->GetType() == kV_Vec2DArray)
	{
		std::vector<kPoint> *vec2Array = (std::vector<kPoint> *)(((char *)v + Offset));
		*vec2Array = value->Vec2ArrayValue();
	}
}

BoolProperty::BoolProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * BoolProperty::GetValue(void *v)
{
	if (v)
	{
		bool *bV = (bool *)(((char *)v + Offset));
		return &GValueFactory->Create(*bV);
	}
	return nullptr;
}

void BoolProperty::SetValue(void *v, const IValue *value)
{
	if (value->GetType() == kV_Bool)
	{
		bool *bV = (bool *)(((char *)v + Offset));
		*bV = value->BoolValue();
	}
}

FloatProperty::FloatProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * FloatProperty::GetValue(void *obj)
{
	if (obj)
	{
		float *fV = (float *)(((char *)obj + Offset));
		return &GValueFactory->Create(*fV);
	}
	return nullptr;
}

void FloatProperty::SetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Float)
	{
		float *fV = (float *)(((char *)obj + Offset));
		*fV = value->FloatValue();
	}
}

RawStringProperty::RawStringProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * RawStringProperty::GetValue(void *obj)
{
	if (obj)
	{
		const char *strV = (const char *)(((char *)obj + Offset));
		return &GValueFactory->Create(strV);
	}
	return nullptr;
}

StdStringProperty::StdStringProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * StdStringProperty::GetValue(void *obj)
{
	if (obj)
	{
		std::string *strV = (std::string *)(((char *)obj + Offset));
		return &GValueFactory->Create(strV);
	}
	return nullptr;
}

void StdStringProperty::SetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_StdString || value->GetType() ==kV_RawString)
	{
		std::string *strV = (std::string *)(((char *)obj + Offset));
		*strV = value->StrValue();
	}
}

Vec3DProperty::Vec3DProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * Vec3DProperty::GetValue(void *obj)
{
	if (obj)
	{
		kVector3D *vec3D = (kVector3D *)(((char *)obj + Offset));
		return &GValueFactory->Create(vec3D);
	}
	return nullptr;
}

void Vec3DProperty::SetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Vec3D)
	{
		kVector3D *vec3D = (kVector3D *)(((char *)obj + Offset));
		*vec3D = value->Vec3Value();
	}
}

RotatorProperty::RotatorProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * RotatorProperty::GetValue(void *obj)
{
	if (obj)
	{
		kRotation *vec3D = (kRotation *)(((char *)obj + Offset));
		return &GValueFactory->Create(vec3D);
	}
	return nullptr;
}

void RotatorProperty::SetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Rotator)
	{
		kRotation *rotation = (kRotation *)(((char *)obj + Offset));
		*rotation = value->RotationValue();
	}
}

Vec2DProperty::Vec2DProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * Vec2DProperty::GetValue(void *obj)
{
	if (obj)
	{
		kPoint *vec2D = (kPoint *)(((char *)obj + Offset));
		return &GValueFactory->Create(vec2D);
	}
	return nullptr;
}

void Vec2DProperty::SetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Vec2D)
	{
		kPoint *vec2D = (kPoint *)(((char *)obj + Offset));
		*vec2D = value->Vec2Value();
	}
}

BoxProperty::BoxProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * BoxProperty::GetValue(void *obj)
{
	if (obj)
	{
		kBox3D *box = (kBox3D *)(((char *)obj + Offset));
		return &GValueFactory->Create(box);
	}
	return nullptr;
}

void BoxProperty::SetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Bounds)
	{
		kBox3D *box = (kBox3D *)(((char *)obj + Offset));
		*box = value->Bounds();
	}
}

PlaneProperty::PlaneProperty(int InOffset)
	:Property(InOffset)
{
}

IValue * PlaneProperty::GetValue(void *obj)
{
	if (obj)
	{
		kPlane3D *plane = (kPlane3D *)(((char *)obj + Offset));
		return &GValueFactory->Create(plane);
	}
	return nullptr;
}

void PlaneProperty::SetValue(void *obj, const IValue *value)
{
	if (value->GetType() == kV_Plane)
	{
		kPlane3D *plane = (kPlane3D *)(((char *)obj + Offset));
		*plane = value->PlaneValue();
	}
}


