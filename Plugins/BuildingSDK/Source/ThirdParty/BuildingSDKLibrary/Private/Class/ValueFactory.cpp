
#include "PValue.h"
#include "ValueFactory.h"
#include "assert.h"

IValue &ValueFactory::Create()
{
	PValue *pValue = new PValue();
	_AutoReleasePool.push_back(pValue);
	return *pValue;
}

void ValueFactory::AutoRelease()
{
	for (size_t i = 0; i < _AutoReleasePool.size(); ++i)
	{
		PValue *pValue = _AutoReleasePool[i];
		assert(pValue->_RefCount > 0);
		pValue->Release();
	}
}

void ValueFactory::DestroyValue(IValue *Value)
{
	assert(Value);
	delete Value;
}


