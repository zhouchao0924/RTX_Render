
#pragma once

#include "IClass.h"
#include "IBuildingResourceMgr.h"

class WLink
{
public:
	WLink()
		:_Ptr(nullptr)
	{
	}

	WLink(IObject *pObj)
	{
		if (pObj)
		{
			_Ptr = pObj->GetWatcher();
		}
		else
		{
			_Ptr = nullptr;
		}
	}

	WLink & operator = (IObject *pObj)
	{
		if (_Ptr)
		{
			_Ptr->Destroy();
			_Ptr = nullptr;
		}

		if (pObj)
		{
			_Ptr = pObj->GetWatcher();
		}

		return *this;
	}

	WLink & operator = (WLink &Oth)
	{
		if (_Ptr)
		{
			_Ptr->Destroy();
			_Ptr = nullptr;
		}

		_Ptr = Oth._Ptr;
		Oth._Ptr = nullptr;
		
		return *this;
	}

	WLink(WLink &Oth)
	{
		_Ptr = Oth._Ptr;
		Oth._Ptr = nullptr;
	}

	~WLink()
	{
		if (_Ptr)
		{
			_Ptr->Destroy();
			_Ptr = nullptr;
		}
	}

	IObject * operator ->()
	{
		if (_Ptr)
		{
			return _Ptr->Get();
		}
		return nullptr;
	}
	
	bool Equals(IObject *pObj) const
	{
		return Get()==pObj;
	}

	bool Equals(ObjectID ID) const
	{
		IObject *pObj = Get();
		return (pObj && pObj->GetID() == ID) || (!pObj && ID==INVALID_OBJID);
	}

	bool Equals(const WLink &Oth) const
	{
		return _Ptr && Oth._Ptr && _Ptr == Oth._Ptr;
	}

	bool IsReady()
	{
		IObject *pObj = Get();
		IResource *pResource = pObj ? pObj->GetResource() : nullptr;
		return (pObj && !pResource) || (pResource && pResource->GetState() == EResLoaded);
	}

	ObjectID GetID()
	{
		IObject *pObj = Get();
		return pObj ? pObj->GetID() : INVALID_OBJID;
	}

	IObject *Get()
	{
		return _Ptr ? _Ptr->Get() : nullptr;
	}

	IObject *Get() const
	{
		return _Ptr ? _Ptr->Get() : nullptr;
	}

	void Link(IObject *pObj)
	{
		if (pObj && _Ptr)
		{
			_Ptr->Link(pObj);
		}
	}
	
	bool SetWatcher(FuncChangedCallback Func)
	{
		if (_Ptr)
		{
			_Ptr->SetWatcher(Func);
			return true;
		}
		return false;
	}

	bool IsValid()
	{
		return _Ptr!=nullptr;
	}
protected:
	IWatcherPtr *_Ptr;
};

#define CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define CALLBACK_4(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)
#define CALLBACK_5(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, ##__VA_ARGS__)


