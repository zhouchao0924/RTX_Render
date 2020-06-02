
#pragma  once

#include <vector>
#include <string>
#include "ISerialize.h"

class CharPtr
{
public:
	CharPtr()
		:_str(nullptr)
	{
	}
	CharPtr(const char *str)
	{
		_str = str;
		_bwchar = false;
	}
	CharPtr(const wchar_t *str)
	{
		_str = (char *)str;
		_bwchar = true;
	}
	bool is_wstr() const { return _bwchar; }
	const char *c_str() const { return _str ? _str : ""; }
	const wchar_t *w_str() const { return _str ? (const wchar_t *)_str : L""; }
	friend class kString;
protected:
	const char *_str;
	bool _bwchar;
};

class kString
{
public:
	kString() { _wchar = false;}

	const char *c_str() const
	{
		if (!_wchar && !_data.empty())
		{
			return &_data[0];
		}
		return "";
	}

	const wchar_t *w_str() const
	{
		if (_wchar && !_data.empty())
		{
			return (wchar_t *)(&_data[0]);
		}
		return L"";
	}

	const wchar_t* operator*() const
	{
		return !_data.empty() ? (wchar_t *)(&_data[0]) : L"";
	}

	kString(const char *str)
	{
		SetStr(str);
	}

	kString(const wchar_t *str)
	{
		SetStr(str);
	}

	void SetStr(const char *str)
	{
		if (str)
		{
			size_t clen = strlen(str);
			size_t len = clen + 1;
			_data.resize(len);
			memcpy(data(), str, len);
			_data[clen] = 0;
			_wchar = false;
		}
	}

	void SetStr(const wchar_t *str)
	{
		if (str)
		{
			size_t clen = wcslen(str);
			size_t len = 2*clen + 2;
			_data.resize(len);
			memcpy(data(), str, 2*clen);
			_data[len-1] = 0;
			_data[len-2] = 0;
			_wchar = true;
		}
	}

	kString(const CharPtr &Ptr)
	{
		if (Ptr._str)
		{
			if (Ptr._bwchar)
			{
				SetStr((wchar_t *)Ptr._str);
			}
			else
			{
				SetStr(Ptr._str);
			}
		}
	}

	CharPtr GetPtr()
	{
		if (_data.empty())
		{
			return CharPtr();
		}

		if (_wchar)
		{
			return CharPtr((const wchar_t*)&_data[0]);
		}

		return CharPtr((const char*)&_data[0]);
	}

	kString(const kString &v)
	{
		_wchar = v._wchar;
		_data = v._data;
	}

	kString &operator = (const kString &v)
	{
		_wchar = v._wchar;
		_data = v._data;
		return *this;
	}

	bool operator == (const kString &v) const
	{
		if (_wchar != v._wchar)
		{
			return false;
		}

		if (_data.size() != v._data.size())
		{
			return false;
		}

		return memcmp(c_data(), v.c_data(), size()) == 0;
	}

	bool operator == (const CharPtr &v) const
	{
		if (_wchar != v._bwchar)
		{
			return false;
		}

		if (_wchar)
		{
			return (*this) == (v.w_str());
		}

		return (*this) == (v.c_str());
	}

	bool operator == (const char *v) const
	{
		if (_wchar || !v)
		{
			return false;
		}

		size_t vlen = 0;
		size_t clen = strlen(v);
		vlen = clen + 1;

		if (_data.size() != vlen)
		{
			return false;
		}

		return memcmp(c_data(), v, size()) == 0;
	}

	bool operator == (const wchar_t *v) const
	{
		if (!_wchar || !v)
		{
			return false;
		}

		size_t vlen = 0;
		size_t clen = wcslen(v);
		vlen = 2 * clen + 2;

		if (_data.size() != vlen)
		{
			return false;
		}

		return memcmp(c_data(), v, size()) == 0;
	}


	size_t length()
	{
		if (_data.empty())
		{
			return 0;
		}
		return _wchar ? (_data.size() / 2 - 1) : (_data.size() - 1);
	}

	void *data() const { return (void  *)&_data[0]; }

	const char *c_data() const { return _data.empty()? nullptr : &_data[0]; }

	size_t size() const { return _data.size(); }

	bool isWstr() const { return _wchar; }

	void reset()
	{
		if (_data.empty())
		{
			if (_wchar)
			{
				_data.resize(2);
			}
			else
			{
				_data.resize(1);
			}
		}

		size_t i = _data.size();
		if (_wchar)
		{
			_data[i - 2] = 0;
		}
		_data[i - 1] = 0;
	}

	friend inline ISerialize & operator << (ISerialize &Ar, kString &v);
private:
	bool	_wchar;
	std::vector<char> _data;
};

inline ISerialize & operator << (ISerialize &Ar, kString &v)
{
	if (Ar.IsLoading())
	{
		int Len = 0;
		Ar << Len;
		bool LoadUCS2Char = Len < 0;
		if (LoadUCS2Char)
		{
			Len = -Len;
		}

		if (Len > 0)
		{
			if (LoadUCS2Char)
			{
				v._wchar = true;
				v._data.resize(2*Len);
				Ar.Serialize(v.data(), v.size());
			}
			else
			{
				v._wchar = false;
				v._data.resize(Len);
				Ar.Serialize(v.data(), v.size());
			}
			v.reset();
		}
	}
	else if(Ar.IsSaving())
	{
		int len = (int)v.length() + 1;
		
		int flagLen = len;

		if (v.isWstr())
		{
			flagLen = -len;
		}

		Ar << flagLen;
	
		if (len > 0)
		{
			if (v.size() <= 0)
			{
				v.reset();
			}
			Ar.Serialize(v.data(), v.size());
		}
	}

	return Ar;
}



