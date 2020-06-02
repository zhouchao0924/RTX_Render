
#include "FileStream.h"

FileReader::FileReader(const char *filename)
{
	isLoading = true;
	_reader.open(filename, std::ios_base::in | std::ios_base::binary);
}

FileReader::~FileReader()
{
	if (_reader.is_open())
	{
		_reader.close();
	}
}

void FileReader::Serialize(void *pData, size_t szData)
{
	if (_reader.is_open())
	{
		_reader.read((char *)pData, szData);
	}
}

void FileReader::Seek(size_t Pos)
{
	if (_reader.is_open())
	{
		_reader.seekg(Pos);
	}
}

size_t FileReader::Tell()
{
	if (_reader.is_open())
	{
		return (size_t)_reader.tellg();
	}
	return 0;
}

void FileReader::Close()
{
	if (_reader.is_open())
	{
		_reader.close();
	}
	delete this;
}

//////////////////////////////////////////////////////////////////////////
FileWriter::FileWriter(const char *filename)
{
	isSaving = true;
	_writer.open(filename, std::ios_base::out | std::ios_base::binary);
}

FileWriter::~FileWriter()
{
	if (_writer.is_open())
	{
		_writer.close();
	}
}

void FileWriter::Serialize(void *pData, size_t szData)
{
	if (_writer.is_open())
	{
		_writer.write((char *)pData, szData);
	}
}

void FileWriter::Seek(size_t Pos)
{
	if (_writer.is_open())
	{
		_writer.seekp(Pos);
	}
}

size_t FileWriter::Tell()
{
	if (_writer.is_open())
	{
		return (size_t)_writer.tellp();
	}
	return 0;
}

void FileWriter::Close()
{
	if (_writer.is_open())
	{
		_writer.close();
	}
	delete this;
}



