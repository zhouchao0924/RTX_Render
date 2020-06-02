#ifndef CRAFT_PAVING_EDITOR_INTERFACE_H
#define CRAFT_PAVING_EDITOR_INTERFACE_H

#include "IAreaLayerObject.h"

class ICraftPavingEditor
{
public:

	virtual IAreaLayerObject* CreateAreaLayer() = 0;
};

#endif
