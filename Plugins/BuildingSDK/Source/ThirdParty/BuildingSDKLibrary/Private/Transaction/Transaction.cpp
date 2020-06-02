
#include "assert.h"
#include <strstream>
#include "Transaction.h"
#include "IBuildingSDK.h"

FAtomUnit::FAtomUnit(int InIndex, class FTransaction *InTransaction)
{ 
	Index = InIndex; 
	Transaction = InTransaction;
}

FAtomUnit::~FAtomUnit()
{
	for (size_t i = 0; i < Tasks.size(); ++i)
	{
		IAtomTask *pTask = Tasks[i];
		if (pTask)
		{
			delete pTask;
		}
	}
	Tasks.clear();
}

bool FAtomUnit::TestDo()
{
	for (size_t i = 0; i < Tasks.size(); ++i)
	{
		IAtomTask *pTask = Tasks[i];
		if (!pTask || !pTask->TestDo())
		{
			return false;
		}
	}
	return true;
}

bool FAtomUnit::TestUnDo()
{
	for (size_t i = 0; i < Tasks.size(); ++i)
	{
		IAtomTask *pTask = Tasks[i];
		if (!pTask || !pTask->TestUnDo())
		{
			return false;
		}
	}
	return true;
}

void FAtomUnit::Do()
{
	for (size_t i = 0; i < Tasks.size(); ++i)
	{
		IAtomTask *pTask = Tasks[i];
		if (pTask)
		{
			Transaction->_Do(pTask);
		}
	}
}

void FAtomUnit::UnDo()
{
	for (size_t i = 0; i < Tasks.size(); ++i)
	{
		IAtomTask *pTask = Tasks[i];
		if (pTask)
		{
			Transaction->_UnDo(pTask);
		}
	}
}

void FAtomUnit::AddTask(IAtomTask *Task)
{
	if (Task)
	{
		Tasks.push_back(Task);
	}
}

void FAtomUnit::Serialize(ISerialize &Ar)
{
	if (Ar.IsLoading())
	{
		Ar << Index;

		int n = 0;
		Ar << n;
		Tasks.resize(n);

		for (int i = 0; i < n; ++i)
		{
			int sysIndex = -1;
			Ar << sysIndex;
			ISubTransactionSystem *pSys = Transaction->_GetSystem(sysIndex);
			assert(pSys);
			IAtomTask *pTask = nullptr;
			pSys->Serialize(pTask, Ar);
			assert(pTask);
			Tasks[i] = pTask;
		}
	}
	else if (Ar.IsSaving())
	{
		Ar << Index;
		int n = (int)Tasks.size();
		Ar << n;

		for (int i = 0; i < n; ++i)
		{
			IAtomTask *pTask = Tasks[i];
			assert(pTask);
			ISubTransactionSystem *pSys = pTask->GetSystem();
			assert(pSys);
			int sysIndex = Transaction->_GetSystemIndex(pSys);
			Ar << sysIndex;
			pSys->Serialize(pTask, Ar);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
FTransaction::FTransaction()
	: _Unit(false)
	, _TransacIndex(0)
	, _LoadedUnit(0)
{
}

FTransaction::~FTransaction()
{
	if (_Unit)
	{
		delete _Unit;
		_Unit = nullptr;
	}
}

void FTransaction::BeginTransaction()
{
	if (!_Unit)
	{
		_Unit = new FAtomUnit(_TransacIndex++, this);
		for (size_t i = 0; i < _SubSystems.size(); ++i)
		{
			ISubTransactionSystem *SubSystem = _SubSystems[i];
			if (SubSystem)
			{
				SubSystem->OnBeginTransaction(this);
			}
		}
	}
}

void FTransaction::EndTransaction()
{
	if (_Unit)
	{
		for (size_t i = 0; i < _SubSystems.size(); ++i)
		{
			ISubTransactionSystem *SubSystem = _SubSystems[i];
			if (SubSystem)
			{
				SubSystem->OnEndTransaction(this);
			}
		}
		_AddUnit(_Unit);
		_FinishUnits.push_back(_Unit->Index);
		_Unit = nullptr;
	}
}

void FTransaction::AddSubSystem(ISubTransactionSystem *SubSystem)
{
	for (size_t i = 0; i < _SubSystems.size(); ++i)
	{
		if (_SubSystems[i] == SubSystem)
		{
			return;
		}
	}
	_SubSystems.push_back(SubSystem);
}

void FTransaction::RemoveSubSystem(ISubTransactionSystem *SubSystem)
{
	for (size_t i = 0; i < _SubSystems.size(); ++i)
	{
		if (_SubSystems[i] == SubSystem)
		{
			_SubSystems.erase(_SubSystems.begin() + i);
		}
	}
}

void FTransaction::Clear()
{
	for (std::unordered_map<int, FAtomUnit *>::iterator it = _TaskMap.begin(); it != _TaskMap.end(); ++it)
	{
		FAtomUnit *pUnit = it->second;
		if (pUnit)
		{
			delete pUnit;
		}
	}
	
	_LoadedUnit = 0;
	_TaskMap.clear();
	_FinishUnits.clear();
	_RevokeUnits.clear();
}

void FTransaction::PushTask(IAtomTask *Task)
{
	if (Task)
	{
		if (_Unit)
		{
			_Unit->AddTask(Task);
		}
		else
		{
			delete Task;
		}

		for (size_t i = 0 ; i < _Listeners.size(); ++i)
		{
			if (_Listeners[i])
			{
				_Listeners[i]->AfterExecDo(Task);
			}
		}

		_ClearRedo();
	}
}

void FTransaction::_ClearRedo()
{
	for (size_t i = 0; i < _RevokeUnits.size(); ++i)
	{
		int index = _RevokeUnits[i];
		_RemoveUnit(index);
	}
	_RevokeUnits.clear();
}

void FTransaction::AddListener(ITransactionListener *Listener)
{
	if (Listener)
	{
		size_t i = 0;
		for (; i < _Listeners.size() && Listener != _Listeners[i]; ++i);

		if (i >= _Listeners.size())
		{
			_Listeners.push_back(Listener);
		}
	}
}

void FTransaction::RemoveListener(ITransactionListener *Listener)
{
	if (Listener && !_Listeners.empty())
	{
		size_t i = 0;
		for (; i < _Listeners.size() && Listener != _Listeners[i]; ++i);

		if (i < _Listeners.size())
		{
			_Listeners.erase(_Listeners.begin() + i);
		}
	}
}

void FTransaction::_Do(IAtomTask *Atom)
{
	if (Atom)
	{
		Atom->Do();

		for (size_t i = 0; i < _Listeners.size(); ++i)
		{
			if (_Listeners[i])
			{
				_Listeners[i]->AfterExecDo(Atom);
			}
		}
	}
}

void FTransaction::_UnDo(IAtomTask *Atom)
{
	if (Atom)
	{
		Atom->UnDo();
		for (size_t i = 0; i < _Listeners.size(); ++i)
		{
			if (_Listeners[i])
			{
				_Listeners[i]->AfterExecDo(Atom);
			}
		}
	}
}

void FTransaction::Redo()
{
	while (!_RevokeUnits.empty())
	{
		int index = _RevokeUnits.back();
		_RevokeUnits.pop_back();

		FAtomUnit *Unit = GetUnit(index);
		if (Unit)
		{
			if (Unit->TestDo())
			{
				_FinishUnits.push_back(index);
				Unit->Do();
				break;
			}
			else
			{
				_RemoveUnit(index);
			}
		}
	}
}

void FTransaction::Undo()
{
	while (!_FinishUnits.empty())
	{
		int index = _FinishUnits.back();
		_FinishUnits.pop_back();

		FAtomUnit *Unit = GetUnit(index);
		if (Unit)
		{
			if (Unit->TestUnDo())
			{
				_RevokeUnits.push_back(index);
				Unit->UnDo();
				break;
			}
		}
		else
		{
			_RemoveUnit(index);
		}
	}
}

int FTransaction::GetRedoCount()
{
	return (int)_RevokeUnits.size();
}

int FTransaction::GetUndoCount()
{
	return (int)_FinishUnits.size();
}

ISubTransactionSystem *FTransaction::_GetSystem(int index)
{
	if (index >= 0 && index < _SubSystems.size())
	{
		return _SubSystems[index];
	}
	return nullptr;
}

int FTransaction::_GetSystemIndex(ISubTransactionSystem *pSys)
{
	int foundIndex = -1;
	for (size_t i = 0; i < _SubSystems.size(); ++i)
	{
		if (_SubSystems[i] == pSys)
		{
			foundIndex = (int)i;
			break;
		}
	}
	return foundIndex;
}

std::string FTransaction::_GetFilename(int Index)
{
	char temp[512] = { 0 };
	sprintf_s(temp, "transaction/%d.tsk", Index);
	std::string filename = temp;
	return filename;
}

void FTransaction::_Save(FAtomUnit *Unit)
{
	std::string filename = _GetFilename(Unit->Index);
	ISerialize  *saver = GetBuildingSDK()->CreateFileWriter(filename.c_str());
	if (saver)
	{
		Unit->Serialize(*saver);
		saver->Close();
	}
}

FAtomUnit *FTransaction::_Load(int Index)
{
	FAtomUnit *unit = nullptr;
	std::string filename = _GetFilename(Index);
	ISerialize  *loader = GetBuildingSDK()->CreateFileReader(filename.c_str());
	if (loader)
	{
		unit = new FAtomUnit(Index, this);
		unit->Serialize(*loader);
		loader->Close();
	}
	return unit;
}

FAtomUnit *FTransaction::GetUnit(int index)
{
	FAtomUnit *foundUnit = nullptr;
	std::unordered_map<int, FAtomUnit *>::iterator it = _TaskMap.find(index);
	if (it != _TaskMap.end())
	{
		FAtomUnit *pUnit = it->second;
		if (!pUnit)
		{
			pUnit = _Load(index);
			_SetUnit(index, pUnit);
		}
		foundUnit = pUnit;
	}
	return foundUnit;
}


void FTransaction::_AddUnit(FAtomUnit *Unit)
{
	if (Unit)
	{
		assert(_TaskMap.find(Unit->Index) == _TaskMap.end());
		_TaskMap[Unit->Index] = Unit;
		_OnLoadUnit();
	}
}

void FTransaction::_SetUnit(int index, FAtomUnit *Unit)
{
	std::unordered_map<int, FAtomUnit *>::iterator it = _TaskMap.find(index);
	if (it != _TaskMap.end())
	{
		FAtomUnit *oldValue = it->second;
		if (oldValue != Unit)
		{
			_TaskMap[index] = Unit;

			if (oldValue)
			{
				_Save(oldValue);
				delete oldValue;
			}

			if (oldValue && !Unit)
			{
				_LoadedUnit--;
			}
			else if (!oldValue && Unit)
			{
				_OnLoadUnit();
			}
		}
	}
}

void FTransaction::_RemoveUnit(int index)
{
	std::unordered_map<int, FAtomUnit *>::iterator it =  _TaskMap.find(index);
	if (it != _TaskMap.end())
	{
		FAtomUnit *pUnit = it->second;
		if (pUnit)
		{
			_LoadedUnit--;
			delete pUnit;
		}
		_TaskMap.erase(it);
	}
}

void FTransaction::_OnLoadUnit()
{
	static const int min_unit = 2;
	static const int max_unit = 5;

	_LoadedUnit++;

	if (_LoadedUnit > max_unit)
	{
		int nRedo = _RevokeUnits.size();
		int nUndo = _FinishUnits.size();

		int half_min = min_unit / 2;

		if ( nRedo > half_min)
		{
			int nRemove = nRedo - half_min;
			for (int i = 0; i < nRemove; ++i)
			{
				int index = _RevokeUnits[i];
				_SetUnit(index, nullptr);
			}
		}

		if (nUndo > half_min)
		{
			int nRemove = nUndo - half_min;
			for (int i = 0; i < nRemove; ++i)
			{
				int index = _FinishUnits[i];
				_SetUnit(index, nullptr);
			}
		}
	}
}

