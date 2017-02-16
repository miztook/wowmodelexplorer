#ifndef __COMPILE_CONFIG_H_INCLUDED__
#define __COMPILE_CONFIG_H_INCLUDED__

//这个宏用在interface下的纯虚接口中，定义为了保持实现的一致性，即子类必须实现统一的接口
//实际为了效率考虑，基类只保留必要的接口，避免虚函数调用带来的性能副作用
//#define FULL_INTERFACE

//编辑器模式
//#define MW_EDITOR

//#define WOW60

#if !defined(WOW30) && !defined(WOW40) && !defined(WOW50) && !defined(WOW60)
	#define WOW70
#endif

#if defined(WOW60) || defined(WOW70)
	#define MW_USE_CASC
#else
	#define MW_USE_MPQ
#endif

//#define MW_USE_MPQ

//#define MW_USE_CASC

//#define MW_USE_AUDIO

//#define MW_USE_DIRECTINPUT

//#define MW_USE_TOUCH_INPUT

//#define MW_USE_FRAME_RT

//#define MW_PLATFORM_WINDOWS

//#define MW_PLATFORM_IOS

//#define MW_PLATFORM_ANDROID

//platform
#if defined(_MSC_VER)

#define MW_PLATFORM_WINDOWS 1

#elif defined(__ANDROID__)
//Attention: "__linux__" is also defined on Android platform.
#define MW_PLATFORM_ANDROID 1

#elif defined(__APPLE__)

#define MW_PLATFORM_XOS 1

#elif defined(__linux__)

#define MW_PLATFORM_LINUX 1

#endif

//定义平台相关的宏
//#define MW_COMPILE_WITH_DIRECT3D9

//#define MW_COMPILE_WITH_DIRECT3D11

//#define MW_COMPILE_WITH_OPENGL

//#define USE_WITH_GLES2					//if defined, only use features that match gles2

//#define MW_COMPILE_WITH_GLES2

//#define USE_PVR			//use pvr resources

//#define USE_KTX			//use ktx resources

//#define USE_QALLOCATOR

//#define MW_VIDEO_MULTITHREAD			//显存资源多线程，dx9,dx11下开启, gl不支持

#if defined(MW_PLATFORM_WINDOWS)

	#define MW_EDITOR

	//#define MW_USE_AUDIO

	#define USE_QALLOCATOR

	//#define MW_USE_DIRECTINPUT

	//#define MW_USE_TOUCH_INPUT

	#define MW_USE_FRAME_RT

	#define MW_COMPILE_WITH_DIRECT3D9

	#define MW_COMPILE_WITH_DIRECT3D11

	#define MW_COMPILE_WITH_OPENGL

	#define USE_WITH_GLES2					//if defined, only use features that match gles2

	//#define MW_COMPILE_WITH_GLES2

	//#define USE_PVR

	//#define USE_KTX

#elif defined(MW_PLATFORM_IOS)			//ios

	#define MW_USE_TOUCH_INPUT

	#define USE_WITH_GLES2					//if defined, only use features that match gles2

	#define MW_COMPILE_WITH_GLES2

	#define USE_PVR

#elif defined(MW_PLATFORM_ANDROID)		//android

	#define MW_USE_TOUCH_INPUT

	#define USE_WITH_GLES2					//if defined, only use features that match gles2

	#define MW_COMPILE_WITH_GLES2	

	#define USE_KTX
#else		//other

	#define MW_USE_TOUCH_INPUT

	#define USE_WITH_GLES2					//if defined, only use features that match gles2

	#define MW_COMPILE_WITH_GLES2

#endif

#endif