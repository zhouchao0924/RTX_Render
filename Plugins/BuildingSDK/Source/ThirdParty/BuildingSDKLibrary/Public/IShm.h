
#pragma once

#include "IProperty.h"
#include <functional>

typedef std::function<void(int MsgID, IValue *Value)> EventHandler;  //可以注册的事件类型

class IShmSender
{
public:
	virtual ~IShmSender() {}
	virtual void Send(int MsgID, IValue *Value) = 0;
	virtual void Close() = 0;
};

class IShmReceiver
{
public:
	virtual ~IShmReceiver() {}
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void PeekMSG() = 0;
	virtual void RegisterHandler(const EventHandler &Handler) = 0;
	virtual void Close() = 0;
};

class IShm
{
public:
	virtual ~IShm() {}
	virtual void Tick(float DeltaSecond) = 0;
	virtual IShmSender *CreateSender(const char *mapname) = 0;
	virtual IShmReceiver *CreateReceiver(const char *mapname) = 0;
};


