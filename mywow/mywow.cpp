#include "StdAfx.h"
#include "mywow.h"

CRITICAL_SECTION g_refCS;
CRITICAL_SECTION g_allocatorCS;

void initGlobal()
{
	::InitializeCriticalSection(&g_refCS);
	::InitializeCriticalSection(&g_allocatorCS);
}

void deleteGlobal()
{
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