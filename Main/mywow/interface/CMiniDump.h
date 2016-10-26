#pragma once

#ifdef MW_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1	// Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>

#include "base.h"


class CMiniDump
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMiniDump);

public:
	static void begin();
	static void end();

private:
	static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );
};

#endif