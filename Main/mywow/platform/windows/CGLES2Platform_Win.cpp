#include "stdafx.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Platform.h"
#include "mywow.h"
#include "CGLES2Extension.h"

#ifdef MW_PLATFORM_WINDOWS

CGLES2Platform::CGLES2Platform()
{
	HWnd = NULL_PTR;
	Hdc = NULL_PTR;
	EglDisplay = NULL_PTR;
	EglConfig = NULL_PTR;
	EglSurface = NULL_PTR;
	EglContext = NULL_PTR;
}

CGLES2Platform::~CGLES2Platform()
{
	if (EglDisplay)
	{
		if(!eglMakeCurrent(EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
		{
			ASSERT(false);
		}

		if (!eglTerminate(EglDisplay))
		{
			ASSERT(false);
		}
	}

	if (Hdc && HWnd)
		::ReleaseDC(HWnd, Hdc);
}

bool CGLES2Platform::initDriver( const SWindowInfo& wndInfo, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread )
{
	ASSERT(::IsWindow(wndInfo.hwnd));

	HWnd = (EGLNativeWindowType)wndInfo.hwnd;

	//changeWindowSize
	if (fullscreen)
	{
		WindowSize = dimension2du(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
		SetWindowLongPtrA(HWnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		::MoveWindow(HWnd, 0, 0, WindowSize.Width, WindowSize.Height, TRUE);
	}
	else
	{
		RECT rc;
		::GetClientRect(HWnd, &rc);
		WindowSize =  dimension2du((u32)rc.right-rc.left, (u32)rc.bottom-rc.top);
	}

	Hdc = ::GetDC(HWnd);

	//step 1
	EglDisplay = eglGetDisplay(Hdc);
	if (EglDisplay == EGL_NO_DISPLAY)
		EglDisplay = eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY);

	//step 2
	EGLint major, minor;
	if (!eglInitialize(EglDisplay, &major, &minor))
	{
		ASSERT(false);
		return false;
	}

	//step 3, context
	if(!eglBindAPI(EGL_OPENGL_ES_API))
	{
		ASSERT(false);
		return false;
	}

	const u8 depthBits = 24;
	const u8 stencilBits = 8;

	//step 4 choose format
	const EGLint pi32ConfigAttribs[] =
	{
		EGL_LEVEL,				0,
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE,		8,
		EGL_BLUE_SIZE,			8,
		EGL_DEPTH_SIZE,		depthBits,
		EGL_STENCIL_SIZE,	stencilBits,
		EGL_NONE
	};

	int iConfigs;
	if (!eglChooseConfig(EglDisplay, pi32ConfigAttribs, &EglConfig, 1, &iConfigs) || (iConfigs != 1))
	{
		ASSERT(false);
		return false;
	}

	//step 5 create surface
	EglSurface = eglCreateWindowSurface(EglDisplay, EglConfig, HWnd, NULL_PTR);
	if (EglSurface == EGL_NO_SURFACE)
	{
		EGLint err = eglGetError();
		switch(err)
		{
		case EGL_BAD_MATCH:
			//g_Engine->getFileSystem()->writeLog(ELOG_GX,  "eglCreateWindowSurface failed: EGL_BAD_MATCH");
			break;
		case EGL_BAD_CONFIG:
			//g_Engine->getFileSystem()->writeLog(ELOG_GX,  "eglCreateWindowSurface failed: EGL_BAD_CONFIG");
			break;
		case EGL_BAD_NATIVE_WINDOW:
			//g_Engine->getFileSystem()->writeLog(ELOG_GX,  "eglCreateWindowSurface failed: EGL_BAD_NATIVE_WINDOW");
			break;
		case EGL_BAD_ALLOC:
			//g_Engine->getFileSystem()->writeLog(ELOG_GX,  "eglCreateWindowSurface failed: EGL_BAD_ALLOC");
			break;
		default:
			break;
		}
		ASSERT(false);
		return false;
	}

	//create context
	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	EglContext = eglCreateContext(EglDisplay, EglConfig, NULL_PTR, ai32ContextAttribs);
	if (EglContext == EGL_NO_CONTEXT)
	{
		ASSERT(false);
		return false;
	}

	//apply context
	if (!eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext))
	{
		ASSERT(false);
		return false;
	}

	applyVsync(vsync);

	return true;
}

bool CGLES2Platform::swapBuffers()
{
	return CGLES2Extension::extGlSwapBuffers(EglDisplay, EglSurface) == EGL_TRUE;
}

bool CGLES2Platform::applyVsync( bool vsync )
{
	return CGLES2Extension::extGlSwapInterval(EglDisplay, vsync ? 1 : 0) == EGL_TRUE;
}

#endif

#endif


