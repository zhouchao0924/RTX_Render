

#include "DRSerializer.h"
#include "Math/kString.h"

ISerialize & operator << (ISerialize &Ar, FString &v)
{
	if (Ar.IsSaving())
	{
		kString kStr(*v);
		Ar << kStr;
	}
	else if(Ar.IsLoading())
	{
		kString kStr;
		Ar << kStr;
		v = kStr.w_str();
	}
	return Ar;
}

ISerialize & operator << (ISerialize &Ar, FText &V)
{
	if (Ar.IsSaving())
	{
		FString Str = V.ToString();
		Ar << Str;
	}
	else if (Ar.IsLoading())
	{
		FString Str;
		Ar << Str;
		V.FromString(Str);
	}
	return Ar;
}

