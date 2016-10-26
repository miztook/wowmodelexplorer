#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "core.h"
#include "SWindowInfo.h"

#ifdef MW_PLATFORM_WINDOWS
#include "EGL/egl.h"
#endif

class CGLES2Platform
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2Platform);
public:
	CGLES2Platform();
	~CGLES2Platform();

public:
	bool initDriver(const SWindowInfo& wndInfo, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread);

	dimension2du getInitWindowSize() const { return WindowSize; }

	bool swapBuffers();

	bool applyVsync(bool vsync);
    
private:
    
#ifdef MW_PLATFORM_WINDOWS
	EGLNativeWindowType		HWnd;
	EGLNativeDisplayType		Hdc;
	EGLDisplay		EglDisplay;
	EGLConfig		EglConfig;
	EGLSurface		EglSurface;
	EGLContext		EglContext;
#endif

	dimension2du	WindowSize;
};

#endif