#include "Tube.h"
#include "IBuildingSDK.h"

extern IBuildingSDK *GSDK;

IPluginObject* TubeDesc::CreateObject()
{
	return new Tube();
}

void TubeDesc::DestroyObject(IPluginObject *tObj)
{
	if (tObj)
	{
		delete tObj;
	}
}

IValue *Tube::GetFunctionProperty(const char* name)
{
	IValue *pValue = nullptr;
	std::string PropName = name;
	IValueFactory* ValueFactory = GSDK->GetValueFactory();

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
		else if (PropName == "Location")
		{
			pValue = &ValueFactory->Create(&Location);
		}
		else if (PropName == "WallIDs")
		{
			kArray<int> WallKArr(WallIDs);
			pValue = &ValueFactory->Create(&WallKArr);
		}
		else if (PropName == "TubeSurfArea")
		{
			pValue = &ValueFactory->Create(&TubeSurfArea);
		}
		else if (PropName == "WallArea")
		{
			pValue = &ValueFactory->Create(&WallArea);
		}
		else if (PropName=="Border")
		{
			kArray<kVector3D> BorderArray(BorderVertex);
			pValue = &ValueFactory->Create(&BorderArray);
		}
	}

	return pValue;
}

bool Tube::SetFunctionProperty(const char *name, const IValue *Value)
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
			kArray<int> KWallVal = Value->IntArrayValue();
			WallIDs.clear();
			WallIDs.push_back(KWallVal[0]);
			WallIDs.push_back(KWallVal[1]);
			KWallVal.reset();
			return true;
		}
		else if (PropName == "TubeSurfArea")
		{
			TubeSurfArea = Value->FloatValue();
			return true;
		}
		else if (PropName == "WallArea")
		{
			WallArea = Value->Vec2Value();
			return true;
		}
		
	}
	return false;
}

void Tube::Serialize(ISerialize &Ar, unsigned int Ver)
{
	Ar << Width;
	Ar << Length;
	Ar << Height;
	Ar << Location;
	Ar << IsWidth;
	int WallNum = WallIDs.size();
	for (int i = 0; i < WallNum; i++)
	{
		Ar << WallIDs[i];
	}
	int CompType = 2;
	Ar << CompType;
	Ar << TubeID;
	Ar << CurAngle;
}

void Tube::SetWidth(float width)
{
	Width = width;
}
float Tube::GetWidth()
{
	return Width;
}

void Tube::SetLength(float length)
{
	Length = length;
}
float Tube::GetLength()
{
	return Length;
}

void Tube::SetHeight(float height)
{
	Height = height;
}
float Tube::GetHeight()
{
	return Height;
}

void Tube::SetOrigAngle(float angle)
{
	OrigAngle = angle;
}
float Tube::GetOrigAngle()
{
	return OrigAngle;
}

//void Tube::SetCurWallAngle(float curAngle)
//{
//	CurAngle = curAngle;
//}
//float Tube::GetCurWallAngle()
//{
//	return CurAngle;
//}
//
//void Tube::SetIsWidth(bool close)
//{
//	IsWidth = close;
//}
//bool Tube::GetIsWidth()
//{
//	return IsWidth;
//}

void Tube::SetIsAngle(bool isSet)
{
	IsAngle = isSet;
}
bool Tube::GetIsAngle()
{
	return IsAngle;
}

void Tube::SetSize(kVector3D size)
{
	DefaultSize = size;
}
kVector3D Tube::GetSize()
{
	return DefaultSize;
}

void Tube::SetDefaultLoc(kPoint DefLoc)
{
	DefaultLoc = DefLoc;
}
kPoint Tube::GetDefaultLoc()
{
	return DefaultLoc;
}

void Tube::SetLocation(kPoint loc)
{
	Location = loc;
}
kPoint Tube::GetLocation()
{
	return Location;
}

//void Tube::SetWallForw(kPoint WallForw)
//{
//	CurWallForward = WallForw;
//}
//kPoint Tube::GetWallForw()
//{
//	return CurWallForward;
//}
//
//void Tube::SetWallID(std::vector<int> wallId)
//{
//	WallIDs = wallId;
//}
//std::vector<int> Tube::GetWallId()
//{
//	return WallIDs;
//}

void Tube::SetOrigBorder(std::vector<kVector3D> OrigBorderArr)
{
	OrigBorderVer = OrigBorderArr;
}
std::vector<kVector3D> Tube::GetOrigBorder()
{
	return OrigBorderVer;
}

void Tube::SetBorder(std::vector<kVector3D> BorderArr)
{
	BorderVertex = BorderArr;
}
std::vector<kVector3D> Tube::GetBorder()
{
	return BorderVertex;
}

//void Tube::SetTubeSurfArea(float area)
//{
//	TubeSurfArea = area;
//}
//float Tube::GetTubeArea()
//{
//	return TubeSurfArea;
//}
//
//void Tube::SetWallArea(kPoint wallArea)
//{
//	WallArea = wallArea;
//}
//kPoint Tube::GetWallArea()
//{
//	return WallArea;
//}
//
//void Tube::SetDirection(kPoint dir)
//{
//	TubeDirVect = dir;
//}
//kPoint Tube::GetDirection()
//{
//	return TubeDirVect;
//}