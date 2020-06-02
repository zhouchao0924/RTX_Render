
#pragma once

#include <vector>
#include "ISerialize.h"

class StreamImpl :public ISerialize
{
public:
	StreamImpl();
	void Skip(size_t szData);
	bool IsSaving() { return isSaving; }
	bool IsLoading() { return isLoading; }
	int  ReadChunk();
	void WriteChunk(int ChunkID);
	void EndChunk(int ChunkID);
	void SkipChunk();
protected:
	bool				isLoading;
	bool				isSaving;
	std::vector<size_t> chunkStack;
};



