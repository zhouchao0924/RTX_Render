

#pragma once

#include "kMathBase.h"
#include <algorithm>

template<class T>
class TColorTemp
{
public:
	union
	{
		struct
		{
			T B,G,R,A;
		};
		struct
		{
			T b, g, r, a;
		};
		T Indices[4];
	};
	TColorTemp() { R = G = B = A = 0; }
	TColorTemp(T Red, T Green, T Blue, T Alpha ) { R = Red; G = Green; B = Blue; A = Alpha; }
	bool operator == (TColorTemp &oth) const
	{
		return B==oth.B && G==oth.G && R==oth.R && A==oth.A;
	}
	bool operator != (TColorTemp &oth) const
	{
		return B!=oth.B || G!=oth.G || R!=oth.R || A!=oth.A;
	}
	void SwapBR()
	{
		T t = B;
		B = R;
		R = t;
	}
};

struct kColor :public TColorTemp<unsigned char>
{
	kColor() { R = G = B = A = 0; }
	kColor(unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha) { R = Red; G = Green; B = Blue; A = Alpha; }
	unsigned int toDword() const { return *(unsigned int *)Indices; }
	static kColor fromColorAlpha(unsigned int color, unsigned int alpha)
	{
		 return kColor(((color)&0x00ffffff)|(alpha)<<24);
	}
	kColor(unsigned int dwWord) { memcpy(Indices, &dwWord, sizeof(unsigned int)); }
	bool operator != (const kColor &oth) const
	{
		return R != oth.R || G != oth.G || B != oth.B || A != oth.A;
	}
};

struct kLinearColor :public TColorTemp<float>
{
	kLinearColor() { R = G = B = A = 0; }
	kLinearColor(float Red, float Green, float Blue, float Alpha) { R = Red; G = Green; B = Blue; A = Alpha; }
	kLinearColor(kColor bcolor) 
	{
		A = bcolor.A / 255.0f;
		R = bcolor.R / 255.0f;
		G = bcolor.G / 255.0f;
		B = bcolor.B / 255.0f;
	}
	kColor	toBColor()
	{
		unsigned char rv = (unsigned char)std::min<int>(255, int(R * 255));
		unsigned char gv = (unsigned char)std::min<int>(255, int(G * 255));
		unsigned char bv = (unsigned char)std::min<int>(255, int(B * 255));
		unsigned char av = (unsigned char)std::min<int>(255, int(A * 255));
		return kColor(rv, gv, bv, av);
	}
};




