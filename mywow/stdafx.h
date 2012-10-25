// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// #pragma warning( error: 4018 )
// #pragma warning( error: 4150 )
// #pragma warning( error: 4244 )

// TODO: reference additional headers your program requires here
#include <Windows.h>
#include <WindowsX.h>

#include <stdio.h>
#include <crtdbg.h>
#include <memory.h>
#include <stdlib.h>
#include <direct.h>
#include <io.h>

//stl
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>

//
#if defined(DEBUG) || defined(_DEBUG)
	#define D3D_DEBUG_INFO
#endif

#include <dinput.h>

#include <d3d9.h>
#include <d3dx9.h>

//audio
#include <mmreg.h>

//freetype
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

//stormlib
#include "stormlib.h"			

