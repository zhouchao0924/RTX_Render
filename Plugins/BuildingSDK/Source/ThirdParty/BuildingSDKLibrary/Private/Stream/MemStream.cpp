
#include "MemStream.h"
#include "assert.h"

MemReader::MemReader(std::vector<char> &InData)
{
	pos = 0;
	isLoading = true;
	m_data = &InData;
}

void MemReader::Serialize(void *pData, size_t szData)
{
	size_t n = m_data->size();
	int szNeed = pos + szData;
	if (szNeed < n)
	{
		memcpy(pData, &(*m_data)[pos], szData);
		pos += szData;
	}
}

void MemReader::Seek(size_t InPos)
{
	assert(InPos < m_data->size());
	pos = InPos;
}

//////////////////////////////////////////////////////////////////////////
MemWriter::MemWriter(std::vector<char> &InData)
{
	pos = 0;
	isSaving = true;
	m_data = &InData;
}

void MemWriter::Serialize(void *pData, size_t szData)
{
	size_t n = m_data->size();
	int szNeed = pos + szData;
	if ((szNeed +1) > n)
	{
		m_data->resize(szNeed + 1);
	}
	memcpy(&(*m_data)[pos], pData, szData);
	pos += szData;
}

void MemWriter::Seek(size_t InPos)
{
	if ((InPos + 1)> m_data->size())
	{
		m_data->resize(InPos + 1);
	}
	pos = InPos;
}

