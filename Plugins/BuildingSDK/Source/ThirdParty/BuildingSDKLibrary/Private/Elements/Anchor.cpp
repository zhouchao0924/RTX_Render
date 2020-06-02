
#include "ISuite.h"
#include "Anchor.h"
#include "Corner.h"
#include "ModelInstance.h"
#include "Class/Property.h"

BEGIN_DERIVED_CLASS(Anchor, BuildingObject)
	ADD_PROP_READONLY(CornerID, IntProperty)
	ADD_PROP_READONLY(LinkObjects, IntProperty)
END_CLASS()

Anchor::Anchor()
{
}

void Anchor::Serialize(ISerialize &Ar)
{
	BuildingObject::Serialize(Ar);
}

void Anchor::GetPins(std::vector<ObjectID> &OutPins)
{
	OutPins.push_back(CornerID);
}

IValue *Anchor::GetFunctionProperty(const std::string &name)
{
	IValue *pValue = BuildingObject::GetFunctionProperty(name);
	if (name == "Location")
	{
		kPoint Location = GetLocation();
		pValue = &GValueFactory->Create(&Location, false);
	}
	else if (name == "Pins")
	{
		pValue = &GValueFactory->Create();
		
		std::vector<ObjectID> Pins;
		GetPins(Pins);

		for (size_t i = 0; i < Pins.size(); ++i)
		{
			IValue *pID = &GValueFactory->Create(Pins[i]);
			if (pID)
			{
				pValue->AddField(*pID);
			}
		}
	}
	return pValue;
}

bool Anchor::SetFunctionProperty(const std::string &name, const IValue *Value)
{
	if (Value && name == "Location")
	{
		Corner *pCorner = SUITE_GET_BUILDING_OBJ(CornerID, Corner);
		if (pCorner)
		{
			pCorner->SetLocation(Value->Vec2Value());
		}
		return true;
	}
	return false;
}

kPoint Anchor::GetLocation()
{
	Corner *pCorner = SUITE_GET_BUILDING_OBJ(CornerID, Corner);
	if (pCorner)
	{
		return pCorner->Location;
	}
	return kPoint();
}

void Anchor::Link(ModelInstance *InModel)
{
	if (InModel)
	{
		ObjectID ModelID = InModel->GetID();
			
		size_t i = 0;
		for (; i < LinkObjects.size(); ++i);

		if (i >= LinkObjects.size())
		{
			InModel->AnchorID = GetID();
			UpdateTransform(InModel);
			LinkObjects.push_back(ModelID);
		}
	}
}

void Anchor::MarkNeedUpdate()
{
	for (size_t i = 0; i < LinkObjects.size(); ++i)
	{
		ModelInstance *pModel =  SUITE_GET_BUILDING_OBJ(LinkObjects[i], ModelInstance);
		if (pModel)
		{
			UpdateTransform(pModel);
		}
	}
}

void Anchor::UpdateTransform(ModelInstance *pModel)
{
	pModel->Location = GetLocation();
}

void Anchor::UnLink(ModelInstance *InModel)
{
	if (InModel)
	{
		ObjectID ModelID = InModel->GetID();

		size_t i = 0;
		for (; i < LinkObjects.size(); ++i);
		
		if (i < LinkObjects.size())
		{
			LinkObjects.erase(LinkObjects.begin() + i);
		}
	}
}

ObjectID Anchor::GetModel(int index /*= 0*/)
{
	if (LinkObjects.empty())
	{
		return INVALID_OBJID;
	}
	return LinkObjects[0];
}

ModelInstance *Anchor::GetModelByType(int type)
{
	for (int i = 0; i < LinkObjects.size(); ++i)
	{
		ModelInstance *pModel = SUITE_GET_BUILDING_OBJ(LinkObjects[i], ModelInstance);
		if (pModel && pModel->Type == type)
		{
			return pModel;
		}
	}
	return nullptr;
}

ModelInstance * Anchor::SetModelByType(int type, const std::string ResID)
{
	ModelInstance *pModel = GetModelByType(0);
	if (!pModel)
	{
		ObjectID ModelID = _Suite->AddModelToAnchor(GetID(), ResID.c_str(), kVector3D(), kRotation(), kVector3D(1.0f), 0);
		pModel = SUITE_GET_BUILDING_OBJ(ModelID, ModelInstance);
	}

	if (pModel)
	{
		pModel->ResID = ResID;
		pModel->MarkNeedUpdate();
	}

	return pModel;
}

void Anchor::OnDestroy()
{
	if (_Suite)
	{
		_Suite->DeleteObject(CornerID, true);
			
		for (size_t i = 0; i < LinkObjects.size(); ++i)
		{
			ObjectID ObjID = LinkObjects[i];
			_Suite->DeleteObject(ObjID, true);
		}

		CornerID = INVALID_OBJID;
		LinkObjects.clear();
	}
	BuildingObject::OnDestroy();
}


