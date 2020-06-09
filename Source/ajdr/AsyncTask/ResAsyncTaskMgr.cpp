
#include "ResAsyncTaskMgr.h"
#include "Download/HttpDownloadMgr.h"

FDRAsyncTaskManager &FDRAsyncTaskManager::Get()
{
	static FDRAsyncTaskManager GDRAsyncTaskMgr;
	return GDRAsyncTaskMgr;
}

void FDRAsyncTaskManager::Tick(float DeltaTime)
{
	TickTasks<FImportAsyncTask>(QueueImportTasks);

	TickTasks<FResLoadAsyncTask>(QueueLoadTasks);

	TickTasks<FUploadAsyncTask>(QueueUploadTasks);

	TickTasks<FCookAsyncTask>(QueueCookTasks);

	TickTasks<FProtocalTask>(ProtocalTasks);

	TickTasks<FDownloadAsyncTask>(DownloadTasks);
}

void FDRAsyncTaskManager::Clear()
{
	ClearTasks<FImportAsyncTask>(QueueImportTasks);
	
	ClearTasks<FResLoadAsyncTask>(QueueLoadTasks);

	ClearTasks<FUploadAsyncTask>(QueueUploadTasks);

	ClearTasks<FCookAsyncTask>(QueueCookTasks);

	ClearTasks<FProtocalTask>(ProtocalTasks);

	ClearDownload();
}

void FDRAsyncTaskManager::ClearDownload()
{
	for (int i = 0; i < DownloadTasks.Num(); ++i) 
	{
		FAsyncTask<FDownloadAsyncTask> *pTask = DownloadTasks[i];
		FDownloadAsyncTask &RawTask = pTask->GetTask();
		RawTask.Stop();
	}

	ClearTasks<FDownloadAsyncTask>(DownloadTasks);
	FHttpDownloadMgr *Mgr = FHttpDownloadMgr::GetDownloadMgr();
	Mgr->ClearMgr();
}

