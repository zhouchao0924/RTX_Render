
#include "PValue.h"
#include "IProperty.h"
#include "assert.h"

PValue PValue::Nil;

PValue::PValue()
	:ValueType(kV_Nil)
	,WeightData(nullptr)
	,_RefCount(1)
	,_Field(nullptr)
{
}

#define  DEL_WEIGHTDATE(valuetype, type){							\
	if (ValueType == valuetype) {									\
		std::vector<int> *pValue = (std::vector<int> *)WeightData;	\
		delete pValue;												\
		WeightData = nullptr;										\
		break;														\
	}																\
}


PValue::~PValue()
{
	while (WeightData)
	{
		DEL_WEIGHTDATE(kV_IntArray, std::vector<int>);
		DEL_WEIGHTDATE(kV_RawString, std::string);
		DEL_WEIGHTDATE(kV_StdString, std::string);
		DEL_WEIGHTDATE(kV_Vec2D, kPoint);
		DEL_WEIGHTDATE(kV_Vec3D, kVector3D);
		DEL_WEIGHTDATE(kV_Vec4D, kVector4D);
		DEL_WEIGHTDATE(kV_Rotator, kRotation);
		DEL_WEIGHTDATE(kV_Bounds, kBox3D);
		DEL_WEIGHTDATE(kV_Plane, kPlane3D);
	}
}

PValue::PValue(int value) :PValue()
{
	Data.iValue = value;
	ValueType = kV_Int;
}

PValue::PValue(float value) :PValue()
{
	Data.fValue = value;
	ValueType = kV_Float;
}

PValue::PValue(const char *str) :PValue()
{
	Data.strValue0 = str;
	ValueType = kV_RawString;
}

PValue::PValue(std::string *str) :PValue()
{
	Data.strValue1 = str;
	ValueType = kV_StdString;
}

PValue::PValue(kVector3D *value) :PValue()
{
	Data.vec3Value = value;
	ValueType = kV_Vec3D;
}

PValue::PValue(kVector4D *value) :PValue()
{
	Data.vec4Value = value;
	ValueType = kV_Vec4D;
}

PValue::PValue(kRotation *value) :PValue()
{
	Data.rotationValue = value;
	ValueType = kV_Rotator;
}

PValue::PValue(kPoint *value) :PValue()
{
	Data.vec2Value = value;
	ValueType = kV_Vec2D;
}

PValue::PValue(bool value) :PValue()
{
	Data.bValue = value;
	ValueType = kV_Bool;
}

PValue::PValue(kBox3D *InBox) :PValue()
{
	Data.box = InBox;
	ValueType = kV_Bounds;
}

PValue::PValue(std::vector<int> *value) :PValue()
{
	Data.IntArray = value;
	ValueType = kV_IntArray;
}

PValue::PValue(kPlane3D *value) :PValue()
{
	Data.plane = value;
	ValueType = kV_Plane;
}

int PValue::IntValue() const
{
	if (ValueType == kV_Int)
	{
		return Data.iValue;
	}
	return 0;
}

float PValue::FloatValue() const
{
	if (ValueType == kV_Float)
	{
		return Data.fValue;
	}
	return 0;
}

bool PValue::BoolValue() const
{
	if (ValueType == kV_Bool)
	{
		return Data.bValue;
	}
	return false;
}

#define  RET_PVALUE(valuetype, type, mem, defvalue){	\
	if (ValueType == valuetype){						\
		if (WeightData) {								\
			return *((type*)WeightData);				\
		}												\
		else {											\
			return *Data.mem;							\
		}												\
	}													\
	return defvalue;									\
}

static std::vector<int> defArray;
static std::vector<kPoint> defVec2Array;

std::vector<int> & PValue::IntArrayValue() const { RET_PVALUE(kV_IntArray, std::vector<int>, IntArray, defArray) }

std::vector<kPoint> &PValue::Vec2ArrayValue() const { RET_PVALUE(kV_Vec2DArray, std::vector<kPoint>, Vec2Array, defVec2Array) }

kPoint PValue::Vec2Value() const { RET_PVALUE(kV_Vec2D, kPoint, vec2Value, kPoint()) }

kVector3D PValue::Vec3Value() const { RET_PVALUE(kV_Vec3D, kVector3D, vec3Value, kVector3D()) }

kVector4D PValue::Vec4Value() const { RET_PVALUE(kV_Vec4D, kVector4D, vec4Value, kVector4D()) }

kRotation PValue::RotationValue() const { RET_PVALUE(kV_Rotator, kRotation, rotationValue, kRotation()) }

kBox3D PValue::Bounds() const { RET_PVALUE(kV_Bounds, kBox3D, box, kBox3D()) }

kPlane3D PValue::PlaneValue() const { RET_PVALUE(kV_Plane, kPlane3D, plane, kPlane3D()) }

const char *PValue::StrValue() const
{
	if (kV_RawString == ValueType)
	{
		if (WeightData)
		{
			return ((std::string *)WeightData)->c_str();
		}
		return Data.strValue0;
	}
	else if (kV_StdString == ValueType)
	{
		if (WeightData)
		{
			return ((std::string *)WeightData)->c_str();
		}
		return Data.strValue1->c_str();
	}
	return "";
}

void PValue::Retain()
{
	if (ValueType == kV_IntArray)
	{
		std::vector<int> *pData = new std::vector<int>();
		*pData = *Data.IntArray;
		WeightData = pData;
	}
	else if (ValueType == kV_Vec2DArray)
	{
		std::vector<kPoint> *pData = new std::vector<kPoint>();
		*pData = *Data.Vec2Array;
		WeightData = pData;
	}
	else if (ValueType == kV_RawString)
	{
		std::string *pData = new std::string();
		*pData = Data.strValue0;
		WeightData = pData;
	}
	else if (ValueType == kV_StdString)
	{
		std::string *pData = new std::string();
		*pData = *Data.strValue1;
		WeightData = pData;
	}
	else if (ValueType == kV_Vec2D)
	{
		kPoint *pData = new kPoint();
		*pData = *Data.vec2Value;
		WeightData = pData;
	}
	else if (ValueType == kV_Vec3D)
	{
		kVector3D *pData = new kVector3D();
		*pData = *Data.vec3Value;
		WeightData = pData;
	}
	else if (ValueType == kV_Vec4D)
	{
		kVector4D *pData = new kVector4D();
		*pData = *Data.vec4Value;
		WeightData = pData;
	}
	else if (ValueType == kV_Rotator)
	{
		kRotation *pData = new kRotation();
		*pData = *Data.rotationValue;
		WeightData = pData;
	}
	else if (ValueType == kV_Bounds)
	{
		kBox3D *pData = new kBox3D();
		*pData = *Data.box;
		WeightData = pData;
	}
	else if (ValueType == kV_Plane)
	{
		kPlane3D *pData = new kPlane3D();
		*pData = *Data.plane;
		WeightData = pData;
	}
}

void PValue::AddRef()
{
	assert(_RefCount >= 0);
	++_RefCount;
}

void PValue::Release()
{
	assert(_RefCount > 0);
	--_RefCount;
	if (_RefCount <= 0)
	{
		if (_Field)
		{
			for (size_t i = 0; i < _Field->VecFields.size(); ++i)
			{
				IValue *value = _Field->VecFields[i];
				if (value)
				{
					value->Release();
				}
			}
			_Field->VecFields.clear();
			_Field->MapFields.clear();
			delete _Field;
			_Field = nullptr;
		}
		GValueFactory->DestroyValue(this);
	}
}

int PValue::GetNumFields()
{
	if (_Field)
	{
		return (int)_Field->VecFields.size();
	}
	return 0;
}

IValue &PValue::GetField(int Index)
{
	if (_Field && Index >= 0 && Index < (int)_Field->VecFields.size())
	{
		return *_Field->VecFields[Index];
	}
	return Nil;
}

const char *PValue::GetFieldName(int Index)
{
	if (_Field && Index >= 0 && Index < (int)_Field->VecFields.size())
	{
		IValue *pValue = _Field->VecFields[Index];
		for (std::unordered_map<std::string, IValue*>::iterator it = _Field->MapFields.begin(); it != _Field->MapFields.end(); ++it)
		{
			if (it->second == pValue)
			{
				return it->first.c_str();
			}
		}
	}
	return "";
}

IValue &PValue::GetField(const char *name)
{
	IValue *pValue = nullptr;

	if (_Field)
	{
		std::unordered_map<std::string, IValue*>::iterator it = _Field->MapFields.find(name);
		if (it != _Field->MapFields.end())
		{
			pValue = it->second;
		}
	}

	return pValue ? *pValue : Nil;
}

bool PValue::AddField(const char *name, IValue &Value)
{
	if (name)
	{
		if (!_Field)
		{
			_Field = new FField();
		}

		if (_Field)
		{
			std::unordered_map<std::string, IValue*>::iterator it = _Field->MapFields.find(name);
			if (it != _Field->MapFields.end())
			{
				return false;
			}

			_Field->MapFields[name] = &Value;
			_Field->VecFields.push_back(&Value);
			Value.AddRef();

			return true;
		}
	}

	return false;
}

bool PValue::AddField(IValue &Value)
{
	if (!_Field)
	{
		_Field = new FField();
	}

	if (_Field)
	{
		_Field->VecFields.push_back(&Value);
		Value.AddRef();
		return true;
	}
	return false;
}




