
#pragma once

#include <unordered_map>

typedef std::unordered_map<int, ObjectDesc*>	ClassMap;

class FClassLibaray
{
public:
	void InitClassLibaray();
	void RegisterClass(ObjectDesc *ObjectClass);
	Object *CreateObject(int ObjectType);
protected:
	ClassMap	_ClassMap;
};


