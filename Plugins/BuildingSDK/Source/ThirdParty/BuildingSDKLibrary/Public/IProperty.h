
#pragma once

#include <string>
#include <vector>
#include <functional>
#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "Math/kBox.h"
#include "Math/kPlane.h"
#include "Math/kColor.h"
#include "Math/kArray.h"
#include "Math/kString.h"

enum EVarType
{
	kV_Nil,
	kV_Float,
	kV_Int,
	kV_Bool,
	kV_Color,
	kV_RawString,
	kV_WRawString,
	kV_StdString,
	kV_Vec4D,
	kV_Vec3D,
	kV_Vec2D,
	kV_Rotator,
	kV_Bounds,
	kV_IntArray,
	kV_Vec2DArray,
	kV_Vec3DArray,
	kV_Plane,
};

class IValue
{
public:
	virtual ~IValue() {}
	virtual void Retain() = 0;
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual bool IsNil() = 0;
	virtual EVarType GetType() const = 0;

	virtual int IntValue() const = 0;
	virtual float FloatValue() const = 0;
	virtual bool BoolValue() const = 0;
	virtual kPoint Vec2Value() const = 0;
	virtual kVector3D Vec3Value() const = 0;
	virtual kVector4D Vec4Value() const = 0;
	virtual kRotation RotationValue() const = 0;
	virtual kBox3D	Bounds() const = 0;
	virtual kPlane3D PlaneValue() const = 0;
	virtual kColor ColorValue() const = 0;
	virtual kArray<int> IntArrayValue() const = 0;
	virtual kArray<kPoint> Vec2ArrayValue() const = 0;
	virtual kArray<kVector3D> Vec3ArrayValue() const = 0;
	virtual const char *StrValue() const = 0;
	virtual const wchar_t *WStrValue() const = 0;
	virtual int GetArrayCount() = 0;
	virtual IValue &GetField(int Index) = 0;
	virtual IValue &GetField(const char *name) const = 0;
	virtual void GetPairs(kArray<const char*> &key, kArray<IValue *> &value) = 0;
	virtual bool AddField(const char *name, IValue &Value) = 0;
	virtual bool AddField(IValue &Value) = 0;
	virtual const char *ToString(size_t *szData = nullptr) = 0;
	virtual IValue &operator[] (int index) = 0;
	virtual IValue &operator[] (const char *name) = 0;

	virtual bool AddInt(int Value) = 0;
	virtual bool AddInt(const char *name, int Value) = 0;
	virtual bool AddBool(bool Value) = 0;
	virtual bool AddBool(const char *name, bool Value) = 0;
	virtual bool AddString(const CharPtr &Value) = 0;
	virtual bool AddString(const char *name, const CharPtr &Value) = 0;
	virtual bool AddFloat(float Value) = 0;
	virtual bool AddFloat(const char *name, float Value) = 0;
	virtual bool AddVector2D(kPoint &Value) = 0;
	virtual bool AddVector2D(const char *name, kPoint &Value) = 0;
	virtual bool AddVector3D(kVector3D &Value) = 0;
	virtual bool AddVector3D(const char *name, kVector3D &Value) = 0;
	virtual bool AddVector4D(kVector4D &Value) = 0;
	virtual bool AddVector4D(const char *name, kVector4D &Value) = 0;
	virtual bool AddIntArray(const kArray<int> &Value) = 0;
	virtual bool AddIntArray(const char *name, const kArray<int> &Value) = 0;
	virtual bool AddVector2DArray(const kArray<kPoint> &Value) = 0;
	virtual bool AddVector2DArray(const char *name, const kArray<kPoint> &Value) = 0;
	virtual bool AddVector3DArray(const kArray<kVector3D> &Value) = 0;
	virtual bool AddVector3DArray(const char *name, const kArray<kVector3D> &Value) = 0;
};

class IValueFactory
{
public:
	virtual ~IValueFactory() {}
	virtual IValue & Create() = 0;
	virtual IValue & Create(int Value) = 0;
	virtual IValue & Create(float Value) = 0;
	virtual IValue & Create(bool Value) = 0;
	virtual IValue & Create(kColor Value) = 0;
	virtual IValue & Create(const char *Value, bool bLight = false) = 0;
	virtual IValue & Create(const wchar_t *Value, bool bLight = false) = 0;
	virtual IValue & Create(kVector3D *Value, bool bLight = true) = 0;
	virtual IValue & Create(kVector4D *Value, bool bLight = true) = 0;
	virtual IValue & Create(kRotation *Value, bool bLight = true) = 0;
	virtual IValue & Create(kPoint *Value, bool bLight = true) = 0;
	virtual IValue & Create(kBox3D *Value, bool bLight = true) = 0;
	virtual IValue & Create(const kArray<int> &Value, bool bLight = true) = 0;
	virtual IValue & Create(const kArray<kPoint> &Value, bool bLight = true) = 0;
	virtual IValue & Create(const kArray<kVector3D> &Value, bool bLight = true) = 0;
	virtual IValue & Create(kPlane3D *Value, bool bLight = true) = 0;
	virtual IValue & FromJson(const char *Json) = 0;
	virtual void AutoRelease() = 0;
	virtual void DestroyValue(IValue *Value) = 0;
};

class IProperty 
{
public:
	virtual ~IProperty() {}
	virtual EVarType GetValueType() = 0;
	virtual IValue *GetValue(void *v) = 0;
	virtual void SetValue(void *v, const IValue *value) = 0;
	virtual const char *GetName() = 0;
	virtual void SetName(const char *InName) = 0;
	virtual bool IsReadOnly() = 0;
	virtual bool IsTransient() = 0;
	virtual bool IsFunction() = 0;
	virtual class IObjectDesc *GetDesc() = 0;
	virtual int GetOffset() = 0;
};

typedef std::function<void(class IObject *, bool, IValue *&)> PropertyFunctor;

