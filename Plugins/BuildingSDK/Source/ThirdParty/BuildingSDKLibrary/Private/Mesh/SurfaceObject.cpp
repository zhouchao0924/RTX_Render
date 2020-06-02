
#include "SurfaceObject.h"
#include "Class/Property.h"
#include "IProperty.h"
#include "MXFile/MXFile.h"

extern std::string LoadStr(ISerialize &Ar);

void FSurfaceParameter::Serialize(ISerialize &Ar, int Ver)
{
	if (Ar.IsSaving())
	{
		Ar << ParamName;
	}
	else if (Ar.IsLoading())
	{
		ParamName = LoadStr(Ar);
	}
}

IValue *FSurfaceParameterFloat::GetValue()
{
	return GValueFactory? &GValueFactory->Create(Value) : nullptr;
}

void FSurfaceParameterFloat::Serialize(ISerialize &Ar, int Ver)
{
	FSurfaceParameter::Serialize(Ar, Ver);
	Ar << Value;
}

IValue *FSurfaceParameterVec2::GetValue()
{
	return GValueFactory ? &GValueFactory->Create(&Value) : nullptr;
}

void FSurfaceParameterVec2::Serialize(ISerialize &Ar, int Ver)
{
	FSurfaceParameter::Serialize(Ar, Ver);
	Ar << Value;
}

IValue *FSurfaceParameterVec3::GetValue()
{
	return GValueFactory ? &GValueFactory->Create(&Value) : nullptr;
}

void FSurfaceParameterVec3::Serialize(ISerialize &Ar, int Ver)
{
	FSurfaceParameter::Serialize(Ar, Ver);
	Ar << Value;
}

IValue *FSurfaceParameterVector::GetValue()
{
	return GValueFactory ? &GValueFactory->Create(&Value) : nullptr;
}

void FSurfaceParameterVector::Serialize(ISerialize &Ar, int Ver)
{
	FSurfaceParameter::Serialize(Ar, Ver);
	Ar << Value;
}

IValue *FSurfaceParameterTexture::GetValue()
{
	return GValueFactory ? &GValueFactory->Create(Value) : nullptr;
}

void FSurfaceParameterTexture::Serialize(ISerialize &Ar, int Ver)
{
	FSurfaceParameter::Serialize(Ar, Ver);
	Ar << Value;
}

//////////////////////////////////////////////////////////////////////////
void SurfaceObject::Serialize(ISerialize &Ar, int Ver)
{
	if (Ar.IsSaving())
	{
		int NumParas = OverrideFloatParameters.size();
		Ar << NumParas;
		for (int i = 0; i < OverrideFloatParameters.size(); ++i)
		{
			OverrideFloatParameters[i].Serialize(Ar, Ver);
		}

		NumParas = OverrideVectorParameters.size();
		Ar << NumParas;
		for (int i = 0; i < OverrideVectorParameters.size(); ++i)
		{
			OverrideVectorParameters[i].Serialize(Ar, Ver);
		}

		NumParas = OverrideTextureParameters.size();
		Ar << NumParas;
		for (int i = 0; i < OverrideTextureParameters.size(); ++i)
		{
			OverrideTextureParameters[i].Serialize(Ar, Ver);
		}
	
		NumParas = OverrideVec2Parameters.size();
		Ar << NumParas;
		for (int i = 0; i < OverrideVec2Parameters.size(); ++i)
		{
			OverrideVec2Parameters[i].Serialize(Ar, Ver);
		}

		NumParas = OverrideVec3Parameters.size();
		Ar << NumParas;
		for (int i = 0; i < OverrideVec3Parameters.size(); ++i)
		{
			OverrideVec3Parameters[i].Serialize(Ar, Ver);
		}
	}
	else if (Ar.IsLoading())
	{
		int NumParas = 0;
		Ar << NumParas;

		OverrideFloatParameters.resize(NumParas);
		for (int i = 0; i < OverrideFloatParameters.size(); ++i)
		{
			OverrideFloatParameters[i].Serialize(Ar, Ver);
		}

		Ar << NumParas;
		OverrideVectorParameters.resize(NumParas);
		for (int i = 0; i < OverrideVectorParameters.size(); ++i)
		{
			OverrideVectorParameters[i].Serialize(Ar, Ver);
		}

		Ar << NumParas;
		OverrideTextureParameters.resize(NumParas);
		for (int i = 0; i < OverrideTextureParameters.size(); ++i)
		{
			OverrideTextureParameters[i].Serialize(Ar, Ver);
		}

		if (Ver > MODELFILE_BODY_VER_7)
		{
			Ar << NumParas;
			OverrideVec2Parameters.resize(NumParas);
			for (int i = 0; i < OverrideVec2Parameters.size(); ++i)
			{
				OverrideVec2Parameters[i].Serialize(Ar, Ver);
			}

			NumParas = OverrideVec3Parameters.size();
			Ar << NumParas;
			for (int i = 0; i < OverrideVec3Parameters.size(); ++i)
			{
				OverrideVec3Parameters[i].Serialize(Ar, Ver);
			}
		}
	}

	if (Ar.IsSaving())
	{
		Ar << Uri;
	}
	else if (Ar.IsLoading())
	{
		Uri = LoadStr(Ar);
	}
}

const char *SurfaceObject::GetUri()
{
	return Uri.c_str();
}

void SurfaceObject::SetSource(int InType, const char *InUri)
{
	Type = InType;
	Uri = InUri;
}

#define  Set_Value(ParamType, OverrideParameters){			\
	size_t i = 0;											\
	for (; i < OverrideParameters.size(); ++i){				\
		ParamType &VecParam = OverrideParameters[i];		\
		if (VecParam.ParamName == name){					\
			VecParam.Value = Value;							\
			break;											\
		}													\
	}														\
	if (i >= OverrideParameters.size()){					\
		ParamType V;										\
		V.ParamName = name;									\
		V.Value = Value;									\
		OverrideParameters.push_back(V);					\
	}														\
}

#define  Get_Value(ParamType, DefValue, OverrideParameters){	\
	for (size_t i = 0; i < OverrideParameters.size(); ++i) {	\
		ParamType &VecParam = OverrideParameters[i];			\
		if (VecParam.ParamName == name)	{						\
			return VecParam.Value;								\
		}														\
	}															\
	return DefValue;											\
}


void SurfaceObject::SetScalar(const char *name, float Value)
{
	Set_Value(FSurfaceParameterFloat, OverrideFloatParameters);
}

void SurfaceObject::SetVector2D(const char *name, const kPoint &Value)
{
 	Set_Value(FSurfaceParameterVec2, OverrideVec2Parameters);
}

void SurfaceObject::SetVector3D(const char *name, const kVector3D &Value)
{
	Set_Value(FSurfaceParameterVec3, OverrideVec3Parameters);
}

void SurfaceObject::SetVector(const char *name, const kVector4D &Value)
{
	Set_Value(FSurfaceParameterVector, OverrideVectorParameters);
}

float SurfaceObject::GetScalar(const char *name)
{
 	Get_Value(FSurfaceParameterFloat, 0.0f, OverrideFloatParameters)
}

kPoint SurfaceObject::GetVector2D(const char *name)
{
	Get_Value(FSurfaceParameterVec2, kPoint(), OverrideVec2Parameters)
}

kVector3D SurfaceObject::GetVector3D(const char *name)
{
	Get_Value(FSurfaceParameterVec3, kVector3D(), OverrideVec3Parameters)
}

kVector4D SurfaceObject::GetVector(const char *name)
{
	Get_Value(FSurfaceParameterVector, kVector4D(), OverrideVectorParameters)
}

extern std::string  GetTexSlotName(ETexSlot Slot);

int SurfaceObject::GetTexture(ETexSlot slot)
{
	std::string name = GetTexSlotName(slot);
	for (size_t i = 0; i < OverrideTextureParameters.size(); ++i)
	{
		FSurfaceParameterTexture &TexParam = OverrideTextureParameters[i];
		if (TexParam.ParamName == name)
		{
			return (int)i;
		}
	}
	return -1;
}

IValue *SurfaceObject::FindParamValue(const char *name)
{
	if (GValueFactory)
	{
		for (size_t i = 0; i < OverrideFloatParameters.size(); ++i)
		{
			if (OverrideFloatParameters[i].ParamName == name)
			{
				return &GValueFactory->Create(OverrideFloatParameters[i].Value);
			}
		}

		for (size_t i = 0; i < OverrideVec2Parameters.size(); ++i)
		{
			if (OverrideVec2Parameters[i].ParamName == name)
			{
				return &GValueFactory->Create(&OverrideVec2Parameters[i].Value);
			}
		}

		for (size_t i = 0; i < OverrideVec3Parameters.size(); ++i)
		{
			if (OverrideVec3Parameters[i].ParamName == name)
			{
				return &GValueFactory->Create(&OverrideVec3Parameters[i].Value);
			}
		}

		for (size_t i = 0; i < OverrideVectorParameters.size(); ++i)
		{
			if (OverrideVectorParameters[i].ParamName == name)
			{
				return &GValueFactory->Create(&OverrideVectorParameters[i].Value);
			}
		}

		for (size_t i = 0; i < OverrideTextureParameters.size(); ++i)
		{
			if (OverrideTextureParameters[i].ParamName == name)
			{
				return &GValueFactory->Create(OverrideTextureParameters[i].Value);
			}
		}
	}
	return nullptr;
}


