
#pragma once

class IUnRedoTask
{
public:
	virtual ~IUnRedoTask() {}
	virtual bool  TestDo() = 0;
	virtual bool  TestUnDo() = 0;
	virtual void  Do() = 0;
	virtual void  UnDo() = 0;
	virtual class ISubUndoRedoSystem *GetSystem() = 0;
};

class IUndoRedoListener
{
public:
	virtual ~IUndoRedoListener() {}
	virtual void AfterExecDo(IUnRedoTask *Task) = 0;
	virtual void AfterExecUnDo(IUnRedoTask *Task) = 0;
};

class ISubUndoRedoSystem
{
public:
	virtual ~ISubUndoRedoSystem() {}
	virtual void OnBeginTransaction(class IUndoRedo *Transaction) = 0;
	virtual void OnEndTransaction(class IUndoRedo *Transaction) = 0;
	virtual void Serialize(IUnRedoTask *&pTask, class ISerialize &Ar) = 0;
};

class IUndoRedo
{
public:
	virtual ~IUndoRedo() {}
	virtual void BeginTransaction() = 0;
	virtual void EndTransaction() = 0;
	virtual void Clear() = 0;
	virtual void Redo() = 0;
	virtual void Undo() = 0;
	virtual int  GetRedoCount() = 0;
	virtual int  GetUndoCount() = 0;
	virtual void PushTask(IUnRedoTask *Task) = 0;
	virtual void AddListener(IUndoRedoListener *Listener) = 0;
	virtual void RemoveListener(IUndoRedoListener *Listener) = 0;
	virtual void AddSubSystem(ISubUndoRedoSystem *SubSystem) = 0;
	virtual void RemoveSubSystem(ISubUndoRedoSystem *SubSystem) = 0 ;
};



