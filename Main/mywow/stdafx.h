// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "compileconfig.h"


////////////////////////////////////
//			platform header		  //
////////////////////////////////////
#ifdef MW_PLATFORM_WINDOWS

#include "targetver.h"
            
// #pragma warning( error: 4018 )
// #pragma warning( error: 4150 )
// #pragma warning( error: 4244 )
#pragma warning (disable: 4996)
#pragma warning (disable: 6011)
#pragma warning (disable: 6211)

// TODO: reference additional headers your program requires here
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1	// Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>
#include <WinInet.h>
#include <DbgHelp.h>

#ifdef MW_USE_DIRECTINPUT
#include <dinput.h>
#endif

//audio
#include <mmreg.h>
#ifdef MW_USE_AUDIO
#include <dsound.h>
#endif

#include <io.h>
#include <direct.h>
#include <share.h>
#include <malloc.h>

#elif defined(MW_PLATFORM_LINUX)        //linux

#include <sys/io.h>
#include <iconv.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#else       //ios, android

#include <iconv.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#endif


////////////////////////////////////
//			d3d9				  //
////////////////////////////////////
#ifdef MW_COMPILE_WITH_DIRECT3D9
#include <d3d9.h>
#endif


////////////////////////////////////
//			d3d11				  //
////////////////////////////////////
#ifdef MW_COMPILE_WITH_DIRECT3D11
#include <DXGI.h>
#include <d3d11.h>
#endif


////////////////////////////////////
//			opengl				  //
////////////////////////////////////
#ifdef MW_COMPILE_WITH_OPENGL
#include <GL/GL.h>
#endif


////////////////////////////////////
//			opengles			  //
////////////////////////////////////
#ifdef MW_COMPILE_WITH_GLES2

#ifdef MW_PLATFORM_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#endif

#endif


////////////////////////////////////
//			c lib				  //
////////////////////////////////////
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <cfloat>

#include <stdint.h>

////////////////////////////////////
//			stl					  //
////////////////////////////////////
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>


////////////////////////////////////
//			free libs			  //
////////////////////////////////////
//freetype
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

//jpeglib
#include "jpeglib.h"

//libpng
#include "png.h"

#ifdef MW_PLATFORM_WINDOWS  //在非windows平台下暂时屏蔽下面的开源库

#if defined(MW_USE_MPQ)
//stormlib
#include "stormlib.h"
#elif defined(MW_USE_CASC)
#include "CascLib.h"
#endif

//libogg
#include "ogg/ogg.h"

//libvorbis
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

//libmad
#include "mad.h"

#endif