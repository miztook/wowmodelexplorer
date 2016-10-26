#include "stdafx.h"
#include "CGLES2Platform.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Extension.h"

#ifdef MW_PLATFORM_IOS

#import <OpenGLES/EAGL.h>

extern EAGLContext* g_GLKViewContext;

CGLES2Platform::CGLES2Platform()
{

}

CGLES2Platform::~CGLES2Platform()
{
	
}

bool CGLES2Platform::initDriver( const SWindowInfo& wndInfo, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread )
{
	if (g_GLKViewContext == NULL)
	{
		_ASSERT(false);
		return false;
	}

	[EAGLContext setCurrentContext : g_GLKViewContext];

	WindowSize.Width = wndInfo.width;
	WindowSize.Height = wndInfo.height;

	return true;
}

bool CGLES2Platform::swapBuffers()
{
	//return CGLES2Extension::extGlSwapBuffers(EglDisplay, EglSurface) == EGL_TRUE;
	return true;
}

bool CGLES2Platform::applyVsync( bool vsync )
{
	//return CGLES2Extension::extGlSwapInterval(EglDisplay, vsync ? 1 : 0) == EGL_TRUE;
	return true;
}

#endif

#endif
