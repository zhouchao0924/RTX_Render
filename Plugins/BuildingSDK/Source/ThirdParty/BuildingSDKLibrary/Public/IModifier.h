
#pragma once

class IObject;
class IModifier
{
public:
	virtual void OnWeak(IObject *pObj) = 0;
	virtual void OnSleep(IObject *pObj) = 0;
	virtual bool BuildMesh(IObject *pObj) = 0;
	virtual void Destroy() = 0;
};

class IModifierStack
{
public:
	virtual bool BuildMesh() = 0;
	virtual int  GetModifierCount() = 0;
	virtual bool IsEnable(IModifier *Modifier) = 0;
	virtual void SetEnable(IModifier *Modifier, bool bEnable) = 0;
	virtual void Push(IModifier *Modifier, bool bEnable = true) = 0;
	virtual void Delete(IModifier *Modifier) = 0;
	virtual void Clear() = 0;
};

