
#pragma once

#include <vector>
#include "assert.h"

template<class T>
class kArray
{
public:
	kArray()
		:Elem(nullptr)
		, Num(0)
	{
	}

	kArray(std::vector<T> &Elms)
	{
		set(Elms);
	}

	kArray(T *elem, int n)
	{
		set(elem, n);
	}

	void set(T *pElm, int n)
	{
		Num = n;
		Elem = pElm;
	}

	void set(std::vector<T> &Elms)
	{
		if (Elms.empty())
		{
			Elem = nullptr;
			Num = 0;
		}
		else
		{
			Elem = &Elms[0];
			Num = (int)Elms.size();
		}
	}

	T & operator[](int index) const
	{
		assert(index < Num);
		return Elem[index];
	}

	T & back() const
	{
		assert(Num > 0);
		return Elem[Num-1];
	}

	T * data() const { return Elem; }

	int size() const { return Num; }

	int bytes() const { return Num * sizeof(T); }

	void saveto(std::vector<T> &oth) const
	{
		oth.resize(Num);
		if (Num > 0)
		{
			memcpy(&oth[0], Elem, bytes());
		}
	}

	void assign(std::vector<T> &oth) const
	{
		oth.resize(Num);
		for (size_t i = 0; i < oth.size(); ++i)
		{
			oth[i] = Elem[i];
		}
	}

	void reset() { Elem = 0; Num = 0; }

	bool empty() const { return Num == 0; }
private:
	T	*Elem;
	int  Num;
};


