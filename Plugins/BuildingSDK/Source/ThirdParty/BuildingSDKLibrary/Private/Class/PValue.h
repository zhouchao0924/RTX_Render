
#pragma once

#include "IProperty.h"
#include <unordered_map>

class PValue :public IValue
{
	struct  FLightData 
	{
		union
		{
			bool				bValue;
			int					iValue;
			float				fValue;
			std::vector<int>	*IntArray;
			std::vector<kPoint>	*Vec2Array;
			const char			*strValue0;
			std::string			*strValue1;
			kPoint				*vec2Value;
			kVector3D			*vec3Value;
			kVector4D			*vec4Value;
			kRotation			*rotationValue;
			kBox3D				*box;
			kPlane3D			*plane;
		};
	};
	struct FField
	{
		std::vector<IValue *> VecFields;
		std::unordered_map<std::string, IValue*> MapFields;
	};
public:
	PValue();
	~PValue();
	PValue(int value);
	PValue(float value);
	PValue(const char *str);
	PValue(std::string *str);
	PValue(kVector3D *value);
	PValue(kVector4D *value);
	PValue(kRotation *value);
	PValue(kPoint *value);
	PValue(bool value);
	PValue(kBox3D *InBox);
	PValue(std::vector<int> *value);
	PValue(kPlane3D *value);

	void Retain() override;
	void AddRef() override;
	void Release() override;
	bool IsNil() override { return ValueType == kV_Nil; }
	int IntValue() const override;
	float FloatValue() const override;
	bool BoolValue() const override;
	kPoint Vec2Value() const override;
	kVector3D Vec3Value() const override;
	kVector4D Vec4Value() const override;
	kRotation RotationValue() const override;
	kBox3D	Bounds() const override;
	kPlane3D PlaneValue() const override;
	const char *StrValue() const override;	
	std::vector<int> &IntArrayValue() const override;
	std::vector<kPoint> &Vec2ArrayValue() const override;
	EVarType GetType() const override { return ValueType; }
	int GetNumFields();
	IValue &GetField(int Index);
	const char *GetFieldName(int Index) override;
	IValue &GetField(const char *name);
	bool AddField(const char *name, IValue &Value);
	bool AddField(IValue &Value);
	friend class ValueFactory;
private:
	FLightData				Data;
	void					*WeightData;
	EVarType				ValueType;
	int						_RefCount;
	FField					*_Field;
public:
	static PValue			Nil;
};

