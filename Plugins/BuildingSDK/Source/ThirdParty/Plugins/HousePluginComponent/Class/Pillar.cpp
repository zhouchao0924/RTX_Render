#include "Pillar.h"
#include "IBuildingSDK.h"

extern IBuildingSDK *GSDK;

IPluginObject* PillarDesc::CreateObject()
{
	return new Pillar();
}

void PillarDesc::DestroyObject(IPluginObject* pObj)
{
	if (pObj)
	{
		delete pObj;
	}
}

IValue *Pillar::GetFunctionProperty(const char *name)
{
	IValue *pValue = nullptr;
	std::string PropName = name;
	IValueFactory *ValueFactory = GSDK->GetValueFactory();

	if (ValueFactory)
	{
		if (PropName == "Width")
		{
			pValue = &ValueFactory->Create(Width);
		}
		else if (PropName == "Height")
		{
			pValue = &ValueFactory->Create(Height);
		}
		else if (PropName == "Length")
		{
			pValue = &ValueFactory->Create(Length);
		}
		else if (PropName == "IsAnchor")
		{
			pValue = &ValueFactory->Create(IsAnchor);
		}
		else if (PropName == "Location")
		{
			pValue = &ValueFactory->Create(&Location);
		}
		else if (PropName == "WallIDs")
		{
			kArray<int> WallKArr(WallIDs);
			pValue = &ValueFactory->Create(&WallKArr);
		}
		else if (PropName == "PillarSurfArea")
		{
			pValue = &ValueFactory->Create(&PillarSurfArea);
		}
		else if (PropName == "PillarDirVect")
		{
			pValue = &ValueFactory->Create(&PillarDirVect);
		}
		else if (PropName == "WallArea")
		{
			pValue = &ValueFactory->Create(&WallArea);
		}
		else if (PropName == "Border")
		{
			kArray<kVector3D> BorderArray(BorderVertex);
			pValue = &ValueFactory->Create(&BorderArray);
		}
	}

	return pValue;
}

bool Pillar::SetFunctionProperty(const char *name, const IValue *Value)
{
	if (Value)
	{
		std::string PropName = name;

		if (PropName == "Width")
		{
			Width = Value->FloatValue();
			return true;
		}
		else if (PropName == "Height")
		{
			Height = Value->FloatValue();
			return true;
		}
		else if (PropName == "Length")
		{
			Length = Value->FloatValue();
			return true;
		}
		else if (PropName == "Location")
		{
			Location = Value->Vec2Value();
			return true;
		}
		else if (PropName == "WallIDs")
		{
			//WallIDs = Value->IntArrayValue();
			kArray<int> KWallVal = Value->IntArrayValue();
			WallIDs.clear();
			WallIDs.push_back(KWallVal[0]);
			WallIDs.push_back(KWallVal[1]);
			KWallVal.reset();
			return true;
		}
		else if (PropName == "PillarSurfArea")
		{
			PillarSurfArea = Value->FloatValue();
			return true;
		}
		else if (PropName == "IsAnchor")
		{
			IsAnchor = Value->BoolValue();
			return true;
		}
		else if (PropName == "WallArea")
		{
			WallArea = Value->FloatValue();
			return true;
		}
		else if (PropName == "PillarDirVect")
		{
			PillarDirVect = Value->Vec2Value();
			return true;
		}

	}
	return false;
}

void Pillar::Serialize(ISerialize &Ar, unsigned int Ver)
{
	Ar << Width;
	Ar << Height;
	Ar << Length;
	Ar << Location;
	Ar << IsWidth;
	int WallNum = WallIDs.size();
	for (int i = 0; i < WallNum; i++)
	{
		Ar << WallIDs[i];
	}
	int CompType = 3;
	Ar << CompType;
	Ar << PillarID;
	Ar << CurAngle;
	Ar << IsAnchor;
}

void Pillar::SetWidth(float width)
{
	Width = width;
}
float Pillar::GetWidth()
{
	return Width;
}

void Pillar::SetLength(float length)
{
	Length = length;
}
float Pillar::GetLength()
{
	return Length;
}

void Pillar::SetHeight(float height)
{
	Height = height;
}
float Pillar::GetHeight()
{
	return Height;
}

void Pillar::SetOrigAngle(float angle)
{
	OrigAngle = angle;
}
float Pillar::GetOrigAngle()
{
	return OrigAngle;
}

//void Pillar::SetCurWallAngle(float curAngle)
//{
//	CurAngle = curAngle;
//}
//float Pillar::GetCurWallAngle()
//{
//	return CurAngle;
//}
//
//void Pillar::SetIsWidth(bool close)
//{
//	IsWidth = close;
//}
//bool Pillar::GetIsWidth()
//{
//	return IsWidth;
//}

void Pillar::SetIsAngle(bool isSet)
{
	IsAngle = isSet;
}
bool Pillar::GetIsAngle()
{
	return IsAngle;
}

void Pillar::SetSize(kVector3D size)
{
	DefaultSize = size;
}
kVector3D Pillar::GetSize()
{
	return DefaultSize;
}

void Pillar::SetDefaultLoc(kPoint DefLoc)
{
	DefaultLoc = DefLoc;
}
kPoint Pillar::GetDefaultLoc()
{
	return DefaultLoc;
}

//void Pillar::SetAnchor(bool IsAnch)
//{
//	IsAnchor = IsAnch;
//}
//bool Pillar::GetAnchor()
//{
//	return IsAnchor;
//}

void Pillar::SetLocation(kPoint loc)
{
	Location = loc;
}
kPoint Pillar::GetLocation()
{
	return Location;
}

//void Pillar::SetWallForw(kPoint WallForw)
//{
//	CurWallForward = WallForw;
//}
//kPoint Pillar::GetWallForw()
//{
//	return CurWallForward;
//}
//
//void Pillar::SetWallID(std::vector<int> wallId)
//{
//	WallIDs = wallId;
//}
//std::vector<int> Pillar::GetWallId()
//{
//	return WallIDs;
//}

void Pillar::SetOrigBorder(std::vector<kVector3D> OrigBorderArr)
{
	OrigBorderVer = OrigBorderArr;
}
std::vector<kVector3D> Pillar::GetOrigBorder()
{
	return OrigBorderVer;
}

void Pillar::SetBorder(std::vector<kVector3D> BorderArr)
{
	BorderVertex = BorderArr;
}
std::vector<kVector3D> Pillar::GetBorder()
{
	return BorderVertex;
}

//void Pillar::SetPillarSurfArea(float area)
//{
//	PillarSurfArea = area;
//}
//float Pillar::GetPillarArea()
//{
//	return PillarSurfArea;
//}
//
//void Pillar::SetWallArea(kPoint wallArea)
//{
//	WallArea = wallArea;
//}
//kPoint Pillar::GetWallArea()
//{
//	return WallArea;
//}
//
//void Pillar::SetDirection(kPoint dir)
//{
//	PillarDirVect = dir;
//}
//kPoint Pillar::GetDirection()
//{
//	return PillarDirVect;
//}