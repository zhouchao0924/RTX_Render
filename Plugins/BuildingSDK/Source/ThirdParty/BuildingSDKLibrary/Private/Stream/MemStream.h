
#pragma once

#include "StreamImpl.h"

class MemReader :public StreamImpl
{
public:
	MemReader(std::vector<char> &InData);
	void Serialize(void *pData, size_t szData);
	void Seek(size_t Pos);
	size_t Tell() { return pos; }
	void Close() {}
protected:
	size_t pos;
	std::vector<char> *m_data;
};


class MemWriter :public StreamImpl
{
public:
	MemWriter(std::vector<char> &InData);
	virtual void Serialize(void *pData, size_t szData);
	virtual void Seek(size_t Pos);
	virtual size_t Tell() { return pos; }
	virtual void Close() { }
protected:
	size_t pos;
	std::vector<char> *m_data;
};


