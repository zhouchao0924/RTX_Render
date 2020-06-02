#include "Sewer.h"
#include"IBuildingSDK.h"

extern IBuildingSDK* GSDK;

IPluginObject* SewerDesc::CreateObject()
{
	return new Sewer();
}

void SewerDesc::DestroyObject(IPluginObject* pObj)
{
	if (pObj)
	{
		delete pObj;
	}
}

IValue* Sewer::GetFunctionProperty(const char* name)
{
	IValue* pValue = nullptr;
	std::string PropName = name;
	IValueFactory *ValueFactory = GSDK->GetValueFactory();
	if (ValueFactory)
	{
		if (PropName=="Diameter")
		{
			pValue = &ValueFactory->Create(Diam);
		}
		else if (PropName=="Height")
		{
			pValue = &ValueFactory->Create(Height);
		}
		else if (PropName=="Location")
		{
			pValue = &ValueFactory->Create(&Location);
		}
	}

	return pValue;
}

bool Sewer::SetFunctionProperty(const char* name, const IValue* Value)
{
	if (Value)
	{
		std::string PropName = name;
		if (PropName=="Diameter")
		{
			Diam = Value->FloatValue();
			return true;
		}
		else if (PropName=="Height")
		{
			Height = Value->FloatValue();
			return true;
		}
		else if (PropName == "Loc")
		{
			Location = Value->Vec2Value();
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void Sewer::Serialize(ISerialize &Ar, unsigned int Ver)
{
	Ar << Diam;
	Ar << Height;
	Ar << Location;
}

void Sewer::SetDiam(float dval)
{
	Diam = dval;
}
float Sewer::GetDiam()
{
	return Diam;
}

void Sewer::SetHeght(float height)
{
	Height = height;
}
float Sewer::GetHeight()
{
	return Height;
}

void Sewer::SetSize(kPoint size)
{
	DefaultSize = size;
}
kPoint Sewer::GetSize()
{
	return DefaultSize;
}

void Sewer::SetDefaultLoc(kVector3D defLoc)
{
	DefaultLoc = defLoc;
}
kVector3D Sewer::GetDefaultLoc()
{
	return DefaultLoc;
}

void Sewer::SetLocation(kPoint Loc)
{
	Location = Loc;
}
kPoint Sewer::GetLocation()
{
	return Location;
}
