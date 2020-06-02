// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once


template<typename InElementType>
class TDataReusedPool
{
public:
	int32 AllocSlot()
	{
		int32 SlotIndex = INDEX_NONE;
		if (UnusedData.Num() > 0)
		{
			SlotIndex = UnusedData.Pop();
		}
		else
		{
			SlotIndex = PooledDatas.AddDefaulted();
		}
		return SlotIndex;
	}

	void FreeSlot(int32 SlotIndex)
	{
		if (PooledDatas.IsValidIndex(SlotIndex))
		{
			InElementType &Info = PooledDatas[SlotIndex];
			Info = InElementType();
			UnusedData.Add(SlotIndex);
		}
	}

	const InElementType& GetSlot(int32 SlotIndex) const
	{
		if (PooledDatas.IsValidIndex(SlotIndex))
		{
			return PooledDatas[SlotIndex];
		}

		return InElementType();
	}

	InElementType& AcquireSlot(int32 SlotIndex)
	{
		if (PooledDatas.IsValidIndex(SlotIndex))
		{
			return PooledDatas[SlotIndex];
		}

		return InElementType();
	}

	void Clear()
	{
		PooledDatas.Empty();
	}


public:
	TArray<InElementType> PooledDatas;
	TArray<int32> UnusedData;
};
