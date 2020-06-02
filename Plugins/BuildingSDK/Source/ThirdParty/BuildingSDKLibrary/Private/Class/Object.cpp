
#include "Object.h"
#include "PValue.h"
#include "MXFile/MXTexture.h"
#include "png/lodepng_util.h"

BEGIN_CLASS(Object)
END_CLASS()

Object::Object()
	:_ID(INVALID_OBJID)
	, ClsDesc(nullptr)
	, _Suite(nullptr)
{
}

bool Object::IsA(EObjectType Type)
{
	if (GetType() == Type)
	{
		return true;
	}

	ObjectDesc *Desc = ClsDesc ? ClsDesc->SuperClassDesc : nullptr;
	while (Desc)
	{
		if (Desc->ObjectType == Type)
		{
			return true;
		}
		Desc = Desc->SuperClassDesc;
	}

	return false;
}

Object::~Object()
{
	for (std::unordered_map<std::string, IValue *>::iterator it = _dictionary.begin(); it != _dictionary.end(); ++it)
	{
		IValue *pValue = it->second;
		if (pValue)
		{
			pValue->Release();
		}
	}
	_dictionary.clear();
}

void Object::GetAllDesc(std::vector<ObjectDesc*> &Descs, std::vector<int> &DescCounts)
{
	ObjectDesc *Desc = ClsDesc;
	while (Desc)
	{
		Descs.push_back(Desc);
		DescCounts.push_back(Desc->GetNumberOfProperty());
		Desc = Desc->SuperClassDesc;
	}
}

int Object::GetNumberOfProperties()
{
	std::vector<ObjectDesc*> Descs;
	std::vector<int> DescCounts;

	int NumProps = 0;
	GetAllDesc(Descs, DescCounts);

	for (size_t i = 0; i < DescCounts.size(); ++i)
	{
		NumProps += DescCounts[i];
	}

	return NumProps;
}

IProperty *Object::GetProperty(int Index)
{
	IProperty *Prop = nullptr;

	std::vector<ObjectDesc*> Descs;
	std::vector<int> DescCounts;

	GetAllDesc(Descs, DescCounts);

	int SubIndex = Index;
	for (int i = DescCounts.size() - 1; i >= 0; --i)
	{
		if (SubIndex < DescCounts[i])
		{
			ObjectDesc *Desc = Descs[i];
			Prop = Desc->GetProperty(SubIndex);
			break;
		}
		else
		{
			SubIndex -= DescCounts[i];
		}
	}

	return Prop;
}

IProperty *Object::GetProperty(const char *Name)
{
	IProperty *Prop = nullptr;

	std::vector<ObjectDesc*> Descs;
	std::vector<int> DescCounts;

	GetAllDesc(Descs, DescCounts);

	for (size_t i = 0; i < Descs.size(); ++i)
	{
		ObjectDesc *Desc = Descs[i];
		if (Desc)
		{
			Prop = Desc->GetProperty(Name);
			if (Prop)
			{
				break;
			}
		}
	}

	return Prop;
}

IValue &Object::GetPropertyValue(const char  *Name)
{
	IValue *pValue = nullptr;
	IProperty *Prop = GetProperty(Name);
	if (Prop)
	{
		pValue = Prop->GetValue(this);
	}
	else
	{
		pValue = GetFunctionProperty(Name);
	}

	return pValue ? *pValue : PValue::Nil;
}

void Object::SetPropertyValue(const char *Name, const IValue *Value)
{
	if (!REG_SetPropertyValue(Name, Value))
	{
		SetFunctionProperty(Name, Value);
	}
}

IValue *Object::REG_GetPropertyValue(const char  *Name)
{
	static PValue s_defValue;
	IValue *pValue = nullptr;
	IProperty *Prop = GetProperty(Name);
	if (Prop)
	{
		pValue = Prop->GetValue(this);
	}
	return pValue;
}

bool Object::REG_SetPropertyValue(const char *Name, const IValue *Value)
{
	IProperty *Prop = GetProperty(Name);
	if (Prop)
	{
		Prop->SetValue(this, Value);
		return true;
	}
	return false;
}

void Object::GetProperties(std::vector<IProperty *> &Properties)
{
	ObjectDesc *Desc = ClsDesc;
	while (Desc)
	{
		for (int i = 0; i < Desc->GetNumberOfProperty(); ++i)
		{
			IProperty *prop = Desc->GetProperty(i);
			if (prop)
			{
				Properties.push_back(prop);
			}
		}
		Desc = Desc->SuperClassDesc;
	}
}

void SaveValue(ISerialize &Ar, IValue *pValue)
{
	EVarType VarType = pValue->GetType();

	switch (VarType)
	{
		case kV_Float:
		{
			float v = pValue->FloatValue();
			Ar << v;
			break;
		}
		case kV_Int:
		{
			int v = pValue->IntValue();
			Ar << v;
			break;
		}
		case kV_Bool:
		{
			bool v = pValue->BoolValue();
			Ar << v;
			break;
		}
		case kV_RawString:
		{
			Ar << pValue->StrValue();
			break;
		}
		case kV_StdString:
		{
			Ar << pValue->StrValue();
			break;
		}
		case kV_Vec3D:
		{
			Ar << pValue->Vec3Value();
			break;
		}
		case kV_Vec2D:
		{
			Ar << pValue->Vec2Value();
			break;
		}
		case kV_Rotator:
		{
			Ar << pValue->RotationValue();
			break;
		}
		case kV_Bounds:
		{
			Ar << pValue->Bounds();
			break;
		}
		case kV_IntArray:
		{
			std::vector<int> &intArray = pValue->IntArrayValue();
			int Num = (int)intArray.size();
			Ar << Num;
			if (Num > 0)
			{
				Ar.Serialize(&(intArray[0]), Num * sizeof(int));
			}
			break;
		}
		case kV_Vec2DArray:
		{
			std::vector<kPoint> &vec2Array = pValue->Vec2ArrayValue();
			int Num = (int)vec2Array.size();
			Ar << Num;
			if (Num > 0)
			{
				Ar.Serialize(&(vec2Array[0]), Num * sizeof(kPoint));
			}
			break;
		}
		case kV_Plane:
		{
			Ar << pValue->PlaneValue();
			break;
		}
		default: break;
	}
}

IValue *LoadValue(EVarType VarType, ISerialize &Ar)
{
	IValue *pValue = nullptr;

	switch (VarType)
	{
	case kV_Float:
	{
		float v = 0;
		Ar << v;
		pValue = &GValueFactory->Create(v);
		break;
	}
	case kV_Int:
	{
		int v = 0;
		Ar << v;
		pValue = &GValueFactory->Create(v);
		break;
	}
	case kV_Bool:
	{
		bool v = false;
		Ar << v;
		pValue = &GValueFactory->Create(v);
		break;
	}
	case kV_StdString:
	{
		std::string v;
		Ar << v;
		pValue = &GValueFactory->Create(&v);
		break;
	}
	case kV_Vec3D:
	{
		kVector3D v;
		Ar << v;
		pValue = &GValueFactory->Create(&v);
		break;
	}
	case kV_Vec2D:
	{
		kPoint v;
		Ar << v;
		pValue = &GValueFactory->Create(&v);
		break;
	}
	case kV_Rotator:
	{
		kRotation v;
		Ar << v;
		pValue = &GValueFactory->Create(&v);
		break;
	}
	case kV_Bounds:
	{
		kBox3D v;
		Ar << v;
		pValue = &GValueFactory->Create(&v);
		break;
	}
	case kV_IntArray:
	{
		std::vector<int> intArray;
		int Num = 0;
		Ar << Num;
		intArray.resize(Num);
		if (Num > 0)
		{
			Ar.Serialize(&intArray[0], Num * sizeof(int));
		}
		pValue = &GValueFactory->Create(&intArray);
		break;
	}
	case kV_Vec2DArray:
	{
		std::vector<kPoint> vec2Array;
		int Num = 0;
		Ar << Num;
		vec2Array.resize(Num);
		if (Num > 0)
		{
			Ar.Serialize(&vec2Array[0], Num * sizeof(int));
		}
		pValue = &GValueFactory->Create(&vec2Array);
		break;
	}
	case kV_Plane:
	{
		kPlane3D v;
		Ar << v;
		pValue = &GValueFactory->Create(&v);
		break;
	}
	default: break;
	}
	return pValue;
}

void Object::SerializeProperties(ISerialize &Ar)
{
	if (ClsDesc)
	{
		if (Ar.IsSaving())
		{
			std::vector<IProperty *> Properties;
			GetProperties(Properties);

			int NumProperty = (int)Properties.size();
			Ar << NumProperty;

			for (int i = 0; i < NumProperty; ++i)
			{
				IProperty *prop = GetProperty(i);
				int type = (int)prop->GetValueType();
				const char *propName = prop->GetName();

				Ar << propName;
				Ar << type;

				PValue &Value = *((PValue *)prop->GetValue(this));
				SaveValue(Ar, &Value);
			}
		}
		else if (Ar.IsLoading())
		{
			int NumProperty = 0;
			Ar << NumProperty;
			for (int i = 0; i < NumProperty; ++i)
			{
				std::string propName;
				Ar << propName;

				int type = 0;
				Ar << type;

				IProperty *prop = GetProperty(propName.c_str());
				if (prop)
				{
					IValue *pValue = LoadValue(EVarType(type), Ar);
					if (pValue)
					{
						prop->SetValue(this, pValue);
					}
				}
			}
		}
	}
}

void Object::SerializeDictionary(ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		std::unordered_map<std::string, IValue *> _dictionary;
		int num = _dictionary.size();
		Ar << num;
		for (std::unordered_map<std::string, IValue *>::iterator it = _dictionary.begin(); it != _dictionary.end(); ++it)
		{
			std::string name = it->first;
			Ar << name;
			IValue *pValue = it->second;
			int VarType = (int)pValue->GetType();
			Ar << VarType;
			SaveValue(Ar, pValue);
		}
	}
	else if (Ar.IsLoading())
	{
		int num = 0;
		Ar << num;
		for (int i = 0; i < num; ++i)
		{
			std::string name;
			Ar << name;
			int VarType;
			Ar << VarType;
			IValue *pValue = LoadValue((EVarType)VarType, Ar);
			if (pValue)
			{
				SetValue(name.c_str(), pValue);
			}
		}
	}
}

void Object::Serialize(ISerialize &Ar)
{
	Ar << _ID;
	SerializeProperties(Ar);
	SerializeDictionary(Ar);
}

bool Object::SaveTextureToFile(int iTex, const char *Filename)
{
	void *pData = nullptr;
	int Width, Height, Pitch, Format, MipCount, nBytes;

	if (GetTextureInfo(iTex, Width, Height, MipCount, Pitch, Format))
	{
		if (GetTextureData(iTex, 0, pData, nBytes))
		{
			static std::vector<unsigned long> RGBAImage;
			RGBAImage.resize(Width*Height);

			if (Format == 5)
			{
				FModelTexture::BlockDecompressImageDXT1(Width, Height, (unsigned char *)pData, &RGBAImage[0]);
			}
			else if (Format == 7)
			{
				FModelTexture::BlockDecompressImageDXT5(Width, Height, (unsigned char *)pData, &RGBAImage[0]);
			}
			
			unsigned error = lodepng_encode32_file(Filename, (unsigned char *)&RGBAImage[0], Width, Height);
			if (error)
			{
				return false;
			}

			return true;
		}
	}

	return false;
}


IValue *Object::FindValue(const char *name)
{
	if (name)
	{
		std::unordered_map<std::string, IValue *>::iterator it = _dictionary.find(name);
		if (it != _dictionary.end())
		{
			return it->second;
		}
	}
	return nullptr;
}

void Object::SetValue(const char *name, IValue *value)
{
	if (name)
	{
		IValue *foundValue = FindValue(name);
		
		if (foundValue)
		{
			foundValue->Release();
		}

		if (value)
		{
			value->Retain();
			value->AddRef();
			_dictionary[name] = value;
		}
		else
		{
			std::unordered_map<std::string, IValue *>::iterator it = _dictionary.find(name);
			if (it != _dictionary.end())
			{
				_dictionary.erase(it);
			}
		}
	}
}

IValue *Object::GetValueArray()
{
	if (GValueFactory)
	{
		IValue *pValue = &GValueFactory->Create();
		for (std::unordered_map<std::string, IValue *>::iterator it = _dictionary.begin(); it != _dictionary.end(); ++it)
		{
			pValue->AddField(it->first.c_str(), *it->second);
		}
		return pValue;
	}
	return nullptr;
}

struct FHitInfo
{
	int			SectionIndex;
	kVector3D	Position;
	kVector3D	Normal;
	ObjectID	SurfaceID;
	float		Dist;
};

bool Object::HitTest(const kVector3D &RayStart, const kVector3D &RayDir, ObjectID &OutSurfaceID, int *OutSectionIndex /*= nullptr*/, kVector3D *OutPosition /*= nullptr*/, kVector3D *OutNormal /*= nullptr*/)
{
	int MeshCount = GetMeshCount();

	FHitInfo HitInfo;
	std::vector<FHitInfo> HitInfos;

	for (int i = 0; i < MeshCount; ++i)
	{
		IMeshObject *meshObj = GetMeshObject(i);
		if (meshObj->HitTest(RayStart, RayDir, &HitInfo.SectionIndex, &HitInfo.Position, &HitInfo.Normal))
		{
			HitInfo.Dist = (HitInfo.Position - RayStart).Size();
			HitInfo.SurfaceID = meshObj->GetSectionSurfaceID(HitInfo.SectionIndex);

			size_t k = HitInfos.size();
			HitInfos.resize(k + 1);

			if (k > 0)
			{
				size_t i = k - 1;
				for (; i>0; --i)
				{
					if (HitInfo.Dist < HitInfos[i-1].Dist)
					{
						HitInfos[i] = HitInfos[i - 1];
					}
					else
					{
						break;
					}
				}
				HitInfos[i] = HitInfo;
			}
			else
			{
				HitInfos[k] = HitInfo;
			}
		}
	}

	if (!HitInfos.empty())
	{
		if (OutPosition)
		{
			*OutPosition = HitInfos[0].Position;
		}

		if (OutNormal)
		{
			*OutNormal = HitInfos[0].Normal;
		}

		if (OutSectionIndex)
		{
			*OutSectionIndex = HitInfos[0].SectionIndex;
		}

		OutSurfaceID = HitInfos[0].SurfaceID;
	}

	return !HitInfos.empty();
}

