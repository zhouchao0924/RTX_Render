
#include "Pole.h"
#include "IBuildingSDK.h"

extern IBuildingSDK *GSDK;

IPluginObject *PoleDesc::CreateObject()
{
	return new Pole();
}

void PoleDesc::DestroyObject(IPluginObject *pObj)
{
	if (pObj)
	{
		delete pObj;
	}
}

IValue *Pole::GetFunctionProperty(const char *name)
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
		else if (PropName == "Length")
		{
			pValue = &ValueFactory->Create(Length);
		}
		else if (PropName == "Height")
		{
			pValue = &ValueFactory->Create(Height);
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
		else if (PropName == "FlueSurfArea")
		{
			pValue = &ValueFactory->Create(&FlueSurfArea);
		}
		else if (PropName == "WallArea")
		{
			pValue = &ValueFactory->Create(&WallArea);
		}
		else if (PropName=="Border")
		{
			/*kArray<kVector3D> BorderArray(BorderVertex);
			int num = BorderArray.size();*/
			pValue = &ValueFactory->Create(BorderVertex.data());
		}
	}

	return pValue;
}

bool Pole::SetFunctionProperty(const char *name, const IValue *Value)
{
	if (Value)
	{
		std::string PropName = name;

		if (PropName == "Width")
		{
			Width = Value->FloatValue();
			return true;
		}
		else if (PropName == "Length")
		{
			Length = Value->FloatValue();
			return true;
		}
		else if (PropName == "Height")
		{
			Height = Value->FloatValue();
			return true;
		}
		else if (PropName == "Angle")
		{
			OrigAngle = Value->FloatValue();
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
		else if (PropName == "FlueSurfArea")
		{
			FlueSurfArea = Value->FloatValue();
			return true;
		}
		else if (PropName == "WallArea")
		{
			WallArea = Value->Vec2Value();
			return true;
		}
		else if (PropName == "ComponentID")
		{
			PoleID = Value->IntValue();
			return true;
		}
		else if (PropName == "ComponentAngle")
		{
			CurAngle = Value->FloatValue();
			return true;
		}
		else if (PropName == "SectionIDs")
		{
			kArray<int> DataPt= Value->IntArrayValue();
			SectionIndex.clear();
			for (size_t i = 0; i < DataPt.size(); i++)
			{
				SectionIndex.push_back(DataPt[i]);
			}
			return true;
		}
	}
	return false;
}

void Pole::Serialize(ISerialize &Ar, unsigned int Ver)
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
	int CompType = 1;
	Ar << CompType;
	Ar << PoleID;
	Ar << CurAngle;

}

void Pole::SetWidth(float width)
{
	Width = width;
}
float Pole::GetWidth()
{
	return Width;
}

void Pole::SetLength(float length)
{
	Length = length;
}
float Pole::GetLength()
{
	return Length;
}

void Pole::SetHeight(float height)
{
	Height = height;
}
float Pole::GetHeight()
{
	return Height;
}

void Pole::SetOrigAngle(float angle)
{
	OrigAngle = angle;
}
float Pole::GetOrigAngle()
{
	return OrigAngle;
}

//void Pole::SetCurWallAngle(float curAngle)
//{
//	CurAngle = curAngle;
//}
//float Pole::GetCurWallAngle()
//{
//	return CurAngle;
//}
//
//void Pole::SetIsWidth(bool close)
//{
//	IsWidth = close;
//}
//bool Pole::GetIsWidth()
//{
//	return IsWidth;
//}

void Pole::SetIsAngle(bool isSet)
{
	IsAngle = isSet;
}
bool Pole::GetIsAngle()
{
	return IsAngle;
}

void Pole::SetSize(kVector3D size)
{
	DefaultSize = size;
}
kVector3D Pole::GetSize()
{
	return DefaultSize;
}

void Pole::SetDefaultLoc(kPoint DefLoc)
{
	DefaultLoc = DefLoc;
}
kPoint Pole::GetDefaultLoc()
{
	return DefaultLoc;
}

void Pole::SetLocation(kPoint loc)
{
	Location = loc;
}
kPoint Pole::GetLocation()
{
	return Location;
}

//void Pole::SetWallForw(kPoint WallForw)
//{
//	CurWallForward = WallForw;
//}
//kPoint Pole::GetWallForw()
//{
//	return CurWallForward;
//}
//
//void Pole::SetWallID(std::vector<int> WallArr)
//{
//	WallIDs = WallArr;
//}
//std::vector<int> Pole::GetWallId()
//{
//	return WallIDs;
//}

void Pole::SetOrigBorder(std::vector<kVector3D> OrigBorderArr)
{
	OrigBorderVer = OrigBorderArr;
}
std::vector<kVector3D> Pole::GetOrigBorder()
{
	return OrigBorderVer;
}

void Pole::SetBorder(std::vector<kVector3D> BorderArr)
{
	BorderVertex = BorderArr;
}
std::vector<kVector3D> Pole::GetBorder()
{
	return BorderVertex;
}

//void Pole::SetPoleSurfArea(float area)
//{
//	FlueSurfArea = area;
//}
//float Pole::GetPoleArea()
//{
//	return FlueSurfArea;
//}
//
//void Pole::SetWallArea(kPoint wallArea)
//{
//	WallArea = wallArea;
//}
//kPoint Pole::GetWallArea()
//{
//	return WallArea;
//}
//
//void Pole::SetDirection(kPoint dir)
//{
//	Forward = dir;
//}
//kPoint Pole::GetDirection()
//{
//	return Forward;
//}


void Pole::SetMaterialUris(std::vector<std::string> MatUris)
{
	SectMatUris = MatUris;
}
