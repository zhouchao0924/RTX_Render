#ifndef CRAFT_PAVING_MACROS_H
#define CRAFT_PAVING_MACROS_H

#include <cassert>
#include <typeinfo>

#ifdef CRAFTPAVING_EXPORT
#define CRAFTPAVING_API __declspec(dllexport)
#else
#define CRAFTPAVING_API
#endif

typedef int  Int32;

#define CP_FORCEINLINE	__forceinline

#define CP_INLINE inline

#define CP_CHECK(expr) if(!(expr)){std::wcerr << "check error : " << __FUNCTION__ << " - (" << #expr << ") file " << __FILE__ << " line " << __LINE__ << std::endl;abort();}

#endif // CRAFT_PAVING_MACROS_H