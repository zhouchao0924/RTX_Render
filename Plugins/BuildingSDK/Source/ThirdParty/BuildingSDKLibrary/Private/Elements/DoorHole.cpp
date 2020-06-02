
#include "DoorHole.h"
#include "Wall.h"
#include "Corner.h"
#include "ISuite.h"
#include "ModelInstance.h"

BEGIN_DERIVED_CLASS(DoorHole, WallHole)
END_CLASS()

DoorHole::DoorHole()
{
}

void DoorHole::Serialize(ISerialize &Ar)
{
	WallHole::Serialize(Ar);
}

void DoorHole::UpdateTransform(ModelInstance *pModel)
{
	Corner *pCorner = SUITE_GET_BUILDING_OBJ(CornerID, Corner);
	if (pCorner)
	{
		pModel->Location = pCorner->Location;
	}

	Wall *pWall = SUITE_GET_BUILDING_OBJ(WallID, Wall);
	if (pWall)
	{
		pModel->Forward = pWall->GetForward();
	}

	pModel->AlignType = EAlignBottomCenter;
}

void DoorHole::SetModel(const std::string &Door, const std::string &DoorFrame, const std::string &DoorSone)
{
	if (Door.length() > 0)
	{
		_Suite->AddModelToAnchor(GetID(), Door.c_str(), kVector3D(), kRotation(), kVector3D(1.0f), 0);
	}

	if (DoorFrame.length() > 0)
	{
		_Suite->AddModelToAnchor(GetID(), DoorFrame.c_str(), kVector3D(), kRotation(), kVector3D(1.0f), 1);
	}

	if (DoorSone.length() > 0)
	{
		_Suite->AddModelToAnchor(GetID(), DoorSone.c_str(), kVector3D(), kRotation(), kVector3D(1.0f), 2);
	}
}

IValue *DoorHole::GetFunctionProperty(const std::string &name)
{
	IValue *pValue = WallHole::GetFunctionProperty(name);
	
	if (!pValue)
	{
		if (name == "Door")
		{
			ModelInstance *pModel = GetModelByType(0);
			if (pModel)
			{
				pValue = &GValueFactory->Create(pModel->GetID());
			}
		}
		else if (name == "DoorFrame")
		{
			ModelInstance *pModel = GetModelByType(1);
			if (pModel)
			{
				pValue = &GValueFactory->Create(pModel->GetID());
			}
		}
		else if (name == "DoorSone")
		{
			ModelInstance *pModel = GetModelByType(2);
			if (pModel)
			{
				pValue = &GValueFactory->Create(pModel->GetID());
			}
		}
	}

	return pValue;
}

bool DoorHole::SetFunctionProperty(const std::string &name, const IValue *Value)
{
	if (WallHole::SetFunctionProperty(name, Value))
	{
		return true;
	}

	if (Value)
	{
		std::string ResID = Value->StrValue();

		if (name == "Door")
		{
			SetModelByType(0, ResID);
			return true;
		}
		else if (name == "DoorFrame")
		{
			SetModelByType(1, ResID);
			return true;
		}
		else if (name == "DoorSone")
		{
			SetModelByType(2, ResID);
			return true;
		}
	}

	return false;
}

