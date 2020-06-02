#ifndef I_AREALAYER_OBJECT_H
#define I_AREALAYER_OBJECT_H

#include "CraftPavingMacros.h"
#include "IClass.h"
#include "TArray.h"

#define AREALAYER_OBJECT_TYPE (PLUGIN_SECTION_1 + 2)

class IAreaLayerObject
{
public:

	virtual IObject* GetRawObject() = 0;

	virtual IAreaLayerObject* AddChildAreaLayer() = 0;

	virtual void GetDependentAreaUIDList(std::shared_ptr<TArrayTemplate<kString>>& DependentAreas) = 0;

	virtual void SetDependentAreaUIDList(const TArrayTemplate<kString>& DependentAreas) = 0;
};

enum class AreaLayerVersion
{
	PV_Init = 0,
};

#endif