
#pragma once

#include <fstream>
#include "StreamImpl.h"

class FileReader :public StreamImpl
{
public:
	FileReader(const char *filename);
	~FileReader();
	virtual void Serialize(void *pData, size_t szData);
	virtual void Seek(size_t Pos);
	virtual size_t Tell();
	virtual void Close();
protected:
	std::fstream  _reader;
};

class FileWriter :public StreamImpl
{
public:
	FileWriter(const char *filename);
	~FileWriter();
	virtual void Serialize(void *pData, size_t SzData);
	virtual void Seek(size_t Pos); 
	virtual size_t Tell();
	virtual void Close();
protected:
	std::fstream _writer;
};


