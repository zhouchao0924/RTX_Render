
#include "Protocal.h"
#include "DRGameMode.h"
#include "AsyncTask/ResAsyncTaskMgr.h"
#include "EditorGameInstance.h"

UProtocalImpl::UProtocalImpl(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
	,MyGame(NULL)
{
}

UProtocalImpl *UProtocalImpl::GetProtocal(UObject *Obj)
{
	return NULL;
}

void UProtocalImpl::CallProtocal(UVaRestJsonObject * JsonObj, FProtocalDelegate Delegate)
{
	FDRAsyncTaskManager::Get().ExecuteTask<FProtocalTask>(new FAsyncTask<FProtocalTask>(nullptr, JsonObj, Delegate));
}

