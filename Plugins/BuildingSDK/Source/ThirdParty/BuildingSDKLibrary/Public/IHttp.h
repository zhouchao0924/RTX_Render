
#pragma once

#include "IProperty.h"
#include <functional>

typedef	 int HttpSession;
#define  INVLID_SESSION -1

enum ESessionState
{
	Pending,
	PostRequst,
	Success,
	Timeout,
	NetFailed,
	WirteFailed,
	CheckMD5Failed
};

enum ESessionCommand
{
	SessionCreate,
	SessionClose,
	SessionUpdate,
};

class IHttp;
typedef std::function<void(HttpSession, IValue &)>	ResponseFunctor;
typedef std::function<void(HttpSession, float)>		DownloadFunctor;
typedef std::function<void(ESessionCommand, HttpSession)> MonitorFunctor;

class IHttp
{
public:
	virtual void SetMaxChunkSize(size_t szChunk) = 0;
	virtual void SetMaxActiveSession(size_t maxActiveSession) = 0;
	virtual void SetAuthorizationToken(const char *token) = 0;
	virtual HttpSession Download(const char *URL, const char *TargetFilename, DownloadFunctor functor = nullptr) = 0;
	virtual HttpSession CallURL(const char *URL, const char *Json, ResponseFunctor functor = nullptr) = 0;
	virtual void Close(HttpSession Session) = 0;
	virtual bool GetInfo(HttpSession Session, float &OutProgress, ESessionState &OutState, const char *&URL, const char *&TargetFilename) = 0;
	virtual void RegisterMonitor(MonitorFunctor functor) = 0;
};

