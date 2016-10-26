#pragma once

#include "compileconfig.h"

#ifdef AUTOMATIC_LINK

#pragma comment(lib, "stormlib.lib")
#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "freetype.lib")
//#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "libmad.lib")
#pragma comment(lib, "libogg.lib")
#pragma comment(lib, "libvorbis.lib")
#pragma comment(lib, "jpeglib.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "wininet.lib") 

#ifdef MW_COMPILE_WITH_OPENGL
	#pragma comment(lib, "opengl32.lib")
#endif

#ifdef MW_COMPILE_WITH_GLES2
	#pragma comment(lib, "libEGL.lib")
	#pragma comment(lib, "libGLESv2.lib")
	//#pragma comment(lib, "PVRTexLib.lib")
#endif

#endif

#include "q_memory.h"
#include "temp_memory.h"
#include "core.h"
#include "iosys.h"
#include "font.h"
#include "video.h"
#include "wow.h"
#include "scene.h"
#include "renderer.h"
#include "input.h"
#include "audio.h"
#include "ai.h"
#include "system.h"

#include "enginesetting.h"
#include "engine.h"

void createEngine(const SEngineInitParam& param, const SWindowInfo& wndInfo);

void destroyEngine();

void initGlobal();

void deleteGlobal();

