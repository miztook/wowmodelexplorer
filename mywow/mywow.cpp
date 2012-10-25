#include "StdAfx.h"
#include "mywow.h"

CRITICAL_SECTION g_refCS;
CRITICAL_SECTION g_allocatorCS;
CRITICAL_SECTION g_textureCS;
CRITICAL_SECTION g_hwbufferCS;
CRITICAL_SECTION g_adtCS;
CRITICAL_SECTION g_m2CS;
CRITICAL_SECTION g_wmoCS;

void initGlobal()
{
	::InitializeCriticalSection(&g_refCS);
	::InitializeCriticalSection(&g_allocatorCS);
	::InitializeCriticalSection(&g_textureCS);
	::InitializeCriticalSection(&g_hwbufferCS);
	::InitializeCriticalSection(&g_adtCS);
	::InitializeCriticalSection(&g_m2CS);
	::InitializeCriticalSection(&g_wmoCS);
}

void deleteGlobal()
{
	::DeleteCriticalSection(&g_wmoCS);
	::DeleteCriticalSection(&g_m2CS);
	::DeleteCriticalSection(&g_adtCS);
	::DeleteCriticalSection(&g_hwbufferCS);
	::DeleteCriticalSection(&g_textureCS);
	::DeleteCriticalSection(&g_allocatorCS);
	::DeleteCriticalSection(&g_refCS);
}

void createEngine()
{
	initGlobal();

	g_Engine = new Engine;
}

void destroyEngine()
{
	delete g_Engine;

	deleteGlobal();
}