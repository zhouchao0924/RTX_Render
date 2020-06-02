
#ifndef __AITRANSACTION_H__
#define __AITRANSACTION_H__

#include <vector>
#include <unordered_map>
#include "ITransaction.h"

class FAtomUnit :public IAtomTask
{
public:
	FAtomUnit(int InIndex, class FTransaction *InTransaction);
	~FAtomUnit();
	bool TestDo();
	bool TestUnDo();
	void Do();
	void UnDo();
	void AddTask(IAtomTask *Task);
	void Serialize(ISerialize &Ar);
	class ISubTransactionSystem *GetSystem() { return nullptr; }
	friend class FTransaction;
protected:
	class FTransaction		 *Transaction;
	int						 Index;
	std::vector<IAtomTask*>  Tasks;
};

class FTransaction :public ITransact
{
public:
	FTransaction();
	~FTransaction();
	void BeginTransaction() override;
	void EndTransaction() override;
	void Clear() override;
	void Redo() override;
	void Undo() override;
	int  GetRedoCount() override;
	int  GetUndoCount() override;
	void PushTask(IAtomTask *Task) override;
	void AddListener(ITransactionListener *Listener) override;
	void RemoveListener(ITransactionListener *Listener) override;
	void AddSubSystem(ISubTransactionSystem *SubSystem) override;
	void RemoveSubSystem(ISubTransactionSystem *SubSystem) override;
protected:
	friend class FAtomUnit;
	void _Do(IAtomTask *Atom);
	void _UnDo(IAtomTask *Atom);
	void _Save(FAtomUnit *Unit);
	FAtomUnit *_Load(int Index);
	std::string _GetFilename(int Index);
	FAtomUnit *GetUnit(int index);
	void _AddUnit(FAtomUnit *Unit);
	void _RemoveUnit(int index);
	void _SetUnit(int index, FAtomUnit *Unit);
	void _OnLoadUnit();
	void _ClearRedo();
	ISubTransactionSystem *_GetSystem(int index);
	int _GetSystemIndex(ISubTransactionSystem *pSys);
protected:
	int										_TransacIndex;
	std::vector<ITransactionListener*>		_Listeners;
	FAtomUnit								*_Unit;
	std::vector<int>						_FinishUnits;  //也完成事物
	std::vector<int>						_RevokeUnits;  //已撤销事物
	std::unordered_map<int, FAtomUnit *>	_TaskMap;
	std::vector<ISubTransactionSystem*>		_SubSystems;   //支持事务的子系统
	int										_LoadedUnit;
};

#endif //__AITRANSACTION_H__


