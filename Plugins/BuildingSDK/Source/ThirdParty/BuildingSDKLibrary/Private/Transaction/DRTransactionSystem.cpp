
#include "assert.h"
#include "ISuite.h"
#include "Stream/MemStream.h"
#include "DRTransactionSystem.h"

class FSuiteDataTask :public IAtomTask
{
public:
	FSuiteDataTask(std::shared_ptr<ISuite *> &suite, std::vector<char> &InRedoData, std::vector<char> &InUndoData, FDRTransactionSystem *System);
	FSuiteDataTask(std::shared_ptr<ISuite *> &suite, FDRTransactionSystem *System);
	bool TestDo();
	bool TestUnDo();
	void Do();
	void UnDo();
	ISuite *GetSuite();
	ISubTransactionSystem *GetSystem();
	void Serialize(ISerialize &Ar);
protected:
	std::vector<char> UndoData;
	std::vector<char> RedoData;
	std::weak_ptr<ISuite *> suite_ptr;
	FDRTransactionSystem *_System;
};

FSuiteDataTask::FSuiteDataTask(std::shared_ptr<ISuite *> &suite, FDRTransactionSystem *System)
	:suite_ptr(suite)
	,_System(System)
{
}

FSuiteDataTask::FSuiteDataTask(std::shared_ptr<ISuite *> &suite, std::vector<char> &InRedoData, std::vector<char> &InUndoData, FDRTransactionSystem *System)
	: FSuiteDataTask(suite, System)
{
	RedoData.swap(InRedoData);
	UndoData.swap(InUndoData);
}

ISuite *FSuiteDataTask::GetSuite()
{
	return *suite_ptr.lock().get();
}

bool FSuiteDataTask::TestDo()
{
	return !suite_ptr.expired();
}

bool FSuiteDataTask::TestUnDo()
{
	return !suite_ptr.expired();
}

void FSuiteDataTask::Do()
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		MemReader mem(RedoData);
		Suite->Serialize(mem);
	}
}

void FSuiteDataTask::UnDo()
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		MemReader mem(UndoData);
		Suite->Serialize(mem);
	}
}

ISubTransactionSystem *FSuiteDataTask::GetSystem()
{
	return _System;
}

void FSuiteDataTask::Serialize(ISerialize &Ar)
{
	if (Ar.IsLoading())
	{
		int n = 0;
		Ar << n;
		UndoData.resize(n);
		if (n > 0)
		{
			Ar.Serialize(&UndoData[0], n);
		}

		Ar << n;
		RedoData.resize(n);
		if (n > 0)
		{
			Ar.Serialize(&RedoData[0], n);
		}
	}
	else if (Ar.IsSaving())
	{
		int n = (int)UndoData.size();
		Ar << n;
		Ar.Serialize(&UndoData[0], n);

		n = (int)RedoData.size();
		Ar << n;
		Ar.Serialize(&RedoData[0], n);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
FDRTransactionSystem::FDRTransactionSystem(ISuite *InSuite)
	:Suite(InSuite)
{
	suite_ptr = std::make_shared<ISuite *>(InSuite);
}

void FDRTransactionSystem ::OnBeginTransaction(class ITransact *Transaction)
{
	if (Suite)
	{
		UndoData.clear();
		RedoData.clear();
		MemWriter mem(UndoData);
		Suite->Serialize(mem);
	}
}

void FDRTransactionSystem::OnEndTransaction(class ITransact *Transaction)
{
	if (Suite)
	{
		MemWriter mem(RedoData);
		Suite->Serialize(mem);
		FSuiteDataTask *pTask = new FSuiteDataTask(suite_ptr, RedoData, UndoData, this);
		Transaction->PushTask(pTask);
	}
}

void FDRTransactionSystem::Serialize(IAtomTask *&pTask, ISerialize &Ar)
{
	if (Ar.IsSaving())
	{
		assert(pTask);
		FSuiteDataTask *pSuiteTask = (FSuiteDataTask *)pTask;
		pSuiteTask->Serialize(Ar);
	}
	else if(Ar.IsLoading())
	{
		FSuiteDataTask *pSuiteTask = new FSuiteDataTask(suite_ptr, this);
		pSuiteTask->Serialize(Ar);
		pTask = pSuiteTask;
	}
}

