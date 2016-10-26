// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "compileconfig.h"

#ifdef MW_PLATFORM_WINDOWS

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// #pragma warning( error: 4018 )
// #pragma warning( error: 4150 )
// #pragma warning( error: 4244 )
#pragma warning (disable: 6011)
#pragma warning (disable: 6211)

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


////////////////////////////////////
//			stl					  //
////////////////////////////////////
#include <vector>
#include <list>
#include <map>
#include <set>

//mywow
#include "mywow.h"

#ifdef MW_PLATFORM_WINDOWS

//jsoncpp
#include "json/json.h"

//fbx sdk
#include <fbxsdk.h>

#endif