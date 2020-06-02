
#pragma once

#include "IClass.h"

enum ECookedPlatform
{
	ETargetPC,
	ETargetIOS,
	ETargetWebGL,
	ETargetAndroid
};

class ITargetPlatformHandler
{
public:
	virtual ~ITargetPlatformHandler() { }
	virtual bool Cook(IObject *Object) = 0;
	virtual ECookedPlatform GetPlatformType() = 0;
};


