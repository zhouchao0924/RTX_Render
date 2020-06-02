
#pragma once

#include <memory>
#include "ITransaction.h"

class FDRTransactionSystem :public ISubTransactionSystem
{
public:
	FDRTransactionSystem(class ISuite *InSuite);
	void OnBeginTransaction(class ITransact *Transaction) override;
	void OnEndTransaction(class ITransact *Transaction) override;
	void Serialize(IAtomTask *&pTask, ISerialize &Ar) override;
protected:
	ISuite						*Suite;
	std::shared_ptr<ISuite *>	suite_ptr;
	std::vector<char>			UndoData;
	std::vector<char>			RedoData;
};


