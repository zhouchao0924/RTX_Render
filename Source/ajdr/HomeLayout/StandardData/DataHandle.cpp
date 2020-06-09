

#include "DataHandle.h"


FDataHandle::FDataHandle()
{
	ID = FGuid::NewGuid().ToString().ToLower();

	bValid = true;
}