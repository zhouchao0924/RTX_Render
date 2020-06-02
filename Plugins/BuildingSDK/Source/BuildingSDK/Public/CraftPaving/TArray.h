#ifndef ARRAY_TEMPLATE_H

#define ARRAY_TEMPLATE_H

#include "CraftPavingMacros.h"

template<typename ElemType>
class CRAFTPAVING_API TArrayTemplate
{
public:

	// constructor / destructor
	TArrayTemplate()
		:ArrayNum(0), MaxNum(4)
	{
		Data = new ElemType[MaxNum];

		std::memset(Data, 0, MaxNum * sizeof(ElemType));
	};

	TArrayTemplate(Int32 StackNum)
	{
		assert(StackNum >= 0);

		ArrayNum = StackNum > 0 ? StackNum : 0;

		MaxNum = ArrayNum + 4;

		Data = new ElemType[MaxNum];

		std::memset(Data, 0, MaxNum * sizeof(ElemType));

	};

	TArrayTemplate(const ElemType* ElementList, Int32 Count)
	{
		assert(ElementList);
		assert(Count >= 0);

		ArrayNum = Count;

		MaxNum = ArrayNum + 4;

		Data = new ElemType[MaxNum];

		std::memset(Data, 0, MaxNum * sizeof(ElemType));

		std::copy(ElementList, ElementList + ArrayNum, Data);
	};

	TArrayTemplate(const std::initializer_list<ElemType>& ElemList)
	{
		ArrayNum = (Int32)ElemList.size();

		MaxNum = ArrayNum + 4;

		Data = new ElemType[MaxNum];

		std::memset(Data, 0, MaxNum * sizeof(ElemType));

		std::copy(ElemList.begin(), ElemList.end(), Data);
	};

	TArrayTemplate(const TArrayTemplate<ElemType>& OtherArray)
	{
		const ElemType* OtherBuf = OtherArray.RawData();

		ArrayNum = OtherArray.Num();

		MaxNum = OtherArray.Size() + 4;

		Data = new ElemType[MaxNum];

		std::memset(Data, 0, MaxNum * sizeof(ElemType));

		std::copy(OtherBuf, OtherBuf + ArrayNum, Data);
	};

	~TArrayTemplate()
	{
		if (Data)
		{
			delete[] Data;
		}

		MaxNum = ArrayNum = 0;
	};

	// override operator
	void operator=(const TArrayTemplate<ElemType>& OtherArray)
	{
		ArrayNum = OtherArray.Num();

		MaxNum = ArrayNum + 4;

		delete[] Data;

		Data = new ElemType[MaxNum];

		std::memset(Data, 0, MaxNum * sizeof(ElemType));

		std::copy(OtherArray.RawData(), OtherArray.RawData() + ArrayNum, Data);
	};

	void operator=(std::initializer_list<ElemType> InList)
	{
		delete[] Data;

		ArrayNum = InList.size();

		MaxNum = ArrayNum + 4;

		Data = new ElemType[MaxNum];

		std::memset(Data, 0, MaxNum * sizeof(ElemType));

		std::copy(InList.begin(), InList.end(), Data);
	}

	bool operator==(const TArrayTemplate<ElemType>& OtherArray) const
	{
		if (Num() != OtherArray.Num())
		{
			return false;
		}

		for (Int32 Index = 0; Index < ArrayNum; Index++)
		{
			if (Data[Index] != OtherArray[Index])
			{
				return false;
			}
		}

		return true;
	};

	CP_INLINE ElemType& operator[](Int32 Index) const
	{
		assert(Index >= 0);
		assert(ArrayNum > Index);

		return Data[Index];
	};

	// operation

	CP_INLINE const ElemType* RawData() const
	{
		return Data;
	};

	// return the used num of the array
	CP_INLINE Int32 Num() const
	{
		return ArrayNum;
	};

	// return the max num of the array
	CP_INLINE Int32 Size() const
	{
		return MaxNum;
	};

	void AddUninitialized(Int32 ExtraSlack)
	{
		MaxNum += ExtraSlack;

		ElemType* TempBuf = new ElemType[MaxNum];

		std::memset(TempBuf, 0, MaxNum * sizeof(ElemType));

		std::copy(Data, Data + ArrayNum, TempBuf);

		delete[] Data;

		Data = TempBuf;
	};

	Int32 Add(const ElemType& Element)
	{
		Int32 ElementIndex = ArrayNum;

		if (ArrayNum < MaxNum - 1)
		{
			Data[ElementIndex] = Element;
		}
		else
		{
			AddUninitialized(4);

			Data[ElementIndex] = Element;
		}

		ArrayNum++;

		return ElementIndex;
	};

	void Append(const TArrayTemplate<ElemType>& OtherArray)
	{
		Int32 ExtraSlack = OtherArray.Num() - (MaxNum - ArrayNum);

		if (ExtraSlack > 0)
		{
			ExtraSlack += 4;
			AddUninitialized(ExtraSlack);
		}

		std::copy(OtherArray.RawData(), OtherArray.RawData() + OtherArray.Num(), Data + ArrayNum);

		ArrayNum += OtherArray.Num();
	};

	void Append(const ElemType* ElementList, Int32 Count)
	{
		assert(ElementList);
		assert(Count >= 0);

		Int32 ExtraSlack = Count - (MaxNum - ArrayNum);

		if (ExtraSlack >= 0)
		{
			ExtraSlack += 4;
			AddUninitialized(ExtraSlack);
		}

		std::copy(ElementList, ElementList + Count, Data + ArrayNum);

		ArrayNum += Count;
	};

	void Remove(const ElemType& Element, Int32 Count = 1)
	{
		if (Count > 0)
		{
			Int32 ArrayIndex = 0;

			while (ArrayIndex < ArrayNum || Count < 0)
			{

				if (Data[ArrayIndex] == Element)
				{
					std::memmove(Data + ArrayIndex, Data + ArrayIndex + 1, ArrayNum - ArrayIndex - 1);

					ArrayNum--;

					Count--;

					std::memset(Data + ArrayNum, 0, (MaxNum - ArrayNum) * sizeof(ElemType));
				}
				else
				{
					ArrayIndex++;
				}
			}

			Shrink();
		}
	};

	void RemoveAt(Int32 Index, bool bAllowSharinking = true)
	{
		assert(Index >= 0);
		assert(Index < ArrayNum);

		std::memmove(Data + Index, Data + Index + 1, MaxNum - (Index + 1));

		ArrayNum--;

		std::memset(Data + ArrayNum, 0, (MaxNum - ArrayNum) * sizeof(ElemType));

		Shrink();
	};

	Int32 Find(const ElemType& Element)
	{

		for (Int32 Index = 0; Index < ArrayNum; Index++)
		{
			if (Data[Index] == Element)
			{
				return Index;
			}
		}

		return -1;
	};

	bool IsValidIndex(Int32 Index) const
	{
		if (Index >= ArrayNum || Index < 0)
		{
			return false;
		}

		ElemType* DirtyPtr = Data + Index;

		return DirtyPtr != nullptr;
	};

	Int32 LastIndex()
	{
		return ArrayNum - 1;
	};

	void Reset(Int32 NewSize = 0)
	{
		
		if (Data)
		{
			delete[] Data;
		}

		ArrayNum = 0;

		MaxNum = NewSize + 4;

		Data = new ElemType[MaxNum];

		std::memset(Data, 0, MaxNum * sizeof(ElemType));
	}

	void Reverse()
	{
		bool bHasMid = ArrayNum % 2 == 0 ? false : true;
		
		Int32 HalfSize = bHasMid ? ArrayNum / 2 + 1 : ArrayNum / 2;

		for (Int32 Index = 0; Index < HalfSize; Index++)
		{
			ElemType TempFront = Data[Index];
			ElemType TempBack = Data[ArrayNum - (Index + 1)];

			Data[Index] = TempBack;
			Data[ArrayNum - (Index + 1)] = TempFront;
		}

	};

	void Push(const ElemType& Element)
	{
		Add(Element);
	}

	ElemType Pop(bool bAllowShrinking = true)
	{
		CP_CHECK(ArrayNum > 0);
		ElemType Result = Data[ArrayNum - 1];
		RemoveAt(ArrayNum - 1, bAllowShrinking);

		return Result;
	}

	CP_FORCEINLINE ElemType& Last(Int32 TheIndexFromEnd = 0)
	{
		CP_CHECK(ArrayNum > 0);

		return Data[ArrayNum - 1 - TheIndexFromEnd];
	}

	CP_FORCEINLINE const ElemType& Last(Int32 TheIndexFromEnd = 0) const
	{
		CP_CHECK(ArrayNum > 0);

		return Data[ArrayNum - 1 - TheIndexFromEnd];
	}

	CP_FORCEINLINE ElemType& Top()
	{
		return Last();
	}

	CP_FORCEINLINE const ElemType& Top() const
	{
		return Last();
	}

private:

	void Shrink()
	{
		if (MaxNum - ArrayNum > 8)
		{
			MaxNum = ArrayNum + 8;

			ElemType* Temp = new ElemType[MaxNum];
			std::memset(Temp, 0, MaxNum);

			std::move(Data, Data + ArrayNum, Temp);

			delete[] Data;

			Data = Temp;

			Temp = nullptr;
		}
	}
	;

private:

	ElemType*	Data;

	Int32		ArrayNum;

	Int32		MaxNum;
};

#endif // !ARRAY_H