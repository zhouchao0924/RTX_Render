#ifndef PATTERN_EDITOR_INTERFACE_H
#define PATTERN_EDITOR_INTERFACE_H

#include "IClass.h"
#include <memory>

#include "IPatternObject.h"

class IShapeObject;

class IPatternEditor
{
public:
	virtual IPatternObject* CreatePattern() = 0;

	virtual IPatternObject* GetPatternObject(IObject* Object) = 0;
};

#endif
