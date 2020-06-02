

#pragma once

#include "IClass.h"

class ISuChannel
{
public:
	virtual const char *GetName() = 0;
};

class IRemoteProxy
{
public:
	virtual int GetChannelCount() = 0;
	virtual ISuChannel *GetChannel(int ChannelIndex) = 0;
};

class IRemoteService
{
public:
	virtual void Startup() = 0;
	virtual void Shutdown() = 0;
	virtual int GetProxyCount() = 0;
	virtual IRemoteProxy *GetProxy(int Index) = 0;
};




