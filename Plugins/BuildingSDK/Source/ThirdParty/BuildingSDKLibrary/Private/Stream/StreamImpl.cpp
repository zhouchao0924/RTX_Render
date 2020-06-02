
#include "StreamImpl.h"

static std::string Const_ChunkMask = "!2j1";

StreamImpl::StreamImpl()
	:isSaving(false)
	,isLoading(false)
{
}

void StreamImpl::Skip(size_t szData)
{
	size_t pos = Tell() + szData;
	Seek(pos);
}

void StreamImpl::WriteChunk(int ChunkID)
{
	if (isSaving)
	{
		size_t sPos = Tell();

		*this << Const_ChunkMask;

		size_t endMaskPos = Tell();
		Serialize(&ChunkID, sizeof(ChunkID));

		size_t chunkPos = Tell();
		chunkStack.push_back(chunkPos);

		int szChunk = 0;
		Serialize(&szChunk, sizeof(szChunk));
	}
}

int StreamImpl::ReadChunk()
{
	int chunkID = -1;

	if (isLoading)
	{
		size_t pos = Tell();
			
		std::string mask;
		*this << mask;
		size_t endMaskPos = Tell();

		if (mask.compare(Const_ChunkMask) == 0)
		{
			Serialize(&chunkID, sizeof(chunkID));
			size_t chunkPos = Tell();
			chunkStack.push_back(chunkPos);
			Skip(sizeof(int)); //Chunk Size
		}
		else
		{
			Seek(pos);
		}
	}

	return chunkID;
}

void StreamImpl::SkipChunk()
{
	if (isLoading)
	{
		size_t pos = Tell();
		int szData = 0;
		Serialize(&szData, sizeof(szData));

		int szSkip = szData - sizeof(szData);
		if (szSkip > 0)
		{
			Skip(szSkip); //jump to chunk end
		}			
	}
}

void StreamImpl::EndChunk(int ChunkID)
{
	if (isSaving)
	{
		size_t chunkPos = chunkStack.back();
		chunkStack.pop_back();

		size_t endPos = Tell();
		int szData = int(endPos - chunkPos);

		Seek(chunkPos);
		Serialize(&szData, sizeof(szData));

		Seek(endPos);  // save chunk size and save chunk pos
	}
	else if (isLoading)
	{
		if (ChunkID > 0)
		{
			size_t pos = chunkStack.back();
			chunkStack.pop_back();

			Seek(pos);
			int szData = 0;
			Serialize(&szData, sizeof(szData));

			int szSkip = szData - sizeof(szData);
			if (szSkip > 0)
			{
				Skip(szSkip); //jump to chunk end
			}
		}
	}
}

/*
ChunkMask	%`&-!j;|^/@093?2d-}
ChunkID
Chunk Size  here-->|end
XXXXXX
XXXXXX
XXXXXX
----------------------------
*/


