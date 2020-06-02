
#pragma once

#include "math.h"
#include <algorithm>

const float  K_ROUNDING_ERROR = 0.0001f;

const float  K_SMALL_NUMBER = K_ROUNDING_ERROR;

const float  KM_PI = 3.14159265358979f;

const double K_RAD_G = 180.0 / 3.1415926535897932384626433832795;

const double K_G_RAD = 3.1415926535897932384626433832795 / 180.0;

inline bool Equals_Float(float a, float b, float epsilon)
{
	return fabs(a-b)<= epsilon;
}

//if r is 0,return v1,if r is 1,return v2
template<typename T>
inline T Lerp(T v1,T v2,T r)
{
	return v1*(((T)1.0)-r)+v2*r;
}

template <typename T>
inline T Wrap(T v,T r)
{
	v=fmod(v,r);
	if (v<0)
		v+=r;
	return v;
}

template <typename T>
inline T Wrap_angle(T a)//to 0..360
{
	return Wrap<T>(a,360.0f);
}
template <typename T>
inline T Wrap_radian(T r)//to 0..2*Pi
{
	return Wrap<T>(r,2*KM_PI);
}

inline bool Judge_rotate_dir(float r, float target)
{
	float d=target-r;
	d = Wrap_radian<float>(d);
	if (d < KM_PI)
	{
		return true;
	}
	return false;
}

//return whether reach limit
inline bool Rotate_limited(float &r,float limit,float delta)
{
	float d = Wrap_radian(limit-r);
	if (d<KM_PI)
	{
		if (delta>=d)
		{
			r=limit;
			return true;
		}
		else
		{
			r+=delta;
			return false;
		}
	}
	else
	{
		d-=2*KM_PI;
		d=-d;
		if (delta>=d)
		{
			r=limit;
			return true;
		}
		else
		{
			r-=delta;
			return false;
		}
	}
}


inline float Normalize_angle(float a)//to -180..180
{
	return Wrap_angle( a + (float)180.0 )-(float)180.0;
}

inline float Normalize_radian(float r)//to -Pi~Pi
{
	r = Wrap_radian(r);
	if (r>KM_PI)
		r-=2*KM_PI;
	return r;
}

//rate==0  --> full r1
//rate==1  --> full r2
inline float Lerp_angle(float r1,float r2,float rate)
{
	float d = Normalize_radian(r2-r1);
	return r1 + d*rate;
}

inline float Div_safe(float a, float b)
{
	if (b>=0)
	{
		if (b<K_ROUNDING_ERROR)
			b=K_ROUNDING_ERROR;
	}
	else
	{
		if (b>K_ROUNDING_ERROR)
			b=K_ROUNDING_ERROR;
	}
	return a/b;
}

// returns accurate values only for integers that are power of 2
inline int Fastlog2( int x)
{
	int slRet;
// #if RS_HARDWARE_PLATFORM == HPLATFORM_X86
// 		__asm {
// 			bsr   eax, dword ptr[x]
// 			mov   dword ptr[slRet], eax
// 		}
// #else
	slRet = (int)log(x);
// #endif
	return slRet;
}

// returns log2 of first larger value that is a power of 2
inline int Fastmaxlog2(int x)
{
	int slRet;
// #if RS_HARDWARE_PLATFORM == HPLATFORM_X86
// 		__asm 
// 		{
// 			bsr   eax,dword ptr [x]
// 			bsf   edx,dword ptr [x]
// 			cmp   edx,eax
// 				adc   eax,0
// 				mov   dword ptr [slRet],eax
// 		}
// #else
	slRet = (int)log(slRet);
// #endif
	return slRet;
}

inline bool ispower2( int x)
{
	int p = Fastlog2(x);
	return ((1<<p)==x);
}

//divide signed s by u ,and floor the result
inline int Idiv_signed(int s, int u)
{
	if (s>=0)
		return s/(int)u;

	return (s-(((int)u)-1))/(int)u;
}

//mod signed s by u.
inline int Imod_signed(int s, int u)
{
	return s - Idiv_signed(s,u)*(int)u;
}

//rate==0  --> full c1
//rate==1  --> full c2
inline int Lerp_color( int c1, int c2, float rate )
{
	int result;
	unsigned char *pCol1 = (unsigned char *)&( c1 );
	unsigned char *pCol2 = (unsigned char *)&( c2 );
	unsigned char *pResult = (unsigned char *)&( result );
	for ( int i = 0; i < 4; ++i )
	{
		pResult[i] = ( unsigned char )( (int)pCol1[i] * ( 1.0f - rate ) + (int)pCol2[i] * rate );
	}
	return result;
}

template<class T>
inline T In_range( T val, T start, T end )
{
	if ( val < start )	val = start;
	if ( val > end )	val = end;
	return val;
}

inline int Round(float X){ return int(floor(X + 0.5f)); }
inline int Floor(float X) { return int (floor(X)); }
inline int Ceil(float X) { return int (ceilf(X)); }

inline void SinCos (float angle, float *sine, float *cosine) 
{
// #if RS_HARDWARE_PLATFORM == HPLATFORM_X86
// 		__asm {
// 			push		ecx
// 				fld         dword ptr angle
// 				fsincos
// 				mov 		ecx, dword ptr[cosine]
// 			fstp        dword ptr [ecx]
// 			mov 		ecx, dword ptr[sine]
// 			fstp        dword ptr [ecx]
// 			pop			ecx
// 		}
// #else
	*sine   = (float)sin (angle);
	*cosine = (float)cos (angle);
// #endif
}

inline float Sin(float angle)
{
#ifdef __USE_ASM_CODE_
	float s, c;
	SinCos(angle, &s, &c);
	return s;
#else
	return (float)sin((double)angle);
#endif
}

inline float Cos(float angle)
{
#ifdef __USE_ASM_CODE_
	float s, c;
	SinCos(angle, &s, &c);
	return c;
#else
	return (float)cos((double)angle);
#endif
}

inline float Sqrt(float arg)
{
// #if RS_HARDWARE_PLATFORM == HPLATFORM_X86
// 		float ans;
// 		__asm {
// 			fld         dword ptr arg
// 				fsqrt
// 				fstp        dword ptr [ans]
// 		}
// 		return ans;
// #else
	return (float)sqrt((double)arg);
// #endif
}

inline float Pow(float v, float p)
{
	return powf(v, p);
}

template< class T > inline T Square(const T A)
{
	return A*A;
}

inline float Exp(float Value) { return expf(Value); }
	
// Returns 2^Value
inline float Exp2(float Value) { return powf(2.f, Value); /*exp2f(Value);*/ }
inline float Loge(float Value) { return logf(Value); }
inline float LogX(float Base, float Value) { return Loge(Value) / Loge(Base); }
	
// 1.0 / Loge(2) = 1.4426950f
inline float Log2(float Value) { return Loge(Value) * 1.4426950f; }
inline float Tan(float Value) { return tanf(Value); }
inline float Atan(float Value) { return atanf(Value); }
inline float Asin(float Value) { return asinf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
inline float Acos(float Value) { return acosf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
inline bool IsNaN(float A) { return isnan(A) != 0; }
inline bool IsFinite(float A) { return isfinite(A); }

float InvSqrt(float F);
float InvSqrtEst(float F);
float Atan2(float Y, float X);




