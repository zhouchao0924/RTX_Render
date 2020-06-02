#pragma once

#include "DRChunkID.h"
#include "ISerialize.h"

inline ISerialize & operator << (ISerialize &Ar, FVector &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, FTransform &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, FRotator &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, FVector2D &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, FLinearColor &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

ISerialize & operator << (ISerialize &Ar, FText &v);
ISerialize & operator << (ISerialize &Ar, FString &v);

template<typename K, typename V>
void SerializeMap(ISerialize &Ar, TMap<K, V> &Map, int32 SpChunkID = 0)
{
	if (Ar.IsLoading())
	{
		if (SpChunkID > 0)
		{
			int ChunkID = Ar.ReadChunk();
			if (ChunkID != SpChunkID)
			{
				return;
			}
		}
		K key; V value;
		int Size = 0;
		Ar << Size;
		for (int i = 0; i < Size; ++i)
		{
			Ar << key;
			Ar << value;
			Map.Add(key, value);
		}
	}
	else if (Ar.IsSaving())
	{
		if (SpChunkID > 0)
		{
			Ar.WriteChunk(SpChunkID);
		}

		int Size = Map.Num();
		Ar << Size;
		for (TMap<K, V>::TIterator It(Map); It; ++It)
		{
			K &key = It.Key();
			V &value = It.Value();
			Ar << key;
			Ar << value;
		}
	}
}

template<typename V>
void SerializeArray(ISerialize &Ar, TArray<V> &Array, int32 SpChunkID = 0)
{
	if (Ar.IsLoading())
	{
		if (SpChunkID > 0)
		{
			int ChunkID = Ar.ReadChunk();
			if (ChunkID != SpChunkID)
			{
				return;
			}
		}

		int Size = 0;
		Ar << Size;
		Array.SetNum(Size);
		for (int i = 0; i < Size; ++i)
		{
			V &value = Array[i];
			Ar << value;
		}
		if (SpChunkID > 0)
		{
			Ar.EndChunk(SpChunkID);
		}
	}
	else if (Ar.IsSaving())
	{
		if (SpChunkID > 0)
		{
			Ar.WriteChunk(SpChunkID);
		}
		int Size = Array.Num();
		Ar << Size;
		for (int i = 0; i < Size; ++i)
		{
			Ar << Array[i];
		}
		if (SpChunkID > 0)
		{
			Ar.EndChunk(SpChunkID);
		}
	}
}

template<typename V>
void SerializeStructArray(ISerialize &Ar, TArray<V> &Array, int32 Version, int32 SpChunkID = 0)
{
	if (Ar.IsLoading())
	{
		if (SpChunkID > 0)
		{
			int ChunkID = Ar.ReadChunk();
			if (ChunkID != SpChunkID)
			{
				return;
			}
		}

		int Size = 0;
		Ar << Size;
		Array.SetNum(Size);
		for (int i = 0; i < Size; ++i)
		{
			V &value = Array[i];
			value.SerializeDataByVersion(Ar, Version);
		}

		if (SpChunkID > 0)
		{
			Ar.EndChunk(SpChunkID);
		}
	}
	else if (Ar.IsSaving())
	{
		if (SpChunkID > 0)
		{
			Ar.WriteChunk(SpChunkID);
		}
		int Size = Array.Num();
		Ar << Size;
		for (int i = 0; i < Size; ++i)
		{
			V &value = Array[i];
			value.SerializeDataByVersion(Ar, Version);
		}
		if (SpChunkID > 0)
		{
			Ar.EndChunk(SpChunkID);
		}
	}
}


