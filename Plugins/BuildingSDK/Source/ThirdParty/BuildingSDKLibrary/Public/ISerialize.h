
#pragma once

#include "Math/kVector2D.h"
#include "Math/kVector3D.h"
#include "Math/kPlane.h"
#include "Math/kBox.h"
#include "Math/kColor.h"
#include "Math/kXform.h"
#include <string>
#include <vector>

class ISerialize
{
public:
	virtual ~ISerialize() {}
	virtual void Serialize(void *pData, size_t SzData) = 0;
	virtual void Seek(size_t Pos) = 0;
	virtual size_t Tell() = 0;
	virtual void Close() = 0;
	virtual void Skip(size_t szData) = 0;
	virtual bool IsSaving() = 0;
	virtual bool IsLoading() = 0;
	virtual int  ReadChunk() = 0;
	virtual void WriteChunk(int ChunkID) = 0;
	virtual void EndChunk(int ChunkID) = 0;
	virtual void SkipChunk() = 0;
};

#define  SERIALIZE_VEC(vec){					\
		if(Ar.IsSaving()){						\
			int nVec = (int)vec.size();			\
			Ar << nVec;							\
			for (int i = 0; i < nVec; ++i) {	\
				Ar << vec[i];					\
			}									\
		}										\
		else if (Ar.IsLoading()) {				\
			int nVec = 0;						\
			Ar << nVec;							\
			vec.resize(nVec);					\
			for (int i = 0; i < nVec; ++i) {	\
				Ar << vec[i];					\
			}									\
		}										\
	}

#define  SERIALIZE_COMPLEXVEC(vec){				\
		if(Ar.IsSaving()){						\
			int nVec = (int)vec.size();			\
			Ar << nVec;							\
			for (int i = 0; i < nVec; ++i) {	\
				vec[i].Serialize(Ar);			\
			}									\
		}										\
		else if (Ar.IsLoading()) {				\
			int nVec = 0;						\
			Ar << nVec;							\
			vec.resize(nVec);					\
			for (int i = 0; i < nVec; ++i) {	\
				vec[i].Serialize(Ar);			\
			}									\
		}										\
	}

#define  KSERIALIZE_ENUM(enumtype, enumvalue){	\
			if (Ar.IsSaving()) {				\
				char v = (char)enumvalue;		\
				Ar << v;						\
			}									\
			else if (Ar.IsLoading()) {			\
				char v = 0;						\
				Ar << v;						\
				enumvalue = (enumtype)v;		\
			}									\
		}

//////////////////////////////////////////////////////////////////////////
inline ISerialize & operator << (ISerialize &Ar, char &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, unsigned char &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, int &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, unsigned int &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline  ISerialize & operator << (ISerialize &Ar, float &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, double &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, bool &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, __int64 &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, kColor &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, kPoint &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, kVector3D &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, kVector4D &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, kRotation &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, kPlane3D &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, kBox3D &v)
{
	Ar.Serialize(&v, sizeof(v));
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, kXform &v)
{
	Ar << v.Location;
	Ar << v.Rotation;
	Ar << v.Scale;
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, const char *v)
{
	if (Ar.IsSaving())
	{
		int len = (int)strlen(v);
		Ar.Serialize(&len, sizeof(len));
		Ar.Serialize((char *)v, len + 1);
	}
	return Ar;
}

inline ISerialize & operator << (ISerialize &Ar, std::string &v)
{
	if (Ar.IsLoading())
	{
		int len = 0;
		Ar.Serialize(&len, sizeof(len));
		std::vector<char> buff;
		buff.resize(len + 1);
		Ar.Serialize(&buff[0], len + 1);
		buff[len] = 0;
		v = &buff[0];
	}
	else if (Ar.IsSaving())
	{
		int len = (int)v.length();
		Ar.Serialize(&len, sizeof(len));
		Ar.Serialize((char *)v.data(), len + 1);
	}
	return Ar;
}

