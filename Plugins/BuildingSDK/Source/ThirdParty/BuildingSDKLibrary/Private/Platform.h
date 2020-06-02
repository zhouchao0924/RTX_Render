
#pragma once

#define  PLATFORM_WINDOWS 1
#define  PLATFORM_ANDROID 2
#define  PLATFORM_IOS	  3

#if defined _WINDOWS
#define  RS_PLATFORM   PLATFORM_WINDOWS
#endif

#if defined __ANDROID__
#define  RS_PLATFORM   PLATFORM_ANDROID
#endif 

#if defined __APPLE__
#define  RS_PLATFORM   PLATFORM_IOS
#endif


#define  USE_MX_ONLY	0


